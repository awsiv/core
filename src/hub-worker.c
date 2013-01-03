/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf.nova.h"
#include "hub.h"

#include "promises.h"
#include "transaction.h"
#include "files_names.h"
#include "files_hashes.h"
#include "db_query.h"
#include "db_save.h"
#include "cfstream.h"
#include "string_lib.h"
#include "client_code.h"
#include "communication.h"
#include "lastseen.h"
#include "item_lib.h"
#include "keyring.h"
#include "db_maintain.h"

static void Nova_CreateHostID(EnterpriseDB *dbconn, char *hostkey, char *ipaddr);
static int Nova_HailPeer(EnterpriseDB *dbconn, char *hostID, char *peer);
static void GetHostIdentifier(EnterpriseDB *dbconn, char *keyhash, char *ip,
                              char *buffer, int bufsize);

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

static void GetHostIdentifier(EnterpriseDB *dbconn, char *keyhash, char *ip,
                              char *buffer, int bufsize)
{
    const char *hostname = NULL;
    char identifier[CF_MAXVARSIZE] = {0};

    buffer[0] = '\0';

    bool ret = CFDB_HandleGetValue(cfr_host_identifier, identifier, CF_MAXVARSIZE - 1,
                                   NULL, dbconn, MONGO_SCRATCH);

    if(ret)
    {
        char **split_scope = String2StringArray(identifier, '.');

        hostname = CFDB_QueryVariableValueStr(dbconn, keyhash, "s", split_scope[0], split_scope[1]);

        if(hostname)
        {
            snprintf(buffer, bufsize, "%s", hostname);
        }

        FreeStringArray(split_scope);
    }

    if(strlen(buffer) < 1)
    {
        buffer[0] = '\0';
        snprintf(buffer, bufsize, "%s", IPString2Hostname(ip));
    }
}
/*******************************************************************/
/* Make sure an entry for the given keyhash,ip exists */

static void Nova_CreateHostID( EnterpriseDB *dbconn, char *hostkey, char *ipaddr )
{
    char custom_identifier[CF_MAXVARSIZE] = {0};

    GetHostIdentifier( dbconn, hostkey, ipaddr, custom_identifier, CF_MAXVARSIZE - 1 );

    CFDB_SaveHostID( dbconn, MONGO_DATABASE, cfr_keyhash, hostkey, ipaddr, custom_identifier );
    CFDB_SaveHostID( dbconn, MONGO_ARCHIVE, cfr_keyhash, hostkey, ipaddr, custom_identifier );
}

/*********************************************************************/

Item *Nova_ScanClients(void)
{
    CF_DB *dbp;
    CF_DBC *dbcp;

    if (!OpenDB(&dbp, dbid_lastseen))
    {
        return NULL;
    }

    if (!NewDBCursor(dbp, &dbcp))
    {
        CloseDB(dbp);
        CfOut(cf_inform, "", " !! Unable to scan last-seen database");
        return NULL;
    }

    char *key;
    void *value;
    int ksize, vsize;
    Item *list = NULL;
    time_t now = time(NULL);
    int counter = 0;

    while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
    {
        /* Only read the hostkey entries */

        if (key[0] != 'k')
        {
            continue;
        }

        char hostkey[CF_BUFSIZE];
        strlcpy(hostkey, (char *)key + 1, CF_BUFSIZE);
        const char *address = value;

        /* Get the last seen timestamp */

        time_t timestamp = 0;

        char quality_key[CF_BUFSIZE];
        snprintf(quality_key, CF_BUFSIZE, "qi%s", hostkey);

        KeyHostSeen quality;

        if (ReadDB(dbp, quality_key, &quality, sizeof(quality)) == true)
        {
            timestamp = MAX(timestamp, now - quality.lastseen);
        }

        snprintf(quality_key, CF_BUFSIZE, "qo%s", hostkey);

        if (ReadDB(dbp, quality_key, &quality, sizeof(quality)) == true)
        {
            timestamp = MAX(timestamp, now - quality.lastseen);
        }
        }

        Item *ip = PrependItem(&list, hostkey, address);
        ip->time = timestamp;

        if (counter++ > LICENSES)
        {
            CfOut(cf_error,""," !! This hub is only licensed to support %d clients, so truncating at %d", LICENSES, LICENSES);
            break;
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    return list;
}

/*********************************************************************/

void DBRefreshHostsListCache(Item *list)
{
    Item *ip = NULL, *lastseen = NULL, *ip2 = NULL, *new_lastseen = NULL;
    Item *deleted_hosts = NULL;
    int count = 0;

    deleted_hosts = CFDB_GetDeletedHosts();
    lastseen = CFDB_GetLastseenCache();

// add from the new list
    for (ip = list; ip != NULL; ip = ip->next)
    {
        if (deleted_hosts && IsItemIn(deleted_hosts, ip->name))
        {
            continue;
        }

        PrependFullItem(&new_lastseen, ip->name, ip->classes, 0, time(NULL));
        count++;
    }

// now add items from the prev lastseen db
    for (ip2 = lastseen; ip2 != NULL; ip2 = ip2->next)
    {
        if (IsItemIn(new_lastseen, ip2->name)   //already added from list
            || ((time(NULL) - ip2->time) > CF_HUB_HORIZON)      // entry passed horizon)
            || IsItemIn(deleted_hosts, ip2->name))      //deleted
        {
            continue;
        }

        PrependFullItem(&new_lastseen, ip2->name, ip2->classes, 0, ip2->time);
        count++;
    }

    CFDB_SaveLastseenCache(new_lastseen);

    DeleteItemList(lastseen);
    DeleteItemList(new_lastseen);

    if (deleted_hosts)
    {
        bool removed = false;

        for (ip = deleted_hosts; ip != NULL; ip = ip->next)
        {
            if (!(RemoveHostFromLastSeen(ip->name)))
            {
                CfOut(cf_error, "", "Lastseen entry for host %s could not be removed\n", ip->name);
            }
            else
            {
                CfOut(cf_verbose, "", "Lastseen entry for host %s successfully removed\n", ip->name);

                if (RemovePublicKey(ip->name) != -1)
                {
                    removed = true;
                    CfOut(cf_verbose, "", "Public key for host %s successfully removed\n", ip->name);
                }
                else
                {
                    CfOut(cf_error, "", "Public key for host %s could not be removed\n", ip->name);
                }
            }
        }

        // if all hosts marked as "deleted" were removed from cf_lastseen.tcdb
        // purge the list of deleted host
        // otherwise keep it for the next run of cf-hub

        if (removed)
        {
            CFDB_PurgeDeletedHosts();
        }

        DeleteItemList(deleted_hosts);
    }

    CfOut(cf_inform, "", "%d hosts added to the lastseen cache\n", count);
}

/*********************************************************************/
