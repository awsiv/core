/*****************************************************************************/
/*                                                                           */
/* File: db_save.c                                                           */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report saving.                                  */
/*                                                                           */
/*****************************************************************************/

// TODO: ensure indeces on cfr_keyhash, etc. ?


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
bson_append_string(setObj,lval,rval);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(&dbconn,MONGO_SCRATCH,bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);

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

// insert keys into numbered key array - needed for efficient regexes

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
  bson_buffer *unset, *setObj, *varObj, *keyArr, *keyAdd, *keyArrField, *arr;
  bson host_key;  // host description
  bson setOp,unsetOp;
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

// delete any old report first

unset = bson_append_start_object(&bb, "$unset");
bson_append_int(unset,cfr_vars,1);
bson_append_finish_object(unset);
bson_from_buffer(&unsetOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &unsetOp, 0);
MongoCheckForError(conn,"SaveVariables-DeleteOld",keyhash);
bson_destroy(&unsetOp);


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

// TODO: append scope.varname to key array more efficient ?

/*
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
*/

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveVariables",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveTotalCompliance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char version[CF_MAXVARSIZE];
  struct Item *keys = NULL,*addedKey = NULL;
  int kept,repaired,notrepaired;
  char varName[CF_MAXVARSIZE];
  long t;
  time_t then;
  bson_buffer *sub;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^,],%d,%d,%d\n",&t,version,&kept,&repaired,&notrepaired);
   then = (time_t)t;


   snprintf(varName,sizeof(varName),"%s.%s",cfr_total_compliance,GenTimeKey(then));


   // check for duplicate keys
   addedKey = ReturnItemIn(keys,varName);
   if(addedKey)
     {
     Debug("!! Duplicate key \"%s\" in SaveTotalCompliance - ignoring second time=%s - stored=%s", varName, cf_ctime(&then), addedKey->classes);
     continue; // avoids DB update failure
     }
   else
     {
     PrependItem(&keys,varName,cf_ctime(&then));
     }

   
   sub = bson_append_start_object(setObj, varName);
   bson_append_int(sub,cfr_time, then);
   bson_append_string(sub,cfr_version,version);
   bson_append_int(sub,cfr_kept,kept);
   bson_append_int(sub,cfr_repaired,repaired);
   bson_append_int(sub,cfr_notkept,notrepaired);
   bson_append_finish_object(sub);
   }

DeleteItemList(keys);

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveTotalCompliance",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SavePromiseLog(mongo_connection *conn, char *keyhash, enum promiselog_rep rep_type, struct Item *data)
{
  bson_buffer bb,record;
  bson host_key;  // host description
  char varNameIndex[64];
  bson_buffer *setObj;
  bson setOp;
  struct Item *ip;
  int observable,slot;
  char *dbOperation = {0};
  char timekey[CF_SMALLBUF];
  char handle[CF_MAXVARSIZE],reason[CF_BUFSIZE];
  char *collName;
  char *dbOp;
  long then;
  time_t tthen;
  
  
switch(rep_type)
   {
   case plog_repaired:
       collName = MONGO_LOGS_REPAIRED;
       dbOp = "update promise repaired";
       break;
   case plog_notkept:
       collName = MONGO_LOGS_NOTKEPT;
       dbOp = "update promise not kept";
       break;
   default:
       CfOut(cf_error, "", "!! Unknown promise log report type (%d)", rep_type);
       return;
   }

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%254[^,],%1024[^\n]",&then,handle,reason);
   tthen = (time_t)then;

   snprintf(timekey,sizeof(timekey),"%s",GenTimeKey(tthen));


   // update
   bson_buffer_init(&bb);
   setObj = bson_append_start_object(&bb, "$set");

   bson_append_string(setObj,cfr_cause,reason);
   bson_append_int(setObj,cfr_time,tthen);
   
   bson_append_finish_object(setObj);
   bson_from_buffer(&setOp,&bb);

   // find right host and report - key
   bson_buffer_init(&record);
   bson_append_string(&record,cfr_keyhash,keyhash);
   bson_append_string(&record,cfr_promisehandle,handle);
   bson_append_string(&record,cfr_timeslot,timekey);
   bson_from_buffer(&host_key, &record);

   mongo_update(conn, collName, &host_key, &setOp, MONGO_UPDATE_UPSERT);

   bson_destroy(&setOp);
   bson_destroy(&host_key);
   }

// should do this in loop, but not efficient...
 MongoCheckForError(conn,dbOperation,keyhash);
 
}


/*****************************************************************************/

void CFDB_SaveLastSeen(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *sub;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char inout, ipaddr[CF_MAXVARSIZE];
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
   sscanf(ip->name,"%c %625s %250s %ld %lf %lf %lf\n",
          &inout,
          hostkey,
          ipaddr,
          &fthen,
          &ago,
          &average,
          &dev);
   
   then = (time_t)fthen;

   snprintf(varName, sizeof(varName), "%s.%c%s", cfr_lastseen,inout,hostkey);

   sub = bson_append_start_object(setObj, varName);
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
  char eventname[CF_MAXVARSIZE],eventnameKey[CF_MAXVARSIZE];
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

   // Need to canonify the dots, as dot is not allowed in a mongo key
   ReplaceChar(eventname,eventnameKey,sizeof(eventnameKey),'.','_');
   
   snprintf(varName, sizeof(varName), "%s.%s",cfr_performance,eventnameKey);
   
   sub = bson_append_start_object(setObj , varName);
   bson_append_string(sub, cfr_perf_event, eventname);
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
  bson_buffer *keyArr, *set, *unset;
  bson host_key;  // host description
  bson setOp,unsetOp;
  struct Item *ip;
  char progName[CF_MAXVARSIZE];
  char iStr[32];
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

// old report is replaced

bson_buffer_init(&bb);

// insert keys into key array

set = bson_append_start_object(&bb , "$set");
keyArr = bson_append_start_array(set, cfr_setuid);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   sscanf(ip->name,"%255[^\n]",progName);
   snprintf(iStr, sizeof(iStr), "%d", i);
   bson_append_string(keyArr, iStr, progName);
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(set);

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
  bson_buffer *keyArr, *keyAdd, *keyArrField;
  struct Item *keys = NULL,*addedKey = NULL;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  char handle[CF_MAXVARSIZE];
  char status, statusStr[16];
  time_t then;
  double av,dev;
  int i;
  char iStr[32];

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
   
   addedKey = ReturnItemIn(keys,handle);
   if(addedKey)
     {
     Debug("!! Duplicate key \"%s\" in SavePromiseCompliance - ignoring second time=%s - stored=%s", varName, cf_ctime(&then), addedKey->classes);
     continue; // avoids DB update failure
     }
   else
     {
     PrependItem(&keys,handle,cf_ctime(&then));
     }

   sub = bson_append_start_object(setObj , varName);
   bson_append_string(sub, cfr_promisestatus, statusStr);
   bson_append_double(sub, cfr_obs_E, av);
   bson_append_double(sub, cfr_obs_sigma, dev);
   bson_append_int(sub, cfr_time, then);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

// insert keys into numbered key array

keyAdd = bson_append_start_object(&bb , "$addToSet");
keyArrField = bson_append_start_object(keyAdd,cfr_promisecompl_keys);
keyArr = bson_append_start_array(keyAdd , "$each");

for (ip = keys, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr,sizeof(iStr),"%d",i);
   bson_append_string(keyArr,iStr,ip->name);
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

DeleteItemList(keys);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SavePromiseCompliance",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileChanges(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char name[CF_SMALLBUF],nameNoDot[CF_SMALLBUF];
  char varName[128];
  bson_buffer *sub;
  time_t then;
  long date;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb,"$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%255[^\n]",&date,name);
   then = (time_t)date;

   ReplaceChar(name,nameNoDot,sizeof(nameNoDot),'.','_');
   snprintf(varName,sizeof(varName),"%s.%s@%d",cfr_filechanges,nameNoDot,date);
   
   sub = bson_append_start_object(setObj,varName);
   bson_append_int(sub,cfr_time,then);
   bson_append_string(sub,cfr_name,name);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveFileChanges",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveFileDiffs(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  char name[CF_MAXVARSIZE],change[CF_BUFSIZE],nameNoDot[CF_MAXVARSIZE];
  char varName[128];
  bson_buffer *sub;
  time_t then;
  long t;
  char *sp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
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

   ReplaceChar(name,nameNoDot,sizeof(nameNoDot),'.','_');
   snprintf(varName, sizeof(varName),"%s.%s@%d",cfr_filediffs,nameNoDot,t);
   
   sub = bson_append_start_object(setObj, varName);
   bson_append_int(sub,cfr_time,then);
   bson_append_string(sub,cfr_name,name);
   bson_append_string(sub,cfr_diff,change);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

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
  {// TODO: ISBUG %250s?
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
  bson_buffer *set;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *sub1, *sub2;
  char name[CF_SMALLBUF],datestr[CF_SMALLBUF];
  double kept,notkept,repaired;

  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&host_key, &bb);

bson_buffer_init(&bb);

set = bson_append_start_object(&bb,"$set");

sub1 = bson_append_start_object(set,cfr_valuereport);

for (ip = data; ip != NULL; ip=ip->next)
   {

   sscanf(ip->name,"%100[^,],%lf,%lf,%lf\n",datestr,&kept,&repaired,&notkept);

   sub2 = bson_append_start_object(sub1,datestr);
   bson_append_string(sub2,cfr_day,datestr);
   bson_append_double(sub2,cfr_kept,kept);
   bson_append_double(sub2,cfr_repaired,repaired);
   bson_append_double(sub2,cfr_notkept,notkept);

   bson_append_finish_object(sub2);
   }

bson_append_finish_object(sub1);
bson_append_finish_object(set);

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

/*
 * Commenting
 */
/*****************************************************************************/

void CFDB_AddComment(mongo_connection *conn, char *keyhash, char *subkey, char *handle,struct Item *data)
{
  bson_buffer bb,record;
  bson host_key;
  bson_buffer *setObj, *sub;
  bson setOp;
   
  char timekey[CF_SMALLBUF];
  char username[CF_SMALLBUF];
  char msg[CF_BUFSIZE];
  long datetime;
  
   bson b_key;
   bson_buffer buf_key;
   int options = MONGO_INDEX_UNIQUE |  MONGO_INDEX_DROP_DUPS;
   char *collection = "cfreport.comments";
   int count=0;

   // Add index, TODO: must do this while creating the database
   bson_buffer_init( &buf_key );
   bson_append_int( &buf_key, cfr_keyhash, 1 );
   bson_append_int(&buf_key,cfc_subkey,1); 
   bson_append_int(&buf_key,cfc_handle,1); 
   bson_from_buffer( &b_key, &buf_key );
   mongo_create_index(conn, collection, &b_key, options, NULL);
   MongoCheckForError(conn,"CreateIndex",keyhash);
   bson_destroy(&b_key);

   // find right host
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,keyhash);
   bson_append_string(&bb,cfc_subkey,subkey);
   bson_append_string(&bb,cfc_handle,handle);
   bson_from_buffer(&host_key, &bb);
   bson_print(&host_key);

   bson_buffer_init(&bb);
   setObj = bson_append_start_object(&bb, "$addToSet");
   sscanf(data->name,"%255[^,],%255[^,],%ld\n",username,msg,&datetime);
   sub = bson_append_start_object(setObj, cfc_comment);
   bson_append_string(sub,cfc_username,username);
   bson_append_string(sub,cfc_message,msg);
   bson_append_int(sub,cfc_datetime,datetime);
   bson_append_finish_object(sub);

   bson_append_finish_object(setObj);
   bson_from_buffer(&setOp,&bb);
   bson_print(&setOp);
   mongo_update(conn, collection, &host_key, &setOp, MONGO_UPDATE_UPSERT);
   bson_destroy(&host_key);
   bson_destroy(&setOp);
 MongoCheckForError(conn,"AddComments",keyhash);
}

#endif  /* HAVE_MONGOC */

