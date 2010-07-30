
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

void CFDB_GetValue(char *lval,char *rval,int size)

{ bson_buffer bb;
  bson b,field,query;
  bson_iterator it1,it2;
  mongo_cursor *cursor;
  mongo_connection conn;
 
if (!CFDB_Open(&conn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return;
   }
    
/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_SCRATCH,bson_empty(&query),0,0,0,0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   while(bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),lval) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            strncpy(rval,bson_iterator_string(&it2),size-1);
            mongo_cursor_destroy(cursor);
            CFDB_Close(&conn);
            return;
            }
         }
      }    
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
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
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char rname[CF_MAXVARSIZE],rversion[CF_MAXVARSIZE],rarch[3];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      }      

   hh = NewHubHost(keyhash,addresses,hostnames);
   AppendRlistAlien(&host_list,hh);
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,NULL);
}

/*****************************************************************************/

struct HubQuery *CFDB_QuerySoftware(mongo_connection *conn,bson *query,char *type,char *lname,char *lver,char *larch,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char rname[CF_MAXVARSIZE],rversion[CF_MAXVARSIZE],rarch[3];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
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
                  found = true;
                  AppendRlistAlien(&record_list,NewHubSoftware(NULL,rname,rversion,rarch));
                  }
               }               
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);
      
      // Now cache the host reference in all of the records to flatten the 2d list
      
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubSoftware *hs = (struct HubSoftware *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryClasses(mongo_connection *conn,bson *query,char *lclass,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  time_t rtime;
  double rsigma,rex;
  char rclass[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_class,found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in classes",bson_iterator_key(&it3));
                  }
               }

            match_class = true;
            
            if (regex)
               {
               if (lclass && !FullTextMatch(lclass,rclass))
                  {
                  match_class = false;
                  }
               }
            else
               {
               if (lclass && (strcmp(lclass,rclass) != 0))
                  {
                  match_class = false;
                  }
               }
            
            if (match_class)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubClass(NULL,rclass,rex,rsigma,rtime));
               }            
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryTotalCompliance(mongo_connection *conn,bson *query,char *lversion,time_t lt,int lkept,int lnotkept,int lrepaired,int cmp)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  int rkept,rnotkept,rrepaired,found = false;
  int match_kept,match_notkept,match_repaired,match_version,match_t;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rversion[CF_MAXVARSIZE];
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
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
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in total compliance",bson_iterator_key(&it3));
                  }
               }

            match_version = match_t = match_kept = match_notkept = match_repaired = true;

            if (lversion && !FullTextMatch(lversion,rversion))
               {
               match_version = false;
               }
            
            if (cmp == CFDB_GREATERTHANEQ)
               {
               if (lt != -1 && lt < rt)
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
               if (lt != -1 && lt > rt)
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
               AppendRlistAlien(&record_list,NewHubTotalCompliance(NULL,rt,rversion,rkept,rrepaired,rnotkept));
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);
      
      // Now cache the host reference in all of the records to flatten the 2d list
      
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubTotalCompliance *hs = (struct HubTotalCompliance *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryVariables(mongo_connection *conn,bson *query,char *lscope,char *llval,char *lrval,char *ltype,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3,it4,it5;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL,*newlist = NULL;
  int rkept,rnotkept,rrepaired,found = false;
  int match_type,match_scope,match_lval,match_rval;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rversion[CF_MAXVARSIZE];
  char rscope[CF_MAXVARSIZE], rlval[CF_MAXVARSIZE],mrval[CF_BUFSIZE],dtype[CF_MAXVARSIZE],rtype;
  void *rrval;
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
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
               bson_iterator_init(&it4,bson_iterator_value(&it3));

               strncpy(rlval,bson_iterator_key(&it3),CF_MAXVARSIZE-1);
               
               if (strcmp(bson_iterator_key(&it4),cfr_type) == 0)
                  {
                  strncpy(dtype,bson_iterator_string(&it4),CF_MAXVARSIZE-1);
                  }
               
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
                  else
                     {
                     CfOut(cf_error,"", " !! Unknown key \"%s\" in variables",bson_iterator_key(&it4));
                     }
                  }

               // Now do selection
               
               match_type = match_scope = match_lval = match_rval = true;
               
               if (regex)
                  {
                  if (llval && !FullTextMatch(llval,rlval))
                     {
                     match_lval = false;
                     }

                  if (lrval && !FullTextMatch(lrval,rrval))
                     {
                     match_rval = false;
                     }

                  if (lscope && !FullTextMatch(lscope,rscope))
                     {
                     match_scope = false;
                     }
                  
                  if (ltype && !FullTextMatch(ltype,dtype))
                     {
                     match_type = false;
                     }
                  }
               else
                  {
                  if (llval && strcmp(llval,rlval) != 0)
                     {
                     match_lval = false;
                     }

                  if (lrval && strcmp(lrval,rrval) != 0)
                     {
                     match_rval = false;
                     }
                  
                  if (lscope && strcmp(lscope,rscope) != 0)
                     {
                     match_scope = false;
                     }
                  
                  if (ltype && strcmp(ltype,dtype) != 0)
                     {
                     match_type = false;
                     }
                  }
                  
               if (match_type && match_scope && match_lval && match_rval)
                  {
                  found = true;
                  AppendRlistAlien(&record_list,NewHubVariable(NULL,dtype,rscope,rlval,rrval,rtype));
                  }
               else
                  {
                  switch(rtype)
                     {
                     case CF_SCALAR:
                         free(rrval);
                         break;
                         
                     case CF_LIST:
                         DeleteRlist(rrval);
                         break;
                     }                  
                  }

               
               newlist = NULL;
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);
      
      // Now cache the host reference in all of the records to flatten the 2d list
      
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubVariable *hv = (struct HubVariable *)rp->item;
         hv->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPromiseCompliance(mongo_connection *conn,bson *query,char *lhandle,char lstatus,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  time_t rtime;
  double rsigma,rex;
  char rhandle[CF_MAXVARSIZE],rstatus,*prstat;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_handle,match_status,found;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   keyhash[0] = '\0';
   hostnames[0] = '\0';
   addresses[0] = '\0';
   rhandle[0] = '\0';
   found = false;
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in promise compliance",bson_iterator_key(&it3));
                  }
               }

            match_handle = match_status = true;
            
            if (regex)
               {
               if (lhandle && !FullTextMatch(lhandle,rhandle))
                  {
                  match_handle = false;
                  }
               }
            else
               {
               if (lhandle && (strcmp(lhandle,rhandle) != 0))
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
               AppendRlistAlien(&record_list,NewHubCompliance(NULL,rhandle,rstatus,rex,rsigma,rtime));
               }            
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubPromiseCompliance *hp = (struct HubPromiseCompliance *)rp->item;

         hp->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryLastSeen(mongo_connection *conn,bson *query,char *lhash,char *lhost,char *laddr,time_t lago,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  double rago,ravg,rdev;
  char rhash[CF_MAXVARSIZE],rhost[CF_MAXVARSIZE],raddr[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_host,match_hash,match_addr,found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

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
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
                  rago = bson_iterator_int(&it3);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_dnsname) == 0)
                  {
                  strncpy(rhost,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_ipaddr) == 0)
                  {
                  strncpy(raddr,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else
                  {
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                  }
               }

            match_host = match_addr = match_hash = true;
            
            if (regex)
               {
               if (lhost && !FullTextMatch(lhost,rhost))
                  {
                  match_host = false;
                  }

               // Doesn't make sense to do regex on a key
               if (lhash && (strcmp(lhash,rhash) != 0))
                  {
                  match_hash = false;
                  }

               if (laddr && !FullTextMatch(laddr,raddr))
                  {
                  match_addr = false;
                  }
               }
            else
               {
               if (lhost && (strcmp(lhost,rhost) != 0))
                  {
                  match_host = false;
                  }

               if (lhash && (strcmp(lhash,rhash) != 0))
                  {
                  match_hash = false;
                  }
               
               if (laddr && (strcmp(laddr,raddr) != 0))
                  {
                  match_addr = false;
                  }
               }
            
            if (match_hash && match_host && match_addr)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubLastSeen(NULL,*rhash,rhash+1,rhost,raddr,rago,ravg,rdev));
               }            
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
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
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                  }
               }

            found = true;
            AppendRlistAlien(&record_list,NewHubMeter(NULL,*rcolumn,rkept,rrepaired));
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPerformance(mongo_connection *conn,bson *query,char *lname,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE];
  int match_name,found = false;
  double rsigma,rex,rq;
  time_t rtime;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,cfr_performance,1);
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
   found = false;
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      /* Query specific search/marshalling */

      if (strcmp(bson_iterator_key(&it1),cfr_performance) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3, bson_iterator_value(&it2));
            strncpy(rname,bson_iterator_key(&it2),CF_SMALLBUF-1);

            rex = 0;
            rsigma = 0;
            rtime = 0;
            rq = 0;
            
            while (bson_iterator_next(&it3))
               {
               if (strcmp(bson_iterator_key(&it3),cfr_obs_q) == 0)
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
               else
                  {
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in performance",bson_iterator_key(&it3));
                  }
               }

            
            match_name = true;
            
            if (regex)
               {
               if (lname && !FullTextMatch(lname,rname))
                  {
                  match_name = false;
                  }
               }
            else
               {
               if (lname && (strcmp(lname,rname) != 0))
                  {
                  match_name = false;
                  }
               }
            
            if (match_name)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubPerformance(NULL,rname,rtime,rq,rex,rsigma));
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QuerySetuid(mongo_connection *conn,bson *query,char *lname,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE];
  int match_name,found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);
//cursor = mongo_find(conn,MONGO_DATABASE,bson_empty(&b),&field,0,0,0);

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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
               if (lname && !FullTextMatch(lname,rname))
                  {
                  match_name = false;
                  }
               }
            else
               {
               if (lname && (strcmp(lname,rname) != 0))
                  {
                  match_name = false;
                  }
               }
            
            if (match_name)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubSetUid(NULL,rname));
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryFileChanges(mongo_connection *conn,bson *query,char *lname,int regex,time_t lt,int cmp)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_BUFSIZE];
  int match_name,match_t,found = false;
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,cfr_filechanges,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

hostnames[0] = '\0';
addresses[0] = '\0';

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);
//cursor = mongo_find(conn,MONGO_DATABASE,bson_empty(&b),&field,0,0,0);

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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      /* Query specific search/marshalling */

      if (strcmp(bson_iterator_key(&it1),cfr_filechanges) == 0)
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
                  strncpy(rname,bson_iterator_string(&it3),CF_BUFSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                  {
                  rt = bson_iterator_int(&it3);
                  }
               }
            
            match_name = match_t = true;
            
            if (cmp == CFDB_GREATERTHANEQ)
               {
               if (lt != -1 && lt < rt)
                  {
                  match_t = false;
                  }
               }
            else // CFDB_LESSTHANEQ
               {
               if (lt != -1 && lt > rt)
                  {
                  match_t = false;
                  }
               }
            
            if (regex)
               {
               if (lname && !FullTextMatch(lname,rname))
                  {
                  match_name = false;
                  }
               }
            else
               {
               if (lname && (strcmp(lname,rname) != 0))
                  {
                  match_name = false;
                  }
               }
            
            if (match_name && match_t)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubFileChanges(NULL,rname,rt));
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryFileDiff(mongo_connection *conn,bson *query,char *lname,char *ldiff,int regex,time_t lt,int cmp)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE],rdiff[CF_BUFSIZE];
  int match_name,match_t,match_diff,found = false;
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

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

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
                  strncpy(rname,bson_iterator_string(&it3),CF_BUFSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_diff) == 0)
                  {
                  strncpy(rdiff,bson_iterator_string(&it3),CF_BUFSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                  {
                  rt = bson_iterator_int(&it3);
                  }
               }
            
            match_name = match_t = true;

            if (cmp == CFDB_GREATERTHANEQ)
               {
               if (lt != -1 && lt < rt)
                  {
                  match_t = false;
                  }
               }
            else // CFDB_LESSTHANEQ
               {
               if (lt != -1 && lt > rt)
                  {
                  match_t = false;
                  }
               }
            
            if (regex)
               {
               if (lname && !FullTextMatch(lname,rname))
                  {
                  match_name = false;
                  }

               if (ldiff && !FullTextMatch(ldiff,rdiff))
                  {
                  match_diff = false;
                  }
               }
            else
               {
               if (lname && (strcmp(lname,rname) != 0))
                  {
                  match_name = false;
                  }

               if (ldiff && (strcmp(ldiff,rdiff) != 0))
                  {
                  match_diff = false;
                  }
               }
            
            if (match_name && match_diff && match_t)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubFileDiff(NULL,rname,rdiff,rt));
               }
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryBundleSeen(mongo_connection *conn,bson *query,char *lname,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  double rago,ravg,rdev;
  char rname[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_name,found = false;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
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

cursor = mongo_find(conn,MONGO_DATABASE,query,&field,0,0,0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   keyhash[0] = '\0';
   hostnames[0] = '\0';
   addresses[0] = '\0';
   rname[0] = '\0';
   found = false;
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      /* Query specific search/marshalling */

      if (strcmp(bson_iterator_key(&it1),cfr_bundles) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3, bson_iterator_value(&it2));
            strncpy(rname,bson_iterator_key(&it2),CF_MAXVARSIZE-1);

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
               else
                  {
                  CfOut(cf_error,"", " !! Unknown key \"%s\" in bundle seen",bson_iterator_key(&it3));
                  }
               }

            match_name = true;
            
            if (regex)
               {
               if (lname && !FullTextMatch(lname,rname))
                  {
                  match_name = false;
                  }
               }
            else
               {
               if (lname && (strcmp(lname,rname) != 0))
                  {
                  match_name = false;
                  }
               }
            
            if (match_name)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubBundleSeen(NULL,rname,rago,ravg,rdev));
               }            
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
   
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubClass *hs = (struct HubClass *)rp->item;
         hs->hh = hh;
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

int CFDB_QueryMagView(mongo_connection *conn,char *keyhash,enum observables obs,time_t start_time,double *qa,double *ea,double *da)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
  bson qu,query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  char search_name[CF_MAXVARSIZE];
  double q,e,d;
  int ok = false,start_slot,wrap_around;
  
/* BEGIN query document */

bson_buffer_init(&b);
bson_append_string(&b,cfr_keyhash,keyhash);
bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */

snprintf(search_name,CF_MAXVARSIZE-1,"%s%d",cfr_mag,obs);

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,search_name,1);
bson_from_buffer(&field, &bb);

/* Check from wrap around */

start_slot = GetTimeSlot(start_time);

wrap_around = (int)start_slot + CF_MAGDATA - CF_MAX_SLOTS;

/* BEGIN SEARCH */

cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */

      if (strcmp(bson_iterator_key(&it1),search_name) == 0)
         {
         int slot = 0,st = 0;
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3,bson_iterator_value(&it2));
            sscanf(bson_iterator_key(&it2),"%d",&st);

            // Select the past 4 hours
            
            if (wrap_around > 0)
               {
               if (st >= wrap_around || st < start_slot)
                  {
                  continue;
                  }
               }
            else
               {
               if (st < start_slot || st > start_slot + CF_MAGDATA)
                  {
                  continue;
                  }
               }

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

            qa[slot] = q;
            ea[slot] = e;
            da[slot] = d;
            slot++;
            }
         }
      }
   }

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
  
/* BEGIN query document */

bson_buffer_init(&b);
bson_append_string(&b,cfr_keyhash,keyhash);
bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */

snprintf(search_name,CF_MAXVARSIZE-1,"%s%d",cfr_week,obs);

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,search_name,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

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
            bson_iterator_init(&it3,bson_iterator_value(&it2));
            sscanf(bson_iterator_key(&it2),"%d",&st);

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
  char search_name[CF_MAXVARSIZE];
  double q,e,d;
  int ok = false;
  time_t start_time = CF_MONDAY_MORNING;
  
/* BEGIN query document */

bson_buffer_init(&b);
bson_append_string(&b,cfr_keyhash,keyhash);
bson_from_buffer(&query,&b);
  
/* BEGIN RESULT DOCUMENT */

snprintf(search_name,CF_MAXVARSIZE-1,"%s%d",cfr_yr,obs);

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);
bson_append_int(&bb,search_name,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(conn,MONGO_DATABASE,&query,&field,0,0,0);

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
            bson_iterator_init(&it3,bson_iterator_value(&it2));
            sscanf(bson_iterator_key(&it2),"%d",&st);

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

            // Slot starts at 1 due to coarse graining loop
            
            index;

            qa[index] = q;
            ea[index] = e;
            da[index] = d;

            printf("Storing slot %s %lf,%lf,%lf\n",index,q,e,d);
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


