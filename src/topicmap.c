
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: cfcore.c                                                            */
/*                                                                           */
/*****************************************************************************/

void Nova_SpecialQuote(char *name,char *type)

{ char filename[CF_BUFSIZE],rootpath[CF_BUFSIZE];
  FILE *fin,*fout = stdout;
  struct Item *ip,*file = NULL;
  int have_title = false;

snprintf(filename,CF_BUFSIZE,"%s/%s",DOCROOT,name);

if ((fin = fopen(filename,"r")) == NULL)
   {
   fprintf(fout,"Nova Unable to open the local file fragment %s",filename);
   }
else
   {
   char line[CF_BUFSIZE],buffer[CF_BUFSIZE];

   while (!feof(fin))
      {
      line[0] = '\0';
      fgets(line,CF_BUFSIZE,fin);

      if (IsHtmlHeader(line))
         {
         continue;
         }

      if (strstr(line,"<h1>"))
         {
         have_title = true;
         }
      
      snprintf(buffer,CF_BUFSIZE-1,line,WEBDRIVER);
      AppendItem(&file,buffer,NULL);
      }
   
   fclose(fin);

   if (!have_title)
      {
      CfHtmlTitle(fout,name);
      }

   for (ip = file; ip != NULL; ip=ip->next)
      {      
      fprintf(fout,"%s",ip->name);
      }
   
   DeleteItemList(file);
   }
}

/*****************************************************************************/
/* Plot cosmos                                                               */
/*****************************************************************************/

void Nova_PlotTopicCosmos(int topic,double **full_adj,char **names,int dim,char *view)

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

