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

char Nova_CfEnterpriseOptions()
{
if (LICENSES)
   {
   return 'N';
   }
else
   {
   return 'c';
   }
}

/*****************************************************************************/

int Nova_CfSessionKeySize(char c)

{
if (LICENSES == 0)
   {
   return CF_BLOWFISHSIZE;
   }
 
switch (c)
   {
   case 'c':
       /* This part should only occur on the server side */
       CfOut(cf_verbose,""," -> Community level client connection");
       CfOut(cf_log,""," !! Community level client connection to enterprise server");
       CfOut(cf_log,""," !! Note that full enterprise functionality requires all hosts to run at enterprise level");
       return CF_BLOWFISHSIZE;
       
   case 'N':
       CfOut(cf_verbose,""," -> Selecting FIPS compliant encryption option");
       return CF_FIPS_SIZE;

   default:
       CfOut(cf_error,""," !! Illegal client protocol connection type");
       CfOut(cf_log,""," !! Illegal client connection to enterprise server");       
       return CF_BLOWFISHSIZE;
   }
}

/*****************************************************************************/

const EVP_CIPHER *Nova_CfengineCipher(char type)

{
if (LICENSES <= 0)
   {
   return EVP_bf_cbc();
   }

switch(type)
   {
   case 'N':
       return EVP_aes_256_cbc();
   default:
       return EVP_bf_cbc();
   }
}

/*****************************************************************************/

int Nova_ReturnLiteralData(char *handle,char *recv)

{ char rtype;
  void *retval;

if (LICENSES <= 0)
   {
   return false;
   }

if (GetVariable("remote_access",handle,(void *)&retval,&rtype) != cf_notype)
   {
   if (rtype == CF_SCALAR)
      {     
      strncpy(recv,retval,CF_BUFSIZE-1);
      return true;
      }
   else
      {
      return false;
      }
   }
else
   {
   return false;
   }
}

/*****************************************************************************/

char *Nova_GetRemoteScalar(char *handle,char *server,int encrypted,char *recvbuffer)

{ char in[CF_BUFSIZE],out[CF_BUFSIZE],sendbuffer[CF_BUFSIZE];
  int cipherlen,tosend,n,plainlen;
  struct cfagent_connection *conn;
  struct Attributes a;
  struct Promise *pp = NewPromise("remotescalar","handle");
  char peer[CF_MAXVARSIZE];
  struct Rlist *rp;
  struct ServerItem *svp;
 
a.copy.portnumber = (short)Nova_ParseHostname(server,peer);
a.copy.trustkey = false;
a.copy.encrypt = encrypted;
a.copy.force_ipv4 = false;
a.copy.servers = SplitStringAsRList(peer,'*');

memset(recvbuffer,0,CF_BUFSIZE);

if (LICENSES == 0)
   {
   CfOut(cf_verbose,""," !! The license has expired on this feature");
   return "BAD:";
   }

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
   cipherlen = EncryptString('N',in,out,conn->session_key,cf_strlen(in)+1);
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

if (strncmp(recvbuffer,"BAD:",4) == 0)
   {
   return "BAD:";
   }

if (encrypted)
   {
   memcpy(in,recvbuffer,n);
   plainlen = DecryptString('N',in,recvbuffer,conn->session_key,n);
   }

for (rp = SERVERLIST; rp != NULL; rp = rp->next)
   {
   svp = (struct ServerItem *)rp->item;

   if (svp == NULL)
      {
      continue;
      }
   
   ServerDisconnection(svp->conn);
   
   if (svp->server)
      {
      free(svp->server);
      }
   
   rp->item = NULL;
   }

DeleteRlist(SERVERLIST);

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

pid_t Nova_StartTwin(int argc,char **argv)
#ifdef MINGW
{
CfOut(cf_verbose, "", "Twin exec daemons are not supported on Windows - running as single process.");
return 0;
}
#else  /* NOT MINGW */
{ FILE *fp;
  char name[CF_BUFSIZE];
  time_t last,now = time(NULL);
  struct stat sb;
  int pulse_delay = 0;
  struct Promise *pp = NewPromise("exec_twin","the executor twin"); 
  struct Attributes dummyattr;
  struct CfLock thislock;
  pid_t pid;
  

if (LICENSES == 0)
   {
   return 0;
   }
  
memset(&dummyattr,0,sizeof(dummyattr));
dummyattr.transaction.ifelapsed = 0;
dummyattr.transaction.expireafter = 0;

thislock = AcquireLock(pp->promiser,VUQNAME,now,dummyattr,pp);

if (thislock.lock == NULL)
   {
   return 0;
   }

DeletePromise(pp);

if ((pid = fork()) != 0)
   {
   CfOut(cf_inform,"","cf-execd binary pulsar starting %.24s\n",ctime(&now));
   return pid;
   }

//ActAsDaemon(0);

snprintf(name,CF_BUFSIZE-1,"%s/pulsar",CFWORKDIR);

sb.st_mtime = 0;
CfOut(cf_verbose,""," ** Twin in orbit... ");

while (true)
   {
   Nova_SignalOther();
   sleep(CFPULSETIME);
   now = time(NULL);

   if (cfstat(name,&sb) == -1 && !pulse_delay)
      {
      CfOut(cf_verbose,""," !! No pulse from twin...waiting ");
      pulse_delay++;
      continue;
      }

   pulse_delay = 0;
   
   if (sb.st_mtime >= now - CFPULSETIME*2)
      {
   printf("REVIVETWIN %d = %d\n",sb.st_mtime,now);
      CfOut(cf_verbose,""," -> Pulsar returned, continuing...");
      continue;
      }
   
   CfOut(cf_inform,""," !! No sign of twin, so assuming its role");
   YieldCurrentLock(thislock);
   
   if (execv(argv[0],argv) == -1)
      {
      CfOut(cf_error,"execv","Couldn't restart cfengine %s",argv[0]);
      }      
   }
}
#endif  /* NOT MINGW */

/********************************************************************/

void Nova_SignalTwin()

{ FILE *fp;
  char name[CF_BUFSIZE];

snprintf(name,CF_BUFSIZE-1,"%s/pulsar",CFWORKDIR);
  
if ((fp = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","cf-execd: Cannot write to the work directory %s",CFWORKDIR);
   return;
   }

fprintf(fp,"duh-dum,duh-dum...\n");

fclose(fp);

CfOut(cf_verbose,""," -> Pulse...");
}

/********************************************************************/

void Nova_SignalOther()

{ FILE *fp;
  char name[CF_BUFSIZE];

snprintf(name,CF_BUFSIZE-1,"%s/other",CFWORKDIR);
  
if ((fp = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","cf-execd: Cannot write to the work directory %s",CFWORKDIR);
   return;
   }

fprintf(fp,"duh-dum,duh-dum...\n");

fclose(fp);

CfOut(cf_verbose,""," -> Pulse echo...");
}

/********************************************************************/

void Nova_ReviveOther(int argc,char **argv)

{ time_t now = time(NULL);
  char name[CF_BUFSIZE];
  struct stat sb;
  
snprintf(name,CF_BUFSIZE-1,"%s/other",CFWORKDIR);
 
if (cfstat(name,&sb) == -1)
   {
   CfOut(cf_verbose,""," !! No pulse echo from twin...");
   }
else if (sb.st_mtime >= now - CFPULSETIME*2)
   {
   printf("REVIVEOTHER %d = %d\n",sb.st_mtime,now);
   CfOut(cf_verbose,""," -> Pulse echo returned, continuing...");
   return;
   }
else
   {
   CfOut(cf_verbose,""," !! The pulse echo was lost .. assuming the twin has died");
   }

Nova_StartTwin(argc,argv);
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

