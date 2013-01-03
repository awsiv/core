
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"

#include "cf3.extern.h"
#include "cf.nova.h"
#include "promises.h"
#include "crypto.h"
#include "cfstream.h"
#include "client_code.h"
#include "logging.h"
#include "net.h"

#if defined(HAVE_LIBMONGOC)
#include "db_save.h"
#endif

/*****************************************************************************/
/*                                                                           */
/* File: cfnova.c                                                            */
/*                                                                           */
/*****************************************************************************/

void Nova_Initialize()
{
#ifdef HAVE_FIPS_MODE_SET
    if (FIPS_MODE && !FIPS_mode_set(1))
    {
        FatalError("Unable to initialize validated FIPS mode");
    }
    else
    {
        CfOut(cf_verbose, "", " ** CFEngine Nova is operating in FIPS 140-2 validated mode");
    }
#else
    if (FIPS_MODE)
    {
        FatalError("This software version has not been built with FIPS mode support");
    }
#endif
}

#if defined(__MINGW32__)

bool BootstrapAllowed(void)
{
    return true;
}

#endif

/*****************************************************************************/

void EnterpriseModuleTrick()
{
#ifdef HAVE_LIBMONGOC
    char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c

    if (false)
    {
        //   Nova2PHP_software_report(0,0,0,0,0,cfr_software,NULL,buffer,20);
        //   Nova2PHP_classes_report(NULL,NULL,0,NULL,buffer,1000);
        //   Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,NULL,buffer,1000);
        //   Nova2PHP_compliance_promises(NULL,NULL,"x",0,NULL,buffer,10000);
        //   Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,NULL,buffer,10000);
        //   Nova2PHP_bundles_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_performance_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_setuid_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",NULL,buffer,10000);
        //   Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",NULL,buffer,10000);
        //   Nova2PHP_value_report(NULL,NULL,NULL,NULL,NULL,buffer,1000);
        //   Nova2PHP_promiselog(NULL,NULL,1,0,0,NULL,buffer,1000);
        //   Nova2PHP_promises(NULL, NULL, NULL, 0);
        //   Nova2PHP_getlastupdate(NULL,buffer,10);

        // Nova2PHP_choose_hosts("123",2,buffer,1000);
        Nova2PHP_get_host_colour(NULL, buffer, 3, PROMISE_CONTEXT_MODE_ALL);
        Nova2PHP_meter(NULL, NULL, 0, PROMISE_CONTEXT_MODE_ALL);

        /*
         * commenting
         */
        Nova2PHP_add_note(NULL, NULL, NULL, -1, NULL, NULL, 100);
        Nova2PHP_get_notes(NULL, NULL, NULL, NULL, -1, -1, NULL, NULL, 10000);
        Nova2PHP_get_host_noteid(NULL, NULL, 4096);

        Nova2PHP_get_knowledge_view(0, NULL, NULL);

        CfLDAPAuthenticate(NULL, NULL, NULL, NULL, NULL, 0, false);
    }
#endif
}

/*****************************************************************************/

const char *GetConsolePrefix(void)
{
    return "enterprise";
}

/*****************************************************************************/

const char *MailSubject(void)
{
    if (LICENSES)
    {
        return "Nova";
    }
    else
    {
        return "NO LICENSE";
    }
}

/****************************************************************************/

double BoundedValue(double value, double defval)
{
    if (value < 0 || value > CF_BIGNUMBER)
    {
        return defval;
    }
    else
    {
        return value;
    }
}

void TranslatePath(char *new, const char *old)
{
    if (strncmp(old, "/var/cfengine", strlen("/var/cfengine")) == 0)
    {
        snprintf(new, CF_BUFSIZE - 1, "%s%s", CFWORKDIR, old + strlen("/var/cfengine"));
    }
    else
    {
        strncpy(new, old, CF_BUFSIZE - 1);
    }
}

int Nova_ParseHostname(char *name, char *hostname)
{
    int port = (int) ntohs(SHORT_CFENGINEPORT);

    if (strchr(name, ':'))
    {
        sscanf(name, "%250[^:]:%d", hostname, &port);
    }
    else
    {
        strncpy(hostname, name, CF_MAXVARSIZE);
    }

    return (port);
}

/* Network transmission parameters: ciphers, cipher lengths etc */

const EVP_CIPHER *CfengineCipher(char type)
{
    switch (type)
    {
    case 'N':
        return EVP_aes_256_cbc();
    default:
        return EVP_bf_cbc();
    }
}

int CfSessionKeySize(char c)
{
    switch (c)
    {
    case 'c':
        /* This part should only occur on the server side */
        CfOut(cf_verbose, "", " !! Community level client connection to enterprise server");
        return CF_BLOWFISHSIZE;

    case 'N':
        CfOut(cf_verbose, "", " -> Selecting FIPS compliant encryption option");
        return CF_FIPS_SIZE;

    default:
        CfOut(cf_error, "", " !! Illegal client protocol connection type");
        CfOut(cf_log, "", " !! Illegal client connection to enterprise server");
        return CF_BLOWFISHSIZE;
    }
}

char CfEnterpriseOptions()
{
    /* If we are using the commercial compilation, use the FIPS option consistently */
    return 'N';
}

/* Nova-specific language functions support */

/* remotescalar() */

char *GetRemoteScalar(char *proto, char *handle, char *server, int encrypted, char *recvbuffer)
{
    char in[CF_BUFSIZE], out[CF_BUFSIZE], sendbuffer[CF_BUFSIZE];
    int cipherlen, tosend, n, plainlen;
    AgentConnection *conn;
    Attributes a = { {0} };
    Promise *pp = NewPromise("remotescalar", "handle");
    char peer[CF_MAXVARSIZE];

    a.copy.portnumber = (short) Nova_ParseHostname(server, peer);
    a.copy.trustkey = false;
    a.copy.encrypt = encrypted;
    a.copy.force_ipv4 = false;
    a.copy.servers = SplitStringAsRList(peer, '*');

    memset(recvbuffer, 0, CF_BUFSIZE);

    if (LICENSES == 0)
    {
        CfOut(cf_verbose, "", " !! The license has expired on this feature");
        snprintf(recvbuffer, CF_BUFSIZE - 1, "BAD:");
        return recvbuffer;
    }

    CfOut(cf_verbose, "", " -> * Hailing %s:%u for remote handle \"%s\"\n", peer, (unsigned int) a.copy.portnumber,
          handle);

    conn = NewServerConnection(a, pp);

    if (conn == NULL)
    {
        CfOut(cf_inform, "", " !! No suitable server responded to hail\n");
        DisconnectServer(conn);
        DeleteRlist(a.copy.servers);
        DeletePromise(pp);
        snprintf(recvbuffer, CF_BUFSIZE - 1, "BAD:");
        return recvbuffer;
    }

    if (encrypted)
    {
        snprintf(in, CF_BUFSIZE, "%s %s", proto, handle);

        if ((cipherlen = EncryptString('N', in, out, conn->session_key, strlen(in) + 1)) < 0)
        {
            CfOut(cf_error, "", " !! Encryption failed for \"%s\"", in);
            return recvbuffer;
        }

        snprintf(sendbuffer, CF_BUFSIZE, "S%s %d", proto, cipherlen);
        memcpy(sendbuffer + CF_PROTO_OFFSET, out, cipherlen);
        tosend = cipherlen + CF_PROTO_OFFSET;
    }
    else
    {
        snprintf(sendbuffer, CF_BUFSIZE, "%s %s", proto, handle);
        tosend = strlen(sendbuffer);
    }

    if (SendTransaction(conn->sd, sendbuffer, tosend, CF_DONE) == -1)
    {
        cfPS(cf_error, CF_INTERPT, "send", pp, a, "Failed send");
        DisconnectServer(conn);
        DeleteRlist(a.copy.servers);
        DeletePromise(pp);
        snprintf(recvbuffer, CF_BUFSIZE - 1, "BAD:");
        return recvbuffer;
    }

    if ((n = ReceiveTransaction(conn->sd, recvbuffer, NULL)) == -1)
    {
        cfPS(cf_error, CF_INTERPT, "recv", pp, a, "Failed send");
        CfOut(cf_verbose, "", "No answer from host\n");
        DisconnectServer(conn);
        DeleteRlist(a.copy.servers);
        DeletePromise(pp);
        snprintf(recvbuffer, CF_BUFSIZE - 1, "BAD:");
        return recvbuffer;
    }

    if (strncmp(recvbuffer, "BAD:", 4) == 0)
    {
        snprintf(recvbuffer, CF_BUFSIZE - 1, "BAD:");
        return recvbuffer;
    }

    if (encrypted)
    {
        memcpy(in, recvbuffer, n);

        if ((plainlen = DecryptString('N', in, recvbuffer, conn->session_key, n)) < 0)
        {
        }
    }

    ConnectionsCleanup();

    DeleteRlist(a.copy.servers);
    DeletePromise(pp);
    return recvbuffer;
}

int RetrieveUnreliableValue(char *caller, char *handle, char *buffer)
{
    char key[CF_BUFSIZE];
    CF_DB *dbp;

    snprintf(key, CF_BUFSIZE - 1, "%s_%s", caller, handle);

    CfOut(cf_verbose, "", "Checking cache for last available value");

    memset(buffer, 0, CF_BUFSIZE);

    if (!OpenDB(&dbp, dbid_cache))
    {
        return false;
    }

    ReadDB(dbp, key, buffer, CF_BUFSIZE - 1);
    CloseDB(dbp);
    return strlen(buffer);
}

void CacheUnreliableValue(char *caller, char *handle, char *buffer)
{
    char key[CF_BUFSIZE];
    CF_DB *dbp;

    snprintf(key, CF_BUFSIZE - 1, "%s_%s", caller, handle);

    CfOut(cf_verbose, "", " -> Caching value \"%s\" for fault tolerance", buffer);

    if (!OpenDB(&dbp, dbid_cache))
    {
        return;
    }

    WriteDB(dbp, key, buffer, strlen(buffer) + 1);
    CloseDB(dbp);
}
