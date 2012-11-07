
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "datapack.h"

#include "sysinfo.h"
#include "db_save.h"

static char *CF_CODEBOOK[] =
{
    CFR_PERF,
    CFR_CLASS,
    CFR_SETUID,
    CFR_FCHANGE_OLD,
    CFR_FCHANGE,
    CFR_FDIFF,
    CFR_MONITOR_MAG,
    CFR_MONITOR_WEEK,
    CFR_MONITOR_YEAR,
    CFR_MONITOR_HIST,
    CFR_MONITOR_MG,
    CFR_MONITOR_WK,
    CFR_MONITOR_YR,
    CFR_MONITOR_HG,
    CFR_PCOMPLIANCE,
    CFR_TCOMPLIANCE,
    CFR_SOFTWARE,
    CFR_AVAILPATCH,
    CFR_PATCHSTATUS,
    CFR_PROMISEOUT,
    CFR_VALUE,
    CFR_VARD,
    CFR_VARS,
    CFR_LASTSEEN,
    CFR_REPAIRLOG,
    CFR_NOTKEPTLOG,
    CFR_METER,
    CFR_BUNDLES,
    CFR_SWDATES,
    CFR_EXECUTION_STATUS,
};

#define CF_CODEBOOK_SIZE (sizeof(CF_CODEBOOK)/sizeof(CF_CODEBOOK[0]))

static void Nova_UnPackPerformance(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Performance data ...................");

    if (dbconn)
    {
        CFDB_SavePerformance(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            time_t t;
            char eventname[CF_MAXVARSIZE] = "\0";
            double measure = 0,
                    average = 0,
                    dev = 0;

            sscanf(ip->name, "%ld,%lf,%lf,%lf,%255[^\n]\n", &t, &measure, &average, &dev, eventname);
            CfDebug("Performance of \"%s\" is %.4lf (av %.4lf +/- %.4lf) measured at %s\n", eventname, measure, average,
                    dev, cf_ctime(&t));
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackClasses(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Class data .................");

    if (dbconn)
    {
        CFDB_SaveClasses(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char name[CF_MAXVARSIZE] = "\0";
            time_t t;
            double q = 0,
                   dev = 0;

            // Extract records
            sscanf(ip->name, "%[^,],%ld,%lf,%lf\n", name, &t, &q, &dev);
            CfDebug("Class: \"%s\" seen with probability %.4lf +- %.4lf last seen at %s\n", name, q, dev, cf_ctime(&t));
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackSetuid(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> setuid data ......................");

    if (dbconn)
    {
        CFDB_SaveSetUid(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("Set-uid program: %s", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackFileChangesOld(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Old File change data....................");

    if (dbconn)
    {
        CFDB_SaveFileChangesOld(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char name[CF_MAXVARSIZE] = "\0";
            long date;
            time_t then;

            // Extract records
            sscanf(ip->name, "%ld,%255[^\n]", &date, name);
            then = (time_t) date;
            CfDebug("Old File-change event: in \"%s\" at %ld\n", name, then);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackFileChanges(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> File change data....................");

    if (dbconn)
    {
        CFDB_SaveFileChanges(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char name[CF_MAXVARSIZE] = "\0";
            char handle[CF_MAXVARSIZE] = "\0";
            long date;
            time_t then;

            // Extract records
            sscanf(ip->name, "%ld,%255[^,],%255[^\n]", &date, handle, name);
            then = (time_t) date;

            CfDebug("File-change event: in \"%s\" at %ld (by promise: %s)\n", name, then, handle);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackDiffs(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> File diff data...................");

    if (dbconn)
    {
        CFDB_SaveFileDiffs(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            // Extract records
            char name[CF_MAXVARSIZE] = "\0",
                    change[CF_BUFSIZE] = "\0",
                    handle[CF_MAXVARSIZE] = "\0";

            bool is_handle = false;
            long t;

            /* protocol for  Enterprise 3.0.0 */
            if (strncmp(ip->name, "300", strlen("300")) == 0)
            {
                sscanf(ip->name, "300|%ld|%255[^|]|%255[^|]|%2047[^\n]",
                       &t, name, handle, change);

                is_handle = true;
            }
            else /* protocol for Enterpise 2.x.x */
            {
                sscanf(ip->name, "%ld|%255[^|]|%2047[^\n]", &t, name, change);
            }

            for (char *sp = change; *sp != '\0'; sp++)
            {
                if (*sp == CF_N_CODE)
                {
                    *sp = '\n';
                }
            }

            CfDebug("Change-diff: in file %s at %ld for promise handle: %s\nbegin\n%s\nend\n",
                    name, t, (is_handle)? handle : "undefined", change);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorWeek(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor weekly data.....................");
    CfOut(cf_inform, "", "!! Deprecated monitor weekly format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED             //HAVE_LIBMONGOC
    if (dbconn)
    {
        CFDB_SaveMonitorData(dbconn, id, mon_rep_week, data);
    }
#endif

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            int slot;
            char t[CF_TIME_SIZE] = "\0";

            // Extract time stamp
            if (strncmp(ip->name, "T: ", 3) == 0)
            {
                memset(t, 0, CF_TIME_SIZE);
                sscanf(ip->name + 3, "%31[^,],%d", t, &slot);
                continue;
            }

            // Extract records
            double q = 0,
                   e = 0,
                   dev = 0,
                   dq = 0;

            int observable = 0;

            sscanf(ip->name, "%d %lf %lf %lf %lf\n", &observable, &q, &e, &dev, &dq);
            CfDebug("Week-obs %d in slot %d: %.2lf,%.2lf,%.2lf,%.2lf\n", observable, slot, q, e, dev, dq);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorMag(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor magnified data.....................");
    CfOut(cf_inform, "", "!! Deprecated monitor magnified format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED             //HAVE_LIBMONGOC
    if (dbconn)
    {
        CFDB_SaveMonitorData(dbconn, id, mon_rep_mag, data);
    }
#endif

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            int slot = 0;
            // Extract time stamp
            if (strncmp(ip->name, "T: ", 3) == 0)
            {
                sscanf(ip->name + 3, "%d", &slot);
                continue;
            }

            // Extract records
            int observable = 0;
            double q = 0,
                   e = 0,
                   dev = 0,
                   dq = 0;

            sscanf(ip->name, "%d %lf %lf %lf %lf", &observable, &q, &e, &dev, &dq);
            CfDebug("Mag-obs %d: %.2lf,%.2lf,%.2lf,%.2lf measured for slot %d\n", observable, q, e, dev, dq, slot);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorYear(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor year data.....................");
    CfOut(cf_inform, "", "!! Deprecated monitor year format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED             //HAVE_LIBMONGOC
    if (dbconn)
    {
        CFDB_SaveMonitorData(dbconn, id, mon_rep_yr, data);
    }
#endif

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            int slot = 0;

            // Extract time stamp
            if (strncmp(ip->name, "T: ", 3) == 0)
            {
                sscanf(ip->name + 3, "%d", &slot);  // 3*12
                continue;
            }

            // Extract records
            int observable = 0;
            double q = 0,
                   e = 0,
                   dev = 0,
                   dq = 0;

            sscanf(ip->name, "%d %lf %lf %lf %lf\n", &observable, &q, &e, &dev, &dq);
            CfDebug("Year-obs %d: %.2lf,%.2lf,%.2lf,%.2lf measured at slot %d\n", observable, q, e, dev, dq, slot);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorHist(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor histogram data.....................");
    CfOut(cf_inform, "", "!! Deprecated monitor histogram format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED             // HAVE_LIBMONGOC
    if (dbconn)
    {
        CFDB_SaveMonitorHistograms(dbconn, id, data);
    }
#endif

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("OLDHist-obs %s\n", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorHg(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor histogram data.....................");

    if (dbconn)
    {
        CFDB_SaveMonitorHistograms(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("Hist-obs %s\n", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorMg(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor magnified data.....................");

    if (dbconn)
    {
        CFDB_SaveMonitorData2(dbconn, id, mon_rep_mag, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("Mag-obs %s\n", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorWk(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor weekly data.....................");

    if (dbconn)
    {
        CFDB_SaveMonitorData2(dbconn, id, mon_rep_week, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("Week-obs %s\n", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMonitorYr(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Monitor year data.....................");

    if (dbconn)
    {
        CFDB_SaveMonitorData2(dbconn, id, mon_rep_yr, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("Year-obs %s\n", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackCompliance(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Promise Compliance data..............");

    if (dbconn)
    {
        CFDB_SavePromiseCompliance(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            time_t then;
            double av, dev;
            char type,
                 eventname[CF_MAXVARSIZE] = "\0";

            sscanf(ip->name, "%ld,%255[^,],%c,%lf,%lf\n", &then, eventname, &type, &av, &dev);

            switch (type)
            {
            case 'c':
                CfDebug("Promise \"%s\" was compliant, av %.2lf +/- %.2lf at %s\n", eventname, av, dev, cf_ctime(&then));
                break;
            case 'r':
                CfDebug("Promise \"%s\" was repaired, av %.2lf +/- %.2lf at %s\n", eventname, av, dev, cf_ctime(&then));
                break;
            case 'n':
                CfDebug("Promise \"%s\" was non-compliant, av %.2lf +/- %.2lf at %s\n", eventname, av, dev,
                        cf_ctime(&then));
                break;
            default:
                CfDebug("Unknown state '%c' (eventname=%s,%.2lf +/- %.2lf at %s)\n", type, eventname, av, dev,
                        cf_ctime(&then));
            }
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackSoftware(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Installed software data...............");

    if (dbconn)
    {
        CFDB_SaveSoftware(dbconn, sw_rep_installed, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char name[CF_MAXVARSIZE] = "\0",
                 version[CF_MAXVARSIZE] = "\0",
                 arch;

            sscanf(ip->name, "%250[^,],%250[^,],%c", name, version, &arch);

            // architcure coding, see Nova_ShortArch

            CfDebug("Installed software: %s version (%s on %c)\n", name, version, arch);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackAvailPatches(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Available patch data...................");

    if (dbconn)
    {
        CFDB_SaveSoftware(dbconn, sw_rep_patch_avail, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char arch;
            char name[CF_MAXVARSIZE] = "\0",
                    version[CF_MAXVARSIZE] = "\0";

            sscanf(ip->name, "%250[^,],%250[^,],%c", name, version, &arch);

            // architcure coding, see Nova_ShortArch

            CfDebug("Patch available: %s version (%s on %c)\n", name, version, arch);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackPatchStatus(EnterpriseDB *dbconn, char *id, Item *data)
{    
    CfOut(cf_verbose, "", " -> Patches installed data.......................");

    if (dbconn)
    {
        CFDB_SaveSoftware(dbconn, sw_rep_patch_installed, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char arch;
            char name[CF_MAXVARSIZE] = "\0",
                 version[CF_MAXVARSIZE] = "\0";

            sscanf(ip->name, "%250[^,],%250[^,],%c", name, version, &arch);

            // architcure coding, see Nova_ShortArch

            CfDebug("Patch applied: %s version (%s on %c)\n", name, version, arch);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPack_promise_output_common(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Expanded private promise data.............");

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            CfDebug("POLICY: %s", ip->name);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackValueReport(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Value data..............................");

    if (dbconn)
    {
        CFDB_SaveValueReport(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            double kept, notkept, repaired;
            char date[CF_SMALLBUF] = "\0";

            sscanf(ip->name, "%[^,],%lf,%lf,%lf\n", date, &kept, &repaired, &notkept);

            CfDebug("Business value: (%.0lf,%.0lf,%.0lf) from %s\n", kept, repaired, notkept, date);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackVariables(EnterpriseDB *dbconn, char *id, Item *data)
/* Should be deprecated some time - was replaced after Nova 2.0.4 */
{
    CfOut(cf_verbose, "", " -> Variable data...........................");
    CfOut(cf_inform, "",
          "!! Deprecated variable data format - response from Nova 2.0.4 or earlier (some features unavailible)");

    if (dbconn)
    {
        CFDB_SaveVariables(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char type[CF_SMALLBUF] = "\0",
                    name[CF_MAXVARSIZE] = "\0",
                    value[CF_BUFSIZE] = "\0",
                    scope[CF_MAXVARSIZE] = "\0";

            if (strncmp(ip->name, "S: ", 3) == 0)
            {
                sscanf(ip->name + 3, "%254[^\n]", scope);
                CfDebug("SCOPE: %s\n", scope);
                continue;
            }

            sscanf(ip->name, "%4[^,], %255[^,], %2040[^\n]", type, name, value);

            CfDebug("line is \"%s\"\n", ip->name);
            CfDebug("var: (%s) \"%s\"=\"%s\"\n", type, name, value);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackVariables2(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Variable data with date stamp...........");

    if (dbconn)
    {
        CFDB_SaveVariables2(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char type[CF_SMALLBUF] = "\0",
                    name[CF_MAXVARSIZE] ="\0",
                    value[CF_BUFSIZE] = "\0",
                    scope[CF_MAXVARSIZE] = "\0";

            if (strncmp(ip->name, "S:", 2) == 0)
            {
                sscanf(ip->name + 2, "%254[^\n]", scope);
                CfDebug("SCOPE: %s\n", scope);
                continue;
            }

            time_t t;
            sscanf(ip->name, "%4[^,],%ld,%255[^,],%2040[^\n]", type, &t, name, value);

            CfDebug("var: (%s) at %s \"%s\"=\"%s\"\n", type, cf_ctime(&t), name, value);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackLastSeen(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Last-seen data..........................");

    if (dbconn)
    {
        CFDB_SaveLastSeen(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char inout;
            char asserted[CF_MAXVARSIZE] = "\0",
                    dns[CF_MAXVARSIZE] = "\0",
                    hash[CF_MAXVARSIZE] = "\0";
            double ago, average, dev;
            long fthen;

            sscanf(ip->name, "%c %128s %25s %15s %ld %lf %lf %lf\n",
                   &inout, hash, asserted, dns, &fthen, &ago, &average, &dev);

            // map our own address
            if (IsInterfaceAddress(asserted))
            {
                snprintf(asserted, sizeof(asserted), "%s", VFQNAME);
            }

            CfDebug("Saw: %c%s (alias %s/%s) seen %.2lf hrs ago, av %.2lf +/- %.2lf at %s", inout, hash, asserted, dns, ago,
                    average, dev, cf_ctime(&fthen));
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackTotalCompliance(EnterpriseDB *dbconn, char *id, Item *data)
{
    Item *ip;
    char version[CF_SMALLBUF];
    long date;
    time_t then;

    CfOut(cf_verbose, "", " -> Total Compliance data......................");

    if (dbconn)
    {
        CFDB_SaveTotalCompliance(dbconn, id, data);
    }

    time_t agent_last_run_time = 0;
    for (ip = data; ip != NULL; ip = ip->next)
    {
        int kept = 0,
            repaired = 0,
            notrepaired = 0;
        int kept_user = -1,
            repaired_user = -1,
            notrepaired_user = -1;
        int kept_internal = -1,
            repaired_internal = -1,
            notrepaired_internal = -1;

        sscanf(ip->name, "%ld,%127[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &date, version,
               &kept, &repaired, &notrepaired,
               &kept_user, &repaired_user, &notrepaired_user,
               &kept_internal, &repaired_internal, &notrepaired_internal);
        then = (time_t) date;

        if ((agent_last_run_time < (time_t)date) && (strstr(version, "Promises.cf") != NULL))
        {
            agent_last_run_time = (time_t)date;
        }

        if (kept_user == -1)
        {
            CfDebug("Tcompliance: (%d,%d,%d) for version %s at %ld\n", kept, repaired, notrepaired, version, then);
        }
        else
        {
        CfDebug("Tcompliance: %d,%d,%d (including: per user: %d,%d,%d per cfe internal: %d,%d,%d) for version %s at %ld\n",
                kept, repaired, notrepaired,
                kept_user, repaired_user, notrepaired_user,
                kept_internal, repaired_internal, notrepaired_internal,
                version, then);
        }
    }

    /* nova agent < 2.2- black status estimation */
    if (dbconn)
    {
        /* due to not beeing able to estimate real scheduling interval it is set on 0 */
        CFDB_SaveDeltaAgentExecution(dbconn, id, (long)0);

        if (agent_last_run_time != 0)
        {
            CFDB_SaveLastAgentExecution(dbconn, id, (long)agent_last_run_time);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackRepairLog(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Repair log data........................");

    if (dbconn)
    {
        CFDB_SavePromiseLog(dbconn, id, PROMISE_LOG_STATE_REPAIRED, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char handle[CF_MAXVARSIZE] = "\0";
            long then;

            sscanf(ip->name, "%ld,%127[^\n]", &then, handle);
            time_t tthen = (time_t) then;

            CfDebug("Repair: of promise \"%s\" at %lu\n", handle, tthen);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackNotKeptLog(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Not kept data...........................");

    if (dbconn)
    {
        CFDB_SavePromiseLog(dbconn, id, PROMISE_LOG_STATE_NOTKEPT, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char handle[CF_MAXVARSIZE] = "\0";
            long then;

            sscanf(ip->name, "%ld,%127[^\n]", &then, handle);
            time_t tthen = (time_t) then;
            CfDebug("Failure: of promise \"%s\" at %lu\n", handle, tthen);
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackMeter(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Meter data...........................");

    if (dbconn)
    {
        CFDB_SaveMeter(dbconn, id, data);
        CFDB_SaveScore(dbconn, id, data, HOST_RANK_METHOD_COMPLIANCE);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char type;
            double kept, repaired;

            sscanf(ip->name, "%c: %lf %lf", &type, &kept, &repaired);

            switch (type)
            {
            case cfmeter_week:
                CfDebug("Meter week compliance: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_week_user:
                CfDebug("Meter week compliance by user promises: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_week_internal:
                CfDebug("Meter week compliance by CFE internal promises: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_day:
                CfDebug("Meter daily compliance: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_day_user:
                CfDebug("Meter daily compliance by user promises: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_day_internal:
                CfDebug("Meter daily compliance by CFE internal promises: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_hour:
                CfDebug("Meter hourly compliance: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_hour_user:
                CfDebug("Meter hourly compliance by user: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_hour_internal:
                CfDebug("Meter hourly compliance by CFE internal promises: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_perf:
                CfDebug("Meter performance: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_other:
                CfDebug("Meter licenses: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_comms:
                CfDebug("Meter comms: %lf, %lf\n", kept, repaired);
                break;
            case cfmeter_anomaly:
                CfDebug("Meter anomalies: %lf, %lf\n", kept, repaired);
                break;
            }
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackSoftwareDates(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Software dates data...........................");

    if (dbconn)
    {
        CFDB_SaveSoftwareDates(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char type;
            time_t t;

            sscanf(ip->name, "%c:%ld", &type, &t);

            switch (type)
            {
            case 'S':
                CfDebug("Software installed date: %ld", t);
                break;
            }
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackBundles(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Bundle data...........................");

    if (dbconn)
    {
        CFDB_SaveBundles(dbconn, id, data);
    }

    if(DEBUG)
    {
        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            char bundle_name[CF_SMALLBUF] = { 0 }, bundle_namespace[CF_SMALLBUF] = { 0 };
            double compliance = 0.0, average = 0.0, dev = 0.0;
            long fthen = 0;

            sscanf(ip->name, "%127s %ld %lf %lf %lf %127s\n", bundle_name, &fthen, &compliance, &average, &dev, bundle_namespace);

            CfDebug("Bundle: $%s.%s done %.2lf hrs ago, av %.2lf +/- %.2lf at %s",
                    !NULL_OR_EMPTY(bundle_namespace) ? bundle_namespace : "default",
                    bundle_name, compliance, average, dev, cf_ctime(&fthen));
        }
    }
}

/*****************************************************************************/

static void Nova_UnPackExecutionStatus(EnterpriseDB *dbconn, char *id, Item *data)
{
    CfOut(cf_verbose, "", " -> Execution status...........................");

    char is_blackhost = 'f';
    long delta_schedule = 0;
    sscanf(data->name, "%c %ld\n", &is_blackhost, &delta_schedule);

    if (dbconn)
    {
        CFDB_SaveDeltaAgentExecution(dbconn, id, delta_schedule);
        CFDB_SaveExecutionStatus(dbconn, id,
                                 (is_blackhost == 't')? true:false);
    }

    CfDebug("Execution status: black %s with agent schedule interval: %ld",
            (is_blackhost == 't')? "true" : "false", delta_schedule);
}


void *CF_CODEBOOK_HANDLER[CF_CODEBOOK_SIZE] =
{
    Nova_UnPackPerformance,     // DBOK
    Nova_UnPackClasses,         // DBOK (nopurge)
    Nova_UnPackSetuid,          // DBOK (nopurge)
    Nova_UnPackFileChangesOld,     // DBOK
    Nova_UnPackFileChanges,     // DBOK
    Nova_UnPackDiffs,           // DBOK
    Nova_UnPackMonitorMag,      // DBOK   - DEPRECATED
    Nova_UnPackMonitorWeek,     // DBOK   - DEPRECATED
    Nova_UnPackMonitorYear,     // nodata - DEPRECATED
    Nova_UnPackMonitorHist,     // DEPRECATED
    Nova_UnPackMonitorMg,
    Nova_UnPackMonitorWk,
    Nova_UnPackMonitorYr,
    Nova_UnPackMonitorHg,       // DBOK
    Nova_UnPackCompliance,      // DBOK
    Nova_UnPackTotalCompliance, // DBOK
    Nova_UnPackSoftware,        // DBOK
    Nova_UnPackAvailPatches,    // nodata
    Nova_UnPackPatchStatus,     // nodata
    Nova_UnPack_promise_output_common,  // nodata
    Nova_UnPackValueReport,     //  (append) LOG
    Nova_UnPackVariables2,      // DBOK
    Nova_UnPackVariables,       // DBOK  - DEPRECATED
    Nova_UnPackLastSeen,        // DBOK
    Nova_UnPackRepairLog,       // DBOK (nopurge)
    Nova_UnPackNotKeptLog,      // DBOK (nopurge)
    Nova_UnPackMeter,           // DBOK
    Nova_UnPackBundles,
    Nova_UnPackSoftwareDates,
    Nova_UnPackExecutionStatus
};

void UnpackReportBook(EnterpriseDB *dbconn, char *id, Item **reports)
{
    int i;

    for (i = 0; i < CF_CODEBOOK_SIZE; i++)
    {
        if (reports[i] != NULL)
        {
            void (*fnptr) () = CF_CODEBOOK_HANDLER[i];

            (*fnptr) (dbconn, id, reports[i]);
        }
    }

}

Item **NewReportBook(void)
{
    return xcalloc(CF_CODEBOOK_SIZE, sizeof(Item *));
}

bool ReportBookHasData(Item **reports)
{
    for (int i = 0; i < CF_CODEBOOK_SIZE; i++)
    {
        if (reports[i] != NULL)
        {
            return true;
        }
    }

    return false;
}

void DeleteReportBook(Item **reports)
{
    for (int i = 0; i < CF_CODEBOOK_SIZE; ++i)
    {
        DeleteItemList(reports[i]);
    }
    free(reports);
}

int Nova_StoreIncomingReports(char *reply, Item **reports, int current_report)
{
    int report;

    for (report = 0; report < CF_CODEBOOK_SIZE; report++)
    {
        if (strcmp(reply, CF_CODEBOOK[report]) == 0)
        {
            CfOut(cf_verbose, "", " -> New chapter ... %s", reply);
            current_report = report;
            return report;
        }
    }

    if (current_report < 0)
    {
        CfOut(cf_error, "", " !! Report format is corrupted. Got \"%s\" before codebook", reply);
        return -1;
    }

    AppendItem(&(reports[current_report]), reply, NULL);
    return current_report;
}

