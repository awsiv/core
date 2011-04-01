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
/**********************************************************fosdem*******************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC


/*****************************************************************************/

int CFDB_GetValue(char *lval,char *rval,int size)

{ mongo_connection conn;
  
 // clients do not run mongo server -- will fail to connect

 if (!IsDefinedClass("am_policy_hub") && !IsDefinedClass("am_php_module"))
    {
    CfOut(cf_verbose,"","Ignoring DB get of (%s) - we are not a policy server",lval);
    return false;
    }
  
 if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database to get value %s", lval);
    return false;
    }
    
 CFDB_HandleGetValue(lval,rval,size,&conn);


 CFDB_Close(&conn);

 return true;
}

/*****************************************************************************/

void CFDB_HandleGetValue(char *lval, char *rval, int size, mongo_connection *conn)
{
 bson_buffer bb;
 bson field,query;
 bson_iterator it1;
 mongo_cursor *cursor;

 rval[0] = '\0';

 cursor = mongo_find(conn,MONGO_SCRATCH,bson_empty(&query),0,0,0,0);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1),lval) == 0)
          {
          snprintf(rval,size,"%s",bson_iterator_string(&it1));
          break;
          }
       }    
    }

 mongo_cursor_destroy(cursor);

}

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

struct HubQuery *CFDB_QueryHosts(mongo_connection *conn,bson *query)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char rname[CF_MAXVARSIZE],rversion[CF_MAXVARSIZE],rarch[3];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 int found = false;
  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
       }      

    hh = NewHubHost(keyhash,addresses,hostnames);
    PrependRlistAlien(&host_list,hh);
    }

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,NULL);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryHostsByAddress(mongo_connection *conn, char *hostNameRegex, char *ipRegex, char *classRegex)
{ bson_buffer bb;
 bson query;
 struct HubQuery *hq;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
  
/* BEGIN query document */
 bson_buffer_init(&bb);

 if (!EMPTY(hostNameRegex))
    {
    bson_append_regex(&bb,cfr_host_array,hostNameRegex,"");
    emptyQuery = false;
    }

 if (!EMPTY(ipRegex))
    {
    bson_append_regex(&bb,cfr_ip_array,ipRegex,"");
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

 hq = CFDB_QueryHosts(conn,&query);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }
 
 return hq;
}

/*****************************************************************************/

struct HubQuery *CFDB_QuerySoftware(mongo_connection *conn,char *keyHash,char *type,char *lname,char *lver,char *larch,int regex, char *classRegex, int sort)
{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char rname[CF_MAXVARSIZE] = {0},rversion[CF_MAXVARSIZE] = {0},rarch[3] = {0},arch[3] = {0};
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
 int found = false;

 if(!EMPTY(larch))
   {
     snprintf(arch,2,"%c",larch[0]);
   }
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,type,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_empty(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
       found = false;
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),type) == 0)
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
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in software packages",bson_iterator_key(&it3));
                   }
                }

             if (strlen(rname) > 0)
                {
                int match_name = true, match_version = true, match_arch = true;
               
                if (regex)
                   {
                   if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                      {
                      match_name = false;
                      }

                   if (!EMPTY(lver) && !FullTextMatch(lver,rversion))
                      {
                      match_version = false;
                      }
                   if (!EMPTY(larch) && !FullTextMatch(arch,rarch))
                      {
                      match_arch = false;
                      }
                   }
                else
                   {
                   if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                      {
                      match_name = false;
                      }
                  
                   if (!EMPTY(lver) && (strcmp(lver,rversion) != 0))
                      {
                      match_version = false;
                      }
                  
                   if (!EMPTY(larch) && (strcmp(arch,rarch) != 0))
                      {
                      match_arch = false;
                      }                  
                   }
                if (match_name && match_version && match_arch)
                   {
                   found = true;
                   rp = PrependRlistAlien(&record_list,NewHubSoftware(CF_THIS_HH,rname,rversion,rarch));
                   }
                }               
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);
      
       // Now cache the host reference in all of the records to flatten the 2d list
      
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubSoftware *hs = (struct HubSoftware *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if (sort)
    {
    record_list = SortRlist(record_list,SortSoftware);
    }



 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 time_t rtime, now = time(NULL);
 double rsigma,rex;
 char rclass[CF_MAXVARSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 int match_class,found = false;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_class,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {

    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    rclass[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_class) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rclass,bson_iterator_key(&it2),CF_MAXVARSIZE-1);

             rex = 0;
             rsigma = 0;
             rtime = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   rex = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   rsigma = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rtime = bson_iterator_int(&it3);
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in classes",bson_iterator_key(&it3));
                   }
                }

             match_class = true;
            
             if (regex)
                {
                if (!EMPTY(lclass) && !FullTextMatch(lclass,rclass))
                   {
                   match_class = false;
                   }
                }
             else
                {
                if (!EMPTY(lclass) && (strcmp(lclass,rclass) != 0))
                   {
                   match_class = false;
                   }
                }
            
             if (match_class && (now - rtime < horizon))
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubClass(CF_THIS_HH,rclass,rex,rsigma,rtime));
                }            
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubClass *hs = (struct HubClass *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }
 

 if (sort)
    {
    record_list = SortRlist(record_list,SortClasses);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryClassSum(mongo_connection *conn, char **classes)
/**
 * classes is NULL or NULL-terminated array of strings
 *
 * NOTE: Can probably be made more efficient using group by class keys with a count
 **/
{
 bson_buffer bb, *arr1, *arr2, *obj;
 bson_iterator it1,it2;
 bson query, field;
 mongo_cursor *cursor;
 struct Rlist *hostList = NULL, *recordList = NULL;
 struct Item *classList = NULL, *ip;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_MAXVARSIZE],addresses[CF_MAXVARSIZE];
 char iStr[CF_SMALLBUF];
 int classFrequency;
 bool emptyQuery = true;
 int i;

 Debug("CFDB_QueryClassSum()\n");

  // query
 bson_buffer_init(&bb);

 if(classes && classes[0])
    {
    obj = bson_append_start_object(&bb,cfr_class_keys);
    arr1 = bson_append_start_array(&bb,"$all");

    for(i = 0; classes[i] != NULL; i++)
       {
       snprintf(iStr, sizeof(iStr), "%d", i);
       bson_append_string(obj, iStr, classes[i]);
       }
        
    bson_append_finish_object(arr1);
    bson_append_finish_object(obj);
    
    emptyQuery = false;
    }
 
 
 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

// returned attribute
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_from_buffer(&field,&bb);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
 bson_destroy(&field);
 // query freed below

 // 1: collect hosts matching new class

 while(mongo_cursor_next(cursor))
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    addresses[0] = '\0';
    hostnames[0] = '\0';
    
    while (bson_iterator_next(&it1))
       {
       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
       }
    
    if(!EMPTY(keyhash))
        {
        PrependRlistAlien(&hostList,NewHubHost(keyhash,addresses,hostnames));
        Debug("matched host %s,%s\n", keyhash, addresses);
        }
    }

 mongo_cursor_destroy(cursor);

 
 // 2: find all distinct classes in these hosts
 classList = CFDB_QueryDistinct(conn, MONGO_BASE, "hosts", cfr_class_keys, &query);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }

 
 // 3: count occurences of each class in subset of hosts
 for(ip = classList; ip != NULL; ip = ip->next)
    {
     bson_buffer_init(&bb);
     
     obj = bson_append_start_object(&bb,cfr_class_keys);
     arr1 = bson_append_start_array(&bb,"$all");
     
     for(i = 0; classes && classes[i] != NULL; i++)
        {
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(obj, iStr, classes[i]);
        }
     snprintf(iStr, sizeof(iStr), "%d", i);
     bson_append_string(obj, iStr, ip->name);     
     
     bson_append_finish_object(arr1);
     bson_append_finish_object(obj);

     
     bson_from_buffer(&query,&bb);

     classFrequency = (int)mongo_count(conn, MONGO_BASE, "hosts", &query);

     Debug("class (%s,%d)\n", ip->name, classFrequency);

     PrependRlistAlien(&recordList,NewHubClassSum(CF_THIS_HH, ip->name, classFrequency));

     bson_destroy(&query);
    }
 

 DeleteItemList(classList);

 recordList = SortRlist(recordList,SortClassSum);

 return NewHubQuery(hostList, recordList);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryTotalCompliance(mongo_connection *conn,char *keyHash,char *lversion,time_t lt,int lkept,int lnotkept,int lrepaired,int cmp, int sort, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 int rkept,rnotkept,rrepaired,found = false;
 int match_kept,match_notkept,match_repaired,match_version,match_t;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rversion[CF_MAXVARSIZE];
 int emptyQuery = true;
 char classRegexAnch[CF_MAXVARSIZE];
 time_t rt;
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_total_compliance,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
       found = false;
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_total_compliance) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));

             rkept = -1;
             rrepaired = -1;
             rnotkept = -1;
             rversion[0] = '\0';
             rt = 0;
 
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_kept) == 0)
                   {
                   rkept = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_notkept) == 0)
                   {
                   rnotkept = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_repaired) == 0)
                   {
                   rrepaired = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rt = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_version) == 0)
                   {
                   strncpy(rversion,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in total compliance",bson_iterator_key(&it3));
                   }
                }

             match_version = match_t = match_kept = match_notkept = match_repaired = true;

             if (!EMPTY(lversion) && !FullTextMatch(lversion,rversion))
                {
                match_version = false;
                }
            
             if (cmp == CFDB_GREATERTHANEQ)
                {
                if (lt != -1 && lt > rt)
                   {
                   match_t = false;
                   }

                if (lkept != -1 && lkept < rkept)
                   {
                   match_kept = false;
                   }
               
                if (lnotkept != -1 && lnotkept < rnotkept)
                   {
                   match_notkept = false;
                   }               

                if (lrepaired != -1 && lrepaired < rrepaired)
                   {
                   match_repaired = false;
                   }               
                }
             else // CFDB_LESSTHANEQ
                {
                if (lt != -1 && lt < rt)
                   {
                   match_t = false;
                   }

                if (lkept != -1 && lkept > rkept)
                   {
                   match_kept = false;
                   }
               
                if (lnotkept != -1 && lnotkept > rnotkept)
                   {
                   match_notkept = false;
                   }               

                if (lrepaired != -1 && lrepaired > rrepaired)
                   {
                   match_repaired = false;
                   }               
                }

             if (match_kept && match_notkept && match_repaired && match_t && match_version)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubTotalCompliance(CF_THIS_HH,rt,rversion,rkept,rrepaired,rnotkept));
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);
      
       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubTotalCompliance *hs = (struct HubTotalCompliance *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortTotalCompliance);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryVariables(mongo_connection *conn,char *keyHash,char *lscope,char *llval,char *lrval,char *ltype,int regex, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3,it4,it5;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL,*newlist = NULL;
 int rkept,rnotkept,rrepaired,found = false;
 int match_type,match_scope,match_lval,match_rval;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rversion[CF_MAXVARSIZE];
 char rscope[CF_MAXVARSIZE], rlval[CF_MAXVARSIZE],mrval[CF_BUFSIZE],dtype[CF_MAXVARSIZE],rtype;
 void *rrval;
 time_t rt;
 int emptyQuery = true;
 char classRegexAnch[CF_MAXVARSIZE];
  
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_vars,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
       found = false;

       rlval[0] = '\0';
       rrval = NULL;
       rtype = CF_SCALAR;
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_vars) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));

             strncpy(rscope,bson_iterator_key(&it2),CF_MAXVARSIZE);

             while (bson_iterator_next(&it3))
                {
                rlval[0] = '\0';
                rtype = CF_SCALAR;
                rrval = NULL;
                newlist = NULL;
                dtype[0] = '\0';
                rt = 0;

                bson_iterator_init(&it4,bson_iterator_value(&it3));

                strncpy(rlval,bson_iterator_key(&it3),CF_MAXVARSIZE-1);

                while (bson_iterator_next(&it4))
                   {
                   if (strcmp(bson_iterator_key(&it4),cfr_rval) == 0)
                      {
                      switch (bson_iterator_type(&it4))
                         {
                         case bson_array:
                         case bson_object:
                             bson_iterator_init(&it5,bson_iterator_value(&it4));
                             rtype = CF_LIST;
                            
                             while (bson_iterator_next(&it5))
                                {
                                AppendRScalar(&newlist,(char *)bson_iterator_string(&it5),CF_SCALAR);   
                                }

                             rrval = newlist;
                             break;

                         default:
                             rrval = strdup(bson_iterator_string(&it4));
                             rtype = CF_SCALAR;
                             break;                            
                         }
                      }
                   else if (strcmp(bson_iterator_key(&it4),cfr_type) == 0)
                      {
                      strncpy(dtype,bson_iterator_string(&it4),CF_MAXVARSIZE);
                      }
                   else if (strcmp(bson_iterator_key(&it4),cfr_time) == 0)
                      {
                      rt = bson_iterator_int(&it4);
                      }
                   else
                      {
                      CfOut(cf_inform,"", " !! Unknown key \"%s\" in variables",bson_iterator_key(&it4));
                      }
                   }

                // Now do selection
               
                match_type = match_scope = match_lval = match_rval = true;
               
                if (regex)
                   {
                   if (!EMPTY(llval) && !FullTextMatch(llval,rlval))
                      {
                      match_lval = false;
                      }

                   if (!EMPTY(lrval) && !FullTextMatch(lrval,rrval))
                      {
                      match_rval = false;
                      }

                   if (!EMPTY(lscope) && !FullTextMatch(lscope,rscope))
                      {
                      match_scope = false;
                      }
                  
                   if (!EMPTY(ltype) && !FullTextMatch(ltype,dtype))
                      {
                      match_type = false;
                      }
                   }
                else
                   {
                   if (!EMPTY(llval) && strcmp(llval,rlval) != 0)
                      {
                      match_lval = false;
                      }

                   if (!EMPTY(lrval) && strcmp(lrval,rrval) != 0)
                      {
                      match_rval = false;
                      }
                  
                   if (!EMPTY(lscope) && strcmp(lscope,rscope) != 0)
                      {
                      match_scope = false;
                      }
                  
                   if (!EMPTY(ltype) && strcmp(ltype,dtype) != 0)
                      {
                      match_type = false;
                      }
                   }
                  
                // NOTE: rrval's ownership (deallocation) is either transferred, or it is freed here
                if (match_type && match_scope && match_lval && match_rval)
                   {
                   found = true;
                   rp = PrependRlistAlien(&record_list,NewHubVariable(CF_THIS_HH,dtype,rscope,rlval,rrval,rtype,rt));
                   }
                else
                   {
                   switch(rtype)
                      {
                      case CF_SCALAR:
                          if (rrval)
                             {
                             free(rrval);
                             }
                          break;
                         
                      case CF_LIST:
                          DeleteRlist(rrval);
                          break;
                      }                  
                   }
               
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);
      
       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubVariable *hv = (struct HubVariable *)rp->item;
          if (hv->hh == CF_THIS_HH)
             {
             hv->hh = hh;
             }
          }
       }
    }

 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPromiseCompliance(mongo_connection *conn,char *keyHash,char *lhandle,char lstatus,int regex, int sort, char *classRegex)
// status = c (compliant), r (repaired) or n (not kept), x (any)
{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 time_t rtime;
 double rsigma,rex;
 char rhandle[CF_MAXVARSIZE],rstatus,*prstat;
 char keyHashDb[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 int match_handle,match_status,found;
 int emptyQuery = true;
 char classRegexAnch[CF_MAXVARSIZE];
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_promisecompl,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyHashDb[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    rhandle[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyHashDb,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_promisecompl) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rhandle,bson_iterator_key(&it2),CF_MAXVARSIZE-1);

             rex = 0;
             rsigma = 0;
             rtime = 0;
             rstatus = 'x';
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   rex = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   rsigma = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rtime = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_promisestatus) == 0)
                   {
                   prstat = (char *)bson_iterator_string(&it3);
                   rstatus = *prstat;
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in promise compliance",bson_iterator_key(&it3));
                   }
                }

             match_handle = match_status = true;
            
             if (regex)
                {
                if (!EMPTY(lhandle) && !FullTextMatch(lhandle,rhandle))
                   {
                   match_handle = false;
                   }
                }
             else
                {
                if (!EMPTY(lhandle) && (strcmp(lhandle,rhandle) != 0))
                   {
                   match_handle = false;
                   }
                }

             if (lstatus != 'x' && lstatus != rstatus)
                {
                match_status = false;
                }
            
             if (match_handle && match_status)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubCompliance(CF_THIS_HH,rhandle,rstatus,rex,rsigma,rtime));
                }            
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyHashDb,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubPromiseCompliance *hp = (struct HubPromiseCompliance *)rp->item;      

          if (hp->hh == CF_THIS_HH)
             {
             hp->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortPromiseCompliance);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryLastSeen(mongo_connection *conn,char *keyHash,char *lhash,char *lhost,char *laddr,time_t lago,int regex,int sort,char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL,*list_start = NULL;
 double rago,ravg,rdev;
 char rhash[CF_MAXVARSIZE],rhost[CF_MAXVARSIZE],raddr[CF_MAXVARSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 int match_host,match_hash,match_addr,match_ago,found = false;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
 time_t rt;
  
/* BEGIN query document */
 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }


  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_lastseen,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    rhash[0] = '\0';
    raddr[0] = '\0';
    rhost[0] = '\0';
    found = false;
    list_start = NULL;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_lastseen) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rhash,bson_iterator_key(&it2),CF_MAXVARSIZE-1);

             ravg = 0;
             rdev = 0;
             rago = 0;
             rt = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_hrsavg) == 0)
                   {
                   ravg = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_hrsdev) == 0)
                   {
                   rdev = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_hrsago) == 0)
                   {
                   rago = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_ipaddr) == 0)
                   {
                   strncpy(raddr,bson_iterator_string(&it3),CF_MAXVARSIZE-1);

                   // try to find host name of IP from local DB

                   CFDB_QueryHostName(conn,raddr,rhost,sizeof(rhost));

                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rt = (time_t)bson_iterator_int(&it3);
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                   }
                }

             match_host = match_addr = match_hash = match_ago = true;
            
             if (regex)
                {
                if (!EMPTY(lhost) && !FullTextMatch(lhost,rhost))
                   {
                   match_host = false;
                   }

                // Doesn't make sense to do regex on a key
                if (!EMPTY(lhash) && (strcmp(lhash,rhash) != 0))
                   {
                   match_hash = false;
                   }

                if (!EMPTY(laddr) && !FullTextMatch(laddr,raddr))
                   {
                   match_addr = false;
                   }
                }
             else
                {
                if (!EMPTY(lhost) && (strcmp(lhost,rhost) != 0))
                   {
                   match_host = false;
                   }

                if (!EMPTY(lhash) && (strcmp(lhash,rhash) != 0))
                   {
                   match_hash = false;
                   }
               
                if (!EMPTY(laddr) && (strcmp(laddr,raddr) != 0))
                   {
                   match_addr = false;
                   }
                }

             if (lago > rago)
                {
                match_ago = false;
                }
            
             if (match_hash && match_host && match_addr && match_ago)
                {
                found = true;
                PrependRlistAlien(&record_list,NewHubLastSeen(CF_THIS_HH,*rhash,rhash+1,rhost,raddr,rago,ravg,rdev,rt));
                }            
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list

       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubLastSeen *hs = (struct HubLastSeen *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortLastSeen);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryMeter(mongo_connection *conn,char *lkeyhash)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qe,field,query;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 double rkept,rrepaired;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
 int found = false;
  
/* BEGIN query document */

 if (lkeyhash && strlen(lkeyhash) != 0)
    {
    bson_buffer_init(&b);
    bson_append_string(&b,cfr_keyhash,lkeyhash);
    bson_from_buffer(&query,&b);
    }

/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_meter,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 if (lkeyhash == NULL || strlen(lkeyhash) == 0)
    {
    cursor = mongo_find(conn,MONGO_DATABASE,bson_empty(&qe),&field,0,0,0);
    }
 else
    {
    cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
    bson_destroy(&query);
    }

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_meter) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rcolumn,bson_iterator_key(&it2),CF_SMALLBUF-1);

             rkept = 0;
             rrepaired = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_meterkept) == 0)
                   {
                   rkept = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_meterrepaired) == 0)
                   {
                   rrepaired = bson_iterator_double(&it3);
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in meter",bson_iterator_key(&it3));
                   }
                }

             found = true;
             rp = PrependRlistAlien(&record_list,NewHubMeter(CF_THIS_HH,*rcolumn,rkept,rrepaired));
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubMeter *hm = (struct HubMeter *)rp->item;
          if (hm->hh == CF_THIS_HH)
             {
             hm->hh = hh;
             }
          }
       }
    }

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPerformance(mongo_connection *conn,char *keyHash,char *lname,int regex,int sort,char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp =  NULL,*record_list = NULL, *host_list = NULL;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE], noteid[CF_MAXVARSIZE], rhandle[CF_MAXVARSIZE];
 int match_name,found = false;
 double rsigma,rex,rq;
 time_t rtime;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
  
/* BEGIN query document */
 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_performance,1);
 bson_append_int(&bb,cfn_nid,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_performance) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             snprintf(rhandle,CF_MAXVARSIZE,"%s",bson_iterator_key(&it2));
             snprintf(noteid,CF_MAXVARSIZE,"%s",CF_NONOTE);
	    
             snprintf(rname,sizeof(rname),"(unknown)");
             rex = 0;
             rsigma = 0;
             rtime = 0;
             rq = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_perf_event) == 0)
                   {
                   snprintf(rname,sizeof(rname),"%s",bson_iterator_string(&it3));
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_q) == 0)
                   {
                   rq = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   rex = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   rsigma = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rtime = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it1),cfn_nid) == 0)
                   {
		   snprintf(noteid,CF_MAXVARSIZE,"%s",bson_iterator_string(&it3));
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in performance",bson_iterator_key(&it3));
                   }
                }
            
             match_name = true;
            
             if (regex)
                {
                if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                   {
                   match_name = false;
                   }
                }
             else
                {
                if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                   {
                   match_name = false;
                   }
                }
            
             if (match_name)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubPerformance(CF_THIS_HH,rname,rtime,rq,rex,rsigma,noteid,rhandle));
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubPerformance *hp = (struct HubPerformance *)rp->item;
          if (hp->hh == CF_THIS_HH)
             {
             hp->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortPerformance);
    }

 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QuerySetuid(mongo_connection *conn,char *keyHash,char *lname,int regex, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE];
 int match_name,found = false;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;

  
/* BEGIN query document */
 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }


/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_setuid,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_setuid) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rname,bson_iterator_string(&it2),CF_SMALLBUF-1);
            
             match_name = true;
            
             if (regex)
                {
                if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                   {
                   match_name = false;
                   }
                }
             else
                {
                if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                   {
                   match_name = false;
                   }
                }
            
             if (match_name)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubSetUid(CF_THIS_HH,rname));
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list

       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubSetUid *hs = (struct HubSetUid *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryFileChanges(mongo_connection *conn,char *keyHash,char *lname,int regex,time_t lt,int cmp, int sort, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_BUFSIZE], handle[CF_MAXVARSIZE],noteid[CF_MAXVARSIZE];
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
 int match_name,match_t,found = false;
 time_t rt;
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_filechanges,1);
 bson_append_int(&bb,cfn_nid,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_filechanges) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          rname[0] = '\0';
          rt = 0;
         
          while (bson_iterator_next(&it2))
             {
             snprintf(handle,CF_MAXVARSIZE,"%s",bson_iterator_key(&it2));
             snprintf(noteid,CF_MAXVARSIZE,"%s",CF_NONOTE);
             bson_iterator_init(&it3,bson_iterator_value(&it2));
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_name) == 0)
                   {
                   strncpy(rname,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rt = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfn_nid) == 0)
                   {
                   snprintf(noteid,CF_MAXVARSIZE,"%s",bson_iterator_string(&it3));
                   }
                }
            
             match_name = match_t = true;
            
             if (cmp == CFDB_GREATERTHANEQ)
                {
                if (lt != -1 && lt > rt)
                   {
                   match_t = false;
                   }
                }
             else // CFDB_LESSTHANEQ
                {
                if (lt != -1 && lt < rt)
                   {
                   match_t = false;
                   }
                }
            
             if (regex)
                {
                if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                   {
                   match_name = false;
                   }
                }
             else
                {
                if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                   {
                   match_name = false;
                   }
                }
            
             if (match_name && match_t)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubFileChanges(CF_THIS_HH,rname,rt,noteid,handle));
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubFileChanges *hs = (struct HubFileChanges *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortFileChanges);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryFileDiff(mongo_connection *conn,char *keyHash,char *lname,char *ldiff,int regex,time_t lt,int cmp, int sort, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE],rdiff[CF_BUFSIZE];
 int match_name,match_t,match_diff,found = false;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
 time_t rt = 0;
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_filediffs,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_filediffs) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          rname[0] = '\0';
          rt = 0;
         
          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_name) == 0)
                   {
                   snprintf(rname,sizeof(rname),"%s",bson_iterator_string(&it3));
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_diff) == 0)
                   {
                   snprintf(rdiff,sizeof(rdiff),"%s",bson_iterator_string(&it3));
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rt = bson_iterator_int(&it3);
                   }
                }
            
             match_name = match_diff = match_t = true;

             if (cmp == CFDB_GREATERTHANEQ)
                {
                if (lt != -1 && lt > rt)
                   {
                   match_t = false;
                   }
                }
             else // CFDB_LESSTHANEQ
                {
                if (lt != -1 && lt < rt)
                   {
                   match_t = false;
                   }
                }
            
             if (regex)
                {
                if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                   {
                   match_name = false;
                   }

                if (!EMPTY(ldiff) && !FullTextMatch(ldiff,rdiff))
                   {
                   match_diff = false;
                   }
                }
             else
                {
                if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                   {
                   match_name = false;
                   }

                if (!EMPTY(ldiff) && (strcmp(ldiff,rdiff) != 0))
                   {
                   match_diff = false;
                   }
                }

             if (match_name && match_diff && match_t)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubFileDiff(CF_THIS_HH,rname,rdiff,rt));
                }
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubFileDiff *hs = (struct HubFileDiff *)rp->item;
          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortFileDiff);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPromiseLog(mongo_connection *conn,char *keyHash,enum promiselog_rep type,char *lhandle, int regex, time_t from, time_t to, int sort,char *classRegex)
{
 char classRegexAnch[CF_MAXVARSIZE];
 char rhandle[CF_MAXVARSIZE],rcause[CF_BUFSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE], noteid[CF_MAXVARSIZE], oid[CF_MAXVARSIZE];
 bson_iterator it1;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 bson query, field;
 int emptyQuery = true;  
 char *collName;
 mongo_cursor *cursor;
 bson_buffer bb;
 bson_buffer *timeRange, *sortObj;
 time_t rt;  


 // query

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }


 if (!EMPTY(lhandle))  // promise handle
    {
    if(regex)
       {
       bson_append_regex(&bb,cfr_promisehandle,lhandle,"");
       }
    else
       {
       bson_append_string(&bb,cfr_promisehandle,lhandle);
       }

    emptyQuery = false;
    }


 if(from || to)  // time interval
    {
    timeRange = bson_append_start_object(&bb, cfr_time);
   
    if(from)
       {
       bson_append_int(timeRange, "$gte",from);
       }
   
    if(to)
       {
       bson_append_int(timeRange, "$lte",to);
       }

    bson_append_finish_object(timeRange);
    emptyQuery = false;
    }


 if(!EMPTY(classRegex))  // class
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));

    if(!QueryHostsWithClass(conn,&bb,classRegexAnch))
       {
       // no host matches class regex, so stop
       bson_from_buffer(&query,&bb);
       bson_destroy(&query);
       return NewHubQuery(NULL,NULL);
       }

    emptyQuery = false;
    }


 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }


 // result 

 bson_buffer_init(&bb);
 bson_append_int(&bb,"_id",1);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_cause,1);
 bson_append_int(&bb,cfr_promisehandle,1);
 bson_append_int(&bb,cfr_time,1);
 bson_append_int(&bb,cfn_nid,1);
 bson_from_buffer(&field,&bb);

 switch (type)
    {
    case plog_repaired:
        collName = MONGO_LOGS_REPAIRED;
        break;
    case plog_notkept:
    default:
        collName = MONGO_LOGS_NOTKEPT;
        break;
    }


 // do search
 
 cursor = mongo_find(conn,collName,&query,&field,0,0,0);

 bson_destroy(&field);
 
 if(!emptyQuery)
    {
    bson_destroy(&query);
    }

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    rhandle[0] = '\0';
    rcause[0] = '\0';
    noteid[0] = '\0';
    oid[0] = '\0';
    rt = 0;

    while (bson_iterator_next(&it1))
       {
       /* Query specific search/marshalling */
       snprintf(noteid,sizeof(noteid),"%s",CF_NONOTE);
            
       if (strcmp(bson_iterator_key(&it1),cfr_keyhash) == 0)
          {
          snprintf(keyhash,sizeof(keyhash),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1),cfr_promisehandle) == 0)
          {
          snprintf(rhandle,sizeof(rhandle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1),cfr_cause) == 0)
          {
          snprintf(rcause,sizeof(rcause),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1),cfn_nid) == 0)
          {
          snprintf(noteid,sizeof(noteid),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1),cfr_time) == 0)
          {
          rt = bson_iterator_int(&it1);
          }
       else if (strcmp(bson_iterator_key(&it1),"_id") == 0)
          {
          bson_oid_to_string(bson_iterator_oid(&it1), oid);
          }
       }

    hh = GetHubHostIn(host_list,keyhash);
     
    if(!hh)
       {
       hh = NewHubHost(keyhash,NULL,NULL);  // we get more host info later
       PrependRlistAlien(&host_list,hh);
       }
     

    rp = PrependRlistAlien(&record_list,NewHubPromiseLog(hh,rhandle,rcause,rt,noteid,oid));
    }


 // now fill in hostnames and ips of the hosts

 QueryInsertHostInfo(conn,host_list);
 
 if(sort)
    {
    record_list = SortRlist(record_list,SortPromiseLog);
    }

 mongo_cursor_destroy(cursor);

 return NewHubQuery(host_list,record_list);
}


/*****************************************************************************/

struct HubQuery *CFDB_QueryValueReport(mongo_connection *conn,char *keyHash,char *lday,char *lmonth,char *lyear, int sort, char *classRegex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 double rkept,rnotkept,rrepaired;
 char rday[CF_MAXVARSIZE],rmonth[CF_MAXVARSIZE],ryear[CF_MAXVARSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rhandle[CF_MAXVARSIZE],noteid[CF_MAXVARSIZE];
 int match_day,match_month,match_year,found = false;
 char classRegexAnch[CF_MAXVARSIZE];
 int emptyQuery = true;
 time_t rt;
  
/* BEGIN query document */
 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

 // Turn start_time into Day Month Year
  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_valuereport,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_valuereport) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));
	 
          while (bson_iterator_next(&it2))
             {
	     snprintf(noteid,CF_MAXVARSIZE,"%s",CF_NONOTE);
	     snprintf(rhandle,CF_MAXVARSIZE,"%s",bson_iterator_key(&it2));
	     bson_iterator_init(&it3, bson_iterator_value(&it2));
	    
             rday[0] = '\0';
             rkept = 0;
             rnotkept = 0;
             rrepaired = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_day) == 0)
                   {
                   strncpy(rday,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_kept) == 0)
                   {
                   rkept = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_notkept) == 0)
                   {
                   rnotkept = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_repaired) == 0)
                   {
                   rrepaired = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfn_nid) == 0)
                   {
                   snprintf(noteid,CF_MAXVARSIZE,"%s",bson_iterator_string(&it3));
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in value report",bson_iterator_key(&it3));
                   }
                }

             match_day = match_month = match_year = true;
            
             if (lday && (strcmp(lday,rday) != 0))
                {
                match_day = false;
                }
            
             if (lmonth && (strcmp(lmonth,rmonth) != 0))
                {
                match_month = false;
                }

             if (lyear && (strcmp(lyear,ryear) != 0))
                {
                match_year = false;
                }
            
             if (match_day && match_month && match_year)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubValue(CF_THIS_HH,rday,rkept,rrepaired,rnotkept,noteid,rhandle));
                }
             }
          }   
       }


    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubValue *hs = (struct HubValue *)rp->item;

          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if(sort)
    {
    record_list = SortRlist(record_list,SortBusinessValue);
    }


 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryBundleSeen(mongo_connection *conn, char *keyHash, char *lname,int regex, char *classRegex, int sort)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 double rago,ravg,rdev;
 char rname[CF_MAXVARSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],noteid[CF_BUFSIZE];
 int match_name,found = false;
 int emptyQuery = true;
 char classRegexAnch[CF_MAXVARSIZE];
 time_t rt;
  
/* BEGIN query document */

 bson_buffer_init(&bb);

 if (!EMPTY(keyHash))
    {
    bson_append_string(&bb,cfr_keyhash,keyHash);
    emptyQuery = false;
    }

 if(!EMPTY(classRegex))
    {
    AnchorRegex(classRegex,classRegexAnch,sizeof(classRegexAnch));
    bson_append_regex(&bb,cfr_class_keys,classRegexAnch,"");
    emptyQuery = false;
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_bundles,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 if(!emptyQuery)
    {
    bson_destroy(&query);
    }


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    rname[0] = '\0';
    found = false;
    rt = 0;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_bundles) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rname,bson_iterator_key(&it2),CF_MAXVARSIZE-1);
             snprintf(noteid,CF_MAXVARSIZE,"%s",CF_NONOTE);

             ravg = 0;
             rdev = 0;
             rago = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_hrsavg) == 0)
                   {
                   ravg = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_hrsdev) == 0)
                   {
                   rdev = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_hrsago) == 0)
                   {
                   rago = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                   {
                   rt = bson_iterator_int(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfn_nid) == 0)
                   {
		   snprintf(noteid,CF_MAXVARSIZE,"%s",bson_iterator_string(&it3)); 
                   }
                else
                   {
                   CfOut(cf_inform,"", " !! Unknown key \"%s\" in bundle seen",bson_iterator_key(&it3));
                   }
                }

             match_name = true;
            
             if (regex)
                {
                if (!EMPTY(lname) && !FullTextMatch(lname,rname))
                   {
                   match_name = false;
                   }
                }
             else
                {
                if (!EMPTY(lname) && (strcmp(lname,rname) != 0))
                   {
                   match_name = false;
                   }
                }
            
             if (match_name)
                {
                found = true;
                rp = PrependRlistAlien(&record_list,NewHubBundleSeen(CF_THIS_HH,rname,rago,ravg,rdev,rt,noteid));
                }            
             }
          }   
       }

    if (found)
       {
       hh = NewHubHost(keyhash,addresses,hostnames);
       PrependRlistAlien(&host_list,hh);

       // Now cache the host reference in all of the records to flatten the 2d list
       for (rp = record_list; rp != NULL; rp=rp->next)
          {
          struct HubBundleSeen *hs = (struct HubBundleSeen *)rp->item;

          if (hs->hh == CF_THIS_HH)
             {
             hs->hh = hh;
             }
          }
       }
    }

 if (sort)
    {
    record_list = SortRlist(record_list,SortBundleSeen);
    }
 

 mongo_cursor_destroy(cursor);
 return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

int CFDB_QueryMagView(mongo_connection *conn,char *keyhash,enum observables obs,time_t start_time,double *qa,double *ea,double *da)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qu,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 int ok = false,slot,start_slot,wrap_around;
 double q,e,d;
  
/* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,keyhash);
 bson_append_int(&b,cfm_magobs,obs);
 bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfm_data,1);
 bson_from_buffer(&field, &bb);

/* Check from wrap around */

 start_slot = GetTimeSlot(start_time);

// Check that start + 4 hours is not greater than the week buffer

 wrap_around = (int)start_slot + CF_MAGDATA - CF_MAX_SLOTS;

// Initialize as there might be missing values

 for (slot = 0; slot < CF_MAGDATA; slot++)
    {
    qa[slot] = -1;
    ea[slot] = 0;
    da[slot] = 0;
    }

/* BEGIN SEARCH */

 cursor = mongo_find(conn,MONGO_DATABASE_MON,&query,&field,0,0,0);
 bson_destroy(&query);
 bson_destroy(&field);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    while (bson_iterator_next(&it1))
       {
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfm_data) == 0)
          {
          int st = 0;
          slot = 0;
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));
             sscanf(bson_iterator_key(&it2),"%d",&st);

             // Select the past 4 hours
            
             if (wrap_around >= 0)
                {
                if (st >= wrap_around || st < start_slot)
                   {
                   continue;
                   }
                }
             else
                {
                if (st < start_slot || st >= start_slot + CF_MAGDATA)
                   {
                   continue;
                   }
                }

             ok = true; // Have some relevant data
             q = e = d = 0;

             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_obs_q) == 0)
                   {
                   q = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   e = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   d = bson_iterator_double(&it3);
                   }
                }

             qa[Nova_MagViewOffset(start_slot,st,wrap_around)] = q;
             ea[Nova_MagViewOffset(start_slot,st,wrap_around)] = e;
             da[Nova_MagViewOffset(start_slot,st,wrap_around)] = d;
             }
          }
       }
    }

// Now we should transform the data to re-order during wrap-around,
// since at the boundary the data come in the wrong order

 mongo_cursor_destroy(cursor);
 return ok;
}

/*****************************************************************************/

int Nova_MagViewOffset(int start_slot,int db_slot,int wrap)
{
 int offset = CF_MAX_SLOTS - start_slot;

// Offset is the non-wrapped data size

 if (wrap >= 0 && db_slot < start_slot)
    {
    return offset + db_slot; // assumes db_slot is now < CF_MAGDATA
    }
 else
    {
    return db_slot - start_slot;
    }
}

/*****************************************************************************/

int CFDB_QueryHostCount(mongo_connection *conn)
/**
 * Counts number of hosts.
 * Can be extended to support some query criteria.
 *
 **/
{
 bson query;

 bson_empty(&query);
 
 return (int)mongo_count(conn, MONGO_BASE, "hosts", &query);
}

/*****************************************************************************/

int CFDB_QueryHostName(mongo_connection *conn, char *ipAddr, char *hostName, int hostNameSz)
/*
 * Scan DB to try to find the hostname of the given IP.
 * Falls back to ip addres (parameter).
 */
{
 bson_buffer bb;
 bson query,field;
 bson_iterator it1,it2;
 mongo_cursor *cursor;
 int ret = false;

 // query
 bson_buffer_init(&bb);
 bson_append_string(&bb,cfr_ip_array,ipAddr);
 bson_from_buffer(&query,&bb);

 // result
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_host_array,1);
 bson_from_buffer(&field, &bb);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
 bson_destroy(&query);
 bson_destroy(&field);

 if(mongo_cursor_next(cursor))  // take first match
    {
    bson_iterator_init(&it1,cursor->current.data);
      
    while (bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1),cfr_host_array) == 0)
	  {
          bson_iterator_init(&it2,bson_iterator_value(&it1));
	    
          if(bson_iterator_next(&it2))
             {
             snprintf(hostName,hostNameSz,"%s",bson_iterator_string(&it2));
             ret = true;
             break;
             }
	    
	  }
       }

    }

 mongo_cursor_destroy(cursor);

 // fallback
 if(ret == false)
    {
    snprintf(hostName,hostNameSz,"%s",ipAddr);
    }

 return ret;

}

/*****************************************************************************/

int CFDB_QueryLastUpdate(mongo_connection *conn,char *keyhash,time_t *date)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qu,query,field;
 mongo_cursor *cursor;
 bson_iterator it1;
 char search_name[CF_MAXVARSIZE];
 int ok = false,slot,start_slot,wrap_around;
 double q,e,d;
  
/* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,keyhash);
 bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */

 snprintf(search_name,CF_MAXVARSIZE-1,"%s",cfr_day);

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_day,1);
 bson_from_buffer(&field, &bb);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
 bson_destroy(&query);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while (bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1),cfr_day) == 0)
          {
          *date = (time_t)bson_iterator_int(&it1);
          }
       }
    }

// Now we should transform the data to re-order during wrap-around,
// since at the boundary the data come in the wrong order

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return ok;
}

/*****************************************************************************/

int CFDB_QueryWeekView(mongo_connection *conn,char *keyhash,enum observables obs,double *qa,double *ea,double *da)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qu,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 char search_name[CF_MAXVARSIZE];
 double q,e,d;
 int ok = false;
 time_t start_time = CF_MONDAY_MORNING;
 int slot;

// Initialize as there might be missing values

 for (slot = 0; slot < CF_TIMESERIESDATA; slot++)
    {
    qa[slot] = -1;
    ea[slot] = 0;
    da[slot] = 0;
    }

  
/* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,keyhash);
 bson_append_int(&b,cfm_weekobs,obs);
 bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfm_data,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 cursor = mongo_find(conn,MONGO_DATABASE_MON,&query,&field,0,0,0);
 bson_destroy(&query);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while (bson_iterator_next(&it1))
       {
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfm_data) == 0)
          {
          int st = 0, index = 0;
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));
             sscanf(bson_iterator_key(&it2),"%d",&st);

             // Select the past 4 hours
            
             q = e = d = 0;
             ok  = true;

             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_obs_q) == 0)
                   {
                   q = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   e = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   d = bson_iterator_double(&it3);
                   }
                }

             // Slot starts at 1 due to coarse graining loop
            
             index = st/12 - 1;

             qa[index] = q;
             ea[index] = e;
             da[index] = d;
             }
          }
       }
    }

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return ok;
}

/*****************************************************************************/

int CFDB_QueryYearView(mongo_connection *conn,char *keyhash,enum observables obs,double *qa,double *ea,double *da)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qu,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 double q,e,d;
 int ok = false;
  
/* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,keyhash);
 bson_append_int(&b,cfm_yearobs,obs);
 bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfm_data,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 cursor = mongo_find(conn,MONGO_DATABASE_MON,&query,&field,0,0,0);
 bson_destroy(&query);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while (bson_iterator_next(&it1))
       {
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfm_data) == 0)
          {
          int st = 0, index = 0;
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));
             sscanf(bson_iterator_key(&it3),"%d",&st);

             // Select the past 4 hours
            
             q = e = d = 0;

             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_obs_q) == 0)
                   {
                   q = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_E) == 0)
                   {
                   e = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_obs_sigma) == 0)
                   {
                   d = bson_iterator_double(&it3);
                   }
                }

             // Store 
            
             index = st;

             qa[index] = q;
             ea[index] = e;
             da[index] = d;
             }
          }
       }
    }

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return ok;
}

/*****************************************************************************/

int CFDB_QueryHistogram(mongo_connection *conn,char *keyhash,enum observables obs,double *histo)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qu,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 char search_name[CF_MAXVARSIZE];
 double q,e,d;
 int ok = false;
 time_t start_time = CF_MONDAY_MORNING;
  
/* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,keyhash);
 bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */

 snprintf(search_name,CF_MAXVARSIZE-1,"%s%d",cfr_histo,obs);

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,search_name,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);
 bson_destroy(&query);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    while (bson_iterator_next(&it1))
       {
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),search_name) == 0)
          {
          int st = 0, index = 0;
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             histo[index] = bson_iterator_double(&it2);
             index++;
             }
          }
       }
    }

 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return ok;
}

/*****************************************************************************/
/* Promises collections                                                      */
/*****************************************************************************/

struct HubPromise *CFDB_QueryPromise(mongo_connection *conn, char *handle, char *file, int lineNo)
/*
 * Returns all attribs of one promise by its handle XOR (file,lineno).
 */
{ bson_buffer bb;
 bson query,result;
 mongo_cursor *cursor,*cursor2;
 bson_iterator it1,it2;
 char bn[CF_MAXVARSIZE] = {0}, bt[CF_MAXVARSIZE] = {0},ba[CF_MAXVARSIZE] = {0},
                                                           pt[CF_MAXVARSIZE] = {0}, pr[CF_MAXVARSIZE] = {0}, pe[CF_MAXVARSIZE] = {0},
                                                                                                                 cl[CF_MAXVARSIZE] = {0}, ha[CF_MAXVARSIZE] = {0}, co[CF_MAXVARSIZE] = {0},
                                                                                                                                                                       fn[CF_MAXVARSIZE] = {0}, **cons = {0};
                                                                                                                                                                       char fileExp[CF_MAXVARSIZE] = {0};
                                                                                                                                                                       int lineExp = 0;
                                                                                                                                                                       int lno = -1;
                                                                                                                                                                       int i,constCount;
                                                                                                                                                                       int fileLineSearch = false;

  
                                                                                                                                                                       if(EMPTY(file))  // use handle by default, (file,lineNo) if handle is not found
                                                                                                                                                                          {
                                                                                                                                                                          fileLineSearch = false;
                                                                                                                                                                          }
                                                                                                                                                                       else
                                                                                                                                                                          {
                                                                                                                                                                          fileLineSearch = true;
                                                                                                                                                                          }  
  

/* BEGIN query document */

                                                                                                                                                                       bson_buffer_init(&bb);

                                                                                                                                                                       if(fileLineSearch)
                                                                                                                                                                          {
                                                                                                                                                                          bson_append_string(&bb,cfp_file,file);
                                                                                                                                                                          bson_append_int(&bb,cfp_lineno,lineNo);
                                                                                                                                                                          }
                                                                                                                                                                       else
                                                                                                                                                                          {
                                                                                                                                                                          bson_append_string(&bb,cfp_handle,handle);
                                                                                                                                                                          }

                                                                                                                                                                       bson_from_buffer(&query,&bb);


/* BEGIN SEARCH */
                                                                                                                                                                       cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,NULL,0,0,0);
                                                                                                                                                                       bson_destroy(&query);


                                                                                                                                                                       if (mongo_cursor_next(cursor))  // loops over documents
                                                                                                                                                                          {
                                                                                                                                                                          bson_iterator_init(&it1,cursor->current.data);
   
                                                                                                                                                                          while (bson_iterator_next(&it1))
                                                                                                                                                                             {
                                                                                                                                                                             if(strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(bn, sizeof(bn), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_bundletype) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(bt, sizeof(bt), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_bundleargs) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                bson_iterator_init(&it2,bson_iterator_value(&it1));
         
                                                                                                                                                                                memset(ba,0,sizeof(ba));
         
                                                                                                                                                                                bson_iterator_init(&it2,bson_iterator_value(&it1));
         
                                                                                                                                                                                while(bson_iterator_next(&it2))
                                                                                                                                                                                   {
                                                                                                                                                                                   if(strlen(ba) + strlen(bson_iterator_string(&it2)) < sizeof(ba))
                                                                                                                                                                                      {
                                                                                                                                                                                      strcat(ba,bson_iterator_string(&it2));
                                                                                                                                                                                      strcat(ba, ",");
                                                                                                                                                                                      }
                                                                                                                                                                                   else
                                                                                                                                                                                      {
                                                                                                                                                                                      break;
                                                                                                                                                                                      }
                                                                                                                                                                                   }
         
                                                                                                                                                                                if(ba[0] != '\0')
                                                                                                                                                                                   {
                                                                                                                                                                                   ba[strlen(ba)-1] = '\0';  // remove last comma
                                                                                                                                                                                   }
         
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_promisetype) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(pt, sizeof(pt), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_promiser) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(pr, sizeof(pr), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_promisee) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(pe, sizeof(pe), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_classcontext) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(cl, sizeof(cl), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_comment) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(co, sizeof(co), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_handle) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(ha, sizeof(ha), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_file) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                snprintf(fn, sizeof(fn), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_lineno) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                lno = bson_iterator_int(&it1);
                                                                                                                                                                                }
                                                                                                                                                                             else if(strcmp(bson_iterator_key(&it1), cfp_constraints) == 0)
                                                                                                                                                                                {
                                                                                                                                                                                bson_iterator_init(&it2,bson_iterator_value(&it1));
         
                                                                                                                                                                                // count constraints
                                                                                                                                                                                constCount = 0;
         
                                                                                                                                                                                while(bson_iterator_next(&it2))
                                                                                                                                                                                   {
                                                                                                                                                                                   constCount++;
                                                                                                                                                                                   }
         
                                                                                                                                                                                if(constCount == 0)
                                                                                                                                                                                   {
                                                                                                                                                                                   cons = NULL;
                                                                                                                                                                                   continue;
                                                                                                                                                                                   }
         
                                                                                                                                                                                // save constraints (freed in DeleteHubPromise)
                                                                                                                                                                                bson_iterator_init(&it2,bson_iterator_value(&it1));
                                                                                                                                                                                cons = malloc(sizeof(char *) * (constCount + 1));
         
                                                                                                                                                                                i = 0;  // race-safe check
         
                                                                                                                                                                                while(bson_iterator_next(&it2) && (i < constCount))
                                                                                                                                                                                   {
                                                                                                                                                                                   cons[i] = strdup(bson_iterator_string(&it2));
                                                                                                                                                                                   i++;
                                                                                                                                                                                   }
         
                                                                                                                                                                                cons[i] = NULL;           
                                                                                                                                                                                }
                                                                                                                                                                             }
                                                                                                                                                                          }
                                                                                                                                                                       else if(!fileLineSearch)  // if not found in unexpanded promise DB, try expanded
                                                                                                                                                                          {
                                                                                                                                                                          mongo_cursor_destroy(cursor);

                                                                                                                                                                          /* query */
                                                                                                                                                                          bson_buffer_init(&bb);
                                                                                                                                                                          bson_append_string(&bb,cfp_handle_exp,handle);
                                                                                                                                                                          bson_from_buffer(&query,&bb);   

                                                                                                                                                                          /* result */
                                                                                                                                                                          bson_buffer_init(&bb);
                                                                                                                                                                          bson_append_int(&bb,cfp_file,1);
                                                                                                                                                                          bson_append_int(&bb,cfp_lineno,1);
                                                                                                                                                                          bson_from_buffer(&result,&bb);   


                                                                                                                                                                          cursor2 = mongo_find(conn,MONGO_PROMISES_EXP,&query,&result,0,0,0);

                                                                                                                                                                          bson_destroy(&query);
                                                                                                                                                                          bson_destroy(&result);


                                                                                                                                                                          if (mongo_cursor_next(cursor2))  // loops over documents
                                                                                                                                                                             {
                                                                                                                                                                             bson_iterator_init(&it1,cursor2->current.data);
   
                                                                                                                                                                             while (bson_iterator_next(&it1))
                                                                                                                                                                                {
                                                                                                                                                                                if(strcmp(bson_iterator_key(&it1), cfp_file) == 0)
                                                                                                                                                                                   {
                                                                                                                                                                                   snprintf(fileExp, sizeof(fileExp), "%s", bson_iterator_string(&it1));
                                                                                                                                                                                   }
                                                                                                                                                                                else if(strcmp(bson_iterator_key(&it1), cfp_lineno) == 0)
                                                                                                                                                                                   {
                                                                                                                                                                                   lineExp = bson_iterator_int(&it1);
                                                                                                                                                                                   }
                                                                                                                                                                                }
                                                                                                                                                                             }

                                                                                                                                                                          mongo_cursor_destroy(cursor2);
   
                                                                                                                                                                          if(*fileExp != '\0' && lineExp != 0)
                                                                                                                                                                             {
                                                                                                                                                                             return CFDB_QueryPromise(conn,NULL,fileExp,lineExp);
                                                                                                                                                                             }
                                                                                                                                                                          else  // not found in expanded promise DB either
                                                                                                                                                                             {
                                                                                                                                                                             Debug("Promise handle \"%s\" not found in expanded promise DB", handle);
                                                                                                                                                                             return NULL;
                                                                                                                                                                             }

                                                                                                                                                                          }
                                                                                                                                                                       else  // not found in unexpanded DB by file and line either
                                                                                                                                                                          {
                                                                                                                                                                          mongo_cursor_destroy(cursor);
                                                                                                                                                                          return NULL;
                                                                                                                                                                          }

                                                                                                                                                                       mongo_cursor_destroy(cursor);

                                                                                                                                                                       return NewHubPromise(bn,bt,ba,pt,pr,pe,cl,ha,co,fn,lno,cons);
}

/*****************************************************************************/

int CFDB_QueryPromiseAttr(mongo_connection *conn, char *handle, char *attrKey, char *attrVal, int attrValSz)
/*
 * For the promise with the given handle, returns the given field
 * (e.g. comment, promisee, etc.)
 */
{
 bson_buffer b;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 int found = false;

 // query
 bson_buffer_init(&b);
 bson_append_string(&b,cfp_handle,handle);
 bson_from_buffer(&query,&b);

 // returned attribute
 bson_buffer_init(&b);
 bson_append_int(&b,attrKey,1);
 bson_from_buffer(&field,&b);


 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);
 bson_destroy(&query);
 bson_destroy(&field);

 if (mongo_cursor_next(cursor))  // take first doc should be (unique)
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), attrKey) == 0)
          {
          snprintf(attrVal, attrValSz, "%s", bson_iterator_string(&it1));
          found = true;
          break;
          }
       }
    }  

 mongo_cursor_destroy(cursor);

 return found;
}

/*****************************************************************************/

struct Item *CFDB_QueryExpandedPromiseAttr(mongo_connection *conn, char *handle, char *attrKey)
/*
 * For the promise with the given (expanded) handle, returns a list of
 * the given field (e.g. cfp_comment_exp, cfp_promisee_exp, etc.) expanded.  
 * MEMORY NOTE: Caller must free returned val (!=NULL) with DeleteItemList()
 */
{
 bson_buffer b;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *matched = {0};

 // query
 bson_buffer_init(&b);
 bson_append_string(&b,cfp_handle_exp,handle);
 bson_from_buffer(&query,&b);

// returned attribute
 bson_buffer_init(&b);
 bson_append_int(&b,attrKey,1);
 bson_from_buffer(&field,&b);


 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);
 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), attrKey) == 0)
          {

          // constraints are stored in an array
          if(strcmp(attrKey,cfp_constraints_exp) == 0)
             {
             bson_iterator_init(&it2,bson_iterator_value(&it1));

             while(bson_iterator_next(&it2))
                {
                IdempPrependItem(&matched,(char *)bson_iterator_string(&it2),NULL);
                }

             }
          else
             {
             IdempPrependItem(&matched,(char *)bson_iterator_string(&it1),NULL);
             }

          break;
          }
       }
    }  

 mongo_cursor_destroy(cursor);

 return matched;
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName, int regex, bool filter)
/*
 * Returns a set of handles of promises matching given promiser regex
 * XOR promise type XOR (bundle type, bundle name) XOR all.  All
 * promiser types of vars and classes, and bundle types of edit_line
 * and server may optinally be excluded since they often only lead to
 * mess in Knowledge Management. 
 */
{ bson_buffer bb, *obj, *arr;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 struct Rlist *recordList = NULL;
 bool emptyQuery = true;

 // query
 bson_empty(&query);
 bson_buffer_init(&bb);

 if(regex)
    {
    if (!EMPTY(promiser))
       {
       bson_append_regex(&bb, cfp_promiser, promiser,"");
       emptyQuery = false;
       }
    else if(!EMPTY(promiserType))
       {
       bson_append_regex(&bb, cfp_promisetype, promiserType,"");
       emptyQuery = false;
       }
    else if(!EMPTY(bType))
       {
       bson_append_regex(&bb,cfp_bundletype,bType,"");
       bson_append_regex(&bb,cfp_bundlename,bName,"");
       emptyQuery = false;
       }
    }
 else
    {
    if (!EMPTY(promiser))
       {
       bson_append_string(&bb, cfp_promiser, promiser);
       emptyQuery = false;
       }
    else if(!EMPTY(promiserType))
       {
       bson_append_string(&bb, cfp_promisetype, promiserType);
       emptyQuery = false;
       }
    else if(!EMPTY(bType))
       {
       bson_append_string(&bb,cfp_bundletype,bType);
       bson_append_string(&bb,cfp_bundlename,bName);
       emptyQuery = false;
       }
    }

 
 if(filter)
    {
    // filter promises of type vars and classes
    obj = bson_append_start_object(&bb, cfp_promisetype);
    arr = bson_append_start_array(obj, "$nin");

    bson_append_string(arr, "0", "vars");
    bson_append_string(arr, "1", "classes");
    
    bson_append_finish_object(arr);
    bson_append_finish_object(obj);

    // filter promises in edit_line and server bundles
    obj = bson_append_start_object(&bb, cfp_bundletype);
    arr = bson_append_start_array(obj, "$nin");

    bson_append_string(arr, "0", "edit_line");
    bson_append_string(arr, "1", "server");
    
    bson_append_finish_object(arr);
    bson_append_finish_object(obj);

    emptyQuery = false;
    }

 
 if(!emptyQuery)
    {
    bson_from_buffer(&query,&bb);
    }


// returned attribute
 bson_buffer_init(&bb);
 bson_append_int(&bb,cfp_handle,1);
 bson_from_buffer(&field,&bb);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

 bson_destroy(&query);  // ok for empty as well
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_handle) == 0)
          {
          PrependRlistAlien(&recordList,NewHubPromise(NULL,NULL,NULL,NULL,NULL,NULL,NULL,(char *)bson_iterator_string(&it1),NULL,NULL,0,NULL));
          }
       }
    }

 mongo_cursor_destroy(cursor);

 return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

struct Item *CFDB_QueryBundles(mongo_connection *conn,char *bTypeRegex,char *bNameRegex)
/*
 * Returns bundles "type name" matching the given regex.
 * Gets all if left unspecified.
 */

{ bson_buffer bbuf;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 int emptyQuery = true;
 struct Item *matched = {0};
 char type[CF_MAXVARSIZE] = {0};
 char name[CF_MAXVARSIZE] = {0};
 char match[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 
 if (!EMPTY(bTypeRegex))
    {
    bson_append_regex(&bbuf, cfp_bundletype, bTypeRegex,"");
    emptyQuery = false;
    }

 if (!EMPTY(bNameRegex))
    {
    bson_append_regex(&bbuf, cfp_bundlename, bNameRegex,"");
    emptyQuery = false;
    }

 if (emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bbuf);
    }

// returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_bundletype,1);
 bson_append_int(&bbuf,cfp_bundlename,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

 bson_destroy(&field);

 if (!emptyQuery)
    {
    bson_destroy(&query);
    }


 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
      
       if (strcmp(bson_iterator_key(&it1), cfp_bundletype) == 0)
          {
          snprintf(type,sizeof(type),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
          {
          snprintf(name,sizeof(name),"%s",bson_iterator_string(&it1));
          }
       }
   
    IdempPrependItem(&matched,name,type);
    }

 mongo_cursor_destroy(cursor);

 return matched;
}

/*****************************************************************************/

struct Rlist *CFDB_QueryBundleClasses(mongo_connection *conn, char *bType, char *bName)
/*
 * Returns the set of classes used in the given bundle.
 * MEMORY NOTE: Caller must free returned value with DeleteRlist()
 */
{ bson_buffer bbuf;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 struct Rlist *classList = {0}, *tmpList = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundletype,bType);
 bson_append_string(&bbuf,cfp_bundlename,bName);
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_classcontext,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_classcontext) == 0)
          {
          tmpList = SplitRegexAsRList((char *)bson_iterator_string(&it1),"[.!()|&]+",100,false);
          IdempAppendRlist(&classList,tmpList,CF_LIST);
          DeleteRlist(tmpList);
          }
       }
    }

 mongo_cursor_destroy(cursor);
 return classList;
}

/*****************************************************************************/

struct Item *CFDB_QueryBundleArgs(mongo_connection *conn, char *bType, char *bName)
/*
 * Returns the set of argument names for a given bundle, NULL if none.
 * MEMORY NOTE: Caller must free returned value (!= NULL) with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *args = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundletype,bType);
 bson_append_string(&bbuf,cfp_bundlename,bName);
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_bundleargs,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_bundleargs) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));
         
          while(bson_iterator_next(&it2))
             {
             IdempPrependItem(&args,(char *)bson_iterator_string(&it2),NULL);
             }
         
          break;  // all records show the same args
          }
       }
    }

 mongo_cursor_destroy(cursor);

 return args;
}

/*****************************************************************************/

struct Item *CFDB_QueryBundlesUsing(mongo_connection *conn, char *bNameReferenced)
/*
 * Returns the set of bundle names using the given bundle though
 * methods:usebundle, NULL if none.
 * Bundle types are implicitly agent (both referencing and referees).
 * MEMORY NOTE: Caller must free returned value (!= NULL) with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *bNameReferees = {0};
 char queryConstr[CF_MAXVARSIZE];

 snprintf(queryConstr,sizeof(queryConstr),"usebundle => %s",bNameReferenced);

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_constraints,queryConstr);
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_bundlename,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
          {
          IdempPrependItem(&bNameReferees,(char *)bson_iterator_string(&it1),"agent");
          }
       }
    }

 mongo_cursor_destroy(cursor);

 return bNameReferees;
}

/*****************************************************************************/

int CFDB_QueryBundleCount(mongo_connection *conn)
/**
 * Returns the number of bundles (may appear multiple times in the
 * promises DB).
 **/
{ bson_buffer bbuf;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *bundleNames = {0};
 int bundleCount = 0;

 // query all

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_bundlename,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,bson_empty(&query),&field,0,0,0);

 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
          {
          IdempPrependItem(&bundleNames,(char *)bson_iterator_string(&it1),NULL);
          }
       }
    }

 mongo_cursor_destroy(cursor);

 bundleCount = ListLen(bundleNames);

 DeleteItemList(bundleNames);

 return bundleCount;
}

/*****************************************************************************/

int CFDB_QueryPromiseCount(mongo_connection *conn)
{ bson_buffer bbuf;
 bson_iterator it1;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *bundleNames = {0};
 int promiseCount = 0;

 promiseCount = (int)mongo_count(conn,MONGO_BASE,MONGO_PROMISES_UNEXP_COLLECTION,NULL);
  
 return promiseCount;
}


/*****************************************************************************/

struct HubBody *CFDB_QueryBody(mongo_connection *conn, char *type, char *name)

/*
 * Returns all attribs of one body by its type and name.
 * MEMORY NOTE: Caller must use DeleteHubBody() on the reutrned val (!=NULL)
 */

{ bson_buffer b;
 bson query;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 char ba[CF_MAXVARSIZE] = {0}, cc[CF_MAXVARSIZE] = {0};
 char lval[CF_MAXVARSIZE] = {0}, rval[CF_MAXVARSIZE] = {0};
 struct HubBody *hb = {0};
  
 /* BEGIN query document */

 bson_buffer_init(&b);
 bson_append_string(&b,cfb_bodytype,type);
 bson_append_string(&b,cfb_bodyname,name);
 bson_from_buffer(&query,&b);

/* BEGIN SEARCH */
 cursor = mongo_find(conn,MONGO_BODIES,&query,NULL,0,0,0);
 bson_destroy(&query);

 if (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    hb = NewHubBody(type,name,NULL);
   
    while (bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfb_bodyargs) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));
         
          memset(ba,0,sizeof(ba));
         
          bson_iterator_init(&it2,bson_iterator_value(&it1));
         
          while(bson_iterator_next(&it2))
             {
             if (strlen(ba) + strlen(bson_iterator_string(&it2)) < sizeof(ba))
                {
                strcat(ba,bson_iterator_string(&it2));
                strcat(ba, ",");
                }
             else
                {
                break;
                }
             }
         
          if(ba[0] != '\0')
             {
             ba[strlen(ba)-1] = '\0';  // remove last comma
             hb->bodyArgs = strdup(ba);  // freed in DeleteHubBody()
             }
         
          }
       else if(strcmp(bson_iterator_key(&it1), cfb_classcontext) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             snprintf(cc,sizeof(cc),"%s",bson_iterator_key(&it2));
	   
             bson_iterator_init(&it3,bson_iterator_value(&it2));
	     
             while(bson_iterator_next(&it3))
                {
                snprintf(lval,sizeof(lval),"%s",bson_iterator_key(&it3));
                snprintf(rval,sizeof(rval),"%s",bson_iterator_string(&it3));
              
                NewHubBodyAttr(hb,lval,rval,cc);
                }	     
             }
          }
       }
    }

 mongo_cursor_destroy(cursor);

 return hb;
}

/*****************************************************************************/

struct Item *CFDB_QueryAllBodies(mongo_connection *conn,char *bTypeRegex,char *bNameRegex)

/*
 * Returns all attribs of one body by its type and name.
 * MEMORY NOTE: Caller must use DeleteHubBody() on the reutrned val (!=NULL)
 */

{ bson_buffer b;
 bson_buffer bbuf; 
 bson query,field;
 int emptyQuery = true,found;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 char ba[CF_MAXVARSIZE] = {0}, cc[CF_MAXVARSIZE] = {0};
 char lval[CF_MAXVARSIZE] = {0}, rval[CF_MAXVARSIZE] = {0};
 struct HubBody *hb = {0};
 char type[CF_MAXVARSIZE] = {0};
 char name[CF_MAXVARSIZE] = {0};
 struct Item *record_list = NULL;

 /* BEGIN query document */

 bson_buffer_init(&bbuf);
 
 if (!EMPTY(bTypeRegex))
    {
    bson_append_regex(&bbuf,cfb_bodytype,bTypeRegex,"");
    emptyQuery = false;
    }

 if (!EMPTY(bNameRegex))
    {
    bson_append_regex(&bbuf,cfb_bodyname,bNameRegex,"");
    emptyQuery = false;
    }

 if (emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bbuf);
    }

// returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfb_bodytype,1);
 bson_append_int(&bbuf,cfb_bodyname,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_BODIES,&query,&field,0,0,0);

 bson_destroy(&field);

 if (!emptyQuery)
    {
    bson_destroy(&query);
    }

/* BEGIN SEARCH */

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);
    found = false;

    while (bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1),cfb_bodyname) == 0)
          {
          strncpy(name,(char *)bson_iterator_string(&it1),CF_MAXVARSIZE-1);
          }
       else if (strcmp(bson_iterator_key(&it1),cfb_bodytype) == 0)
          {
          strncpy(type,(char *)bson_iterator_string(&it1),CF_MAXVARSIZE-1);
          }

       found = true;
       }

    if (found)
       {
       PrependItem(&record_list,name,type);
       }
    }
             
 mongo_cursor_destroy(cursor);

 return record_list;
}

/*****************************************************************************/
/*                     Content-Driven Policy queries                         */
/*****************************************************************************/

struct Item *CFDB_QueryCdpAcls(mongo_connection *conn, char *sep)
/*
 * Returns all CDP ACLs from expanded policy as
 * "handle sep path sep aces sep owner sep action sep ifvarclass".
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 char path[CF_SMALLBUF] = {0};
 char aces[CF_SMALLBUF] = {0};
 char action[CF_SMALLBUF] = {0};
 char handle[CF_SMALLBUF] = {0};
 char owner[CF_SMALLBUF] = {0};
 char ifvarclass[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundlename,cfp_cdp_bundle_acls);
 bson_append_string(&bbuf,cfp_promisetype,"files");
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_promiser_exp,1);
 bson_append_int(&bbuf,cfp_handle_exp,1);
 bson_append_int(&bbuf,cfp_constraints_exp,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(handle,sizeof(handle),"(unknown)");
    snprintf(path,sizeof(path),"(unknown)");
    snprintf(aces,sizeof(aces),"(unknown)");
    snprintf(action,sizeof(action),"(unknown)");
    snprintf(owner,sizeof(owner),"(unknown)");
    snprintf(ifvarclass,sizeof(ifvarclass),"(unknown)");

   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
          {
          snprintf(path,sizeof(path),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
          {
          snprintf(handle,sizeof(handle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             if(strncmp(bson_iterator_string(&it2), "aces =>", 7) == 0)
                {
                StripListSep((char *)bson_iterator_string(&it2)+8,aces,sizeof(aces));
                }
             else if(strncmp(bson_iterator_string(&it2), "owners =>", 9) == 0)
                {
                GetStringListElement((char *)bson_iterator_string(&it2)+10,0,owner,sizeof(owner));
                }
             else if(strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                {
                snprintf(action,sizeof(action),"%s",bson_iterator_string(&it2) + 17);
                }
             else if(strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                {
                snprintf(ifvarclass,sizeof(ifvarclass),"%s",bson_iterator_string(&it2) + 14);
                }

             }
          }
       }
   
    snprintf(buf,sizeof(buf),"%s%s%s%s%s%s%s%s%s%s%s",
             handle,sep,path,sep,aces,sep,owner,sep,action,sep,ifvarclass);
    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}

/*****************************************************************************/

struct Item *CFDB_QueryCdpCommands(mongo_connection *conn, char *sep)
/*
 * Returns all CDP commands from expanded policy as
 * "handle sep command sep failclass sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 char handle[CF_SMALLBUF] = {0};
 char command[CF_SMALLBUF] = {0};
 char failClass[CF_SMALLBUF] = {0};
 char action[CF_SMALLBUF] = {0};
 char ifvarclass[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundlename,cfp_cdp_bundle_commands);
 bson_append_string(&bbuf,cfp_promisetype,"commands");
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_promiser_exp,1);
 bson_append_int(&bbuf,cfp_handle_exp,1);
 bson_append_int(&bbuf,cfp_constraints_exp,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(handle,sizeof(handle),"(unknown)");
    snprintf(command,sizeof(command),"(unknown)");
    snprintf(failClass,sizeof(failClass),"(unknown)");
    snprintf(action,sizeof(action),"(unknown)");
    snprintf(ifvarclass,sizeof(ifvarclass),"(unknown)");

   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
          {
          snprintf(command,sizeof(command),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
          {
          snprintf(handle,sizeof(handle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             if(strncmp(bson_iterator_string(&it2), "repair_failed =>", 16) == 0)
                {
                GetStringListElement((char *)bson_iterator_string(&it2)+17,0,failClass,sizeof(failClass));
                }
             else if(strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                {
                snprintf(action,sizeof(action),"%s",bson_iterator_string(&it2) + 17);
                }
             else if(strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                {
                snprintf(ifvarclass,sizeof(ifvarclass),"%s",bson_iterator_string(&it2) + 14);
                }

             }
          }
       }
   
    snprintf(buf,sizeof(buf),"%s%s%s%s%s%s%s%s%s",
             handle,sep,command,sep,failClass,sep,action,sep,ifvarclass);
    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}

/*****************************************************************************/

struct Item *CFDB_QueryCdpPromiser(mongo_connection *conn, char *sep, char *bundleName, char *promiseType)
/*
 * Returns all CDP promisers from expanded policy as
 * "handle sep promiser sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 char handle[CF_SMALLBUF] = {0};
 char path[CF_SMALLBUF] = {0};
 char ifvarclass[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundlename,bundleName);
 bson_append_string(&bbuf,cfp_promisetype,promiseType);
 bson_append_regex(&bbuf,cfp_handle_exp,"^cdp_.*","");
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_promiser_exp,1);
 bson_append_int(&bbuf,cfp_handle_exp,1);
 bson_append_int(&bbuf,cfp_constraints_exp,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(handle,sizeof(handle),"(unknown)");
    snprintf(path,sizeof(path),"(unknown)");
    snprintf(ifvarclass,sizeof(ifvarclass),"(unknown)");

   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
          {
          snprintf(path,sizeof(path),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
          {
          snprintf(handle,sizeof(handle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             if(strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                {
                snprintf(ifvarclass,sizeof(ifvarclass),"%s",bson_iterator_string(&it2) + 14);
                }

             }
          }
       }
   
    snprintf(buf,sizeof(buf),"%s%s%s%s%s",
             handle,sep,path,sep,ifvarclass);
    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}

/*****************************************************************************/

int CFDB_QueryLastFileChange(mongo_connection *conn, char *keyHash, char *reportType, char *fileName, char *outBuf, int outBufSz)
/*
 * Queries a file change / file diff report for a given host and file,
 * and writes the last time this file has been changed there to
 * outBuf (of size outBufSz).
 * No memory is allocated.
 * Returns true if a date is found, false otherwise.
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2,it3;
 bson query,field;
 mongo_cursor *cursor;
 char currFileName[CF_MAXVARSIZE];
 time_t currTime;
 time_t lastChange = 0;

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfr_keyhash,keyHash);
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,reportType,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 if(mongo_cursor_next(cursor))  // should be only one document
    {
    bson_iterator_init(&it1,cursor->current.data);

    while(bson_iterator_next(&it1))
       {
       if(strcmp(bson_iterator_key(&it1), reportType) == 0)
          {
	  bson_iterator_init(&it2,bson_iterator_value(&it1));
	  
	  while(bson_iterator_next(&it2))  // loops over report rows
             {
	      
             bson_iterator_init(&it3,bson_iterator_value(&it2));

             currFileName[0] = '\0';
             currTime = 0;

             while(bson_iterator_next(&it3))
		{
                if (strcmp(bson_iterator_key(&it3), cfr_name) == 0)
                   {
                   snprintf(currFileName,sizeof(currFileName),"%s",bson_iterator_string(&it3));
                   }
                else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                   {
                   currTime = bson_iterator_int(&it3);
                   }
		}

             if(strcmp(currFileName,fileName) == 0 && currTime > lastChange)
		{
		lastChange = currTime;
		}
             }
          }
       }
   
    }

 mongo_cursor_destroy(cursor);

 if(lastChange == 0)
    {
    snprintf(outBuf,outBufSz,"%s", "(never changed)");
    }
 else
    {
    snprintf(outBuf,outBufSz,"%s",cf_ctime(&lastChange)); 

    // ctime makes newline for some reason...

    if(outBuf[strlen(outBuf) - 1] == '\n')
       {
       outBuf[strlen(outBuf) - 1] = '\0';
       }

    }

 return (lastChange != 0);
}

/*****************************************************************************/

struct Item *CFDB_QueryCdpRegistry(mongo_connection *conn, char *sep)
/*
 * Returns all CDP registry from expanded policy as
 * "handle sep key sep value sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 char handle[CF_SMALLBUF] = {0};
 char key[CF_SMALLBUF] = {0};
 char value[CF_SMALLBUF] = {0};
 char action[CF_SMALLBUF] = {0};
 char ifvarclass[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundlename,cfp_cdp_bundle_registry);
 bson_append_string(&bbuf,cfp_promisetype,"databases");
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_promiser_exp,1);
 bson_append_int(&bbuf,cfp_handle_exp,1);
 bson_append_int(&bbuf,cfp_constraints_exp,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(handle,sizeof(handle),"(unknown)");
    snprintf(key,sizeof(key),"(unknown)");
    snprintf(value,sizeof(value),"(unknown)");
    snprintf(action,sizeof(action),"(unknown)");
    snprintf(ifvarclass,sizeof(ifvarclass),"(unknown)");

   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
          {
          snprintf(key,sizeof(key),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
          {
          snprintf(handle,sizeof(handle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             if(strncmp(bson_iterator_string(&it2), "database_rows =>", 16) == 0)
                {
                GetStringListElement((char *)bson_iterator_string(&it2)+17,0,value,sizeof(value));
                }
             else if(strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                {
                snprintf(action,sizeof(action),"%s",bson_iterator_string(&it2) + 17);
                }
             else if(strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                {
                snprintf(ifvarclass,sizeof(ifvarclass),"%s",bson_iterator_string(&it2) + 14);
                }

             }
          }
       }
   
    snprintf(buf,sizeof(buf),"%s%s%s%s%s%s%s%s%s",
             handle,sep,key,sep,value,sep,action,sep,ifvarclass);

    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}

/*****************************************************************************/

struct Item *CFDB_QueryCdpServices(mongo_connection *conn, char *sep)
/*
 * Returns all CDP Services from expanded policy as
 * "handle sep servicename sep servicepolicy sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 char handle[CF_SMALLBUF] = {0};
 char serviceName[CF_SMALLBUF] = {0};
 char servicePolicy[CF_SMALLBUF] = {0};
 char action[CF_SMALLBUF] = {0};
 char ifvarclass[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfp_bundlename,cfp_cdp_bundle_services);
 bson_append_string(&bbuf,cfp_promisetype,"services");
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfp_promiser_exp,1);
 bson_append_int(&bbuf,cfp_handle_exp,1);
 bson_append_int(&bbuf,cfp_constraints_exp,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_PROMISES_EXP,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(handle,sizeof(handle),"(unknown)");
    snprintf(serviceName,sizeof(serviceName),"(unknown)");
    snprintf(servicePolicy,sizeof(servicePolicy),"(unknown)");
    snprintf(action,sizeof(action),"(unknown)");
    snprintf(ifvarclass,sizeof(ifvarclass),"(unknown)");

   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
          {
          snprintf(serviceName,sizeof(serviceName),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
          {
          snprintf(handle,sizeof(handle),"%s",bson_iterator_string(&it1));
          }
       else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
             if(strncmp(bson_iterator_string(&it2), "service_policy =>", 17) == 0)
                {
                snprintf(servicePolicy,sizeof(servicePolicy),"%s",bson_iterator_string(&it2) + 18);
                }
             else if(strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                {
                snprintf(action,sizeof(action),"%s",bson_iterator_string(&it2) + 17);
                }
             else if(strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                {
                snprintf(ifvarclass,sizeof(ifvarclass),"%s",bson_iterator_string(&it2) + 14);
                }

             }
          }
       }
   
    snprintf(buf,sizeof(buf),"%s%s%s%s%s%s%s%s%s",
             handle,sep,serviceName,sep,servicePolicy,sep,action,sep,ifvarclass);
    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}

/*****************************************************************************/

struct Item *CFDB_QueryCdpCompliance(mongo_connection *conn, char *handle)
/*
 * Returns all CDP Compliance host entries as
 * "hostkeyhash;host;status;time_t"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{ bson_buffer bbuf;
 bson_iterator it1,it2,it3;
 bson query,field;
 mongo_cursor *cursor;
 struct Item *retList = {0};
 time_t t;
 char host[CF_SMALLBUF] = {0};
 char hostKeyHash[CF_SMALLBUF] = {0};
 char status = {0};
 char time[CF_SMALLBUF] = {0};
 char buf[CF_MAXVARSIZE] = {0};

 // query
 bson_buffer_init(&bbuf);
 bson_append_string(&bbuf,cfr_promisecompl_keys,handle);
 bson_from_buffer(&query,&bbuf);

 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfr_promisecompl,1);
 bson_append_int(&bbuf,cfr_keyhash,1);
 bson_append_int(&bbuf,cfr_host_array,1);
 bson_from_buffer(&field,&bbuf);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    // make sure everything gets defined
    snprintf(host,sizeof(host),"(unknown)");
    snprintf(hostKeyHash,sizeof(hostKeyHash),"(unknown)");
    snprintf(time,sizeof(time),"(unknown)");
    status = '?';
    t = 0;
   
    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfr_promisecompl) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while(bson_iterator_next(&it2))
             {
	     if(strcmp(bson_iterator_key(&it2), handle) == 0)
                {
	        bson_iterator_init(&it3,bson_iterator_value(&it2));
	     
		while(bson_iterator_next(&it3))
                   {
                   if (strcmp(bson_iterator_key(&it3), cfr_promisestatus) == 0)
                      {
                      status = *bson_iterator_string(&it3);
                      }
                   else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                      {
                      t = bson_iterator_int(&it3);
                      snprintf(time,sizeof(time),"%lu",t);
                      }
                   }
                }
             }
          }
       else if (strcmp(bson_iterator_key(&it1), cfr_host_array) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          if(bson_iterator_next(&it2))  // just pick first hostname
             {
	     snprintf(host,sizeof(host),"%s",bson_iterator_string(&it2));
             }
          }
       else if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
          {
          snprintf(hostKeyHash,sizeof(hostKeyHash),"%s",bson_iterator_string(&it1));
          }
       }
   
    snprintf(buf,sizeof(buf),"%s;%s;%c;%s",hostKeyHash,host,status,time);

    PrependItem(&retList,buf,NULL);
    }

 mongo_cursor_destroy(cursor);
 return retList;
}


/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void CFDB_ScanHubHost(bson_iterator *it1,char *keyhash,char *ipaddr,char *hostnames)

{ bson_iterator it2;
  
 int ipFound = false;
 int hostFound = false;

 if (bson_iterator_type(it1) == bson_string && strcmp(bson_iterator_key(it1),cfr_keyhash) == 0)
    {         
    strncpy(keyhash,bson_iterator_string(it1),CF_MAXVARSIZE-1);
    }

 if (strcmp(bson_iterator_key(it1),cfr_ip_array) == 0)
    {         
    bson_iterator_init(&it2, bson_iterator_value(it1));
   
    while (bson_iterator_next(&it2))
       {
       ipFound = true;
       Join(ipaddr,(char *)bson_iterator_string(&it2),CF_MAXVARSIZE);
       Join(ipaddr,", ",CF_MAXVARSIZE);
       }
    }

 if (strcmp(bson_iterator_key(it1),cfr_host_array) == 0)
    {         
    bson_iterator_init(&it2, bson_iterator_value(it1));
   
    while (bson_iterator_next(&it2))
       {
       hostFound = true;
       Join(hostnames,(char *)bson_iterator_string(&it2),CF_MAXVARSIZE);
       Join(hostnames,", ",CF_MAXVARSIZE);
       }
    }

// remove any trailing ", "

 if(ipFound)
    {
    if(ipaddr[strlen(ipaddr) - 2] == ',' &&
       ipaddr[strlen(ipaddr) - 1] == ' ')
       {
       ipaddr[strlen(ipaddr) - 2] = '\0';
       }
    }


 if(hostFound)
    {
    if(hostnames[strlen(hostnames) - 2] == ',' &&
       hostnames[strlen(hostnames) - 1] == ' ')
       {
       hostnames[strlen(hostnames) - 2] = '\0';
       }
    }

}

/*****************************************************************************/

int QueryHostsWithClass(mongo_connection *conn, bson_buffer *bb, char *classRegex)
/**
 * Appends to bb the keyhash of hosts matching classRegex.
 * Useful for "joins".
 **/
{
 bson_iterator it1;
 bson_buffer bbuf;
 bson query, field;
 mongo_cursor *cursor;
 int found = false;
 char khMatches[16384];  /* each keyhash is ~70 bytes: size / 70
			    gives max hosts returned */

 // query
 bson_buffer_init(&bbuf);
 bson_append_regex(&bbuf,cfr_class_keys,classRegex,"");
 bson_from_buffer(&query,&bbuf);
  
 // returned attribute
 bson_buffer_init(&bbuf);
 bson_append_int(&bbuf,cfr_keyhash,1);
 bson_from_buffer(&field,&bbuf);
  
 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&query);
 bson_destroy(&field);
  
 khMatches[0] = '\0';
  
 while(mongo_cursor_next(cursor))  // iterate over docs
    {
    bson_iterator_init(&it1,cursor->current.data);

    while(bson_iterator_next(&it1))
       {
       if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
          {
          Join(khMatches,(char *)bson_iterator_string(&it1),sizeof(khMatches));
          Join(khMatches,"|",sizeof(khMatches));
          found = true;
          }
       }
    }
  
 mongo_cursor_destroy(cursor);

 if(found)
    {
    // remove trailing |
    khMatches[strlen(khMatches) - 1] = '\0';
    bson_append_regex(bb,cfr_keyhash,khMatches,"");

    Debug("QueryHostsWithClass regex=\"%s\"\n", khMatches);
    }

 return found;
}

/*****************************************************************************/

int QueryInsertHostInfo(mongo_connection *conn, struct Rlist *host_list)
/**
 * Do a db lookup from keyhash to (hostname,ip) for all hosts in the list.
 **/
{
 bson_buffer bb;
 bson query,field;
 struct Rlist *rp;
 struct HubHost *hh;
 mongo_cursor *cursor;
 bson_iterator it1;
 char keyHash[CF_MAXVARSIZE],hostNames[CF_MAXVARSIZE],ipAddrs[CF_MAXVARSIZE];
  
 if(host_list == NULL)
    {
    return false;
    }

 // use empty query for now - filter result manually

 bson_empty(&query);

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_from_buffer(&field,&bb);

 cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

 bson_destroy(&field);

 while(mongo_cursor_next(cursor))  // loops over documents
    {
    keyHash[0] = '\0';
    ipAddrs[0] = '\0';
    hostNames[0] = '\0';
      
    bson_iterator_init(&it1,cursor->current.data);
      
    while(bson_iterator_next(&it1))
       {
       CFDB_ScanHubHost(&it1,keyHash,ipAddrs,hostNames);
       }
      
    hh = GetHubHostIn(host_list,keyHash);
      
    if(hh)
       {
       if(*ipAddrs != '\0')
	  {
	  hh->ipaddr = strdup(ipAddrs);
	  }

       if(*hostNames != '\0')
	  {
	  hh->hostname = strdup(hostNames);
	  }
       }
    }

 mongo_cursor_destroy(cursor);
 
 return true;
}


/*****************************************************************************/

int CFDB_IteratorNext(bson_iterator *it, bson_type valType)

{
 if (bson_iterator_next(it))
    {
    if (bson_iterator_type(it) != valType)
       {
       CfOut(cf_verbose,"", "!! CFDB value of unexpected type (was=%d,expected=%d)",bson_iterator_type(it), valType);
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

/*****************************************************************************/

void BsonToString(char *retBuf, int retBufSz, char *data)
/* data = (bson *)b->data*/
{
 bson_iterator i;
 const char * key;
 int temp;
 char oidhex[25];
 char buf[CF_MAXVARSIZE];

 memset(retBuf,0,retBufSz);
 bson_iterator_init( &i , data );

 while ( bson_iterator_next( &i ) ){

 bson_type t = bson_iterator_type( &i );

 if ( t == 0 )
    {
    break;
    }

 key = bson_iterator_key( &i );

 snprintf(buf,sizeof(buf),"%s : ",key);
 Join(retBuf,buf,retBufSz);
	

 switch ( t ){
 case bson_int: 
     snprintf(buf,sizeof(buf), "%d" , bson_iterator_int( &i ) ); 
     break;

 case bson_double: 
     snprintf(buf,sizeof(buf), "%f" , bson_iterator_double( &i ) ); 
     break;

 case bson_bool: 
     snprintf(buf,sizeof(buf), "%s" , bson_iterator_bool( &i ) ? "true" : "false" ); 
     break;

 case bson_string: 
     snprintf(buf,sizeof(buf), "%s" , bson_iterator_string( &i ) ); 
     break;

 case bson_null: 
     snprintf(buf,sizeof(buf), "null"); 
     break;

 case bson_oid: 
     bson_oid_to_string(bson_iterator_oid(&i), oidhex); 
     snprintf(buf,sizeof(buf), "%s" , oidhex ); 
     break;

 case bson_object:
     buf[0] = '{';
     BsonToString(buf+1, sizeof(buf-1), (char *)bson_iterator_value( &i ));
     EndJoin(buf,"}",sizeof(buf));
     break;

 case bson_array:

     buf[0] = '[';
     BsonToString(buf+1, sizeof(buf-1), (char *)bson_iterator_value( &i ));
     EndJoin(buf,"]",sizeof(buf));
     break;

 default:
     snprintf(buf,sizeof(buf) , "can't print type : %d\n" , t );
 }

 Join(retBuf,buf,retBufSz);
 Join(retBuf,", ",retBufSz);
 }
    
 retBuf[strlen(retBuf)-2] = 0;  // clear last comma
}

/*****************************************************************************/

void MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra)
{
 char dbErr[CF_MAXVARSIZE];
 bson b;

 if(!extra)
    {
    extra = "";
    }

 if(mongo_cmd_get_last_error(conn, MONGO_BASE, &b))
    {
    BsonToString(dbErr,sizeof(dbErr),b.data);
    CfOut(cf_error, "", "!! Database error on %s (%s): %s", operation,extra,dbErr);
    }
 
 bson_destroy(&b);

}

/*****************************************************************************/

struct HubQuery *CFDB_QueryCachedTotalCompliance(mongo_connection *conn, char *policy, time_t minGenTime)
{
 bson_buffer bb;
 bson_iterator it1,it2,it3;
 mongo_cursor *cursor;
 struct Rlist *record_list = NULL;
 char policyDB[CF_MAXVARSIZE];
 bson query,field;
 double kept,repaired,notkept;
 int slot,count;
 time_t genTime;

 bson_buffer_init(&bb);
 bson_append_string(&bb,cfc_cachetype,cfc_cachecompliance);
 bson_from_buffer(&query,&bb);

 cursor = mongo_find(conn,MONGO_CACHE,&query,bson_empty(&field),0,0,0);

 bson_destroy(&query);

 if (mongo_cursor_next(cursor))  // loops over cache types (want just one)
    {
    bson_iterator_init(&it1,cursor->current.data);

    while(bson_iterator_next(&it1))
       {

       if(bson_iterator_type(&it1) != bson_object)
          {
          continue;
          }

       snprintf(policyDB,sizeof(policyDB),"%s",bson_iterator_key(&it1));
	
       // if policy specified, retrieve only that one
       if(policy && strcmp(policyDB,policy) != 0) 
          {
          continue;
          }
      
       bson_iterator_init(&it2,bson_iterator_value(&it1));

       while(bson_iterator_next(&it2))
          {
          slot = -1;
          sscanf(bson_iterator_key(&it2), "%d", &slot);
	
          bson_iterator_init(&it3,bson_iterator_value(&it2));
	
          kept = 0;
          repaired = 0;
          notkept = 0;
          count = 0;
          genTime = 0;

          while(bson_iterator_next(&it3))
             {
             if(strcmp(bson_iterator_key(&it3),cfr_kept) == 0)
                {
		kept = bson_iterator_double(&it3);
                }
             else if(strcmp(bson_iterator_key(&it3),cfr_repaired) == 0)
                {
		repaired = bson_iterator_double(&it3);
                }
             else if(strcmp(bson_iterator_key(&it3),cfr_notkept) == 0)
                {
		notkept = bson_iterator_double(&it3);
                }
             else if(strcmp(bson_iterator_key(&it3),cfc_count) == 0)
                {
		count = bson_iterator_int(&it3);
                }
             else if(strcmp(bson_iterator_key(&it3),cfc_timegen) == 0)
                {
		genTime = bson_iterator_int(&it3);
                }
             }
          if(genTime >= minGenTime)
             {
             PrependRlistAlien(&record_list,NewHubCacheTotalCompliance(policyDB,slot,count,kept,repaired,notkept,genTime));
             }
          }
      
       }

    }

 return NewHubQuery(NULL, record_list);
}

/*****************************************************************************/

struct Rlist *CFDB_QueryNotes(mongo_connection *conn,char *keyhash, char *nid,  struct Item *data)

{ bson_buffer bb;
 bson b,query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubNoteInfo *hci = NULL;
 struct HubNote *hc = NULL, *tail=NULL;
 struct HubHost *hh;
 struct Rlist *ret = NULL, *host_list = NULL;

 char kh[CF_MAXVARSIZE] = {0},noteId[CF_MAXVARSIZE] = {0}, username[CF_MAXVARSIZE] = {0}, note[CF_BUFSIZE] = {0}, rptData[CF_BUFSIZE] = {0};
 char  fusername[CF_SMALLBUF] = {0};
 time_t datetime = -1,from = -1,to = -1;
 bson_oid_t bsonid;
 bson_type t;
 int emptyQuery = true, firstComment=false, specificQuery = false;

 if(BEGINSWITH(data->name,","))
    {
    sscanf(data->name + 1,"%ld,%ld\n",&from,&to);
    }
 else
    {
    sscanf(data->name,"%255[^','],%ld,%ld\n",fusername,&from,&to);
    }

 bson_buffer_init(&bb);

 if (!EMPTY(nid))
    {
    bson_oid_from_string(&bsonid,nid);
    bson_append_oid(&bb,"_id",&bsonid);
    emptyQuery = false;
    }
 else 
    {
    if (!EMPTY(keyhash))
       {
       bson_append_string(&bb,cfn_keyhash,keyhash);
       emptyQuery = false;
       }
        
    if (!EMPTY(fusername))
       {
       bson_append_string(&bb,"n.u",fusername);
       emptyQuery = false;
       specificQuery = true;
       }

    if(from >= 0 || to >= 0)
       {
       specificQuery = true;
       }
    }

 if(emptyQuery)
    {
    bson_empty(&query);
    }
 else
    {
    bson_from_buffer(&query,&bb);
    }

 bson_buffer_init(&bb);
 bson_append_int(&bb,"_id",1);
 bson_append_int(&bb,cfn_keyhash,1);
 bson_append_int(&bb,cfn_reportdata,1);
 bson_append_int(&bb,cfn_note,1);
 bson_from_buffer(&field, &bb);

 cursor = mongo_find(conn,MONGO_NOTEBOOK,&query,&field,0,0,0);
 bson_destroy(&field);
 bson_destroy(&query);

 while (mongo_cursor_next(cursor))  // loops over documents
    { 
    bson_iterator_init(&it1,cursor->current.data);
    kh[0] = '\0';
    username[0] = '\0';
    note[0] = '\0';
    noteId[0] = '\0';
    while (bson_iterator_next(&it1))
       {
       switch(bson_iterator_type(&it1))
          {
          case bson_oid:
	      if (strcmp(bson_iterator_key(&it1),"_id") == 0)
                 {
                 bson_oid_to_string(bson_iterator_oid(&it1),noteId);
                 }
	      break;
          case bson_string:
              if (strcmp(bson_iterator_key(&it1),cfn_keyhash) == 0)
                 {
                 strncpy(kh, bson_iterator_string(&it1),CF_MAXVARSIZE - 1);   		 
                 }
	      else if (strcmp(bson_iterator_key(&it1),cfn_reportdata) == 0)
                 {
                 strncpy(rptData, bson_iterator_string(&it1),CF_BUFSIZE - 1);
                 }
	      break;
          case bson_object:
          case bson_array:
              bson_iterator_init(&it2,bson_iterator_value(&it1));
	      
	      while (bson_iterator_next(&it2))
                 {
                 bson_iterator_init(&it3, bson_iterator_value(&it2));
                 while (bson_iterator_next(&it3))
		    {
                    t = bson_iterator_type( &it3 );
                    if ( t == 0 )
                       {
                       break;
                       }
                    switch(t)
                       {
                       case bson_string:
                           if (strcmp(bson_iterator_key(&it3),cfn_username) == 0)
                              {
			      strncpy(username, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                              }
                           else if (strcmp(bson_iterator_key(&it3),cfn_message) == 0)
                              {
			      strncpy(note, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                              }
                           break;
                       case bson_int:
                           if (strcmp(bson_iterator_key(&it3),cfn_datetime) == 0)
                              {
			      datetime = (time_t)bson_iterator_int(&it3);
			      //Define brackets for the start and end time eg. (from,to] => including to
                              }
                           break;
                       default:
                           CfOut(cf_inform,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                           break;
                       }
		    }
                 /* apply filter: username then datetime*/
                 if(specificQuery)
		    {
                    if(strcmp(username, fusername) != 0 && !EMPTY(fusername)) 
                       {
                       continue;			  			  
                       }
                    if (!(datetime <= to && datetime >= from) && from >= 0 && to >= 0)
                       {
                       continue;
                       }
		    }
		  
                 if (hci == NULL)
		    {	
                    hh = NewHubHost(kh,NULL,NULL);
                    PrependRlistAlien(&host_list,hh);		      
                    QueryInsertHostInfo(conn,host_list);
                    hci = NewHubNoteInfo(hh,noteId,username,note,datetime);
                    firstComment = true;
		    }
                 else 
		    {
                    hc = NewHubNote(username,note,datetime);
                    if(firstComment)
                       {
                       hci->note->next = hc;
                       firstComment=false;
                       }
                    else
                       {
                       tail->next=hc;
                       }
                    tail=hc;		      
		    }
                 }
	      if(hci)
                 {
                 AppendRlistAlien(&ret,hci);
                 }
      	      hci = NULL;
	      tail = NULL;
	      hc=NULL;
              break;
          default:
              break;
          }
       }
    }
 mongo_cursor_destroy(cursor);
 return ret;
}
/*****************************************************************************/

struct Rlist *CFDB_QueryNoteId(mongo_connection *conn,bson *query)

{ bson_buffer bb,*sub1,*sub2,*sub3;
 bson b,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
 char rname[CF_MAXVARSIZE],rversion[CF_MAXVARSIZE],rarch[3];
 char noteId[CF_MAXVARSIZE];
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
 int found = false;
  
/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfn_nid,1);
//bson_append_int(&bb,cfr_ip_array,1);
//bson_append_int(&bb,cfr_host_array,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 addresses[0] = '\0';

 cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);
 if(!cursor)
    {
    return NULL;
    }
 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    noteId[0] = '\0';
   
    while (bson_iterator_next(&it1))
       {
       switch(bson_iterator_type(&it1))
	  {
	  case bson_oid:

              break;
	  case bson_string:
              if (strcmp(bson_iterator_key(&it1),cfn_keyhash) == 0)
                 {
                 strncpy(keyhash, bson_iterator_string(&it1),CF_MAXVARSIZE - 1);
                 }
              else if (strcmp(bson_iterator_key(&it1),cfn_nid) == 0)
                 {
                 strncpy(noteId, bson_iterator_string(&it1),CF_MAXVARSIZE - 1);
                 }
              break;
	  }      
	
       }
    if(noteId && strlen(noteId)>15)
       {
       PrependRlistAlien(&host_list,noteId);
       }
    }
 bson_destroy(&field);
 mongo_cursor_destroy(cursor);
 return host_list;
}
/*****************************************************************************/

int CFDB_GetRow(mongo_connection *conn, char *db, bson *query, char *rowname, char *row, int rowSz, int level)

{ bson_buffer bb;
 bson field;
 bson_iterator it1,it2,it3,it4;
 mongo_cursor *cursor;
 char buffer[CF_BUFSIZE] = {0};
 bson_type t;

 if(strcmp(rowname,"*") == 0)
    {
    bson_empty(&field);
    }
 else
    {
    bson_buffer_init(&bb);
    bson_append_int(&bb,rowname,1);
    bson_from_buffer(&field, &bb);
    }

 cursor = mongo_find(conn,db,query,&field,0,0,0);
 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);
    if(level == 1)
       {
       BsonIteratorToString(buffer, sizeof(buffer), &it1, 1);
       snprintf(row,rowSz,"%s",buffer);
       return true;
       }
    if(level > 1)
       {
       while ( bson_iterator_next( &it1 ) )
          {
          t = bson_iterator_type( &it1 );
          if((t == bson_object || t == bson_array))
             {
             bson_iterator_init(&it2,bson_iterator_value(&it1));
             buffer[0] = '\0';
             if(level == 2)
                {
                BsonIteratorToString(buffer, sizeof(buffer), &it2, 1);
                snprintf(row,rowSz,"%s",buffer);
                return true;
                }
             if(level >2)
                {
                while ( bson_iterator_next( &it2 ) )
                   {
                   t = bson_iterator_type( &it2 );
                   if((t == bson_object || t == bson_array))
                      {
                      bson_iterator_init(&it3,bson_iterator_value(&it2));
                      buffer[0] = '\0';
                      if(level == 3)
                         {
                         BsonIteratorToString(buffer, sizeof(buffer), &it3, 1);
                         snprintf(row,rowSz,"%s",buffer);
                         return true;
                         }
                      }
                   }
                }
             }
          }
       }
    }
 return false;
}


/******************************************************************/

struct Item *CFDB_QueryDistinctStr(mongo_connection *conn, char *database, char *collection, char *dKey, char *qKey, char *qVal)
/**
 * Queries for distinct occurences of dKey in a query matching qKey = qVal
 */
{
 struct Item *retVal;
 bson_buffer bb;

 bson query;

 bson_buffer_init(&bb);
 bson_append_string(&bb, qKey, qVal);
 bson_from_buffer(&query, &bb);

 retVal = CFDB_QueryDistinct(conn, database, collection, dKey, &query);

 bson_destroy(&query);

 return retVal;
}

/******************************************************************/
 
struct Item *CFDB_QueryDistinct(mongo_connection *conn, char *database, char *collection, char *dKey, bson *queryBson)
{
 bson_buffer bb,*query;
 bson cmd,result;
 bson_iterator it1,values;
 struct Item *ret = NULL;
 
 bson_buffer_init(&bb);
 bson_append_string(&bb, "distinct", collection);
 bson_append_string(&bb, "key", dKey);
 

 if(queryBson)
    {
    bson_append_bson(&bb, "query", queryBson);
    }
 
 bson_from_buffer(&cmd, &bb);

 if (!mongo_run_command(conn, database, &cmd, &result))
    {
    MongoCheckForError(conn,"CFDB_QueryDistinct()", "");
    bson_buffer_destroy(&bb);
    bson_destroy(&cmd);
    return false;
    }

 bson_destroy(&cmd);

 if (!bson_find(&it1, &result, "values"))
    {
    CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryDistinct()");
    bson_destroy(&result);
    return false;
    }

 if (bson_iterator_type(&it1) != bson_array)
    {
    CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryDistinct()");
    bson_destroy(&result);
    return false;
    }

 bson_iterator_subiterator(&it1, &values);

 while (bson_iterator_next(&values))
    {
    PrependItem(&ret,(char *)bson_iterator_string(&values),NULL);
    }

 bson_destroy(&result);
 
 return ret;
}

/******************************************************************/

void BsonIteratorToString(char *retBuf, int retBufSz, bson_iterator *i, int depth)
/* NOTE: Only depth 1 is implemented */
{
 const char * key;
 int temp;
 char oidhex[25];
 char buf[CF_MAXVARSIZE];

 memset(retBuf,0,retBufSz);

 while ( bson_iterator_next( i ) ){

 bson_type t = bson_iterator_type( i );

 if ( t == 0 )
    {
    break;
    }

 key = bson_iterator_key( i );

 switch ( t ){
 case bson_int:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     snprintf(buf,sizeof(buf), "%d, " , bson_iterator_int( i ) );
     break;

 case bson_double:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     snprintf(buf,sizeof(buf), "%f, " , bson_iterator_double( i ) );
     break;

 case bson_bool:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     snprintf(buf,sizeof(buf), "%s, " , bson_iterator_bool( i ) ? "true" : "false" );
     break;
 case bson_string:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     snprintf(buf,sizeof(buf), "%s, " , bson_iterator_string( i ) );
     break;

 case bson_null:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     snprintf(buf,sizeof(buf), "null, ");
     break;

 case bson_oid:
     snprintf(buf,sizeof(buf),"%s : ",key);
     Join(retBuf,buf,retBufSz);
     bson_oid_to_string(bson_iterator_oid(i), oidhex);
     snprintf(buf,sizeof(buf), "%s, " , oidhex );
     break;

 case bson_object:
 case bson_array:
     memset(buf,0,sizeof(buf));
     break;

 default:
     break;
 }
 Join(retBuf,buf,retBufSz);
 //      Join(retBuf,", ",retBufSz);
 }

 retBuf[strlen(retBuf)-2] = 0;  // clear last comma
}
/*************************************************/
#endif  /* HAVE LIB_MONGOC */


