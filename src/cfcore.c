
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

snprintf(filename,CF_BUFSIZE-1,"/var/cfengine/document_root.dat");

if ((fin = fopen(filename,"r")) == NULL)
   {
   fprintf(fout,"<h1>Unable to open the map %s</h1>",filename);
   return;
   }

rootpath[0] = '\0';
fscanf(fin,"%s",rootpath);
fclose(fin);

snprintf(filename,CF_BUFSIZE,"%s/%s",rootpath,name);

if ((fin = fopen(filename,"r")) == NULL)
   {
   fprintf(fout,"Unable to open the map fragment %s",filename);
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

      snprintf(buffer,CF_BUFSIZE-1,line,WEBDRIVER);
      fprintf(fout,"%s",buffer);
      }
   
   fclose(fin);
   }
}

/*****************************************************************************/

void Nova_MainPage(char *host,struct Item *eliminate)
    
{ FILE *fout;

if (LICENSES == 0)
   {
   return;
   }
 
if ((fout = fopen("mainpage.html","w")) == NULL)
   {
   return;
   }

Nova_ShowAllGraphs(fout,host,eliminate);

fclose(fout);
}

/*****************************************************************************/

void Nova_OtherPages(char *host,struct Item *eliminate)
    
{ FILE *fout;
  char name[CF_BUFSIZE],exist[CF_BUFSIZE];
  char id[CF_BUFSIZE],desc[CF_BUFSIZE];
  struct stat s1,s2;
  int i;

if (LICENSES == 0)
   {
   return;
   }
  
for (i = 0; i < CF_OBSERVABLES; i++)
   {
   Nova_LookupAggregateClassName(i,id,desc);

   if (strcmp(id,"spare") == 0)
      {
      continue;
      }

   snprintf(exist,15,"%d",i);

   if (IsItemIn(eliminate,exist))
      {
      continue;
      }

   snprintf(name,CF_BUFSIZE,"%s.html",id);
   snprintf(exist,CF_BUFSIZE,"%s.mag",id);

   if (cfstat(name,&s1) != -1 && cfstat(exist,&s2) != -1)
      {
      if (s1.st_mtime > s2.st_mtime)
         {
         return;
         }
      }
   
   if ((fout = fopen(name,"w")) == NULL)
      {
      return;
      }

   Nova_ShowGraph(fout,host,i,s2.st_mtime,(enum observables)i);
   fclose(fout);
   }
}

/*****************************************************************************/

void Nova_ShowAllGraphs(FILE *fp,char *host,struct Item *eliminate)

{ int i,terminated1,terminated2,terminated3;
  struct stat sb;
  char img[CF_BUFSIZE];
  char url[CF_BUFSIZE];
  char hist[CF_BUFSIZE];
  char mag[CF_BUFSIZE];
  char week[CF_BUFSIZE];
  char datestr[CF_MAXVARSIZE];
  char name[CF_BUFSIZE],description[CF_BUFSIZE];
  
fprintf(fp,"<table border=1>");
 
for (i = 0; i < CF_OBSERVABLES; i++)
    {
    Nova_LookupAggregateClassName(i,name,description);

    if (strcmp(name,"spare") == 0)
       {
       continue;
       }

    snprintf(hist,15,"%d",i);

    snprintf(datestr,CF_MAXVARSIZE,"%s",cf_ctime(&DATESTAMPS[i]));
    Chop(datestr);

    /* Check is data stream has stopped for this service */
    
    if (IsItemIn(eliminate,hist))
       {
       continue;
       }

    /* Check current data stream */
    
    if (i > ob_spare)
       {
       snprintf(description,CF_BUFSIZE-1,"measurement %s",name);
       }

    fprintf(fp,"<tr>");

    snprintf(img,CF_BUFSIZE,"%s",description);
    snprintf(url,CF_BUFSIZE,"%s.html",name);
    snprintf(hist,CF_BUFSIZE,"%s_hist.html",name);
    snprintf(mag,CF_BUFSIZE,"%s_mag.html",name);
    snprintf(week,CF_BUFSIZE,"%s_week.html",name);

    if (cfstat(url,&sb) == -1)
       {
       continue;
       }    

    snprintf(img,CF_BUFSIZE,"%s_weekly.png",name);
    
    if (cfstat(img,&sb) == -1)
       {
       terminated2 = true;
       continue;
       }

    snprintf(url,CF_BUFSIZE,"reports/%s/%s.html",host,name);
    fprintf(fp,"<th nowrap><div id=\"ip\">%s</div><br><br><a href=\"%s\">%s</a><br><br><small>Latest data<br>%s</small></th>\n",host,URLControl("%s",url),name,datestr);

    terminated1 = terminated2 = terminated3  = false;
    
    snprintf(img,CF_BUFSIZE,"%s_mag.png",name);

    if (cfstat(img,&sb) == -1)
       {
       terminated1 = true;
       }

    snprintf(img,CF_BUFSIZE,"%s_hist.png",name);
        
    if (cfstat(img,&sb) == -1)
       {
       terminated3 = true;
       }
    
    if (!terminated1)
       {
       snprintf(mag,CF_BUFSIZE,"reports/%s/%s_mag.html",host,name);
       snprintf(img,CF_BUFSIZE,"reports/%s/%s_mag.png",host,name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=\"300\"></a></td>\n",URLControl("%s",mag),img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated2)
       {
       snprintf(week,CF_BUFSIZE,"reports/%s/%s_week.html",host,name);
       snprintf(img,CF_BUFSIZE,"reports/%s/%s_weekly.png",host,name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",URLControl("%s",week),img);
       }
    else
       {
       continue;
       //fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated3)
       {
       snprintf(hist,CF_BUFSIZE,"reports/%s/%s_hist.html",host,name);
       snprintf(img,CF_BUFSIZE,"reports/%s/%s_hist.png",host,name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",URLControl("%s",hist),img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Insufficient data</center></td>\n");
       }
    
    fprintf(fp,"</tr>");
    }

fprintf(fp,"</table>");
}

/*****************************************************************************/

void Nova_ShowGraph(FILE *fout,char *host,int i,time_t date,enum observables obs)

{ char name1[CF_BUFSIZE],name2[CF_BUFSIZE],img[CF_BUFSIZE],datestr[CF_BUFSIZE];
  char name[CF_BUFSIZE],description[CF_BUFSIZE];
  double x1,y1,z1,x2,y2,z2;
  FILE *fp1,*fp2;

Nova_LookupAggregateClassName(i,name,description);
  
snprintf(name1,CF_BUFSIZE-1,"%s.mag",name);
snprintf(img,CF_BUFSIZE-1,"reports/%s/%s_mag.png",host,name);

snprintf(datestr,CF_MAXVARSIZE,"%s",cf_ctime(&date));
Chop(datestr);

fprintf(fout,"<h4>%s</h4>\n",description);

fprintf(fout,"<div id=\"legend\">\n");

fprintf(fout,"<h4>Last 3 years</h4>\n");

snprintf(name2,CF_MAXVARSIZE-1,"reports/%s/%s_yr.html",host,name);
fprintf(fout,"<p><a href=\"%s\">Long history</a> provides a rough trend over the past 3 years\n",URLControl("%s",name2));

fprintf(fout,"<h2>Last 4 hours</h2>\n");

fprintf(fout,"<h4>Latest info observed %s</h4>\n",datestr);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"590\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");


fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   return;
   }

while (!feof(fp1))
   {
   fscanf(fp1,"%lf %lf %lf %lf",&x1,&y1,&z1,&y2);
   
   fprintf(fout,"<tr><td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td></tr>\n",x1,y2,y1,z1);
   }

fprintf(fout,"</table></p>\n");
fclose(fp1);

/* Averages */

fprintf(fout,"<h2>Past and previous weeks</h2>\n");

snprintf(name1,CF_BUFSIZE-1,"%s.E-sigma",name);
snprintf(name2,CF_BUFSIZE-1,"%s.q",name);
snprintf(img,CF_BUFSIZE-1,"reports/%s/%s_weekly.png",host,name);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"590\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   return;
   }

if ((fp2 = fopen(name2,"r")) == NULL)
   {
   return;
   }

while (!feof(fp1))
   {
   fscanf(fp1,"%lf %lf %lf",&x1,&y1,&z1);
   fscanf(fp2,"%lf %lf %lf",&x2,&y2,&z2);
   
   fprintf(fout,"<tr><td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td></tr>\n",x1,y2,y1,z1);
   }

fprintf(fout,"</table></p>\n");
fprintf(fout,"</div>\n");
fclose(fp1);
fclose(fp2);
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

