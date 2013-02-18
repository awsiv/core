
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "datapack.h"

#include "lastseen.h"
#include "granules.h"
#include "files_names.h"
#include "files_interfaces.h"
#include "item_lib.h"
#include "sort.h"
#include "conversion.h"
#include "nova_reporting.h"
#include "cfstream.h"
#include "string_lib.h"
#include "logging.h"

#define CF_MAXTRANSSIZE (CF_BUFSIZE - CF_INBAND_OFFSET - 64)

/* For sorting */
typedef struct
{
    char name[256];
    time_t date;
    double q;
    double d;
} CEnt;

typedef struct
{
    double kept;
    double repaired;
    double notrepaired;
    long eventCount;
} ComplianceLevel;

typedef struct
{
    ComplianceLevel total;
    ComplianceLevel user;
    ComplianceLevel internal;
} ComplianceSet;

static ComplianceLevel BalanceCompliance(const ComplianceLevel c1, const ComplianceLevel c2);
static ComplianceLevel GAvgCompliance(ComplianceLevel av, ComplianceLevel comp, double trust);
static Item* ReadTotalComplianceLog(void);
static ComplianceSet InitWeigthCompliance(ComplianceSet comp_set);

/*****************************************************************************/

static void Nova_PackPerformance(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    time_t now = time(NULL);
    double average = 0, var = 0;
    char eventname[CF_BUFSIZE], buffer[CF_BUFSIZE];
    Event entry;
    int ksize, vsize, first = true, kept = 0, repaired = 0, not_kept = 0;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing performance data");

    if (!OpenDB(&dbp, dbid_performance))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to open performance database");
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan performance database");
        CloseDB(dbp);
        return;
    }

    /* Initialize the key/data return pair. */

    memset(&key, 0, sizeof(key));
    memset(&value, 0, sizeof(value));
    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        double measure;
        time_t then;
        char tbuf[CF_BUFSIZE];

        strncpy(eventname, (char *) key, ksize);

        if (value != NULL)
        {
            if (sizeof(entry) < vsize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in performance database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
                continue;
            }

            memcpy(&entry, value, MIN(vsize, sizeof(entry)));

            then = entry.t;
            measure = entry.Q.q;
            average = entry.Q.expect;
            var = entry.Q.var;

            // Promise: reply with data only after the "from" time

            if (now - then > (int) (average + 2.0 * sqrt(var) + 0.5))
            {
                not_kept++;
            }
            else if (now - then > (int) (average + sqrt(var) + 0.5))
            {
                repaired++;
            }
            else
            {
                kept++;
            }

            if (then < from)
            {
                continue;
            }

            snprintf(tbuf, CF_BUFSIZE - 1, "%s", cf_ctime(&then));
            tbuf[strlen(tbuf) - 9] = '\0';      /* Chop off second and year */

            // Format: time_t entry.t; char * "name"; double q; double E(q); double sigma

            if (strlen(eventname) == 0 || strlen(eventname) > CF_MAXVARSIZE / 2)
            {
                CfOut(OUTPUT_LEVEL_INFORM, "", " !! Corrupt entry in database");
                continue;
            }

            if (first)
            {
                first = false;
                AppendItem(reply, header, NULL);
            }

            snprintf(buffer, CF_MAXVARSIZE - 1, "%ld,%7.4lf,%7.4lf,%7.4lf,%s\n", entry.t, measure, average,
                     sqrt(var), eventname);
            AppendItem(reply, buffer, NULL);
        }
        else
        {
            continue;
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    METER_KEPT[meter_perf_day] = 100 * (kept / (kept + not_kept + repaired));
    METER_REPAIRED[meter_perf_day] = 100 * (repaired / (kept + not_kept + repaired));
}

/*****************************************************************************/

static void Nova_PackClasses(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    double now = (double) time(NULL), average = 0, var = 0;
    char eventname[CF_BUFSIZE], buffer[CF_MAXVARSIZE];
    Event entry;
    int i, ksize, vsize, first = true;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing class data");

    if (!OpenDB(&dbp, dbid_classes))
    {
        return;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

/* Initialize the key/data return pair. */

    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

    CEnt *array = xcalloc(1024,sizeof(CEnt));

    for (i = 0; i < 1024; i++)
    {
        *(array[i].name) = '\0';
        array[i].q = -1;
    }

    i = 0;

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        time_t then;

        strncpy(eventname, (char *) key, CF_BUFSIZE - 1);

        if (value != NULL)
        {
            if (sizeof(entry) < vsize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in classes database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
                continue;
            }

            memcpy(&entry, value, MIN(vsize, sizeof(entry)));

            then = entry.t;
            average = entry.Q.expect;
            var = entry.Q.var;

            if (now - then > (time_t) SECONDS_PER_WEEK)
            {
                DBCursorDeleteEntry(dbcp);
                CfOut(OUTPUT_LEVEL_INFORM, "", " -> Deleting expired entry for %s\n", eventname);
                continue;
            }

            if (then < from)
            {
                continue;
            }

            if (i++ < 1024)
            {
                array[i].date = then;
                strncpy(array[i].name, eventname, 254);
                array[i].q = average;
                array[i].d = var;
            }
            else
            {
                break;
            }
        }
    }

    for (i = 0; i < 1024; i++)
    {
        if (array[i].q <= 0.00001)
        {
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        snprintf(buffer, CF_MAXVARSIZE - 1, "%s,%ld,%7.4lf,%7.4lf\n", array[i].name, array[i].date, array[i].q,
                 sqrt(array[i].d));
        AppendItem(reply, buffer, NULL);
    }

    free(array);

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/

static void Nova_PackSetuid(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    char start[32];
    int first = true;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing setuid data");

    snprintf(name, CF_BUFSIZE, "%s/cfagent.%s.log", CFWORKDIR, VSYSNAME.nodename);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, sizeof(line), fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    file = SortItemListNames(file);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(start, 0, 32);
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        /* Promise: Keep a small time-key enabling further compression by delta elimination */

        AppendItem(reply, ip->name, NULL);
    }

    DeleteItemList(file);
}

/*****************************************************************************/
// for versions < 2.3
// Deprecate in favour of Nova_PackFileChanges
/*****************************************************************************/
static void Nova_PackFileChangesOld(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    char start[32];
    long lthen;
    time_t then, now = time(NULL);
    int i = 0, first = true, kept = CF_CHANGE_HORIZON, repaired = 0, not_kept = 0;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing old file change data");
    snprintf(name, CF_BUFSIZE - 1, "%s/state/%s", CFWORKDIR, CF_FILECHANGE);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        return;
    }

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, sizeof(line), fin);

        sscanf(line, "%ld", &lthen);
        then = (time_t) lthen;

        if (now - then < SECONDS_PER_DAY)
        {
            repaired++;
        }

        if (then < from)
        {
            continue;
        }

        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(start, 0, 32);
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, ip->name, NULL);

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    DeleteItemList(file);
    METER_KEPT[meter_other_day] = 100.0 * kept / (kept + repaired + not_kept);
    METER_REPAIRED[meter_other_day] = 100.0 * repaired / (kept + repaired + not_kept);
}

/*****************************************************************************/
static void Nova_PackFileChanges(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    char start[32];
    long lthen;
    time_t then, now = time(NULL);
    int i = 0, first = true, kept = CF_CHANGE_HORIZON, repaired = 0, not_kept = 0;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing file change data");
    snprintf(name, CF_BUFSIZE - 1, "%s/state/%s", CFWORKDIR, CF_FILECHANGE_NEW);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return;
    }

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, sizeof(line), fin);

        sscanf(line, "%ld", &lthen);
        then = (time_t) lthen;

        if (now - then < SECONDS_PER_DAY)
        {
            repaired++;
        }

        if (then < from)
        {
            continue;
        }

        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(start, 0, 32);
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, ip->name, NULL);

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    DeleteItemList(file);
    METER_KEPT[meter_other_day] = 100.0 * kept / (kept + repaired + not_kept);
    METER_REPAIRED[meter_other_day] = 100.0 * repaired / (kept + repaired + not_kept);
}

/*****************************************************************************/

static void Nova_PackDiffs(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_BUFSIZE], size[CF_MAXVARSIZE];
    char no[CF_SMALLBUF], change[CF_BUFSIZE], changeNoTab[CF_BUFSIZE], reformat[CF_BUFSIZE],
         output[CF_MAXTRANSSIZE], aggregate[CF_BUFSIZE];
    char handle[CF_BUFSIZE] = { 0 };
    Item *ip, *file = NULL;
    char pm;
    int first = true;
    time_t then;
    long lthen;
    bool is_handle;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing diff data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, NOVA_DIFF_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        is_handle = false;

        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        if (strncmp(line, "CHANGE", strlen("CHANGE")) != 0)
        {
            continue;
        }

        name[0] = '\0';
        sscanf(line, "CHANGE %[^\n]", name);

        handle[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        /* check if promise handle is included into log record */
        if (strncmp(line, "promise handle:", strlen("promise handle:")) == 0)
        {
            is_handle = true;
            sscanf(line, "promise handle: %[^\n]", handle);
            fgets(line, CF_BUFSIZE - 1, fin);
        }

        sscanf(line, "%ld;%[^\n]", &lthen, size);

        then = (time_t) lthen;

        if (from > then)
        {
            continue;
        }

        if (strncmp(line, "END", strlen("END")) == 0)
        {
            continue;
        }

        memset(aggregate, 0, CF_BUFSIZE);
        output[0] = '\0';

        while (!feof(fin))
        {
            line[0] = '\0';
            fgets(line, CF_BUFSIZE - 1, fin);

            if (strncmp(line, "END", strlen("END")) == 0)
            {
                break;
            }

            no[0] = '\0';
            change[0] = '\0';
            sscanf(line, "%c,%64[^,],%1024[^\n]", &pm, no, change);

            // the web interface doesn't handle TABs
            changeNoTab[0] = '\0';
            ReplaceStr(change, changeNoTab, sizeof(changeNoTab), "\t", "(TAB)");

            if (Chop(line, CF_EXPANDSIZE) == -1)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Chop was called on a string that seemed to have no terminator");
            }
            snprintf(reformat, CF_BUFSIZE - 1, "%c,%s,%s%c", pm, no, changeNoTab, CF_N_CODE);

            if (strlen(aggregate) + strlen(name) < 800) // truncate output if too large
            {
                Join(aggregate, reformat, sizeof(aggregate));
            }
            else
            {
                CfOut(OUTPUT_LEVEL_INFORM, "", "!! Diff of file \"%s\" is too large be transmitted - truncating", name);
                snprintf(reformat, sizeof(reformat), "%c,%s,%s%c", pm, no, "(TRUNCATED)", CF_N_CODE);
                Join(aggregate, reformat, sizeof(aggregate));
                break;
            }
        }

        /* prefix is a protocol version number Enterprise 3.0.0 -> 300
           which make it easier for modyfication */
        if (is_handle)
        {
            snprintf(output, sizeof(output), "300|%ld|%s|%s|%s\n",
                     then, name, handle, aggregate);
        }
        else /* for 2.x.x where handle is missing*/
        {
            /* if promise handle is missing then send empty space for parsing on hub */
            snprintf(output, sizeof(output), "%ld|%s|%s\n",
                     then, name, aggregate);

        }

        if (strlen(aggregate) > 0)
        {
            PrependItem(&file, output, NULL);
            aggregate[0] = '\0';
        }
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, ip->name, NULL);
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_FormatMonitoringReply(Item **datap, Item **reply, enum cfd_menu type)
{
    Item *ip;
    int currId = -1;
    char buffer[CF_MAXTRANSSIZE];

    // sort by observable id
    *datap = SortItemListCounters(*datap);

    for (ip = *datap; ip != NULL; ip = ip->next)
    {
        int slot = ip->counter;

        if (!NovaHasSlot(slot) && slot != 65)
        {
            continue;
        }

        if (currId != slot)
        {
            if (type == cfd_menu_full)  // include more meta-data in full query
            {
                bool consolidable = NovaIsSlotConsolidable(slot);
                int exp_min = NovaGetSlotExpectedMinimum(slot);
                int exp_max = NovaGetSlotExpectedMaximum(slot);
                const char *name = NovaGetSlotName(slot);
                const char *desc = NovaGetSlotDescription(slot);
                const char *units = NovaGetSlotUnits(slot);

                snprintf(buffer, sizeof(buffer), "M:%s,%d,%d,%d,%s,%s",
                         name, consolidable, exp_min, exp_max, units, desc);

            }
            else
            {
                snprintf(buffer, sizeof(buffer), "M:%s", NovaGetSlotName(slot));
            }

            AppendItem(reply, buffer, NULL);
            currId = slot;
        }

        AppendItem(reply, ip->name, NULL);
    }
}

/*****************************************************************************/

static void Nova_PackMonitorMg(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, slot;
    Averages entry, det;
    time_t now, here_and_now;
    double havedata;
    char timekey[CF_MAXVARSIZE], buffer[CF_MAXTRANSSIZE];
    Item *data = { 0 };
    CF_DB *dbp;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing monitor magnified data");

    if (!OpenDB(&dbp, dbid_observations))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Couldn't open average database\n");
        return;
    }

    now = time(NULL);
    here_and_now = now - (SECONDS_PER_HOUR * 4);        // we only display last 4hrs in mag graph

    strcpy(timekey, GenTimeKey(here_and_now));
    slot = GetTimeSlot(here_and_now);

    while (here_and_now < now)
    {
        havedata = 0;
        memset(&entry, 0, sizeof(entry));

        CfDebug("timekey=%s\n", timekey);

        if (from > here_and_now)        // skip if client wants newer data
        {
            here_and_now += CF_MEASURE_INTERVAL;
            strcpy(timekey, GenTimeKey(here_and_now));
            slot = (slot + 1) % CF_MAG_SLOTS;
            continue;
        }

        if (ReadDB(dbp, timekey, &det, sizeof(Averages)))
        {
            if (now > det.last_seen + SECONDS_PER_WEEK)
            {
               for (i = 0; i < CF_OBSERVABLES; i++)
               {
                   // Data are stale because cf-monitord was not running, delete last measured value
                   entry.Q[i].expect = det.Q[i].expect;
                   entry.Q[i].var = det.Q[i].var;
                   entry.Q[i].q = 0;
                   entry.Q[i].dq = det.Q[i].dq;
                   havedata += entry.Q[i].expect;
               }
           }
           else
           {
               for (i = 0; i < CF_OBSERVABLES; i++)
               {
                   entry.Q[i].expect += det.Q[i].expect;
                   entry.Q[i].var += det.Q[i].var;
                   entry.Q[i].q += det.Q[i].q;
                   entry.Q[i].dq += det.Q[i].dq;
                   havedata += entry.Q[i].expect;
               }
           }
        }

        if (havedata != 0)
        {
            for (i = 0; i < CF_OBSERVABLES; i++)
            {
                if (entry.Q[i].expect > 0 || entry.Q[i].var > 0 || entry.Q[i].q > 0)
                {
                    snprintf(buffer, sizeof(buffer), "%d %.4lf %.4lf %.4lf %.4lf", slot, entry.Q[i].q, entry.Q[i].expect,
                             sqrt(entry.Q[i].var), entry.Q[i].dq);
                    PrependItem(&data, buffer, NULL);
                    data->counter = i;  // OBS index - sorted on later
                }
            }
        }

        here_and_now += CF_MEASURE_INTERVAL;
        strcpy(timekey, GenTimeKey(here_and_now));
        slot = (slot + 1) % CF_MAG_SLOTS;
    }

    CloseDB(dbp);

    if (data)
    {
        AppendItem(reply, header, NULL);
        Nova_FormatMonitoringReply(&data, reply, type);
        DeleteItemList(data);
    }

}

/*****************************************************************************/

static void Nova_PackMonitorWk(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int its, i, j, slot;
    double kept = 0, not_kept = 0, repaired = 0;
    Averages entry, det;
    char timekey[CF_MAXVARSIZE], buffer[CF_MAXTRANSSIZE];
    Item *data = { 0 };
    time_t now;
    CF_DB *dbp;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing monitor weekly data");

    if (!OpenDB(&dbp, dbid_observations))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Couldn't open average database\n");
        return;
    }

    its = 12;                   // 1 hour coarse graining resolution (12 5min slots)

    now = CF_MONDAY_MORNING;

    for (slot = 0; now < CF_MONDAY_MORNING + SECONDS_PER_WEEK; slot++)  // one-hour slots
    {
        memset(&entry, 0, sizeof(entry));

        for (j = 0; j < its; j++)       // average into one-hour slots
        {
            strcpy(timekey, GenTimeKey(now));

            if (ReadDB(dbp, timekey, &det, sizeof(Averages)))
            {
                if (now > det.last_seen + SECONDS_PER_WEEK)
                {
                    for (i = 0; i < CF_OBSERVABLES; i++)
                    {
                        // Data are stale because cf-monitord was not running, delete last measured value
                        entry.Q[i].expect += det.Q[i].expect / (double) its;
                        entry.Q[i].var += det.Q[i].var / (double) its;
                        entry.Q[i].q += 0;
                        entry.Q[i].dq += det.Q[i].dq / (double) its;
                        
                    }
                }
                else
                {
                    for (i = 0; i < CF_OBSERVABLES; i++)
                    {
                        entry.Q[i].expect += det.Q[i].expect / (double) its;
                        entry.Q[i].var += det.Q[i].var / (double) its;
                        entry.Q[i].q += det.Q[i].q / (double) its;
                        entry.Q[i].dq += det.Q[i].dq / (double) its;
                    }
                }
            }
            
            now += CF_MEASURE_INTERVAL;
        }

        for (i = 0; i < CF_OBSERVABLES; i++)
        {
            if (entry.Q[i].q > entry.Q[i].expect + 2.0 * sqrt(entry.Q[i].var))
            {
                not_kept++;
                continue;
            }

            if (entry.Q[i].q > entry.Q[i].expect + sqrt(entry.Q[i].var))
            {
                repaired++;
                continue;
            }

            if (entry.Q[i].q < entry.Q[i].expect - 2.0 * sqrt(entry.Q[i].var))
            {
                not_kept++;
                continue;
            }

            if (entry.Q[i].q < entry.Q[i].expect - sqrt(entry.Q[i].var))
            {
                repaired++;
                continue;
            }

            kept++;
        }

        for (i = 0; i < CF_OBSERVABLES; i++)
        {
            if (entry.Q[i].expect > 0 || entry.Q[i].var > 0 || entry.Q[i].q > 0)
            {
                snprintf(buffer, sizeof(buffer), "%d %.4lf %.4lf %.4lf %.4lf", slot, entry.Q[i].q, entry.Q[i].expect,
                         sqrt(entry.Q[i].var), entry.Q[i].dq);
                PrependItem(&data, buffer, NULL);
                data->counter = i;      // OBS index - sorted on later
            }
        }

    }

    CloseDB(dbp);

// Promise: need to reproduce this for the monitoring, possibly on the far side

    METER_KEPT[meter_anomalies_day] = 100.0 * kept / (kept + repaired + not_kept);
    METER_REPAIRED[meter_anomalies_day] = 100.0 * repaired / (kept + repaired + not_kept);

    if (data)
    {
        AppendItem(reply, header, NULL);
        Nova_FormatMonitoringReply(&data, reply, type);
        DeleteItemList(data);
    }

}

/*****************************************************************************/

static void Nova_PackMonitorYr(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, j, k;
    CF_DB *dbp;
    time_t now = CFSTARTTIME;

/* Start with 3*52 - 1 weeks ago, so the 3*52th week is the current one */
    time_t w = SubtractWeeks(WeekBegin(now), MONITORING_HISTORY_LENGTH_WEEKS - 1);
    Item *data = { 0 };

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing and compressing monitor 3 year data");

    if (!OpenDB(&dbp, dbid_history))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Couldn't open long history database");
        return;
    }

    for (i = 0; i < MONITORING_HISTORY_LENGTH_WEEKS; ++i)
    {
        /* Collect data for a week */

        bool have_data = false;
        int num[CF_OBSERVABLES] = { 0 };
        double q[CF_OBSERVABLES] = { 0.0 };
        double var[CF_OBSERVABLES] = { 0.0 };
        double e[CF_OBSERVABLES] = { 0.0 };
        double dq[CF_OBSERVABLES] = { 0.0 };

        for (j = 0; j < SHIFTS_PER_WEEK && w <= now; ++j, w = NextShift(w))
        {
            Averages av;

            if (GetRecordForTime(dbp, w, &av))
            {
                have_data = true;
                for (k = 0; k < CF_OBSERVABLES; ++k)
                {
                    num[k]++;
                    q[k] += BoundedValue(av.Q[k].q, 0);
                    var[k] += BoundedValue(av.Q[k].var, q[k] * q[k]);
                    e[k] += BoundedValue(av.Q[k].expect, q[k]);
                    dq[k] += BoundedValue(av.Q[k].dq, dq[k]);
                }
            }
        }

        if (have_data)
        {
            char buffer[CF_MAXTRANSSIZE];

            for (k = 0; k < CF_OBSERVABLES; ++k)
            {
                if (q[k] > 0 && var[k] > 0 && e[k] > 0)
                {
                    snprintf(buffer, sizeof(buffer), "%d %.4lf %.4lf %.4lf %.4lf", i, q[k] / num[k], e[k] / num[k],
                             sqrt(var[k] / num[k]), dq[k] / num[k]);
                    PrependItem(&data, buffer, NULL);
                    data->counter = k;  // OBS index - sorted on later
                }
            }
        }
    }

    CloseDB(dbp);

    if (data)
    {
        AppendItem(reply, header, NULL);
        Nova_FormatMonitoringReply(&data, reply, type);
        DeleteItemList(data);
    }
}

/*****************************************************************************/

static void Nova_PackMonitorHist(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, j, k, day, position;
    int ok[CF_OBSERVABLES];
    char filename[CF_BUFSIZE];
    char buffer[CF_MAXTRANSSIZE], val[CF_SMALLBUF];
    double weekly[CF_OBSERVABLES][CF_GRAINS] = { {0} };
    double histogram[CF_OBSERVABLES][7][CF_GRAINS] = { {{0}} };
    double smoothhistogram[CF_OBSERVABLES][7][CF_GRAINS] = { {{0}} };
    FILE *fp;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing histograms");

    snprintf(filename, CF_BUFSIZE, "%s/state/histograms", CFWORKDIR);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Unable to load histogram data");
        return;
    }

    for (position = 0; position < CF_GRAINS; position++)
    {
        fscanf(fp, "%d ", &position);

        for (i = 0; i < CF_OBSERVABLES; i++)
        {
            for (day = 0; day < 7; day++)
            {
                if (fscanf(fp, "%lf ", &(histogram[i][day][position])) != 1)
                {
                    CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not load histogram data for i=%d,day=%d,position=%d\n",
                          i, day, position);
                    histogram[i][day][position] = 0;
                }
            }
        }
    }

    fclose(fp);

    for (k = 1; k < CF_GRAINS - 1; k++)
    {
        for (j = 0; j < CF_OBSERVABLES; j++)
        {
            for (i = 0; i < 7; i++)
            {
                smoothhistogram[j][i][k] = (histogram[j][i][k - 1] + histogram[j][i][k] + histogram[j][i][k + 1]) / 3.0;
            }
        }
    }

    AppendItem(reply, header, NULL);

    for (j = 0; j < CF_OBSERVABLES; j++)
    {
        ok[j] = false;

        for (k = 0; k < CF_GRAINS; k++)
        {
            for (i = 0; i < 7; i++)
            {
                weekly[j][k] += (smoothhistogram[j][i][k] + 0.5);

                if (weekly[j][k] > 0 && smoothhistogram[j][i][k] != 0)
                {
                    ok[j] = true;
                }

                if (weekly[j][k] < 0 || weekly[j][k] > CF_BIGNUMBER)
                {
                    weekly[j][k] = 0;
                }
            }
        }
    }

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        if (ok[i])
        {
            if (!NovaHasSlot(i))
            {
                CfDebug("Could not find slot %d when packing monitoring histograms - skipping\n", i);
                continue;
            }

            snprintf(buffer, sizeof(buffer), "%s,", NovaGetSlotName(i));

            for (k = 0; k < CF_GRAINS; k++)
            {
                snprintf(val, CF_SMALLBUF, "%d,", (int) round(weekly[i][k]));
                Join(buffer, val, sizeof(buffer));
            }
            ReplaceTrailingChar(buffer, ',', '\0');

            AppendItem(reply, buffer, NULL);
        }
    }
}

/*****************************************************************************/

static void Nova_PackCompliance(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    double lsea = SECONDS_PER_WEEK;     /* expire after a week */
    Event entry;
    int ksize, vsize, first = true;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *stored;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing sum compliance data");

/* Open the db */

    if (!OpenDB(&dbp, dbid_promise_compliance))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not open promise compliance database");
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

/* Initialize the key/data return pair. */

    while (NextDB(dbp, dbcp, &key, &ksize, &stored, &vsize))
    {
        double measure, av, var;
        time_t then, lastseen, now = time(NULL);
        char eventname[CF_BUFSIZE];
        char name[CF_BUFSIZE] = "";

        if (sizeof(entry) < vsize)
        {
            CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in promise compliance database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
            continue;
        }

        strcpy(eventname, (char *) key);
        memcpy(&entry, stored, MIN(vsize, sizeof(entry)));

        then = entry.t;
        measure = entry.Q.q;
        av = entry.Q.expect;
        var = entry.Q.var;
        lastseen = now - then;

        if (then > 0 && lastseen > lsea)
        {
            CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Promise usage record \"%s\" expired, removing...\n", eventname);
            DBCursorDeleteEntry(dbcp);
        }
        else
        {
            if (measure > 0.9)  // avoid rounding errors
            {
                // Compliant
                snprintf(name, sizeof(name), "%ld,%s,c,%.1lf,%.1lf\n", then, eventname, av * 100.0, sqrt(var) * 100.0);
            }
            else if (measure > 0.4 && measure < 0.6)
            {
                // Repaired
                snprintf(name, sizeof(name), "%ld,%s,r,%.1lf,%.1lf\n", then, eventname, av * 100.0, sqrt(var) * 100.0);
            }
            else if (measure < 0.1)
            {
                // Non-compliant
                snprintf(name, sizeof(name), "%ld,%s,n,%.1lf,%.1lf\n", then, eventname, av * 100.0, sqrt(var) * 100.0);
            }

            if (first && strlen(name) > 0)
            {
                first = false;
                AppendItem(reply, header, NULL);
            }

            if (strlen(name) > 0)
            {
                AppendItem(reply, name, NULL);
            }
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/

static void Nova_PackSoftware(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE], line[CF_BUFSIZE];
    char buffer[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    int first = true;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing software data");

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, SOFTWARE_PACKAGES_CACHE);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "stat", "Cannot access software report file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(OUTPUT_LEVEL_INFORM, "", "Software report is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        snprintf(buffer, sizeof(buffer), "%s,%s,%s\n", name, version, Nova_ShortArch(arch));

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, buffer, NULL);
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_PackAvailPatches(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int first = true;
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE];
    char buffer[CF_MAXTRANSSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing available patch report...\n");

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, NOVA_PATCHES_AVAIL);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "stat", "Cannot access patches available file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(OUTPUT_LEVEL_INFORM, "", "Patches available is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }


    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        snprintf(buffer, sizeof(buffer), "%s,%s,%s\n", name, version, Nova_ShortArch(arch));

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, buffer, NULL);
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_PackPatchStatus(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int first = true, count = 0;
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE];
    char buffer[CF_MAXTRANSSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing patch installed data");

    snprintf(name, sizeof(name), "%s/state/%s", CFWORKDIR, NOVA_PATCHES_INSTALLED);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "stat", "Cannot access patch status file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(OUTPUT_LEVEL_INFORM, "", "Patch status is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }


    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        snprintf(buffer, sizeof(buffer), "%s,%s,%s\n", name, version, Nova_ShortArch(arch));

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, buffer, NULL);
        count++;
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_Pack_promise_output_common(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing promise data (deprecated)");
// Do we still want this?
}

/*****************************************************************************/

static void Nova_PackValueReport(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    int ksize, vsize, first = true;
    void *value;
    char *key;
    PromiseValue pt;
    char ref[CF_SMALLBUF];

// Strip out the date resolution so we keep only each day of the year

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing value data");

    if (!OpenDB(&dbp, dbid_value))
    {
        return;
    }

    TimeToDateStr(from, ref, sizeof(ref));

    if (NewDBCursor(dbp, &dbcp))
    {
        while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
        {
            if (value == NULL)
            {
                continue;
            }

            if (sizeof(pt) < vsize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in values database. Expected size: %zu, actual size: %d", sizeof(pt), vsize);
                continue;
            }

            if (!Nova_CoarseLaterThan(key, ref))
            {
                continue;
            }

            memcpy(&pt, value, MIN(vsize, sizeof(pt)));
            char name[CF_BUFSIZE];
            snprintf(name, sizeof(name), "%s,%.4lf,%.4lf,%.4lf\n", key, pt.kept, pt.repaired, pt.notkept);

            if (first)
            {
                first = false;
                AppendItem(reply, header, NULL);
            }

            AppendItem(reply, name, NULL);
        }

        DeleteDBCursor(dbp, dbcp);
    }

    CloseDB(dbp);
}

/*****************************************************************************/

static void Nova_PackVariables2(Item **reply, char *header, time_t from, enum cfd_menu type)
/* Includes date-stamp of variable (but not avg and stddev). */
{
    char buf[CF_MAXTRANSSIZE];
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;                  // scope.lval
    void *val;
    int keySize, valSize;
    int first = true;
    char scope[CF_MAXVARSIZE], lval[CF_MAXVARSIZE], prevScope[CF_MAXVARSIZE] = { 0 };
    char *dtypeStr;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing variable data with date stamp");

    if (!OpenDB(&dbp, dbid_variables))
    {
        return;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan variable db");
        CloseDB(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &keySize, &val, &valSize))
    {
        if (val != NULL)
        {
            if (sizeof(Variable) < valSize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in variables database. Expected size: %zu, actual size: %d", sizeof(Variable), valSize);
                continue;
            }

            Variable var;
            /* Properly align */
            memcpy(&var, val, MIN(valSize, sizeof(Variable)));

            if (var.e.t < from)
            {
                continue;
            }

            sscanf(key, "%255[^.].%s255", scope, lval);

            /* Only return "sys" variables in the delta report */
            if(type == cfd_menu_delta && strcmp(scope, "sys") != 0)
            {
                continue;
            }

            if (strcmp(scope, "const") == 0)
            {
                // skip const scope (newline, etc.)
                continue;
            }

            if (first)
            {
                first = false;
                AppendItem(reply, header, NULL);
            }

            // ensure we are in right scope-section
            if (strcmp(scope, prevScope) != 0)
            {
                snprintf(buf, sizeof(buf), "S:%s", scope);
                AppendItem(reply, buf, NULL);
                strcpy(prevScope, scope);
            }

            dtypeStr = Dtype2Str(var.dtype);

            char rval_without_newline[CF_MAXTRANSSIZE] = "\0";
            ReplaceChar(var.rval, rval_without_newline, sizeof(rval_without_newline), '\n', CF_N_CODE);

            snprintf(buf, sizeof(buf), "%s,%ld,%s,%s\n", dtypeStr, var.e.t, lval, rval_without_newline);
            AppendItem(reply, buf, NULL);
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

}

/*****************************************************************************/

static void Nova_PackLastSeen(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    time_t tid = time(NULL);
    double now = (double) tid, average = 0, var = 0;
    double ticksperhr = (double) SECONDS_PER_HOUR;
    char buffer[CF_MAXTRANSSIZE];
    KeyHostSeen entry;
    int ksize, vsize, first = true;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing last-seen data");

    if (!OpenDB(&dbp, dbid_lastseen))
    {
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan last-seen database");
        CloseDB(dbp);
        return;
    }

    /* Initialize the key/data return pair. */

    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        double then;
        time_t fthen;
        char addr[CF_BUFSIZE];

        /* Only read the 'quality of connection' entries */

        if (key[0] != 'q')
        {
            continue;
        }

        bool incoming = (*(key + 1) == 'i');
        char hostkey[CF_BUFSIZE];
        strlcpy(hostkey, (char *) key + 2, CF_BUFSIZE);

        if (value != NULL)
        {
            if (sizeof(entry) < vsize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in lastseen database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
                continue;
            }

            memcpy(&entry, value, MIN(vsize, sizeof(entry)));

            then = entry.lastseen;
            average = (double) entry.Q.expect;
            var = (double) entry.Q.var;

            /* Resolve address */

            char hostkey_key[CF_BUFSIZE];
            snprintf(hostkey_key, CF_BUFSIZE, "k%s", hostkey);
            ReadDB(dbp, hostkey_key, &addr, CF_BUFSIZE);
        }
        else
        {
            continue;
        }

        if (now - then > (double) LASTSEENEXPIREAFTER)
        {
            DBCursorDeleteEntry(dbcp);
            CfOut(OUTPUT_LEVEL_INFORM, "", " -> Deleting expired entry for %s\n", hostkey);
            continue;
        }

        fthen = (time_t) then;  /* format date */

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        snprintf(buffer, sizeof(buffer), "%c %s %s %ld %.2lf %.2lf %.2lf\n",
                 incoming ? '+' : '-',
                 hostkey,
                 addr,
                 (long) fthen, ((double) (now - then)) / ticksperhr, average / ticksperhr, sqrt(var) / ticksperhr);

        AppendItem(reply, buffer, NULL);
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/

static Item* ReadTotalComplianceLog(void)
{
    char name[CF_BUFSIZE] = "";
    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_PROMISE_LOG);
    MapName(name);

    FILE *fin;
    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return NULL;
    }

/* Max 2016 entries - at least a week */

    Item *file = NULL;
    int line_num = 0;
    char ref[CF_SMALLBUF] = "";

    while (!feof(fin))
    {
        line_num++;
        char line[CF_BUFSIZE] = "";
        fgets(line, CF_BUFSIZE - 1, fin);

        intmax_t start_i, end_i;
        sscanf(line, "%" SCNdMAX ",%" SCNdMAX "", &start_i, &end_i);

        if(start_i > end_i)
        {
            CfOut(OUTPUT_LEVEL_VERBOSE, "",
                  "Start time is greater than end time. Excluding entry from calculation."
                  "(File %s/%s, Line: %d)", CFWORKDIR, CF_PROMISE_LOG, line_num);

            continue;
        }

        char key[CF_SMALLBUF] = "";
        strncpy(key, GenTimeKey((time_t) start_i), CF_SMALLBUF - 1);

        if (strcmp(ref, key) == 0)
        {
            // If more than one (max 2) store the other data in classes
            if (file->classes == NULL)
            {
                file->classes = xstrdup(line);
            }
        }
        else
        {
            PrependItem(&file, line, NULL);
            strcpy(ref, key);
        }
    }

    fclose(fin);

    return file;
}

static ComplianceLevel BalanceCompliance(const ComplianceLevel c1, const ComplianceLevel c2)
{
    ComplianceLevel ret = { 0, 0, 0, 0 };

    if ((c1.eventCount + c2.eventCount) <= 0)
    {
        return ret;
    }

    ret.kept = ((double)((c1.kept * c1.eventCount) + (c2.kept * c2.eventCount))) /
            (double)(c1.eventCount + c2.eventCount);
    ret.repaired = ((double)((c1.repaired * c1.eventCount) + (c2.repaired * c2.eventCount) + 1.0)) /
            (double)(c1.eventCount + c2.eventCount);
    ret.notrepaired = ((double)((c1.notrepaired * c1.eventCount) + (c2.notrepaired * c2.eventCount) + 1.0)) /
            (double)(c1.eventCount + c2.eventCount);

    return ret;
}

static ComplianceLevel GAvgCompliance(ComplianceLevel av, ComplianceLevel comp, double trust)
{
    ComplianceLevel res = { 0, 0, 0, 0 };

    res.kept = GAverage(comp.kept, av.kept, trust);
    res.repaired = GAverage(comp.repaired, av.repaired, trust);

    return res;
}

static ComplianceSet InitWeigthCompliance(ComplianceSet comp_set)
{
    ComplianceSet res = {{ 0, 0, 0, 0 },
                         { 0, 0, 0, 0 },
                         { 0, 0, 0, 0 }};

    res.total.kept = comp_set.total.kept;
    res.total.repaired = comp_set.total.repaired;

    res.user.kept = comp_set.user.kept;
    res.user.repaired = comp_set.user.repaired;

    res.internal.kept = comp_set.internal.kept;
    res.internal.repaired = comp_set.internal.repaired;

    return res;
}

static void Nova_PackTotalCompliance(Item **reply, char *header, time_t from,
                                     enum cfd_menu type)
{     
    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing total compliance data");

    double trust_level = 0.5; // Trust level for geometrical avg for compliance avg

    Item *file = ReadTotalComplianceLog();

    if (file == NULL)
    {
        return;
    }

    ComplianceSet av_day = {{.kept = 0, .repaired = 0, .eventCount = 0},
                            {.kept = 0, .repaired = 0, .eventCount = 0},
                            {.kept = 0, .repaired = 0, .eventCount = 0}};

    ComplianceSet av_week = {{.kept = 0, .repaired = 0, .eventCount = 0},
                             {.kept = 0, .repaired = 0, .eventCount = 0},
                             {.kept = 0, .repaired = 0, .eventCount = 0}};

    ComplianceSet av_hour = {{.kept = 0, .repaired = 0, .eventCount = 0},
                             {.kept = 0, .repaired = 0, .eventCount = 0},
                             {.kept = 0, .repaired = 0, .eventCount = 0}};

    int i = 0;
    bool first = true;
    time_t now = time(NULL);
    bool extended = false; // extended data introduced in Enterprise 3.0

    for (Item *ip = file; ip != NULL; ip = ip->next)
    {        
        if (strlen(ip->name) == 0)
        {
            continue;
        }

        // Complex parsing/extraction
        ComplianceSet comp = {{ 0, 0, 0, -1 },
                              { -1, -1, -1, -1 },
                              { -1, -1, -1, -1 }};

        intmax_t start_i, end_i;
        sscanf(ip->name, "%" SCNdMAX ",%" SCNdMAX "", &start_i, &end_i);

        time_t start = (time_t)start_i;
        time_t end = (time_t)end_i;

        char version[CF_MAXVARSIZE] = "";
        sscanf(strstr(ip->name, "Outcome of version") + strlen("Outcome of version"), "%64[^:]", version);

        sscanf(strstr(ip->name, "Promises observed") + strlen("Promises observed"),
               "%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]",
               &(comp.total.kept), &(comp.total.repaired),
               &(comp.total.notrepaired), &(comp.total.eventCount),
               &(comp.user.kept), &(comp.user.repaired),
               &(comp.user.notrepaired), &(comp.user.eventCount),
               &(comp.internal.kept), &(comp.internal.repaired),
               &(comp.internal.notrepaired), &(comp.internal.eventCount));

        bool extended_record_1 = false;
        if (comp.total.eventCount != -1) /* event_count is first of extended report, if missing then report is in old format */
        {
            extended_record_1 = true;
            extended = true;
        }

        // Check for two entries

        if (ip->classes && strlen(ip->classes) > 0)
        {
            ComplianceSet scomp = {{ 0, 0, 0, -1 },
                                  { -1, -1, -1, -1 },
                                  { -1, -1, -1, -1 }};

            char sversion[CF_MAXVARSIZE];

            sversion[0] = '\0';
            sscanf(ip->classes, "%" SCNdMAX ",%" SCNdMAX "", &start_i, &end_i);

            start = (time_t)start_i;
            end = (time_t)end_i;
            sscanf(strstr(ip->classes, "Outcome of version") + strlen("Outcome of version"), "%64[^:]", sversion);

            sscanf(strstr(ip->classes, "Promises observed") + strlen("Promises observed"),
                   "%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%ld%*[^0-9]",
                   &(scomp.total.kept), &(scomp.total.repaired),
                   &(scomp.total.notrepaired), &(scomp.total.eventCount),
                   &(scomp.user.kept), &(scomp.user.repaired),
                   &(scomp.user.notrepaired), &(scomp.user.eventCount),
                   &(scomp.internal.kept), &(scomp.internal.repaired),
                   &(scomp.internal.notrepaired), &(scomp.internal.eventCount));

            bool extended_record_2 = false;
            if (scomp.total.eventCount != -1) /* sevent_count is first of extended report, if missing then report is in old format */
            {
                extended_record_2 = true;
                extended = true;
            }

            if (strlen(version) + strlen(sversion) + 4 < CF_MAXVARSIZE)
            {
                strcat(version, "<br>");
                strcat(version, sversion);
            }

            if (extended_record_1 && extended_record_2)
            {
                comp.total = BalanceCompliance(comp.total, scomp.total);
                comp.user = BalanceCompliance(comp.user, scomp.user);
                comp.internal = BalanceCompliance(comp.internal, scomp.internal);
            }
            else
            {
                comp.total.kept = (comp.total.kept + scomp.total.kept) / 2;
                comp.total.repaired = (comp.total.repaired + scomp.total.repaired + 1) / 2;
                comp.total.notrepaired = (comp.total.notrepaired + scomp.total.notrepaired + 1) / 2;
            }

        }

        // Calculate compliance meters over time-windows from balanced data sets
        if (first)
        {
            if (extended_record_1)
            {
                av_week = InitWeigthCompliance(comp);
                av_hour = InitWeigthCompliance(comp);
                av_day = InitWeigthCompliance(comp);
            }
            else
            {
                av_week.total.kept = comp.total.kept;
                av_week.total.repaired = comp.total.repaired;

                av_day.total.kept = comp.total.kept;
                av_day.total.repaired = comp.total.repaired;

                av_hour.total.kept = comp.total.kept;
                av_hour.total.repaired = comp.total.repaired;
            }
        }

        if (now - end < SECONDS_PER_DAY)
        {
            if (extended_record_1)
            {
                av_day.total = GAvgCompliance(av_day.total, comp.total, trust_level);
                av_day.user = GAvgCompliance(av_day.user, comp.user, trust_level);
                av_day.internal = GAvgCompliance(av_day.internal, comp.internal, trust_level);
            }
            else
            {
                av_day.total = GAvgCompliance(av_day.total, comp.total, trust_level);
            }
        }

        if (now - end < SECONDS_PER_HOUR)
        {
            if (extended_record_1)
            {
                av_hour.total = GAvgCompliance(av_hour.total, comp.total, trust_level);
                av_hour.user = GAvgCompliance(av_hour.user, comp.user, trust_level);
                av_hour.internal = GAvgCompliance(av_hour.internal, comp.internal, trust_level);
            }
            else
            {
                av_hour.total = GAvgCompliance(av_hour.total, comp.total, trust_level);
            }
        }

        if (now - end < SECONDS_PER_WEEK)
        {
            if (extended_record_1)
            {
                av_week.total = GAvgCompliance(av_week.total, comp.total, trust_level);
                av_week.user = GAvgCompliance(av_week.user, comp.user, trust_level);
                av_week.internal = GAvgCompliance(av_week.internal, comp.internal, trust_level);
            }
            else
            {
                av_week.total = GAvgCompliance(av_week.total, comp.total, trust_level);
            }
        }

        // Now store

        // NOTE: we must compute the meters above (av_*),
        //       even though we are not sending each entry - so don't continue earlier than this
        i++;

        if (start < from)
        {
            continue;
        }

        char buffer[CF_MAXTRANSSIZE] = "";
        if (comp.user.repaired == -1) // for old format < Nova 2.x
        {
            snprintf(buffer, sizeof(buffer), "%ld,%s,%d,%d,%d\n",
                     start, version, (int)comp.total.kept,
                     (int)comp.total.repaired, (int)comp.total.notrepaired);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%ld,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                     start, version,
                     (int)(comp.total.kept + 0.5), (int)(comp.total.repaired + 0.5),
                     (int)(comp.total.notrepaired + 0.5), (int)(comp.user.kept + 0.5),
                     (int)(comp.user.repaired + 0.5), (int)(comp.user.notrepaired + 0.5),
                     (int)(comp.internal.kept + 0.5), (int)(comp.internal.repaired + 0.5),
                     (int)(comp.internal.notrepaired + 0.5));
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, buffer, NULL);

        if (i > 12 * 24 * 7)
        {
            break;
        }
    }

    DeleteItemList(file);

    if (extended) /* kept_user is first of extended report, if missing then report is in old format */
    {
        METER_KEPT[meter_compliance_week] = (int)(av_week.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_week] = (int)(av_week.total.repaired + 0.5);
        METER_KEPT[meter_compliance_day] = (int)(av_day.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_day] = (int)(av_day.total.repaired + 0.5);
        METER_KEPT[meter_compliance_hour] = (int)(av_hour.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_hour] = (int)(av_hour.total.repaired + 0.5);

        METER_KEPT[meter_compliance_week_user] = (int)(av_week.user.kept + 0.5);
        METER_REPAIRED[meter_compliance_week_user] = (int)(av_week.user.repaired + 0.5);
        METER_KEPT[meter_compliance_day_user] = (int)(av_day.user.kept + 0.5);
        METER_REPAIRED[meter_compliance_day_user] = (int)(av_day.user.repaired + 0.5);
        METER_KEPT[meter_compliance_hour_user] = (int)(av_hour.user.kept + 0.5);
        METER_REPAIRED[meter_compliance_hour_user] = (int)(av_hour.user.repaired + 0.5);

        METER_KEPT[meter_compliance_week_internal] = (int)(av_week.internal.kept + 0.5);
        METER_REPAIRED[meter_compliance_week_internal] = (int)(av_week.internal.repaired + 0.5);
        METER_KEPT[meter_compliance_day_internal] = (int)(av_day.internal.kept + 0.5);
        METER_REPAIRED[meter_compliance_day_internal] = (int)(av_day.internal.repaired + 0.5);
        METER_KEPT[meter_compliance_hour_internal] = (int)(av_hour.internal.kept + 0.5);
        METER_REPAIRED[meter_compliance_hour_internal] = (int)(av_hour.internal.repaired + 0.5);
    }
    else
    {
        METER_KEPT[meter_compliance_week] = (int)(av_week.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_week] = (int)(av_week.total.repaired + 0.5);
        METER_KEPT[meter_compliance_day] = (int)(av_day.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_day] = (int)(av_day.total.repaired + 0.5);
        METER_KEPT[meter_compliance_hour] = (int)(av_hour.total.kept + 0.5);
        METER_REPAIRED[meter_compliance_hour] = (int)(av_hour.total.repaired + 0.5);

        METER_KEPT[meter_compliance_week_user] = -1;
        METER_REPAIRED[meter_compliance_week_user] = -1;
        METER_KEPT[meter_compliance_day_user] = -1;
        METER_REPAIRED[meter_compliance_day_user] = -1;
        METER_KEPT[meter_compliance_hour_user] = -1;
        METER_REPAIRED[meter_compliance_hour_user] = -1;

        METER_KEPT[meter_compliance_week_internal] = -1;
        METER_REPAIRED[meter_compliance_week_internal] = -1;
        METER_KEPT[meter_compliance_day_internal] = -1;
        METER_REPAIRED[meter_compliance_day_internal] = -1;
        METER_KEPT[meter_compliance_hour_internal] = -1;
        METER_REPAIRED[meter_compliance_hour_internal] = -1;
    }
}

/*****************************************************************************/

static void Nova_PackRepairLog(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip = NULL, *file = NULL;
    int i = 0, first = true;
    long then;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing repair data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_REPAIR_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, sizeof(line), fin);

        sscanf(line, "%ld", &then);

        if ((time_t) then < from)
        {
            continue;
        }

        if (strlen(line) == 0 || (line[0] == '\n'))
        {
            continue;
        }

        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, ip->name, NULL);

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_PackNotKeptLog(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    int i = 0, first = true;
    long then;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing promise not-kept data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_NOTKEPT_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, sizeof(line), fin);

        sscanf(line, "%ld", &then);

        if ((time_t) then < from)
        {
            continue;
        }

        if (strlen(line) == 0 || (line[0] == '\n'))
        {
            continue;
        }

        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        AppendItem(reply, ip->name, NULL);

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    DeleteItemList(file);
}

/*****************************************************************************/

static void Nova_PackMeter(Item **reply, char *header, time_t from, enum cfd_menu type)
/**
 * NOTE: This function depends on the meters being computed correctly first,
 *       see e.g. Nova_PackTotalCompliance().
 **/
{
    char line[CF_MAXTRANSSIZE];

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing meter");

    AppendItem(reply, header, NULL);

    if (METER_KEPT[meter_compliance_week] > 0 || METER_REPAIRED[meter_compliance_week] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_week,
                 METER_KEPT[meter_compliance_week],
                 METER_REPAIRED[meter_compliance_week]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_week_user] > 0 || METER_REPAIRED[meter_compliance_week_user] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_week_user,
                 METER_KEPT[meter_compliance_week_user],
                 METER_REPAIRED[meter_compliance_week_user]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_week_internal] > 0 || METER_REPAIRED[meter_compliance_week_internal] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_week_internal,
                 METER_KEPT[meter_compliance_week_internal],
                 METER_REPAIRED[meter_compliance_week_internal]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_day] > 0 || METER_REPAIRED[meter_compliance_day] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_day,
                 METER_KEPT[meter_compliance_day],
                 METER_REPAIRED[meter_compliance_day]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_day_user] > 0 || METER_REPAIRED[meter_compliance_day_user] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_day_user,
                 METER_KEPT[meter_compliance_day_user],
                 METER_REPAIRED[meter_compliance_day_user]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_day_internal] > 0 || METER_REPAIRED[meter_compliance_day_internal] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_day_internal,
                 METER_KEPT[meter_compliance_day_internal],
                 METER_REPAIRED[meter_compliance_day_internal]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_hour] > 0 || METER_REPAIRED[meter_compliance_hour] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_hour,
                 METER_KEPT[meter_compliance_hour],
                 METER_REPAIRED[meter_compliance_hour]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_hour_user] > 0 || METER_REPAIRED[meter_compliance_hour_user] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_hour_user,
                 METER_KEPT[meter_compliance_hour_user],
                 METER_REPAIRED[meter_compliance_hour_user]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_hour_internal] > 0 || METER_REPAIRED[meter_compliance_hour_internal] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_hour_internal,
                 METER_KEPT[meter_compliance_hour_internal],
                 METER_REPAIRED[meter_compliance_hour_internal]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_perf_day] > 0 || METER_REPAIRED[meter_perf_day] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_perf,
                 METER_KEPT[meter_perf_day],
                 METER_REPAIRED[meter_perf_day]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_other_day] > 0 || METER_REPAIRED[meter_other_day] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_other,
                 METER_KEPT[meter_other_day],
                 METER_REPAIRED[meter_other_day]);
        AppendItem(reply, line, NULL);
    }

    Nova_SummarizeComms();

    if (METER_KEPT[meter_comms_hour] > 0 || METER_REPAIRED[meter_comms_hour] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_comms,
                 METER_KEPT[meter_comms_hour],
                 METER_REPAIRED[meter_comms_hour]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_anomalies_day] > 0 || METER_REPAIRED[meter_anomalies_day] > 0)
    {
        snprintf(line, sizeof(line), "%c: %.4lf %.4lf\n",
                 cfmeter_anomaly,
                 METER_KEPT[meter_anomalies_day],
                 METER_REPAIRED[meter_anomalies_day]);
        AppendItem(reply, line, NULL);
    }
}

/*****************************************************************************/

static void Nova_PackSoftwareDates(Item **reply, char *header, time_t from, enum cfd_menu type)
/**
 * What time the list of installed packages got updated.
 * TODO: Inlcude time for NOVA_PATCHES_INSTALLED and NOVA_PATCHES_AVAIL?
 */
{
    char line[CF_MAXTRANSSIZE];
    char path[CF_MAXVARSIZE];
    struct stat sb;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing software dates");

    GetSoftwareCacheFilename(path);

    if (cfstat(path, &sb) != 0)
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Nova_PackSoftwareDates: Could not stat %s", path);
        return;
    }

    time_t lastSeenSw = sb.st_mtime;

    if (lastSeenSw == 0)
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Software cache has been invalidated - skipping\n");
        return;
    }

    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(OUTPUT_LEVEL_INFORM, "", "Software report is unchanged since %s -- skipping",
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }

    AppendItem(reply, header, NULL);

    snprintf(line, sizeof(line), "S:%ld\n", lastSeenSw);
    AppendItem(reply, line, NULL);

}

/*****************************************************************************/

static void Nova_PackBundles(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    char line[CF_MAXTRANSSIZE];
    char bundle_fqname[CF_MAXVARSIZE];
    Item *file = NULL;
    int first = true, ksize, vsize;
    time_t tid = time(NULL);
    double now = (double) tid, average = 0, var = 0, compliance = 0;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    Event entry;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing bundle log");

    if (!OpenDB(&dbp, dbid_bundles))
    {
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan last-seen database");
        return;
    }

    /* Initialize the key/data return pair. */

    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        time_t then;

        strncpy(bundle_fqname, (char *) key, ksize);

        /* ignore tcdb versioning record */
        if (!strcmp(bundle_fqname, "version"))
        {
            continue;
        }


        if (value != NULL)
        {
            if (sizeof(entry) < vsize)
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", "Invalid entry in bundles database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
                continue;
            }

            memcpy(&entry, value, MIN(vsize, sizeof(entry)));

            then = entry.t;
            compliance = (double) entry.Q.q;
            average = (double) entry.Q.expect;
            var = (double) entry.Q.var;
        }
        else
        {
            continue;
        }

        if (now - then > (double) LASTSEENEXPIREAFTER)
        {
            DBCursorDeleteEntry(dbcp);
            CfOut(OUTPUT_LEVEL_INFORM, "", " -> Deleting expired entry for %s\n", bundle_fqname);
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }


        char bundle_namespace[CF_MAXVARSIZE];
        char bundle_name[CF_MAXVARSIZE];
        if (!BundleQualifiedNameSplit(bundle_fqname, bundle_namespace, bundle_name))
        {
            CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to extract bundle name and namespace from %s:", bundle_fqname);
            continue;
        }

        snprintf(line, sizeof(line), "%s %ld %.2lf %.2lf %.2lf %s\n",
                 bundle_name,
                 (long) then, compliance, average, sqrt(var), bundle_namespace);

        AppendItem(reply, line, NULL);
    }

    DeleteDBCursor(dbp, dbcp);
    DeleteItemList(file);
    CloseDB(dbp);
}

/*****************************************************************************/

static void Nova_PackExecutionStatus(Item **reply, char *header)
{
    CF_DB *dbp = NULL;
    double avr_interval = 0;
    bool is_black = false;
    time_t last_execution = 0;

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Packing execution status data");

    if (!OpenDB(&dbp, dbid_agent_execution))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to open agent_execution db");
        return;
    }

    if (!ReadDB(dbp, NOVA_TRACK_DELTA_SCHEDULE, &avr_interval, sizeof(double)))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to read from agent_execution db");
        CloseDB(dbp);
        return;
    }

    if (!ReadDB(dbp, NOVA_TRACK_LAST_EXEC, &last_execution, sizeof(time_t)))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to read from agent_execution db");
        CloseDB(dbp);
        return;
    }

    CloseDB(dbp);

    time_t now = time(NULL);
    double last_exec_interval = (double)(now - last_execution);
    double threshold = (avr_interval * CF_BLACKHOST_THRESHOLD);
    threshold += (double)(threshold * (CF_BLACKHOST_THRESHOLD_VARIATION * 0.01)); // add mesurment error tolerance

    if (last_exec_interval > threshold)
    {
    is_black = true;
    }

    char buffer[CF_MAXTRANSSIZE];
    snprintf(buffer, sizeof(buffer), "%c %ld\n", (is_black)? 't' : 'f', (long)avr_interval);

    AppendItem(reply, header, NULL);
    AppendItem (reply, buffer, NULL);
}


void Nova_PackAllReports(Item **reply, time_t from, time_t delta1, enum cfd_menu type)
/**
 * Creates the reply item list from all available reports on this host.
 */
{
    time_t tReply;
    char buffer[CF_MAXVARSIZE];

    switch (type)
    {
    case cfd_menu_delta:
        Nova_PackPerformance(reply, CFR_PERF, from, type);
        Nova_PackClasses(reply, CFR_CLASS, from, type);
        // read from the old filechange log during until all clients have been upgraded
        Nova_PackFileChangesOld(reply, CFR_FCHANGE_OLD, from, type);
        Nova_PackFileChanges(reply, CFR_FCHANGE, from, type);
        Nova_PackDiffs(reply, CFR_FDIFF, from, type);
        //Nova_PackMonitorMg(reply,CFR_MONITOR_MAG,from,type);  // DEPRECATED
        Nova_PackMonitorMg(reply, CFR_MONITOR_MG, from, type);
        Nova_PackCompliance(reply, CFR_PCOMPLIANCE, from, type);
        Nova_PackTotalCompliance(reply, CFR_TCOMPLIANCE, from, type);
        Nova_PackLastSeen(reply, CFR_LASTSEEN, from, type);
        Nova_PackRepairLog(reply, CFR_REPAIRLOG, from, type);
        Nova_PackNotKeptLog(reply, CFR_NOTKEPTLOG, from, type);
        Nova_PackMeter(reply, CFR_METER, from, type);
        Nova_PackBundles(reply, CFR_BUNDLES, from, type);
        Nova_PackExecutionStatus(reply, CFR_EXECUTION_STATUS);
        Nova_PackSoftware(reply, CFR_SOFTWARE, from, type);
        Nova_PackSoftwareDates(reply, CFR_SWDATES, from, type);
        Nova_PackAvailPatches(reply, CFR_AVAILPATCH, from, type);
        Nova_PackPatchStatus(reply, CFR_PATCHSTATUS, from, type);
        Nova_PackVariables2(reply, CFR_VARD, from, type);
        break;

    case cfd_menu_full:

        Nova_PackPerformance(reply, CFR_PERF, from, type);
        Nova_PackClasses(reply, CFR_CLASS, from, type);
        Nova_PackSetuid(reply, CFR_SETUID, from, type);
        // read from the old filechange log during until all clients have been upgraded
        Nova_PackFileChangesOld(reply, CFR_FCHANGE_OLD, from, type);
        Nova_PackFileChanges(reply, CFR_FCHANGE, from, type);
        Nova_PackDiffs(reply, CFR_FDIFF, from, type);
        //Nova_PackMonitorMg(reply,CFR_MONITOR_MAG,from,type);  // DEPRECATED
        //Nova_PackMonitorWk(reply,CFR_MONITOR_WEEK,from,type); // DEPRECATED
        //Nova_PackMonitorYr(reply,CFR_MONITOR_YEAR,from,type); // DEPRECATED
        Nova_PackMonitorMg(reply, CFR_MONITOR_MG, from, type);
        Nova_PackMonitorWk(reply, CFR_MONITOR_WK, from, type);
        Nova_PackMonitorYr(reply, CFR_MONITOR_YR, from, type);
        Nova_PackMonitorHist(reply, CFR_MONITOR_HG, from, type);
        Nova_PackCompliance(reply, CFR_PCOMPLIANCE, from, type);
        Nova_PackTotalCompliance(reply, CFR_TCOMPLIANCE, from, type);
        Nova_PackSoftware(reply, CFR_SOFTWARE, from, type);
        Nova_PackSoftwareDates(reply, CFR_SWDATES, from, type);
        Nova_PackAvailPatches(reply, CFR_AVAILPATCH, from, type);
        Nova_PackPatchStatus(reply, CFR_PATCHSTATUS, from, type);
        Nova_Pack_promise_output_common(reply, CFR_PROMISEOUT, from, type);
        Nova_PackValueReport(reply, CFR_VALUE, from, type);
        Nova_PackVariables2(reply, CFR_VARD, from, type);
        Nova_PackLastSeen(reply, CFR_LASTSEEN, from, type);
        Nova_PackRepairLog(reply, CFR_REPAIRLOG, from, type);
        Nova_PackNotKeptLog(reply, CFR_NOTKEPTLOG, from, type);
        Nova_PackMeter(reply, CFR_METER, from, type);
        Nova_PackBundles(reply, CFR_BUNDLES, from, type);
        Nova_PackExecutionStatus(reply, CFR_EXECUTION_STATUS);
        break;

    default:
        FatalError("Unknown type '%d' in Nova_PackAllReports", type);
    }

    tReply = time(NULL);
    CfOut(OUTPUT_LEVEL_VERBOSE, "", " -> Assembled reply at %s", cf_ctime(&tReply));

    snprintf(buffer, sizeof(buffer), "CFR: %ld %ld %d", delta1, tReply, ItemListSize(*reply));
    PrependItem(reply, buffer, NULL);

}

