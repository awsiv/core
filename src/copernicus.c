/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: copernicus.c                                                        */
/*                                                                           */
/* Created: Wed Sep  2 04:41:11 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

extern int LIGHTRED,YELLOW,ORANGE,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,SKY;
extern int GREYS[CF_SHADES];
extern int BLUES[CF_SHADES];
extern int YELLOWS[CF_SHADES];
extern int PINKS[CF_SHADES];
extern int BROWNS[CF_SHADES];

#ifdef HAVE_GD_H

/*****************************************************************************/

void Nova_PrimeGraph(struct Rlist **semantics)

{
PrependRScalar(semantics,NOVA_GIVES,CF_SCALAR);
PrependRScalar(semantics,NOVA_USES,CF_SCALAR);
PrependRScalar(semantics,NOVA_IMPACTS,CF_SCALAR);
PrependRScalar(semantics,NOVA_ISIMPACTED,CF_SCALAR);
PrependRScalar(semantics,NOVA_BUNDLE_DATA,CF_SCALAR);
PrependRScalar(semantics,NOVA_BUNDLE_DATA_INV_B,CF_SCALAR);
PrependRScalar(semantics,NOVA_BUNDLE_DATA_INV_P,CF_SCALAR);
}

/*****************************************************************************/

void Nova_DrawTribe(char *filename,int *tribe_id,struct CfGraphNode *tribe_node,double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,double *tribe_evc,int topic,char *buffer,int bufsize)

{ struct CfGraphNode neighbours1[CF_TRIBE_SIZE],neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE],
     neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE];
  char s[CF_MAXVARSIZE],pngfile[CF_MAXVARSIZE],work[CF_BUFSIZE];
  int tribe_tops[CF_TRIBE_SIZE], num_tops,size1,size2[CF_TRIBE_SIZE],
      size3[CF_TRIBE_SIZE][CF_TRIBE_SIZE],trail[CF_TRIBE_SIZE],i,j,k;
  double x,y,theta0,dtheta0,theta1,dtheta1,theta2,dtheta2,pi = 3.1416;
  double orbital_r1,orbital_r2,orbital_r3,orbital_r4,orbital_r5,corona;
  double max_x = 0,max_y = 0, min_x = 0, min_y = 0, lagrange;
  struct CfDataView cfv;
  int centre = Nova_GetMaxEvcNode(tribe_evc,tribe_size);
  FILE *fout;

Nova_ClearTrail(trail);

for (i = 0; i < tribe_size; i++)
   {
   if (i == topic)
      {
      // The topic might not be central, so inflate its importance a little
      tribe_node[i].radius = 1.5 * CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }
   else
      {
      tribe_node[i].radius = CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }
   }

/* Centre on most imporant topic */

Nova_AnchorTrail(trail,centre);

cfv.height = 550;
cfv.width = 800;
cfv.origin_x = 500;
cfv.origin_y = 375;
cfv.scale_x = (double)cfv.width/2;
cfv.scale_y = (double)cfv.height/4;
cfv.im = gdImageCreate(cfv.width,cfv.height);
Nova_MakeCosmosPalette(&cfv);

/* Rendering */

for (y = 0; y < cfv.height; y++)
   {
   gdImageLine(cfv.im,0,y,cfv.width,y,SKY);
   }

tribe_node[centre].x = 0;
tribe_node[centre].y = 0;
tribe_node[centre].distance_from_centre = 0;
neighbours1[centre].angle = 0;

// Write the image map

strncpy(pngfile,ReadLastNode(filename),CF_MAXVARSIZE-1);

snprintf(buffer,bufsize,"<img src=\"graphs/%s\" USEMAP=\"#knowledge_system_%s\" alt=\"%s image pending\">\n",pngfile,CanonifyName(pngfile),tribe_node[0].fullname);
snprintf(work,CF_BUFSIZE,"<map name=\"knowledge_system_%s\">\n",CanonifyName(pngfile));
Join(buffer,work,bufsize);

/* Pre-compute relative positions for scaling */

// First orbit

size1 = Nova_SplayAdjacent(centre,tribe_adj,tribe_size,tribe_node,trail,neighbours1);

if (size1 < 1)
   {
   snprintf(buffer,CF_MAXVARSIZE,"Singleton topic - %d associations",size1);
   return;
   }

theta0 = 0.64; 
dtheta0 = 2 * pi / (double)size1;

corona = CF_MIN_RADIUS + CF_RADIUS_SCALE;

for (i = 0; i < size1; i++)
   {
   struct CfGraphNode tmp[CF_TRIBE_SIZE];
   int n = Nova_GetAdjacent(neighbours1[i].tribe_id,tribe_adj,tribe_size,tribe_node,tmp);
   
   Nova_CopyNeighbours2(tmp,tribe_size,neighbours2,i);

   lagrange = neighbours1[i].radius + tribe_node[centre].radius;
       
   orbital_r1 = Nova_Orbit(cfv,corona + 2.5 * lagrange,min_x,min_y,max_x,max_y);
   orbital_r2 = Nova_Orbit(cfv,corona + 4.5 * lagrange,min_x,min_y,max_x,max_y);

   orbital_r1 += 0.2 * orbital_r1 * Nova_SignPerturbation(i);
   orbital_r2 += 0.2 * orbital_r2 * Nova_SignPerturbation(i);
   
   if (n > 2)
      {
      x = orbital_r2 * cos(theta0);
      y = orbital_r2 * sin(theta0);
      }
   else
      {
      x = orbital_r1 * cos(theta0);
      y = orbital_r1 * sin(theta0);
      }

   neighbours1[i].x = x;
   neighbours1[i].y = y;
   neighbours1[i].distance_from_centre = 1;
   neighbours1[i].angle = theta0;
   Nova_MapHorizon(cfv,x,y,&min_x,&min_y,&max_x,&max_y);
   theta0 += dtheta0;
   }

// Secondary orbits

Nova_TribeUnion(trail,neighbours1,tribe_size,size1);

for (i = 0; i < size1; i++)
   {
   struct CfGraphNode tmp2[CF_TRIBE_SIZE];
   size2[i] = Nova_SplayAdjacent(neighbours1[i].tribe_id,tribe_adj,tribe_size,tribe_node,trail,tmp2);

   Nova_CopyNeighbours2(tmp2,tribe_size,neighbours2,i);

   dtheta1 = 2 * pi / (double)(size2[i] + 1);
   theta1 = neighbours1[i].angle - pi + dtheta1;   

   for (j = 0; j < size2[i]; j++)
      {
      struct CfGraphNode tmp3[CF_TRIBE_SIZE];
      size3[i][j] = Nova_SplayAdjacent(neighbours2[i][j].tribe_id,tribe_adj,tribe_size,tribe_node,trail,tmp3);

      Nova_CopyNeighbours3(tmp3,tribe_size,neighbours3,i,j);

      lagrange = neighbours1[i].radius + neighbours2[i][j].radius;   
         
      orbital_r3 = Nova_Orbit(cfv,2.5 * lagrange,min_x,min_y,max_x,max_y);
      orbital_r4 = Nova_Orbit(cfv,4.0 * lagrange,min_x,min_y,max_x,max_y);
      
      if (size3[i][j] > 5)
         {
         orbital_r4 *= 2.0;
         orbital_r4 += 0.2 * orbital_r4;

         x = neighbours1[i].x + orbital_r4 * cos(theta1);
         y = neighbours1[i].y + orbital_r4 * sin(theta1);
         }
      else if (size3[i][j] > 2)
         {
         orbital_r4 += 0.2 * orbital_r4;

         x = neighbours1[i].x + orbital_r4 * cos(theta1);
         y = neighbours1[i].y + orbital_r4 * sin(theta1);
         }
      else
         {
         orbital_r3 += 0.2 * orbital_r3 * Nova_SignPerturbation(j);

         x = neighbours1[i].x + orbital_r3 * cos(theta1);
         y = neighbours1[i].y + orbital_r3 * sin(theta1);               
         }

      Nova_AlignmentCorrection(&x,&y,neighbours1[i].x,neighbours1[i].y);
      Nova_MapHorizon(cfv,x,y,&min_x,&min_y,&max_x,&max_y);

      neighbours2[i][j].x = x;
      neighbours2[i][j].y = y;
      neighbours2[i][j].distance_from_centre = 2;
      neighbours2[i][j].angle = theta1;

      // Tertiary orbits
      
      dtheta2 = 2 * pi / (double)(size3[i][j] + 1);
      theta2 = neighbours2[i][j].angle - pi + dtheta2;   
      
      for (k = 0; k < size3[i][j]; k++)
         {
         lagrange = neighbours2[i][j].radius + neighbours3[i][j][k].radius;

         orbital_r5 = Nova_Orbit(cfv,2.0 * lagrange,min_x,min_y,max_x,max_y);
         
         if (size3[i][j] > 5)
            {
            orbital_r5 *= 2.0;
            }
         
         orbital_r5 += 0.2 * orbital_r5 * Nova_SignPerturbation(j);
                  
         x = neighbours2[i][j].x + orbital_r5 * cos(theta2);
         y = neighbours2[i][j].y + orbital_r5 * sin(theta2);      

         Nova_AlignmentCorrection(&x,&y,neighbours2[i][j].x,neighbours2[i][j].y);
         Nova_MapHorizon(cfv,x,y,&min_x,&min_y,&max_x,&max_y);
      
         neighbours3[i][j][k].x = x;
         neighbours3[i][j][k].y = y;
         neighbours3[i][j][k].distance_from_centre = 3;
         neighbours3[i][j][k].angle = theta2;

         theta2 += dtheta2;
         }      

      theta1 += dtheta1;
      }

   Nova_TribeUnion(trail,tmp2,tribe_size,size2[i]);
   }

/* Do some annealing */

//Nova_Annealing(neighbours1,neighbours2,neighbours3,size1,size2,size3,tribe_size);

/* Now plot everything after corrections for centre of mass */

Nova_CentreScale(&cfv,min_x,max_x,min_y,max_y);

// Centre-piece

for (i = 0; i < size1; i++)
   {
   Nova_Line(cfv,0,0,neighbours1[i].x,neighbours1[i].y,LIGHTRED);

   for (j = 0; j < size2[i]; j++)
      {
      Nova_Line(cfv,neighbours1[i].x,neighbours1[i].y,neighbours2[i][j].x,neighbours2[i][j].y,LIGHTRED);

      for (k = 0; k < size3[i][j]; k++)
         {      
         Nova_AlignmentCorrection(&x,&y,neighbours2[i][j].x,neighbours2[i][j].y);

         Nova_Line(cfv,neighbours2[i][j].x,neighbours2[i][j].y,neighbours3[i][j][k].x,neighbours3[i][j][k].y,LIGHTRED);
         Nova_MapBall(cfv,neighbours3[i][j][k],buffer,bufsize);

         if (neighbours3[i][j][k].real_id == topic)
            {
            Nova_HotBall(cfv,neighbours3[i][j][k].x,neighbours3[i][j][k].y,neighbours3[i][j][k].radius,YELLOWS);
            Nova_BigPrint(cfv,neighbours3[i][j][k].x,neighbours3[i][j][k].y,neighbours3[i][j][k].shortname,RED);         
            }
         else
            {
            Nova_HotBall(cfv,neighbours3[i][j][k].x,neighbours3[i][j][k].y,neighbours3[i][j][k].radius,GREYS);
            Nova_Print(cfv,neighbours3[i][j][k].x,neighbours3[i][j][k].y,neighbours3[i][j][k].shortname,BLACK);
            }
         }      

      // Render 2ndary after

      Nova_MapBall(cfv,neighbours2[i][j],buffer,bufsize);
                  
      if (neighbours2[i][j].real_id == topic)
         {
         Nova_HotBall(cfv,neighbours2[i][j].x,neighbours2[i][j].y,neighbours2[i][j].radius,YELLOWS);
         Nova_BigPrint(cfv,neighbours2[i][j].x,neighbours2[i][j].y,neighbours2[i][j].shortname,RED);
         }
      else
         {
         Nova_HotBall(cfv,neighbours2[i][j].x,neighbours2[i][j].y,neighbours2[i][j].radius,BROWNS);
         Nova_Print(cfv,neighbours2[i][j].x,neighbours2[i][j].y,neighbours2[i][j].shortname,BLACK);     
         }
      }

   Nova_MapBall(cfv,neighbours1[i],buffer,bufsize);
   
   if (neighbours1[i].real_id == topic)
      {
      Nova_HotBall(cfv,neighbours1[i].x,neighbours1[i].y,neighbours1[i].radius,YELLOWS);
      Nova_BigPrint(cfv,neighbours1[i].x,neighbours1[i].y,neighbours1[i].shortname,RED);
      }
   else
      {
      Nova_HotBall(cfv,neighbours1[i].x,neighbours1[i].y,neighbours1[i].radius,PINKS);
      Nova_Print(cfv,neighbours1[i].x,neighbours1[i].y,neighbours1[i].shortname,BLACK);
      }
   }

Nova_MapBall(cfv,tribe_node[centre],buffer,bufsize);

if (size1 == 0)
   {   
   Nova_HotBall(cfv,0,0,tribe_node[centre].radius*8,GREYS);
   Nova_HotBall(cfv,0,0,tribe_node[centre].radius*5,BROWNS);
   Nova_HotBall(cfv,0,0,tribe_node[centre].radius*3,YELLOWS);
   }
else
   {
   Nova_HotBall(cfv,0,0,tribe_node[centre].radius,YELLOWS);
   }

if (tribe_node[centre].real_id == topic)
   {
   Nova_BigPrint(cfv,0,0,tribe_node[centre].shortname,RED);
   }
else
   {
   Nova_Print(cfv,0,0,tribe_node[centre].shortname,BLACK);
   }

// Write the png file

if ((fout = fopen(filename, "wb")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot write %s file\n",filename);
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",filename);
   }

gdImagePng(cfv.im, fout);
fclose(fout);
gdImageDestroy(cfv.im);

// Close image map

Join(buffer,"</map>\n",bufsize);

for (i = 0; i < CF_TRIBE_SIZE; i++)
   {
   if (tribe_node[i].fullname)
      {
      free(tribe_node[i].fullname);
      }
   if (tribe_node[i].shortname)
      {
      free(tribe_node[i].shortname);
      }
   }
}

/*****************************************************************************/

int Nova_GetMaxEvcNode(double *evc,int tribe_size)

{ int i, imax = 0;
  double max = 0;

for (i = 0; i < tribe_size; i++)
   {
   if (evc[i] > max)
      {
      max = evc[i];
      imax = i;
      }
   }

return imax;
}

/*****************************************************************************/

int Nova_SplayAdjacent(int i,double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,struct CfGraphNode *tribe,int *trail,struct CfGraphNode *neighbours)

{ int j,k,l,m,counter = 0;
  int incr,max = 0,min = 999;
  int f[CF_TRIBE_SIZE],s1[CF_TRIBE_SIZE],s2[CF_TRIBE_SIZE];
  int n[CF_TRIBE_SIZE],n1[CF_TRIBE_SIZE],n2[CF_TRIBE_SIZE];
  struct CfGraphNode neighboursd[CF_TRIBE_SIZE];

/* Distribute arm weights evenly over the interval - like pseudo PCA */

if (i < 0)
   {
   return 0;
   }
  
for (j = 0; j < tribe_size; j++)
   {
   s2[j] = -1;
   n2[j] = -1;
   n[j] = -1;
   
   if (Nova_InTrail(trail,j))
      {
      continue;
      }

   if (adj[i][j] > 0 || adj[j][i] > 0)
      {
      f[counter] = Nova_GetAdjacent(j,adj,tribe_size,tribe,neighboursd);

      if (f[counter] > max)
         {
         max = f[counter];
         }

      if (f[counter] < min)
         {
         min = f[counter];
         }

      // n records the correct j for tribe[j]       
      n[counter++] = j;

      Debug(" ...... c=%d, @%d n=%s\n",counter-1,j,tribe[j].shortname);
      }
   }

if (counter == 0)
   {
   return 0;
   }

// s = sort(f) biggest first

m = 0;

for (k = max; k >= min; k--)
   {
   for (l = 0; l < counter; l++)
      {
      if (k == f[l])
         {
         n1[m] = n[l];
         s1[m++] = f[l];
         }            
      }
   }

// s1 is now the list of counters order by highest link frequency
// now sort these to spread load, using clock arithmetic + 1 mod 2
// TODO: need to prove this is a dense 1:1 covering

if (m != counter)
   {
   Debug("ERROR: m=%d, counter=%d\n",m,counter);
   }

k = counter / 2;

for (j = 0; j < counter; j++)
   {
   while(s2[k] != -1)
      {
      k = (k+1) % counter;
      }

   s2[k] = s1[j];
   n2[k] = n1[j];

   k = (k + (counter/2 + 1)) % counter;
   }

// s2 of counter is now the load distrbuted ordering 

// Finally transform j by the same

for (j = 0; j < counter; j++)
   {
   neighbours[j] = tribe[n2[j]];
   neighbours[j].tribe_id = n2[j];
   }

return counter;
}

/*****************************************************************************/

int Nova_GetAdjacent(int i,double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,struct CfGraphNode *tribe, struct CfGraphNode *neighbours)

{ int j,counter = 0;

if (i < 0)
   {
   return 0;
   }
 
for (j = 0; j < tribe_size; j++)
   {
   if (adj[i][j] > 0 || adj[j][i] > 0)
      {
      neighbours[counter] = tribe[j];
      neighbours[counter++].tribe_id = j; 
      }
   }

return counter;
}

/*****************************************************************************/

void Nova_CopyNeighbours2(struct CfGraphNode from[CF_TRIBE_SIZE],int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1)

{ int i;


for (i = 0; i < tribe_size; i++)
   {
   to[index1][i] = from[i];
   }
}

/*****************************************************************************/

void Nova_CopyNeighbours3(struct CfGraphNode *from,int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1,int index2)

{ int i;

for (i = 0; i < tribe_size; i++)
   {
   to[index1][index2][i] = from[i];
   }
}

/*****************************************************************************/

void Nova_AnchorTrail(int *array,int node)
    
{
array[0] = node; 
}

/*****************************************************************************/

void Nova_ClearTrail(int *array)

{ int j;
 
for (j = 0; j < CF_TRIBE_SIZE; j++)
   {
   array[j] = -1;
   }
}

/*****************************************************************************/

int Nova_InTrail(int *trail,int node)

{ int l;
 
for (l = 0; (trail[l] != -1) && (l < CF_TRIBE_SIZE); l++)
   {
   if (node == trail[l])
      {
      return true;  
      }
   }

return false;
}

/*****************************************************************************/

int Nova_TribeUnion(int *array1,struct CfGraphNode *array2, int size1, int size2)

{ int i,j,found,top = 0;

for (i = 0; i < size1; i++)
   {
   if (array1[i] == -1)
      {
      top = i;
      break;
      }
   }
 
for (j = 0; j < size2; j++)
   {
   found = false;
   
   for (i = 0; (i < top) && (i < size1); i++)
      {
      if (array1[i] == array2[j].tribe_id)
         {
         found = true;
         break;
         }
      }

   if (!found)
      {
      array1[top++] = array2[j].tribe_id;
      }
   }

// Return the size of the union
return top;
}

/*****************************************************************************/

int Nova_GetEvcTops(double **adj,int tribe_size,double *evc, int *tops)

{ int i,j,istop,counter = 0;
  int localmax[CF_TRIBE_SIZE];
  double max;

/* Find the local maxima on the EVC surface */
  
for (i = 0; i < tribe_size; i++)
   {
   localmax[i] = true;
   
   for (j = 0; j < tribe_size; j++)
      {
      if ((adj[i][j] != 0) && (i != j))
         {
         if (evc[j] >= evc[i])
            {
            localmax[i] = false;
            }
         }
      }
   }

for (i = 0; i < tribe_size; i++)
   {
   tops[i] = -1; 
   
   if (localmax[i])
      {
      tops[counter++] = i;
      }
   }

return counter;
}

/*****************************************************************************/

double Nova_SignPerturbation(int i)

// Stagger the radial distances of bodies so they fit better into the space.
    
{ int j;
  double x = -1.0;

for (j = 0; j < i; j++)
   {
   x *= -1.0;
   }

return x;
}

/*****************************************************************************/

void Nova_MapHorizon(struct CfDataView cfv,double x,double y,double *min_x,double *min_y,double *max_x,double *max_y)

{
if (x < *min_x)
   {
   *min_x = x - 3.5*CF_MIN_RADIUS;
   }

if (x > *max_x)
   {
   *max_x = x + 3.5*CF_MIN_RADIUS;
   }

if (y < *min_y)
   {
   *min_y = y - 3.5*CF_MIN_RADIUS;
   }

if (y > *max_y)
   {
   *max_y = y + 3.5*CF_MIN_RADIUS;
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_Annealing(struct CfGraphNode neighbours1[CF_TRIBE_SIZE],struct CfGraphNode neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE],struct CfGraphNode neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int size1,int size2[CF_TRIBE_SIZE],int size3[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size)

{ int i1,j1,k1,i2,j2,k2;

for (i1 = 0; i1 < size1; i1++)
   {
   for (j1 = 0; j1 < size2[i1]; j1++)
      {
      for (k1 = 0; k1 < size3[i1][j1]; k1++)
         {
         for (i2 = 0; i2 < size1; i2++)
            {
            for (j2 = 0; j2 < size2[i2]; j2++)
               {
               for (k2 = 0; k2 < size3[i2][j2]; k2++)
                  {
                  if (k1 != k2)
                     {
                     Nova_AlignmentCorrection(&(neighbours3[i1][j1][k1].x),&(neighbours3[i1][j1][k1].y),neighbours3[i2][j2][k2].x,neighbours3[i2][j2][k2].y);

                     Nova_AlignmentCorrection(&(neighbours3[i2][j2][k2].x),&(neighbours3[i2][j2][k2].y),neighbours2[i1][j1].x,neighbours2[i1][j1].y);

                     Nova_AlignmentCorrection(&(neighbours3[i2][j2][k2].x),&(neighbours3[i2][j2][k2].y),neighbours1[i1].x,neighbours1[i1].y);
                     }
                  }
               }
            }
         }
      }
   }
}

/*****************************************************************************/
/* Coords                                                                    */
/*****************************************************************************/

void Nova_CentreScale(struct CfDataView *cfv,double min_x,double max_x,double min_y,double max_y)

{ double av_x = (max_x+min_x)/2;
  double av_y = (max_y+min_y)/2;
     
cfv->scale_x = (max_x-min_x > 0) ? max_x-min_x : (double)cfv->width/2;
cfv->scale_y = (max_y-min_y > 0) ? max_y-min_y : (double)cfv->height/2;

// - because correction
cfv->origin_x = cfv->width/2 - av_x /(CF_CONTAINMENT_FACTOR * cfv->scale_x) * (double)cfv->width;

// + because reversal of y
cfv->origin_y = cfv->height/2 + av_y /(CF_CONTAINMENT_FACTOR * cfv->scale_y) * (double)cfv->height;
}

/*****************************************************************************/

int Nova_X(struct CfDataView cfv,double x)

{
/* Factor of 1.3 to allow for margin and disc radius */
 
return cfv.origin_x + (int)(x /(CF_CONTAINMENT_FACTOR * cfv.scale_x) * (double)cfv.width);
}

/*****************************************************************************/

int Nova_Y(struct CfDataView cfv,double y)

{
/* Factor of 1.3 to allow for margin and disc radius */

return cfv.origin_y - (int)(y/(CF_CONTAINMENT_FACTOR * cfv.scale_y) *(double)cfv.height);
}

/*****************************************************************************/

void Nova_Line(struct CfDataView cfv,double x1,double y1,double x2,double y2,int colour)

{
/* gdImageSetThickness(cfv.im,2); */
gdImageLine(cfv.im,Nova_X(cfv,x1),Nova_Y(cfv,y1),Nova_X(cfv,x2),Nova_Y(cfv,y2),colour);
}

/*****************************************************************************/

void Nova_Disc(struct CfDataView cfv,double x,double y,double radius,int colour)

{
gdImageFilledArc(cfv.im,Nova_X(cfv,x),Nova_Y(cfv,y),radius,radius,0,360,colour,gdArc);
}

/*****************************************************************************/

void Nova_HotBall(struct CfDataView cfv,double x,double y,double radius,int *shade)

{ int dr = 0;
  int col;

for (dr = 0; dr <= radius; dr += radius/(double)CF_SHADES)
   {
   col = (int)(dr/radius * (double)(CF_SHADES-1));
   gdImageArc(cfv.im,Nova_X(cfv,x),Nova_Y(cfv,y),dr,dr,0,360,shade[col]);
   }
}

/*****************************************************************************/

void Nova_ColdBall(struct CfDataView cfv,double x,double y,double radius,int *shade)
 
{ int dr = 0;
  int col;

for (dr = 0; dr <= radius; dr += radius/(double)CF_SHADES)
   {
   col = (int)(dr/radius * (double)(CF_SHADES-1));
   gdImageArc(cfv.im,Nova_X(cfv,x),Nova_Y(cfv,y),radius-dr,radius-dr,0,360,shade[col]);
   }
}

/*****************************************************************************/

void Nova_MapBall(struct CfDataView cfv,struct CfGraphNode n,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 
snprintf(work,CF_BUFSIZE,"<area shape = \"circle\" coords=\"%d,%d,%d\" href=\"%s?pid=%d\" alt=\"topic\" title=\"%s\">\n",
         (int)Nova_X(cfv,n.x),(int)Nova_Y(cfv,n.y),(int)n.radius,CF_KMAP,n.real_id,n.fullname);
Join(buffer,work,bufsize);
}

/*****************************************************************************/

void Nova_Print(struct CfDataView cfv,double x,double y,char *s,int colour)
    
{ char *err,ps[CF_MAXVARSIZE];
  int x1,y1,x2,y2,margin = 1,padding=2,tab=3;
  char font1[CF_MAXVARSIZE] = {0};
  char *font2 = "Vera";
  char *font3 = "DejaVuSans.ttf";
  char *font;
  int brect[8];
  double size = 8.0;

  snprintf(font1,sizeof(font1),"%s/fonts/DejaVuSans.ttf", CFWORKDIR);
  MapName(font1);

if (strlen(s) > CF_NODEVISIBLE)
   {
   strncpy(ps,s,CF_NODEVISIBLE);
   snprintf(ps+CF_NODEVISIBLE,4,"..");
   }
else
   {
   snprintf(ps,CF_MAXVARSIZE,"%s",s);
   }

/* brect
   0	lower left corner, X position
   1	lower left corner, Y position
   2	lower right corner, X position
   3	lower right corner, Y position
   4	upper right corner, X position
   5	upper right corner, Y position
   6	upper left corner, X position
   7	upper left corner, Y position
*/

font = font1;
err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

if (err)
   {
   font = font2;
   err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

   if (err)
      {
      font = font3;
      err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);
      
      if (err)
	{
	CfOut(cf_error, "", "!! Font rendering failure: %s", err);
	}
      }
   }


// Top left = x2,y2
// Top right = x1,y2
// Bottom right = x1,y1

x1 = Nova_X(cfv,x) + (brect[2]-brect[6])/2 + padding;
y1 = Nova_Y(cfv,y) + (brect[3]-brect[7])/2 + padding;
x2 = x1 - (brect[2]-brect[6]) - padding;
y2 = y1 - (brect[3]-brect[7]) - padding;

Nova_BoundaryCheck(&cfv,&x1,&y1,&x2,&y2);

// Plus y is now downward

if (Nova_InRange(cfv,x1,y1) && Nova_InRange(cfv,x2,y2))
   {
   gdImageSetThickness(cfv.im,tab);
   gdImageLine(cfv.im,x1+tab,y2+tab,x1+tab,y1+tab,LIGHTGREY);
   gdImageLine(cfv.im,x2+tab,y1+tab,x1+tab,y1+tab,LIGHTGREY);
   gdImageSetThickness(cfv.im,1);
   gdImageRectangle(cfv.im,x1+margin,y1+margin,x2-margin,y2-margin,LIGHTGREY);
   gdImageFilledRectangle(cfv.im,x1,y1,x2,y2,WHITE);
   gdImageStringFT(cfv.im,&brect[0],BLACK,font,size,0.0,x2+margin+padding,y1-margin-padding,ps);
   }
else
   {
   CfOut(cf_error,""," -> Numerical Overflow in Nova_Print at position (%d,%d)-(%d,%d) while mapping \"%s\"",x1,y1,x2,y2,s);
   }
}

/*****************************************************************************/

void Nova_BigPrint(struct CfDataView cfv,double x,double y,char *s,int colour)
    
{ char *err,ps[CF_MAXVARSIZE];
  int x1,y1,x2,y2,margin = 1,padding=4,tab=3;
  char font1[CF_MAXVARSIZE] = {0};
  char *font2 = "Vera";
  char *font3 = "DejaVuSans.ttf";
  char *font;
  int brect[8];
  double size = 10.0;

  snprintf(font1,sizeof(font1),"%s/fonts/DejaVuSans.ttf", CFWORKDIR);
  MapName(font1);


if (strlen(s) > CF_NODEVISIBLE)
   {
   strncpy(ps,s,CF_NODEVISIBLE);
   snprintf(ps+CF_NODEVISIBLE,4,"..");
   }
else
   {
   snprintf(ps,CF_MAXVARSIZE,"%s",s);
   }

/* brect
   0	lower left corner, X position
   1	lower left corner, Y position
   2	lower right corner, X position
   3	lower right corner, Y position
   4	upper right corner, X position
   5	upper right corner, Y position
   6	upper left corner, X position
   7	upper left corner, Y position
*/

font = font1;
err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

if (err)
   {
   font = font2;
   err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

   if (err)
      {
      font = font3;
      err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);
      
      if (err)
	{
	CfOut(cf_error, "", "!! Font rendering failure: %s", err);
	}
      }
   }

// Top left = x2,y2
// Top right = x1,y2
// Bottom right = x1,y1

x1 = Nova_X(cfv,x) + (brect[2]-brect[6])/2 + padding;
y1 = Nova_Y(cfv,y) + (brect[3]-brect[7])/2 + padding;
x2 = x1 - (brect[2]-brect[6]) - padding;
y2 = y1 - (brect[3]-brect[7]) - padding;

Nova_BoundaryCheck(&cfv,&x1,&y1,&x2,&y2);

// Plus y is now downward

if (Nova_InRange(cfv,x1,y1) && Nova_InRange(cfv,x2,y2))
   {
   gdImageSetThickness(cfv.im,tab);
   gdImageLine(cfv.im,x1+tab,y2+tab,x1+tab,y1+tab,LIGHTGREY);
   gdImageLine(cfv.im,x2+tab,y1+tab,x1+tab,y1+tab,LIGHTGREY);
   gdImageSetThickness(cfv.im,1);
   gdImageRectangle(cfv.im,x1+margin,y1+margin,x2-margin,y2-margin,LIGHTGREY);
   gdImageFilledRectangle(cfv.im,x1,y1,x2,y2,BLACK);
   gdImageStringFT(cfv.im,&brect[0],WHITE,font,size,0.0,x2+padding,y1-padding,ps);
   }
else
   {
   CfOut(cf_error,""," -> Numerical Overflow in Nova_BigPrint at position (%d,%d)-(%d,%d) while mapping \"%s\"",x1,y1,x2,y2,s);
   }
}

/*****************************************************************************/

void Nova_AlignmentCorrection(double *x,double *y,double cx,double cy)

{ /* If nodes are too close, move them apart */

if (!Overlap(*x,*y,cx,cy))
   {
   return;
   }

if (*x > cx)
   {
   *x += 1.5*CF_MIN_RADIUS;
   }
else
   {
   *x -= 1.5*CF_MIN_RADIUS;
   }

if (*y > cy)
   {
   *y = cy + CF_MIN_RADIUS;
   }
else
   {
   *y = cy - CF_MIN_RADIUS;
   }
}
 
/*****************************************************************************/

int Overlap(double x1,double y1,double x2,double y2)

{ double scale_x = gdFontGetLarge()->w * CF_NODEVISIBLE;
 double scale_y = 2*gdFontGetLarge()->h;
 
if (fabs(x1-x2) < scale_x && fabs(y1-y2) < scale_y)
   {
   return true;
   }
else
   {
   return false;
   }
}

/*****************************************************************************/

double Nova_Orbit(struct CfDataView cfv,double radius,double min_x,double max_x,double min_y,double max_y)

{ double min_orbit = (double)(gdFontGetLarge()->w) * (double)CF_NODEVISIBLE / (CF_CONTAINMENT_FACTOR * cfv.scale_x) * (double)cfv.width;

if (radius < min_orbit)
   {
   return min_orbit;
   }
else
   {
   return radius;
   }
}

/*****************************************************************************/

int Nova_InRange(struct CfDataView cfv,int x,int y)
    
{
if (x < 0 || x > cfv.width)
   {
   return false;
   }

if (y < 0 || y > cfv.height)
   {
   return false;
   }

return true;
}

/*****************************************************************************/

void Nova_BoundaryCheck(struct CfDataView *cfv,int *x1,int *y1,int *x2, int *y2)

{ int delta_x,delta_y;
 
// Top left = x2,y2
// Top right = x1,y2
// Bottom right = x1,y1

if (*x1 > cfv->width)
   {
   delta_x = *x1 - cfv->width;
   *x1 -= delta_x;
   *x2 -= delta_x;
   }

if (*x2 < 0)
   {
   *x1 -= *x2;
   *x2 -= *x2;   
   }

if (*y1 > cfv->height)
   {
   delta_y = *y1 - cfv->width;
   *y1 -= delta_y;
   *y2 -= delta_y;
   }

if (*y2 < 0)
   {
   *y1 -= *y2;
   *y2 -= *y2;
   }
}

#else

void Nova_DrawTribe(char *filename,int *tribe_id,struct CfGraphNode *tribe_node,double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,double *tribe_evc,int topic,char *buffer,int bufsize)
{
}
    
#endif
