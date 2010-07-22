
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: db_query.c                                                          */
/*                                                                           */
/* Created: Wed Jul 16 14:41:22 2010                                         */
/*                                                                           */
/* MongoDB implementation of report query drivers.                           */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void CFDB_ListEverything(mongo_connection *conn)

{ mongo_cursor *cursor;
  bson_iterator it;
  bson b,*query;

query = bson_empty(&b);
 
cursor = mongo_find(conn,MONGO_DATABASE,query,0,0,0,0);

while(mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it,cursor->current.data);

   while(bson_iterator_next(&it))
      {
      PrintCFDBKey(&it,1);   
      }
   }

mongo_cursor_destroy(cursor);
}

/*****************************************************************************/

struct HubQuery *CFDB_QuerySoftware(mongo_connection *conn,char *lname,char *lver,char *larch,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char rname[CF_MAXVARSIZE],rversion[CF_MAXVARSIZE],rarch[3];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,cfr_software,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

hostnames[0] = '\0';
addresses[0] = '\0';

//cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
cursor = mongo_find(conn,MONGO_DATABASE,bson_empty(&b),&field,0,0,0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   keyhash[0] = '\0';
   hostnames[0] = '\0';
   addresses[0] = '\0';

   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      /* Query specific search/marshalling */

      if (strcmp(bson_iterator_key(&it1),cfr_software) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3, bson_iterator_value(&it2));

            rname[0] = '\0';
            rversion[0] = '\0';
            rarch[0] = '\0';         

            while (bson_iterator_next(&it3))
               {
               if (strcmp(bson_iterator_key(&it3),cfr_name) == 0)
                  {
                  strncpy(rname,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_version) == 0)
                  {
                  strncpy(rversion,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_arch) == 0)
                  {
                  strncpy(rarch,bson_iterator_string(&it3),2);
                  }
               else
                  {
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in software packages",bson_iterator_key(&it3));
                  }
               }

            if (strlen(rname) > 0)
               {
               int match_name = true, match_version = true, match_arch = true;
               
               if (regex)
                  {
                  if (lname && !FullTextMatch(lname,rname))
                     {
                     match_name = false;
                     }

                  if (lver && !FullTextMatch(lver,rversion))
                     {
                     match_version = false;
                     }

                  if (larch && !FullTextMatch(larch,rarch))
                     {
                     match_arch = false;
                     }
                  }
               else
                  {
                  if (lname && (strcmp(lname,rname) != 0))
                     {
                     match_name = false;
                     }
                  
                  if (lver && (strcmp(lver,rversion) != 0))
                     {
                     match_version = false;
                     }
                  
                  if (larch && (strcmp(larch,rarch) != 0))
                     {
                     match_arch = false;
                     }                  
                  }

               if (match_name && match_version && match_arch)
                  {
                  AppendRlistAlien(&record_list,NewHubSoftware(NULL,rname,rversion,rarch));
                  }
               }               
            }
         }   
      }

   hh = NewHubHost(keyhash,addresses,hostnames);
   AppendRlistAlien(&host_list,hh);
   printf("FOUND ON %s,%s,%s\n",keyhash,addresses,hostnames);

   // Now cache the host reference in all of the records to flatten the 2d list
   
   for (rp = record_list; rp != NULL; rp=rp->next)
      {
      struct HubSoftware *hs = (struct HubSoftware *)rp->item;
      hs->hh = hh;
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/



/*****************************************************************************/

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void CMDB_ScanHubHost(bson_iterator *it1,char *keyhash,char *ipaddr,char *hostnames)

{ bson_iterator it2;
 
if (bson_iterator_type(it1) == bson_string && strcmp(bson_iterator_key(it1),cfr_keyhash) == 0)
   {         
   strncpy(keyhash,bson_iterator_string(it1),CF_MAXVARSIZE-1);
   }

if (strcmp(bson_iterator_key(it1),cfr_ip_array) == 0)
   {         
   bson_iterator_init(&it2, bson_iterator_value(it1));
   
   while (bson_iterator_next(&it2))
      {
      Join(ipaddr,(char *)bson_iterator_string(&it2));
      }
   }

if (strcmp(bson_iterator_key(it1),cfr_host_array) == 0)
   {         
   printf("FOUND KEY.\n");
   bson_iterator_init(&it2, bson_iterator_value(it1));
   
   while (bson_iterator_next(&it2))
      {
      Join(hostnames,(char *)bson_iterator_string(&it2));
      }
   }
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

void PrintCFDBKey(bson_iterator *it1, int depth)

{ bson_iterator it2;
  char hex_oid[25];
  int i;

for (i = 0; i < depth; i++)
   {
   printf("\t");
   }

printf("key: %s - ", bson_iterator_key(it1));

switch(bson_iterator_type(it1))
   {
   case bson_double:
       printf("(double) %f\n", bson_iterator_double(it1));
       break;
   case bson_int:
       printf("(int) %d\n", bson_iterator_int(it1));
       break;
   case bson_string:
       printf("(string) \"%s\"\n", bson_iterator_string(it1));
       break;
   case bson_oid:
       bson_oid_to_string(bson_iterator_oid(it1), hex_oid);
       printf("(oid) \"%s\"\n", hex_oid);
       break;

   case bson_object:
   case bson_array:

       printf("(subobject/array):\n");
       bson_iterator_init(&it2, bson_iterator_value(it1));
       
       while(bson_iterator_next(&it2))
          {
          PrintCFDBKey(&it2, depth + 1);
          }
       
       break;

   default:
    printf("(type %d)\n", bson_iterator_type(it1));
    break;
   }
}


#endif  /* HAVE LIB_MONGOC */


