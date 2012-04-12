/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf.nova.h"
#include "db_query.h"
#include "db_save.h"
#include "hub.h"

static void Nova_CreateHostID(mongo_connection *dbconnp, char *hostID, char *ipaddr);
static int Nova_HailPeer(mongo_connection *dbconn, char *hostID, char *peer);

void Nova_SequentialScan(Item *masterlist)
{
    mongo_connection dbconn;
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

static int Nova_HailPeer(mongo_connection *dbconn, char *hostID, char *peer)
{
    AgentConnection *conn;
    time_t average_time = 600, now = time(NULL);
    int long_time_no_see = false;
    CfLock thislock;
    Promise *ppp = NewPromise("hail", "open");
    Attributes aa = { {0} };

    aa.restart_class = "nonce";
    aa.transaction.ifelapsed = 60 * 6;
    aa.transaction.expireafter = CF_INFINITY;

    thislock = AcquireLock(ppp->promiser, CanonifyName(peer), now, aa, ppp, true);

    if (thislock.lock != NULL)
    {
        long_time_no_see = true;
    }

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
    ppp->cache = NULL;

    conn = NewServerConnection(aa, ppp);
    DeletePromise(ppp);

    if (conn == NULL)
    {
        CfOut(cf_verbose, "", " !! Peer \"%s\" did not respond to hail\n", peer);

        if (thislock.lock != NULL)
        {
            YieldCurrentLock(thislock);
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

        YieldCurrentLock(thislock);
    }
    else
    {
        menu = "delta";

        CfOut(cf_verbose, "", " -> Running differential sensor sweep of %s",
              HashPrint(CF_DEFAULT_DIGEST, conn->digest));
        report_len = Nova_QueryClientForReports(dbconn, conn, "delta", now - average_time);

        // don't yield lock here - we never got it
    }

    Nova_HubLog("Received %d bytes of reports from %s with %s menu", report_len, peer, menu);

    ServerDisconnection(conn);
    DeleteRlist(aa.copy.servers);
    return true;
}

/* Make sure an entry for the given keyhash,ip exists */

static void Nova_CreateHostID(mongo_connection *dbconn, char *hostID, char *ipaddr)
{
    CFDB_SaveHostID(dbconn, MONGO_DATABASE, cfr_keyhash, hostID, ipaddr, NULL);
    CFDB_SaveHostID(dbconn, MONGO_ARCHIVE, cfr_keyhash, hostID, ipaddr, NULL);
}
