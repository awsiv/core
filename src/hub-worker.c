/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf.nova.h"
#include "hub.h"

#include "promises.h"
#include "transaction.h"
#include "files_names.h"
#include "db_query.h"
#include "db_save.h"


static void Nova_CreateHostID(EnterpriseDB *dbconnp, char *hostID, char *ipaddr);
static int Nova_HailPeer(EnterpriseDB *dbconn, char *hostID, char *peer);

/*******************************************************************/

void Nova_SequentialScan(Item *masterlist)
{
    EnterpriseDB dbconn;
    Item *ip;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    for (ip = masterlist; ip != NULL; ip = ip->next)
    {
        Nova_HailPeer(&dbconn, ip->name, ip->classes);
    }

    CFDB_Close(&dbconn);
}

/*******************************************************************/

static int Nova_HailPeer(EnterpriseDB *dbconn, char *hostID, char *peer)
{

    AgentConnection *conn;
    int long_time_no_see = false;

    char lock_id[CF_MAXVARSIZE];
    snprintf(lock_id, sizeof(lock_id), "%s%s", LOCK_HAIL_PREFIX, CanonifyName(peer));

    // FIXME: unhandled error condition here: if the function fails we do delta query (but do not report)
    if (AcquireLockByID(lock_id, 6 * MINUTES_PER_HOUR))
    {
        long_time_no_see = true;
    }

    Promise *pp = NewPromise("hail", "open");
    Attributes aa = { {0} };

    aa.copy.portnumber = (short) atoi(STR_CFENGINEPORT);

    CfOut(cf_inform, "", "...........................................................................\n");
    CfOut(cf_inform, "", " * Hailing %s : %u\n", peer, (int) aa.copy.portnumber);
    CfOut(cf_inform, "", "...........................................................................\n");

// record client host id (from lastseen) immediately so we can track failed connection attempts
// the timestamp is updated when we get response - see UnpackReportBook

    Nova_CreateHostID(dbconn, hostID, peer);

/* Check trust interaction*/

    aa.copy.trustkey = true;
    aa.copy.servers = SplitStringAsRList(peer, '*');
    pp->cache = NULL;

    conn = NewServerConnection(aa, pp);
    DeletePromise(pp);

    if (conn == NULL)
    {
        CfOut(cf_verbose, "", " !! Peer \"%s\" did not respond to hail", peer);
        
        if(long_time_no_see)
        {
            CfOut(cf_verbose, "", "Invalidating full query timelock since connection failed this time");

            if(!InvalidateLockTime(lock_id))
            {
                CfOut(cf_error, "", "!! Could not remove full query lock %s", lock_id);
            }
        }
        
        return false;
    }

// Choose full / delta

    int report_len;
    char *menu;

    if (long_time_no_see)
    {
        menu = "full";
        time_t last_week = time(0) - (time_t) SECONDS_PER_WEEK;

        CfOut(cf_verbose, "", " -> Running FULL sensor sweep of %s", HashPrint(CF_DEFAULT_DIGEST, conn->digest));
        report_len = Nova_QueryClientForReports(dbconn, conn, "full", last_week);
        
        if(report_len <= 0)
        {
            CfOut(cf_verbose, "", "Invalidating full query timelock since no data was received this time");
            
            if(!InvalidateLockTime(lock_id))
            {
                CfOut(cf_error, "", "!! Could not remove full query lock %s", lock_id);
            }
        }
    }
    else
    {
        menu = "delta";

        CfOut(cf_verbose, "", " -> Running differential sensor sweep of %s",
              HashPrint(CF_DEFAULT_DIGEST, conn->digest));
        report_len = Nova_QueryClientForReports(dbconn, conn, "delta", time(NULL) - SECONDS_PER_MINUTE * 10);
    }

    Nova_HubLog("Received %d bytes of reports from %s with %s menu", report_len, peer, menu);

    DisconnectServer(conn);
    DeleteRlist(aa.copy.servers);
    return true;
}

/*******************************************************************/

/* Make sure an entry for the given keyhash,ip exists */

static void Nova_CreateHostID(EnterpriseDB *dbconn, char *hostID, char *ipaddr)
{
    CFDB_SaveHostID(dbconn, MONGO_DATABASE, cfr_keyhash, hostID, ipaddr, NULL);
    CFDB_SaveHostID(dbconn, MONGO_ARCHIVE, cfr_keyhash, hostID, ipaddr, NULL);
}
