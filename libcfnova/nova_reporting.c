
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "nova_reporting.h"

#include "cf3.extern.h"
#include "env_context.h"
#include "lastseen.h"
#include "files_names.h"
#include "files_interfaces.h"
#include "item_lib.h"
#include "sort.h"
#include "conversion.h"
#include "reporting.h"
#include "datapack.h"
#include "scope.h"
#include "assoc.h"
#include "cfstream.h"
#include "string_lib.h"
#include "communication.h"
#include "promises.h"
#include "logging.h"
#include "policy.h"
#include "generic_agent.h" // OpenCompilationReportFiles
#include "promise_db.h"

#ifdef HAVE_LIBMONGOC
#include "db_save.h"
#include "db_query.h"
#include "db_maintain.h"
#endif

ReportInfo BASIC_REPORTS[] =
{
    {CFRID_BUNDLE_PROFILE, "policy", "compliance by bundle", "bundle profile",
     "aggregate compliance levels of promise bundles"},
    {CFRID_PROMISE_COMPLIANCE, "policy", "compliance by promise", "compliance by promise",
     "compliance of each individual promise"},
    {CFRID_COMPLIANCE_SUMMARY, "accounting", "compliance summary", "compliance summary",
     "total summary of host compliance"},
    {CFRID_PROMISES_REPAIRED_LOG, "policy", "promises repaired (log)", "promises repaired log",
     "log of actual repairs made to the system"},
    {CFRID_PROMISES_REPAIRED_SUMMARY, "policy", "promises repaired (summary)", "promises repaired summary",
     "cumulative (histogram) summary of promises repaired"},
    {CFRID_PROMISES_NOTKEPT_LOG, "policy", "promises not kept (log)", "promises not kept log",
     "log of promises that could not or would not be kept"},
    {CFRID_PROMISES_NOTKEPT_SUMMARY, "policy", "promises not kept (summary)", "promises not kept summary",
     "cumulative (histogram) summary of promises not kept"},

    {CFRID_CONTEXTS, "system", "context classes", "class profile", "user defined classes observed on the system"},
    {CFRID_NEIGHBORS, "system", "last saw neighbours", "last seen hosts",
     "when host neighbours were last seen online"},
    {CFRID_WEAKEST_HOSTS, "system", "weakest hosts", "weakest hosts", "worst available hosts ranked by compliance level"},
    {CFRID_VALUES, "system", "variables", "variables", "table of variable values last observed"},    

    {CFRID_PATCHES_AVAILABLE, "software", "patches available", "patches available",
     "a list of patches currently claimed to be available by the local package manager"},
    {CFRID_PATCHES_INSTALLED, "software", "patches installed", "patch status",
     "a list of (un)applied patches according to the local package manager"},

    {CFRID_SOFTWARE_INSTALLED, "software", "installed", "software installed",
     "list of software packages claimed to be installed according to the local package manager"},

    {CFRID_FILECHANGE_LOG, "file watch", "change summary", "file change log",
     "Log of all detected changes to files from changes promises"},
    {CFRID_FILECHANGE_DIFFS, "file watch", "text changes", "file change diffs",
     "delta/difference comparison showing file changes"},
    {CFRID_SETUID_PROGRAMS, "file watch", "setuid", "setuid/gid root programs",
     "current list of observed setuid/setgid root programs"},

    {CFRID_BUSINESS_VALUE, "accounting", "business value", "business value report",
     "accumulated value of promises kept"},
    {CFRID_BENCHMARKS, "accounting", "benchmarks", "performance", "execution time used to verify selected promises"},
    {NULL, NULL, NULL, NULL, NULL}
};

/*****************************************************************************/


void Nova_SummarizeComms()
/* Go through the database of recent connections and check for
   Long Time No See ...*/
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    int ksize, vsize;
    KeyHostSeen entry;
    double kept = 1, not_kept = 0, repaired = 0, var, average;
    char *key;
    void *value;
    double now = (double) time(NULL), then, resolution = 300;

    if (!OpenDB(&dbp, dbid_lastseen))
    {
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        memset(&entry, 0, sizeof(entry));

        if (value != NULL)
        {
            memcpy(&entry, value, MIN(vsize, sizeof(entry)));

            then = entry.Q.q;
            average = (double) entry.Q.expect;
            var = (double) entry.Q.var;
        }
        else
        {
            continue;
        }

        if ((now - then > average + 2.0 * sqrt(var)) && (now - then > resolution * 2))
        {
            not_kept++;
        }
        else if ((now - then > average + sqrt(var)) && (now - then > resolution * 2))
        {
            repaired++;
        }
        else
        {
            kept++;
        }

        memset(&value, 0, sizeof(value));
        memset(&key, 0, sizeof(key));
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    METER_KEPT[meter_comms_hour] = 100.0 * kept / (kept + repaired + not_kept);
    METER_REPAIRED[meter_comms_hour] = 100.0 * repaired / (kept + repaired + not_kept);
}



ReportContext *Nova_OpenCompilationReportFiles(const char *fname)
{
#if defined(HAVE_LIBMONGOC)

    ReportContext *context = ReportContextNew();
    {
        FILE *ftxt = NULL;
        if ((ftxt = fopen(NULLFILE, "w")) == NULL)
        {
            FatalError("Could not write output log to %s", NULLFILE);
        }
        ReportContextAddWriter(context, REPORT_OUTPUT_TYPE_TEXT, FileWriter(ftxt));
    }
    return context;
#else
    return OpenCompilationReportFiles(fname);
#endif
}

/*****************************************************************************/

void Nova_ShowPromises(const ReportContext *context, ReportOutputType type, const Seq *bundles, const Seq *bodies)
{
#if defined(HAVE_LIBMONGOC)
    if (SHOWREPORTS)
    {
	CFDB_SaveUnExpandedPromises(bundles, bodies);
    }

#else
    ShowPromisesInReport(context, type, bundles, bodies);
#endif
}

/*****************************************************************************/

void Nova_ShowPromise(const ReportContext *context, ReportOutputType type, const char *version, const Promise *pp, int indent)
{
#if defined (HAVE_LIBMONGOC)
    if (SHOWREPORTS)
    {
        CFDB_SaveExpandedPromise(pp);
    }
#else
    ShowPromiseInReport(context, type, version, pp, indent);
#endif
}
