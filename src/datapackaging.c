
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: datapackaging.c                                                     */
/*                                                                           */
/* Created: Wed Jun 16 14:26:50 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "lastseen.h"
#include "granules.h"
#include "files_names.h"
#include "item_lib.h"

#define CF_MAXTRANSSIZE (CF_BUFSIZE - CF_INBAND_OFFSET - 64)

/* For sorting */
typedef struct
{
    char name[256];
    time_t date;
    double q;
    double d;
} CEnt;

/*****************************************************************************/

void Nova_PackPerformance(Item **reply, char *header, time_t from, enum cfd_menu type)
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

    CfOut(cf_verbose, "", " -> Packing performance data");

    if (!OpenDB(&dbp, dbid_performance))
    {
        CfOut(cf_inform, "", " !! Unable to open performance database");
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan performance database");
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
            memcpy(&entry, value, sizeof(entry));

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

            if (PURGE == 'y')
            {
                if (now - then > SECONDS_PER_WEEK)
                {
                    DBCursorDeleteEntry(dbcp);
                }

                CfOut(cf_inform, "", "Deleting expired entry for %s\n", eventname);

                if (measure < 0 || average < 0 || measure > 4 * SECONDS_PER_WEEK)
                {
                    DBCursorDeleteEntry(dbcp);
                }

                CfOut(cf_inform, "",
                      " -> Deleting entry for %s because it seems to take longer than 4 weeks to complete\n",
                      eventname);

                continue;
            }

            // Format: time_t entry.t; char * "name"; double q; double E(q); double sigma

            if (strlen(eventname) == 0 || strlen(eventname) > CF_MAXVARSIZE / 2)
            {
                CfOut(cf_inform, "", " !! Corrupt entry in database");
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

void Nova_PackClasses(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    double now = (double) time(NULL), average = 0, var = 0;
    char eventname[CF_BUFSIZE], buffer[CF_MAXVARSIZE];
    Event entry;
    CEnt array[1024];
    int i, ksize, vsize, first = true;

    CfOut(cf_verbose, "", " -> Packing class data");

    if (!OpenDB(&dbp, dbid_classes))
    {
        return;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

/* Initialize the key/data return pair. */

    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

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
            memcpy(&entry, value, sizeof(entry));

            then = entry.t;
            average = entry.Q.expect;
            var = entry.Q.var;

            if (now - then > (time_t) SECONDS_PER_WEEK)
            {
                DBCursorDeleteEntry(dbcp);
                CfOut(cf_inform, "", " -> Deleting expired entry for %s\n", eventname);
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

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackSetuid(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    char start[32];
    int first = true;

    CfOut(cf_verbose, "", " -> Packing setuid data");

    snprintf(name, CF_BUFSIZE, "%s/cfagent.%s.log", CFWORKDIR, VSYSNAME.nodename);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
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

void Nova_PackFileChanges(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    char start[32];
    long lthen;
    time_t then, now = time(NULL);
    int i = 0, first = true, kept = CF_CHANGE_HORIZON, repaired = 0, not_kept = 0;

    CfOut(cf_verbose, "", " -> Packing file change data");
    snprintf(name, CF_BUFSIZE - 1, "%s/state/%s", CFWORKDIR, CF_FILECHANGE);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
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

void Nova_PackDiffs(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_BUFSIZE], size[CF_MAXVARSIZE];
    char no[CF_SMALLBUF], change[CF_BUFSIZE], changeNoTab[CF_BUFSIZE], reformat[CF_BUFSIZE],
        output[CF_MAXTRANSSIZE], aggregate[CF_BUFSIZE];
    Item *ip, *file = NULL;
    char pm;
    int first = true;
    time_t then;
    long lthen;

    CfOut(cf_verbose, "", " -> Packing diff data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, NOVA_DIFF_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        if (strncmp(line, "CHANGE", strlen("CHANGE")) != 0)
        {
            continue;
        }

        name[0] = '\0';
        sscanf(line, "CHANGE %[^\n]", name);

        fgets(line, CF_BUFSIZE - 1, fin);
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

            Chop(line);
            snprintf(reformat, CF_BUFSIZE - 1, "%c,%s,%s%c", pm, no, changeNoTab, CF_N_CODE);

            if (strlen(aggregate) + strlen(name) < 800) // truncate output if too large
            {
                Join(aggregate, reformat, sizeof(aggregate));
            }
            else
            {
                CfOut(cf_inform, "", "!! Diff of file \"%s\" is too large be transmitted - truncating", name);
                snprintf(reformat, sizeof(reformat), "%c,%s,%s%c", pm, no, "(TRUNCATED)", CF_N_CODE);
                Join(aggregate, reformat, sizeof(aggregate));
                break;
            }
        }

        snprintf(output, sizeof(output), "%ld|%s|%s\n", then, name, aggregate);

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

void Nova_PackMonitorMg(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, slot;
    Averages entry, det;
    time_t now, here_and_now;
    double havedata;
    char timekey[CF_MAXVARSIZE], buffer[CF_MAXTRANSSIZE];
    Item *data = { 0 };
    CF_DB *dbp;

    CfOut(cf_verbose, "", " -> Packing monitor magnified data");

    if (!OpenDB(&dbp, dbid_observations))
    {
        CfOut(cf_verbose, "", "Couldn't open average database\n");
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
            for (i = 0; i < CF_OBSERVABLES; i++)
            {
                entry.Q[i].expect += det.Q[i].expect;
                entry.Q[i].var += det.Q[i].var;
                entry.Q[i].q += det.Q[i].q;
                entry.Q[i].dq += det.Q[i].dq;
                havedata += entry.Q[i].expect;
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

void Nova_PackMonitorWk(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int its, i, j, slot;
    double kept = 0, not_kept = 0, repaired = 0;
    Averages entry, det;
    char timekey[CF_MAXVARSIZE], buffer[CF_MAXTRANSSIZE];
    Item *data = { 0 };
    time_t now;
    CF_DB *dbp;

    CfOut(cf_verbose, "", " -> Packing monitor weekly data");

    if (!OpenDB(&dbp, dbid_observations))
    {
        CfOut(cf_verbose, "", "Couldn't open average database\n");
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
                for (i = 0; i < CF_OBSERVABLES; i++)
                {
                    entry.Q[i].expect += det.Q[i].expect / (double) its;
                    entry.Q[i].var += det.Q[i].var / (double) its;
                    entry.Q[i].q += det.Q[i].q / (double) its;
                    entry.Q[i].dq += det.Q[i].dq / (double) its;
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

void Nova_PackMonitorYr(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, j, k;
    CF_DB *dbp;
    time_t now = CFSTARTTIME;

/* Start with 3*52 - 1 weeks ago, so the 3*52th week is the current one */
    time_t w = SubtractWeeks(WeekBegin(now), MONITORING_HISTORY_LENGTH_WEEKS - 1);
    Item *data = { 0 };

    CfOut(cf_verbose, "", " -> Packing and compressing monitor 3 year data");

    if (!OpenDB(&dbp, dbid_history))
    {
        CfOut(cf_verbose, "", "Couldn't open long history database");
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

void Nova_FormatMonitoringReply(Item **datap, Item **reply, enum cfd_menu type)
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

void Nova_PackMonitorHist(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int i, j, k, day, position;
    int ok[CF_OBSERVABLES];
    char filename[CF_BUFSIZE];
    char buffer[CF_MAXTRANSSIZE], val[CF_SMALLBUF];
    double weekly[CF_OBSERVABLES][CF_GRAINS] = { {0} };
    double histogram[CF_OBSERVABLES][7][CF_GRAINS] = { {{0}} };
    double smoothhistogram[CF_OBSERVABLES][7][CF_GRAINS] = { {{0}} };
    FILE *fp;

    CfOut(cf_verbose, "", " -> Packing histograms");

    snprintf(filename, CF_BUFSIZE, "%s/state/histograms", CFWORKDIR);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        CfOut(cf_verbose, "", "!! Unable to load histogram data");
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
                    CfOut(cf_verbose, "", "!! Could not load histogram data for i=%d,day=%d,position=%d\n",
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

void Nova_PackCompliance(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    double lsea = SECONDS_PER_WEEK;     /* expire after a week */
    Event entry;
    int ksize, vsize, first = true;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *stored;

    CfOut(cf_verbose, "", " -> Packing sum compliance data");

/* Open the db */

    if (!OpenDB(&dbp, dbid_promise_compliance))
    {
        CfOut(cf_verbose, "", "!! Could not open promise compliance database");
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan class db");
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

        strcpy(eventname, (char *) key);
        memcpy(&entry, stored, sizeof(entry));

        then = entry.t;
        measure = entry.Q.q;
        av = entry.Q.expect;
        var = entry.Q.var;
        lastseen = now - then;

        if (then > 0 && lastseen > lsea)
        {
            CfOut(cf_verbose, "", " -> Promise usage record \"%s\" expired, removing...\n", eventname);
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

void Nova_PackSoftware(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE], line[CF_BUFSIZE];
    char buffer[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    int first = true;

    CfOut(cf_verbose, "", " -> Packing software data");

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, SOFTWARE_PACKAGES_CACHE);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(cf_inform, "stat", "Cannot access software report file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(cf_inform, "", "Software report is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
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

void Nova_PackAvailPatches(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int first = true;
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE];
    char buffer[CF_MAXTRANSSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;

    CfOut(cf_verbose, "", " -> Packing available patch report...\n");

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, NOVA_PATCHES_AVAIL);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(cf_inform, "stat", "Cannot access patches available file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(cf_inform, "", "Patches available is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }


    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
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

void Nova_PackPatchStatus(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    int first = true, count = 0;
    FILE *fin;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE];
    char buffer[CF_MAXTRANSSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;

    CfOut(cf_verbose, "", " -> Packing patch installed data");

    snprintf(name, sizeof(name), "%s/state/%s", CFWORKDIR, NOVA_PATCHES_INSTALLED);
    MapName(name);

    struct stat sb;
    
    if(stat(name, &sb) == -1)
    {
        CfOut(cf_inform, "stat", "Cannot access patch status file");
        return;
    }
    
    if(sb.st_mtime < from)
    {
        char timebuf[26];
        CfOut(cf_inform, "", "Patch status is unchanged since %s -- skipping", 
              cf_strtimestamp_local(sb.st_mtime, timebuf));
        return;
    }


    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
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

void Nova_Pack_promise_output_common(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CfOut(cf_verbose, "", " -> Packing promise data (deprecated)");
// Do we still want this?
}

/*****************************************************************************/

void Nova_PackValueReport(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    int ksize, vsize, first = true;
    void *value;
    char *key;
    PromiseValue pt;
    char ref[CF_SMALLBUF];

// Strip out the date resolution so we keep only each day of the year

    CfOut(cf_verbose, "", " -> Packing value data");

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

            if (!Nova_CoarseLaterThan(key, ref))
            {
                continue;
            }

            memcpy(&pt, value, sizeof(pt));
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

void Nova_PackVariables2(Item **reply, char *header, time_t from, enum cfd_menu type)
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

    CfOut(cf_verbose, "", " -> Packing variable data with date stamp");

    if (!OpenDB(&dbp, dbid_variables))
    {
        return;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan variable db");
        CloseDB(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &keySize, &val, &valSize))
    {
        if (val != NULL)
        {
            Variable var;
            /* Properly align */
            memcpy(&var, val, sizeof(Variable));

            if (var.e.t < from)
            {
                continue;
            }

            sscanf(key, "%255[^.].%s255", scope, lval);

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

            snprintf(buf, sizeof(buf), "%s,%ld,%s,%s\n", dtypeStr, var.e.t, lval, var.rval);
            AppendItem(reply, buf, NULL);
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

}

/*****************************************************************************/

void Nova_PackLastSeen(Item **reply, char *header, time_t from, enum cfd_menu type)
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

    CfOut(cf_verbose, "", " -> Packing last-seen data");

    if (!OpenDB(&dbp, dbid_lastseen))
    {
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan last-seen database");
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
            memcpy(&entry, value, sizeof(entry));

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
            CfOut(cf_inform, "", " -> Deleting expired entry for %s\n", hostkey);
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

void Nova_PackTotalCompliance(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_BUFSIZE], buffer[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    time_t start, end;
    char version[CF_MAXVARSIZE];
    int kept, repaired, notrepaired;
    int i = 0, first = true;
    double av_day_kept = 100, av_day_repaired = 0;
    double av_week_kept = 100, av_week_repaired = 0;
    double av_hour_kept = 100, av_hour_repaired = 0;
    char key[CF_SMALLBUF], ref[CF_SMALLBUF];
    time_t then, now = time(NULL);
    long t;

    CfOut(cf_verbose, "", " -> Packing total compliance data");
    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_PROMISE_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    ref[0] = '\0';

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        sscanf(line, "%ld", &t);
        then = (time_t) t;

        strncpy(key, GenTimeKey(then), CF_SMALLBUF);

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

    for (ip = file; ip != NULL; ip = ip->next)
    {
        kept = repaired = notrepaired = 0;
        memset(version, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        // Complex parsing/extraction

        version[0] = '\0';
        sscanf(ip->name, "%ld,%ld", &start, &end);
        sscanf(strstr(ip->name, "Outcome of version") + strlen("Outcome of version"), "%64[^:]", version);
        sscanf(strstr(ip->name, "to be kept") + strlen("to be kept"), "%d%*[^0-9]%d%*[^0-9]%d", &kept, &repaired,
               &notrepaired);

        if (now - end < SECONDS_PER_DAY)
        {
            av_day_kept = GAverage((double) kept, av_day_kept, 0.5);
            av_day_repaired = GAverage((double) repaired, av_day_repaired, 0.5);
        }

        if (now - end < SECONDS_PER_HOUR)
        {
            av_hour_kept = GAverage((double) kept, av_hour_kept, 0.5);
            av_hour_repaired = GAverage((double) repaired, av_hour_repaired, 0.5);
        }

        if (now - end < SECONDS_PER_WEEK)
        {
            av_week_kept = GAverage((double) kept, av_week_kept, 0.5);
            av_week_repaired = GAverage((double) repaired, av_week_repaired, 0.5);
        }

        // Check for two entries

        if (ip->classes && strlen(ip->classes) > 0)
        {
            int skept = 0, srepaired = 0, snotrepaired = 0;
            char sversion[CF_MAXVARSIZE];

            sversion[0] = '\0';
            sscanf(ip->classes, "%ld,%ld", &start, &end);
            sscanf(strstr(ip->classes, "Outcome of version") + strlen("Outcome of version"), "%64[^:]", sversion);
            sscanf(strstr(ip->classes, "to be kept") + strlen("to be kept"), "%d%*[^0-9]%d%*[^0-9]%d", &skept,
                   &srepaired, &snotrepaired);

            if (now - end < SECONDS_PER_DAY)
            {
                av_day_kept = GAverage((double) skept, av_day_kept, 0.5);
                av_day_repaired = GAverage((double) srepaired, av_day_repaired, 0.5);
            }

            if (now - end < SECONDS_PER_HOUR)
            {
                av_hour_kept = GAverage((double) skept, av_hour_kept, 0.5);
                av_hour_repaired = GAverage((double) srepaired, av_hour_repaired, 0.5);
            }

            if (now - end < SECONDS_PER_WEEK)
            {
                av_week_kept = GAverage((double) skept, av_week_kept, 0.5);
                av_week_repaired = GAverage((double) srepaired, av_week_repaired, 0.5);
            }

            if (strlen(version) + strlen(sversion) + 4 < CF_MAXVARSIZE)
            {
                strcat(version, "<br>");
                strcat(version, sversion);
            }

            kept = (kept + skept) / 2;
            repaired = (repaired + srepaired + 1) / 2;
            notrepaired = (notrepaired + snotrepaired + 1) / 2;
        }

        // Now store

        // NOTE: we must compute the meters above (av_*),
        //       even though we are not sending each entry - so don't continue earlier than this
        i++;

        if (start < from)
        {
            continue;
        }

        snprintf(buffer, sizeof(buffer), "%ld,%s,%d,%d,%d\n", start, version, kept, repaired, notrepaired);

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

    METER_KEPT[meter_compliance_week] = av_week_kept;
    METER_REPAIRED[meter_compliance_week] = av_week_repaired;
    METER_KEPT[meter_compliance_day] = av_day_kept;
    METER_REPAIRED[meter_compliance_day] = av_day_repaired;
    METER_KEPT[meter_compliance_hour] = av_hour_kept;
    METER_REPAIRED[meter_compliance_hour] = av_hour_repaired;
}

/*****************************************************************************/

void Nova_PackRepairLog(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip = NULL, *file = NULL;
    int i = 0, first = true;
    long then;

    CfOut(cf_verbose, "", " -> Packing repair data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_REPAIR_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
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

void Nova_PackNotKeptLog(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    FILE *fin;
    char name[CF_BUFSIZE], line[CF_MAXTRANSSIZE];
    Item *ip, *file = NULL;
    int i = 0, first = true;
    long then;

    CfOut(cf_verbose, "", " -> Packing promise not-kept data");

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_NOTKEPT_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
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

void Nova_PackMeter(Item **reply, char *header, time_t from, enum cfd_menu type)
/**
 * NOTE: This function depends on the meters being computed correctly first,
 *       see e.g. Nova_PackTotalCompliance().
 **/
{
    char line[CF_MAXTRANSSIZE];

    CfOut(cf_verbose, "", " -> Packing meter");

    AppendItem(reply, header, NULL);

    if (METER_KEPT[meter_compliance_week] > 0 || METER_REPAIRED[meter_compliance_week] > 0)
    {
        snprintf(line, sizeof(line), "W: %.4lf %.4lf\n", METER_KEPT[meter_compliance_week],
                 METER_REPAIRED[meter_compliance_week]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_day] > 0 || METER_REPAIRED[meter_compliance_day] > 0)
    {
        snprintf(line, sizeof(line), "D: %.4lf %.4lf\n", METER_KEPT[meter_compliance_day],
                 METER_REPAIRED[meter_compliance_day]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_compliance_hour] > 0 || METER_REPAIRED[meter_compliance_hour] > 0)
    {
        snprintf(line, sizeof(line), "H: %.4lf %.4lf\n", METER_KEPT[meter_compliance_hour],
                 METER_REPAIRED[meter_compliance_hour]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_perf_day] > 0 || METER_REPAIRED[meter_perf_day] > 0)
    {
        snprintf(line, sizeof(line), "P: %.4lf %.4lf\n", METER_KEPT[meter_perf_day], METER_REPAIRED[meter_perf_day]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_other_day] > 0 || METER_REPAIRED[meter_other_day] > 0)
    {
        snprintf(line, sizeof(line), "S: %.4lf %.4lf\n", METER_KEPT[meter_other_day], METER_REPAIRED[meter_other_day]);
        AppendItem(reply, line, NULL);
    }

    Nova_SummarizeComms();

    if (METER_KEPT[meter_comms_hour] > 0 || METER_REPAIRED[meter_comms_hour] > 0)
    {
        snprintf(line, sizeof(line), "C: %.4lf %.4lf\n", METER_KEPT[meter_comms_hour],
                 METER_REPAIRED[meter_comms_hour]);
        AppendItem(reply, line, NULL);
    }

    if (METER_KEPT[meter_anomalies_day] > 0 || METER_REPAIRED[meter_anomalies_day] > 0)
    {
        snprintf(line, sizeof(line), "A: %.4lf %.4lf\n", METER_KEPT[meter_anomalies_day],
                 METER_REPAIRED[meter_anomalies_day]);
        AppendItem(reply, line, NULL);
    }
}

/*****************************************************************************/

void Nova_PackSoftwareDates(Item **reply, char *header, time_t from, enum cfd_menu type)
/**
 * What time the list of installed packages got updated.
 * TODO: Inlcude time for NOVA_PATCHES_INSTALLED and NOVA_PATCHES_AVAIL?
 */
{
    char line[CF_MAXTRANSSIZE];
    char path[CF_MAXVARSIZE];
    struct stat sb;

    CfOut(cf_verbose, "", " -> Packing software dates");

    GetSoftwareCacheFilename(path);

    if (cfstat(path, &sb) != 0)
    {
        CfOut(cf_verbose, "", "Nova_PackSoftwareDates: Could not stat %s", path);
        return;
    }

    time_t lastSeenSw = sb.st_mtime;

    if (lastSeenSw == 0)
    {
        CfOut(cf_verbose, "", "Software cache has been invalidated - skipping\n");
        return;
    }

    AppendItem(reply, header, NULL);

    snprintf(line, sizeof(line), "S:%ld\n", lastSeenSw);
    AppendItem(reply, line, NULL);

}

/*****************************************************************************/

void Nova_PackBundles(Item **reply, char *header, time_t from, enum cfd_menu type)
{
    char line[CF_MAXTRANSSIZE];
    char bundle[CF_MAXVARSIZE];
    Item *file = NULL;
    int first = true, ksize, vsize;
    time_t tid = time(NULL);
    double now = (double) tid, average = 0, var = 0, compliance = 0;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *value;
    Event entry;

    CfOut(cf_verbose, "", " -> Packing bundle log");

    if (!OpenDB(&dbp, dbid_bundles))
    {
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan last-seen database");
        return;
    }

    /* Initialize the key/data return pair. */

    memset(&entry, 0, sizeof(entry));

    /* Walk through the database and print out the key/data pairs. */

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        time_t then;

        strncpy(bundle, (char *) key, ksize);

        if (value != NULL)
        {
            memcpy(&entry, value, sizeof(entry));

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
            CfOut(cf_inform, "", " -> Deleting expired entry for %s\n", bundle);
            continue;
        }

        if (first)
        {
            first = false;
            AppendItem(reply, header, NULL);
        }

        snprintf(line, sizeof(line), "%s %ld %.2lf %.2lf %.2lf\n",
                 bundle,
                 (long) then, compliance, average, sqrt(var));

        AppendItem(reply, line, NULL);
    }

    DeleteDBCursor(dbp, dbcp);
    DeleteItemList(file);
    CloseDB(dbp);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_CoarseLaterThan(char *bigger, char *smaller)
{
    char month_small[CF_SMALLBUF];
    char month_big[CF_SMALLBUF];
    int m_small, day_small, year_small, m_big, year_big, day_big;

    sscanf(smaller, "%d %s %d", &day_small, month_small, &year_small);
    sscanf(bigger, "%d %s %d", &day_big, month_big, &year_big);

    if (year_big < year_small)
    {
        return false;
    }

    m_small = Month2Int(month_small);
    m_big = Month2Int(month_big);

    if (m_big < m_small)
    {
        return false;
    }

    if (day_big < day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    return true;
}

/*****************************************************************************/

int Nova_LaterThan(char *bigger, char *smaller)
{
    char month_small[CF_SMALLBUF];
    char month_big[CF_SMALLBUF];
    int m_small, day_small, year_small, m_big, year_big, day_big;
    int min_small, min_big, hour_small, hour_big;

// Format: Fri Mar 27 15:45:52 2009

    month_small[0] = '\0';
    month_big[0] = '\0';

    sscanf(smaller, "%*s %s %d %d:%d:%*d %d", month_small, &day_small, &hour_small, &min_small, &year_small);
    sscanf(bigger, "%*s %s %d %d:%d:%*d %d", month_big, &day_big, &hour_big, &min_big, &year_big);

    if (year_big < year_small)
    {
        return false;
    }

    m_small = Month2Int(month_small);
    m_big = Month2Int(month_big);

    if (m_big < m_small)
    {
        return false;
    }

    if (day_big < day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    if (hour_big < hour_small && day_big == day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    if (min_big < min_small && hour_big == hour_small && day_big == day_small
        && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    return true;
}

/*****************************************************************************/

char *Nova_ShortArch(char *arch)
{
    if (strcmp(arch, "i386") == 0)
    {
        return "3";
    }

    if (strcmp(arch, "i486") == 0)
    {
        return "4";
    }

    if (strcmp(arch, "i586") == 0)
    {
        return "5";
    }

    if (strcmp(arch, "i686") == 0)
    {
        return "6";
    }

    if (strcmp(arch, "noarch") == 0)
    {
        return "";
    }

    if (strcmp(arch, "x86_64") == 0)
    {
        return "x";
    }

    if (strcmp(arch, "s390") == 0)
    {
        return "t";
    }

    if (strcmp(arch, "s390x") == 0)
    {
        return "s";
    }

    if (strcmp(arch, "default") == 0)
    {
        return "d";
    }

    return arch;
}

/*****************************************************************************/

void Nova_PackExecutionStatus(Item **reply, char *header)
{
    CF_DB *dbp = NULL;
    double avr_interval = 0;
    bool is_black = false;
    time_t last_execution = 0;

    CfOut(cf_verbose, "", " -> Packing execution status data");

    if (!OpenDB(&dbp, dbid_agent_execution))
    {
        CfOut(cf_inform, "", " !! Unable to open agent_execution db");
        return;
    }

    if (!ReadDB(dbp, NOVA_TRACK_DELTA_SCHEDULE, &avr_interval, sizeof(double)))
    {
        CfOut(cf_inform, "", " !! Unable to read from agent_execution db");
        CloseDB(dbp);
        return;
    }

    if (!ReadDB(dbp, NOVA_TRACK_LAST_EXEC, &last_execution, sizeof(time_t)))
    {
        CfOut(cf_inform, "", " !! Unable to read from agent_execution db");
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

