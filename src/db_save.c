/*****************************************************************************/
/*                                                                           */
/* File: db_save.c                                                           */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report save and query.                          */
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

int Nova_DBOpen(mongo_connection *conn, char *host, int port)

{ mongo_connection_options connOpts;

snprintf(connOpts.host, sizeof(connOpts.host), "%s", host);
connOpts.port = port;

if (mongo_connect(conn,&connOpts) != 0)
   {
   CfOut(cf_error, "mongo_connect", "!! Could not connect to database");
   return false;
   }

return true;
}

/*****************************************************************************/

int Nova_DBClose(mongo_connection *conn)

{
if (mongo_destroy(conn) != 0)
   {
   CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from database");
   return false;
   }

return true;
}

/*****************************************************************************/

void Nova_DBInitialize()
    
{
 //  make sure indices on cfr_keyhash and sw.n exist and monitord arrays exist
}

/*****************************************************************************/

void Nova_SaveHostID(mongo_connection *conn,char *keyhash,char *ipaddr)

{ bson_buffer bb;
  bson_buffer *setObj, *clObj, *keyArr, *keyAdd, *keyArrField;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char name[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
  time_t t;
  double e = 0, dev = 0;
  char iStr[32];
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash,keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

// insert keys into numbered key array

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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveSoftware(mongo_connection *conn,enum software_rep sw, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson cond;  // host description
  bson setOp;
  int i;
  bson_buffer *arr;
  struct Item *ip;
  char packNumStr[CF_MAXVARSIZE];
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch,archStr[CF_MAXVARSIZE];

// find right host

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb,"$set");

switch (sw)
   {
   case sw_rep_installed:       
       arr = bson_append_start_array(setObj,cfr_software);
       break;
   case sw_rep_patch_installed:       
       arr = bson_append_start_array(setObj,cfr_patch_installed);
       break;
   case sw_rep_patch_avail:       
       arr = bson_append_start_array(setObj,cfr_patch_avail);
       break;

   default:
       arr = bson_append_start_array(setObj,cfr_software);
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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveMonitorData(mongo_connection *conn, char *keyhash, enum monitord_rep rep_type, struct Item *data)

{ bson_buffer bb;
  bson cond;  // host description
  char varNameIndex[64];
  bson_buffer *setObj;
  bson setOp;
  int i, arrIndex;
  struct Item *ip;
  int observable,slot;
  double q,e,dev;
  char *repPrefix = {0};
  char t[CF_TIME_SIZE];
  char timekey[CF_SMALLBUF];

switch(rep_type)
   {
   case mon_rep_mag:
       repPrefix = cfr_mag;
       break;
   case mon_rep_week:
       repPrefix = cfr_week;
       break;
   case mon_rep_yr:
       repPrefix = cfr_yr;
       break;
   default:
       CfOut(cf_error, "", "!! Unknown monitord report type (%d)", rep_type);
       FatalError("Software Error");
   }

// find right host

bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");
 
for (ip = data; ip != NULL; ip=ip->next)
   {
   // extract timestamp
   if (strncmp(ip->name,"T: ", 3) == 0)
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
   
   snprintf(varNameIndex, sizeof(varNameIndex),"%s%d.%c.%d",repPrefix,observable,cfr_obs_q, slot);
   bson_append_double(setObj, varNameIndex, q);
   snprintf(varNameIndex, sizeof(varNameIndex),"%s%d.%c.%d",repPrefix,observable,cfr_obs_E, slot);
   bson_append_double(setObj, varNameIndex, e);
   snprintf(varNameIndex, sizeof(varNameIndex),"%s%d.%c.%d",repPrefix,observable,cfr_obs_sigma,slot);
   bson_append_double(setObj, varNameIndex, dev);
   }
  
bson_append_finish_object(setObj);
bson_from_buffer(&setOp,&bb);

mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveMonitorHistograms(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *arr;
  bson cond;  // host description
  bson setOp;
  char arrName[64], kStr[32];
  struct Item *ip;
  int i,k, currHist;
  char *sp;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

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
      sscanf(sp,"%d",&currHist);
      
      while (*(++sp) != ':')
         {
         }

      if (currHist < 0)
         {
         currHist = 1;
         }

      sp++;
      snprintf(kStr, sizeof(kStr),"%d",k);
      bson_append_int(arr,kStr,currHist);
      }

   bson_append_finish_object(arr);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn,MONGO_DATABASE,&cond,&setOp,MONGO_UPDATE_UPSERT);
 
bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveClasses(mongo_connection *conn, char *keyhash, struct Item *data)

/**
 *  Replacing existing class entry, but not deleting "old" entries (purging)
 */

{ bson_buffer bb;
  bson_buffer *setObj, *clObj, *keyArr, *keyAdd, *keyArrField;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

// insert objects

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%[^,],%ld,%7.4lf,%7.4lf\n",name,&t,&e,&dev);   
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
keyArrField = bson_append_start_object(keyAdd , cfr_class_keys);
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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveVariables(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *varObj, *keyArr, *keyAdd, *keyArrField, *arr;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

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
   
   snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars,scope,lval,cfr_type);
   bson_append_string(setObj, varName, type);
   
   snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars,scope,lval,cfr_rval);
   
   // parse lists into an array
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
      bson_append_string(setObj, varName,rval);
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
   
   bson_append_string(keyArr, iStr, varName);
   i++;
   }

bson_append_finish_object(keyArr);
bson_append_finish_object(keyArrField);
bson_append_finish_object(keyAdd);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSaveTotalCompliance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  bson_buffer *arr;
  char iStr[32];
  char then[CF_SMALLBUF],version[CF_SMALLBUF];
  int kept,repaired,notrepaired;
  bson_buffer *sub;
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb, "$pushAll");

arr = bson_append_start_array(pushObj,cfr_total_compliance);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d", i);
   
   sscanf(ip->name,"%63[^,], %127[^,],%d,%d,%d\n",then,version,&kept,&repaired,&notrepaired);
   
   sub = bson_append_start_object(arr, iStr);
   bson_append_string(sub , cfr_time, then);
   bson_append_string(sub , cfr_version, version);
   bson_append_int(sub, cfr_kept, kept);
   bson_append_int(sub, cfr_repaired, repaired);
   bson_append_int(sub, cfr_notkept, notrepaired);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSavePromiseLog(mongo_connection *conn, char *keyhash, enum promiselog_rep rep_type, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson_buffer *arr, *sub;
  bson cond;  // host description
  bson setOp;
  char handle[CF_MAXVARSIZE];
  long then;
  time_t tthen;
  struct Item *ip;
  int i;
  char iStr[32];
  char *repName = {0};

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

switch(rep_type)
   {
   case plog_repaired:
       repName = cfr_repairlog;
       break;
   case plog_notkept:
       repName = cfr_notkeptlog;
       break;
   default:
       CfOut(cf_error, "", "!! Unknown promise log report type (%d)", rep_type);
       FatalError("Software Error");
   }

pushObj = bson_append_start_object(&bb, "$pushAll");
arr = bson_append_start_array(pushObj, repName);

for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
   {
   snprintf(iStr, sizeof(iStr), "%d", i);   
   sscanf(ip->name,"%ld,%127[^\n]",&then,handle);
   tthen = (time_t)then;
   
   sub = bson_append_start_object(arr, iStr);
   bson_append_string(sub, cfr_promisehandle, handle);
   bson_append_int(sub, cfr_time, then);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveLastSeen(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *sub;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char inout, ipaddr[CF_MAXVARSIZE],dns[CF_MAXVARSIZE];
  char hash[CF_MAXVARSIZE],varName[CF_MAXVARSIZE];
  double ago,average,dev;
  long fthen;
  time_t then;
 
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c %128s %25s %15s %ld %lf %lf %lf\n",
          &inout,
          hash,
          dns,
          ipaddr,
          &fthen,
          &ago,
          &average,
          &dev);
   
   then = (time_t)fthen;
   
   snprintf(varName, sizeof(varName), "%s.%c%s", cfr_lastseen, inout, hash);
   
   sub = bson_append_start_object(setObj, varName);
   bson_append_string(sub, cfr_dnsname, dns);
   bson_append_string(sub, cfr_ipaddr, ipaddr);
   bson_append_double(sub, cfr_hrsago, ago);
   bson_append_double(sub, cfr_hrsavg, average);
   bson_append_double(sub, cfr_hrsdev, dev);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSaveMeter(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  char type;
  double kept,repaired;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c: %lf %lf",&type,&kept,&repaired);
   snprintf(varName, sizeof(varName), "%s.%c", cfr_meter, type);
   
   sub = bson_append_start_object(setObj , varName);
   bson_append_double(sub, cfr_meterkept, kept);
   bson_append_double(sub, cfr_meterrepaired , repaired);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSavePerformance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char varName[CF_MAXVARSIZE];
  time_t t;
  char eventname[CF_MAXVARSIZE];
  double measure = 0,average = 0,dev = 0;

// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

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
   bson_append_int(sub, cfr_time, t);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveSetUid(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *keyArr, *keyAdd, *keyArrField;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char progName[CF_MAXVARSIZE];
  char iStr[32];
  int i;
  
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSavePromiseCompliance(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBSaveFileChanges(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb, "$pushAll");

arr = bson_append_start_array(pushObj , cfr_filechanges);

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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSaveFileDiffs(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

pushObj = bson_append_start_object(&bb, "$pushAll");

arr = bson_append_start_array(pushObj , cfr_filediffs);

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
   bson_append_string(sub,cfr_diff,name);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(arr);
bson_append_finish_object(pushObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSaveBundles(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *sub;
  bson cond;  // host description
  bson setOp;
  struct Item *ip;
  char bundle[CF_MAXVARSIZE];
  char hash[CF_MAXVARSIZE],varName[CF_MAXVARSIZE];
  double ago,average,dev;
  long fthen;
  time_t then;
 
// find right host
bson_buffer_init(&bb);
bson_append_string(&bb, cfr_keyhash, keyhash);
bson_from_buffer(&cond, &bb);

bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%25s %ld %lf %lf %lf\n",
          bundle,
          &fthen,
          &ago,
          &average,
          &dev);

   then = (time_t)fthen;
   
   snprintf(varName, sizeof(varName),"%s.%s",cfr_bundles,bundle);
   
   sub = bson_append_start_object(setObj, varName);
   bson_append_string(sub,cfr_name,bundle);
   bson_append_double(sub, cfr_hrsago, ago);
   bson_append_double(sub, cfr_hrsavg, average);
   bson_append_double(sub, cfr_hrsdev, dev);
   bson_append_finish_object(sub);
   }

bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}

/*****************************************************************************/

void Nova_DBSaveValue(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *pushObj;
  bson cond;  // host description
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
bson_from_buffer(&cond, &bb);

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
mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
bson_destroy(&cond);  
}


#endif  /* HAVE_MONGOC */

