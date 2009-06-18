/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: server.c                                                            */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

void Nova_RegisterLiteralServerData(char *handle,struct Promise *pp)

{
NewScalar("remote_access",handle,pp->promiser,cf_str);
}

/*****************************************************************************/

char *Nova_ReturnLiteralData(char *handle)

{ char rtype,*retval;

if (GetVariable("remote_access",handle,(void *)&retval,&rtype) != cf_notype)
   {
   return retval;
   }
else
   {
   return "";
   }
}

/*****************************************************************************/

char *Nova_GetRemoteScalar(char *handle,char *server,int encrypted)

{ char in[CF_BUFSIZE],out[CF_BUFSIZE],sendbuffer[CF_BUFSIZE];
 int cipherlen,tosend,n,plainlen;
  struct cfagent_connection *conn;
  struct Attributes a;
  struct Promise *pp = NewPromise("remotescalar","handle");
  static char recvbuffer[CF_BUFSIZE];
  char peer[CF_MAXVARSIZE];

a.copy.portnumber = (short)Nova_ParseHostname(server,peer);
a.copy.trustkey = false;
a.copy.encrypt = encrypted;
a.copy.force_ipv4 = false;
a.copy.servers = SplitStringAsRList(peer,'*');

memset(recvbuffer,0,CF_BUFSIZE);

CfOut(cf_verbose,""," -> * Hailing %s:%u for remote variable \"%s\"\n",peer,(unsigned int)a.copy.portnumber,handle);

conn = NewServerConnection(a,pp);

if (conn == NULL)
   {
   CfOut(cf_inform,""," !! No suitable server responded to hail\n");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   return "BAD:";
   }

if (encrypted)
   {
   snprintf(in,CF_BUFSIZE,"VAR %s",handle);   
   cipherlen = EncryptString(in,out,conn->session_key,cf_strlen(in)+1);
   snprintf(sendbuffer,CF_BUFSIZE,"SVAR %d",cipherlen);
   memcpy(sendbuffer+CF_PROTO_OFFSET,out,cipherlen);
   tosend = cipherlen + CF_PROTO_OFFSET;
   }
else
   {
   snprintf(sendbuffer,CF_BUFSIZE,"VAR %s",handle);
   tosend = cf_strlen(sendbuffer);
   } 
 
if (SendTransaction(conn->sd,sendbuffer,tosend,CF_DONE) == -1)
   {
   cfPS(cf_error,CF_INTERPT,"send",pp,a,"Failed send");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   return "BAD:";
   }

if ((n = ReceiveTransaction(conn->sd,recvbuffer,NULL)) == -1)
   {
   cfPS(cf_error,CF_INTERPT,"recv",pp,a,"Failed send");
   CfOut(cf_verbose,"","No answer from host\n");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   return "BAD:";
   }

if (encrypted)
   {
   memcpy(in,recvbuffer,n);
   plainlen = DecryptString(in,recvbuffer,conn->session_key,n);
   }

ServerDisconnection(conn);
DeleteRlist(a.copy.servers);
DeletePromise(pp);
return recvbuffer;
}

/********************************************************************/

void Nova_CacheUnreliableValue(char *caller,char *handle,char *buffer)

{ char key[CF_BUFSIZE],name[CF_BUFSIZE];
  DB *dbp;
  
snprintf(key,CF_BUFSIZE-1,"%s_%s",caller,handle);
snprintf(name,CF_BUFSIZE-1,"%s/nova_cache.db",CFWORKDIR); 

CfOut(cf_verbose,""," -> Caching value \"%s\" for fault tolerance",buffer);

if (!OpenDB(name,&dbp))
   {
   return;
   }

WriteDB(dbp,key,buffer,cf_strlen(buffer)+1);
dbp->close(dbp,0);
}

/********************************************************************/

int Nova_RetrieveUnreliableValue(char *caller,char *handle,char *buffer)

{ char key[CF_BUFSIZE],name[CF_BUFSIZE];
  DB *dbp;

snprintf(key,CF_BUFSIZE-1,"%s_%s",caller,handle);
snprintf(name,CF_BUFSIZE-1,"%s/nova_cache.db",CFWORKDIR);

CfOut(cf_verbose,"","Checking cache %s for last available value",name);

memset(buffer,0,CF_BUFSIZE);

if (!OpenDB(name,&dbp))
   {
   return false;
   }

ReadDB(dbp,key,buffer,CF_BUFSIZE-1);

dbp->close(dbp,0);

return cf_strlen(buffer);
}

/********************************************************************/

void Nova_StartTwin(int argc,char **argv)

{
 /* AcquireLock
// stat binary for mtime

if (fork() != 0)
   {
   CfOut(cf_inform,"","cfExec twin starting %.24s\n",ctime(&now));
   exit(0);
   }


//Writepid twin file?

// do  nothing here that could crash. We assume this process doesn't die

while (true)
   {
   sleep(1);
   // stat file and record

   // if file is older than sched interval, YieldLock, restart(popen) cfexecd and exit
   // Can't see a pulse from cf-execd
   // clean up lock first?

   // if binary mtime is newer yieldlock, execv(cfexecd,arg) and exit

   // if input files are changed (like cfservd) die and restart cf-execd, and die
   }

//never get here exit(0);

// If all this fails, we can still restart by running cf-runagent
*/
}

/********************************************************************/
/* Level                                                            */
/********************************************************************/

int Nova_ParseHostname(char *name,char *hostname)

{ int port = (int)ntohs(SHORT_CFENGINEPORT);

if (cf_strchr(name,':'))
   {
   sscanf(name,"%250[^:]:%d",hostname,&port);
   }
else
   {
   cf_strncpy(hostname,name,CF_MAXVARSIZE);
   }

return(port);
}

