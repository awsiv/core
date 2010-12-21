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

struct Rlist *SERVER_KEYRING = NULL;

/*****************************************************************************/

void Nova_TranslatePath(char *new,char *old)

{
if (strncmp(old,"/var/cfengine",strlen("/var/cfengine")) == 0)
   {
   snprintf(new,CF_BUFSIZE-1,"%s%s",CFWORKDIR,old+strlen("/var/cfengine"));
   }
else
   {
   strncpy(new,old,CF_BUFSIZE-1);
   }
}

/*****************************************************************************/

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
   CfOut(cf_verbose,""," -> License problem, reverting to community edition behaviour");
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

if (Nova_GetPersistentScalar(handle,recv,CF_BUFSIZE-1,CF_HUB_HORIZON))
   {
   return true;
   }
else if (GetVariable("remote_access",handle,(void *)&retval,&rtype) != cf_notype)
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

int Nova_ReturnQueryData(struct cfd_connection *conn,char *menu,char *sendbuffer)

{ char rsignal;
  void *retval;
  time_t date;
  char buffer[CF_MAXVARSIZE],out[CF_BUFSIZE],menu_name[CF_MAXVARSIZE];
  char tbuf[CF_SMALLBUF];
  enum cfd_menu type;
  struct Item *ip,*reply = NULL;
  int cipherlen = 0;
  time_t from = 0, time0 = 0, time1 = 0, time2 = 0,delta1;

if (LICENSES <= 0)
   {
   return false;
   }

// Promise: Get time0 fom stream, find time1 and compute transfer delta

sscanf(menu,"%255s %ld %ld",menu_name,&from,&time0);
time1 = time(NULL);
delta1 = time1 - time0;

if (delta1 >= 30)
   {
   CfOut(cf_verbose,""," !! Poor clock synchronization between peers");
   }

// Promise: use valid menu request

strcpy(tbuf,ctime(&time1));
Chop(tbuf);
CfOut(cf_verbose,""," -> Menu request \"%s\" at %s, clock error %d",menu_name,tbuf,delta1);
strcpy(tbuf,ctime(&from));
Chop(tbuf);
CfOut(cf_verbose,""," -> Menu request starting from %s",tbuf);

if ((type = String2Menu(menu_name)) == cfd_menu_error)
   {
   CfOut(cf_verbose,""," -> Unknown menu type \"%s\"",menu_name);
   return false;
   }

// Promise: use menu data and start-time

switch (type)
   {
   case cfd_menu_delta:
       Nova_PackPerformance(&reply,CFR_PERF,from,type);
       Nova_PackClasses(&reply,CFR_CLASS,from,type);
       Nova_PackFileChanges(&reply,CFR_FCHANGE,from,type);
       Nova_PackDiffs(&reply,CFR_FDIFF,from,type);
       Nova_PackMonitorMag(&reply,CFR_MONITOR_MAG,from,type);
       Nova_PackCompliance(&reply,CFR_PCOMPLIANCE,from,type);
       Nova_PackTotalCompliance(&reply,CFR_TCOMPLIANCE,from,type);
       Nova_PackLastSeen(&reply,CFR_LASTSEEN,from,type);
       Nova_PackRepairLog(&reply,CFR_REPAIRLOG,from,type);
       Nova_PackNotKeptLog(&reply,CFR_NOTKEPTLOG,from,type);
       Nova_PackMeter(&reply,CFR_METER,from,type);
       Nova_PackBundles(&reply,CFR_BUNDLES,from,type);
       break;
       
   case cfd_menu_full:

       Nova_PackPerformance(&reply,CFR_PERF,from,type);
       Nova_PackClasses(&reply,CFR_CLASS,from,type);
       Nova_PackSetuid(&reply,CFR_SETUID,from,type);
       Nova_PackFileChanges(&reply,CFR_FCHANGE,from,type);
       Nova_PackDiffs(&reply,CFR_FDIFF,from,type);
       Nova_PackMonitorWeek(&reply,CFR_MONITOR_WEEK,from,type);
       Nova_PackMonitorMag(&reply,CFR_MONITOR_MAG,from,type);
       Nova_PackMonitorHist(&reply,CFR_MONITOR_HIST,from,type);
       Nova_PackMonitorYear(&reply,CFR_MONITOR_YEAR,from,type);
       Nova_PackCompliance(&reply,CFR_PCOMPLIANCE,from,type);
       Nova_PackTotalCompliance(&reply,CFR_TCOMPLIANCE,from,type);
       Nova_PackSoftware(&reply,CFR_SOFTWARE,from,type);
       Nova_PackAvailPatches(&reply,CFR_AVAILPATCH,from,type);
       Nova_PackPatchStatus(&reply,CFR_PATCHSTATUS,from,type);
       Nova_Pack_promise_output_common(&reply,CFR_PROMISEOUT,from,type);
       Nova_PackValueReport(&reply,CFR_VALUE,from,type);
       Nova_PackVariables(&reply,CFR_VARS,from,type);
       Nova_PackLastSeen(&reply,CFR_LASTSEEN,from,type);
       Nova_PackRepairLog(&reply,CFR_REPAIRLOG,from,type);
       Nova_PackNotKeptLog(&reply,CFR_NOTKEPTLOG,from,type);
       Nova_PackMeter(&reply,CFR_METER,from,type);
       Nova_PackBundles(&reply,CFR_BUNDLES,from,type);
       break;
   }

// Promise: get time2 and return for delta
// Promise: return size as a service

time2 = time(NULL);
CfOut(cf_verbose,""," -> Assembled reply at %s",ctime(&time2));

snprintf(buffer,CF_MAXVARSIZE,"CFR: %ld %ld %ld",delta1,time2,ItemListSize(reply));
PrependItem(&reply,buffer,NULL);

for (ip = reply; ip != NULL; ip=ip->next)
   {
   cipherlen = EncryptString(conn->encryption_type,ip->name,out,conn->session_key,strlen(ip->name)+1);
   
   if (SendTransaction(conn->sd_reply,out,cipherlen,CF_MORE) == -1)
      {
      CfOut(cf_error,"send","Failed send");
      return false;
      }
   }

cipherlen = EncryptString(conn->encryption_type,"QUERY complete",out,conn->session_key,strlen("QUERY complete"));

if (SendTransaction(conn->sd_reply,out,cipherlen,CF_DONE) == -1)
   {
   CfOut(cf_error,"send","Failed send");
   return false;
   }

DeleteItemList(reply);
return true;
}

/*****************************************************************************/

char *Nova_GetRemoteScalar(char *proto,char *handle,char *server,int encrypted,char *recvbuffer)

{ char in[CF_BUFSIZE],out[CF_BUFSIZE],sendbuffer[CF_BUFSIZE];
  int cipherlen,tosend,n,plainlen;
  struct cfagent_connection *conn;
  struct Attributes a = {0};
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
   snprintf(recvbuffer,CF_BUFSIZE-1,"BAD:");
   return recvbuffer;
   }

CfOut(cf_verbose,""," -> * Hailing %s:%u for remote handle \"%s\"\n",peer,(unsigned int)a.copy.portnumber,handle);

conn = NewServerConnection(a,pp);

if (conn == NULL)
   {
   CfOut(cf_inform,""," !! No suitable server responded to hail\n");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   snprintf(recvbuffer,CF_BUFSIZE-1,"BAD:");
   return recvbuffer;
   }

if (encrypted)
   {
   snprintf(in,CF_BUFSIZE,"%s %s",proto,handle);   

   if ((cipherlen = EncryptString('N',in,out,conn->session_key,cf_strlen(in)+1)) < 0)
      {
      CfOut(cf_error,""," !! Encryption failed for \"%s\"",in);
      return recvbuffer;
      }

   snprintf(sendbuffer,CF_BUFSIZE,"S%s %d",proto,cipherlen);
   memcpy(sendbuffer+CF_PROTO_OFFSET,out,cipherlen);
   tosend = cipherlen + CF_PROTO_OFFSET;
   }
else
   {
   snprintf(sendbuffer,CF_BUFSIZE,"%s %s",proto,handle);
   tosend = cf_strlen(sendbuffer);
   } 
 
if (SendTransaction(conn->sd,sendbuffer,tosend,CF_DONE) == -1)
   {
   cfPS(cf_error,CF_INTERPT,"send",pp,a,"Failed send");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   snprintf(recvbuffer,CF_BUFSIZE-1,"BAD:");
   return recvbuffer;
   }

if ((n = ReceiveTransaction(conn->sd,recvbuffer,NULL)) == -1)
   {
   cfPS(cf_error,CF_INTERPT,"recv",pp,a,"Failed send");
   CfOut(cf_verbose,"","No answer from host\n");
   ServerDisconnection(conn);
   DeleteRlist(a.copy.servers);
   DeletePromise(pp);
   snprintf(recvbuffer,CF_BUFSIZE-1,"BAD:");
   return recvbuffer;
   }

if (strncmp(recvbuffer,"BAD:",4) == 0)
   {
   snprintf(recvbuffer,CF_BUFSIZE-1,"BAD:");
   return recvbuffer;
   }

if (encrypted)
   {
   memcpy(in,recvbuffer,n);

   if ((plainlen = DecryptString('N',in,recvbuffer,conn->session_key,n)) < 0)
      {
      }
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
SERVERLIST = NULL;

DeleteRlist(a.copy.servers);
DeletePromise(pp);
return recvbuffer;
}

/********************************************************************/    

void Nova_CacheUnreliableValue(char *caller,char *handle,char *buffer)

{ char key[CF_BUFSIZE],name[CF_BUFSIZE];
  CF_DB *dbp;
  
snprintf(key,CF_BUFSIZE-1,"%s_%s",caller,handle);
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_CACHE); 

CfOut(cf_verbose,""," -> Caching value \"%s\" for fault tolerance",buffer);

if (!OpenDB(name,&dbp))
   {
   return;
   }

WriteDB(dbp,key,buffer,cf_strlen(buffer)+1);
CloseDB(dbp);
}

/********************************************************************/

int Nova_RetrieveUnreliableValue(char *caller,char *handle,char *buffer)

{ char key[CF_BUFSIZE],name[CF_BUFSIZE];
  CF_DB *dbp;

snprintf(key,CF_BUFSIZE-1,"%s_%s",caller,handle);
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_CACHE);

CfOut(cf_verbose,"","Checking cache %s for last available value",name);

memset(buffer,0,CF_BUFSIZE);

if (!OpenDB(name,&dbp))
   {
   return false;
   }

ReadDB(dbp,key,buffer,CF_BUFSIZE-1);
CloseDB(dbp);
return cf_strlen(buffer);
}

/********************************************************************/

RSA *Nova_SelectKeyRing(char *name)

{ struct Rlist *rp;
  struct CfKeyBinding *kp;
  RSA *newkey;
  
CfOut(cf_verbose,""," -> Looking for key for \"%s\" in cache",name);
 
for (rp = SERVER_KEYRING; rp !=  NULL; rp=rp->next)
   {
   kp = (struct CfKeyBinding *) rp->item;

   if (strcmp(kp->name,name) == 0)
      {
      CfOut(cf_verbose,""," -> Retrieving key for \"%s\" from cache with address",name,kp->address);
      ThreadLock(cft_system);
      RSA_up_ref(kp->key);
      ThreadUnlock(cft_system);
      return kp->key;
      }
   }

return NULL;
}

/********************************************************************/

void Nova_IdempAddToKeyRing(char *name,char *ipaddress,RSA *key)

{ struct Rlist *rp;
  struct CfKeyBinding *kp;
  
for (rp = SERVER_KEYRING; rp !=  NULL; rp=rp->next)
   {
   kp = (struct CfKeyBinding *) rp->item;

   if (strcmp(kp->name,name) == 0)
      {
      return;
      }
   }

CfOut(cf_verbose,""," -> Caching key for %s in the ring",name);

ThreadLock(cft_system);
kp = (struct CfKeyBinding *)malloc((sizeof(struct CfKeyBinding)));
ThreadUnlock(cft_system);

if (kp == NULL)
   {
   return;
   }

ThreadLock(cft_system);
if ((kp->name = strdup(name)) == NULL)
   {
   free(kp);
   ThreadUnlock(cft_system);
   return;
   }
ThreadUnlock(cft_system);

rp = PrependRlist(&SERVER_KEYRING,"nothing",CF_SCALAR);

ThreadLock(cft_system);
free(rp->item);
rp->item = kp;
RSA_up_ref(key);
ThreadUnlock(cft_system);
kp->address = strdup(ipaddress);
kp->key = key;
kp->timestamp = time(NULL);
}

/********************************************************************/

void Nova_PurgeKeyRing()

{ struct Rlist *rp,*rpp = NULL;
  struct CfKeyBinding *kp;
  time_t now = time(NULL);
  static time_t then;
  
if (now < then + 3600 && then > 0 && then <= now + 3600)
   {
   // Rate limiter
   return;
   }

then = now;
rp = SERVER_KEYRING;

while (rp !=  NULL)
   {
   kp = (struct CfKeyBinding *) rp->item;

   CfOut(cf_verbose,""," -> Holding key for %s (%s)",kp->name,kp->address);
   
   if (now > kp->timestamp + KEYTTL*3600)
      {
      ThreadLock(cft_system);

      if (rpp == NULL)
         {
         SERVER_KEYRING = rp->next;
         }
      else
         {
         rpp->next = rp->next;
         }

      RSA_free(kp->key);
      
      if (kp->address)
         {
         free(kp->address);
         }

      free(kp->name);
      free(kp);
      free(rp);      
      ThreadUnlock(cft_system);

      if (rpp == NULL)
         {
         rp = SERVER_KEYRING;
         }
      else
         {
         rp = rpp->next;
         }
      }
   else
      {
      rpp = rp;
      rp = rp->next;
      }
   }

CfOut(cf_verbose,"","----");
}

/********************************************************************/
/* Level                                                            */
/********************************************************************/

enum cfd_menu String2Menu(char *s)

{ static char *menus[] = { "delta", "full", NULL };
  int i;
 
for (i = 0; menus[i] != NULL; i++)
   {
   if (strcmp(s,menus[i]) == 0)
      {
      return i;
      }
   }

return cfd_menu_error;
}

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


