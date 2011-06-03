/*****************************************************************************/
/*                                                                           */
/* File: db_maintain.c                                                       */
/*                                                                           */
/* Created: Wed Sept  29 14:47:33 2010                                       */
/*                                                                           */
/* MongoDB implementation of database maintenance functions                  */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


/*****************************************************************************/

void CFDB_Maintenance(void)
{
#ifdef HAVE_LIBMONGOC

  mongo_connection dbconn;


  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_error,"", "!! Could not open connection to report database on maintain");
      return;
    }

  CFDB_EnsureIndeces(&dbconn);
  CFDB_PurgeTimestampedReports(&dbconn);
  CFDB_PurgePromiseLogs(&dbconn,CF_HUB_PURGESECS,time(NULL));
  CFDB_Close(&dbconn);

#endif  /* HAVE_LIBMONGOC */
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC


void CFDB_EnsureIndeces(mongo_connection *conn)
/**
 *  Makes sure certain keys have an index to optimize querying and updating.
 **/
{
  bson_buffer bb;
  bson b;

  // main host collection

  bson_buffer_init(&bb);
  bson_append_int(&bb, cfr_keyhash, 1);
  bson_from_buffer(&b, &bb);

  if(!mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL))
    {
      CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE);
    }

  bson_destroy(&b);

  
  // log collections
  bson_buffer_init(&bb);
  bson_append_int(&bb, cfr_keyhash, 1);
  bson_append_int(&bb, cfr_promisehandle, 1);
  bson_append_int(&bb, cfr_timeslot, 1);
  bson_from_buffer(&b, &bb);

  if(!mongo_create_index(conn, MONGO_LOGS_REPAIRED, &b, 0, NULL))
    {
      CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_LOGS_REPAIRED);
    }

  if(!mongo_create_index(conn, MONGO_LOGS_NOTKEPT, &b, 0, NULL))
    {
      CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_LOGS_NOTKEPT);
    }

  bson_destroy(&b);
  
}

/*****************************************************************************/

void CFDB_PurgeTimestampedReports(mongo_connection *conn)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
  struct Item *purgeKeys = NULL, *ip;
  struct Item *purgePcNames = NULL, *purgeClassNames = NULL;
  mongo_cursor *cursor;
  bson query,field,hostQuery,op;
  bson_iterator it1;
  bson_buffer bb,*unset;
  char keyHash[CF_MAXVARSIZE];
  time_t now;
  
  CfOut(cf_verbose,"", " -> Purge timestamped reports");
  
  // query all hosts
  bson_empty(&query);
  
  // only retrieve the purgable reports
  bson_buffer_init(&bb);
  bson_append_int(&bb,cfr_keyhash,1);
  bson_append_int(&bb,cfr_class,1);
  bson_append_int(&bb,cfr_vars,1);
  bson_append_int(&bb,cfr_performance,1);
  bson_append_int(&bb,cfr_filechanges,1);
  bson_append_int(&bb,cfr_filediffs,1);
  bson_append_int(&bb,cfr_promisecompl,1);
  bson_append_int(&bb,cfr_lastseen,1);
  bson_append_int(&bb,cfr_bundles,1);
  bson_append_int(&bb,cfr_valuereport,1);
  bson_from_buffer(&field, &bb);

  cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
  bson_destroy(&field);

  now = time(NULL);

  while(mongo_cursor_next(cursor))  // iterate over docs
    {
      bson_iterator_init(&it1,cursor->current.data);

      memset(keyHash,0,sizeof(keyHash));
      
      while(bson_iterator_next(&it1))
	{

	  if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
	    {
	    snprintf(keyHash,sizeof(keyHash),"%s",bson_iterator_string(&it1));
	    }


	  CFDB_PurgeScan(conn,&it1,cfr_class,CF_HUB_PURGESECS,now,&purgeKeys,&purgeClassNames);
	  CFDB_PurgeScan(conn,&it1,cfr_vars,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_performance,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_filechanges,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_filediffs,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_promisecompl,CF_HUB_PURGESECS,now,&purgeKeys,&purgePcNames);
	  CFDB_PurgeScan(conn,&it1,cfr_lastseen,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_bundles,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScanStrTime(conn,&it1,cfr_valuereport,CF_HUB_PURGESECS,now,&purgeKeys);
	}

      bson_buffer_init(&bb);
      bson_append_string(&bb,cfr_keyhash,keyHash);
      bson_from_buffer(&hostQuery,&bb);

      // keys
      bson_buffer_init(&bb);
      unset = bson_append_start_object(&bb, "$unset");

      for (ip = purgeKeys; ip != NULL; ip=ip->next)
	{
	bson_append_int(unset, ip->name, 1);
	}

      bson_append_finish_object(unset);


      // key array elements
      DeleteFromBsonArray(&bb,cfr_class_keys,purgeClassNames);
      DeleteFromBsonArray(&bb,cfr_promisecompl_keys,purgePcNames);
      

      bson_from_buffer(&op,&bb);

      mongo_update(conn,MONGO_DATABASE,&hostQuery, &op, 0);
      MongoCheckForError(conn,"PurgeTimestampedReports",keyHash,NULL);
      
      DeleteItemList(purgeClassNames);
      purgeClassNames = NULL;

      DeleteItemList(purgePcNames);
      purgePcNames = NULL;

      DeleteItemList(purgeKeys);
      purgeKeys = NULL;

      bson_destroy(&hostQuery);
      bson_destroy(&op);
    }
  

  mongo_cursor_destroy(cursor);  
}

/*****************************************************************************/

void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now)
/**
 * Deletes old repair and not kept log entries.
 **/
{
  bson_buffer bb, *sub;
  time_t oldStamp;
  bson cond;

  oldStamp = now - oldThreshold;

  bson_buffer_init(&bb);

  sub = bson_append_start_object(&bb,cfr_time);
  bson_append_int(sub, "$lte", oldStamp);
  bson_append_finish_object(sub);

  bson_from_buffer(&cond, &bb);

  mongo_remove(conn, MONGO_LOGS_REPAIRED, &cond);

  MongoCheckForError(conn,"timed delete host from repair logs collection",NULL,NULL);


  mongo_remove(conn, MONGO_LOGS_NOTKEPT, &cond);

  MongoCheckForError(conn,"timed delete host from not kept logs collection",NULL,NULL);


  bson_destroy(&cond);
  
}

/*****************************************************************************/

void CFDB_PurgeDropReports(mongo_connection *conn)
/**
 *  Remove certain reports completely.
 *  UNUSED - currently overwritten on save.
 **/
{
  bson_buffer bb, *unset;
  bson empty,op;
  char *DROP_REPORTS[] = { cfr_setuid, cfr_vars, NULL };
  int i;
  
  CfOut(cf_verbose,"", " -> Purge droppable reports");


  // query all hosts
  bson_empty(&empty);

  
  // define reports to drop (unset)
  bson_buffer_init(&bb);
  unset = bson_append_start_object(&bb, "$unset");
  for (i = 0; DROP_REPORTS[i] != NULL; i++)
    {
    bson_append_int(unset, DROP_REPORTS[i], 1);
    }
  bson_append_finish_object(unset);
  bson_from_buffer(&op,&bb);  


  // run update
  mongo_update(conn,MONGO_DATABASE,&empty,&op,MONGO_UPDATE_MULTI);
  MongoCheckForError(conn,"PurgeDropReports",NULL,NULL);
    
  bson_destroy(&op);
}

/*****************************************************************************/

void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now, struct Item **purgeKeysPtr, struct Item **purgeNamesPtr)

{
  bson_iterator it1,it2,it3;
  char var[CF_MAXVARSIZE], key[CF_MAXVARSIZE];
  int deep;
  int foundStamp;
  int emptyLev2 = true;


  if (strcmp(bson_iterator_key(itp), reportKey) != 0)
    {
    return;
    }

  // some reports have time deeper into the db structure
  if(strcmp(reportKey, cfr_vars) == 0)
    {
      deep = true;
    }
  else
    {
      deep = false;
    }

  bson_iterator_init(&it1,bson_iterator_value(itp));

  while (bson_iterator_next(&it1))
    {
      bson_iterator_init(&it2,bson_iterator_value(&it1));

      emptyLev2 = true;

      while (bson_iterator_next(&it2))
	{
	  emptyLev2 = false;
	  
	  if(deep) // one level extra
	    {
	      bson_iterator_init(&it3,bson_iterator_value(&it2));

	      snprintf(var,sizeof(var),"%s.%s.%s",reportKey,bson_iterator_key(&it1),bson_iterator_key(&it2));
	      snprintf(key,sizeof(key),"%s",(char *)bson_iterator_key(&it2));
	      foundStamp = false;

	      while(bson_iterator_next(&it3))
		{
		foundStamp = CFDB_CheckAge(var,key,&it3,now,oldThreshold,purgeKeysPtr,purgeNamesPtr);

		if(foundStamp)
		  {
		    break;
		  }
		}

	      if(!foundStamp)  // remove keys with missing timestamps
		{
		  PrependItem(purgeKeysPtr,var,NULL);
		  if(purgeNamesPtr)
		    {
		    PrependItem(purgeNamesPtr,key,NULL);
		    }
		}
	  
	    }
	  else  // not deep
	    {
	      snprintf(var,sizeof(var),"%s.%s",reportKey,bson_iterator_key(&it1));
	      snprintf(key,sizeof(key),"%s",(char *)bson_iterator_key(&it1));
	      CFDB_CheckAge(var,key,&it2,now,oldThreshold,purgeKeysPtr,purgeNamesPtr);

	    }
	}

      if(emptyLev2)
	{
	snprintf(var,sizeof(var),"%s.%s",reportKey,bson_iterator_key(&it1));
	PrependItem(purgeKeysPtr,var,NULL);
	}

    }  
}

/*****************************************************************************/

int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, struct Item **purgeKeysPtr, struct Item **purgeNamesPtr)
{
  time_t then;
  int found;
  
  if(strcmp(bson_iterator_key(it),cfr_time) == 0)
    {
      found = true;

      then = bson_iterator_int(it);
		  
      if(now - then >= oldThreshold)  // definition of old
	{
	  PrependItem(purgeKeysPtr,var,NULL);
		      
	  if(purgeNamesPtr)
	    {
	      PrependItem(purgeNamesPtr,key,NULL);
	    }
		      
	  Debug("Report key \"%s\" needs to be purged (%lu seconds old)\n", var, now - then);
	}

    }
  else
    {
      found = false;
    }
  
  return found;
}

/*****************************************************************************/

void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now, struct Item **purgeKeysPtr)
/**
 * Like PurgeScan but uses time in format "30 Sep 2010" instead of time_t.
 */
{
  bson_iterator it1,it2;
  char thenStr[CF_SMALLBUF];
  char purgeKey[CF_MAXVARSIZE];
  char earliest[CF_SMALLBUF];

  if (strcmp(bson_iterator_key(itp), reportKey) != 0)
    {
    return;
    }

  TimeToDateStr(now - oldThreshold,earliest,sizeof(earliest));

  bson_iterator_init(&it1,bson_iterator_value(itp));

  while (bson_iterator_next(&it1))
    {
      bson_iterator_init(&it2,bson_iterator_value(&it1));

      while (bson_iterator_next(&it2))
	{
	  if(strcmp(bson_iterator_key(&it2),cfr_time) == 0)
	    {
	      if(bson_iterator_type(&it2) != bson_string)
		{
		CfOut(cf_error, "", "!! Date is not string type in purge");
		continue;
		}

	      snprintf(thenStr,sizeof(thenStr),"%s",bson_iterator_string(&it2));

	      if(Nova_CoarseLaterThan(earliest,thenStr))  // definition of old
		{
		  snprintf(purgeKey,sizeof(purgeKey),"%s.%s",reportKey,bson_iterator_key(&it1));
		  PrependItem(purgeKeysPtr,purgeKey,NULL);
		  
		  Debug("Report key \"%s\" needs to be purged (date is %s)\n", purgeKey, thenStr);
		}

	    }
	}

    }
}

/*****************************************************************************/

void DeleteFromBsonArray(bson_buffer *bb, char *arrName, struct Item *elements)
{
  struct Item *ip = NULL;
  bson_buffer *pullAll, *arr;
  char iStr[64];
  int i;
  
  if(!elements)
    {
      return;
    }

  pullAll = bson_append_start_object(bb, "$pullAll");
  arr = bson_append_start_array(pullAll,arrName);

  for (ip = elements, i = 0; ip != NULL; ip=ip->next, i++)
    {
      snprintf(iStr,sizeof(iStr),"%d",i);
      bson_append_string(arr, iStr, ip->name);
    }

  bson_append_finish_object(arr);
  bson_append_finish_object(pullAll);


}

/*****************************************************************************/

void CFDB_PurgeHost(mongo_connection *conn, char *keyHash)
{
  bson_buffer bb;
  bson cond;

  bson_buffer_init(&bb);
  bson_append_string(&bb,cfr_keyhash,keyHash);
  bson_from_buffer(&cond,&bb);
  
  mongo_remove(conn, MONGO_DATABASE, &cond);

  MongoCheckForError(conn,"delete host from main collection",keyHash,NULL);

  
  mongo_remove(conn, MONGO_DATABASE_MON, &cond);

  MongoCheckForError(conn,"delete host from monitord collection",keyHash,NULL);


  mongo_remove(conn, MONGO_LOGS_REPAIRED, &cond);

  MongoCheckForError(conn,"delete host from repair logs collection",keyHash,NULL);


  mongo_remove(conn, MONGO_LOGS_NOTKEPT, &cond);

  MongoCheckForError(conn,"delete host from not kept logs collection",keyHash,NULL);


  bson_destroy(&cond);  
  
}

#endif  /* HAVE_LIBMONGOC */

