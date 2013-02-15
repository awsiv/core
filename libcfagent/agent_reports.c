/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "agent_reports.h"

#include "assoc.h"
#include "cfstream.h"
#include "conversion.h"
#include "files_names.h"

void Nova_NoteVarUsageDB(void)
/* WARNING: Not thread safe (access to VSCOPE) */
{
    Scope *ptr;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char key[CF_MAXVARSIZE], *keyDb;    // scope.varname
    void *val;
    Variable var = { {0} };
    int keyDbSize, valSize;
    time_t varExpireAge = SECONDS_PER_WEEK * 4; // remove vars from DB after about one month
    time_t now = time(NULL);

    if (MINUSF)                 /* Only do this for the default policy */
    {
        return;
    }

    if (!OpenDB(&dbp, dbid_variables))
    {
        return;
    }

/* sync db with current vars */

// NOTE: can extend to support avg and stddev in future
    var.e.t = now;              // all are last seen now

    for (ptr = VSCOPE; ptr != NULL; ptr = ptr->next)
    {
        HashIterator i = HashIteratorInit(ptr->hashtable);
        CfAssoc *assoc;

        if (strcmp(ptr->scope, "this") == 0)
        {
            continue;
        }

        while ((assoc = HashIteratorNext(&i)))
        {
            snprintf(key, sizeof(key), "%s.%s", ptr->scope, assoc->lval);
            var.dtype = assoc->dtype;
            var.rtype = assoc->rval.type;
            var.rval[0] = '\0';

            if (!RvalPrint(var.rval, sizeof(var.rval) - 15, assoc->rval))
            {
                CfOut(cf_verbose, "",
                      "!! Variable %s is too large for transmission to reporting hub (larger than %zu bytes) -- will be truncated in reports",
                      key, sizeof(var.rval));
            }

            WriteDB(dbp, key, &var, VARSTRUCTUSAGE(var));
        }
    }

/* purge old entries from DB */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to purge variable db");
        CloseDB(dbp);
        return;
    }

    while (NextDB(dbp, dbcp, &keyDb, &keyDbSize, &val, &valSize))
    {
        if (val != NULL)
        {
            if (sizeof(Variable) < valSize)
            {
                CfOut(cf_error, "", "Invalid entry in variables database. Expected size: %zu, actual size: %d", sizeof(Variable), valSize);
                continue;
            }

            /* May not read from val directly due to unaligned access */
            Variable varDb;
            memcpy(&varDb, val, MIN(valSize, sizeof(Variable)));

            if (varDb.e.t < now - varExpireAge)
            {
                CfDebug("Variable record %s expired\n", keyDb);
                DBCursorDeleteEntry(dbcp);
            }
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);
}

void Nova_TrackExecution(const char *input_file)
{
    CF_DB *dbp = NULL;
    time_t now = time(NULL);
    time_t last_exec;
    double gavr = 0;
    double trust_level = 0.7; // sensivity of scheduling history -> higher more sensitive

    /* failsafe + promises double execution exeption */
    if (strstr(input_file, "failsafe.cf") != NULL)
    {
        CfDebug("TrackExecution: policy file %s SKIPPED\n", input_file);
        return;
    }

    if (strstr(input_file, "update.cf") != NULL)
    {
        CfDebug("TrackExecution: policy file %s SKIPPED\n", input_file);
        return;
    }

    /* get last run data */
    if (!OpenDB(&dbp, dbid_agent_execution))
    {
        CfOut(cf_inform, "", " !! Unable to open nova_agent_execution db");
        return;
    }

    if (!ReadDB(dbp, NOVA_TRACK_LAST_EXEC, &last_exec, sizeof(time_t)))
    {
        last_exec = now;
    }

    if (!ReadDB(dbp, NOVA_TRACK_DELTA_SCHEDULE, &gavr, sizeof(double)))
    {
        gavr = (double)(now - last_exec);
    }

    /* calculate avrage agent scheduling time */
    gavr = GAverage ((double)(now - last_exec), gavr, trust_level);

    /* save current run data */
    if (!WriteDB(dbp, NOVA_TRACK_LAST_EXEC, &now, sizeof(time_t)))
    {
        CfOut(cf_inform, "", " !! Unable to write to nova_agent_execution db");
    }
    if (!WriteDB(dbp, NOVA_TRACK_DELTA_SCHEDULE, &gavr, sizeof(double)))
    {
        CfOut(cf_inform, "", " !! Unable to write to nova_agent_execution db");
    }

    CloseDB(dbp);

    CfDebug("TrackExecution: policy file: %s, last_exec: %" PRIdMAX ", avr: %g PASSED\n", input_file, (intmax_t)last_exec, gavr);
}

void NoteEfficiency(double e)
{
    Attributes a = { {0} };
    Promise p = { 0 };

    NovaNamedEvent("configuration-model-efficiency", e, a, &p);
    CfOut(cf_verbose, "", " -> Configuration model efficiency for %s = %.2lf%%", VUQNAME, e);
}

void ReportPatches(PackageManager *list)
{
    FILE *fout;
    PackageManager *mp = NULL;
    PackageItem *pi;
    char name[CF_BUFSIZE];
    int count = 0;

    snprintf(name, CF_BUFSIZE, "%s/state/%s", CFWORKDIR, NOVA_PATCHES_INSTALLED);
    MapName(name);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    for (mp = list; mp != NULL; mp = mp->next)
    {
        for (pi = mp->patch_list; pi != NULL; pi = pi->next)
        {
            fprintf(fout, "%s,%s,%s,%s\n", pi->name, pi->version, pi->arch, ReadLastNode(GetArg0(mp->manager)));
        }
    }

    fclose(fout);

    snprintf(name, CF_BUFSIZE, "%s/state/%s", CFWORKDIR, NOVA_PATCHES_AVAIL);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    for (mp = list; mp != NULL; mp = mp->next)
    {
        for (pi = mp->patch_avail; pi != NULL; pi = pi->next)
        {
            count++;
            fprintf(fout, "%s,%s,%s,%s\n", pi->name, pi->version, pi->arch, ReadLastNode(GetArg0(mp->manager)));
        }
    }

    fclose(fout);
}
