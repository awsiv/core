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



#endif  /* HAVE_MONGOC */

