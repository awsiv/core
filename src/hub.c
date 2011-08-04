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

#ifdef NT
#include <process.h>
#endif

/*******************************************************************/
/* GLOBAL VARIABLES                                                */
/*******************************************************************/

int  NO_FORK = false;
int  CONTINUOUS = false;
char MAILTO[CF_BUFSIZE];
char MAILFROM[CF_BUFSIZE];
char EXECCOMMAND[CF_BUFSIZE];
char VMAILSERVER[CF_BUFSIZE];
struct Item *SCHEDULE = NULL;
struct Item *FEDERATION = NULL;

pid_t MYTWIN = 0;
int MAXLINES = 30;
int SPLAYTIME = 0;
const int INF_LINES = -2;
int NOSPLAY = false;
int NOWINSERVICE = false;
int THREADS = 0;
int CFH_ZENOSS = false;

extern struct BodySyntax CFEX_CONTROLBODY[];

/* Prototypes */

void StartHub(int argc,char **argv);
int ScheduleRun(void);

/*****************************************************************************/

#ifndef MINGW

void Nova_StartHub(int argc,char **argv)

{ int time_to_run = false;
  time_t now = time(NULL);
  struct Promise *pp = NewPromise("hub_cfengine","the aggregator"); 
  struct Attributes a = {{0}};
  struct CfLock thislock;

Banner("Starting hub core");

a.restart_class = "nonce";
a.transaction.ifelapsed = 0;
a.transaction.expireafter = 0;

thislock = AcquireLock(pp->promiser,VUQNAME,now,a,pp,false);

if (thislock.lock == NULL)
   {
   DeletePromise(pp);
   return;
   }

if ((!NO_FORK) && (fork() != 0))
   {
   CfOut(cf_inform,"","cf-hub starting %.24s\n",cf_ctime(&now));
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

      if(CFDB_QueryIsMaster())  // relevant if we are part of mongo replica set
         {
         struct Item *masterhostlist = Nova_ScanClients();
      
         Nova_ParallelizeScan(masterhostlist,a,pp);      
         DeleteItemList(masterhostlist);

#ifdef HAVE_CONSTELLATION
         Constellation_ScanHubs(FEDERATION);  // query reports from hubs
#endif

         if (Nova_ShiftChange())
            {
            CfOut(cf_verbose,""," -> Scanning all total compliance cache");
            NewClass("am_policy_hub");
            Nova_CacheTotalCompliance(true);
            CFDB_Maintenance(false);
      
#ifdef HAVE_CONSTELLATION
            CFDBCon_CacheVirtualBundleCompliance();
#endif
            }

         // Longterm reports cleanup everyday
         if(IsDefinedClass("Hr10.Min00_05"))
            {
            CFDB_Maintenance(true);
            }
   
         if (CFH_ZENOSS && IsDefinedClass("Min00_05"))
            {
            Nova_ZenossSummary(DOCROOT);
            }
   
         Nova_CountMonitoredClasses();
         }
      else
         {
         CfOut(cf_verbose,"","We are part of report repliaca set, but not master - not collecting reports...\n");
         }
      }
   
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
  struct Promise *ppp = NewPromise("hail","open"); 
  struct Attributes aa = {{0}};

aa.restart_class = "nonce";
aa.transaction.ifelapsed = 60*6;
aa.transaction.expireafter = CF_INFINITY;
  
thislock = AcquireLock(ppp->promiser,CanonifyName(peer),now,aa,ppp,true);

if (thislock.lock != NULL)
   {
   long_time_no_see = true;
   }

aa.copy.portnumber = (short)atoi(STR_CFENGINEPORT);

CfOut(cf_inform,"","...........................................................................\n");
CfOut(cf_inform,""," * Hailing %s : %u\n",peer,(int)aa.copy.portnumber);
CfOut(cf_inform,"","...........................................................................\n");

/* Check trust interaction*/

aa.copy.trustkey = true;
aa.copy.servers = SplitStringAsRList(peer,'*');
ppp->cache = NULL;

conn = NewServerConnection(aa,ppp);
DeletePromise(ppp);

if (conn == NULL)
   {
   CfOut(cf_verbose,""," !! Peer \"%s\" did not respond to hail\n",peer);

   if (thislock.lock != NULL)
      {
      YieldCurrentLock(thislock);
      }
   
   return false;
   }

// Choose full / delta

if (long_time_no_see)
   {
   time_t last_week = time(0) - (time_t)CF_WEEK;
   CfOut(cf_verbose,""," -> Running FULL sensor sweep of %s",HashPrint(CF_DEFAULT_DIGEST,conn->digest));
   Nova_QueryForKnowledgeMap(conn,"full",last_week);

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
   // don't yield lock here - we never got it
   }

ServerDisconnection(conn);
DeleteRlist(aa.copy.servers);
return true;
}

/*********************************************************************/

void Nova_CacheTotalCompliance(bool allSlots)
/*
 * Caches the current slot of total compliance.
 * WARNING: Must be run every 6 hrs (otherwise no data is show in the
 * graph slot).
 */
{
#ifdef HAVE_LIBMONGOC
  time_t curr,now = time(NULL);
  mongo_connection dbconn;
  struct EnvironmentsList *env, *ep;
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

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database to cache total compliance");
   return;
   }


if (!Nova2PHP_environments_list(&env))
   {
   CfOut(cf_error, "", "!! Unable to query list of environments");
   CFDB_Close(&dbconn);
   return;
   }

for(; curr + (3600 * 6) < now; curr += SECONDS_PER_SHIFT) // in case of all slots
   {
   slot = GetShiftSlot(curr);

   // first any environment, then environment-specific
   
   Nova_CacheTotalComplianceEnv(&dbconn,"any",NULL,slot,curr,now);

   for (ep = env; ep != NULL; ep = ep->next)
      {
      snprintf(envName, sizeof(envName), "%s", ep->name);
      snprintf(envClass, sizeof(envClass), "environment_%s", ep->name);
      
      Nova_CacheTotalComplianceEnv(&dbconn,envName,envClass,slot,curr,now);
      }
   }

FreeEnvironmentsList(env);
CFDB_Close(&dbconn);

#endif  /* HAVE_LIBMONGOC */
}

/*********************************************************************/

void Nova_CacheTotalComplianceEnv(mongo_connection *conn, char *envName, char *envClass, int slot, time_t start, time_t now)

{
#ifdef HAVE_LIBMONGOC

  struct HubQuery *hq;
  struct HubTotalCompliance *ht;
  struct Rlist *rp;
  double kept,repaired,notkept;
  int count;
  time_t end;

kept = 0;
repaired = 0;
notkept = 0;
count = 0;

end = start + SECONDS_PER_SHIFT;

hq = CFDB_QueryTotalCompliance(conn,NULL,NULL,start,-1,-1,-1,CFDB_GREATERTHANEQ,false,envClass);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   if(ht->t >= end)
      {
      continue;
      }

   kept += ht->kept;
   repaired += ht->repaired;
   notkept += ht->notkept;
   count++;
   }

DeleteHubQuery(hq,DeleteHubTotalCompliance);

if(count > 0)
   {
   kept = kept/count;
   repaired = repaired/count;
   notkept = notkept/count;
   }

CFDB_SaveCachedTotalCompliance(conn, envName, slot, kept, repaired, notkept, count, now); 

#endif  /* HAVE_LIBMONGOC */
}

/*********************************************************************/

void Nova_CountMonitoredClasses()

{
#ifdef HAVE_LIBMONGOC

  char work[CF_BUFSIZE];
  struct HubQuery *hq;
  struct Rlist *rp;
  struct Item *order_results = NULL,*ip;
  mongo_connection dbconn;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database to count monitored classes");
   return;
   }

snprintf(work,CF_MAXVARSIZE-1,"%s.*",MONITOR_CLASS_PREFIX);

hq = CFDB_QueryClasses(&dbconn,NULL,work,true,CF_HUB_HORIZON,NULL,false);

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

/*********************************************************************/
/* Hub control                                                       */
/*********************************************************************/

struct Item *Nova_ScanClients()

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key,name[CF_BUFSIZE];
  void *value;
  struct CfKeyHostSeen entry;
  int ksize,vsize;
  struct Item *list = NULL,*listm;

  
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
      if (strcmp(value,CF_UNKNOWN_IP) == 0)
         {
         continue;
         }
      
      memcpy(&entry,value,sizeof(entry));
      IdempPrependItem(&list,key+1,entry.address);
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);

#ifdef HAVE_LIBMONGOC
/* Now we need to do some magic for hubs so that only one hub collects reports at a time */
NewClass("am_policy_hub");
CfOut(cf_inform,"","Checking for Hub master\n");
if (CFDB_QueryIsMaster())
   {
   CfOut(cf_inform,"","I am Hub master\n");

   Nova_UpdateMongoHostList(list);
   DeleteItemList(list);

   // If there is a list in Mongo, this takes precedence, else populate one
   list = Nova_GetMongoLastSeen();
   }
else
   {
   CfOut(cf_inform,"","I am not the hub master\n");
   DeleteItemList(list);
   list = NULL;   
   }
#endif
return list;
}

/*********************************************************************/

void Nova_HubLog(char *s)

{ char filename[CF_BUFSIZE];
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

int Nova_ShiftChange()

{
 if (IsDefinedClass("(Hr00|Hr06|Hr12|Hr16|Hr18).Min00_05"))
   {
   return true;
   }
else
   {
   return false;
   }
}

/*******************************************************************/
/* Command line options                                            */
/*******************************************************************/

const char *ID = "The hub is a scheduler and aggregator for the CFDB knowledge\n"
                 "repository. It automatically schedules updates from clients\n"
                 "that have registered by previous connection.";

 
const struct option OPTIONS[15] =
      {
      { "help",no_argument,0,'h' },
      { "debug",optional_argument,0,'d' },
      { "verbose",no_argument,0,'v' },
      { "dry-run",no_argument,0,'n'},
      { "version",no_argument,0,'V' },
      { "file",required_argument,0,'f'},
      { "no-lock",no_argument,0,'K'},
      { "no-fork",no_argument,0,'F' },
      { "continuous",no_argument,0,'c' },
      { "cache",no_argument,0,'a' },
      { "logging",no_argument,0,'l' },
      { NULL,0,0,'\0' }
      };

const char *HINTS[15] =
      {
      "Print the help message",
      "Set debugging level 0,1,2,3",
      "Output verbose information about the behaviour of the agent",
      "All talk and no action mode - make no changes, only inform of promises not kept",
      "Output the version of the software",
      "Specify an alternative input file than the default",
      "Ignore locking constraints during execution (ifelapsed/expireafter) if \"too soon\" to run",
      "Run as a foreground processes (do not fork)",
      "Continuous update mode of operation",
      "Rebuild database caches used for efficient query handling (e.g. compliance graphs)",
      "Enable logging of updates to the promise log",
      NULL
      };

/*****************************************************************************/

#endif  /* NOT MINGW */

int main(int argc,char *argv[])

{
#ifdef MINGW

CfOut(cf_error,"","This service is not available on Windows.");
return 1;

#else

CheckOpts(argc,argv);
GenericInitialize(argc,argv,"hub");
ThisAgentInit();
KeepPromises();
StartHub(argc,argv);

#endif

return 0;
}

#ifndef MINGW

/*****************************************************************************/
/* Level 1                                                                   */
/*****************************************************************************/

void CheckOpts(int argc,char **argv)

{ extern char *optarg;
  char arg[CF_BUFSIZE];
  int optindex = 0;
  int c;

while ((c=getopt_long(argc,argv,"cd:vKf:VhFlMa",OPTIONS,&optindex)) != EOF)
  {
  switch ((char) c)
      {
      case 'f':

          if (optarg && strlen(optarg) < 5)
             {
             snprintf(arg,CF_MAXVARSIZE," -f used but argument \"%s\" incorrect",optarg);
             FatalError(arg);
             }

          strncpy(VINPUTFILE,optarg,CF_BUFSIZE-1);
          VINPUTFILE[CF_BUFSIZE-1] = '\0';
          MINUSF = true;
          break;

      case 'l':
          LOGGING = true;
          break;
          
      case 'd': 
          NewClass("opt_debug");
          switch ((optarg==NULL) ? '3' : *optarg)
             {
             case '1':
                 D1 = true;
                 DEBUG = true;
                 break;
             case '2':
                 D2 = true;
                 DEBUG = true;
                 break;
             default:
                 DEBUG = true;
                 break;
             }
          break;
          
      case 'K': IGNORELOCK = true;
          break;
                    
      case 'I': INFORM = true;
          break;
          
      case 'v':
          VERBOSE = true;
          NO_FORK = true;
          break;
	            
      case 'n': DONTDO = true;
          IGNORELOCK = true;
          NewClass("opt_dry_run");
          break;

      case 'c':
          CONTINUOUS = true;
          break;

      case 'a':
          Nova_CacheTotalCompliance(true);
          exit(0);
          break;          
          
      case 'F':
          NO_FORK = true;
          break;

      case 'V': PrintVersionBanner("cf-hub");
          exit(0);
          
      case 'h': Syntax("cf-hub - cfengine's report aggregator",OPTIONS,HINTS,ID);
          exit(0);

      case 'M': ManPage("cf-hub - cfengine's report aggregator",OPTIONS,HINTS,ID);
          exit(0);
          
      default: Syntax("cf-hub - cfengine's report aggregator",OPTIONS,HINTS,ID);
          exit(1);
          
      }
   }

if (argv[optind] != NULL)
   {
   CfOut(cf_error,"","Unexpected argument with no preceding option: %s\n",argv[optind]);
   }
}

/*****************************************************************************/

void ThisAgentInit()

{
umask(077);

if (CONTINUOUS)
   {
   AppendItem(&SCHEDULE,"any",NULL);
   }
else if (SCHEDULE == NULL)
   {
   AppendItem(&SCHEDULE,"Min00",NULL);
   AppendItem(&SCHEDULE,"Min05",NULL);
   AppendItem(&SCHEDULE,"Min10",NULL);
   AppendItem(&SCHEDULE,"Min15",NULL);
   AppendItem(&SCHEDULE,"Min20",NULL);
   AppendItem(&SCHEDULE,"Min25",NULL);   
   AppendItem(&SCHEDULE,"Min30",NULL);
   AppendItem(&SCHEDULE,"Min35",NULL);
   AppendItem(&SCHEDULE,"Min40",NULL);
   AppendItem(&SCHEDULE,"Min45",NULL);
   AppendItem(&SCHEDULE,"Min50",NULL);
   AppendItem(&SCHEDULE,"Min55",NULL);
   }
}

/*****************************************************************************/

void KeepPromises()

{ struct Constraint *cp;
  char rettype;
  void *retval;

for (cp = ControlBodyConstraints(cf_hub); cp != NULL; cp=cp->next)
   {
   if (IsExcluded(cp->classes))
      {
      continue;
      }
   
   if (GetVariable("control_hub",cp->lval,&retval,&rettype) == cf_notype)
      {
      CfOut(cf_error,"","Unknown lval %s in hub control body",cp->lval);
      continue;
      }
   
   if (strcmp(cp->lval,CFH_CONTROLBODY[cfh_schedule].lval) == 0)
      {
      struct Rlist *rp;
      Debug("schedule ...\n");
      DeleteItemList(SCHEDULE);
      SCHEDULE = NULL;
      
      for (rp  = (struct Rlist *) retval; rp != NULL; rp = rp->next)
         {
         if (!IsItemIn(SCHEDULE,rp->item))
            {
            AppendItem(&SCHEDULE,rp->item,NULL);
            }
         }
      }
   
   if (strcmp(cp->lval,CFH_CONTROLBODY[cfh_federation].lval) == 0)
      {
      struct Rlist *rp;
      Debug("federation ...\n");
      DeleteItemList(FEDERATION);
      FEDERATION = NULL;
      
      for (rp  = (struct Rlist *) retval; rp != NULL; rp = rp->next)
         {
         if (!IsItemIn(FEDERATION,rp->item))
            {
            AppendItem(&FEDERATION,rp->item,NULL);
            }
         }
      }

   if (strcmp(cp->lval,CFH_CONTROLBODY[cfh_export_zenoss].lval) == 0)
      {
      CFH_ZENOSS = GetBoolean(retval);
      CfOut(cf_verbose,"","SET export_zenoss = %d\n",CFH_ZENOSS);
      continue;
      }
   
   if (strcmp(cp->lval,CFH_CONTROLBODY[cfh_port].lval) == 0)
      {
      SHORT_CFENGINEPORT = htons((short)Str2Int(retval));
      strncpy(STR_CFENGINEPORT,retval,15);
      CfOut(cf_verbose,"","SET default portnumber = %u = %s = %s\n",(int)SHORT_CFENGINEPORT,STR_CFENGINEPORT,retval);
      continue;
      }


   }
}

/*****************************************************************************/

#ifndef MINGW

void StartHub(int argc,char **argv)

{
#ifdef HAVE_NOVA
Nova_StartHub(argc,argv);
#else
CfOut(cf_error,"","This component is only used in commercial editions of the Cfengine software");
#endif
}

#endif  /* NOT MINGW */

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int ScheduleRun()

{
struct Item *ip;

if (EnterpriseExpiry())
  {
  CfOut(cf_error,"","Cfengine - autonomous configuration engine. This enterprise license is invalid.\n");
  exit(1);
  }

ThreadLock(cft_system);
DeleteAlphaList(&VHEAP);
InitAlphaList(&VHEAP);
DeleteAlphaList(&VADDCLASSES);
InitAlphaList(&VADDCLASSES);
DeleteItemList(IPADDRESSES);
IPADDRESSES = NULL;

DeleteScope("this");
DeleteScope("mon");
DeleteScope("sys");
DeleteScope("match");
NewScope("this");
NewScope("mon");
NewScope("sys");
NewScope("match");

CfGetInterfaceInfo(cf_executor);
Get3Environment();
OSClasses();
SetReferenceTime(true);
ThreadUnlock(cft_system);

for (ip = SCHEDULE; ip != NULL; ip = ip->next)
   {
   Debug("Checking schedule %s...\n",ip->name);

   if (IsDefinedClass(ip->name))
      {
      CfOut(cf_verbose,"","Waking up the agent at %s ~ %s \n",cf_ctime(&CFSTARTTIME),ip->name);
      return true;
      }
   }

return false;
}

#endif /* !MINGW */

/*****************************************************************************/

void Nova_UpdateMongoHostList(struct Item *list)

{
#ifdef HAVE_LIBMONGOC
 struct Item *ip = NULL;
  int count = 0;

CFDB_PurgeLastseenCache(list); 
  
for(ip=list;ip!=NULL;ip=ip->next)
   {
   CFDB_SaveLastseenCache(ip->name,ip->classes);
   count++;
   }
CfOut(cf_inform,"","%d hosts added to the lastseen cache\n",count);
 #endif
}

/*****************************************************************************/

struct Item *Nova_GetMongoLastSeen()

{
 #ifdef HAVE_LIBMONGOC
 // Read back the full list from Mongo
 return CFDB_GetLastseenCache();
 #else
 return NULL;
 #endif
}

/* EOF */
