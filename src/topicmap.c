/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: topicmap.c                                                          */
/*                                                                           */
/*****************************************************************************/

void Nova_WebTopicMap_Initialize()

{ char retval[CF_MAXVARSIZE];

#ifdef HAVE_LIBMONGOC 
CFDB_GetValue("document_root",retval,CF_MAXVARSIZE);
strncpy(DOCROOT,retval,CF_MAXVARSIZE);

CfDebug("Loaded values: docroot=%s\n",DOCROOT);
#endif
}

/*****************************************************************************/
/* The main panels                                                           */
/*****************************************************************************/

void Nova_DumpTopics()

{
#ifdef HAVE_LIBMONGOC
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  char topic_name[CF_BUFSIZE];
  char topic_context[CF_BUFSIZE];
  int topic_id;
  char work[CF_BUFSIZE];
  struct Item *ip,*nn;

if (!CFDB_Open(&conn))
   {
   return;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_empty(&query);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_append_int(&bb,cfk_associations,1);
bson_append_int(&bb,cfk_associd,1);
bson_append_int(&bb,cfk_assoccontext,1);
bson_append_int(&bb,cfk_assocname,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
   topic_id = 0;
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicname) == 0)
         {
         strncpy(topic_name,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_topiccontext) == 0)
         {
         strncpy(topic_context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topic_id = (int)bson_iterator_int(&it1);
         }
      }
   
   printf("Topic: %s::%s, id: %d \n",topic_context,topic_name,topic_id);

   nn = Nova_ScanLeadsAssociations(topic_id,NULL);

   printf("   {\n");
   for (ip = nn; ip != NULL; ip=ip->next)
      {
      printf("   (%s) \"%s\"\n",ip->name,ip->classes);
      }
   printf("   }\n");
   }

DeleteItemList(nn);
mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
#endif
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
int Nova_GetTopicIdForTopic(char *typed_topic)
    
{
  char topic[CF_BUFSIZE],type[CF_BUFSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  int topic_id = 0;
  
Nova_DeClassifyTopic(ToLowerStr(typed_topic),topic,type); // Linker trouble - copy this from core

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_topicname,topic);

if (strlen(type) > 0 && strcmp(type,"any") != 0)
   {
   bson_append_string(&bb,cfk_topiccontext,type);
   }

bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   while (bson_iterator_next(&it1))
      {      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topic_id = (int)bson_iterator_int(&it1);
         }
      }
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);

return topic_id;
}
#endif

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
int Nova_GetTopicByTopicId(int search_id,char *topic_name,char *topic_id,char *topic_context)

{
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  int topicid = 0;

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicid,search_id);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
   topicid = 0;
   
   while (bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),cfk_topicname) == 0)
         {
         strncpy(topic_name,bson_iterator_string(&it1),CF_BUFSIZE-1);
         strncpy(topic_id,Name2Id((char *)bson_iterator_string(&it1)),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_topiccontext) == 0)
         {
         strncpy(topic_context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topicid = (int)bson_iterator_int(&it1);
         }
      }
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return topicid;
}
#endif

/*********************************************************************/

#ifdef HAVE_LIBMONGOC
int Nova_SearchTopicMap(char *search_topic,char *buffer,int bufsize)

{
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  char topic_name[CF_BUFSIZE],jsonEscapedStr[CF_BUFSIZE];
  char topic_context[CF_BUFSIZE];
  int topic_id;
  char work[CF_BUFSIZE];
  struct Item *ip,*list = NULL;

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

if (!EMPTY(search_topic))
   {
   Nova_DeClassifyTopic(search_topic,topic_name,topic_context);

   bson_buffer_init(&bb);
   bson_append_regex(&bb,cfk_topicname,topic_name,"");
   bson_append_regex(&bb,cfk_topiccontext,topic_context,"");
   bson_from_buffer(&query,&bb);
   }
else
   {
   bson_buffer_init(&bb);
   bson_empty(&query);
   }

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_append_int(&bb,cfk_associations,1);
bson_append_int(&bb,cfk_associd,1);
bson_append_int(&bb,cfk_assoccontext,1);
bson_append_int(&bb,cfk_assocname,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

strcpy(buffer,"[ ");

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
   topic_id = 0;
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicname) == 0)
         {
         strncpy(topic_name,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_topiccontext) == 0)
         {
         strncpy(topic_context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topic_id = (int)bson_iterator_int(&it1);
         }
      }

   PrependFullItem(&list,topic_name,topic_context,topic_id,0);
   }

list = SortItemListNames(list);

for (ip = list; ip != NULL; ip=ip->next)
   {
   EscapeJson(ip->name,jsonEscapedStr,CF_BUFSIZE-1);
   snprintf(work,CF_BUFSIZE,"{ \"context\": \"%s\", \"topic\": \"%s\", \"id\": %d },",ip->classes,jsonEscapedStr,ip->counter);
   Join(buffer,work,CF_BUFSIZE);
   }

buffer[strlen(buffer)-1] = ']';

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return true;
}
#endif

/*****************************************************************************/

void Nova_ScanTheRest(int pid,char *buffer, int bufsize)

/* Find other topics in the same context */
    
{
#ifdef HAVE_LIBMONGOC
  char this_name[CF_BUFSIZE],this_id[CF_BUFSIZE],this_context[CF_BUFSIZE];
  struct Item *worklist, *ip;
  char work[CF_BUFSIZE];
  char name[CF_BUFSIZE] = {0},*a_context;
  int id=0;

id= Nova_GetTopicByTopicId(pid,this_name,this_id,this_context);

if (!id)
  {
  snprintf(buffer,bufsize,"No such topic was found");
  return;
  }

// Find other topics that have this topic as their category (sub topics)

worklist = Nova_GetTopicsInContext(this_id);

snprintf(buffer,CF_BUFSIZE,
         "{ \"topic\" : { \"context\" : \"%s\", \"name\" : \"%s\", \"id\" : %d, \"sub_topics\" : [",
         this_context,this_name,id);

for (ip=worklist; ip != NULL; ip=ip->next)
  {
  EscapeJson(ip->name,name,CF_BUFSIZE);

  snprintf(work,CF_BUFSIZE, "{ \"context\" : \"%s\", \"topic\" : \"%s\", \"id\" : %d},",
           ip->classes, name, ip->counter);

  Join(buffer,work,bufsize);
  }

ReplaceTrailingChar(buffer, ',', '\0');

Join(buffer,"]}, \"other_topics\" : [",bufsize);
name[0]='\0';

// Find other topics in the same context

worklist = Nova_GetTopicsInContext(this_context);

for(ip=worklist;ip!=NULL;ip=ip->next)
  {
  EscapeJson(ip->name,name,CF_BUFSIZE);

  snprintf(work,CF_BUFSIZE, "{ \"context\" : \"%s\", \"topic\" : \"%s\", \"id\" : %d },", 
           ip->classes, name, ip->counter);

  Join(buffer,work,bufsize);
  }

ReplaceTrailingChar(buffer, ',', '\0');
EndJoin(buffer,"]}",bufsize);
#endif
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
struct Item *Nova_ScanLeadsAssociations(int search_id,char *assoc_mask)

/* Look for neighbours and retain/sort link names - return JSON array

  [
   {
   assoc: "is related to",
   topics: [  { topic: "name", id: 7},   { topic: "name2", id: 8}   ]
   }, ...
  ]

*/
    
{
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  char assoc_name[CF_BUFSIZE];
  char afwd[CF_BUFSIZE];
  char assoc_context[CF_BUFSIZE];
  struct Item *list = NULL;
  int assoc_id;

if (!CFDB_Open(&conn))
   {
   return NULL;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicid,search_id);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_associations,1);
bson_append_int(&bb,cfk_associd,1);
bson_append_int(&bb,cfk_assoccontext,1);
bson_append_int(&bb,cfk_assocname,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while (bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),cfk_associations) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));

             assoc_id = 0;
             assoc_name[0] = '\0';
             assoc_context[0] = '\0';
             afwd[0] = '\0';

             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfk_associd) == 0)
                   {
                   assoc_id = bson_iterator_int(&it3);
                   }   
                
                if (strcmp(bson_iterator_key(&it3),cfk_assocname) == 0)
                   {
                   strncpy(assoc_name,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   

                if (strcmp(bson_iterator_key(&it3),cfk_fwd) == 0)
                   {
                   strncpy(afwd,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   

                if (strcmp(bson_iterator_key(&it3),cfk_assoccontext) == 0)
                   {
                   strncpy(assoc_context,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   
                }

             if (assoc_mask == NULL || FullTextMatch(assoc_mask,afwd))
                {
                char qualified[CF_BUFSIZE];
                snprintf(qualified,CF_BUFSIZE,"%s::%s",assoc_context,assoc_name);
                PrependFullItem(&list,afwd,qualified,assoc_id,0);
                }
             }
         }
      }
   }

list = SortItemListNames(list);

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return list;
}
#endif

/*****************************************************************************/

void Nova_ScanOccurrences(int this_id,char *buffer, int bufsize)

{
#ifdef HAVE_LIBMONGOC
  enum representations locator_type;
  struct Rlist *rp,*frags = NULL,*atoms = NULL,*rrp;
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE];
  char locator[CF_BUFSIZE],context[CF_BUFSIZE],represents[CF_BUFSIZE],searchstring[CF_BUFSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;

// Do we want to prune using the topic context?

/* Match occurrences that could overlap with the current context.
   Take a simple approach - if we are in topic_context, then
   topic_context&anything is within topic_context, but topic_context|anything
   must be too big. BUT anything must!= "any"
*/

if (!CFDB_Open(&conn))
   {
   return;
   }

/* BEsnprintf(searchstring,CF_BUFSIZE,".*\.*%s\.*.*",topic);GIN query document */

Nova_GetTopicByTopicId(this_id,topic_name,topic_id,topic_context);

if (strcmp("any",topic_context) == 0)
   {
   snprintf(searchstring,CF_BUFSIZE,".*\\.*%s\\.*.*",topic_id);
   }
else
   {
   snprintf(searchstring,CF_BUFSIZE,"%s",topic_id);
   }

bson_buffer_init(&bb);
bson_append_regex(&bb,cfk_occurcontext,searchstring,"");
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurcontext,1);
bson_append_int(&bb,cfk_occurlocator,1);
bson_append_int(&bb,cfk_occurtype,1);
bson_append_int(&bb,cfk_occurrep,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

strcpy(buffer,"[ ");

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   locator[0] = '\0';
   context[0] = '\0';
   represents[0] = '\0';
   locator_type = cfk_literal;
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         strncpy(locator,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_occurcontext) == 0)
         {
         strncpy(context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }

      if (strcmp(bson_iterator_key(&it1),cfk_occurrep) == 0)
         {
         strncpy(represents,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_occurtype) == 0)
         {
         locator_type = (int)bson_iterator_int(&it1);
         }
      }

   // Now, for each plausible occurrence, see if it overlaps with the current topic in
   // any of its component parts. We do some `lifting' here to infer potential relevance

   frags = SplitStringAsRList(context,'|');
   frags = AlphaSortRListNames(frags);
   
   for (rp = frags; rp != NULL; rp=rp->next)
      {
      if (strcmp(rp->item,topic_id) == 0)
         {
         Nova_AddOccurrenceBuffer("any/all",locator,locator_type,represents,buffer,bufsize);
         continue;
         }
      else
         {
         atoms = SplitRegexAsRList(context,"[.&()]",10,false);

         for (rrp = atoms; rrp != NULL; rrp=rrp->next)
            {
            char *sp,*stripped = rrp->item;

            // Try to strip out the common topic string
            
            stripped = Nova_StripString(rrp->item,topic_id);
            
            if (strcmp(rrp->item,topic_id) == 0)
               {
               if (strlen(stripped) == 0)
                  {
                  Nova_AddOccurrenceBuffer("any/all",locator,locator_type,represents,buffer,bufsize);
                  }
               else
                  {
                  Nova_AddOccurrenceBuffer(stripped,locator,locator_type,represents,buffer,bufsize);
                  }
               
               free(stripped);
               break;
               }
            else
               {
               free(stripped);
               }
            }

         DeleteRlist(atoms);
         }
      }

   DeleteRlist(frags);
   }

buffer[strlen(buffer)-1] = ']';
#endif
}

/*************************************************************************/

int Nova_GetReportDescription(int this_id,char *buffer, int bufsize)

{
#ifdef HAVE_LIBMONGOC
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE];
  char searchstring[CF_BUFSIZE];
  bson query,field;
  mongo_cursor *cursor;
  bson_buffer bb;
  bson_iterator it1,it2,it3;
  mongo_connection conn;

if (!CFDB_Open(&conn))
   {
   return false;
   }

Nova_GetTopicByTopicId(this_id,topic_name,topic_id,topic_context);

if (strcmp("system_reports",topic_context) == 0)
   {
   snprintf(searchstring,CF_BUFSIZE,"%s.%s",topic_context,topic_id);
   }
else
   {
   CfOut(cf_verbose,"", "!! No description found for topic");
   return false;
   }

bson_buffer_init(&bb);
bson_append_regex(&bb,cfk_occurcontext,searchstring,"");
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurlocator,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while (bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         strncpy(buffer,bson_iterator_string(&it1),bufsize-1);
         return true;
         }
      }
   }
return false;
#endif
return false;
}

/*************************************************************************/

#ifdef HAVE_LIBMONGOC
struct Item *Nova_GetBusinessGoals(char *handle)

{ char querytopic[CF_BUFSIZE];
  struct Item *worklist = NULL, *ip;
  int pid;

snprintf(querytopic,CF_BUFSIZE,"bundles::%s",handle);
pid = Nova_GetTopicIdForTopic(querytopic);

worklist = Nova_NearestNeighbours(pid,NOVA_GOAL);

for (ip = worklist; ip !=  NULL; ip=ip->next)
   {
   Nova_FillInGoalComment(ip);
   }

worklist = SortItemListNames(worklist);
return worklist;
}
#endif
    
/*************************************************************************/

#ifdef HAVE_LIBMONGOC
int Nova_GetUniqueBusinessGoals(char *buffer, int bufsize)

{
 struct Rlist *rp,*rp2;
 bson_buffer bb;
 bson query,field;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 mongo_connection conn;

 char topic_name[CF_MAXVARSIZE] = {0};
 int topic_id;
 char work[CF_BUFSIZE] = {0};
 char goals[CF_MAXVARSIZE] = {0};
 char searchstring[CF_BUFSIZE] = {0};
 struct Rlist *goal_categories = NULL, *goal_patterns = NULL;
 char db_goal_patterns[CF_BUFSIZE] = {0}; 
 char db_goal_categories[CF_BUFSIZE] = {0}; 

if(CFDB_GetValue("goal_patterns",db_goal_patterns,sizeof(db_goal_patterns)))
  {
  goal_patterns = SplitStringAsRList(db_goal_patterns,',');
  }

if(CFDB_GetValue("goal_categories",db_goal_categories,sizeof(db_goal_categories)))
  {
  goal_categories = SplitStringAsRList(db_goal_categories,',');
  }
 
for (rp = goal_categories; rp != NULL; rp=rp->next)
   {
   for (rp2 = goal_patterns; rp2 != NULL; rp2=rp2->next)
      {
      snprintf(work,CF_MAXVARSIZE-1,"%s\\.%s|",(char*)rp->item, (const char *)rp2->item);
      strcat(searchstring,work);
      }
   }

if(strlen(searchstring) > 1)
   {
   searchstring[strlen(searchstring)-1] = '\0';
   }
else
   {
   snprintf(searchstring,CF_MAXVARSIZE-1,"goals\\.goal_.*");
   }

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_regex(&bb,cfk_occurcontext,searchstring,"");
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */
bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurcontext,1);
bson_append_int(&bb,cfk_occurlocator,1);
bson_append_int(&bb,cfk_occurtype,1);
bson_append_int(&bb,cfk_occurrep,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

strcpy(buffer,"[");

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         snprintf(work,CF_BUFSIZE,"{\"desc\": \"%s\",",bson_iterator_string(&it1));
         Join(buffer,work,CF_BUFSIZE);
         }
      if (strcmp(bson_iterator_key(&it1),cfk_occurcontext) == 0)
         {
         snprintf(goals,CF_MAXVARSIZE,"%s",bson_iterator_string(&it1));
         strncpy(topic_name,goals+6,strlen(goals));
         topic_id = Nova_GetTopicIdForTopic(topic_name);
         snprintf(goals,CF_MAXVARSIZE,"\"name\":\"%s\",\"pid\":%d},",topic_name,topic_id);
         Join(buffer,goals,CF_BUFSIZE);
         }       
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');

EndJoin(buffer,"]",CF_BUFSIZE);
mongo_cursor_destroy(cursor);
CFDB_Close(&conn);

return true;
}
#endif
    
/*************************************************************************/
/* Level                                                                 */
/*************************************************************************/

void Nova_AddOccurrenceBuffer(char *context,char *locator,enum representations locator_type,char *represents,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 
snprintf(work,CF_BUFSIZE-1,"{ \"context\": \"%s\", \"ref\": \"%s\", \"represents\": \"%s\", \"type\": %d},",context,locator,represents,locator_type);
Join(buffer,work,bufsize);
}

/*************************************************************************/

char *Nova_URL(char *s,char *rep)

{ static char buf[CF_MAXVARSIZE];
 
snprintf(buf,CF_MAXVARSIZE-1,"<a href=\"%s\">%s</a>: ",s,rep);
return buf;
}

/*************************************************************************/

void Nova_FillInGoalComment(struct Item *ip)

{
#ifdef HAVE_LIBMONGOC
  struct Rlist *rp;
  char searchstring[CF_MAXVARSIZE],work[CF_MAXVARSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  struct Rlist *goal_categories = NULL;
  char db_goal_categories[CF_BUFSIZE] = {0}; 
  char canonified_goal[CF_BUFSIZE] = {0}; 

// Get comment goals.* or targets.%s etc

searchstring[0] = '\0';
ReplaceChar(ip->name,canonified_goal,CF_BUFSIZE,' ','_');
if(CFDB_GetValue("goal_categories",db_goal_categories,sizeof(db_goal_categories)))
  {
  goal_categories = SplitStringAsRList(db_goal_categories,',');
  }

for (rp = goal_categories; rp != NULL; rp=rp->next)
   {
   snprintf(work,CF_MAXVARSIZE-1,"%s\\.%s|",(char*)rp->item,canonified_goal);
   strcat(searchstring,work);
   }

if (strlen(searchstring) > 1)
   {
   searchstring[strlen(searchstring)-1] = '\0';
   }
else
   {
   snprintf(searchstring,CF_MAXVARSIZE-1,"%s",canonified_goal);
   }

if (!CFDB_Open(&conn))
   {
   return;
   }

bson_buffer_init(&bb);
bson_append_regex(&bb,cfk_occurcontext,searchstring,"");
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurcontext,1);
bson_append_int(&bb,cfk_occurlocator,1);
bson_append_int(&bb,cfk_occurtype,1);
bson_append_int(&bb,cfk_occurrep,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);

   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         ip->classes = xstrdup(bson_iterator_string(&it1));
         return;
         }   
      }
   }

ip->classes = xstrdup("No description found");
#endif
}

/*************************************************************************/

#ifdef HAVE_LIBMONGOC
char *Nova_GetBundleComment(char *bundle)

{
  static char buf[CF_BUFSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;

// Get comment goals.* or targets.%s etc

if (!CFDB_Open(&conn))
   {
   return NULL;
   }

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_occurrep,"description");
bson_append_string(&bb,cfk_occurcontext,bundle);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurlocator,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         strncpy(buf,bson_iterator_string(&it1),CF_BUFSIZE-1);
         return buf;
         }
      }
   }
return "";
}
#endif

/*********************************************************************/

char *Nova_StripString(char *source,char *substring)

{ char *replace = xmalloc(strlen(source)+1);
  struct Rlist *rp,*new = SplitStringAsRList(source,'.');

replace[0] = '\0';
  
for (rp = new; rp != NULL; rp=rp->next)
   {
   if (strcmp(rp->item,substring) == 0)
      {
      continue;
      }
   
   strcat(replace,rp->item);

   if (rp->next)
      {
      strcat(replace,".");
      }
   }

DeleteRlist(new);

return replace;
}

/*****************************************************************************/
/* Plot cosmos                                                               */
/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
void Nova_PlotTopicCosmos(int topic,char *view,char *buffer,int bufsize)

/* This assumes that we have the whole graph in a matrix */

{
  struct CfGraphNode tribe_nodes[CF_TRIBE_SIZE];
  int tribe_id[CF_TRIBE_SIZE],tribe_size;
  double tribe_evc[CF_TRIBE_SIZE] = {0};
  double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE];

/* Count the  number of nodes in the solar system, to max number based on Dunbar's limit */  

snprintf(buffer,bufsize,"[");

if ((tribe_size = Nova_GetTribe(tribe_id,tribe_nodes,tribe_adj,topic,view)))
   {
   Nova_EigenvectorCentrality(tribe_adj,tribe_evc,CF_TRIBE_SIZE);
   Nova_DrawTribe(tribe_id,tribe_nodes,tribe_adj,tribe_size,tribe_evc,topic,buffer,bufsize);
   }

Join(buffer,"]",bufsize);
}
#endif

/*************************************************************************/
/* Local patch computation                                               */
/*************************************************************************/

#ifdef HAVE_LIBMONGOC
int Nova_GetTribe(int *tribe_id,struct CfGraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int pid,char *view_pattern)

/* This function generates a breadth-first connected sub-graph of the full graph
   and identifies the orbits and distances, up to a maximum of Dunbar's tribe-size */

{ char topic_name[CF_BUFSIZE],topic_context[CF_BUFSIZE];
  char *a_name,*a_context,view[CF_MAXVARSIZE];
  int from_pid,to_pid,a_pid;
  char query[CF_BUFSIZE];
  struct CfGraphNode neighbours1[CF_TRIBE_SIZE],neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE];
  int tribe_counter = 0,secondary_boundary,tertiary_boundary,i,j;
  struct Item *ip,*nn = NULL;

for (i = 0; i < CF_TRIBE_SIZE; i++)
   {
   Nova_InitVertex(tribe_nodes,i);
   tribe_id[i] = -1;
   
   for (j = 0; j < CF_TRIBE_SIZE; j++)
      {
      tribe_adj[i][j] = 0;
      }      
   }

if (view_pattern && strcmp(view_pattern,"influence") == 0)
   {
   snprintf(view,CF_MAXVARSIZE,"%s|%s|%s|%s|%s|%s|%s",NOVA_GIVES,NOVA_USES,NOVA_IMPACTS,NOVA_ISIMPACTED,NOVA_BUNDLE_DATA,NOVA_BUNDLE_DATA_INV_B,NOVA_BUNDLE_DATA_INV_P);
   }
else
   {
   snprintf(view,CF_MAXVARSIZE,".*");
   }

tribe_id[0] = pid;
topic_name[0] = '\0';
topic_context[0] = '\0';
Nova_NewVertex(tribe_nodes,0,0,pid,topic_name,topic_context);

/* Probe sub-graph */
    
tribe_counter = 1;

// Nearest neighbours

nn = Nova_NearestNeighbours(pid,view);

for (ip = nn; ip != NULL; ip = ip->next)
   {
   a_name = ip->name;
   a_context = ip->classes;
   a_pid = ip->counter;

   if (Nova_AlreadyInTribe(a_pid,tribe_id))
      {
      continue;
      }

   if (Nova_NewVertex(tribe_nodes,tribe_counter,1,a_pid,a_name,a_context))
      {
      neighbours1[tribe_counter].real_id = a_pid;
      tribe_id[tribe_counter] = a_pid;
      tribe_adj[tribe_counter][0] = 1;
      tribe_adj[0][tribe_counter] = 1;
      tribe_counter++;
      }

   CfDebug("NEAREST NEIGHOUR (%d): %s::%s at %d\n",tribe_counter,a_context,a_name,a_pid);

   if (tribe_counter >= CF_TRIBE_SIZE-1)
      {
      break;
      }   
   }

DeleteItemList(nn);

secondary_boundary = tribe_counter;

if (tribe_counter < CF_TRIBE_SIZE-1 && secondary_boundary > 0)
   {
   for (i = 0; i < secondary_boundary; i++)
      {
      nn = Nova_NearestNeighbours(neighbours1[i].real_id,view);

      for (ip = nn; ip != NULL; ip=ip->next)
         {
         if (tribe_counter >= CF_TRIBE_SIZE-1)
            {
            break;
            }
         
         a_name = ip->name;
         a_context = ip->classes;
         a_pid = ip->counter;

         if (Nova_AlreadyInTribe(a_pid,tribe_id))
            {
            continue;
            }
         
         CfDebug("  2nd NEIGHOUR (%d): %s::%s at %d\n",tribe_counter,a_context,a_name,a_pid);

         if (Nova_NewVertex(tribe_nodes,tribe_counter,2,a_pid,a_name,a_context))
            {            
            neighbours2[i][tribe_counter].real_id = a_pid;
            tribe_id[tribe_counter] = a_pid;
            tribe_adj[tribe_counter][i] = 1;
            tribe_adj[i][tribe_counter] = 1;
            tribe_counter++;
            }

         if (tribe_counter >= CF_TRIBE_SIZE-1)
            {
            break;
            }                
         }
      
      if (tribe_counter >= CF_TRIBE_SIZE-1)
         {
         break;
         }                     
      
      DeleteItemList(nn);
      }
   }

tertiary_boundary = tribe_counter;

if (tribe_counter < CF_TRIBE_SIZE-1 && tertiary_boundary > 0)
   {
   for (i = 0; i < secondary_boundary; i++)
      {
      for (j = secondary_boundary; j < tertiary_boundary; j++)
         {
         if (tribe_counter >= CF_TRIBE_SIZE-1)
            {
            break;
            }               

         nn = Nova_NearestNeighbours(neighbours2[i][j].real_id,view);
         
         for (ip = nn; ip != NULL; ip=ip->next)
            {         
            a_name = ip->name;
            a_context = ip->classes;
            a_pid = ip->counter;

            if (a_pid == neighbours1[i].real_id)
               {
               continue;
               }

            if (Nova_AlreadyInTribe(a_pid,tribe_id))
               {
               continue;
               }            

            CfDebug("     3rd NEIGHBOUR (%d): %s::%s at %d\n",tribe_counter,a_context,a_name,a_pid);
            
            if (Nova_NewVertex(tribe_nodes,tribe_counter,3,a_pid,a_name,a_context))
               {            
               tribe_id[tribe_counter] = a_pid;
               tribe_adj[tribe_counter][j] = 1;
               tribe_adj[j][tribe_counter] = 1;
               tribe_counter++;
               }
            
            if (tribe_counter >= CF_TRIBE_SIZE-1)
               {
               break;
               }            
            }
         
         if (tribe_counter >= CF_TRIBE_SIZE-1)
            {
            break;
            }               
         
         DeleteItemList(nn);
         }
      
      if (tribe_counter >= CF_TRIBE_SIZE-1)
         {
         break;
         }                     
      }
   }

return tribe_counter;
}
#endif

/*************************************************************************/
/* Level                                                                 */
/*************************************************************************/

void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE],double *v,int dim)

{ int i, n;
  double max = 0.0001;

for (i = 0; i < dim; i++)
   {
   v[i] = 1.0;
   }

for (n = 0; n < 10; n++)
   {
   Nova_MatrixOperation(A,v,dim);
   }

for (i = 0; i < dim; i++)
   {
   if (v[i] > max)
      {
      max = v[i];
      }
   }

// Renormalize

if (max > 0.0001)
   {
   for (i = 0; i < dim; i++)
      {
      v[i] = v[i] / max;
      }
   }
}

/*************************************************************************/

void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE],double *v,int dim)

{ int i,j;
  double max = 0.000000001;
  double vp[CF_TRIBE_SIZE];

for (i = 0; i < dim; i++)
   {
   for (j = 0; j < dim; j++)
      {
      vp[i] += A[i][j] * v[j];
      }

   if (vp[i] > max)
      {
      max = vp[i];
      }
   }

if (max > 0.0001)
   {
   for (i = 0; i < dim; i++)
      {
      v[i] = vp[i] / max;

      if (isnan(v[i]))
         {
         v[i] = 0;
         }      
      }
   }
}

/*************************************************************************/
  
int Nova_AlreadyInTribe(int node, int *tribe_id)

{ int i;

for (i = 0; tribe_id[i] > 0; i++)
    {
    if (tribe_id[i] == node)
       {
       return true;
       }
    }

return false;
}

/*************************************************************************/

void Nova_InitVertex(struct CfGraphNode *tribe,int i)

{
tribe[i].real_id = 0;
tribe[i].shortname = NULL;
tribe[i].fullname = NULL;
tribe[i].context = NULL;
tribe[i].distance_from_centre = 0;
}

/*************************************************************************/

int Nova_NewVertex(struct CfGraphNode *tribe,int node,int distance,int real,char *topic_name,char *topic_context)

{ char sshort[CF_BUFSIZE],name[CF_BUFSIZE];
  char topic_id[CF_BUFSIZE];
  int j;

CfDebug("NEWVERT(%d,%d,%s:%s)\n",distance,real,topic_context,topic_name);

/* If more than a few nodes, don't waste visual space on repeated topics */

#ifdef HAVE_LIBMONGOC
if (strlen(topic_name) == 0)
   {
   Nova_GetTopicByTopicId(real,topic_name,topic_id,topic_context);
   }
#endif

sscanf(topic_name,"%32[^\n]",sshort);

if (strlen(sshort) == 0)
   {
   return false;
   }

if (node > 5)
   {
   for (j = 0; j < node; j++)
      {
      if (strcmp(tribe[j].shortname,sshort) == 0)
         {
         CfDebug("A similar topic already exists, so don't waste the space: %s\n",sshort);
         return false;
         }
      }
   }

tribe[node].real_id = real; 
tribe[node].shortname = xstrdup(sshort);
tribe[node].fullname = xstrdup(topic_name);
tribe[node].context = xstrdup(topic_context);
tribe[node].distance_from_centre = distance;
return true;
}

/*********************************************************************/

#ifdef HAVE_LIBMONGOC
struct Item *Nova_NearestNeighbours(int search_id,char *assoc_mask)

{
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  struct Item *list = NULL;
  int topic_id;
  char assoc_name[CF_BUFSIZE];
  char afwd[CF_BUFSIZE],abwd[CF_BUFSIZE];
  char assoc_context[CF_BUFSIZE];
  int assoc_id;

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicid,search_id);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_append_int(&bb,cfk_associations,1);
bson_append_int(&bb,cfk_associd,1);
bson_append_int(&bb,cfk_assoccontext,1);
bson_append_int(&bb,cfk_assocname,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_id = 0;
   
   while (bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),cfk_associations) == 0)
         {
         bson_iterator_init(&it2,bson_iterator_value(&it1));

         while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3,bson_iterator_value(&it2));

             assoc_id = 0;
             assoc_name[0] = '\0';
             assoc_context[0] = '\0';
             afwd[0] = '\0';
             abwd[0] = '\0';

             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfk_associd) == 0)
                   {
                   assoc_id = bson_iterator_int(&it3);
                   }   
                
                if (strcmp(bson_iterator_key(&it3),cfk_assocname) == 0)
                   {
                   strncpy(assoc_name,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   

                if (strcmp(bson_iterator_key(&it3),cfk_fwd) == 0)
                   {
                   strncpy(afwd,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   

                if (strcmp(bson_iterator_key(&it3),cfk_bwd) == 0)
                   {
                   strncpy(abwd,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   
                
                if (strcmp(bson_iterator_key(&it3),cfk_assoccontext) == 0)
                   {
                   strncpy(assoc_context,bson_iterator_string(&it3),CF_BUFSIZE-1);
                   }   
                }
             
             CfDebug(" - NEIGH topic %d has association %s %s::%s (%d)\n",topic_id,afwd,assoc_context,assoc_name,assoc_id);

             if (assoc_mask == NULL || FullTextMatch(assoc_mask,afwd))
                {
                PrependFullItem(&list,assoc_name,assoc_context,assoc_id,0);
                }
             }
         }
      }
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return list;
}
#endif

/*********************************************************************/

#ifdef HAVE_LIBMONGOC
struct Item *Nova_GetTopicsInContext(char *context)
    
{
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  struct Item *list = NULL;
  char topic_name[CF_BUFSIZE];
  char topic_context[CF_BUFSIZE];
  int topic_id;
  char assoc_name[CF_BUFSIZE];
  char afwd[CF_BUFSIZE],abwd[CF_BUFSIZE];
  char assoc_context[CF_BUFSIZE];
  int assoc_id;

if (!CFDB_Open(&conn))
   {
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_topiccontext,context);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_topicname,1);
bson_append_int(&bb,cfk_topicid,1);
bson_append_int(&bb,cfk_topiccontext,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
   topic_id = 0;
   
   while (bson_iterator_next(&it1))
      {
      if (strcmp(bson_iterator_key(&it1),cfk_topicname) == 0)
         {
         strncpy(topic_name,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_topiccontext) == 0)
         {
         strncpy(topic_context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topic_id = (int)bson_iterator_int(&it1);
         }
      }
   
   PrependFullItem(&list,topic_name,topic_context,topic_id,0);
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return list;
}
#endif

/*********************************************************************/
// Copy from ontology.c because of a linker bug
/*********************************************************************/

void Nova_DeClassifyTopic(char *classified_topic,char *topic,char *context)

{
context[0] = '\0';
topic[0] = '\0';

if (classified_topic == NULL)
   {
   return;
   }

if (*classified_topic == ':')
   {
   sscanf(classified_topic,"::%255[^\n]",topic);
   }
else if (strstr(classified_topic,"::"))
   {
   sscanf(classified_topic,"%255[^:]::%255[^\n]",context,topic);
   
   if (strlen(topic) == 0)
      {
      sscanf(classified_topic,"::%255[^\n]",topic);
      }
   }
else
   {
   strncpy(topic,classified_topic,CF_MAXVARSIZE-1);
   }
}

