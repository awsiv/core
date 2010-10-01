/*****************************************************************************/
/*                                                                           */
/* File: db_purge.c                                                          */
/*                                                                           */
/* Created: Wed Sept  29 14:47:33 2010                                       */
/*                                                                           */
/* MongoDB implementation of report purging.                                 */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


void CFDB_PurgeReports(void)
{
#ifdef HAVE_LIBMONGOC

  mongo_connection dbconn;


  CfOut(cf_verbose,"","Purging mongo report database....");


  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database on purge");
      return;
    }

  
  CFDB_PurgeTimestampedReports(&dbconn);
  //CFDB_PurgeDropReports(&dbconn);  - rely on replace on update for now
  

  CFDB_Close(&dbconn);

#endif  /* HAVE_LIBMONGOC */
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

void CFDB_PurgeTimestampedReports(mongo_connection *conn)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
  struct Item *purgeKeys = NULL, *purgePcNames = NULL, *ip = NULL;
  mongo_cursor *cursor;
  bson query,field,hostQuery,op;
  bson_iterator it1;
  bson_buffer bb,*unset, *pullAll, *arr;
  char keyHash[CF_MAXVARSIZE];
  time_t now;
  char iStr[64];
  int i;
  
  CfOut(cf_verbose,"", " -> Purge timestamped reports");
  
  // query all hosts
  bson_empty(&query);
  
  // only retrieve the purgable reports
  bson_buffer_init(&bb);
  bson_append_int(&bb,cfr_keyhash,1);
  bson_append_int(&bb,cfr_class,1);
  bson_append_int(&bb,cfr_performance,1);
  bson_append_int(&bb,cfr_filechanges,1);
  bson_append_int(&bb,cfr_filediffs,1);
  bson_append_int(&bb,cfr_promisecompl,1);
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


	  CFDB_PurgeScan(conn,&it1,cfr_class,CF_HUB_HORIZON,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_performance,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_filechanges,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_filediffs,CF_HUB_PURGESECS,now,&purgeKeys,NULL);
	  CFDB_PurgeScan(conn,&it1,cfr_promisecompl,CF_HUB_PURGESECS,now,&purgeKeys,&purgePcNames);
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
      if(purgePcNames)  // cfr_promisecompl_keys array
	{
	pullAll = bson_append_start_object(&bb, "$pullAll");
	arr = bson_append_start_array(pullAll,cfr_promisecompl_keys);

	for (ip = purgePcNames, i = 0; ip != NULL; ip=ip->next, i++)
	  {
	  snprintf(iStr,sizeof(iStr),"%d",i);
	  bson_append_string(arr, iStr, ip->name);
	  }

	bson_append_finish_object(arr);
	bson_append_finish_object(pullAll);
	}
      

      bson_from_buffer(&op,&bb);

      mongo_update(conn,MONGO_DATABASE,&hostQuery, &op, 0);
      MongoCheckForError(conn,"PurgeTimestampedReports",keyHash);
      
      DeleteItemList(purgeKeys);
      purgeKeys = NULL;
      DeleteItemList(purgePcNames);
      purgePcNames = NULL;
      bson_destroy(&hostQuery);
      bson_destroy(&op);
    }
  

  mongo_cursor_destroy(cursor);  
}

/*****************************************************************************/

void CFDB_PurgeDropReports(mongo_connection *conn)
/**
 *  Remove certain reports completely.
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
  MongoCheckForError(conn,"PurgeDropReports",NULL);
    
  bson_destroy(&op);
}

/*****************************************************************************/

/*
void CFDB_PurgeScanClasses(mongo_connection *conn, bson_iterator *itp, time_t now, struct Item **purgeKeysPtr)

{
  bson_iterator it1,it2;
  time_t classTime;
  char purgeKey[CF_SMALLBUF];

  if (strcmp(bson_iterator_key(itp), cfr_class) != 0)
    {
    return;
    }

  bson_iterator_init(&it1,bson_iterator_value(itp));

  while (bson_iterator_next(&it1))
    {
      bson_iterator_init(&it2,bson_iterator_value(&it1));

      while (bson_iterator_next(&it2))
	{
	  if(strcmp(bson_iterator_key(&it2),cfr_time) == 0)
	    {
	      classTime = bson_iterator_int(&it2);
	      
	      if(now - classTime >= CF_HUB_HORIZON)
		{
		  // purge both the class object and the class key array

		  Debug("Class \"%s\" needs to be purged (%lu seconds old)\n", bson_iterator_key(&it1), now - classTime);

		  snprintf(purgeKey,sizeof(purgeKey),"%s.%s",cfr_class,bson_iterator_key(&it1));
		  PrependItem(purgeKeysPtr,purgeKey,NULL);

		}

	    }
	}
    }

}
*/

/*****************************************************************************/

void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now, struct Item **purgeKeysPtr, struct Item **purgeNamesPtr)

{
  bson_iterator it1,it2;
  time_t then;
  char purgeKey[CF_MAXVARSIZE];

  if (strcmp(bson_iterator_key(itp), reportKey) != 0)
    {
    return;
    }

  bson_iterator_init(&it1,bson_iterator_value(itp));

  while (bson_iterator_next(&it1))
    {
      bson_iterator_init(&it2,bson_iterator_value(&it1));

      while (bson_iterator_next(&it2))
	{
	  if(strcmp(bson_iterator_key(&it2),cfr_time) == 0)
	    {
	      then = bson_iterator_int(&it2);
	      
	      if(now - then >= oldThreshold)  // definition of old
		{
		  snprintf(purgeKey,sizeof(purgeKey),"%s.%s",reportKey,bson_iterator_key(&it1));
		  PrependItem(purgeKeysPtr,purgeKey,NULL);
		  
		  if(purgeNamesPtr)
		    {
		    PrependItem(purgeNamesPtr,bson_iterator_key(&it1),NULL);
		    }

		  Debug("Report key \"%s\" needs to be purged (%lu seconds old)\n", purgeKey, now - then);
		}

	    }
	}

    }  
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

	      snprintf(thenStr,sizeof(thenStr),bson_iterator_string(&it2));

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


#endif  /* HAVE_LIBMONGOC */

