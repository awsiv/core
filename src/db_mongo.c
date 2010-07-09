/*****************************************************************************/
/*                                                                           */
/* File: db_mongo.c                                                          */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report save and query.                          */
/*                                                                           */
/*****************************************************************************/

// TODO: Fix regex queries, ensure index on keyHash and sw.n

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC

int Nova_DBOpen(mongo_connection *conn, char *host, int port)
{
  mongo_connection_options connOpts;

  snprintf(connOpts.host, sizeof(connOpts.host), "%s", host);
  connOpts.port = port;
  
  if(mongo_connect(conn , &connOpts) != 0)
    {
      CfOut(cf_error, "mongo_connect", "!! Could not connect to database");
      return false;
    }

  return true;

}

/*****************************************************************************/

int Nova_DBClose(mongo_connection *conn)
{
  if(mongo_destroy(conn) != 0)
    {
      CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from database");
      return false;
    }
  
  return true;
}

/*****************************************************************************/

void Nova_DBInitialize()
{
  //  make sure monitord arrays exists
}

/*****************************************************************************/

void Nova_DBSaveSoftware(mongo_connection *conn, char *keyHash, struct Item *data)
{
  bson_buffer bb;
  bson_buffer *setObj;
  bson_buffer *sub;
  bson cond;  // host description
  bson setOp;
  int i;
  bson_buffer *arr;
  struct Item *ip;
  char packNumStr[CF_MAXVARSIZE];
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch, archStr[CF_MAXVARSIZE];


  // find right host
  bson_buffer_init(&bb);
  bson_append_string(&bb, "keyHash", keyHash);
  bson_from_buffer(&cond, &bb);


  bson_buffer_init(&bb);

  setObj = bson_append_start_object(&bb, "$set");
  arr = bson_append_start_array(setObj , "sw");
  
  for (ip = data, i = 0; ip != NULL; ip=ip->next, i++)
    {
      sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

      snprintf(packNumStr, sizeof(packNumStr), "%d", i);
      snprintf(archStr, sizeof(archStr), "%c", arch);

      sub = bson_append_start_object(arr , packNumStr);
      bson_append_string(sub , "n" , name);
      bson_append_string(sub , "v" , version);
      bson_append_string(sub , "a" , archStr);
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

void Nova_DBSaveMonitorData(mongo_connection *conn, char *keyHash, enum monitord_rep rep_type, struct Item *data)
{
  bson_buffer bb;
  bson cond;  // host description
  char varNameIndex[64];
  bson_buffer *setObj;
  bson setOp;
  int i, arrIndex;
  struct Item *ip;
  int observable,slot;
  double q,e,dev;
  char *repPrefix;
  char t[CF_TIME_SIZE];
  char timekey[CF_SMALLBUF];


  switch(rep_type)
    {
    case mon_rep_mag:
      repPrefix = "mag";
      break;
    case mon_rep_week:
      repPrefix = "week";
      break;
    case mon_rep_yr:
      repPrefix = "yr";
      break;
    default:
      CfOut(cf_error, "", "!! Unknown monitord report type (%d)", rep_type);
      FatalError("Software Error");
    }


  // find right host
  bson_buffer_init(&bb);
  bson_append_string(&bb, "keyHash", keyHash);
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

   snprintf(varNameIndex, sizeof(varNameIndex), "%s%d.%d",repPrefix, observable, slot*3);
   bson_append_double(setObj, varNameIndex, q);
   snprintf(varNameIndex, sizeof(varNameIndex), "%s%d.%d",repPrefix, observable, slot*3 + 1);
   bson_append_double(setObj, varNameIndex, e);
   snprintf(varNameIndex, sizeof(varNameIndex), "%s%d.%d",repPrefix, observable, slot*3 + 2);
   bson_append_double(setObj, varNameIndex, dev);

   }
  

  
  bson_append_finish_object(setObj);
  bson_from_buffer(&setOp,&bb);

  mongo_update(conn, MONGO_DATABASE, &cond, &setOp, MONGO_UPDATE_UPSERT);
  
  bson_destroy(&setOp);
  bson_destroy(&cond);
}

/*****************************************************************************/

void Nova_DBQueryHosts(mongo_connection *conn, bson *query, char *resKeyVal, struct Item **result)
/* Takes a query document and returns a set of public key hashes
 * of hosts that matches the query. Use bson_empty(&b) as query to
 * match all hosts.
 * TODO: Generalise to take a list of wanted key values instead of
 * one. */

{
  mongo_cursor *cursor;
  bson_iterator it;
  bson_buffer bb;
  bson field;  // field description
  

  // only return specific fields (key hash for now)

  bson_buffer_init(&bb);
  bson_append_int(&bb, resKeyVal, 1);
  bson_from_buffer(&field, &bb);
  

  cursor = mongo_find(conn, MONGO_DATABASE, query, &field, 0, 0, 0);

  while(mongo_cursor_next(cursor))  // loops over documents
    {
      
      bson_iterator_init(&it, cursor->current.data);

      if(!Nova_MongoKeyPosition(&it, resKeyVal, bson_string))
	{
	  CfOut(cf_error, "", "!! Could not find \"%s\" element in DB report document", resKeyVal);
	  continue;
	}
      
      Debug("Found DB key \"%s\"\n", bson_iterator_string(&it));

      AppendItem(result, bson_iterator_string(&it), NULL);
      
    }
  
  
  bson_destroy(&field);
  mongo_cursor_destroy(cursor);

}

void Nova_DBQuerySoftware(mongo_connection *conn, char *name, char *ver, char *arch, int regex, char *resKeyVal, struct Item **result)
/**
 * !! FIXME: REGEX NOT WORKING
 *
 * Returns a set of hosts having the given software (name,ver,arch).
 * ver and arch may be NULL, in which case they are not considered,
 * otherwise an AND search is done (e.g. name AND ver AND arch).
 */
{
  bson_buffer bb;
  bson query;
  bson_buffer *sub1, *sub2;

  // return resKeyVal of documents matching this query
  bson_buffer_init(&bb);

  sub1 = bson_append_start_object(&bb, "sw");
  sub2 = bson_append_start_object(&bb, "$elemMatch");
  
  if(regex)
    {
      bson_append_regex(sub2, "n" , name, "");
      if(ver)
	{
	  bson_append_regex(sub2, "v" , ver, "");
	}
      if(arch)
	{
	  bson_append_regex(sub2, "a" , arch, "");
	}
    }
  else
    {
      bson_append_string(sub2, "n" , name);
      if(ver)
	{
	  bson_append_string(sub2, "v" , ver);
	}
      if(arch)
	{
	  bson_append_string(sub2, "a" , arch);
	}
    }
  
  bson_append_finish_object(sub2);
  bson_append_finish_object(sub1);

  bson_from_buffer(&query, &bb);

  Nova_DBQueryHosts(conn, &query, resKeyVal, result);

  bson_destroy(&query);  
  
}

/*****************************************************************************/

int Nova_MongoKeyPosition(bson_iterator *it, char *keyName, bson_type valType)
/* Positions the iterator at the given string key name.
 * Returns true if found, false otherwise */
{
  while(bson_iterator_next(it))
    {
      if(strcmp(bson_iterator_key(it), keyName) != 0)
	{
	  Debug("Report DB key=\"%s\", looking for \"%s\"\n", bson_iterator_key(it), keyName);
	  continue;
	}

      if((valType != -1) &&
	 bson_iterator_type(it) != valType)
	{
	  CfOut(cf_error, "", "!! Key \"%s\" value in report DB is of wrong type (looking for type=%d, found type=%d)", 
		keyName, valType, bson_iterator_type(it));
	  return false;
	}
      
      return true;
    }
  
  return false;

}


#endif  /* HAVE_MONGOC */

