/*****************************************************************************/
/*                                                                           */
/* File: db_mongo.c                                                          */
/*                                                                           */
/* Created: Wed Jul  7 09:25:43 2010                                         */
/*                                                                           */
/* MongoDB implementation of report save and query.                          */
/*                                                                           */
/*****************************************************************************/

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

void Nova_DBQueryHosts(mongo_connection *conn, bson *query, struct Item **result)
/* Takes a query document and returns a set of public key hashes
 * of hosts that matches the query. */
{
  mongo_cursor *cursor;
  bson_iterator it;
  bson_buffer bb;
  bson field;  // field description
  

  // only return specific fields (key hash for now)

  bson_buffer_init(&bb);
  bson_append_int(&bb, "keyHash", 1);
  bson_from_buffer(&field, &bb);
  

  cursor = mongo_find(conn, MONGO_DATABASE, query, &field, 0, 0, 0);

  while(mongo_cursor_next(cursor))  // loops over documents
    {
      
      bson_iterator_init(&it, cursor->current.data);

      if(!Nova_MongoKeyPosition(&it, "keyHash", bson_string))
	{
	  CfOut(cf_error, "", "!! Could not find keyHash element in DB report document");
	  continue;
	}
      
      Debug("Found DB key \"%s\"\n", bson_iterator_string(&it));

      AppendItem(result, bson_iterator_string(&it), NULL);
      
    }
  
  
  bson_destroy(&field);
  mongo_cursor_destroy(cursor);

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

