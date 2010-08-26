/*****************************************************************************/
/*                                                                           */
/* File: db_save.c                                                           */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report saving.                                  */
/*                                                                           */
/*****************************************************************************/

// TODO: Fix regex queries, ensure index on cfr_keyhash and sw.n
// TODO: Purge old classes (only adding for now) - TTL "LASTSEENEXPIRE" in cfengine code
// TODO: Report update errors!

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC

/*****************************************************************************/

int CFDB_Open(mongo_connection *conn, char *host, int port)

{ mongo_connection_options connOpts;

snprintf(connOpts.host, sizeof(connOpts.host), "%s", host);
connOpts.port = port;

if (mongo_connect(conn,&connOpts) != 0)
   {
   CfOut(cf_verbose, "mongo_connect", " -> Could not connect to database");
   return false;
   }

return true;
}

/*****************************************************************************/

int CFDB_Close(mongo_connection *conn)

{
if (mongo_destroy(conn) != 0)
   {
   CfOut(cf_verbose, "mongo_destroy", " -> Could not disconnect from database");
   return false;
   }

return true;
}

/*****************************************************************************/

void CFDB_Initialize()
    
{
 //  make sure indices on cfr_keyhash and sw.n exist and monitord arrays exist
}

/*****************************************************************************/
/* Cache / scratch space                                                     */
/*****************************************************************************/

void CFDB_PutValue(char *lval,char *rval)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson setOp,empty;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  mongo_connection dbconn;

if (!IsDefinedClass("am_policy_hub"))
   {
   return;
   }
  
if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"","!! Could not open connection to report database");
   return;
   }
  
bson_buffer_init(&bb);
setObj = bson_append_start_object(&bb, "$set");
snprintf(varName, sizeof(varName),"%s.%s",lval,cfr_rval);
bson_append_string(setObj,varName,rval);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(&dbconn, MONGO_SCRATCH,bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
CFDB_Close(&dbconn);
}

/*****************************************************************************/
/* Monitor data                                                              */
/*****************************************************************************/

void CFDB_SaveHostID(mongo_connection *conn,char *keyhash,char *ipaddr)

{ bson_buffer bb;
  bson_buffer *setObj,*clObj,*keyArr,*keyAdd,*keyArrField;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char name[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
  time_t t;
  double e = 0, dev = 0;
  char iStr[32];
  int i;
  
// locate right host key

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key,&bb);

// insert keys into numbered key array

bson_buffer_init(&bb);
keyAdd = bson_append_start_object(&bb,"$addToSet");
keyArrField = bson_append_start_object(keyAdd,cfr_ip_array);
keyArr = bson_append_start_array(keyAdd,"$each");

i = 0;
snprintf(iStr,sizeof(iStr),"%d",i);
bson_append_string(keyArr,iStr,ipaddr);

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE,&host_key,&setOp,MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveHostID",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);

// Again - locate right host key

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key,&bb);

// insert keys into numbered key array

bson_buffer_init(&bb);
keyAdd = bson_append_start_object(&bb,"$addToSet");
keyArrField = bson_append_start_object(keyAdd,cfr_host_array);
keyArr = bson_append_start_array(keyAdd,"$each");

i = 0;
snprintf(iStr,sizeof(iStr),"%d",i);
ThreadLock(cft_getaddr);
bson_append_string(keyArr,iStr,IPString2Hostname(ipaddr));
ThreadUnlock(cft_getaddr);

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE,&host_key,&setOp,MONGO_UPDATE_UPSERT);
bson_destroy(&setOp);

bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSoftware(mongo_connection *conn,enum software_rep sw, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson host_key;  // host description
  bson setOp;
  int i;
  bson_buffer *arr;
  struct Item *ip;
  char *dbOp = {0};
  char packNumStr[CF_MAXVARSIZE];
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch,archStr[CF_MAXVARSIZE];

// find right host

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb,"$set");

switch (sw)
   {
   case sw_rep_installed:       
       arr = bson_append_start_array(setObj,cfr_software);
       dbOp = "update installed software";
       break;
   case sw_rep_patch_installed:       
       arr = bson_append_start_array(setObj,cfr_patch_installed);
       dbOp = "update patch installed software";
       break;
   case sw_rep_patch_avail:       
       arr = bson_append_start_array(setObj,cfr_patch_avail);
       dbOp = "update patch available software";
       break;

   default:
       arr = bson_append_start_array(setObj,cfr_software);
       dbOp = "update installed software";
       break;
   }

/* Read and insert data */

for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);
   snprintf(packNumStr,sizeof(packNumStr),"%d",i);
   snprintf(archStr,sizeof(archStr),"%c",arch);
   sub = bson_append_start_object(arr,packNumStr);

   bson_append_string(sub,cfr_name,name);
   bson_append_string(sub,cfr_version,version);
   bson_append_string(sub,cfr_arch,archStr);

   bson_append_finish_object(sub);   
   }

bson_append_finish_object(arr);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,dbOp,keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveMonitorData(mongo_connection *conn, char *keyhash, enum monitord_rep rep_type, struct Item *data)

{ bson_buffer bb,record;
  bson host_key;  // host description
  char varNameIndex[64];
  bson_buffer *setObj;
  bson setOp;
  int i, arrIndex;
  struct Item *ip;
  int observable,slot;
  double q,e,dev;
  char *dbOperation = {0};
  char t[CF_TIME_SIZE];
  char timekey[CF_SMALLBUF];
  char *obsKey = {0};

switch(rep_type)
   {
   case mon_rep_mag:
       obsKey = cfm_magobs;
       dbOperation = "update mag";
       break;
   case mon_rep_week:
       obsKey = cfm_weekobs;
       dbOperation = "update week";
       break;
   case mon_rep_yr:
       obsKey = cfm_yearobs;
       dbOperation = "update year";
       break;
   default:
       CfOut(cf_error, "", "!! Unknown monitord report type (%d)", rep_type);
       FatalError("Software Error");
   }


for (ip = data; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"T: ",3) == 0)
      {
      switch(rep_type)
         {
         case mon_rep_mag:
             sscanf(ip->name+3,"%d",&slot);
             break;
         case mon_rep_week:
             memset(t,0,CF_TIME_SIZE);
             sscanf(ip->name+3,"%31[^,],%d",t,&slot);
             break;
         case mon_rep_yr:
             memset(t,0,CF_TIME_SIZE);
             sscanf(ip->name+3,"%31[^,],%d",t,&slot);
             break;
         }
      continue;
      }
   
   // Extract records

   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf",&observable,&q,&e,&dev);

   bson_buffer_init(&bb);
   setObj = bson_append_start_object(&bb, "$set");
   
   snprintf(varNameIndex, sizeof(varNameIndex),"%s.%d.%s",cfm_data,slot,cfr_obs_q);
   bson_append_double(setObj,varNameIndex,q);
   snprintf(varNameIndex, sizeof(varNameIndex),"%s.%d.%s",cfm_data,slot,cfr_obs_E);
   bson_append_double(setObj,varNameIndex,e);
   snprintf(varNameIndex, sizeof(varNameIndex),"%s.%d.%s",cfm_data,slot,cfr_obs_sigma);
   bson_append_double(setObj,varNameIndex,dev);

   bson_append_finish_object(setObj);
   bson_from_buffer(&setOp,&bb);

   // find right host and report
   bson_buffer_init(&record);
   bson_append_string(&record,cfr_keyhash,keyhash);
   bson_append_int(&record,obsKey,observable);
   bson_from_buffer(&host_key, &record);

   mongo_update(conn, MONGO_DATABASE_MON, &host_key, &setOp, MONGO_UPDATE_UPSERT);

   MongoCheckForError(conn,dbOperation,keyhash);

   bson_destroy(&setOp);
   bson_destroy(&host_key);
   }
 
}

/*****************************************************************************/

void CFDB_SaveMonitorHistograms(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *arr;
  bson host_key;  // host description
  bson setOp;
  char arrName[64], kStr[32];
  struct Item *ip;
  int i,k;
  double currHist;
  char *sp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");
  
for (ip = data; ip != NULL; ip=ip->next)
   {
   sp = ip->name;
   
   sscanf(ip->name,"%d",&i);
   
   while (*(++sp) != ':')
      {
      }

   sp++;

   snprintf(arrName, sizeof(arrName), "%s%d", cfr_histo, i);
   arr = bson_append_start_array(setObj , arrName);
   
   for (k = 0; k < CF_GRAINS; k++)
      {
      sscanf(sp,"%lf",&currHist);
      
      while (*(++sp) != ':')
         {
         }

      if (currHist < 0)
         {
         currHist = 1;
         }

      sp++;
      snprintf(kStr, sizeof(kStr),"%d",k);
      bson_append_double(arr,kStr,currHist);
      }

   bson_append_finish_object(arr);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn,MONGO_DATABASE,&host_key,&setOp,MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveMonitorHistograms",keyhash);
 
bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveClasses(mongo_connection *conn, char *keyhash, struct Item *data)

/*
 *  Replacing existing class entry, but not deleting "old" entries (purging)
 */

{ bson_buffer bb,*setObj,*clObj,*keyArr,*keyAdd,*keyArrField;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char name[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
  time_t t;
  double e = 0, dev = 0;
  char iStr[32];
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

// insert objects

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%[^,],%ld,%lf,%lf\n",name,&t,&e,&dev);   
   snprintf(varName, sizeof(varName), "%s.%s", cfr_class, name);
   
   clObj = bson_append_start_object(setObj , varName);
   
   bson_append_double(clObj,cfr_obs_E, e);
   bson_append_double(clObj,cfr_obs_sigma, dev);
   bson_append_int(clObj, cfr_time, t);
   bson_append_finish_object(clObj);
   }

bson_append_finish_object(setObj);

// insert keys into numbered key array

keyAdd = bson_append_start_object(&bb , "$addToSet");
keyArrField = bson_append_start_object(keyAdd,cfr_class_keys);
keyArr = bson_append_start_array(keyAdd , "$each");

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   sscanf(ip->name,"%[^,],%ld,%lf,%lf\n",name,&t,&e,&dev);   
   snprintf(iStr,sizeof(iStr),"%d",i);
   bson_append_string(keyArr,iStr,name);
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveClasses",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveVariables(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *varObj, *keyArr, *keyAdd, *keyArrField, *arr;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  int i;
  char iStr[32];
  struct Rlist *rp,*list;
  char type[CF_SMALLBUF],lval[CF_MAXVARSIZE],rval[CF_BUFSIZE],
      scope[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"S: ", 3) == 0)
      {
      scope[0] = '\0';
      sscanf(ip->name+3,"%254[^\n]",scope);
      continue;
      }
   
   sscanf(ip->name,"%4[^,], %255[^,], %2040[^\n]",type,lval,rval);
   
   if (strchr(lval,'/'))
      {
      // Private data not meant to be seen
      continue;
      }
   
   snprintf(varName, sizeof(varName),"%s.%s.%s.%s",cfr_vars,scope,lval,cfr_type);
   bson_append_string(setObj, varName, type);
   
   snprintf(varName, sizeof(varName),"%s.%s.%s.%s",cfr_vars,scope,lval,cfr_rval);
   
   if (IsCfList(type))
      {
      arr = bson_append_start_array(setObj, varName);
  
      list = ParseShownRlist(rval);
      
      for (rp = list, i = 0; rp != NULL; rp=rp->next, i++)
         {
         snprintf(iStr, sizeof(iStr), "%d", i);
         bson_append_string(arr, iStr, rp->item);
         }
      
      DeleteRlist(list);
      
      bson_append_finish_object(arr);
      }
   else
      {
      bson_append_string(setObj,varName,rval);
      }
   }

bson_append_finish_object(setObj);

// append scope.varname to key array

keyAdd = bson_append_start_object(&bb , "$addToSet");
keyArrField = bson_append_start_object(keyAdd , cfr_var_keys);
keyArr = bson_append_start_array(keyAdd , "$each");

for (ip = data, i = 0; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"S: ", 3) == 0)
      {
      scope[0] = '\0';
      sscanf(ip->name+3,"%254[^\n]",scope);
      continue;
      }
   
   sscanf(ip->name,"%4[^,], %255[^,], %2040[^\n]",type,lval,rval);
   
   snprintf(iStr, sizeof(iStr), "%d", i);
   snprintf(varName, sizeof(varName), "%s.%s", scope,lval);
   
   bson_append_string(keyArr,iStr,varName);
   i++;
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveVariables",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveTotalCompliance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *arr;
  char iStr[32];
  char version[CF_MAXVARSIZE];
  int kept,repaired,notrepaired;
  long t;
  time_t then;
  bson_buffer *sub;
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb, "$pushAll");

arr = bson_append_start_array(pushObj,cfr_total_compliance);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d", i);
   
   sscanf(ip->name,"%ld,%127[^,],%d,%d,%d\n",&t,version,&kept,&repaired,&notrepaired);
   then = (time_t)t;
   
   sub = bson_append_start_object(arr, iStr);
   bson_append_int(sub,cfr_time, then);
   bson_append_string(sub,cfr_version,version);
   bson_append_int(sub,cfr_kept,kept);
   bson_append_int(sub,cfr_repaired,repaired);
   bson_append_int(sub,cfr_notkept,notrepaired);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveTotalCompliance",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SavePromiseLog(mongo_connection *conn, char *keyhash, enum promiselog_rep rep_type, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *arr, *sub;
  bson host_key;  // host description
  bson setOp;
  char handle[CF_MAXVARSIZE],reason[CF_BUFSIZE];
  long then;
  time_t tthen;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  char *repName = {0};
  char *dbOp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

switch(rep_type)
   {
   case plog_repaired:
       repName = cfr_repairlog;
       dbOp = "update promise repaired";
       break;
   case plog_notkept:
       repName = cfr_notkeptlog;
       dbOp = "update promise not kept";
       break;
   default:
       CfOut(cf_error, "", "!! Unknown promise log report type (%d)", rep_type);
       return;
   }

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%254[^,],%1024[^\n]",&then,handle,reason);
   tthen = (time_t)then;

   snprintf(varName, sizeof(varName), "%s.%s@%d", repName,handle,then);
   
   sub = bson_append_start_object(setObj, varName);
   bson_append_string(sub,cfr_promisehandle, handle);
   bson_append_string(sub,cfr_cause,reason);
   bson_append_int(sub, cfr_time, then);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,dbOp,keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLastSeen(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char inout, ipaddr[CF_MAXVARSIZE],dns[CF_MAXVARSIZE];
  char hostkey[CF_MAXVARSIZE],varName[CF_MAXVARSIZE];
  double ago = 0 ,average = 0,dev = 0;
  long fthen = 0;
  time_t then = 0;
 
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c %625s %250s %250s %ld %lf %lf %lf\n",
          &inout,
          hostkey,
          dns,
          ipaddr,
          &fthen,
          &ago,
          &average,
          &dev);
   
   then = (time_t)fthen;
   
   snprintf(varName, sizeof(varName), "%s.%c%s", cfr_lastseen,inout,hostkey);

   sub = bson_append_start_object(setObj, varName);
   bson_append_string(sub, cfr_dnsname, dns);
   bson_append_string(sub, cfr_ipaddr, ipaddr);
   bson_append_double(sub, cfr_hrsago, ago);
   bson_append_double(sub, cfr_hrsavg, average);
   bson_append_double(sub, cfr_hrsdev, dev);
   bson_append_int(sub, cfr_time, then);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveLastSeen",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveMeter(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  char type;
  double kept,repaired;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c: %lf %lf",&type,&kept,&repaired);
   snprintf(varName, sizeof(varName), "%s.%c", cfr_meter, type);
   
   sub = bson_append_start_object(setObj,varName);
   bson_append_double(sub, cfr_meterkept,kept);
   bson_append_double(sub, cfr_meterrepaired,repaired);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveMeter",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePerformance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  long t;
  char eventname[CF_MAXVARSIZE];
  double measure = 0,average = 0,dev = 0;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");
  
for (ip = data; ip != NULL; ip=ip->next)
   {
   eventname[0] = '\0';
   sscanf(ip->name,"%ld,%lf,%lf,%lf,%255[^\n]\n",&t,&measure,&average,&dev,eventname);
   
   snprintf(varName, sizeof(varName), "%s.%s", cfr_performance, eventname);
   
   sub = bson_append_start_object(setObj , varName);
   bson_append_double(sub, cfr_obs_q, measure);
   bson_append_double(sub, cfr_obs_E, average);
   bson_append_double(sub, cfr_obs_sigma, dev);
   bson_append_int(sub, cfr_time,t);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SavePerformance",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSetUid(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *keyArr, *keyAdd, *keyArrField;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char progName[CF_MAXVARSIZE];
  char iStr[32];
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

// insert keys into key array

keyAdd = bson_append_start_object(&bb , "$addToSet");
keyArrField = bson_append_start_object(keyAdd , cfr_setuid);
keyArr = bson_append_start_array(keyAdd , "$each");

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   sscanf(ip->name,"%255[^\n]",progName);
   snprintf(iStr, sizeof(iStr), "%d", i);
   bson_append_string(keyArr, iStr, progName);
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveSetUid",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePromiseCompliance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  char handle[CF_MAXVARSIZE];
  char status, statusStr[16];
  time_t then;
  double av,dev;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");
  
for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%255[^,],%c,%lf,%lf\n",&then,handle,&status,&av,&dev);
   
   snprintf(varName, sizeof(varName), "%s.%s", cfr_promisecompl, handle);
   snprintf(statusStr, sizeof(statusStr), "%c", status);
   
   sub = bson_append_start_object(setObj , varName);
   bson_append_string(sub, cfr_promisestatus, statusStr);
   bson_append_double(sub, cfr_obs_E, av);
   bson_append_double(sub, cfr_obs_sigma, dev);
   bson_append_double(sub, cfr_time, then);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SavePromiseCompliance",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileChanges(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *arr;
  char iStr[32];
  char name[CF_SMALLBUF];
  bson_buffer *sub;
  time_t then;
  long date;
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb,"$pushAll");

arr = bson_append_start_array(pushObj,cfr_filechanges);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d", i);
   
   sscanf(ip->name,"%ld,%255[^\n]",&date,name);
   then = (time_t)date;
   
   sub = bson_append_start_object(arr,iStr);
   bson_append_int(sub,cfr_time,then);
   bson_append_string(sub,cfr_name,name);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveFileChanges",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveFileDiffs(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *arr;
  char iStr[32];
  char name[CF_MAXVARSIZE],change[CF_BUFSIZE];
  bson_buffer *sub;
  time_t then;
  int i;
  long t;
  char *sp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb, "$pushAll");

arr = bson_append_start_array(pushObj,cfr_filediffs);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d", i);
   
   change[0] = '\0';
   sscanf(ip->name,"%ld|%255[^|]|%2047[^\n]",&t,name,change);

   for (sp = change; *sp != '\0'; sp++)
      {
      if (*sp == CF_N_CODE)
         {
         *sp = '\n';
         }
      }

   then = (time_t)t;
   sub = bson_append_start_object(arr,iStr);
   bson_append_int(sub,cfr_time,then);
   bson_append_string(sub,cfr_name,name);
   bson_append_string(sub,cfr_diff,change);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveFileDiffs",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveBundles(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char bundle[CF_MAXVARSIZE];
  char hash[CF_MAXVARSIZE],varName[CF_MAXVARSIZE];
  double ago = 0,average = 0,dev = 0;
  long fthen = 0;
  time_t then = 0;
 
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250s %ld %lf %lf %lf\n",
          bundle,
          &fthen,
          &ago,
          &average,
          &dev);

   then = (time_t)fthen;
   
   snprintf(varName, sizeof(varName),"%s.%s",cfr_bundles,bundle);
   
   sub = bson_append_start_object(setObj, varName);
   bson_append_double(sub,cfr_hrsago,ago);
   bson_append_double(sub,cfr_hrsavg,average);
   bson_append_double(sub,cfr_hrsdev,dev);
   bson_append_int(sub,cfr_time,then);
   bson_append_finish_object(sub);
   }
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveBundles",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveValueReport(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *arr;
  char iStr[32];
  char name[CF_SMALLBUF],datestr[CF_SMALLBUF];
  double kept,notkept,repaired;
  bson_buffer *sub;
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb,"$pushAll");

arr = bson_append_start_array(pushObj,cfr_valuereport);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d",i);   
   sub = bson_append_start_object(arr,iStr);


   sscanf(ip->name,"%[^,],%lf,%lf,%lf\n",datestr,&kept,&repaired,&notkept);

   bson_append_string(sub,cfr_day,datestr);
   bson_append_double(sub,cfr_kept,kept);
   bson_append_double(sub,cfr_repaired,repaired);
   bson_append_double(sub,cfr_notkept,notkept);

   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveValueReport",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveLastUpdate(mongo_connection *conn, char *keyhash)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson host_key;  // host description
  bson setOp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

bson_append_int(setObj,cfr_day,(long)time(NULL));

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveLastUpdate",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}


#endif  /* HAVE_MONGOC */

