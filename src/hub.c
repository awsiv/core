/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: hub.c                                                               */
/*                                                                           */
/* Created: Sat Jul 24 08:35:41 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

extern int  NO_FORK;
extern int  CONTINUOUS;

/*****************************************************************************/

#ifndef MINGW

void Nova_StartHub(int argc,char **argv)

{ int pid,time_to_run = false;
  time_t now = time(NULL);
  struct Promise *pp = NewPromise("hub_cfengine","the aggregator"); 
  struct Attributes a;
  struct CfLock thislock;

Banner("Starting hub core");
memset(&a,0,sizeof(a));

a.restart_class = "nonce";
a.transaction.ifelapsed = 0;
a.transaction.expireafter = 0;

thislock = AcquireLock(pp->promiser,VUQNAME,CFSTARTTIME,a,pp);

if (thislock.lock == NULL)
   {
   DeletePromise(pp);
   return;
   }

if ((!NO_FORK) && (fork() != 0))
   {
   CfOut(cf_inform,"","cf-execd starting %.24s\n",cf_ctime(&now));
   exit(0);
   }

if (!NO_FORK)
   {
   ActAsDaemon(0);
   }
   
WritePID("cf-hub.pid");
signal(SIGINT,HandleSignals);
signal(SIGTERM,HandleSignals);
signal(SIGHUP,SIG_IGN);
signal(SIGPIPE,SIG_IGN);
signal(SIGUSR1,HandleSignals);
signal(SIGUSR2,HandleSignals);
 
umask(077);

while (true)
   {
   time_to_run = ScheduleRun();
   
   if (time_to_run)
      {
      CfOut(cf_verbose,""," -> Wake up");
      struct Item *masterhostlist = Nova_ScanClients();
      
      Nova_ParallelizeScan(masterhostlist,a,pp);      
      DeleteItemList(masterhostlist);
      }

   Nova_CountMonitoredClasses();
   CfOut(cf_verbose,"","Sleeping...\n");
   sleep(CFPULSETIME);
   }

YieldCurrentLock(thislock); // Never get here
}

/********************************************************************/
/* level                                                            */
/********************************************************************/

void Nova_ParallelizeScan(struct Item *masterlist,struct Attributes a,struct Promise *pp)

{ const int cf_max_slots = 1024;
  struct Item *ip, *list[cf_max_slots];
  int i,scans = 50;

for (i = 0; i < cf_max_slots; i++)
   {
   list[i] = NULL;
   }

// Divide ...

for (ip = masterlist, i = 0; ip != NULL; ip=ip->next)
   {
   PrependItem(&(list[i]),ip->name,ip->classes);
   i = (i + 1) % scans;
   }

// Conquer ...

for (i = 0; i < scans; i++)
   {
   if (list[i])
      {
      Nova_ScanList(list[i],a,pp);
      }
   }
}

/********************************************************************/

void Nova_ScanList(struct Item *list,struct Attributes a,struct Promise *pp)

{ struct Item *ip;
  pid_t child_id = 1;
  
CfOut(cf_verbose,"","----------------------------------------------------------------\n");
CfOut(cf_verbose,""," Initiating scan on: ");

for (ip = list; ip != NULL; ip=ip->next)
   {
   CfOut(cf_verbose,""," ? %s ",ip->name);
   }

CfOut(cf_verbose,"","----------------------------------------------------------------\n");

if (NO_FORK)
   {
   for (ip = list; ip != NULL; ip=ip->next)
      {
      Nova_HailPeer(ip->classes,a,pp);
      }
   }
else
   {
   CfOut(cf_verbose,"","Spawning new process...\n");
   child_id = fork();

   if (child_id == 0)
      {
      // Am child
      ALARM_PID = -1;
      
      for (ip = list; ip != NULL; ip=ip->next)
         {
         Nova_HailPeer(ip->classes,a,pp);
         }
      
      exit(0);
      }
   else
      {
      // Parent returns to spawn again
      cf_pwait(child_id);
      return;
      }
   }
}

/********************************************************************/

int Nova_HailPeer(char *peer,struct Attributes a,struct Promise *pp)

{ struct cfagent_connection *conn;
  time_t average_time = 600, now = time(NULL);
  int long_time_no_see = false;
  struct CfLock thislock;

a.restart_class = "nonce";
a.transaction.ifelapsed = 6*60;
a.transaction.expireafter = CF_INFINITY;
  
thislock = AcquireLock(pp->promiser,CanonifyName(peer),CFSTARTTIME,a,pp);

if (thislock.lock != NULL)
   {
   long_time_no_see = true;
   }

a.copy.portnumber = (short)5308;

CfOut(cf_inform,"","...........................................................................\n");
CfOut(cf_inform,""," * Hailing %s : %u\n",peer,a.copy.portnumber);
CfOut(cf_inform,"","...........................................................................\n");

a.copy.servers = SplitStringAsRList(peer,'*');

conn = NewServerConnection(a,pp);

if (conn == NULL)
   {
   CfOut(cf_verbose,""," !! Peer \"%s\" did not respond to hail\n",peer);
   return false;
   }

/* Check trust interaction*/

pp->cache = NULL;

// Choose full / delta

if (long_time_no_see)
   {
   CfOut(cf_verbose,""," -> Running FULL sensor sweep of %s",HashPrint(CF_DEFAULT_DIGEST,conn->digest));
   Nova_QueryForKnowledgeMap(conn,"full",time(0));

   if (LOGGING)
      {
      char msg[CF_MAXVARSIZE];
      snprintf(msg,CF_MAXVARSIZE,"HUB full sensor sweep of peer %s",peer);
      Nova_HubLog(msg);
      }
   
   YieldCurrentLock(thislock);
   }
else
   {
   CfOut(cf_verbose,""," -> Running differential sensor sweep of %s",HashPrint(CF_DEFAULT_DIGEST,conn->digest));
   Nova_QueryForKnowledgeMap(conn,"delta",now - average_time);

   if (LOGGING)
      {
      char msg[CF_MAXVARSIZE];
      snprintf(msg,CF_MAXVARSIZE,"HUB delta sensor sweep of peer %s",peer);
      Nova_HubLog(msg);
      }
   }

ServerDisconnection(conn);
DeleteRlist(a.copy.servers);
return true;
}

/*********************************************************************/
/* Hub control                                                       */
/*********************************************************************/

struct Item *Nova_ScanClients()

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key,name[CF_BUFSIZE];
  void *value;
  struct CfKeyHostSeen entry;
  int ret,ksize,vsize, ok = false;
  struct Item *list = NULL;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_LASTDB_FILE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return NULL;
   }

if (!NewDBCursor(dbp,&dbcp))
   {
   CloseDB(dbp);
   CfOut(cf_inform,""," !! Unable to scan last-seen database");
   return NULL;
   }

 /* Initialize the key/data return pair. */

memset(&entry, 0, sizeof(entry));

 /* Walk through the database and print out the key/data pairs. */

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));
      IdempPrependItem(&list,key+1,entry.address);
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
return list;
}

/*********************************************************************/

void Nova_HubLog(char *s)

{ char filename[CF_BUFSIZE],start[CF_BUFSIZE],end[CF_BUFSIZE];
  time_t now = time(NULL);
  FILE *fout;

if (s == NULL || strlen(s) ==  0)
   {
   return;
   }
  
snprintf(filename,CF_BUFSIZE,"%s/%s",CFWORKDIR,"hub_log");

if ((fout = fopen(filename,"a")) == NULL)
   {
   CfOut(cf_error,"fopen","Could not open %s",filename);
   return;
   }

fprintf(fout,"%ld,%ld: %s\n",CFSTARTTIME,now,s);
fclose(fout);
}

/*********************************************************************/

void Nova_CountMonitoredClasses()

{
#ifdef HAVE_LIBMONGOC

  char *report,work[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  struct Item *order_results = NULL,*ip;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return;
   }

snprintf(work,CF_MAXVARSIZE-1,"%s.*",MONITOR_CLASS_PREFIX);

hq = CFDB_QueryClasses(&dbconn,bson_empty(&b),work,true,CF_HUB_HORIZON);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   struct HubClass *hc = (struct HubClass *)rp->item;
   IdempItemCount(&order_results,hc->class,NULL);
   }

DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

for (ip = order_results; ip != NULL; ip = ip->next)
   {
   char countstr[CF_SMALLBUF];
   snprintf(countstr,CF_SMALLBUF,"%d",ip->counter+1);
   Nova_SetPersistentScalar(ip->name+strlen(MONITOR_CLASS_PREFIX),countstr);
   }

#endif  /* HAVE_LIBMONGOC */
}


#endif  /* NOT MINGW */

