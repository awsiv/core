/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "server.h"
#include "cf.nova.h"
#include "client_protocol.h"

#include "promises.h"
#include "crypto.h"
#include "files_names.h"
#include "files_hashes.h"
#include "vars.h"
#include "item_lib.h"
#include "conversion.h"
#include "datapack.h"
#include "lastseen.h"
#include "sysinfo.h"
#include "cf-serverd-functions.h"
#include "unix.h"
#include "cfstream.h"
#include "client_code.h"
#include "communication.h"
#include "string_lib.h"
#include "net.h"

// These are needed for the collect calls, integrating with hub

#include "transaction.h"

#if defined(HAVE_LIBMONGOC)
#include "db_common.h" 
#endif

static int HailPeerCollect(char *host, Attributes a, Promise *pp);
static Promise *MakeCollectCallPromise(void);

/*****************************************************************************/

/*****************************************************************************/

void RegisterLiteralServerData(char *handle, Promise *pp)
{
    NewScalar("remote_access", handle, pp->promiser, cf_str);
}

/*****************************************************************************/

static int Nova_GetPersistentScalar(char *lval, char *rval, int size, time_t timeout)
{
    CF_DB *dbp;
    PersistentScalar var;
    time_t now = time(NULL);
    *rval = '\0';

    if (!OpenDB(&dbp, dbid_scalars))
    {
        CfOut(cf_verbose, "", " -> Unable to open db while looking for persistent scalar");
        return false;
    }

    if (ReadDB(dbp, lval, &var, sizeof(PersistentScalar)))
    {
        if (now > var.time + timeout)
        {
            DeleteDB(dbp, lval);
            CfOut(cf_verbose, "", " -> Persistent scalar timed out (%jd too late), so looking for default",
                  (intmax_t) now - var.time);
            CloseDB(dbp);
            return false;
        }
        else
        {
            CloseDB(dbp);
            strncpy(rval, var.rval, size - 1);
            return true;
        }
    }
    else
    {
        CloseDB(dbp);
        CfOut(cf_verbose, "", " -> Persistent scalar was not found, so looking for default");
        return false;
    }
}

int ReturnLiteralData(char *handle, char *recv)
{
    Rval retval;

    if (LICENSES <= 0)
    {
        return false;
    }

    if (Nova_GetPersistentScalar(handle, recv, CF_BUFSIZE - 1, CF_HUB_HORIZON))
    {
        CfOut(cf_verbose,""," Found a persistent scalar with handle %s authorized for remote access: %s",handle,recv);
        return true;
    }
    else if (GetVariable("remote_access", handle, &retval) != cf_notype)
    {
        if (retval.type == RVAL_TYPE_SCALAR)
        {
            strncpy(recv, retval.item, CF_BUFSIZE - 1);
            CfOut(cf_verbose,""," Found a literal string with handle %s authorized for remote access: %s",handle,recv);
            return true;
        }
        else
        {
            CfOut(cf_verbose,""," Found nothing with handle %s authorized for remote access",handle);
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/

int Nova_ReturnQueryData(ServerConnectionState *conn, char *menu)
{
    char out[CF_BUFSIZE], menu_name[CF_MAXVARSIZE];
    char tbuf[CF_SMALLBUF];
    enum cfd_menu type;
    Item *ip, *reply = NULL;
    int cipherlen = 0;
    time_t from = 0, time0 = 0, time1 = 0, delta1;

    if (LICENSES <= 0)
    {
        return false;
    }

// Promise: Get time0 fom stream, find time1 and compute transfer delta

    intmax_t from_i, time0_i;

    sscanf(menu, "%255s %" SCNdMAX " %" SCNdMAX "", menu_name, &from_i, &time0_i);
    from = (time_t)from_i;
    time0 = (time_t)time0_i;

    time1 = time(NULL);
    delta1 = time1 - time0;

    if (delta1 >= 30)
    {
        CfOut(cf_verbose, "", " !! Poor clock synchronization between peers");
    }

// Promise: use valid menu request

    cf_strtimestamp_local(time1, tbuf);
    if (Chop(tbuf, CF_EXPANDSIZE) == -1)
    {
        CfOut(cf_error, "", "Chop was called on a string that seemed to have no terminator");
    }
    CfOut(cf_verbose, "", " -> Menu request \"%s\" at %s, clock error %jd", menu_name, tbuf, (intmax_t) delta1);
    cf_strtimestamp_local(from, tbuf);
    if (Chop(tbuf, CF_EXPANDSIZE) == -1)
    {
        CfOut(cf_error, "", "Chop was called on a string that seemed to have no terminator");
    }
    CfOut(cf_verbose, "", " -> Menu request starting from %s", tbuf);

    if ((type = String2Menu(menu_name)) == cfd_menu_error)
    {
        CfOut(cf_verbose, "", " -> Unknown menu type \"%s\"", menu_name);
        return false;
    }

// Promise: use menu data and start-time

    Nova_PackAllReports(&reply, from, delta1, type);

// Promise: get time2 and return for delta
// Promise: return size as a service

    for (ip = reply; ip != NULL; ip = ip->next)
    {
        cipherlen = EncryptString(conn->encryption_type, ip->name, out, conn->session_key, strlen(ip->name) + 1);

        if (SendTransaction(conn->sd_reply, out, cipherlen, CF_MORE) == -1)
        {
            CfOut(cf_error, "send", "Failed send");
            DeleteItemList(reply);
            return false;
        }
    }

    cipherlen =
        EncryptString(conn->encryption_type, "QUERY complete", out, conn->session_key, strlen("QUERY complete"));

    if (SendTransaction(conn->sd_reply, out, cipherlen, CF_DONE) == -1)
    {
        CfOut(cf_error, "send", "Failed send");
        DeleteItemList(reply);
        return false;
    }

    DeleteItemList(reply);
    return true;
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

int Nova_AcceptCollectCall(ServerConnectionState *conn)
{

    int long_time_no_see = false, report_len;
    char lock_id[CF_MAXVARSIZE];
    EnterpriseDB dbconn;
    AgentConnection *aconn;
    
    snprintf(lock_id, sizeof(lock_id), "collect%s%s", LOCK_HAIL_PREFIX, CanonifyName(conn->ipaddr));

    if (LICENSES <= 0)
    {
        return false;
    }

    if (AcquireLockByID(lock_id, 6 * MINUTES_PER_HOUR))
    {
        long_time_no_see = true;
    }

    CfOut(cf_verbose, "", " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

    if ((aconn = ExtractCallBackChannel(conn)) == NULL)
    {
       return false;
    }
    
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (long_time_no_see)
    {
        time_t last_week = time(0) - (time_t) SECONDS_PER_WEEK;
       
        CfOut(cf_verbose, "", " -> Requesting FULL sensor sweep of %s", HashPrint(CF_DEFAULT_DIGEST, conn->digest));
        report_len = Nova_QueryClientForReports(&dbconn, aconn, "full", last_week);
        
        if (report_len <= 0)
        {
            CfOut(cf_verbose, "", "Invalidating full query timelock since no data was received this time");
           
            if (!InvalidateLockTime(lock_id))
            {
                CfOut(cf_error, "", "!! Could not remove full query lock %s", lock_id);
            }
        }
    }
    else
    {
        CfOut(cf_verbose, "", " -> Hub Requesting differential sensor sweep of satellite %s", HashPrint(CF_DEFAULT_DIGEST, conn->digest));
        report_len = Nova_QueryClientForReports(&dbconn, aconn, "delta", time(NULL) - SECONDS_PER_MINUTE * 10);
    }

    CfOut(cf_verbose, "", " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    
    CFDB_Close(&dbconn);
    
    DeleteAgentConn(aconn); // Because ExtractCallBackChannel allocates new memory
    return true;
}

#endif

/********************************************************************/
/* Collect calling from unknown or inaccessible networks            */
/********************************************************************/

void Nova_DoTryCollectCall(void)
{
    Promise *pp = MakeCollectCallPromise();
    Attributes a = { {0} };
 
    a.copy.trustkey = false;
    a.copy.encrypt = true;
    a.copy.force_ipv4 = false;
    a.copy.portnumber = SHORT_CFENGINEPORT;

    CfOut(cf_verbose,""," -> Going to attempt a collect call to the hub \"%s\" to see if it wants my reports",POLICY_SERVER);

    if (HailPeerCollect(POLICY_SERVER, a, pp))
    {
        CfOut(cf_verbose,""," -> Collect call with hub finished, and I closed the channel");
    }
    else
    {
        CfOut(cf_verbose,""," -> Collect call did not succeed in making contact");
    }

    PromiseDestroy(pp);
}


/********************************************************************/

static int HailPeerCollect(char *host, Attributes a, Promise *pp)
{
    AgentConnection *conn;
    char peer[CF_MAXVARSIZE], ipv4[CF_MAXVARSIZE],
        digest[CF_MAXVARSIZE], user[CF_SMALLBUF];
    extern int COLLECT_WINDOW;

    a.copy.portnumber = (short) Nova_ParseHostname(host, peer);

    snprintf(ipv4, CF_MAXVARSIZE, "%s", Hostname2IPString(peer));
    Address2Hostkey(ipv4, digest);
    GetCurrentUserName(user, CF_SMALLBUF);

    CfOut(cf_inform, "", "...........................................................................\n");
    CfOut(cf_inform, "", " * Peer collect call back to hub %s : %u \n", peer, a.copy.portnumber);
    CfOut(cf_inform, "", "...........................................................................\n");

    a.copy.servers = SplitStringAsRList(peer, '*');

    if (a.copy.servers == NULL || strcmp(a.copy.servers->item, "localhost") == 0)
    {
        cfPS(cf_inform, CF_NOP, "", pp, a, "No hub is registered to connect to");
        return false;
    }
    else
    {
        conn = NewServerConnection(a, pp);

        if (conn == NULL)
        {
            DeleteRlist(a.copy.servers);
            CfOut(cf_verbose, "", " -> No suitable hub server responded to hail\n");
            return false;
        }
    }

    pp->cache = NULL;

    // We are on the satellite host and calling the hub
    
    if (!Nova_PlaceCollectCall(conn))
    {
        DisconnectServer(conn);
        DeleteRlist(a.copy.servers);
        return false;
    }

    // We are on the satellite, handling a callback (if it comes)
    // The socket is still open and can just sit back and wait for
    // a possible report query now

    ServerEntryPoint(conn->sd, conn->remoteip, SV);

    // We don't want to formally wait, blocking, or we can be DDOSed
    // If the hub doesn't ask for reports in < 10s, it is probably not going to
    
    sleep(COLLECT_WINDOW);
    
    DisconnectServer(conn);
    DeleteRlist(a.copy.servers);

    return true;
}

/********************************************************************/

static Promise *MakeCollectCallPromise()
{
    Promise *pp;

/* The default promise here is to hail associates */

    pp = xcalloc(1, sizeof(Promise));

    pp->bundle = xstrdup("peer_collect_call");
    pp->promiser = xstrdup("peer-collect");
    pp->promisee = (Rval) {NULL, RVAL_TYPE_NOPROMISEE };
    pp->donep = &(pp->done);
    pp->this_server = pp->bundle;

    return pp;
}


/**********************************************************************/

AgentConnection *ExtractCallBackChannel(ServerConnectionState *conn)
{
    Attributes attr = { { 0 } };
    AgentConnection *ap = NewAgentConn();
    Promise *pp = MakeCollectCallPromise();

    ap->sd = conn->sd_reply;
    ap->trust = true;
    ap->authenticated = false;
    ap->protoversion = 0;
    strncpy(ap->username,conn->username,CF_SMALLBUF-1);
    //ap->localip[CF_MAX_IP_LEN];
    strncpy(ap->remoteip, conn->ipaddr,CF_MAX_IP_LEN);
    memcpy(ap->digest,conn->digest,EVP_MAX_MD_SIZE + 1);
    ap->session_key = NULL;
    ap->encryption_type = 'N';
    ap->family = SocketFamily(conn->sd_reply);

    if (!IdentifyAgent(ap->sd, ap->localip, ap->family))
       {
       CfOut(cf_error, "", " !! Id-authentication for %s failed\n", VFQNAME);
       errno = EPERM;
       PromiseDestroy(pp);
       return NULL;
       }

    if (!AuthenticateAgent(ap, attr, pp))
       {
       CfOut(cf_error, "", " !! Authentication dialogue on callback failed\n");
       errno = EPERM;
       PromiseDestroy(pp);
       return NULL;
       }

    ap->authenticated = true;
    PromiseDestroy(pp);

    return ap;
}

/********************************************************************/

int Nova_PlaceCollectCall(AgentConnection *conn)
{
    int tosend, cipherlen = 0;
    char in[CF_BUFSIZE], out[CF_BUFSIZE], workbuf[CF_BUFSIZE] = { 0 };
    time_t now = time(NULL);

    snprintf(in, CF_BUFSIZE - CF_PROTO_OFFSET, "CALL_ME_BACK collect_calls");
    cipherlen = EncryptString(conn->encryption_type, in, out, conn->session_key, strlen(in) + 1);
    snprintf(workbuf, CF_BUFSIZE, "SCALLBACK %4d", cipherlen);
    memcpy(workbuf + CF_PROTO_OFFSET, out, cipherlen);
    tosend = cipherlen + CF_PROTO_OFFSET;

    /* Remote client formulates the query to send to the receiver */

    CfOut(cf_verbose, "", " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    CfOut(cf_verbose, "", " -> Collect calling hub at %s", cf_ctime(&now));
    CfOut(cf_verbose, "", " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    if (SendTransaction(conn->sd, workbuf, tosend, CF_DONE) == -1)
    {
        CfOut(cf_error, "send", "Couldn't send data");
        return false;
    }

    // We don't wait for any reply here from the hub, as this just makes the interaction more fragile
    return true;
}

/********************************************************************/
/* Level                                                            */
/********************************************************************/

int SetServerListenState(size_t queue_size)
{
    if (SERVER_LISTEN || IsDefinedClass("am_policy_hub", NULL))
    {
        return InitServer(queue_size);
    }

    return -1;
}

void TryCollectCall(void)
{
    pthread_t tid;
    pthread_attr_t threadattrs;

    CfOut(cf_verbose, "", " -> Spawning new thread for the collect call...\n");

    pthread_attr_init(&threadattrs);
    pthread_attr_setdetachstate(&threadattrs, PTHREAD_CREATE_DETACHED);

#  ifdef HAVE_PTHREAD_ATTR_SETSTACKSIZE
    pthread_attr_setstacksize(&threadattrs, (size_t) 1024 * 1024);
#  endif

    // This thread will eventually be appropriated to run the ServerEntryPoint

    pthread_create(&tid, &threadattrs, (void *) Nova_DoTryCollectCall, (void *) NULL);
    pthread_attr_destroy(&threadattrs);
}

int ReceiveCollectCall(ServerConnectionState *conn, char *sendbuffer)
{
#if defined(HAVE_LIBMONGOC)
    CfOut(cf_verbose, "", "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    CfOut(cf_verbose, "", "  Hub: Accepting Collect Call from %s ", conn->hostname);
    CfOut(cf_verbose, "", "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

    return Nova_AcceptCollectCall(conn);
#else

    CfOut(cf_verbose, "", "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    CfOut(cf_verbose, "", "  Collect Call is only supported on the hub ");
    CfOut(cf_verbose, "", "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

    return false;
#endif
}
