/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf.nova.h"
#include "hub.h"
#include "hub-maintenance.h"
#include "db_maintain.h"
#include "db_save.h"
#include "db_query.h"
#include "granules.h"

static void ScheduleRunMaintenanceJobs(void);

void Nova_Maintain(void)
{
    NewClass("am_policy_hub");
    ScheduleRunMaintenanceJobs();
}

static void ScheduleRunMaintenanceJobs(void)
{
    time_t now = time(NULL);

    CfOut(cf_verbose, "", " -> Scanning total compliance cache and doing db maintenance");
    Nova_CacheTotalCompliance(true);
    CFDB_Maintenance();

    Nova_HubLog("Last maintenance took %ld seconds", time(NULL) - now);
}

void Nova_CacheTotalCompliance(bool allSlots)
/*
 * Caches the current slot of total compliance.
 * WARNING: Must be run every 6 hrs (otherwise no data is show in the
 * graph slot).
 */
{
    time_t curr, now = time(NULL);
    mongo_connection dbconn;
    EnvironmentsList *env, *ep;
    int slot;
    char envName[CF_SMALLBUF];
    char envClass[CF_SMALLBUF];

// Query all hosts in one time slot
// ht->hh->hostname,ht->kept,ht->repaired,ht->notkept,ht->t;
// Divide each day into 4 lifecycle units 3600 * 24 / 4 seconds

    now = time(NULL);

    if (allSlots)
    {
        curr = GetShiftSlotStart(now - SECONDS_PER_WEEK);
    }
    else
    {
        curr = GetShiftSlotStart(now - SECONDS_PER_SHIFT);
    }

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    if (!Nova2PHP_environment_list(&env, NULL))
    {
        CfOut(cf_error, "", "!! Unable to query list of environments");
        CFDB_Close(&dbconn);
        return;
    }

    for (; curr + (3600 * 6) < now; curr += SECONDS_PER_SHIFT)  // in case of all slots
    {
        slot = GetShiftSlot(curr);

        // first any environment, then environment-specific

        Nova_CacheTotalComplianceEnv(&dbconn, "any", NULL, slot, curr, now);

        for (ep = env; ep != NULL; ep = ep->next)
        {
            snprintf(envName, sizeof(envName), "%s", ep->name);
            snprintf(envClass, sizeof(envClass), "environment_%s", ep->name);

            Nova_CacheTotalComplianceEnv(&dbconn, envName, envClass, slot, curr, now);
        }
    }

    FreeEnvironmentsList(env);

    Rlist *hostkeys = CFDB_QueryHostKeys(&dbconn, NULL, NULL, now - (2 * SECONDS_PER_SHIFT), now, NULL);
    for (const Rlist *rp = hostkeys; rp; rp = rp->next)
    {
        CFDB_RefreshLastHostComplianceShift(&dbconn, ScalarValue(rp));
    }
    DeleteRlist(hostkeys);

    CFDB_Close(&dbconn);
}


void Nova_CacheTotalComplianceEnv(mongo_connection *conn, char *envName, char *envClass, int slot,
                                  time_t start, time_t now)
{
    HubQuery *hq;
    HubTotalCompliance *ht;
    Rlist *rp;
    double kept, repaired, notkept;
    int count;
    time_t end;

    kept = 0;
    repaired = 0;
    notkept = 0;
    count = 0;

    end = start + SECONDS_PER_SHIFT;

    HostClassFilter *filter = NewHostClassFilter(envClass, NULL);

    hq = CFDB_QueryTotalCompliance(conn, NULL, NULL, start, end, -1, -1, -1, false, filter);
    DeleteHostClassFilter(filter);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        ht = (HubTotalCompliance *) rp->item;

        kept += ht->kept;
        repaired += ht->repaired;
        notkept += ht->notkept;
        count++;
    }

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (count > 0)
    {
        kept = kept / count;
        repaired = repaired / count;
        notkept = notkept / count;
    }

    CFDB_SaveCachedTotalCompliance(conn, envName, slot, kept, repaired, notkept, count, now);
}
