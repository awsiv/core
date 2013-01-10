/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "generic_agent.h"
#include "cf-hub.h"
#include "cf.nova.h"

#include "sysinfo.h"
#include "env_context.h"
#include "constraints.h"
#include "promises.h"
#include "db_maintain.h"
#include "db_query.h"
#include "db_save.h"
#include "files_interfaces.h"
#include "granules.h"
#include "cf-hub-maintenance.h"
#include "cf-hub-worker.h"
#include "item_lib.h"
#include "conversion.h"
#include "nova_reporting.h"
#include "cf-hub-scheduled-reports.h"
#include "scope.h"
#include "instrumentation.h"
#include "unix.h"
#include "cfstream.h"
#include "signals.h"
#include "transaction.h"
#include "logging.h"
#include "exec_tools.h"

#include <assert.h>

/*******************************************************************/

static const int BIG_UPDATES = 6;

/*******************************************************************/

static bool NO_FORK = false;
static bool CONTINUOUS = false;
static bool PERFORM_DB_MAINTENANCE = false;
static bool DB_CACHE_COMPLIANCE = false;

static bool LOGGING = false;
static Item *SCHEDULE = NULL;
static Item *EXCLUDE_HOSTS = NULL;

static bool CFH_ZENOSS = false;
static char ZENOSS_PATH[CF_BUFSIZE];

/*******************************************************************/

static void ThisAgentInit(void);
static GenericAgentConfig CheckOpts(int argc, char **argv);

static void StartHub(void);
static void Nova_CollectReports(void);
static int ScheduleRun(void);
static void Nova_RemoveExcludedHosts(Item **list, Item *hosts_exclude);
static void KeepPromises(Policy *policy, GenericAgentConfig config);

static void Nova_Scan(Item *masterlist);
static pid_t Nova_ScanList(Item *list);
static void Nova_ParallelizeScan(Item *masterlist);
static void SplayLongUpdates(void);
static void Nova_CountMonitoredClasses(void);
static void CollectSchedulerChildAndSleep(int wait_seconds);
static int GetHubSleepSeconds(time_t start);

/*******************************************************************/
/* Command line options                                            */
/*******************************************************************/

static const char *ID = "The hub is a scheduler and aggregator for the CFDB knowledge\n"
    "repository. It automatically schedules updates from clients\n" "that have registered by previous connection.";

static const struct option OPTIONS[17] =
{
    {"cache", no_argument, 0, 'a'},
    {"continuous", no_argument, 0, 'c'},
    {"debug", optional_argument, 0, 'd'},
    {"no-fork", no_argument, 0, 'F'},
    {"file", required_argument, 0, 'f'},
    {"help", no_argument, 0, 'h'},
    {"index", no_argument, 0, 'i'},
    {"no-lock", no_argument, 0, 'K'},
    {"logging", no_argument, 0, 'l'},
    {"maintain", no_argument, 0, 'm'},
    {"dry-run", no_argument, 0, 'n'},
    {"splay_updates", no_argument, 0, 's'},
    {"version", no_argument, 0, 'V'},
    {"verbose", no_argument, 0, 'v'},
    {NULL, 0, 0, '\0'}
};

static const char *HINTS[17] =
{    
    "Rebuild database caches used for efficient query handling (e.g. compliance graphs)",
    "Continuous update mode of operation",
    "Set debugging level 0,1,2,3",
    "Run as a foreground processes (do not fork)",
    "Specify an alternative input file than the default",
    "Print the help message",
    "Reindex all collections in the CFEngine report database",
    "Ignore locking constraints during execution (ifelapsed/expireafter) if \"too soon\" to run",
    "Enable logging of report collection and maintenance to hub_log in the working directory",
    "Start database maintenance process. By default, entries older than 7 days (1 year for longterm reports) are purged.",
    "All talk and no action mode - make no changes, only inform of promises not kept",
    "Splay/load balance full-updates, overriding bootstrap times, assuming a default 5 minute update schedule.",
    "Output the version of the software",
    "Output verbose information about the behaviour of the agent",
    NULL
};

/*****************************************************************************/

int main(int argc, char *argv[])
{
    GenericAgentConfig config = CheckOpts(argc, argv);

    ReportContext *report_context = OpenReports("hub");
    Policy *policy = GenericInitialize("hub", config, report_context);

    if (DB_CACHE_COMPLIANCE)
    {
        EnterpriseDB dbconn;

        if (CFDB_Open(&dbconn))
        {
            Nova_CacheTotalCompliance(&dbconn, true);

            CFDB_Close(&dbconn);
        }
        else
        {
            CfOut(cf_error, "", "Unable to connect to enterprise database");
        }

        ReportContextDestroy(report_context);
        return 0;
    }

    if (PERFORM_DB_MAINTENANCE)
    {
        Nova_Maintain();
        ReportContextDestroy(report_context);
        return 0;
    }

    ThisAgentInit();
    KeepPromises(policy, config);
    StartHub();

    ReportContextDestroy(report_context);
    return 0;
}

/*****************************************************************************/
/* Level 1                                                                   */
/*****************************************************************************/

static GenericAgentConfig CheckOpts(int argc, char **argv)
{
    extern char *optarg;
    int optindex = 0;
    int c;
    GenericAgentConfig config = GenericAgentDefaultConfig(AGENT_TYPE_HUB);

    while ((c = getopt_long(argc, argv, "acdFf:hiKlMmnsVv", OPTIONS, &optindex)) != EOF)
    {
        switch ((char) c)
        {
        case 'a':
            DB_CACHE_COMPLIANCE = true;
            break;

        case 'c':
            CONTINUOUS = true;
            break;

        case 'd':
            HardClass("opt_debug");
            DEBUG = true;
            break;

        case 'F':
            NO_FORK = true;
            break;

        case 'f':

            if (optarg && strlen(optarg) < 5)
            {
                FatalError("-f used but argument \"%s\" incorrect", optarg);
            }

            SetInputFile(optarg);
            MINUSF = true;
            break;

        case 'h':
            Syntax("cf-hub - cfengine's report aggregator", OPTIONS, HINTS, ID);
            exit(0);

        case 'I':
            INFORM = true;
            break;

        case 'i':
            CFDB_ReIndexAll();
            exit(0);

        case 'K':
            IGNORELOCK = true;
            break;

        case 'l':
            LOGGING = true;
            break;

        case 'M':
            ManPage("cf-hub - cfengine's report aggregator", OPTIONS, HINTS, ID);
            exit(0);

        case 'm':
            PERFORM_DB_MAINTENANCE = true;
            break;

        case 'n':
            DONTDO = true;
            IGNORELOCK = true;
            HardClass("opt_dry_run");
            break;

        case 's':
            {
                CheckOpts(argc, argv);
                ReportContext *report_context = ReportContextNew();
                InitializeGA(report_context);

                SplayLongUpdates();
                ReportContextDestroy(report_context);
                exit(0);
                break;
            }

        case 'V':
            PrintVersionBanner("cf-hub");
            exit(0);

        case 'v':
            VERBOSE = true;
            NO_FORK = true;
            break;

        default:
            Syntax("cf-hub - cfengine's report aggregator", OPTIONS, HINTS, ID);
            exit(1);

        }
    }

    if (argv[optind] != NULL)
    {
        CfOut(cf_error, "", "Unexpected argument with no preceding option: %s\n", argv[optind]);
    }

    return config;
}

/*****************************************************************************/

static void ThisAgentInit(void)
{
    umask(077);

    if (CONTINUOUS)
    {
        AppendItem(&SCHEDULE, "any", NULL);
    }
    else if (SCHEDULE == NULL)
    {
        AppendItem(&SCHEDULE, "Min00", NULL);
        AppendItem(&SCHEDULE, "Min05", NULL);
        AppendItem(&SCHEDULE, "Min10", NULL);
        AppendItem(&SCHEDULE, "Min15", NULL);
        AppendItem(&SCHEDULE, "Min20", NULL);
        AppendItem(&SCHEDULE, "Min25", NULL);
        AppendItem(&SCHEDULE, "Min30", NULL);
        AppendItem(&SCHEDULE, "Min35", NULL);
        AppendItem(&SCHEDULE, "Min40", NULL);
        AppendItem(&SCHEDULE, "Min45", NULL);
        AppendItem(&SCHEDULE, "Min50", NULL);
        AppendItem(&SCHEDULE, "Min55", NULL);
    }
}

/*****************************************************************************/

void KeepPromises(Policy *policy, GenericAgentConfig config)
{
    Constraint *cp;
    Rval retval;

    for (cp = ControlBodyConstraints(policy, AGENT_TYPE_HUB); cp != NULL; cp = cp->next)
    {
        if (IsExcluded(cp->classes, NULL))
        {
            continue;
        }

        if (GetVariable("control_hub", cp->lval, &retval) == cf_notype)
        {
            CfOut(cf_error, "", "Unknown lval %s in hub control body", cp->lval);
            continue;
        }

        if (strcmp(cp->lval, CFH_CONTROLBODY[cfh_schedule].lval) == 0)
        {
            Rlist *rp;

            CfDebug("schedule ...\n");
            DeleteItemList(SCHEDULE);
            SCHEDULE = NULL;

            for (rp = (Rlist *) retval.item; rp != NULL; rp = rp->next)
            {
                if (!IsItemIn(SCHEDULE, rp->item))
                {
                    AppendItem(&SCHEDULE, rp->item, NULL);
                }
            }
        }

        if (strcmp(cp->lval, CFH_CONTROLBODY[cfh_exclude_hosts].lval) == 0)
        {
            Rlist *rp;

            CfDebug("exclude_hosts ...\n");
            DeleteItemList(EXCLUDE_HOSTS);
            EXCLUDE_HOSTS = NULL;

            for (rp = (Rlist *) retval.item; rp != NULL; rp = rp->next)
            {
                if (!IsItemIn(EXCLUDE_HOSTS, rp->item))
                {
                    AppendItem(&EXCLUDE_HOSTS, rp->item, NULL);
                }
            }
        }

        if (strcmp(cp->lval, CFH_CONTROLBODY[cfh_export_zenoss].lval) == 0)
        {
            if (!strcmp((const char *)retval.item, "true") == 0
                || !strcmp((const char*)retval.item, "false") == 0
                || !strcmp((const char *)retval.item, "yes") == 0
                || !strcmp((const char *)retval.item, "no") == 0
                || !strcmp((const char *)retval.item, "on") == 0
                || !strcmp((const char *)retval.item, "off") == 0)
            {
                CfOut(cf_error, "", "export_zenoss now requires a full path to summary.z file");
                CFH_ZENOSS = false;
            }
            else
            {
                CFH_ZENOSS = true;
                strlcpy(ZENOSS_PATH, (const char*)retval.item, CF_BUFSIZE);
            }

            CfOut(cf_verbose, "", "SET export_zenoss = %d\n", CFH_ZENOSS);
            continue;
        }

        if (strcmp(cp->lval, CFH_CONTROLBODY[cfh_port].lval) == 0)
        {
            SHORT_CFENGINEPORT = htons((short) Str2Int((const char *) retval.item));
            strncpy(STR_CFENGINEPORT, (const char *) retval.item, 15);
            CfOut(cf_verbose, "", "SET default portnumber = %u = %s = %s\n", (int) SHORT_CFENGINEPORT, STR_CFENGINEPORT,
                  (const char *) retval.item);
            continue;
        }

    }
}

/*****************************************************************************/

void SplayLongUpdates(void)
{
    CF_DB *dbp;
    LockData entry, update;
    CF_DBC *dbcp;
    void *value;
    int ksize, vsize, count = 0, optimum_splay_interval;
    char *key, *slots;
    time_t now = time(NULL), min = now + 30000, max = 0;
    int slot = 0, total_slots;
    time_t newtime;

    if ((dbp = OpenLock()) == NULL)
    {
        return;
    }

// key format: "last.internal_bundle.hail.HOSTIP"

    if (!NewDBCursor(dbp, &dbcp))
    {
        CloseLock(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &ksize, (void *) &value, &vsize))
    {
        // Just look at the hail promise locks

        if(BEGINSWITH(key, LOCK_HAIL_PREFIX "handle"))
        {
            // DEPRECATED: old format before Nova 2.2.0, will get purged by PurgeLocks() eventually
            continue;
        }

        if (!BEGINSWITH(key, LOCK_HAIL_PREFIX))
        {
            continue;
        }
        
        count++;
        
        memcpy(&entry, value, MIN(vsize, sizeof(entry)));

        if (entry.time < 0 || entry.time > now + 300)
        {
            // The value may be uninitialized
            printf("Found an unitialized lock time for %s, set to %s\n", key, cf_ctime(&now));
            update.pid = entry.pid;
            update.time = entry.time = now;
            DBCursorWriteEntry(dbcp, &update, sizeof(update));
        }

        if (entry.time > max)
        {
            max = entry.time;
        }

        if (entry.time < min)
        {
            min = entry.time;
        }
    }

    DeleteDBCursor(dbp, dbcp);

    CfOut(cf_verbose, "", " -> Marshalling full updates within a minimal max-entropy pattern");
    CfOut(cf_verbose, "", " -> A total of %d hosts made known hails.", count);
    CfOut(cf_verbose, "", " -> Oldest non-expired hail at %s", cf_ctime(&min));
    CfOut(cf_verbose, "", " -> Most recent hail at %s", cf_ctime(&max));

// Assume a full update might take 2s => we can do 150 in each time slot, say 100

// A time slot is less than 6 hours, but we want to minimize the distance between updates
// so that the results between all hosts are comparable

    total_slots = count / 100 + 1;
    optimum_splay_interval = 300 * total_slots; // In seconds

    CfOut(cf_verbose, "", " -> Optimum splay renormalizes to %d mins for %d hosts, i.e. %d update slot(s)",
          optimum_splay_interval / 60, count, total_slots);

    if (optimum_splay_interval > BIG_UPDATES * 3600)
    {
        CfOut(cf_verbose, "", " !! Warning, the host count is exceeds the supported limits");
    }

    slots = xmalloc(total_slots);

// Don't rely on hashing, as we can't guarantee sufficient entropy, so just round-robin

    if (!NewDBCursor(dbp, &dbcp))
    {
        CloseLock(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &ksize, (void *) &entry, &vsize))
    {
        if(BEGINSWITH(key, LOCK_HAIL_PREFIX "handle"))
        {
            // DEPRECATED: old format before Nova 2.2.0, will get purged by PurgeLocks() eventually
            continue;
        }

        if (!BEGINSWITH(key, LOCK_HAIL_PREFIX))
        {
            continue;
        }

        // Keep the splaying simple, and use round-robin

        newtime = now + 300 * (slot++ % total_slots);

        update.time = newtime;
        update.pid = entry.pid;

        if (!DONTDO)
        {
            WriteDB(dbp, key, &update, sizeof(update));
        }
        else
        {
            CfOut(cf_verbose, "", " -> Want to set update of %s to %s\n", key, cf_ctime(&update.time));
        }
    }

    free(slots);
    DeleteDBCursor(dbp, dbcp);
    CloseLock(dbp);

    printf
        (" -> Redistributed host updates with <= %d per slot, each ~%d secs per slot into %d 5-min time slots, total time = %d mins\n",
         slot / total_slots, slot / total_slots * 2, total_slots, 5 * total_slots);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static int ScheduleRun(void)
{
    Item *ip;

    if (EnterpriseExpiry())
    {
        CfOut(cf_error, "", "Cfengine - autonomous configuration engine. This enterprise license is invalid.\n");
        exit(1);
    }

    ThreadLock(cft_system);
    DeleteAlphaList(&VHEAP);
    InitAlphaList(&VHEAP);
    DeleteAlphaList(&VADDCLASSES);
    InitAlphaList(&VADDCLASSES);
    DeleteItemList(IPADDRESSES);
    DeleteAlphaList(&VHARDHEAP);
    InitAlphaList(&VHARDHEAP);
    IPADDRESSES = NULL;

    DeleteScope("this");
    DeleteScope("mon");
    DeleteScope("sys");
    DeleteScope("match");
    NewScope("this");
    NewScope("mon");
    NewScope("sys");
    NewScope("match");

    GetInterfacesInfo(AGENT_TYPE_EXECUTOR);
    Get3Environment();
    OSClasses();
    BuiltinClasses();
    SetReferenceTime(true);
    ThreadUnlock(cft_system);

    for (ip = SCHEDULE; ip != NULL; ip = ip->next)
    {
        CfDebug("Checking schedule %s...\n", ip->name);

        if (IsDefinedClass(ip->name, NULL))
        {
            CfOut(cf_verbose, "", "Waking up the agent at %s ~ %s \n", cf_ctime(&CFSTARTTIME), ip->name);
            return true;
        }
    }

    return false;
}

/*****************************************************************************/

static void Nova_RemoveExcludedHosts(Item **listp, Item *hosts_exclude)
{
    Item *ip;
    Item *include = NULL;

    for (ip = *listp; ip != NULL; ip = ip->next)
    {
        if (IsMatchItemIn(hosts_exclude, ip->classes))
        {
            CfDebug("Excluding host %s(%s) from hub report query\n", ip->classes, ip->name);
        }
        else
        {
            IdempPrependItem(&include, ip->name, ip->classes);
        }
    }

    DeleteItemList(*listp);
    *listp = include;
}

/***************************************************************************/

static void StartHub(void)
{
    time_t now = time(NULL);
    Promise *pp = NewPromise("hub_cfengine", "the aggregator");
    Attributes a = { {0} };
    CfLock thislock;

    Banner("Starting hub core");

    a.restart_class = "nonce";
    a.transaction.ifelapsed = 0;
    a.transaction.expireafter = 0;

    thislock = AcquireLock(pp->promiser, VUQNAME, now, a, pp, false);

    if (thislock.lock == NULL)
    {
        DeletePromise(pp);
        return;
    }

    if ((!NO_FORK) && (fork() != 0))
    {
        CfOut(cf_inform, "", "cf-hub starting %.24s\n", cf_ctime(&now));
        _exit(0);
    }

    if (!NO_FORK)
    {
        ActAsDaemon(0);
    }

    WritePID("cf-hub.pid");
    signal(SIGINT, HandleSignalsForDaemon);
    signal(SIGTERM, HandleSignalsForDaemon);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, HandleSignalsForDaemon);
    signal(SIGUSR2, HandleSignalsForDaemon);

    umask(077);

// operating on a non-indexed database can choke the hub
// before Nova_Maintain() gets run
    CFDB_ConnectAndEnsureIndices();


    while (!IsPendingTermination())
    {
        time_t start = time( NULL );

        RunScheduledEnterpriseReports();

        if (ScheduleRun())
        {
            CfOut(cf_verbose, "", " -> Wake up");

            HardClass("am_policy_hub");
            if (CFDB_QueryIsMaster())
            {
                Nova_CollectReports();
            }
        }

        if (CFDB_QueryDeleteHostPending())
        {
            DBRefreshHostsListCache(NULL);
        }

        int sleep_time = GetHubSleepSeconds(start);

        CollectSchedulerChildAndSleep(sleep_time);
    }
}

/********************************************************************/
/* level                                                            */
/********************************************************************/

static void Nova_CollectReports(void)
{
    Item *list = Nova_ScanClients();

    Nova_RemoveExcludedHosts(&list, EXCLUDE_HOSTS);

    DBRefreshHostsListCache(list);
    DeleteItemList(list);

    // Now read the updated cache
    Item *masterhostlist = CFDB_GetLastseenCache();
    Nova_RemoveExcludedHosts(&masterhostlist, EXCLUDE_HOSTS);

    Nova_Scan(masterhostlist);
    DeleteItemList(masterhostlist);

    if (CFH_ZENOSS && IsDefinedClass("Min00_05", NULL))
    {
        Nova_ZenossSummary(ZENOSS_PATH);
    }

    Nova_CountMonitoredClasses();
}

/********************************************************************/

static void Nova_Scan(Item *masterlist)
{
    Nova_HubLog("Starting report collection");
    
    struct timespec measure_start = BeginMeasure();
    
    if (NO_FORK)
    {
        Nova_SequentialScan(masterlist);
    }
    else
    {
        Nova_ParallelizeScan(masterlist);
    }

    EndMeasure("DBReportCollectAll", measure_start);

    Nova_HubLog("Finished report collection -- tried %d hosts", ListLen(masterlist));
}

/********************************************************************/

static bool RemoveFinishedPid(pid_t finished, pid_t *children, int *nchildren)
{
    int i;

    for (i = 0; i < *nchildren; ++i)
    {
        if (children[i] == finished)
        {
            CfOut(cf_verbose, "", "Reaped finished hostscan subprocess, pid %d", finished);
            memmove(children + i, children + i + 1, sizeof(pid_t) * (*nchildren - i - 1));
            (*nchildren)--;
            return true;
        }
    }
    return false;
}

/********************************************************************/

static void DistributeScanTasks(Item *scanhosts, Item **queues, int nqueues)
{
/*
 * In order to avoid large number of processes to be started just to get the
 * information from a single child each, we distribute tasks in chunks of CHUNKSIZE
 * clients each.
 */
#define CHUNKSIZE 10

    for (;;)
    {
        int i, j;

        for (i = 0; i < nqueues; ++i)
        {
            for (j = 0; j < CHUNKSIZE; ++j)
            {
                Item *host = scanhosts;

                if (host == NULL)
                {
                    return;
                }
                PrependItem(&(queues[i]), host->name, host->classes);

                scanhosts = scanhosts->next;
            }
        }
    }
}

/********************************************************************/

static void Nova_ParallelizeScan(Item *masterlist)
{
#define SCAN_CHILDREN 50

    Item *list[SCAN_CHILDREN] = { NULL };
    int i = 0;
    pid_t children[SCAN_CHILDREN] = { 0 };
    int nchildren = 0;

// Divide ...

    DistributeScanTasks(masterlist, list, SCAN_CHILDREN);

// Conquer ...

    for (i = 0; i < SCAN_CHILDREN; i++)
    {
        if (list[i])
        {
            pid_t child = Nova_ScanList(list[i]);

            children[nchildren++] = child;
            CfOut(cf_verbose, "", "Started new hostscan subprocess, %d/%d, pid %d", nchildren, SCAN_CHILDREN, child);
            DeleteItemList(list[i]);
        }
    }

    while (nchildren)
    {
        CfOut(cf_verbose, "", "Awaiting %d hostscan processes to finish", nchildren);

        int status;
        pid_t finished = wait(&status);

        if (finished == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            CfOut(cf_error, "wait", "Error waiting hostscan processes to finish");
            return;
        }

        RemoveFinishedPid(finished, children, &nchildren);
    }

    CfOut(cf_verbose, "", "All hostscan processes finished execution");
}

/********************************************************************/

static pid_t Nova_ScanList(Item *list)
{
    Item *ip;
    pid_t child_id;

    CfOut(cf_verbose, "", "----------------------------------------------------------------\n");
    CfOut(cf_verbose, "", " Initiating scan on: ");

    for (ip = list; ip != NULL; ip = ip->next)
    {
        CfOut(cf_verbose, "", " ? %s ", ip->name);
    }

    CfOut(cf_verbose, "", "----------------------------------------------------------------\n");

    child_id = fork();

    if (child_id == 0)
    {
        // Am child
        ALARM_PID = -1;

        Nova_SequentialScan(list);

        exit(0);
    }
    else
    {
        return child_id;
    }
}

/*********************************************************************/

static void Nova_SetPersistentScalar(char *lval, char *rval)
{
    CF_DB *dbp;
    PersistentScalar new;

    strncpy(new.rval, rval, CF_MAXVARSIZE);
    new.time = time(NULL);

    if (!OpenDB(&dbp, dbid_scalars))
    {
        return;
    }

    CfOut(cf_verbose, "", " -> Setting persistent hub knowledge: %s =>\"%s\"", lval, rval);
    WriteDB(dbp, lval, &new, sizeof(PersistentScalar));
    CloseDB(dbp);
}

static void Nova_CountMonitoredClasses(void)
{
    char work[CF_BUFSIZE];
    HubQuery *hq;
    Rlist *rp;
    Item *order_results = NULL, *ip;
    EnterpriseDB dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    snprintf(work, CF_MAXVARSIZE - 1, "%s.*", MONITOR_CLASS_PREFIX);

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, NULL, work, now - CF_HUB_HORIZON, now,
                           NULL, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        IdempItemCount(&order_results, hc->class, NULL);
    }

    DeleteHubQuery(hq, DeleteHubClass);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    for (ip = order_results; ip != NULL; ip = ip->next)
    {
        char countstr[CF_SMALLBUF];

        snprintf(countstr, CF_SMALLBUF, "%d", ip->counter + 1);
        Nova_SetPersistentScalar(ip->name + strlen(MONITOR_CLASS_PREFIX), countstr);
    }
}

/*********************************************************************/
/* Hub control                                                       */
/*********************************************************************/

void Nova_HubLog(const char *fmt, ...)
{
    if (!LOGGING)
    {
        return;
    }

    char filename[CF_BUFSIZE];
    time_t now = time(NULL);
    FILE *fout;

    snprintf(filename, CF_BUFSIZE, "%s/%s", CFWORKDIR, "hub_log");

    if ((fout = fopen(filename, "a")) == NULL)
    {
        CfOut(cf_error, "fopen", "Could not open %s", filename);
        return;
    }
    
    char timebuf[26];
    cf_strtimestamp_local(now, timebuf);

    fprintf(fout, "%s [%d]: ", timebuf, getpid());
    va_list ap;

    va_start(ap, fmt);
    vfprintf(fout, fmt, ap);
    va_end(ap);
    fprintf(fout, "\n");
    fclose(fout);
}

/*********************************************************************/

static void CollectSchedulerChildAndSleep(int wait_seconds)
{
    assert(wait_seconds >= 0);

    // Poll for scheduler process every second for wait_seconds
    // if process has already terminated, sleep for the remaining time

    while (wait_seconds > 0 && !IsPendingTermination())
    {
        if (!IsSchedulerProcRunning())
        {
            CfOut(cf_verbose, "", "Sleeping...");
            sleep(wait_seconds);
            return;
        }

        if (--wait_seconds < 1)
        {
            break;
        }

        sleep(1);
    }

    IsSchedulerProcRunning();
}

/*********************************************************************/

static int GetHubSleepSeconds(time_t start)
{
    int time_taken = time(NULL) - start;
    int sleep_time = 0;

    if (time_taken >= 0 && time_taken < CFPULSETIME)
    {
        sleep_time = CFPULSETIME - time_taken;
    }

    return sleep_time;
}

/*********************************************************************/