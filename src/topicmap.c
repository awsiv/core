
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
}

/*****************************************************************************/
/* The main panels                                                           */
/*****************************************************************************/

int Nova_QueryTopicMap(char *typed_topic,char *result_type,char *buffer,int bufsize)

{

}

/*****************************************************************************/

int Nova_TopicByID(int id,char *result_type,char *buffer,int bufsize)
{
}

/*****************************************************************************/

void Nova_LookupUniqueTopic(char *typed_topic)

{ char query[CF_BUFSIZE],topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],to_type[CF_BUFSIZE];
  char topic_comment[CF_BUFSIZE];
  char from_name[CF_BUFSIZE],to_name[CF_BUFSIZE],from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE];
  char cfrom_assoc[CF_BUFSIZE],cto_assoc[CF_BUFSIZE],ctopic_type[CF_BUFSIZE],cto_type[CF_BUFSIZE];
  char type[CF_MAXVARSIZE],topic[CF_MAXVARSIZE];
  int sql_database_defined = false,trymatch = false,count = 0,matched = 0;
  struct Topic *tp,*tmatches = NULL;
  char safe[CF_MAXVARSIZE],safetype[CF_MAXVARSIZE];
  CfdbConn cfdb;
  int s,e;


DeTypeTopic(typed_topic,topic,type);

/* We need to set a scope for the regex stuff */

if (strlen(SQL_OWNER) > 0)
   {
   sql_database_defined = true;
   }
 
if (!sql_database_defined)
   {
   printf("No database defined...\n");
   return;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);

if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

/* First assume the item is a topic */

CfOut(cf_verbose,""," -> Treating the search string %s as a literal string\n",topic);

strcpy(safe,EscapeSQL(&cfdb,topic));
strcpy(safetype,EscapeSQL(&cfdb,type));

if (strlen(type) > 0)
   {
   snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment from topics where (topic_name='%s' or topic_id='%s') and topic_type='%s'",safe,CanonifyName(safe),safetype);
   }
else
   {
   snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment from topics where topic_name='%s' or topic_id='%s'",safe,CanonifyName(safe));
   }

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
   strncpy(topic_type,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   
   if (CfFetchColumn(&cfdb,3))
      {
      strncpy(topic_comment,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
      }
   else
      {
      strncpy(topic_comment,"",CF_BUFSIZE-1);
      }

   AddCommentedTopic(&tmatches,topic_name,topic_comment,topic_type);   
   count++;
   }

CfDeleteQuery(&cfdb);

if (count >= 1)
   {
   Nova_ShowTopicCosmos(&cfdb,topic_name,topic_id,topic_type,topic_comment);
   CfCloseDB(&cfdb);
   return;
   }

/* If no matches, try the associations */

count = 0;
matched = 0;
tmatches = NULL;

strncpy(safe,EscapeSQL(&cfdb,topic),CF_MAXVARSIZE);

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name from associations where from_assoc='%s' or to_assoc='%s'",safe,safe);

/* Expect multiple matches always with associations */

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,6);
   CfCloseDB(&cfdb);
   return;
   }

while (CfFetchRow(&cfdb))
   {
   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(from_assoc,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(to_assoc,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);

   if (!TopicExists(tmatches,from_assoc,to_assoc))
      {
      matched++;
      AddTopic(&tmatches,from_assoc,to_assoc);
      strncpy(ctopic_type,topic_type,CF_BUFSIZE-1);
      strncpy(cfrom_assoc,from_assoc,CF_BUFSIZE-1);
      strncpy(cto_assoc,to_assoc,CF_BUFSIZE-1);
      strncpy(cto_type,to_type,CF_BUFSIZE-1);
      }

   count++;
   }

CfDeleteQuery(&cfdb);

if (matched == 0)
   {
   Nova_NothingFound(typed_topic);
   CfCloseDB(&cfdb);
   return;
   }

if (matched > 1)
   {
   /* End assoc summary */
   
   for (tp = tmatches; tp != NULL; tp=tp->next)
      {
      Nova_ShowAssociationSummary(&cfdb,tp->topic_name,tp->topic_type);
      }

   CfCloseDB(&cfdb);
   return;
   }

if (matched == 1)
   {
   Nova_ShowAssociation(&cfdb,cfrom_assoc,cto_assoc,ctopic_type,cto_type);
   }

CfCloseDB(&cfdb);
}

/*********************************************************************/

void Nova_LookupMatchingTopics(char *typed_topic)

{ char query[CF_BUFSIZE],topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],to_type[CF_BUFSIZE];
  char topic_comment[CF_BUFSIZE];
  char from_name[CF_BUFSIZE],to_name[CF_BUFSIZE],from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE];
  char cfrom_assoc[CF_BUFSIZE],cto_assoc[CF_BUFSIZE],ctopic_type[CF_BUFSIZE],cto_type[CF_BUFSIZE];
  char type[CF_MAXVARSIZE],topic[CF_MAXVARSIZE];
  int  sql_database_defined = false,trymatch = false,count = 0,matched = 0;
  struct Topic *tp,*tmatches = NULL;
  CfdbConn cfdb;
  int s,e;

DeTypeTopic(typed_topic,topic,type);

CfOut(cf_verbose,""," -> Looking up topics matching \"%s\"\n",topic);

/* We need to set a scope for the regex stuff */

if (strlen(SQL_OWNER) > 0)
   {
   sql_database_defined = true;
   }
 
if (!sql_database_defined)
   {
   CfOut(cf_error,""," !! No knowlegde database defined...\n");
   return;
   }

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);

if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

/* First assume the item is a topic */

snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment from topics");

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,3);
   CfCloseDB(&cfdb);
   return;
   }

while(CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);

   if (BlockTextCaseMatch(topic,topic_name,&s,&e))
      {
      AddTopic(&tmatches,topic_name,topic_type);
      matched++;
      }
   }

CfDeleteQuery(&cfdb);

if (matched > 1)
   {
   //ShowTopicDisambiguation(&cfdb,tmatches,topic);
   CfCloseDB(&cfdb);
   return;
   }

if (matched == 1)
   {
   for (tp = tmatches; tp != NULL; tp=tp->next)
      {
      //ShowTopicCosmology(&cfdb,tp->topic_name,topic_id,tp->topic_type,tp->topic_comment);
      }
   CfCloseDB(&cfdb);
   return;
   }

/* If no matches, try the associations */

count = 0;
matched = 0;
tmatches = NULL;

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name from associations");

/* Expect multiple matches always with associations */

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,6);
   CfCloseDB(&cfdb);
   return;
   }

while(CfFetchRow(&cfdb))
   {
   strncpy(from_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(from_assoc,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(to_assoc,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(&cfdb,4),CF_BUFSIZE-1);
   strncpy(to_name,CfFetchColumn(&cfdb,5),CF_BUFSIZE-1);

   if (BlockTextCaseMatch(topic,from_assoc,&s,&e)||BlockTextCaseMatch(topic,to_assoc,&s,&e))
      {
      if (!TopicExists(tmatches,from_assoc,to_assoc))
         {
         matched++;
         AddTopic(&tmatches,from_assoc,to_assoc);
         strncpy(ctopic_type,topic_type,CF_BUFSIZE-1);
         strncpy(cfrom_assoc,from_assoc,CF_BUFSIZE-1);
         strncpy(cto_assoc,to_assoc,CF_BUFSIZE-1);
         strncpy(cto_type,to_type,CF_BUFSIZE-1);
         }
      }
   }

CfDeleteQuery(&cfdb);

if (matched == 0)
   {
   //NothingFound(typed_topic);
   CfCloseDB(&cfdb);
   return;
   }

if (matched > 1)
   {
   /* End assoc summary */
   
   for (tp = tmatches; tp != NULL; tp=tp->next)
      {
      Nova_ShowAssociationSummary(&cfdb,tp->topic_name,tp->topic_type);
      }

   CfCloseDB(&cfdb);
   return;
   }

if (matched == 1)
   {
   Nova_ShowAssociation(&cfdb,cfrom_assoc,cto_assoc,ctopic_type,cto_type);
   }

CfCloseDB(&cfdb);
}

/*****************************************************************************/

void Nova_ScanLocalHierarchy(CfdbConn *cfdb,char *this_name,char *this_id,char *this_type,char *this_comment)

{ struct Topic *other_topics = NULL,*topics_this_type = NULL;
  struct TopicAssociation *associations = NULL;
  struct Occurrence *occurrences = NULL;
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],associate[CF_BUFSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],safe[CF_BUFSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE],to_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE];
  enum representations locator_type;
  struct Rlist *rp;

/* Collect data - first other topics of same type */

snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment from topics where topic_type='%s' order by topic_name desc",this_type);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,3);
   return;
   }

while(CfFetchRow(cfdb))
   {
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);

   if (CfFetchColumn(cfdb,3))
      {
      strncpy(topic_comment,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
      }
   else
      {
      strncpy(topic_comment,"",CF_BUFSIZE-1);
      }

   if (strcmp(topic_name,this_name) == 0 || strcmp(topic_id,this_name) == 0)
      {
      continue;
      }

   AddCommentedTopic(&other_topics,topic_name,topic_comment,topic_type);
   }

CfDeleteQuery(cfdb);

/* Collect data - then topics of this topic-type */

snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment from topics where topic_type='%s' order by topic_name desc",CanonifyName(this_name));

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,3);
   return;
   }

while(CfFetchRow(cfdb))
   {
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);

   if (CfFetchColumn(cfdb,3))
      {
      strncpy(topic_comment,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
      }
   else
      {
      strncpy(topic_comment,"",CF_BUFSIZE-1);
      }
   
   if (strcmp(topic_name,this_name) == 0 || strcmp(topic_id,this_name) == 0)
      {
      continue;
      }

   AddCommentedTopic(&topics_this_type,topic_name,topic_comment,topic_type);
   }

CfDeleteQuery(cfdb);

// pass these data back into a buffer

return Nova_ReturnResults(this_name,this_type,this_comment,other_topics,associations,occurrences,topics_this_type);
}

/*****************************************************************************/

void Nova_ScanLeadsAssociations(CfdbConn *cfdb,char *this_name,char *this_id,char *this_type,char *this_comment)

{ struct Topic *other_topics = NULL,*topics_this_type = NULL;
  struct TopicAssociation *associations = NULL;
  struct Occurrence *occurrences = NULL;
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],associate[CF_BUFSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],safe[CF_BUFSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE],to_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE];
  enum representations locator_type;
  struct Rlist *rp;

/* Then associated topics */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name from associations where to_name='%s'",this_name);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,6);
   return;
   }

/* Look in both directions for associations - first into */

while(CfFetchRow(cfdb))
   {
   struct Rlist *this = NULL;
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(cfdb,4),CF_BUFSIZE-1);
   strncpy(associate,CfFetchColumn(cfdb,5),CF_BUFSIZE-1);

   AppendRlist(&this,TypedTopic(topic_name,topic_type),CF_SCALAR);
   AddTopicAssociation(&associations,bassociation,NULL,this,false);
   DeleteRlist(this);
   }

CfDeleteQuery(cfdb);

/* ... then onto */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name from associations where from_name='%s'",this_name);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,6);
   return;
   }

while(CfFetchRow(cfdb))
   {
   struct Rlist *this = NULL;
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(cfdb,4),CF_BUFSIZE-1);
   strncpy(associate,CfFetchColumn(cfdb,5),CF_BUFSIZE-1);

   AppendRlist(&this,TypedTopic(associate,to_type),CF_SCALAR);
   AddTopicAssociation(&associations,fassociation,NULL,this,false);
   DeleteRlist(this);
   }

CfDeleteQuery(cfdb);
}

/*****************************************************************************/

void Nova_ScanOccurrences(CfdbConn *cfdb,char *this_name,char *this_id,char *this_type,char *this_comment)

{ struct Topic *other_topics = NULL,*topics_this_type = NULL;
  struct TopicAssociation *associations = NULL;
  struct Occurrence *occurrences = NULL;
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],associate[CF_BUFSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],safe[CF_BUFSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE],to_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE];
  enum representations locator_type;
  struct Rlist *rp;

/* Finally occurrences of the mentioned topic */

strncpy(safe,EscapeSQL(cfdb,this_name),CF_BUFSIZE);
snprintf(query,CF_BUFSIZE,"SELECT topic_name,locator,locator_type,subtype from occurrences where topic_name='%s' or subtype='%s' order by locator_type,subtype",this_id,safe);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The occurrences database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,4);
   return;
   }

while(CfFetchRow(cfdb))
   {
   struct Rlist *this = NULL;
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(locator,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   locator_type = Str2Int(CfFetchColumn(cfdb,2));
   strncpy(subtype,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);

   if (strcmp(subtype,this_name) == 0)
      {
      AppendRlist(&this,topic_name,CF_SCALAR);
      }
   else
      {
      AppendRlist(&this,subtype,CF_SCALAR);
      }
   
   AddOccurrence(&occurrences,locator,this,locator_type);
   DeleteRlist(this);
   }

CfDeleteQuery(cfdb);

/* Now print the results */

//ShowHtmlResults(this_name,this_type,this_comment,other_topics,associations,occurrences,topics_this_type);
}

/*****************************************************************************/

char *LocateTopicMapImage()
{
 return NULL;
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
   snprintf(filenode,CF_MAXVARSIZE-1,"%s_%s.png",view,CanonifyName(names[topic]));
   }
else
   {
   snprintf(filenode,CF_MAXVARSIZE-1,"%s.png",CanonifyName(names[topic]));
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

if (cfstat(filename,&sb) != -1)
   {
   CfOut(cf_inform,"","Graph \"%s\" already exists, delete to refresh\n",filename);
   return;
   }

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
/* Level                                                                 */
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

