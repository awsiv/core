
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
  
  
  // clients do not run mongo server -- will fail to connect
if (!IsDefinedClass("am_policy_hub"))
   {
   return;
   }

  
if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return;
   }
    
rval[0] = '\0';

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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  char rname[CF_MAXVARSIZE] = {0},rversion[CF_MAXVARSIZE] = {0},rarch[3] = {0};
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
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in software packages",bson_iterator_key(&it3));
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
                  rp = AppendRlistAlien(&record_list,NewHubSoftware(CF_THIS_HH,rname,rversion,rarch));
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
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryClasses(mongo_connection *conn,bson *query,char *lclass,int regex,time_t horizon)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  time_t rtime, now = time(NULL);
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
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in classes",bson_iterator_key(&it3));
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
            
	    if (match_class && (now - rtime < CF_HUB_HORIZON))
               {
               found = true;
               rp = AppendRlistAlien(&record_list,NewHubClass(CF_THIS_HH,rclass,rex,rsigma,rtime));
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
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
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
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in total compliance",bson_iterator_key(&it3));
                  }
               }

            match_version = match_t = match_kept = match_notkept = match_repaired = true;

            if (lversion && !FullTextMatch(lversion,rversion))
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
               rp = AppendRlistAlien(&record_list,NewHubTotalCompliance(CF_THIS_HH,rt,rversion,rkept,rrepaired,rnotkept));
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
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL,*newlist = NULL;
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
                     CfOut(cf_inform,"", " !! Unknown key \"%s\" in variables",bson_iterator_key(&it4));
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
                  rp = AppendRlistAlien(&record_list,NewHubVariable(CF_THIS_HH,dtype,rscope,rlval,rrval,rtype));
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
         if (hv->hh == CF_THIS_HH)
            {
            hv->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
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
               rp = AppendRlistAlien(&record_list,NewHubCompliance(CF_THIS_HH,rhandle,rstatus,rex,rsigma,rtime));
               }            
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL,*list_start = NULL;
  double rago,ravg,rdev;
  char rhash[CF_MAXVARSIZE],rhost[CF_MAXVARSIZE],raddr[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_host,match_hash,match_addr,match_ago,found = false;
  time_t rt;
  
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
   list_start = NULL;
   
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
               else if (strcmp(bson_iterator_key(&it3),cfr_dnsname) == 0)
                  {
                  strncpy(rhost,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_ipaddr) == 0)
                  {
                  strncpy(raddr,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
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

            if (lago > rago)
               {
               match_ago = false;
               }
            
            if (match_hash && match_host && match_addr && match_ago)
               {
               found = true;
               AppendRlistAlien(&record_list,NewHubLastSeen(CF_THIS_HH,*rhash,rhash+1,rhost,raddr,rago,ravg,rdev,rt));
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
         struct HubLastSeen *hs = (struct HubLastSeen *)rp->item;
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in meter",bson_iterator_key(&it3));
                  }
               }

            found = true;
            rp = AppendRlistAlien(&record_list,NewHubMeter(CF_THIS_HH,*rcolumn,rkept,rrepaired));
            }
         }   
      }

   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

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

struct HubQuery *CFDB_QueryPerformance(mongo_connection *conn,bson *query,char *lname,int regex)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp =  NULL,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE];
  int match_name,found = false;
  double rsigma,rex,rq;
  time_t rtime;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..
  
/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
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
	    snprintf(rname,sizeof(rname),"%s",bson_iterator_key(&it2));

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
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in performance",bson_iterator_key(&it3));
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
               rp = AppendRlistAlien(&record_list,NewHubPerformance(CF_THIS_HH,rname,rtime,rq,rex,rsigma));
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
         struct HubPerformance *hp = (struct HubPerformance *)rp->item;
         if (hp->hh == CF_THIS_HH)
            {
            hp->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
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
               rp = AppendRlistAlien(&record_list,NewHubSetUid(CF_THIS_HH,rname));
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
         struct HubSetUid *hs = (struct HubSetUid *)rp->item;
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
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
               rp = AppendRlistAlien(&record_list,NewHubFileChanges(CF_THIS_HH,rname,rt));
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
         struct HubFileChanges *hs = (struct HubFileChanges *)rp->item;
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rname[CF_MAXVARSIZE],rdiff[CF_BUFSIZE];
  int match_name,match_t,match_diff,found = false;
  time_t rt = 0;
  
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
               rp = AppendRlistAlien(&record_list,NewHubFileDiff(CF_THIS_HH,rname,rdiff,rt));
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
         struct HubFileDiff *hs = (struct HubFileDiff *)rp->item;
         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);
return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryPromiseLog(mongo_connection *conn,bson *query,enum promiselog_rep type,char *lhandle,int regex,int sortDescending)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  double rago,ravg,rdev;
  char rhandle[CF_MAXVARSIZE],rcause[CF_BUFSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_name,found = false;
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_keyhash,1);
bson_append_int(&bb,cfr_ip_array,1);
bson_append_int(&bb,cfr_host_array,1);

switch (type)
   {
   case plog_repaired:
       bson_append_int(&bb,cfr_repairlog,1);
       break;
   case plog_notkept:
   default:
       bson_append_int(&bb,cfr_notkeptlog,1);
       break;
   }

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

      if (strcmp(bson_iterator_key(&it1),cfr_repairlog) == 0 || strcmp(bson_iterator_key(&it1),cfr_notkeptlog) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
            bson_iterator_init(&it3, bson_iterator_value(&it2));

            rhandle[0] = '\0';
            rcause[0] = '\0';
            rt = 0;
            
            while (bson_iterator_next(&it3))
               {
               if (strcmp(bson_iterator_key(&it3),cfr_promisehandle) == 0)
                  {
                  strncpy(rhandle,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_cause) == 0)
                  {
                  strncpy(rcause,bson_iterator_string(&it3),CF_MAXVARSIZE-1);
                  }
               else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                  {
                  rt = bson_iterator_int(&it3);
                  }
               else
                  {
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in promise log",bson_iterator_key(&it3));
                  }
               }

            match_name = true;
            
            if (regex)
               {
	       if (!EMPTY(lhandle) && !FullTextMatch(lhandle,rhandle))
                  {
                  match_name = false;
                  }
               }
            else
               {
	       if (!EMPTY(lhandle) && (strcmp(lhandle,rhandle) != 0))
                  {
                  match_name = false;
                  }
               }
            
            if (match_name)
               {
               found = true;
               rp = AppendRlistAlien(&record_list,NewHubPromiseLog(CF_THIS_HH,rhandle,rcause,rt));
               }
            }
         }   
      }


   if(sortDescending)
      {
      record_list = SortRlist(record_list,DescendingTimePromiseLog);
      }


   if (found)
      {
      hh = NewHubHost(keyhash,addresses,hostnames);
      AppendRlistAlien(&host_list,hh);

      // Now cache the host reference in all of the records to flatten the 2d list
      for (rp = record_list; rp != NULL; rp=rp->next)
         {
         struct HubPromiseLog *hs = (struct HubPromiseLog *)rp->item;

         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);

return NewHubQuery(host_list,record_list);
}

/*****************************************************************************/

struct HubQuery *CFDB_QueryValueReport(mongo_connection *conn,bson *query,char *lday,char *lmonth,char *lyear)

{ bson_buffer bb,*sub1,*sub2,*sub3;
  bson b,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  double rkept,rnotkept,rrepaired;
  char rday[CF_MAXVARSIZE],rmonth[CF_MAXVARSIZE],ryear[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_day,match_month,match_year,found = false;
  time_t rt;
  
/* BEGIN query document */

  // Can't understand the bson API for nested objects this, so work around..

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

      if (strcmp(bson_iterator_key(&it1),cfr_valuereport) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
            {
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
               rp = AppendRlistAlien(&record_list,NewHubValue(CF_THIS_HH,rday,rkept,rrepaired,rnotkept));
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
         struct HubValue *hs = (struct HubValue *)rp->item;

         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
  struct Rlist *rp = NULL,*record_list = NULL, *host_list = NULL;
  double rago,ravg,rdev;
  char rname[CF_MAXVARSIZE];
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE];
  int match_name,found = false;
  time_t rt;
  
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
   rt = 0;
   
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
               else if (strcmp(bson_iterator_key(&it3),cfr_time) == 0)
                  {
                  rt = bson_iterator_int(&it3);
                  }
               else
                  {
                  CfOut(cf_inform,"", " !! Unknown key \"%s\" in bundle seen",bson_iterator_key(&it3));
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
               rp = AppendRlistAlien(&record_list,NewHubBundleSeen(CF_THIS_HH,rname,rago,ravg,rdev,rt));
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
         struct HubBundleSeen *hs = (struct HubBundleSeen *)rp->item;

         if (hs->hh == CF_THIS_HH)
            {
            hs->hh = hh;
            }
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
   qa[slot] = 0;
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
  time_t start_time = CF_MONDAY_MORNING;
  
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

            printf("Storing slot %d %lf,%lf,%lf\n",index,q,e,d);
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

struct HubPromise *CFDB_QueryPromise(mongo_connection *conn, char *handle)
/*
 * Returns all attribs of one promise by its handle.
 */
{ bson_buffer b;
  bson query;
  mongo_cursor *cursor;
  bson_iterator it1,it2;
  char bn[CF_MAXVARSIZE] = {0}, bt[CF_MAXVARSIZE] = {0},ba[CF_MAXVARSIZE] = {0},
    pt[CF_MAXVARSIZE] = {0}, pr[CF_MAXVARSIZE] = {0}, pe[CF_MAXVARSIZE] = {0},
    cl[CF_MAXVARSIZE] = {0}, ha[CF_MAXVARSIZE] = {0}, co[CF_MAXVARSIZE] = {0},
    fn[CF_MAXVARSIZE] = {0}, **cons = {0};
  int lno = -1;
  int i,constCount;
  
  /* BEGIN query document */

bson_buffer_init(&b);
bson_append_string(&b,cfp_handle,handle);
bson_from_buffer(&query,&b);


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
else
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
	     IdempAppendItem(&matched,(char *)bson_iterator_string(&it2),NULL);
	     }

	   }
	 else
	   {
	   IdempAppendItem(&matched,(char *)bson_iterator_string(&it1),NULL);
	   }

         break;
         }
      }
   }  

mongo_cursor_destroy(cursor);

return matched;
}

/*****************************************************************************/

struct Rlist *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName, int regex)
/*
 * Returns a set of handles of promises matching given promiser regex
 * XOR promise type XOR (bundle type, bundle name)
 */
{ bson_buffer b;
  bson_iterator it1;
  bson query,field;
  mongo_cursor *cursor;
  struct Rlist *handles = {0};

  // query
bson_buffer_init(&b);

 if(regex)
   {
     if (!EMPTY(promiser))
       {
	 bson_append_regex(&b, cfp_promiser, promiser,"");
       }
     else if(!EMPTY(promiserType))
       {
	 bson_append_regex(&b, cfp_promisetype, promiserType,"");
       }
     else
       {
	 bson_append_regex(&b,cfp_bundletype,bType,"");
	 bson_append_regex(&b,cfp_bundlename,bName,"");
       }
   }
 else
   {
     if (!EMPTY(promiser))
       {
	 bson_append_string(&b, cfp_promiser, promiser);
       }
     else if(!EMPTY(promiserType))
       {
	 bson_append_string(&b, cfp_promisetype, promiserType);
       }
     else
       {
	 bson_append_string(&b,cfp_bundletype,bType);
	 bson_append_string(&b,cfp_bundlename,bName);
       }
   }


bson_from_buffer(&query,&b);

// returned attribute
bson_buffer_init(&b);
bson_append_int(&b,cfp_handle,1);
bson_from_buffer(&field,&b);

cursor = mongo_find(conn,MONGO_PROMISES_UNEXP,&query,&field,0,0,0);

bson_destroy(&query);
bson_destroy(&field);

while(mongo_cursor_next(cursor))  // iterate over docs
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while(bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1), cfp_handle) == 0)
         {
         AppendRScalar(&handles,(void *)bson_iterator_string(&it1),CF_SCALAR);
         }
      }
   }

mongo_cursor_destroy(cursor);

return handles;
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
            IdempAppendItem(&args,(char *)bson_iterator_string(&it2),NULL);
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
         IdempAppendItem(&bNameReferees,(char *)bson_iterator_string(&it1),"agent");
         }
      }
   }

mongo_cursor_destroy(cursor);

return bNameReferees;
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
      Join(ipaddr,(char *)bson_iterator_string(&it2),CF_BUFSIZE);
      }
   }

if (strcmp(bson_iterator_key(it1),cfr_host_array) == 0)
   {         
   bson_iterator_init(&it2, bson_iterator_value(it1));
   
   while (bson_iterator_next(&it2))
      {
      Join(hostnames,(char *)bson_iterator_string(&it2),CF_BUFSIZE);
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

void BsonToString(char *retBuf, int retBufSz, bson *b, int depth)
/* NOTE: Only depth 1 is implemented */
{
    bson_iterator i;
    const char * key;
    int temp;
    char oidhex[25];
    char buf[CF_MAXVARSIZE];

    memset(retBuf,0,retBufSz);
    bson_iterator_init( &i , b->data );

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
        case bson_array:
	  // TODO: Not implemented yet..
	  memset(buf,0,sizeof(buf));
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

void MongoCheckForError(mongo_connection *conn, char *operation, char *extra)
{
  char dbErr[CF_MAXVARSIZE];
  bson b;

  if(!extra)
    {
      extra = "";
    }

if(mongo_cmd_get_last_error(conn, MONGO_BASE, &b))
  {
    BsonToString(dbErr,sizeof(dbErr),&b,1);
    CfOut(cf_error, "", "!! Database error on %s (%s): %s", operation,extra,dbErr);
  }
 
 bson_destroy(&b);

}


#endif  /* HAVE LIB_MONGOC */


