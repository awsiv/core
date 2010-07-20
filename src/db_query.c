
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

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

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

void CFDB_ListEverything(mongo_connection *conn)

{ mongo_cursor *cursor;
  bson_iterator it;
  bson_buffer bb;
  bson field;  // field description
  bson b,*query;

query = bson_empty(&b);
 
cursor = mongo_find(conn,MONGO_DATABASE,query,0,0,0,0);

while(mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it, cursor->current.data);

   while(bson_iterator_next(&it))
      {
      PrintCFDBKey(&it,1);   
      }
   }

bson_destroy(&field);

mongo_cursor_destroy(cursor);
}

/***************************************************************************************/

void CFDB_ListAllHosts(mongo_connection *conn)

{ mongo_cursor *cursor;
  bson_iterator it;
  bson field;  // field description
  bson *query,b;

query = bson_empty(&b);
 
cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

while(mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it, cursor->current.data);

   while (bson_iterator_next(&it))
      {
      if (bson_iterator_type(&it) == bson_string && strcmp(bson_iterator_key(&it),cfr_keyhash) == 0)
         {         
         printf("Host %s matches\n",bson_iterator_string(&it));         
         }
      }
   }

bson_destroy(&field);

mongo_cursor_destroy(cursor);
}

/***************************************************************************************/

void CFDB_ListAllHostsWithArrayElement(mongo_connection *conn,char *type,char *lval,char *rval)

{ mongo_cursor *cursor;
  bson_iterator it,it2,it3;
  bson_buffer bb;
  bson field;  // field description
  bson *query,b;
  int found_type,found_lval;
  char host[CF_MAXVARSIZE],ipaddr[CF_BUFSIZE];
  
// only return specific field - the key hash

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,type,1);
bson_from_buffer(&field, &bb);

// Search document

query = bson_empty(&b);

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

printf("Looking for %s = %s.?.%s\n",type,lval,rval);

while(mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it, cursor->current.data);

   found_type = false;
   
   while (bson_iterator_next(&it))
      {
      found_lval = false;

      // Make a note of the key
      
      if (strcmp(bson_iterator_key(&it),cfr_keyhash) == 0)
         {
         strcpy(host,bson_iterator_string(&it));
         }

      // Now get the IP addresses
      
      if (strcmp(bson_iterator_key(&it),cfr_ip_array) == 0)
         {         
         bson_iterator_init(&it2, bson_iterator_value(&it));
         ipaddr[0] = '\0';
         
         while (bson_iterator_next(&it2))
            {
            Join(ipaddr,(char *)bson_iterator_string(&it2));
            }
         }

      // Get the lval type we want - don't know the depth

      if (strcmp(bson_iterator_key(&it),type) == 0)
         {
         found_type = true;
         
         bson_iterator_init(&it2, bson_iterator_value(&it));
         
         while (!found_lval && bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3, bson_iterator_value(&it2));

            // Try level 3
            
            while (bson_iterator_next(&it3))
               {
               if (bson_iterator_type(&it3) == bson_string && strcmp(lval,bson_iterator_key(&it3)) == 0)
                  {
                  if (strcmp(bson_iterator_string(&it3),rval) == 0)
                     {
                     found_lval = true;
                     break;
                     }
                  }
               }
            }
         }

      if (found_lval)
         {         
         printf("Host \"%s\" (seen at %s) matches search for \"%s\" = \"%s\"\n",host,ipaddr,lval,rval);
         break;
         }
      }
   }

bson_destroy(&field);

mongo_cursor_destroy(cursor);
}

/***************************************************************************************/

void CFDB_QueryHosts(mongo_connection *conn, bson *query, char *resKeyVal, struct Item **result)

/* Takes a query document and returns one field (resKeyVal) from
 * each host that matches the query. Use bson_empty(&b) as query to
 * match all hosts.
 * TODO: Generalise to take a list of wanted field values instead of
 * one. */

{ mongo_cursor *cursor;
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
   
   if (!CFDB_KeyPosition(&it, resKeyVal, bson_string))
      {
      CfOut(cf_error, "", "!! Could not find \"%s\" element in DB report document", resKeyVal);
      continue;
      }
   
   Debug("Found DB key \"%s\"\n", bson_iterator_string(&it));
   
   //AppendItem(result,(char *)bson_iterator_string(&it), NULL);
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
}

/*****************************************************************************/

void CFDB_QuerySoftware(mongo_connection *conn, char *name, char *ver, char *arch, int regex, char *resKeyVal, struct Item **result)

/**
 * !! FIXME: REGEX NOT WORKING
 *
 * Returns a set of hosts having the given software (name,ver,arch).
 * ver and arch may be NULL, in which case they are not considered,
 * otherwise an AND search is done (e.g. name AND ver AND arch).
 */

{ bson_buffer bb;
  bson query;
  bson_buffer *sub1, *sub2;

// return resKeyVal of documents matching this query
bson_buffer_init(&bb);

sub1 = bson_append_start_object(&bb, cfr_software);
sub2 = bson_append_start_object(&bb, "$elemMatch");

if (regex)
   {
   bson_append_regex(sub2,cfr_name, name, "");

   if (ver)
      {
      bson_append_regex(sub2,cfr_version, ver, "");
      }

   if (arch)
      {
      bson_append_regex(sub2,cfr_arch, arch, "");
      }
   }
else
   {
   bson_append_string(sub2,cfr_name, name);

   if (ver)
      {
      bson_append_string(sub2,cfr_version, ver);
      }

   if (arch)
      {
      bson_append_string(sub2,cfr_arch, arch);
      }
   }

bson_append_finish_object(sub2);
bson_append_finish_object(sub1);

bson_from_buffer(&query, &bb);

CFDB_QueryHosts(conn, &query, resKeyVal, result);

bson_destroy(&query);  
}

/*****************************************************************************/

int CFDB_KeyPosition(bson_iterator *it, char *keyName, bson_type valType)

/* Positions the iterator at the given string key name.
 * Returns true if found, false otherwise */

{
while (bson_iterator_next(it))
   {
   if (strcmp(bson_iterator_key(it), keyName) != 0)
      {
      Debug("Report DB key=\"%s\", looking for \"%s\"\n", bson_iterator_key(it), keyName);
      continue;
      }
   
   if ((valType != -1) && bson_iterator_type(it) != valType)
      {
      CfOut(cf_error, "", "!! Key \"%s\" value in report CFDB is of wrong type (looking for type=%d, found type=%d)", 
            keyName, valType, bson_iterator_type(it));
      return false;
      }
   
   return true;
   }

return false;  
}

/*****************************************************************************/

struct Rlist *CFDB_ReadAllSoftware(mongo_connection *conn, bson *query)

/*
 * Returns all software packages in all matching hosts.
 * They need to be parsed further if required (e.g. only one package
 * version is of interest).
 */

{ mongo_cursor *cursor;
  bson_iterator it, subIt, currPack;
  bson_buffer bb;
  bson field;  // field description
  struct Rlist *list = NULL;
  
// Set flag "1" to only return software-field

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_software,1);
bson_from_buffer(&field, &bb);

cursor = mongo_find(conn, MONGO_DATABASE, query, &field, 0, 0, 0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it, cursor->current.data);
   
   // _id-element may come first

   if (!CFDB_KeyPosition(&it,cfr_software,bson_array))  
      {
      CfOut(cf_error, "", "!! Could not find \"%s\" element in CFDB report document", cfr_software);
      continue;
      }

   bson_iterator_init(&subIt, bson_iterator_value(&it));
      
   while (CFDB_IteratorNext(&subIt, bson_object))  // loops over software packages
      {
      bson_iterator_init(&currPack, bson_iterator_value(&subIt));

      printf("PACKAGE on %s: ",bson_iterator_key(&it));

      while(CFDB_IteratorNext(&currPack, bson_string)) // loops over package objects (n,v,a)
         {
         // allocate new and put in struct HubSoftware
         if (strcmp(bson_iterator_key(&currPack),cfr_name) == 0)
            {
            printf("n:%s", bson_iterator_string(&currPack));
            }
         else if (strcmp(bson_iterator_key(&currPack),cfr_version) == 0)
            {
            printf("v:%s", bson_iterator_string(&currPack));
            }
         else if (strcmp(bson_iterator_key(&currPack),cfr_arch) == 0)
            {
            printf("a:%s",Nova_LongArch(bson_iterator_string(&currPack)));
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
return list;
}

/*****************************************************************************/

int CFDB_IteratorNext(bson_iterator *it, bson_type valType)

{
if (bson_iterator_next(it))
   {
   if (bson_iterator_type(it) != valType)
      {
      CfOut(cf_error, "", "!! CFDB value of unexpected type (was=%d,expected=%d)",bson_iterator_type(it), valType);
      return false;
      }
   
   return true;
   }

return false;
}

/*****************************************************************************/

void PrintCFDBKey(bson_iterator *it, int depth)

{ bson_iterator subIt;
  char hex_oid[25];
  int i;

for (i = 0; i < depth; i++)
   {
   printf("\t");
   }

printf("key: %s - ", bson_iterator_key(it));

switch(bson_iterator_type(it))
   {
   case bson_double:
       printf("(double) %f\n", bson_iterator_double(it));
       break;
   case bson_int:
       printf("(int) %d\n", bson_iterator_int(it));
       break;
   case bson_string:
       printf("(string) \"%s\"\n", bson_iterator_string(it));
       break;
   case bson_oid:
       bson_oid_to_string(bson_iterator_oid(it), hex_oid);
       printf("(oid) \"%s\"\n", hex_oid);
       break;

   case bson_object:
   case bson_array:
       printf("(subobject/array):\n");
       bson_iterator_init(&subIt, bson_iterator_value(it));
       
       while(bson_iterator_next(&subIt))
          {
          PrintCFDBKey(&subIt, depth + 1);
          }
       
       break;
       //  case bson_array:
       //    printf("(array) [...]\n");
    //    break;
   default:
    printf("(type %d)\n", bson_iterator_type(it));
    break;
   }
}
 
#endif  /* HAVE LIB_MONGOC */


