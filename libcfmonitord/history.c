/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "history.h"

#include "constraints.h"
#include "promises.h"
#include "reporting.h"
#include "transaction.h"
#include "policy.h"
#include "vars.h"
#include "sysinfo.h"
#include "unix.h"
#include "scope.h"
#include "env_context.h"
#include "cfstream.h"
#include "item_lib.h"
#include "exec_tools.h"
#include "conversion.h"
#include "instrumentation.h"
#include "files_interfaces.h"
#include "pipes.h"
#include "logging.h"
#include "matching.h"
#include "string_lib.h"

typedef struct
{
    char *path;
    Item *output;
} CustomMeasurement;

static int MONITOR_RESTARTED = true;
static CustomMeasurement NOVA_DATA[CF_DUNBAR_WORK];

static char CURRENT_SHIFT[CF_MAXVARSIZE];
static Averages SHIFT_VALUE;

static void PutRecordForTime(CF_DB *db, time_t time, const Averages *values)
{
    char timekey[CF_MAXVARSIZE];

    MakeTimekey(time, timekey);

    WriteDB(db, timekey, values, sizeof(Averages));
}

static void Nova_UpdateShiftAverage(Averages *shift_value, Averages *newvals)
{
    int i;

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        shift_value->Q[i] = QAverage(shift_value->Q[i], newvals->Q[i].q, 0.01);
    }
}

static void Nova_ResetShiftAverage(Averages *shift_value)
{
    int i;

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        shift_value->Q[i] = QDefinite(0.0);
    }
}

static void Nova_SaveFilePosition(char *name, long fileptr)
{
    CF_DB *dbp;

    if (!OpenDB(&dbp, dbid_static))
    {
        return;
    }

    CfOut(cf_verbose, "", "Saving state for %s at %ld\n", name, fileptr);
    WriteDB(dbp, name, &fileptr, sizeof(long));
    CloseDB(dbp);
}

static long Nova_RestoreFilePosition(char *name)
{
    CF_DB *dbp;
    long fileptr;

    if (!OpenDB(&dbp, dbid_static))
    {
        return 0L;
    }

    ReadDB(dbp, name, &fileptr, sizeof(long));
    CfOut(cf_verbose, "", "Resuming state for %s at %ld\n", name, fileptr);
    CloseDB(dbp);
    return fileptr;
}

static void Nova_DumpSlowlyVaryingObservations(void)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    FILE *fout;
    char *key;
    void *stored;
    int ksize, vsize;
    char name[CF_BUFSIZE];

    if (LICENSES == 0)
    {
        return;
    }

    if (!OpenDB(&dbp, dbid_static))
    {
        return;
    }

    snprintf(name, CF_BUFSIZE - 1, "%s%cstate%cstatic_data", CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Unable to save discovery data in %s\n", name);
        CloseDB(dbp);
        return;
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &key, &ksize, &stored, &vsize))
    {
        char buf[CF_MAXVARSIZE], lval[CF_MAXVARSIZE], rval[CF_BUFSIZE];

        strncpy(buf, key, CF_MAXVARSIZE - 1);

        sscanf(buf, "%s:", lval);

        if (stored != NULL)
        {
            strncpy(rval, stored, CF_BUFSIZE - 1);
            fprintf(fout, "%s:%s\n", lval, rval);
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
    fclose(fout);
}

static void Nova_HistoryUpdate(time_t time, const Averages *newvals)
{
    CF_DB *dbp;

    if (LICENSES == 0)
    {
        return;
    }

    if (!OpenDB(&dbp, dbid_history))
    {
        return;
    }

//CfOut(cf_verbose,"","Storing %s values\n",);

    PutRecordForTime(dbp, time, newvals);

    CloseDB(dbp);
}

static Item *NovaReSample(int slot, Attributes a, Promise *pp)
{
    CfLock thislock;
    char line[CF_BUFSIZE], eventname[CF_BUFSIZE];
    char comm[20];
    struct timespec start;
    FILE *fin = NULL;
    mode_t maskval = 0;
    Attributes at = { {0} };

    if (LICENSES == 0)
    {
        CfOut(cf_verbose, "", " !! No valid commercial license");
        return NULL;
    }

    if (a.measure.stream_type && strcmp(a.measure.stream_type, "pipe") == 0)
    {
        if (!IsExecutable(GetArg0(pp->promiser)))
        {
            cfPS(cf_error, CF_FAIL, "", pp, a, "%s promises to be executable but isn't\n", pp->promiser);
            return NULL;
        }
        else
        {
            CfOut(cf_verbose, "", " -> Promiser string contains a valid executable (%s) - ok\n", GetArg0(pp->promiser));
        }
    }

    if (MONITOR_RESTARTED)
    {
        // Force a measurement if restarted
        at = a;
        a.transaction.ifelapsed = 0;
    }
    else
    {
        at = a;
    }

    CFSTARTTIME = time(NULL);

    thislock = AcquireLock(pp->promiser, VUQNAME, CFSTARTTIME, at, pp, false);

    if (thislock.lock == NULL)
    {
        if (strcmp(a.measure.history_type, "log") == 0)
        {
            DeleteItemList(NOVA_DATA[slot].output);
            NOVA_DATA[slot].output = NULL;
        }
        else
        {
            /* If static or time-series, and too soon or busy then use a cached value
               to avoid artificial gaps in the history */
        }

        MONITOR_RESTARTED = false;
        return NOVA_DATA[slot].output;
    }
    else
    {
        DeleteItemList(NOVA_DATA[slot].output);
        NOVA_DATA[slot].output = NULL;

        CfOut(cf_inform, "", " -> Sampling \'%s\' ...(timeout=%d,owner=%ju,group=%ju)\n", pp->promiser, a.contain.timeout,
              (uintmax_t)a.contain.owner, (uintmax_t)a.contain.group);

        start = BeginMeasure();

        CommPrefix(pp->promiser, comm);

        if (a.contain.timeout != 0)
        {
            SetTimeOut(a.contain.timeout);
        }

        /* Stream types */

        if (a.measure.stream_type && strcmp(a.measure.stream_type, "file") == 0)
        {
            long filepos = 0;
            struct stat sb;

            CfOut(cf_verbose, "", " -> Stream \"%s\" is a plain file\n", pp->promiser);

            if (cfstat(pp->promiser, &sb) == -1)
            {
                CfOut(cf_inform, "stat", " !! Unable to find stream %s\n", pp->promiser);
                YieldCurrentLock(thislock);
                MONITOR_RESTARTED = false;

                return NULL;
            }

            fin = fopen(pp->promiser, "r");

            if (a.measure.growing)
            {
                filepos = Nova_RestoreFilePosition(pp->promiser);

                if (sb.st_size >= filepos)
                {
                    fseek(fin, filepos, SEEK_SET);
                }
            }
        }
        else if (a.measure.stream_type && strcmp(a.measure.stream_type, "pipe") == 0)
        {
            CfOut(cf_verbose, "", " -> (Setting pipe umask to %jo)\n", (uintmax_t)a.contain.umask);
            maskval = umask(a.contain.umask);

            if (a.contain.umask == 0)
            {
                CfOut(cf_verbose, "", " !! Programming %s running with umask 0! Use umask= to set\n", pp->promiser);
            }


            // Mark: This is strange that we used these wrappers. Currently no way of setting these
            a.contain.owner = -1;
            a.contain.group = -1;
            a.contain.chdir = NULL;
            a.contain.chroot = NULL;
            // Mark: they were unset, and would fail for non-root(!)
            
            if (a.contain.useshell)
            {
                fin =
                    cf_popen_shsetuid(pp->promiser, "r", a.contain.owner, a.contain.group, a.contain.chdir,
                                      a.contain.chroot, false);
            }
            else
            {
                fin =
                    cf_popensetuid(pp->promiser, "r", a.contain.owner, a.contain.group, a.contain.chdir,
                                   a.contain.chroot, false);
            }
        }

        /* generic file stream */

        if (fin == NULL)
        {
            cfPS(cf_error, CF_FAIL, "cf_popen", pp, a, "Couldn't open pipe to command %s\n", pp->promiser);
            YieldCurrentLock(thislock);
            MONITOR_RESTARTED = false;
            return NOVA_DATA[slot].output;
        }

        while (!feof(fin))
        {
            if (ferror(fin))    /* abortable */
            {
                cfPS(cf_error, CF_TIMEX, "ferror", pp, a, "Sample stream %s\n", pp->promiser);
                YieldCurrentLock(thislock);
                return NOVA_DATA[slot].output;
            }

            if (CfReadLine(line, CF_BUFSIZE - 1, fin) == -1)
            {
                FatalError("Error in CfReadLine");
            }
            AppendItem(&(NOVA_DATA[slot].output), line, NULL);
            CfOut(cf_inform, "", "Sampling => %s", line);

            if (ferror(fin))    /* abortable */
            {
                cfPS(cf_error, CF_TIMEX, "ferror", pp, a, "Sample stream %s\n", pp->promiser);
                YieldCurrentLock(thislock);
                return NOVA_DATA[slot].output;
            }
        }

        if (a.measure.stream_type && strcmp(a.measure.stream_type, "file") == 0)
        {
            long fileptr = ftell(fin);

            fclose(fin);
            Nova_SaveFilePosition(pp->promiser, fileptr);
        }
        else if (a.measure.stream_type && strcmp(a.measure.stream_type, "pipe") == 0)
        {
            cf_pclose_def(fin, a, pp);
        }
    }

    if (a.contain.timeout != 0)
    {
        alarm(0);
        signal(SIGALRM, SIG_DFL);
    }

    CfOut(cf_inform, "", " -> Collected sample of %s\n", pp->promiser);
    umask(maskval);
    YieldCurrentLock(thislock);
    MONITOR_RESTARTED = false;

    snprintf(eventname, CF_BUFSIZE - 1, "Sample(%s)", pp->promiser);
    EndMeasure(eventname, start);
    return NOVA_DATA[slot].output;
}

void HistoryUpdate(Averages newvals)
{
    Promise *pp = NewPromise("history_db", "the long term memory");
    Attributes dummyattr = { {0} };
    CfLock thislock;
    time_t now = time(NULL);

/* We do this only once per hour - this should not be changed */

    Banner("Update long-term history");

    if (strlen(CURRENT_SHIFT) == 0)
    {
        // initialize
        Nova_ResetShiftAverage(&SHIFT_VALUE);
    }

    memset(&dummyattr, 0, sizeof(dummyattr));
    dummyattr.transaction.ifelapsed = 59;

    thislock = AcquireLock(pp->promiser, VUQNAME, now, dummyattr, pp, false);

    if (thislock.lock == NULL)
    {
        Nova_UpdateShiftAverage(&SHIFT_VALUE, &newvals);
        PromiseDestroy(pp);
        return;
    }

/* Refresh the class context of the agent */
    DeletePrivateClassContext();
    DeleteEntireHeap();
    DeleteAllScope();

    NewScope("sys");
    NewScope("const");
    NewScope("match");
    NewScope("mon");
    NewScope("control_monitor");
    NewScope("control_common");
    GetNameInfo3();
    GetInterfacesInfo(AGENT_TYPE_MONITOR);
    Get3Environment();
    BuiltinClasses();
    OSClasses();
    SetReferenceTime(true);

    LoadPersistentContext();
    LoadSystemConstants();

    YieldCurrentLock(thislock);
    PromiseDestroy(pp);

    Nova_HistoryUpdate(CFSTARTTIME, &newvals);

    if (strcmp(CURRENT_SHIFT, VSHIFT) != 0)
    {
        strcpy(CURRENT_SHIFT, VSHIFT);
        Nova_ResetShiftAverage(&SHIFT_VALUE);
    }

    Nova_DumpSlowlyVaryingObservations();
}

static Item *NovaGetMeasurementStream(Attributes a, Promise *pp)
{
    int i;

    for (i = 0; i < CF_DUNBAR_WORK; i++)
    {
        if (NOVA_DATA[i].path == NULL)
        {
            break;
        }

        if (strcmp(NOVA_DATA[i].path, pp->promiser) == 0)
        {
            NOVA_DATA[i].output = NovaReSample(i, a, pp);
            return NOVA_DATA[i].output;
        }
    }

    NOVA_DATA[i].path = xstrdup(pp->promiser);
    NOVA_DATA[i].output = NovaReSample(i, a, pp);
    return NOVA_DATA[i].output;
}

static double NovaExtractValueFromStream(char *handle, Item *stream, Attributes a, Promise *pp)
{
    char value[CF_MAXVARSIZE];
    int count = 1, found = false, match_count = 0, done = false;
    double real_val = 0;
    Item *ip, *match = NULL;

    for (ip = stream; ip != NULL; ip = ip->next)
    {
        if (count == a.measure.select_line_number)
        {
            found = true;
            match = ip;
            match_count++;
        }

        if (a.measure.select_line_matching && FullTextMatch(a.measure.select_line_matching, ip->name))
        {
            CfOut(cf_verbose, "", " ?? Look for %s regex %s\n", handle, a.measure.select_line_matching);
            found = true;
            match = ip;

            if (a.measure.extraction_regex)
            {
                switch (a.measure.data_type)
                {
                case cf_int:
                case cf_real:

                    strncpy(value, ExtractFirstReference(a.measure.extraction_regex, match->name), CF_MAXVARSIZE - 1);

                    if (strcmp(value, "CF_NOMATCH") == 0)
                    {
                        real_val = CF_NODOUBLE;
                        CfOut(cf_verbose,""," ! Was not able to match a value with %s on %s",a.measure.extraction_regex, match->name);
                    }
                    else
                    {
                        if (real_val != CF_NODOUBLE)
                        {
                            CfOut(cf_verbose,""," -> Found candidate match value of %s",value);
                            
                            if (a.measure.policy == cfm_sum || a.measure.policy == cfm_average)
                            {
                                real_val += Str2Double(value);
                            }
                            else
                            {
                                real_val = Str2Double(value);
                            }
                            
                            match_count++;

                            if (a.measure.policy == cfm_first)
                            {
                                done = true;
                            }
                        }
                    }
                    break;

                default:
                    CfOut(cf_error, "", "Unexpected data type in data_type attribute: %d\n", a.measure.data_type);
                }
            }

        }

        count++;
        
        if (done)
        {
            break;
        }
    }

    if (!found)
    {
        cfPS(cf_inform, CF_FAIL, "", pp, a, "Could not locate the line for promise \"%s\"", handle);
        return 0.0;
    }

    switch (a.measure.data_type)
    {
    case cf_counter:

        real_val = (double) match_count;
        break;

    case cf_int:

        if (match_count > 1)
        {
            CfOut(cf_inform, "", " !! Warning: %d lines matched the line_selection \"%s\"- making best average",
                  match_count, a.measure.select_line_matching);
        }

        if (match_count > 0 && a.measure.policy == cfm_average) // If not "average" then "sum"
        {
            real_val /= match_count;
        }
        break;

    case cf_real:

        if (match_count > 1)
        {
            CfOut(cf_inform, "", " !! Warning: %d lines matched the line_selection \"%s\"- making best average",
                  match_count, a.measure.select_line_matching);
        }

        if (match_count > 0)
        {
            real_val /= match_count;
        }

        break;

    default:
        CfOut(cf_error, "", "Unexpected data type in data_type attribute: %d", a.measure.data_type);
    }

    if (real_val == CF_NODOUBLE)
    {
        cfPS(cf_inform, CF_FAIL, "", pp, a, " !! Unable to extract a value from the matched line \"%s\"", match->name);
        PromiseRef(cf_inform, pp);
        real_val = 0.0;
    }

    CfOut(cf_inform, "", "Extracted value \"%f\" for promise \"%s\"", real_val, handle);
    return real_val;
}

static void NovaLogSymbolicValue(char *handle, Item *stream, Attributes a, Promise *pp)
{
    char value[CF_BUFSIZE], sdate[CF_MAXVARSIZE], filename[CF_BUFSIZE], *v;
    int count = 1, found = false, match_count = 0;
    Item *ip, *match = NULL, *matches = NULL;
    time_t now = time(NULL);
    FILE *fout;

    if (stream == NULL)
    {
        CfOut(cf_verbose, "", " -> No stream to measure");
        return;
    }

    if (LICENSES == 0)
    {
        CfOut(cf_verbose, "", " !! No valid commercial license");
        return;
    }

    CfOut(cf_verbose, "", " -> Locate and log sample ...");

    for (ip = stream; ip != NULL; ip = ip->next)
    {
        if (ip->name == NULL)
        {
            continue;
        }

        if (count == a.measure.select_line_number)
        {
            CfOut(cf_verbose, "", "Found line %d by number...\n", count);
            found = true;
            match_count = 1;
            match = ip;

            if (a.measure.extraction_regex)
            {
                CfOut(cf_verbose, "", " -> Now looking for a matching extractor \"%s\"", a.measure.extraction_regex);
                strncpy(value, ExtractFirstReference(a.measure.extraction_regex, match->name), CF_MAXVARSIZE - 1);
                CfOut(cf_inform, "", "Extracted value \"%s\" for promise \"%s\"", value, handle);
                AppendItem(&matches, value, NULL);
                
            }
            break;
        }

        if (a.measure.select_line_matching && FullTextMatch(a.measure.select_line_matching, ip->name))
        {
            CfOut(cf_verbose, "", "Found line %d by pattern...\n", count);
            found = true;
            match = ip;
            match_count++;

            if (a.measure.extraction_regex)
            {
                CfOut(cf_verbose, "", " -> Now looking for a matching extractor \"%s\"", a.measure.extraction_regex);
                strncpy(value, ExtractFirstReference(a.measure.extraction_regex, match->name), CF_MAXVARSIZE - 1);
                CfOut(cf_inform, "", "Extracted value \"%s\" for promise \"%s\"", value, handle);
                AppendItem(&matches, value, NULL);
            }
        }

        count++;
    }

    if (!found)
    {
        cfPS(cf_inform, CF_FAIL, "", pp, a, "Promiser \"%s\" found no matching line", pp->promiser);
        return;
    }

    if (match_count > 1)
    {
        CfOut(cf_inform, "", " !! Warning: %d lines matched the line_selection \"%s\"- matching to last", match_count,
              a.measure.select_line_matching);
    }

    switch (a.measure.data_type)
    {
    case cf_counter:
        CfOut(cf_verbose, "", " -> Counted %d for %s\n", match_count, handle);
        snprintf(value, CF_MAXVARSIZE, "%d", match_count);
        break;

    case cf_slist:
        v = ItemList2CSV(matches);
        snprintf(value, CF_BUFSIZE, "%s", v);
        free(v);
        break;

    default:
        snprintf(value, CF_BUFSIZE, "%s", matches->name);
    }

    DeleteItemList(matches);

    if (a.measure.history_type && strcmp(a.measure.history_type, "log") == 0)
    {
        snprintf(filename, CF_BUFSIZE, "%s%cstate%c%s_measure.log", CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR, handle);

        if ((fout = fopen(filename, "a")) == NULL)
        {
            cfPS(cf_error, CF_FAIL, "", pp, a, "Unable to open the output log \"%s\"", filename);
            PromiseRef(cf_error, pp);
            return;
        }

        strncpy(sdate, cf_ctime(&now), CF_MAXVARSIZE - 1);
        if (Chop(sdate, CF_EXPANDSIZE) == -1)
        {
            CfOut(cf_error, "", "Chop was called on a string that seemed to have no terminator");
        }

        fprintf(fout, "%s,%ld,%s\n", sdate, (long) now, value);
        CfOut(cf_verbose, "", "Logging: %s,%s to %s\n", sdate, value, filename);

        fclose(fout);
    }
    else                        // scalar or static
    {
        CF_DB *dbp;
        char id[CF_MAXVARSIZE];

        if (!OpenDB(&dbp, dbid_static))
        {
            return;
        }

        snprintf(id, CF_MAXVARSIZE - 1, "%s:%d", handle, a.measure.data_type);
        WriteDB(dbp, id, value, strlen(value) + 1);
        CloseDB(dbp);
    }
}

void VerifyMeasurement(double *this, Attributes a, Promise *pp)
{
    char *handle = (char *) GetConstraintValue("handle", pp, RVAL_TYPE_SCALAR);
    Item *stream = NULL;
    int slot = 0;
    double new_value;

    if (!handle)
    {
        CfOut(cf_error, "", " !! The promised measurement has no handle to register it by.");
        return;
    }
    else
    {
        CfOut(cf_verbose, "", " -> Considering promise \"%s\"", handle);
    }

    switch (a.measure.data_type)
    {
    case cf_counter:
    case cf_int:
    case cf_real:

        /* First see if we can accommodate this measurement */

        CfOut(cf_verbose, "", " -> Promise \"%s\" is numerical in nature", handle);

        stream = NovaGetMeasurementStream(a, pp);

        if (strcmp(a.measure.history_type, "weekly") == 0)
        {
            if ((slot = NovaRegisterSlot(handle, pp->ref ? pp->ref : "User defined measure",
                                         a.measure.units ? a.measure.units : "unknown", 0.0f, 100.0f, true)) < 0)
            {
                return;
            }

            this[slot] = NovaExtractValueFromStream(handle, stream, a, pp);
            CfOut(cf_verbose, "", " -> Setting Nova slot %d=%s to %lf\n", slot, handle, this[slot]);
        }
        else if (strcmp(a.measure.history_type, "log") == 0)
        {
            CfOut(cf_verbose, "", " -> Promise to log a numerical value");
            NovaLogSymbolicValue(handle, stream, a, pp);
        }
        else                    /* static */
        {
            CfOut(cf_verbose, "", " -> Promise to store a static numerical value");
            new_value = NovaExtractValueFromStream(handle, stream, a, pp);
            NovaNamedEvent(handle, new_value, a, pp);
        }
        break;

    default:

        CfOut(cf_verbose, "", " -> Promise \"%s\" is symbolic in nature", handle);
        stream = NovaGetMeasurementStream(a, pp);
        NovaLogSymbolicValue(handle, stream, a, pp);
        break;
    }

// stream gets de-allocated in ReSample
}
