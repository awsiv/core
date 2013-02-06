
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "sysinfo.h"
#include "env_context.h"
#include "constraints.h"
#include "promises.h"
#include "probes.h"
#include "files_lib.h"
#include "files_names.h"
#include "files_interfaces.h"
#include "vars.h"
#include "item_lib.h"
#include "conversion.h"
#include "reporting.h"
#include "scope.h"
#include "matching.h"
#include "instrumentation.h"
#include "cfstream.h"
#include "pipes.h"
#include "transaction.h"
#include "logging.h"
#include "string_lib.h"
#include "exec_tools.h"
#include "unix.h"

typedef struct
{
    char *name;
    char *description;
    char *units;
    double expected_minimum;
    double expected_maximum;
    bool consolidable;
} MonitoringSlot;

/* Constants */

static const char *UNITS[CF_OBSERVABLES] =
{
    "average users per 2.5 mins",
    "processes",
    "processes",
    "percent",
    "jobs",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "packets",
    "entries",
    "entries",
    "entries",
    "entries",
    "Celcius",
    "Celcius",
    "Celcius",
    "Celcius",
    "percent",
    "percent",
    "percent",
    "percent",
    "percent",
    "packets",
    "packets",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",
    "connections",

};

static time_t slots_load_time = 0;
static MonitoringSlot *SLOTS[CF_OBSERVABLES - ob_spare];

/*****************************************************************************/

static void Nova_FreeSlot(MonitoringSlot *slot)
{
    if (slot)
    {
        free(slot->name);
        free(slot->description);
        free(slot->units);
        free(slot);
    }
}

static MonitoringSlot *Nova_MakeSlot(const char *name, const char *description,
                                     const char *units,
                                     double expected_minimum, double expected_maximum, bool consolidable)
{
    MonitoringSlot *slot = xmalloc(sizeof(MonitoringSlot));

    slot->name = xstrdup(name);
    slot->description = xstrdup(description);
    slot->units = xstrdup(units);
    slot->expected_minimum = expected_minimum;
    slot->expected_maximum = expected_maximum;
    slot->consolidable = consolidable;
    return slot;
}

static void Nova_LoadSlots(void)
{
    FILE *f;
    char filename[CF_BUFSIZE];
    int i;

    snprintf(filename, CF_BUFSIZE - 1, "%s%cstate%cts_key", CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);

    struct stat sb;

    if(cfstat(filename, &sb) != 0)
    {
        return;
    }

    if(sb.st_mtime <= slots_load_time)
    {
        return;
    }

    slots_load_time = sb.st_mtime;

    if ((f = fopen(filename, "r")) == NULL)
    {
        return;
    }

    for (i = 0; i < CF_OBSERVABLES; ++i)
    {
        if (i < ob_spare)
        {
            fscanf(f, "%*[^\n]\n");
        }
        else
        {
            char line[CF_MAXVARSIZE];

            char name[CF_MAXVARSIZE], desc[CF_MAXVARSIZE];
            char units[CF_MAXVARSIZE] = "unknown";
            double expected_min = 0.0;
            double expected_max = 100.0;
            int consolidable = true;

            if (fgets(line, CF_MAXVARSIZE, f) == NULL)
            {
                CfOut(cf_error, "fgets", "Error trying to read ts_key");
                continue;
            }

            int fields = sscanf(line, "%*d,%1023[^,],%1023[^,],%1023[^,],%lf,%lf,%d",
                                name, desc, units, &expected_min, &expected_max, &consolidable);

            if (fields == 2)
            {
                /* Old-style ts_key with name and description */
            }
            else if (fields == 6)
            {
                /* New-style ts_key with additional parameters */
            }
            else
            {
                CfOut(cf_error, "", "Wrong line format in ts_key: %s", line);
            }

            if (strcmp(name, "spare") != 0)
            {
                Nova_FreeSlot(SLOTS[i - ob_spare]);
                SLOTS[i - ob_spare] = Nova_MakeSlot(name, desc, units, expected_min, expected_max, consolidable);
            }
        }
    }
    fclose(f);
}

/*****************************************************************************/

static void Nova_DumpSlots(void)
{
#define MAX_KEY_FILE_SIZE 16384  /* usually around 4000, cannot grow much */

    char filename[CF_BUFSIZE];
    int i;

    snprintf(filename, CF_BUFSIZE - 1, "%s%cstate%cts_key", CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);

    char file_contents_new[MAX_KEY_FILE_SIZE] = {0};

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        char line[CF_MAXVARSIZE];

        if (NovaHasSlot(i))
        {
            snprintf(line, sizeof(line), "%d,%s,%s,%s,%.3lf,%.3lf,%d\n",
                    i,
                    NovaGetSlotName(i),
                    NovaGetSlotDescription(i),
                    NovaGetSlotUnits(i),
                    NovaGetSlotExpectedMinimum(i), NovaGetSlotExpectedMaximum(i), NovaIsSlotConsolidable(i) ? 1 : 0);
        }
        else
        {
            snprintf(line, sizeof(line), "%d,spare,unused\n", i);
        }

        strlcat(file_contents_new, line, sizeof(file_contents_new));
    }

    char *file_contents_existing = NULL;
    bool contents_changed = true;

    if(FileReadMax(&file_contents_existing, filename, MAX_KEY_FILE_SIZE) > 0)
    {
        if(strcmp(file_contents_existing, file_contents_new) == 0)
        {
            contents_changed = false;
        }

        free(file_contents_existing);
    }

    if(contents_changed)
    {
        CfOut(cf_verbose, "", "Updating %s with new slot information", filename);

        if(!FileWriteOver(filename, file_contents_new))
        {
            CfOut(cf_error, "FileWriteOver", "Nova_DumpSlots: Could not write file %s", filename);
        }
    }

    chmod(filename, 0600);
}

/*****************************************************************************/

void LookupObservable(int n, char *name, char *desc)
{
    Nova_LoadSlots();
    GetObservable(n, name, desc);
}

/*****************************************************************************/

void Nova_LookupAggregateClassName(int n, char *name, char *desc)
{
    Nova_LoadSlots();
    GetObservable(n, name, desc);
}

/*****************************************************************************/

void GetObservable(int i, char *name, char *desc)
{
    Nova_LoadSlots();

    if (i < ob_spare)
    {
        strncpy(name, OBS[i][0], CF_MAXVARSIZE - 1);
        strncpy(desc, OBS[i][1], CF_MAXVARSIZE - 1);
    }
    else
    {
        if (SLOTS[i - ob_spare])
        {
            strncpy(name, SLOTS[i - ob_spare]->name, CF_MAXVARSIZE - 1);
            strncpy(desc, SLOTS[i - ob_spare]->description, CF_MAXVARSIZE - 1);
        }
        else
        {
            strncpy(name, OBS[i][0], CF_MAXVARSIZE - 1);
            strncpy(desc, OBS[i][1], CF_MAXVARSIZE - 1);
        }
    }
}

/*****************************************************************************/

static void NovaOpenNewLifeCycle(int age, FILE **fp)
{
    int i;
    char filename[CF_MAXVARSIZE];

    CfDebug("OPEN YEAR %d\n", age);

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        sprintf(filename, "%s_%d.yr", OBS[i][0], age);

        if ((fp[i] = fopen(filename, "w")) == NULL)
        {
            CfOut(cf_error, "fopen", "Could not open %s\n", filename);
            exit(1);
        }
    }
}

/*****************************************************************************/

static void NovaCloseLifeCycle(int age, FILE **fp)
{
    int i;

    CfDebug("CLOSE YEAR %d\n", age);

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        fclose(fp[i]);
    }
}

/*****************************************************************************/

void LongHaul(time_t current)
{
    int y, i, j, k;
    CF_DB *dbp;

    if (LICENSES == 0)
    {
        return;
    }

    if (!OpenDB(&dbp, dbid_history))
    {
        return;
    }

    time_t now = CFSTARTTIME;
    time_t w = SubtractWeeks(WeekBegin(now), MONITORING_HISTORY_LENGTH_WEEKS - 1);

/* Graphs of the past MONITORING_HISTORY_LENGTH_YEARS */

    for (y = 0; y < MONITORING_HISTORY_LENGTH_YEARS; ++y)
    {
        FILE *fp[CF_OBSERVABLES];

        NovaOpenNewLifeCycle(MONITORING_HISTORY_LENGTH_YEARS - 1 - y, fp);

        for (i = 0; i < MONITORING_WEEKS_PER_YEAR; ++i)
        {
            for (j = 0; j < SHIFTS_PER_WEEK && w <= now; ++j, w = NextShift(w))
            {
                Averages av;

                if (GetRecordForTime(dbp, w, &av))
                {
                    for (k = 0; k < CF_OBSERVABLES; ++k)
                    {
                        double q = BoundedValue(av.Q[k].q, 0);
                        double var = BoundedValue(av.Q[k].var, q * q);
                        double expect = BoundedValue(av.Q[k].expect, q);

                        fprintf(fp[k], "%d %.2lf %.2lf %.2lf\n", j + SHIFTS_PER_WEEK * i, expect, sqrt(var), q);
                    }
                }
                else
                {
                    for (k = 0; k < CF_OBSERVABLES; ++k)
                    {
                        fprintf(fp[k], "%d 0.00 0.00 0.00\n", j + SHIFTS_PER_WEEK * i);
                    }
                }
            }
        }

        NovaCloseLifeCycle(MONITORING_WEEKS_PER_YEAR - 1 - y, fp);
    }

    CloseDB(dbp);
}

/*****************************************************************************/

void SetMeasurementPromises(Item **classlist)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char eventname[CF_MAXVARSIZE], assignment[CF_BUFSIZE];
    Event entry;
    char *key;
    void *stored;
    int ksize, vsize;

    if (!OpenDB(&dbp, dbid_measure))
    {
        return;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

    memset(&entry, 0, sizeof(entry));

    while (NextDB(dbp, dbcp, &key, &ksize, &stored, &vsize))
    {
        if (stored != NULL)
        {
            if (sizeof(entry) < vsize)
            {
                CfOut(cf_error, "", "Invalid entry in measurements database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
                continue;
            }

            strcpy(eventname, (char *) key);
            memcpy(&entry, stored, MIN(vsize, sizeof(entry)));

            CfOut(cf_verbose, "", " -> Setting measurement event %s\n", eventname);

            // a.measure.data_type is not longer known here, so look for zero decimals

            if ((int) (entry.Q.q * 10) % 10 == 0)
            {
                snprintf(assignment, CF_BUFSIZE - 1, "value_%s=%.0lf", eventname, entry.Q.q);
            }
            else
            {
                snprintf(assignment, CF_BUFSIZE - 1, "value_%s=%.2lf", eventname, entry.Q.q);
            }

            AppendItem(classlist, assignment, NULL);

            snprintf(assignment, CF_BUFSIZE - 1, "av_%s=%.2lf", eventname, entry.Q.expect);
            AppendItem(classlist, assignment, NULL);
            snprintf(assignment, CF_BUFSIZE - 1, "dev_%s=%.2lf", eventname, sqrt(entry.Q.var));
            AppendItem(classlist, assignment, NULL);
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/

void LoadSlowlyVaryingObservations()
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *stored;
    int ksize, vsize;

    if (!OpenDB(&dbp, dbid_static))
    {
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
        enum cfdatatype type;
        Rlist *list = NULL;

        strncpy(buf, key, CF_MAXVARSIZE - 1);

        int type_i;
        sscanf(buf, "%[^:]:%d", lval, &type_i);
        type = (enum cfdatatype)type_i;

        if (stored != NULL)
        {
            strncpy(rval, stored, CF_BUFSIZE - 1);

            switch (type)
            {
            case cf_str:
            case cf_int:
            case cf_real:
                NewScalar("mon", lval, rval, type);
                break;

            case cf_slist:
                list = SplitStringAsRList(rval, ',');
                NewList("mon", lval, list, cf_slist);
                DeleteRlist(list);
                break;

            case cf_counter:
                NewScalar("mon", lval, rval, cf_str);
                break;

            default:
                CfOut(cf_error, "", "Unexpected value type reading from database: %d\n", (int) type);
            }
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

/*****************************************************************************/
/* Clock handling                                                            */
/*****************************************************************************/

/* MB: We want to solve the geometric series problem to simulate an unbiased
   average over a grain size for long history aggregation at zero cost, i.e.
   we'd ideally like to have

  w = (1-w)^n for all n

  The true average is expensive to compute, so forget the brute force approach
  because this gives a pretty good result. The eqn above has no actual solution
  but we can approximate numerically to w = 0.01, see this test to show that
  the distribution is flat:

main ()

{ int i,j;
  double w = 0.01,wp;

for (i = 1; i < 20; i++)
   {
   printf("(");
   wp = w;

   for (j = 1; j < i; j++)
      {
      printf("%f,",wp);
      wp *= (1- w);
      }
   printf(")\n");
   }
}

*/

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static int NovaGetSlot(const char *name)
{
    int i;

    Nova_LoadSlots();

/* First try to find existing slot */
    for (i = 0; i < CF_OBSERVABLES - ob_spare; ++i)
    {
        if (SLOTS[i] && !strcmp(SLOTS[i]->name, name))
        {
            CfOut(cf_verbose, "", " -> Using slot ob_spare+%d (%d) for %s\n", i, i + ob_spare, name);
            return i + ob_spare;
        }
    }

/* Then find the spare one */
    for (i = 0; i < CF_OBSERVABLES - ob_spare; ++i)
    {
        if (!SLOTS[i])
        {
            CfOut(cf_verbose, "", " -> Using empty slot ob_spare+%d (%d) for %s\n", i, i + ob_spare, name);
            return i + ob_spare;
        }
    }

    CfOut(cf_error, "",
          "Measurement slots are all in use - it is not helpful to measure too much, you can't usefully follow this many variables");

    return -1;
}

/*****************************************************************************/

int NovaRegisterSlot(const char *name, const char *description,
                     const char *units, double expected_minimum, double expected_maximum, bool consolidable)
{
    int slot = NovaGetSlot(name);

    if (slot == -1)
    {
        return -1;
    }

    Nova_FreeSlot(SLOTS[slot - ob_spare]);
    SLOTS[slot - ob_spare] = Nova_MakeSlot(name, description, units, expected_minimum, expected_maximum, consolidable);
    Nova_DumpSlots();

    return slot;
}

bool NovaHasSlot(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare || SLOTS[idx - ob_spare];
}

const char *NovaGetSlotName(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? OBS[idx][0] : SLOTS[idx - ob_spare]->name;
}

const char *NovaGetSlotDescription(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? OBS[idx][1] : SLOTS[idx - ob_spare]->description;
}

const char *NovaGetSlotUnits(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? UNITS[idx] : SLOTS[idx - ob_spare]->units;
}

// TODO: real expected minimum/maximum/consolidable for core slots

double NovaGetSlotExpectedMinimum(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? 0.0f : SLOTS[idx - ob_spare]->expected_minimum;
}

double NovaGetSlotExpectedMaximum(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? 100.0f : SLOTS[idx - ob_spare]->expected_maximum;
}

bool NovaIsSlotConsolidable(int idx)
{
    Nova_LoadSlots();

    return idx < ob_spare ? true : SLOTS[idx - ob_spare]->consolidable;
}


void NovaNamedEvent(char *eventname, double value, Attributes a, Promise *pp)
{
    Event ev_new, ev_old;
    time_t now = time(NULL);
    CF_DB *dbp;

    if (!OpenDB(&dbp, dbid_measure))
    {
        return;
    }

    ev_new.t = now;

    if (ReadDB(dbp, eventname, &ev_old, sizeof(ev_old)))
    {
        if (isnan(ev_old.Q.expect))
        {
            ev_old.Q.expect = value;
        }

        if (isnan(ev_old.Q.var))
        {
            ev_old.Q.var = 0;
        }

        ev_new.Q = QAverage(ev_old.Q, value, 0.7);
    }
    else
    {
        ev_new.Q = QDefinite(value);
    }

    CfOut(cf_verbose, "", " -> Wrote scalar named event \"%s\" = (%.2lf,%.2lf,%.2lf)", eventname, ev_new.Q.q,
          ev_new.Q.expect, sqrt(ev_new.Q.var));
    WriteDB(dbp, eventname, &ev_new, sizeof(ev_new));

    CloseDB(dbp);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/


/****************************************************************************/

/*
 * This function returns beginning of last Monday relative to 'time'. If 'time'
 * is Monday, beginning of the same day is returned.
 */
time_t WeekBegin(time_t time)
{
    struct tm tm;

    gmtime_r(&time, &tm);

/* Move back in time to reach Monday. */

    time -= ((tm.tm_wday == 0 ? 6 : tm.tm_wday - 1) * SECONDS_PER_DAY);

/* Move to the beginning of day */

    time -= tm.tm_hour * SECONDS_PER_HOUR;
    time -= tm.tm_min * SECONDS_PER_MINUTE;
    time -= tm.tm_sec;

    return time;
}

/****************************************************************************/

time_t SubtractWeeks(time_t time, int weeks)
{
    return time - weeks * SECONDS_PER_WEEK;
}

/****************************************************************************/

time_t NextShift(time_t time)
{
    return time + SECONDS_PER_SHIFT;
}

/****************************************************************************/

void MakeTimekey(time_t time, char *result)
{
/* Generate timekey for database */
    struct tm tm;

    gmtime_r(&time, &tm);

    snprintf(result, 64, "%d_%.3s_Lcycle_%d_%s",
             tm.tm_mday, MONTH_TEXT[tm.tm_mon], (tm.tm_year + 1900) % 3, SHIFT_TEXT[tm.tm_hour / 6]);
}

/****************************************************************************/

/* Returns true if entry was found, false otherwise */
bool GetRecordForTime(CF_DB *db, time_t time, Averages *result)
{
    char timekey[CF_MAXVARSIZE];

    MakeTimekey(time, timekey);

    return ReadDB(db, timekey, result, sizeof(Averages));
}
