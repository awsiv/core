
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

NewClass("am_policy_hub");

#ifdef HAVE_LIBMONGOC 
CFDB_GetValue("SQL_TYPE",retval,CF_MAXVARSIZE);
SQL_TYPE = Str2dbType(retval);

CFDB_GetValue("SQL_DATABASE",retval,CF_MAXVARSIZE);
strcpy(SQL_DATABASE,retval);
CFDB_GetValue("SQL_OWNER",retval,CF_MAXVARSIZE);
strcpy(SQL_OWNER,retval);
CFDB_GetValue("SQL_PASSWD",retval,CF_MAXVARSIZE);
strcpy(SQL_PASSWD,retval);
CFDB_GetValue("SQL_SERVER",retval,CF_MAXVARSIZE);
strcpy(SQL_SERVER,retval);
CFDB_GetValue("SQL_CONNECT_NAME",retval,CF_MAXVARSIZE);
strcpy(SQL_CONNECT_NAME,retval);
CFDB_GetValue("document_root",retval,CF_MAXVARSIZE);
strncpy(DOCROOT,retval,CF_MAXVARSIZE);

Debug("Loaded values: db=%s,type=%d,owner=%s,passwd=%s,server=%s,connect=%s,docroot=%s\n",SQL_DATABASE,SQL_TYPE,SQL_OWNER,SQL_PASSWD,SQL_SERVER,SQL_CONNECT_NAME,DOCROOT);
#endif
}

/*****************************************************************************/
/* The main panels                                                           */
/*****************************************************************************/

int Nova_GetPidForTopic(char *typed_topic)
    
{ CfdbConn cfdb;
  char *sp,query[CF_MAXVARSIZE],topic[CF_BUFSIZE],type[CF_BUFSIZE];
  int ret;

Nova_WebTopicMap_Initialize();
  
if (strlen(SQL_OWNER) == 0 || typed_topic == NULL)
   {
   return 0;
   }

Nova_DeClassifyTopic(typed_topic,topic,type); // Linker trouble - copy this from core

//strcpy(topic,typed_topic);

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return false;
   }

if (strlen(type) > 0)
   {
   snprintf(query,CF_MAXVARSIZE-1,"SELECT pid from topics where topic_name = '%s' and topic_context like '%%%s%%'",topic,type);
   }
else
   {
   snprintf(query,CF_MAXVARSIZE-1,"SELECT pid from topics where topic_name = '%s'",topic);
   }

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 1)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,1);
   CfCloseDB(&cfdb);
   return false;
   }

// Pick a representative if there are several

if (CfFetchRow(&cfdb))
   {
   ret = Str2Int(CfFetchColumn(&cfdb,0));
   }
else
   {
   ret = 0;
   }

CfDeleteQuery(&cfdb);
return ret;
}

/*****************************************************************************/

int Nova_GetTopicByPid(int pid,char *topic_name,char *topic_id,char *topic_context)

{ CfdbConn cfdb;
  char *sp,query[CF_MAXVARSIZE];
  int ret;
 
if (strlen(SQL_OWNER) == 0)
   {
   return false;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return false;
   }

snprintf(query,CF_MAXVARSIZE-1,"SELECT topic_name,topic_id,topic_context from topics where pid = '%d'",pid);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 3)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,3);
   CfCloseDB(&cfdb);
   return false;
   }

if (CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   ret = true;
   }
else
   {
   ret = false;
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
return ret;
}

/*****************************************************************************/

int Nova_QueryTopicMap(char *typed_topic,char *result_type,char *buffer,int bufsize)

{
return false;
}

/*****************************************************************************/

int Nova_TopicByID(int id,char *result_type,char *buffer,int bufsize)

{
// if stat DOCROOT/hub/common/id.png
 return false;
}

/******************************************************************************/

void Nova_LookupUniqueAssoc(int pid,char *buffer,int bufsize)

{ char from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE],topic_context[CF_BUFSIZE],to_context[CF_BUFSIZE];
  char work[CF_BUFSIZE],query[CF_MAXVARSIZE],from_name[CF_BUFSIZE],to_name[CF_BUFSIZE],from_id[CF_SMALLBUF];
  CfdbConn cfdb;
  int from_pid,to_pid;  

snprintf(query,sizeof(query),"SELECT from_name,from_context,from_assoc,to_assoc,to_context,to_name,from_id,to_id from associations where from_id='%d'",pid);

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
Debugcfdb(&cfdb);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 8)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,8);
   CfCloseDB(&cfdb);
   return;
   }

while (CfFetchRow(&cfdb))
   {
   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(from_assoc,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(to_assoc,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_context,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);
   from_pid = Str2Int(CfFetchColumn(&cfdb,6));
   to_pid = Str2Int(CfFetchColumn(&cfdb,7));

   snprintf(buffer,CF_BUFSIZE,"Association \"%s\" (with inverse \"%s\"), ",from_assoc,to_assoc);

//format directly

//      Nova_ShowAssociation(&cfdb,cfrom_assoc,cto_assoc,ctopic_context,cto_context);
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*********************************************************************/

int Nova_SearchTopicMap(char *search_topic,char *buffer,int bufsize)

{ CfdbConn cfdb;  
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE],to_context[CF_BUFSIZE];
  char topic_comment[CF_BUFSIZE],query[CF_BUFSIZE];
  char from_name[CF_BUFSIZE],from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE],to_name[CF_BUFSIZE];
  char work[CF_BUFSIZE],*sp;
  int save_pid = 0,pid,s,e,count = 0;

strcpy(buffer,"<div id=\"disambig\">\n<h2>The search suggests these topics:</h2>\n<ul>\n");

// Handle search strings context::topics

if (strlen(SQL_OWNER) == 0)
   {
   snprintf(buffer,bufsize,"No knowledge database has yet formed ... please wait");
   return 0;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return 0;
   }

snprintf(query,CF_MAXVARSIZE-1,"SELECT topic_name,topic_id,topic_context,pid from topics");

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,4);
   CfCloseDB(&cfdb);
   return 0;
   }

while(CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);   
   pid = Str2Int(CfFetchColumn(&cfdb,3));

   if (BlockTextCaseMatch(search_topic,topic_name,&s,&e))
      {
      count++;
      Nova_AddTopicSearchBuffer(pid,topic_name,topic_context,buffer,bufsize);
      save_pid = pid;
      }
   }

CfDeleteQuery(&cfdb);

/* Then matching associations */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_context,from_assoc,to_assoc,to_context,to_name from associations");

/* Expect multiple matches always with associations */

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,6);
   CfCloseDB(&cfdb);
   strcat(buffer,"</ul></div>\n");
   return 0;
   }

while(CfFetchRow(&cfdb))
   {
   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(from_assoc,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(to_assoc,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_context,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);

   if (BlockTextCaseMatch(search_topic,from_assoc,&s,&e)||BlockTextCaseMatch(search_topic,to_assoc,&s,&e))
      {
      count++;
      Nova_AddAssocSearchBuffer(from_assoc,to_assoc,buffer,bufsize);
      }
   }

if (count == 0)
   {
   snprintf(work,CF_MAXVARSIZE,"(no suitable results determined for %s)",search_topic);
   Join(buffer,work,bufsize);
   }

strcat(buffer,"</ul></div>\n");
CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);

if (count == 1)
   {
   return save_pid;
   }
else
   {
   return 0;
   }
}

/*****************************************************************************/

void Nova_ScanTheRest(int pid,char *buffer, int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE],associate[CF_BUFSIZE],work[CF_BUFSIZE];
  char query[CF_MAXVARSIZE],buf[CF_BUFSIZE];
  char this_name[CF_BUFSIZE],this_id[CF_BUFSIZE],this_type[CF_BUFSIZE];
  enum representations locator_type;
  CfdbConn cfdb;  
  int tpid,count = 0;

if (strlen(SQL_OWNER) == 0)
   {
   snprintf(buffer,bufsize,"No knowledge database has yet formed ... please wait");
   return;
   }

if (!Nova_GetTopicByPid(pid,this_name,this_id,this_type))
   {
   snprintf(buffer,bufsize,"No such topic was found (db %s)",SQL_DATABASE);
   return;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

if (strcmp(this_type,"any") == 0)
   {
   return;
   }

snprintf(buffer,CF_MAXVARSIZE,"<div id=\"others\"><h2>Other topics mentioned in the context of \"%s\":</h2>\n",this_type);

/* sub-topics of this topic-type */

strcat(buffer,"<ul>\n"); // outer list
snprintf(buf,CF_BUFSIZE-1,"<li>%s</li><ul>\n",Nova_PidURL(pid,this_name)); // Start sublist
Join(buffer,buf,bufsize);

snprintf(query,sizeof(query),"SELECT topic_name,topic_id,topic_context,pid from topics where topic_context='%s' order by topic_name asc",this_id);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,4);
   CfCloseDB(&cfdb);
   return;
   }

while(CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);

   if (strcmp(topic_name,this_name) == 0 || strcmp(topic_id,this_name) == 0)
      {
      continue;
      }

   tpid = Str2Int(CfFetchColumn(&cfdb,3));

   snprintf(buf,CF_BUFSIZE-1,"<li>%s</li>\n",Nova_PidURL(tpid,topic_name));
   Join(buffer,buf,bufsize);
   count++;
   }

if (count == 0)
   {
   snprintf(buf,CF_BUFSIZE-1,"<li>(no sub-categories)</li>\n");
   Join(buffer,buf,bufsize);
   }

strcat(buffer,"</ul></li>\n"); // close sublist

/* Collect data - other topics of same type */

snprintf(query,sizeof(query),"SELECT topic_name,topic_id,topic_context,pid from topics where topic_context='%s' order by topic_name asc",this_type);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,4);
   CfCloseDB(&cfdb);
   return;
   }

count = 0;

while(CfFetchRow(&cfdb))
   {
   count++;
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);

   if (strcmp(topic_name,this_name) == 0 || strcmp(topic_id,this_name) == 0)
      {
      continue;
      }

   tpid = Str2Int(CfFetchColumn(&cfdb,3));

   snprintf(buf,CF_BUFSIZE-1,"<li>%s</li>\n",Nova_PidURL(tpid,topic_name));
   Join(buffer,buf,bufsize);   
   }

if (count == 0)
   {
   snprintf(buf,CF_BUFSIZE-1,"<li>No other topics</li>\n");
   Join(buffer,buf,bufsize);   
   }

strcat(buffer,"</ul></div>\n");
CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*****************************************************************************/

void Nova_ScanLeadsAssociations(int pid,char *buffer,int bufsize)

{ char from_name[CF_BUFSIZE],from_context[CF_BUFSIZE],to_name[CF_BUFSIZE],work[CF_BUFSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],save[CF_BUFSIZE];
  char to_context[CF_BUFSIZE],*sp;
  enum representations locator_type;
  struct Rlist *rp;
  CfdbConn cfdb;
  int have_data = false, any_data = false;

if (strlen(SQL_OWNER) == 0)
   {
   snprintf(buffer,bufsize,"No knowledge database has yet formed ... please wait");
   return;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

/* Then associated topics */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_context,from_assoc,to_assoc,to_context,to_name,from_id,to_id from associations where from_id='%d' order by from_assoc asc",pid);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 8)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,8);
   return;
   }

/* Look in both directions for associations - first into */

snprintf(buffer,bufsize,"<p><div id=\"associations\">\n<h2>Insight, leads and perspectives:</h2>\n");

strcat(buffer,"<ul>\n");
save[0] = '\0';

while(CfFetchRow(&cfdb))
   {
   int from_pid,to_pid;

   have_data = any_data = true;

   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);   
   strncpy(from_context,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_context,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);
   from_pid = Str2Int(CfFetchColumn(&cfdb,6));
   to_pid = Str2Int(CfFetchColumn(&cfdb,7));

   if (strcmp(fassociation,save) != 0)
      {
      if (strlen(save) != 0)
         {
         strcat(buffer,"</ul></li>\n");
         }
      
      strncpy(save,fassociation,CF_BUFSIZE-1);
      
      snprintf(work,CF_MAXVARSIZE,"<li>  %s \"%s\" \n <ul>\n",from_name,fassociation);
      Join(buffer,work,bufsize);
      }

   if (strlen(to_context) > 0)
      {
      snprintf(work,CF_MAXVARSIZE,"<li>  %s (in %s)</li>\n",Nova_PidURL(to_pid,to_name),to_context);
      }
   else
      {
      snprintf(work,CF_MAXVARSIZE,"<li>  %s </li>\n",Nova_PidURL(to_pid,to_name));
      }
   Join(buffer,work,bufsize);
   }

if (!have_data)
   {
   snprintf(work,CF_MAXVARSIZE,"<li>(no direct leads) </li>\n");
   Join(buffer,work,bufsize);   
   }
else
   {
   strcat(buffer,"</ul></li>\n"); // still have one open <ul> now
   }

CfDeleteQuery(&cfdb);

/* ... then onto */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_context,from_assoc,to_assoc,to_context,to_name,from_id,to_id from associations where to_id='%d' order by to_assoc asc",pid);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 8)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,8);
   strcat(buffer,"</ul>\n");
   CfCloseDB(&cfdb);
   return;
   }

save[0] = '\0';
have_data = false;

while(CfFetchRow(&cfdb))
   {
   int from_pid,to_pid;
   have_data = true;

   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);   
   strncpy(from_context,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_context,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);

   from_pid = Str2Int(CfFetchColumn(&cfdb,6));
   to_pid = Str2Int(CfFetchColumn(&cfdb,7));

   if (strcmp(bassociation,save) != 0)
      {
      if (strlen(save) != 0)
         {
         strcat(buffer,"</ul></li>\n");
         }
      
      strncpy(save,bassociation,CF_BUFSIZE-1);
      
      snprintf(work,CF_MAXVARSIZE,"<li>  %s \"%s\"\n<ul>\n",to_name,bassociation);
      Join(buffer,work,bufsize);
      }
   
   snprintf(work,CF_MAXVARSIZE,"<li>  %s (in %s)<li> \n",Nova_PidURL(from_pid,from_name),from_context);
   Join(buffer,work,bufsize);
   }

if (have_data)
   {
   strcat(buffer,"</ul></li>\n");
   any_data = true;
   }
else
   {
   snprintf(work,CF_MAXVARSIZE,"<li> (no inferred leads)</li> \n");
   Join(buffer,work,bufsize);   
   }

strcat(buffer,"</ul></div>\n");

if (!any_data)
   {
   buffer[0] = '\0';
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*****************************************************************************/

void Nova_ScanOccurrences(int this_id,char *buffer, int bufsize)

{ char topic_name[CF_BUFSIZE],topic_context[CF_BUFSIZE],query[CF_MAXVARSIZE];
  char occurrence_context[CF_BUFSIZE],locator[CF_BUFSIZE],subtype[CF_BUFSIZE];
  enum representations locator_type;
  CfdbConn cfdb;
  int have_data = false;
  struct AlphaList context_list;

if (strlen(SQL_OWNER) == 0)
   {
   snprintf(buffer,bufsize,"No knowledge database has yet formed ... please wait");
   return;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   snprintf(buffer,bufsize," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

/* Finally occurrences of the mentioned topic */

snprintf(query,sizeof(query),"SELECT topic_name,topic_context from topics where pid='%d'",this_id);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 2)
   {
   snprintf(buffer,bufsize," !! The topic database table did not promise the expected number of fields - got %d expected %d for pid %d\n",cfdb.maxcolumns,2,this_id);
   return;
   }

if (CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_context,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   }

CfDeleteQuery(&cfdb);

snprintf(query,sizeof(query),"SELECT context,locator,locator_type,subtype from occurrences where context like '%%%s%%' order by locator_type,subtype",CanonifyName(topic_name));

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 4)
   {
   snprintf(buffer,bufsize," !! The occurrences table did not promise the expected number of fields - got %d expected %d for \"%%%s%%\"\n",cfdb.maxcolumns,4,topic_name);
   return;
   }

snprintf(buffer,bufsize,
         "<div id=\"occurrences\">\n"
         "<h2>References to '<span id=\"subject\">%s</span>' in the context of `<span id=\"category\">%s</span>'</h2>"
         "<ul>\n",topic_name,topic_context);

AtomizeTopicContext(&context_list,topic_context);
PrependAlphaList(&context_list,CanonifyName(topic_name));


while(CfFetchRow(&cfdb))
   {
   have_data = true;
   strncpy(occurrence_context,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(locator,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   locator_type = Str2Int(CfFetchColumn(&cfdb,2));
   strncpy(subtype,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);

   // Match occurrences that could overlap with the current context

   if (EvaluateORString(occurrence_context,context_list,0))
      {
      if (strcmp(occurrence_context,CanonifyName(topic_name)) == 0)
         {
         Nova_AddOccurrenceBuffer("*",locator,locator_type,subtype,buffer,bufsize);
         }
      else
         {
         Nova_AddOccurrenceBuffer(occurrence_context,locator,locator_type,subtype,buffer,bufsize);
         }
      }
   else if (strcmp(occurrence_context,CanonifyName(topic_name)) == 0)
      {
      Nova_AddOccurrenceBuffer(occurrence_context,locator,locator_type,subtype,buffer,bufsize);
      }
   }

DeleteAlphaList(&context_list);

if (!have_data)
   {
   strcat(buffer,"<li>No immediate information is available on this topic</li>");
   }

strcat(buffer,"</ul></div>\n");
CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*************************************************************************/

struct Item *Nova_GetBusinessGoals(char *handle)

{ char from_name[CF_BUFSIZE],from_context[CF_BUFSIZE],to_name[CF_BUFSIZE],work[CF_BUFSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],save[CF_BUFSIZE];
  char to_context[CF_BUFSIZE],*sp;
  struct Item *worklist = NULL, *ip;
  enum representations locator_type;
  int have_data = false;
  CfdbConn cfdb;

if (strlen(SQL_OWNER) == 0)
   {
   return NULL;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return NULL;
   }

/* Then associated topics */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_context,from_assoc,to_assoc,to_context,to_name,from_id,to_id from associations where from_name='%s' and from_assoc='%s'",handle,NOVA_GOAL);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 8)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,8);
   CfCloseDB(&cfdb);
   return NULL;
   }

/* Look in both directions for associations - first into */

while(CfFetchRow(&cfdb))
   {
   int from_pid,to_pid;
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);   
   from_pid = Str2Int(CfFetchColumn(&cfdb,6));
   to_pid = Str2Int(CfFetchColumn(&cfdb,7));

   PrependFullItem(&worklist,to_name,NULL,0,0);   
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);

for (ip = worklist; ip !=  NULL; ip=ip->next)
   {
   Nova_FillInTopicComment(ip);
   }

return worklist;
}
    
/*************************************************************************/
/* Level                                                                 */
/*************************************************************************/

int Nova_AddTopicSearchBuffer(int pid,char *topic_name,char *topic_context,char *buffer,int bufsize)

{ char buf[CF_BUFSIZE];

snprintf(buf,CF_BUFSIZE-1,"<li>\"%s\" is mentioned in the context of %s</li>\n",Nova_PidURL(pid,topic_name),topic_context);
Join(buffer,buf,bufsize);
return true;
}

/*************************************************************************/

int Nova_AddAssocSearchBuffer(char *from_assoc,char *to_assoc,char *buffer,int bufsize)

{ char buf[CF_MAXVARSIZE];
snprintf(buf,sizeof(buf),"<li>There is a relationship: \"%s\" &harr;",Nova_AssocURL(from_assoc));
Join(buffer,buf,bufsize);
snprintf(buf,sizeof(buf)," \"%s\" found between certain topics</li>\n",Nova_AssocURL(to_assoc));
Join(buffer,buf,bufsize);
return true;
}

/*************************************************************************/

void Nova_AddOccurrenceBuffer(char *context,char *locator,enum representations locator_type,char *represents,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 
switch (locator_type)
   {
   case cfk_url:
       snprintf(work,CF_BUFSIZE-1,"<li><i>%s</i>:: <span id=\"url\"> %s</span> (URL)</li>\n",context,Nova_URL(locator,represents));
       break;
       
   case cfk_web:
       snprintf(work,CF_BUFSIZE-1,"<li><i>%s</i>:: <span id=\"url\">%s ...%s</a> </span> (URL)<li>\n",context,Nova_URL(locator,represents),locator);
       break;

   case cfk_file:
       snprintf(work,CF_BUFSIZE-1,"<li>%s (file)<li>\n",locator);
       break;

   case cfk_db:
       snprintf(work,CF_BUFSIZE-1,"<li>%s (DB)</li>\n",locator);
        break;          

   case cfk_literal:
       snprintf(work,CF_BUFSIZE-1,"<li><i>%s</i>:: <p> \"%s\" (Text)</p></li>\n",context,locator);
       break;

   case cfk_image:
       snprintf(work,CF_BUFSIZE-1,"<li><p><div id=\"embedded_image\"><a href=\"%s\"><img src=\"%s\"></a></div></p></li>\n",locator,locator);
       break;

   case cfk_portal:
       snprintf(work,CF_BUFSIZE-1,"<li>Portal: <a href=\"%s\" target=\"_blank\">%s</a> </span> (URL)</li>\n",locator,represents);
       break;
       
   default:
       break;
   }

Join(buffer,work,bufsize);
}

/*************************************************************************/

char *Nova_PidURL(int pid,char *s)

{ static char buf[CF_MAXVARSIZE];

snprintf(buf,CF_MAXVARSIZE-1,"<a href=\"/knowledge.php?pid=%d\">%s</a>",pid,s);
return buf;
}

/*************************************************************************/

char *Nova_AssocURL(char *s)

{ static char buf[CF_MAXVARSIZE];
 
snprintf(buf,CF_MAXVARSIZE-1,"<a href=\"/knowledge.php?assoc=%s\">%s</a>",s,s);
return buf;
}
/*************************************************************************/

char *Nova_URL(char *s,char *rep)

{ static char buf[CF_MAXVARSIZE];
 
snprintf(buf,CF_MAXVARSIZE-1,"<a href=\"%s\">%s</a>: ",s,rep);
return buf;
}

/*************************************************************************/

void Nova_FillInTopicComment(struct Item *ip)

{ CfdbConn cfdb;
  char *sp,query[CF_MAXVARSIZE];
  int ret;
 
if (strlen(SQL_OWNER) == 0)
   {
   return;
   }

/*
CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
    
if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

snprintf(query,CF_MAXVARSIZE-1,"SELECT topic_comment,pid from topics where topic_name = '%s'",ip->name);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 2)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,2);
   CfCloseDB(&cfdb);
   return;
   }

if (CfFetchRow(&cfdb))
   {
   ip->classes = strdup(CfFetchColumn(&cfdb,0));
   ip->counter = Str2Int(CfFetchColumn(&cfdb,1));
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
*/

ip->classes = strdup("PLEASE FILL ME IN topicmap.c");
}



/*************************************************************************/

void AtomizeTopicContext(struct AlphaList *context_list,char *topic_context)

{ struct Rlist *rp,*l = SplitRegexAsRList(topic_context,"[|&.]",99,false);

InitAlphaList(context_list);

for (rp = l; rp != NULL; rp=rp->next)
   {
   PrependAlphaList(context_list,rp->item);
   }

DeleteRlist(l);
}

/*****************************************************************************/
/* Plot cosmos                                                               */
/*****************************************************************************/

void Nova_PlotTopicCosmos(int topic,double **full_adj,char **names,int dim,char *view)

/* This assumes that we have the whole graph in a matrix */
    
{ char filename[CF_BUFSIZE], filenode[CF_MAXVARSIZE];
  struct CfGraphNode tribe_nodes[CF_TRIBE_SIZE];
  int i,tribe_id[CF_TRIBE_SIZE],tribe_size,tertiary_boundary = 0;
  double tribe_evc[CF_TRIBE_SIZE];
  double **tribe_adj;
  struct stat sb;

if (LICENSES == 0)
   {
   return;
   }

CfOut(cf_verbose,""," -> Create Cosmos for %s\n",names[topic]);

if (view)
   {
   snprintf(filenode,CF_MAXVARSIZE-1,"%s_%d.png",view,topic);
   }
else
   {
   snprintf(filenode,CF_MAXVARSIZE-1,"%d.png",topic);
   }

if (strlen(GRAPHDIR) > 0)
   {
   snprintf(filename,CF_BUFSIZE,"%s/%s",GRAPHDIR,filenode);
   MakeParentDirectory(filename,false);
   }
else
   {
   strcpy(filename,filenode);
   }

// Clean up old data
unlink(filename);

tribe_adj = (double **)malloc(sizeof(double *)*CF_TRIBE_SIZE);

for (i = 0; i < CF_TRIBE_SIZE; i++)
   {
   tribe_adj[i] = (double *)malloc(sizeof(double)*CF_TRIBE_SIZE);
   }

/* Count the  number of nodes in the solar system, to max number based on Dunbar's limit */  

if (tribe_size = Nova_GetTribe(tribe_id,tribe_nodes,tribe_adj,names,topic,full_adj,dim,&tertiary_boundary))
   {
   Debug("Tribesize of %d for %s...........\n",tribe_size,names[topic]);

   Nova_EigenvectorCentrality(tribe_adj,tribe_evc,CF_TRIBE_SIZE);
   Nova_IlluminateTribe(tribe_id,tribe_nodes,tribe_adj,tribe_size,tribe_evc,names,topic,full_adj,dim,tertiary_boundary);
   Nova_DrawTribe(filename,tribe_id,tribe_nodes,tribe_adj,tribe_size,tribe_evc,names,topic,full_adj,dim,tertiary_boundary);
   }

for (i = 0; i < CF_TRIBE_SIZE; i++)
   {
   free(tribe_adj[i]);
   }

free(tribe_adj);
}

/*************************************************************************/
/* Local patch computation                                               */
/*************************************************************************/

int Nova_GetTribe(int *tribe_id,struct CfGraphNode *tribe_nodes, double **tribe_adj,char **n,int topic,double **full_adj,int dim_full,int *tertiary_boundary)

/* This function generates a breadth-first connected sub-graph of the full graph
   and identifies the orbits and distances, up to a maximum of Dunbar's tribe-size */
    
{ int counter = 0,possible_neighbour,i,j;
  int nearest_neighbour_boundary;

/* Initialize sub-graph */
  
for (i = 0; i < CF_TRIBE_SIZE; i++)
   {
   Nova_InitVertex(tribe_nodes,i);
   tribe_id[i] = -1;
   
   for (j = 0; j < CF_TRIBE_SIZE; j++)
      {
      tribe_adj[i][j] = 0;
      }      
   }

*tertiary_boundary = 1;

/* Anchor sub-graph */

tribe_id[0] = topic;
Nova_NewVertex(tribe_nodes,0,n[topic],0,topic);

/* Probe sub-graph */
    
counter = 1;

CfOut(cf_verbose,""," -> Tribe for topic %d = %s\n",topic,n[topic]);

for (possible_neighbour = 0; possible_neighbour < dim_full; possible_neighbour++)
   {
   if (possible_neighbour == topic)
      {
      continue;
      }
   
   if (full_adj[topic][possible_neighbour] > 0 || full_adj[possible_neighbour][topic] > 0)
      {
      CfOut(cf_verbose,""," ->> (%d) \"%s\" orbits topic \"%s\"\n",counter,n[possible_neighbour],n[topic]);

      if (Nova_NewVertex(tribe_nodes,counter,n[possible_neighbour],1,possible_neighbour))
         {
         tribe_id[counter] = possible_neighbour;      
         tribe_adj[counter][0] = 1;
         tribe_adj[0][counter] = 1;
         counter++;
         }

      if (counter >= CF_TRIBE_SIZE - 1)
         {
         return counter;
         }
      }
   }

/* Look recursively at 2nd order neighbourhoods */

nearest_neighbour_boundary = counter;
*tertiary_boundary = nearest_neighbour_boundary;

for (j = 1; j < nearest_neighbour_boundary; j++)
   {
   for (possible_neighbour = 0; possible_neighbour < dim_full; possible_neighbour++)
      {
      if (possible_neighbour == topic)
         {
         continue;
         }

      if (possible_neighbour == tribe_id[j])
         {
         continue;
         }

      if (Nova_AlreadyInTribe(possible_neighbour,tribe_id))
         {
         continue;
         }

      if (full_adj[tribe_id[j]][possible_neighbour] > 0)
         {
         CfOut(cf_verbose,""," ->> (%d) \"%s\" 2nd orbit of topic \"%s\"\n",counter,n[possible_neighbour],n[tribe_id[j]]);
            
         if (Nova_NewVertex(tribe_nodes,counter,n[possible_neighbour],2,possible_neighbour))
            {
            tribe_id[counter] = possible_neighbour;            
            tribe_adj[counter][j] = 1;
            tribe_adj[j][counter] = 1;
            counter++;
            }

         if (counter >= CF_TRIBE_SIZE - 1)
            {
            return counter;
            }
         }
      }
   }

/* Look recursively at 3nd order neighbourhoods for bridge connections*/

for (j = nearest_neighbour_boundary; j < counter; j++)
   {
   for (possible_neighbour = 0; possible_neighbour < dim_full; possible_neighbour++)
      {
      if (possible_neighbour == topic)
         {
         continue;
         }

      if (possible_neighbour == tribe_id[j])
         {
         continue;
         }

      if (Nova_AlreadyInTribe(possible_neighbour,tribe_id))
         {
         continue;
         }

      if (full_adj[tribe_id[j]][possible_neighbour] > 0)
         {
         CfOut(cf_verbose,""," -> (%d) \"%s\" 3rd orbit of topic \"%s\"\n",counter,n[possible_neighbour],n[tribe_id[j]]);
               
         if (Nova_NewVertex(tribe_nodes,counter,n[possible_neighbour],3,possible_neighbour))
            {
            tribe_id[counter] = possible_neighbour;            
            tribe_adj[counter][j] = 1;
            tribe_adj[j][counter] = 1;            
            counter++;
            }

         if (counter >= CF_TRIBE_SIZE - 1)
            {
            return counter;
            }
         }
      }
   }

return counter;
}

/*************************************************************************/
/* Level                                                                 */
/*************************************************************************/

void Nova_EigenvectorCentrality(double **A,double *v,int dim)

{ int i, n;

for (i = 0; i < dim; i++)
   {
   v[i] = 1.0;
   }

for (n = 0; n < 10; n++)
   {
   Nova_MatrixOperation(A,v,dim);
   }
}

/*************************************************************************/

void Nova_MatrixOperation(double **A,double *v,int dim)

{ int i,j;
  double max = 0.000000001;
  double *vp;
  
vp = (double *)malloc(sizeof(double)*dim);

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

for (i = 0; i < dim; i++)
   {
   v[i] = vp[i] / max;
   }

free(vp);
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

{ const int parent = 1;
  
tribe[i].real_id = 0;
memset(tribe[i].shortname,0,CF_SMALLBUF);
tribe[i].fullname =  NULL;
tribe[i].potential = 0.0;      
tribe[i].x = 0.0;
tribe[i].y = 0.0;
tribe[i].radius = CF_MIN_RADIUS;
tribe[i].orbit_parent = parent;
tribe[i].distance_from_centre = 0;
}

/*************************************************************************/

int Nova_NewVertex(struct CfGraphNode *tribe,int node,char *name,int distance,int real)

{ const int parent = 1;
  char sshort[CF_MAXVARSIZE];
  int j;

sshort[0] = '\0';

if (name == NULL)
   {
   return false;
   }

sscanf(name,"%*[^:]::%32[^\n]",sshort);

/* If more than a few nodes, don't waste visual space on repeated topics */

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

tribe[node].real_id = real; 
strncpy(tribe[node].shortname,sshort,CF_SMALLBUF-1);
tribe[node].fullname = name;
tribe[node].potential = 0.0;      
tribe[node].x = 0.0;
tribe[node].y = 0.0;
tribe[node].radius = 0.0;
tribe[node].orbit_parent = parent;
tribe[node].distance_from_centre = distance;
return true;
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

