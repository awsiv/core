/*****************************************************************************/
/*                                                                           */
/* File: db_save.c                                                           */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report saving.                                  */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC

/*****************************************************************************/

int CFDB_Open(mongo_connection *conn, char *host, int port)

{
 mongo_connection_options connOpts;
 int result;

 snprintf(connOpts.host, sizeof(connOpts.host), "%s", host);
 connOpts.port = port;

 result = mongo_connect(conn,&connOpts);

 if (result != 0)
    {
    CfOut(cf_verbose, "mongo_connect", "!! Could not connect to mongo server (got %d)", result);
    return false;
    }

 return true;
}

/*****************************************************************************/

int CFDB_Close(mongo_connection *conn)

{
if (mongo_destroy(conn) != 0)
   {
   CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from mongo server");
   return false;
   }

return true;
}

/*****************************************************************************/

void CFDB_Initialize()
    
{
 //  unused
}

/*****************************************************************************/
/* Cache / scratch space                                                     */
/*****************************************************************************/

int CFDB_PutValue(char *lval,char *rval)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson setOp,empty;
  mongo_connection dbconn;

if (!IsDefinedClass("am_policy_hub") && !IsDefinedClass("am_php_module"))
   {
   CfOut(cf_verbose,"","Ignoring DB put of (%s=%s) - we are not a policy server",lval,rval);
   return false;
   }
  
if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"","!! Could not open connection to report database to put value %s",lval);
   return false;
   }

  
bson_buffer_init(&bb);
setObj = bson_append_start_object(&bb, "$set");
bson_append_string(setObj,lval,rval);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(&dbconn,MONGO_SCRATCH,bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);

bson_destroy(&setOp);
CFDB_Close(&dbconn);

return true;
}

/*****************************************************************************/
/* Monitor data                                                              */
/*****************************************************************************/

void CFDB_SaveHostID(mongo_connection *conn, char *database, char *keyhash,char *ipaddr)

{ bson_buffer bb;
 bson_buffer *setObj, *arr;
 bson host_key;  // host description
 bson setOp;
 
// locate right host key

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,keyhash);
bson_from_buffer(&host_key,&bb);

// ip address - replace array with one el (more later - aging..)

bson_buffer_init(&bb);
setObj = bson_append_start_object(&bb,"$set");
arr = bson_append_start_array(setObj,cfr_ip_array);
bson_append_string(setObj,"0",ipaddr);
bson_append_finish_object(arr);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, database,&host_key,&setOp,MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveHostID",keyhash);

bson_destroy(&setOp);

// host name

bson_buffer_init(&bb);
setObj = bson_append_start_object(&bb,"$set");
arr = bson_append_start_array(setObj,cfr_host_array);
bson_append_string(setObj,"0",IPString2Hostname(ipaddr));
bson_append_finish_object(arr);
bson_append_finish_object(setObj);

bson_from_buffer(&setOp,&bb);
mongo_update(conn, database,&host_key,&setOp,MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveHostID",keyhash);

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
  struct Item *ip;
  int observable,slot;
  double q,e,dev;
  char *dbOperation = {0};
  char t[CF_TIME_SIZE];
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

static void CFDB_PutEnvironmentForHost(mongo_connection *conn, const char *keyhash,
                                       const char *environment)
{
    bson_buffer bb;
    bson host_key, op;

    /* { kH: $keyhash } */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    if (environment)
    {
        bson_buffer *env_sub;

        /* { $set: { env: $environment } } */
        bson_buffer_init(&bb);
        env_sub = bson_append_start_object(&bb, "$set");
        bson_append_string(env_sub, cfr_environment, environment);
        bson_append_finish_object(env_sub);
        bson_from_buffer(&op, &bb);
    }
    else
    {
        bson_buffer *env_sub;

        /* { $unset: { env: 1 } } */
        bson_buffer_init(&bb);
        env_sub = bson_append_start_object(&bb, "$unset");
        bson_append_string(env_sub, cfr_environment, "1");
        bson_append_finish_object(env_sub);
        bson_from_buffer(&op, &bb);
    }

    mongo_update(conn, MONGO_DATABASE, &host_key, &op, MONGO_UPDATE_UPSERT);

    MongoCheckForError(conn, "PutEnvironmentForHost", keyhash);

    bson_destroy(&op);
    bson_destroy(&host_key);
}

#define ENV_NAME_PREFIX "environment_"
#define ENV_NAME_LEN (sizeof(ENV_NAME_PREFIX) / sizeof(char) - 1)

static void CFDB_SaveEnvironment(mongo_connection *conn, const char *keyhash,
                                 const struct Item *data)
{
    const struct Item *i;
    char *environment = NULL;

    for (i = data; i; i = i->next)
    {
        char classname[CF_MAXVARSIZE] = "";
        sscanf(i->name, "%[^,]", classname);

        if (!strncmp(ENV_NAME_PREFIX, classname, ENV_NAME_LEN)
            && strlen(classname) > ENV_NAME_LEN)
        {
            if (environment)
            {
                /* FIXME: warn user */
                free(environment);
            }
            environment = strdup(classname + ENV_NAME_LEN);
        }
    }

    CFDB_PutEnvironmentForHost(conn, keyhash, environment);

    free(environment);
}

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
   sscanf(ip->name,"%[^,],%ld,%lf,%lf\n",name,(time_t *)&t,&e,&dev);   
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

CFDB_SaveEnvironment(conn, keyhash, data);

bson_destroy(&setOp);
bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveVariables(mongo_connection *conn, char *keyhash, struct Item *data)

/* Should be deprecated some time - was replaced after Nova 2.0.2 */

{ bson_buffer bb;
  bson_buffer *unset, *setObj, *arr;
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

bson_from_buffer(&setOp,&bb);
mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn,"SaveVariables",keyhash);

bson_destroy(&setOp);
bson_destroy(&host_key);  
}

/*****************************************************************************/

void CFDB_SaveVariables2(mongo_connection *conn, char *keyhash, struct Item *data)

{ bson_buffer bb;
  bson_buffer *setObj, *arr;
  bson host_key;  // host description
  bson setOp;
  struct Item *ip;
  int i;
  char iStr[32];
  long tl;
  time_t t;
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
   if (strncmp(ip->name,"S:", 2) == 0)
      {
      scope[0] = '\0';
      sscanf(ip->name+2,"%254[^\n]",scope);
      continue;
      }
   
   sscanf(ip->name,"%4[^,],%ld,%255[^,],%2040[^\n]",type,&tl,lval,rval);

   t = (time_t)tl;
   
   if (strchr(lval,'/'))
      {
      // Private data not meant to be seen
      continue;
      }
   
   snprintf(varName, sizeof(varName),"%s.%s.%s.%s",cfr_vars,scope,lval,cfr_type);
   bson_append_string(setObj, varName, type);

   snprintf(varName, sizeof(varName),"%s.%s.%s.%s",cfr_vars,scope,lval,cfr_time);
   bson_append_int(setObj, varName, t);
   
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
MongoCheckForError(conn,"SaveVariables2",keyhash);

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
  bson_buffer *setObj;
  bson setOp;
  struct Item *ip;
  char timekey[CF_SMALLBUF];
  char handle[CF_MAXVARSIZE],reason[CF_BUFSIZE];
  char *collName;
  char *dbOp = NULL;
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
 MongoCheckForError(conn,dbOp,keyhash);
 
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
  bson_buffer *keyArr, *set;
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
   snprintf(varName,sizeof(varName),"%s.%s@%ld",cfr_filechanges,nameNoDot,date);
   
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
   snprintf(varName, sizeof(varName),"%s.%s@%ld",cfr_filediffs,nameNoDot,then);
   
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
  char varName[CF_MAXVARSIZE];
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
  char datestr[CF_SMALLBUF];
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

void CFDB_SaveCachedTotalCompliance(mongo_connection *conn, char *policy, int slot, double kept, double repaired, double notkept, int count, time_t genTime)

{ bson_buffer bb;
  bson_buffer *set;
  bson cacheType, setOp;
  bson_buffer *sub1;
  char varName[CF_SMALLBUF];

bson_buffer_init(&bb);
bson_append_string(&bb,cfc_cachetype,cfc_cachecompliance);
bson_from_buffer(&cacheType,&bb);

  
bson_buffer_init(&bb);

set = bson_append_start_object(&bb,"$set");

snprintf(varName,sizeof(varName), "%s.%d", policy, slot);

sub1 = bson_append_start_object(set,varName);

bson_append_double(sub1,cfr_kept,kept);
bson_append_double(sub1,cfr_repaired,repaired);
bson_append_double(sub1,cfr_notkept,notkept);
bson_append_int(sub1,cfc_count,count);
bson_append_int(sub1,cfc_timegen,genTime);

bson_append_finish_object(sub1);
bson_append_finish_object(set);

bson_from_buffer(&setOp,&bb);

mongo_update(conn, MONGO_CACHE, &cacheType, &setOp, MONGO_UPDATE_UPSERT);
MongoCheckForError(conn, "SaveCachedTotalCompliance", policy);

bson_destroy(&setOp);
bson_destroy(&cacheType);  
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

int CFDB_AddNote(mongo_connection *conn, char *keyhash, char *nid, char *reportData, struct Item *data)
{
  bson_buffer bb;
  bson host_key;
  bson_buffer *setObj, *sub;
  bson setOp;
  bson b_key;
  bson_buffer buf_key;   
  char username[CF_MAXVARSIZE];
  char msg[CF_BUFSIZE];
  long datetime;
  
  int options = MONGO_INDEX_UNIQUE |  MONGO_INDEX_DROP_DUPS;

  /* for getting object id */
  mongo_cursor *cursor;
  bson_iterator it1;
  bson field;
  char objectId[CF_MAXVARSIZE] = {0};
  bson_oid_t bsonid;
  int found = false, newnote = false, retval=true;
   
  // Add index, TODO: must do this while creating the database
  
  bson_buffer_init( &buf_key );
  bson_append_int( &buf_key, cfn_keyhash, 1 );
  bson_append_int(&buf_key,cfn_reportdata,1);
  bson_from_buffer( &b_key, &buf_key );
  mongo_create_index(conn, MONGO_NOTEBOOK, &b_key, options, NULL);
  MongoCheckForError(conn,"CreateIndex",keyhash);
  bson_destroy(&b_key);
  
   // find right host
   bson_buffer_init(&bb);

   if(!EMPTY(nid))
      {
      bson_oid_from_string(&bsonid,nid);
      bson_append_oid(&bb,"_id",&bsonid);
      }
   else
      {
      newnote = true;
      bson_append_new_oid(&bb,"_id");
      bson_append_string(&bb,cfn_keyhash,keyhash);
      
      if (!EMPTY(reportData))
	 {
         bson_append_string(&bb,cfn_reportdata,reportData);
	 }
      }   
   bson_from_buffer(&host_key, &bb);
  
   sscanf(data->name,"%255[^,],%255[^,],%ld\n",username,msg,&datetime);
   
   EscapeChar(msg, sizeof(msg),'\\');
   EscapeChar(msg, sizeof(msg),'\"');

   bson_buffer_init(&bb);
   setObj = bson_append_start_object(&bb, "$addToSet");
   sub = bson_append_start_object(setObj, cfn_note);
   bson_append_string(sub,cfn_username,username);
   bson_append_string(sub,cfn_message,msg);
   bson_append_int(sub,cfn_datetime,datetime);
   bson_append_finish_object(sub);   
   bson_append_finish_object(setObj);
   bson_from_buffer(&setOp,&bb);

   mongo_update(conn, MONGO_NOTEBOOK, &host_key, &setOp, MONGO_UPDATE_UPSERT);
   MongoCheckForError(conn,"AddComments",keyhash);
   bson_destroy(&setOp);
  
   if(newnote)
     {
       // get the objectid                                                                                                                                                                         
       bson_buffer_init(&bb);
       bson_append_int(&bb,"_id",1);
       bson_from_buffer(&field, &bb);
       
       cursor = mongo_find(conn, MONGO_NOTEBOOK, &host_key, &field,0,0,0);
       MongoCheckForError(conn,"GetCommentID",keyhash);
       bson_destroy(&field);
       
       while(mongo_cursor_next(cursor) && !found)
	 {
	   bson_iterator_init(&it1,cursor->current.data);
	   objectId[0] = '\0';
	   while (bson_iterator_next(&it1))
	     {	  
	       switch(bson_iterator_type(&it1))
		 {       
		 case bson_oid:
		   if(strcmp(bson_iterator_key(&it1),"_id") == 0)
		     {
		       bson_oid_to_string(bson_iterator_oid(&it1),objectId);
		       found = true;
		     }
		   break;
		 default:
		   break;
		 }
	     }
	 }
       if(!found)
	 {
	   CfOut(cf_verbose, "", "!! Unable to add note");
	   retval=false;
	 }
       snprintf(nid,CF_MAXVARSIZE,"%s",objectId);   
     }
   bson_destroy(&host_key);
   return retval;
}

/*****************************************************************************/

static time_t rev_ctime(char *str_time)

{ struct tm tm;
  char buf[255];

snprintf(buf,sizeof(buf),"%s",str_time);
memset(&tm, 0, sizeof(tm));
strptime(buf, "%A %b %d %H:%M:%S %Y", &tm);
return mktime(&tm);
}

/*****************************************************************************/

void CFDBRef_AddToRow(mongo_connection *conn, char *coll,bson *query, char *row_name, char *nid)

{ bson_buffer bb;
  bson_buffer *setObj;
  bson setOp;
  char varName[CF_MAXVARSIZE];

  bson_buffer_init(&bb);
  setObj = bson_append_start_object(&bb, "$set");
  snprintf(varName, sizeof(varName), "%s",row_name);  
  bson_append_string(setObj, varName, nid);
  bson_append_finish_object(setObj);
  bson_from_buffer(&setOp,&bb);
  
  mongo_update(conn, coll, query, &setOp, 0);
  MongoCheckForError(conn,row_name,nid);
  bson_destroy(&setOp);
}

/*****************************************************************************/

#endif  /* HAVE_LIBMONGOC */

