/*****************************************************************************/
/*                                                                           */
/* File: db_query.c                                                          */
/*                                                                           */
/* Created: Wed Jul 16 14:41:22 2010                                         */
/*                                                                           */
/* MongoDB implementation of report query.                                   */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC


void Nova_DBQueryHosts(mongo_connection *conn, bson *query, char *resKeyVal, struct Item **result)
/* Takes a query document and returns one field (resKeyVal) from
 * each host that matches the query. Use bson_empty(&b) as query to
 * match all hosts.
 * TODO: Generalise to take a list of wanted field values instead of
 * one. */

{
  mongo_cursor *cursor;
  bson_iterator it;
  bson_buffer bb;
  bson field;  // field description
  
  // only return specific field
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

/*****************************************************************************/

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

  sub1 = bson_append_start_object(&bb, cfr_software);
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



//  struct Rlist *rp,*list;

struct Rlist *Nova_DBReadAllSoftware(mongo_connection *conn, bson *query)
/*
 * Returns all software packages in all matching hosts.
 * They need to be parsed further if required (e.g. only one package
 * version is of interest).
 */
{
  mongo_cursor *cursor;
  bson_iterator it, subIt, currPack;
  bson_buffer bb;
  bson field;  // field description
  

  // only return software-field
  bson_buffer_init(&bb);
  bson_append_int(&bb, cfr_software, 1);
  bson_from_buffer(&field, &bb);
  

  cursor = mongo_find(conn, MONGO_DATABASE, query, &field, 0, 0, 0);

  while(mongo_cursor_next(cursor))  // loops over documents
    {
      
      bson_iterator_init(&it, cursor->current.data);

      // _id-element may come first
      if(!Nova_MongoKeyPosition(&it, cfr_software, bson_array))  
	{
	  CfOut(cf_error, "", "!! Could not find \"%s\" element in DB report document", cfr_software);
	  continue;
	}

      
      bson_iterator_init(&subIt, bson_iterator_value(&it));

      
      while(Nova_DBIteratorNext(&subIt, bson_object))  // loops over software packages
	{
	  
	  bson_iterator_init(&currPack, bson_iterator_value(&subIt));


	  printf("PACKAGE: ");
	  while(Nova_DBIteratorNext(&currPack, bson_string)) // loops over package objects (n,v,a)
	    {
	      // allocate new and put in struct HubSoftware
	      if(strcmp(bson_iterator_key(&currPack), "n") == 0)
		{
		  printf("n:%s", bson_iterator_string(&currPack));
		}
	      else if(strcmp(bson_iterator_key(&currPack), "v") == 0)
		{
		  printf("v:%s", bson_iterator_string(&currPack));
		}
	      else if(strcmp(bson_iterator_key(&currPack), "a") == 0)
		{
		  printf("a:%s", bson_iterator_string(&currPack));
		}
	      else
		{
		  CfOut(cf_error, "", "!! Unknown key \"%s\" in software packages",
			bson_iterator_key(&currPack));
		}
	    }
	  printf("\n");
	  
	}
      
    }  

  bson_destroy(&field);
  mongo_cursor_destroy(cursor);  
}


int Nova_DBIteratorNext(bson_iterator *it, bson_type valType)
{
  if(bson_iterator_next(it))
    {
      if(bson_iterator_type(it) != valType)
	{
	  CfOut(cf_error, "", "!! DB value of unexpected type (was=%d,expected=%d)", 
		bson_iterator_type(it), valType);
	  return false;
	}
      
      return true;
    }

  return false;
}


#endif  /* HAVE LIB_MONGOC */


