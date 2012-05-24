/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "generic_agent.h"

#include "cf.nova.h"

#include "hub.h"
#include "db_maintain.h"
#include "db_query.h"
#include "db_save.h"
#include "lastseen.h"
#include "granules.h"
#include "hub-maintenance.h"
#include "hub-worker.h"

/*******************************************************************/

static const int BIG_UPDATES = 6;

/*******************************************************************/

static bool NO_FORK = false;
static bool CONTINUOUS = false;

static bool LOGGING = false;
static Item *SCHEDULE = NULL;
static Item *FEDERATION = NULL;
static Item *EXCLUDE_HOSTS = NULL;

static bool CFH_ZENOSS = false;

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
static void Nova_UpdateMongoHostList(Item **list);
static Item *Nova_ScanClients(void);
static void Nova_CountMonitoredClasses(void);

/*******************************************************************/
/* Command line options                                            */
/*******************************************************************/

static const char *ID = "The hub is a scheduler and aggregator for the CFDB knowledge\n"
    "repository. It automatically schedules updates from clients\n" "that have registered by previous connection.";

static const struct option OPTIONS[16] =
{
    {"help", no_argument, 0, 'h'},
    {"debug", optional_argument, 0, 'd'},
    {"verbose", no_argument, 0, 'v'},
    {"dry-run", no_argument, 0, 'n'},
    {"version", no_argument, 0, 'V'},
    {"file", required_argument, 0, 'f'},
    {"no-lock", no_argument, 0, 'K'},
    {"no-fork", no_argument, 0, 'F'},
    {"continuous", no_argument, 0, 'c'},
    {"cache", no_argument, 0, 'a'},
    {"logging", no_argument, 0, 'l'},
    {"index", no_argument, 0, 'i'},
    {"splay_updates", no_argument, 0, 's'},
    {NULL, 0, 0, '\0'}
};

static const char *HINTS[16] =
{
    "Print the help message",
    "Set debugging level 0,1,2,3",
    "Output verbose information about the behaviour of the agent",
    "All talk and no action mode - make no changes, only inform of promises not kept",
    "Output the version of the software",
    "Specify an alternative input file than the default",
    "Ignore locking constraints during execution (ifelapsed/expireafter) if \"too soon\" to run",
    "Run as a foreground processes (do not fork)",
    "Continuous update mode of operation",
    "Rebuild database caches used for efficient query handling (e.g. compliance graphs)",
    "Enable logging of report collection and maintenance to hub_log in the working directory",
    "Reindex all collections in the CFEngine report database",
    "Splay/load balance full-updates, overriding bootstrap times, assuming a default 5 minute update schedule.",
    NULL
};

/*****************************************************************************/

int main(int argc, char *argv[])
{
    GenericAgentConfig config = CheckOpts(argc, argv);

    Policy *policy = GenericInitialize("hub", config);
    ThisAgentInit();
    KeepPromises(policy, config);
    StartHub();
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
    GenericAgentConfig config = GenericAgentDefaultConfig(cf_hub);

    while ((c = getopt_long(argc, argv, "cdvKf:VhFlMaisn", OPTIONS, &optindex)) != EOF)
    {
        switch ((char) c)
        {
        case 'f':

            if (optarg && strlen(optarg) < 5)
            {
                FatalError("-f used but argument \"%s\" incorrect", optarg);
            }

            SetInputFile(optarg);
            MINUSF = true;
            break;

        case 'l':
            LOGGING = true;
            break;

        case 'd':
            NewClass("opt_debug");
            DEBUG = true;
            break;

        case 'K':
            IGNORELOCK = true;
            break;

        case 'I':
            INFORM = true;
            break;

        case 'v':
            VERBOSE = true;
            NO_FORK = true;
            break;

        case 'n':
            DONTDO = true;
            IGNORELOCK = true;
            NewClass("opt_dry_run");
            break;

        case 'c':
            CONTINUOUS = true;
            break;

        case 'a':
            Nova_CacheTotalCompliance(true);
            exit(0);
            break;

        case 'F':
            NO_FORK = true;
            break;

        case 'i':
            CFDB_ReIndexAll();
            exit(0);
            break;

        case 'V':
            PrintVersionBanner("cf-hub");
            exit(0);

        case 'h':
            Syntax("cf-hub - cfengine's report aggregator", OPTIONS, HINTS, ID);
            exit(0);

        case 'M':
            ManPage("cf-hub - cfengine's report aggregator", OPTIONS, HINTS, ID);
            exit(0);

        case 's':
            CheckOpts(argc, argv);
            InitializeGA();

            SplayLongUpdates();
            exit(0);
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

    for (cp = ControlBodyConstraints(policy, cf_hub); cp != NULL; cp = cp->next)
    {
        if (IsExcluded(cp->classes))
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

        if (strcmp(cp->lval, CFH_CONTROLBODY[cfh_federation].lval) == 0)
        {
            Rlist *rp;

            CfDebug("federation ...\n");
            DeleteItemList(FEDERATION);
            FEDERATION = NULL;

            for (rp = (Rlist *) retval.item; rp != NULL; rp = rp->next)
            {
                if (!IsItemIn(FEDERATION, rp->item))
                {
                    AppendItem(&FEDERATION, rp->item, NULL);
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
            CFH_ZENOSS = GetBoolean((const char *) retval.item);
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
        
        memcpy(&entry, value, sizeof(entry));

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
    IPADDRESSES = NULL;

    DeleteScope("this");
    DeleteScope("mon");
    DeleteScope("sys");
    DeleteScope("match");
    NewScope("this");
    NewScope("mon");
    NewScope("sys");
    NewScope("match");

    GetInterfacesInfo(cf_executor);
    Get3Environment();
    OSClasses();
    BuiltinClasses();
    SetReferenceTime(true);
    ThreadUnlock(cft_system);

    for (ip = SCHEDULE; ip != NULL; ip = ip->next)
    {
        CfDebug("Checking schedule %s...\n", ip->name);

        if (IsDefinedClass(ip->name))
        {
            CfOut(cf_verbose, "", "Waking up the agent at %s ~ %s \n", cf_ctime(&CFSTARTTIME), ip->name);
            return true;
        }
    }

    return false;
}

/*****************************************************************************/

static void Nova_UpdateMongoHostList(Item **list)
{
    Item *ip = NULL, *lastseen = NULL, *ip2 = NULL, *new_lastseen = NULL;
    Item *deleted_hosts = NULL;
    int count = 0;

    deleted_hosts = CFDB_GetDeletedHosts();
    lastseen = CFDB_GetLastseenCache();

// add from the new list
    for (ip = *list; ip != NULL; ip = ip->next)
    {
        if (deleted_hosts && IsItemIn(deleted_hosts, ip->name))
        {
            continue;
        }

        PrependFullItem(&new_lastseen, ip->name, ip->classes, 0, time(NULL));
        count++;
    }

// now add items from the prev lastseen db
    for (ip2 = lastseen; ip2 != NULL; ip2 = ip2->next)
    {
        if (IsItemIn(new_lastseen, ip2->name)   //already added from list
            || ((time(NULL) - ip2->time) > CF_HUB_HORIZON)      // entry passed horizon)
            || IsItemIn(deleted_hosts, ip2->name))      //deleted
        {
            continue;
        }

        PrependFullItem(&new_lastseen, ip2->name, ip2->classes, 0, ip2->time);
        count++;
    }

    CFDB_SaveLastseenCache(new_lastseen);

    DeleteItemList(lastseen);
    DeleteItemList(new_lastseen);

    if (deleted_hosts)
    {
        bool removed = true;

        for (ip = deleted_hosts; ip != NULL; ip = ip->next)
        {
            // remove from the local lastseen db
            // TODO: remove the public keys also?

            removed = (removed && RemoveHostFromLastSeen(ip->name));
        }

        // if all hosts marked as "deleted" were removed from cf_lastseen.tcdb
        // purge the list of deleted host
        // otherwise keep it for the next run of cf-hub

        if (removed)
        {
            CFDB_PurgeDeletedHosts();
        }

        DeleteItemList(deleted_hosts);
    }

    CfOut(cf_inform, "", "%d hosts added to the lastseen cache\n", count);
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
        exit(0);
    }

    if (!NO_FORK)
    {
        ActAsDaemon(0);
    }

    WritePID("cf-hub.pid");
    signal(SIGINT, HandleSignals);
    signal(SIGTERM, HandleSignals);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, HandleSignals);
    signal(SIGUSR2, HandleSignals);

    umask(077);

// operating on a non-indexed database can choke the hub
// before Nova_Maintain() gets run
    CFDB_ConnectAndEnsureIndices();

    while (true)
    {
        if (ScheduleRun())
        {
            CfOut(cf_verbose, "", " -> Wake up");

            NewClass("am_policy_hub");
            if (!FEDERATION && CFDB_QueryIsMaster())    // FEDERATION is for Constellation Mission Observatory
            {
                Nova_CollectReports();
                Nova_Maintain();
            }

#ifdef HAVE_CONSTELLATION
            if (FEDERATION)
            {
                Constellation_CollectFederatedReports(FEDERATION);
            }
#endif
        }

        CfOut(cf_verbose, "", "Sleeping...");
        sleep(CFPULSETIME);
    }

    YieldCurrentLock(thislock); // Never get here
}

/********************************************************************/
/* level                                                            */
/********************************************************************/

static void Nova_CollectReports(void)
{
    Item *masterhostlist = Nova_ScanClients();

    Nova_Scan(masterhostlist);
    DeleteItemList(masterhostlist);

    if (CFH_ZENOSS && IsDefinedClass("Min00_05"))
    {
        Nova_ZenossSummary(DOCROOT);
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

    EndMeasure("ReportCollectAll", measure_start);

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

        if (!RemoveFinishedPid(finished, children, &nchildren))
        {
            CfOut(cf_error, "", "Unexpected child reaped! pid %d.", finished);
        }
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

static void Nova_CountMonitoredClasses(void)
{
    char work[CF_BUFSIZE];
    HubQuery *hq;
    Rlist *rp;
    Item *order_results = NULL, *ip;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    snprintf(work, CF_MAXVARSIZE - 1, "%s.*", MONITOR_CLASS_PREFIX);

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, NULL, work, true, now - CF_HUB_HORIZON, now, NULL, false);

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

static Item *Nova_ScanClients(void)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    int ksize, vsize;
    Item *list = NULL;
    time_t now = time(NULL);
    int counter = 0;

    if (!OpenDB(&dbp, dbid_lastseen))
    {
        return NULL;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CloseDB(dbp);
        CfOut(cf_inform, "", " !! Unable to scan last-seen database");
        return NULL;
    }

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        /* Only read the hostkey entries */

        if (key[0] != 'k')
        {
            continue;
        }

        char hostkey[CF_BUFSIZE];
        strlcpy(hostkey, (char *)key + 1, CF_BUFSIZE);
        const char *address = value;

        /* Get the last seen timestamp */

        time_t timestamp = 0;

        char quality_key[CF_BUFSIZE];
        snprintf(quality_key, CF_BUFSIZE, "qi%s", hostkey);

        KeyHostSeen quality;

        if (ReadDB(dbp, quality_key, &quality, sizeof(quality)) == true)
        {
            timestamp = MAX(timestamp, now - quality.lastseen);
        }

        snprintf(quality_key, CF_BUFSIZE, "qo%s", hostkey);

        if (ReadDB(dbp, quality_key, &quality, sizeof(quality)) == true)
        {
            timestamp = MAX(timestamp, now - quality.lastseen);
        }

        Item *ip = PrependItem(&list, hostkey, address);
        ip->time = timestamp;

        if (counter++ > LICENSES)
        {
            CfOut(cf_error,""," !! This hub is only licensed to support %d clients, so truncating at %d", LICENSES, LICENSES);
            break;
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    Nova_RemoveExcludedHosts(&list, EXCLUDE_HOSTS);

    Nova_UpdateMongoHostList(&list);
    DeleteItemList(list);

// If there is a list in Mongo, this takes precedence, else populate one
    list = CFDB_GetLastseenCache();
    Nova_RemoveExcludedHosts(&list, EXCLUDE_HOSTS);

    return list;
}

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
