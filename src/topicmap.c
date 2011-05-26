/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

void Nova_DeClassifyTopic(char *typed_topic,char *topic,char *type);

/*****************************************************************************/
/*                                                                           */
/* File: topicmap.c                                                          */
/*                                                                           */
/*****************************************************************************/

void Nova_WebTopicMap_Initialize()

{ char retval[CF_MAXVARSIZE];

NewClass("am_php_module");

#ifdef HAVE_LIBMONGOC 
CFDB_GetValue("document_root",retval,CF_MAXVARSIZE);
strncpy(DOCROOT,retval,CF_MAXVARSIZE);
snprintf(GRAPHDIR,CF_MAXVARSIZE,"%s/graphs",DOCROOT);

Debug("Loaded values: docroot=%s\n",DOCROOT);
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
#endif
}

/*****************************************************************************/

int Nova_GetTopicIdForTopic(char *typed_topic)
    
{
#ifdef HAVE_LIBMONGOC
  char topic[CF_BUFSIZE],type[CF_BUFSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  int topic_id = 0;
  
Nova_DeClassifyTopic(typed_topic,topic,type); // Linker trouble - copy this from core

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_topicname,topic);

if (strlen(type) > 0)
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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
#endif
}

/*****************************************************************************/

int Nova_GetTopicByTopicId(int search_id,char *topic_name,char *topic_id,char *topic_context)

{
#ifdef HAVE_LIBMONGOC
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  int topicid = 0;

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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
#endif
}

/*********************************************************************/

int Nova_SearchTopicMap(char *search_topic,char *buffer,int bufsize)

{
#ifdef HAVE_LIBMONGOC
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;
  char topic_name[CF_BUFSIZE],jsonEscapedStr[CF_BUFSIZE];
  char topic_context[CF_BUFSIZE];
  int topic_id;
  char work[CF_BUFSIZE];

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
   return false;
   }

/* BEGIN query document */

if (!EMPTY(search_topic))
   {
   bson_buffer_init(&bb);
   bson_append_regex(&bb,cfk_topicname,search_topic,"");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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
   
   EscapeJson(topic_name,jsonEscapedStr,CF_BUFSIZE-1);
   snprintf(work,CF_BUFSIZE,"{ \"context\": \"%s\", \"topic\": \"%s\", \"id\": %d },",topic_context,jsonEscapedStr,topic_id);
   Join(buffer,work,CF_BUFSIZE);
   }

buffer[strlen(buffer)-1] = ']';

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);
return true;
#endif
}

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

for(ip=worklist;ip!=NULL;ip=ip->next)
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
#ifdef HAVE_LIBMONGOC
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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
#endif
}

/*****************************************************************************/

void Nova_ScanOccurrences(int this_id,char *buffer, int bufsize)

{
#ifdef HAVE_LIBMONGOC
  enum representations locator_type;
  struct Rlist *rp,*frags,*atoms,*rrp;
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
   return;
   }

/* BEsnprintf(searchstring,CF_BUFSIZE,".*\.*%s\.*.*",topic);GIN query document */

Nova_GetTopicByTopicId(this_id,topic_name,topic_id,topic_context);

if (strcmp("any",topic_context) == 0)
   {
   snprintf(searchstring,CF_BUFSIZE,".*\\.*%s\\.*.*",topic_id);
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

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,0);
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

   for (rp = frags; rp != NULL; rp=rp->next)
      {
      int found = false;
      
      if (strcmp(rp->item,topic_id) == 0)
         {
         Nova_AddOccurrenceBuffer(rp->item,locator,locator_type,represents,buffer,bufsize);
         break;
         }
      else
         {
         atoms = SplitRegexAsRList(context,"[.&()]",10,false);

         for (rrp = atoms; rrp != NULL; rrp=rrp->next)
            {
            if (strcmp(rrp->item,topic_id) == 0)
               {               
               Nova_AddOccurrenceBuffer(rp->item,locator,locator_type,represents,buffer,bufsize);
               found = true;
               break;
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

struct Item *Nova_GetBusinessGoals(char *handle)

{ char querytopic[CF_BUFSIZE];
  struct Item *worklist = NULL, *ip;
  int pid;

snprintf(querytopic,CF_BUFSIZE,"handles::%s",handle);
pid = Nova_GetTopicIdForTopic(querytopic);

worklist = Nova_NearestNeighbours(pid,NOVA_GOAL);

for (ip = worklist; ip !=  NULL; ip=ip->next)
   {
   Nova_FillInGoalComment(ip);
   }

return worklist;
}
    
/*************************************************************************/

struct Item *Nova_GetUniqueBusinessGoals()

{
#ifdef HAVE_LIBMONGOC
  struct Item *worklist = NULL, *ip;
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
   return false;
   }

/* BEGIN query document */

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_fwdsearch,NOVA_GOAL);
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_associations,1);
bson_append_int(&bb,cfk_associd,1);
bson_append_int(&bb,cfk_assoccontext,1);
bson_append_int(&bb,cfk_assocname,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';

   topic_context[0] = '\0';
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
                }
             
             // Record the topics that represent goals
             PrependFullItem(&worklist,assoc_name,NULL,assoc_id,0);
             }
         }
      }
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);

for (ip = worklist; ip !=  NULL; ip=ip->next)
   {
   Nova_FillInGoalComment(ip);
   }

return worklist;
#endif
}
    
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

// Get comment goals.* or targets.%s etc

searchstring[0] = '\0';
  
for (rp = GOALCATEGORIES; rp != NULL; rp=rp->next)
   {
   snprintf(work,CF_MAXVARSIZE-1,"%s.%s|",rp->item,CanonifyName(ip->name));
   strcat(searchstring,work);
   }

searchstring[strlen(searchstring)-1] = '\0';

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,0);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   while (bson_iterator_next(&it1))
      {
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_occurlocator) == 0)
         {
         ip->classes = strdup(bson_iterator_string(&it1));
         return;
         }   
      }
   }

ip->classes = strdup("No description available");
#endif
}

/*************************************************************************/

char *Nova_GetBundleComment(char *bundle)

{
#ifdef HAVE_LIBMONGOC
  static char buf[CF_BUFSIZE];
  struct Rlist *rp;
  char searchstring[CF_MAXVARSIZE],work[CF_MAXVARSIZE];
  bson_buffer bb;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  mongo_connection conn;

// Get comment goals.* or targets.%s etc

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
   return NULL;
   }

bson_buffer_init(&bb);
bson_append_string(&bb,cfk_occurrep,"description");
bson_append_string(&bb,cfk_occurcontext,"bundle");
bson_from_buffer(&query,&bb);

/* BEGIN RESULT DOCUMENT */

bson_buffer_init(&bb);
bson_append_int(&bb,cfk_occurlocator,1);
bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

cursor = mongo_find(&conn,MONGO_KM_OCCURRENCES,&query,&field,0,0,0);
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

return NULL;
#endif
}

/*****************************************************************************/
/* Plot cosmos                                                               */
/*****************************************************************************/

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

/*************************************************************************/
/* Local patch computation                                               */
/*************************************************************************/

int Nova_GetTribe(int *tribe_id,struct CfGraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int pid,char *view_pattern)

/* This function generates a breadth-first connected sub-graph of the full graph
   and identifies the orbits and distances, up to a maximum of Dunbar's tribe-size */

{ char topic_name[CF_BUFSIZE],topic_context[CF_BUFSIZE];
  char *a_name,*a_context,view[CF_MAXVARSIZE];
  int from_pid,to_pid,a_pid;
  char query[CF_BUFSIZE];
  struct CfGraphNode neighbours1[CF_TRIBE_SIZE],neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE],neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE];
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

   Debug("NEAREST NEIGHOUR: %s::%s at %d\n",a_context,a_name,a_pid);

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
         
         Debug("  2nd NEIGHOUR: %s::%s at %d\n",a_context,a_name,a_pid);

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

            Debug("     3rd NEIGHBOUR (%d): %s::%s at %d\n",tribe_counter,a_context,a_name,a_pid);
            
            if (Nova_NewVertex(tribe_nodes,tribe_counter,3,a_pid,a_name,a_context))
               {            
               neighbours3[i][j][tribe_counter].real_id = a_pid;
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
tribe[i].distance_from_centre = 0;
}

/*************************************************************************/

int Nova_NewVertex(struct CfGraphNode *tribe,int node,int distance,int real,char *topic_name,char *topic_context)

{ char sshort[CF_BUFSIZE],name[CF_BUFSIZE];
  char topic_id[CF_BUFSIZE];
  int j;

/* If more than a few nodes, don't waste visual space on repeated topics */

if (strlen(topic_name) == 0)
   {
   Nova_GetTopicByTopicId(real,topic_name,topic_id,topic_context);
   }

if (node > 5)
   {
   for (j = 0; j < node; j++)
      {
      if (strcmp(tribe[j].shortname,sshort) == 0)
         {
         Debug("A similar topic already exists, so don't waste the space: %s\n",sshort);
         return false;
         }
      }
   }

sscanf(topic_name,"%32[^\n]",sshort);

if (strlen(sshort) == 0)
   {
   return false;
   }

tribe[node].real_id = real; 
tribe[node].shortname = strdup(sshort);
tribe[node].fullname = strdup(name);
tribe[node].distance_from_centre = distance;
return true;
}

/*********************************************************************/

struct Item *Nova_NearestNeighbours(int search_id,char *assoc_mask)

{
#ifdef HAVE_LIBMONGOC
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
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
             
             Debug(" - topic %d has associate %s::%s (%d)\n",topic_id,afwd,assoc_context,assoc_name,assoc_id);

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
#endif
}

/*********************************************************************/

struct Item *Nova_GetTopicsInContext(char *context)
    
{
#ifdef HAVE_LIBMONGOC
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

if (!CFDB_Open(&conn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to knowledge map");
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

cursor = mongo_find(&conn,MONGO_KM_TOPICS,&query,&field,0,0,0);
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
#endif
}

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

