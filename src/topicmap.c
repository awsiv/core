
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

printf("LOADED: db=%s,type=%d,owner=%s,passwd=%s,server=%s,connect=%s\n",SQL_DATABASE,SQL_TYPE,SQL_OWNER,SQL_PASSWD,SQL_SERVER,SQL_CONNECT_NAME);
}

/*****************************************************************************/
/* The main panels                                                           */
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

{ char from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE],topic_type[CF_BUFSIZE],to_type[CF_BUFSIZE];
  char work[CF_BUFSIZE],query[CF_MAXVARSIZE],from_name[CF_BUFSIZE],to_name[CF_BUFSIZE],from_id[CF_SMALLBUF];
  CfdbConn cfdb;
  


printf("LOADED: db=%s,type=%d,owner=%s,passwd=%s,server=%s,connect=%s\n",SQL_DATABASE,SQL_TYPE,SQL_OWNER,SQL_PASSWD,SQL_SERVER,SQL_CONNECT_NAME);

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name,from_id from associations where from_id='%d'",pid);

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
Debugcfdb(&cfdb);

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 7)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,7);
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
   strncpy(from_id,CfFetchColumn(&cfdb,6),CF_SMALLBUF-1);


   printf("GOT: %s, %s...\n",from_name,from_assoc);
   snprintf(buffer,CF_BUFSIZE,"Association \"%s\" (with inverse \"%s\"), ",from_assoc,to_assoc);

//format directly

//      Nova_ShowAssociation(&cfdb,cfrom_assoc,cto_assoc,ctopic_type,cto_type);
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*********************************************************************/

void Nova_SearchTopicMap(char *typed_topic,char *buffer,int bufsize)

{ CfdbConn cfdb;  
  char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],to_type[CF_BUFSIZE];
  char topic_comment[CF_BUFSIZE],topic[CF_BUFSIZE],type[CF_BUFSIZE],query[CF_BUFSIZE];
  char from_name[CF_BUFSIZE],from_assoc[CF_BUFSIZE],to_assoc[CF_BUFSIZE],to_name[CF_BUFSIZE];
  char work[CF_BUFSIZE];
  int pid,s,e;

printf("LOADED: db=%s,type=%d,owner=%s,passwd=%s,server=%s,connect=%s\n",SQL_DATABASE,SQL_TYPE,SQL_OWNER,SQL_PASSWD,SQL_SERVER,SQL_CONNECT_NAME);

buffer[0] = '\0';
  
DeTypeTopic(typed_topic,topic,type);

CfOut(cf_verbose,""," -> Looking up topics matching \"%s\"\n",topic);

/* We need to set a scope for the regex stuff */

strcat(buffer,"before knowledge<br>");

if (strlen(SQL_OWNER) == 0)
   {
   CfOut(cf_error,""," !! No knowlegde database defined...\n");
   return;
   }

strcat(buffer,"after knowledge<br>");

printf("PTR = %u\n",&cfdb);

CfConnectDB(&cfdb,SQL_TYPE,SQL_SERVER,SQL_OWNER,SQL_PASSWD,SQL_DATABASE);
Debugcfdb(&cfdb);
    
printf("returning (%d,%d)\n",cfdb.maxrows,cfdb.maxcolumns);

if (!cfdb.connected)
   {
   CfOut(cf_error,""," !! Could not open sql_db %s\n",SQL_DATABASE);
   return;
   }

Debugcfdb(&cfdb);

snprintf(query,CF_MAXVARSIZE-1,"SELECT topic_name,topic_id,topic_type,topic_comment,pid from topics");

CfNewQueryDB(&cfdb,query);

printf("returning (%d,%d) WITH SIZE %d=%d\n",cfdb.maxrows,cfdb.maxcolumns,sizeof(cfdb),sizeof(CfdbConn));
Debugcfdb(&cfdb);

if (cfdb.maxcolumns != 5)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,5);
   CfCloseDB(&cfdb);
   return;
   }

Debugcfdb(&cfdb);

while(CfFetchRow(&cfdb))
   {
   strncpy(topic_name,CfFetchColumn(&cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_id,CfFetchColumn(&cfdb,1),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(&cfdb,2),CF_BUFSIZE-1);
   strncpy(topic_comment,CfFetchColumn(&cfdb,3),CF_BUFSIZE-1);
   pid = Str2Int(CfFetchColumn(&cfdb,4));

   if (BlockTextCaseMatch(topic,topic_name,&s,&e))
      {
      snprintf(work,CF_MAXVARSIZE,"Topic %s in category %s with id",topic_name,topic_type,pid);
      strcat(buffer,work);
      }
   }

CfDeleteQuery(&cfdb);

/* Then matching associations */

snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name,pid from associations");

/* Expect multiple matches always with associations */

CfNewQueryDB(&cfdb,query);

if (cfdb.maxcolumns != 7)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb.maxcolumns,7);
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

// format Nova_ShowAssociationSummary(&cfdb,tp->topic_name,tp->topic_type);
      }
   }

CfDeleteQuery(&cfdb);
CfCloseDB(&cfdb);
}

/*****************************************************************************/

void Nova_ScanTheRest(CfdbConn *cfdb,char *this_name,char *this_type,char *buffer, int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],associate[CF_BUFSIZE],work[CF_BUFSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE],to_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE],query[CF_MAXVARSIZE];
  enum representations locator_type;
  int pid;

/* Collect data - first other topics of same type */

snprintf(query,CF_BUFSIZE,"SELECT topic_name,topic_id,topic_type,topic_comment,pid from topics where topic_type='%s' order by topic_name desc",this_type);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 5)
   {
   CfOut(cf_error,""," !! The topics database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,5);
   return;
   }

snprintf(buffer,CF_MAXVARSIZE,"<ul>\n");

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

   pid = Str2Int(CfFetchColumn(cfdb,4));

   snprintf(work,CF_MAXVARSIZE,"<li>link(%s) in %s, %s</li>",topic_name,topic_type,topic_comment);
   Join(buffer,work);
   
   }

CfDeleteQuery(cfdb);

strcat(buffer,"</ul>\n");

/* Then sub-topics of this topic-type */

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

// print
   }

CfDeleteQuery(cfdb);
}

/*****************************************************************************/

void Nova_ScanLeadsAssociations(CfdbConn *cfdb,char *this_name,int from_id,char *buffer, int bufsize)

{ char topic_name[CF_BUFSIZE],topic_type[CF_BUFSIZE],associate[CF_MAXVARSIZE],work[CF_MAXVARSIZE];
  char query[CF_BUFSIZE],fassociation[CF_BUFSIZE],bassociation[CF_BUFSIZE],safe[CF_BUFSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE],to_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE];
  enum representations locator_type;
  struct Rlist *rp;

/* Then associated topics */

  snprintf(query,CF_BUFSIZE,"SELECT from_name,from_type,from_assoc,to_assoc,to_type,to_name from associations where from_id='%d' or to_name='%s' order by from_name desc",from_id,this_name);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 6)
   {
   CfOut(cf_error,""," !! The associations database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,6);
   return;
   }

/* Look in both directions for associations - first into */

while(CfFetchRow(cfdb))
   {
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(cfdb,4),CF_BUFSIZE-1);
   strncpy(associate,CfFetchColumn(cfdb,5),CF_BUFSIZE-1);

   snprintf(work,CF_MAXVARSIZE,"<li>  %s \"%s\" %s::%s\n",topic_name,fassociation,to_type,associate);
   strcat(buffer,work);
   snprintf(work,CF_MAXVARSIZE,"<ul>\n");
   strcat(buffer,work);

   }

strcat(buffer,"</ul>\n");

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
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(topic_type,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   strncpy(fassociation,CfFetchColumn(cfdb,2),CF_BUFSIZE-1);
   strncpy(bassociation,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);
   strncpy(to_type,CfFetchColumn(cfdb,4),CF_BUFSIZE-1);
   strncpy(associate,CfFetchColumn(cfdb,5),CF_BUFSIZE-1);

// print
   }

CfDeleteQuery(cfdb);
}

/*****************************************************************************/

void Nova_ScanOccurrences(CfdbConn *cfdb,int this_id,char *buffer, int bufsize)

{ char topic_name[CF_BUFSIZE],query[CF_MAXVARSIZE];
  char locator[CF_BUFSIZE],subtype[CF_BUFSIZE];
  enum representations locator_type;

/* Finally occurrences of the mentioned topic */

snprintf(query,CF_BUFSIZE,"SELECT topic_name,locator,locator_type,subtype from occurrences where from_id='%d' order by locator_type,subtype",this_id);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 4)
   {
   CfOut(cf_error,""," !! The occurrences database table did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,4);
   return;
   }

while(CfFetchRow(cfdb))
   {
   strncpy(topic_name,CfFetchColumn(cfdb,0),CF_BUFSIZE-1);
   strncpy(locator,CfFetchColumn(cfdb,1),CF_BUFSIZE-1);
   locator_type = Str2Int(CfFetchColumn(cfdb,2));
   strncpy(subtype,CfFetchColumn(cfdb,3),CF_BUFSIZE-1);

// print
   }

CfDeleteQuery(cfdb);
}

/*****************************************************************************/

char *LocateTopicMapImage(int pid)
{
// return hub/common/pid.png
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

