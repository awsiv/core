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

/*****************************************************************************/

void Nova_IlluminateTribe(int *tribe_id,struct CfGraphNode *tribe_node,double **tribe_adj,int tribe_size,double *tribe_evc,char **n,int topic,double **full_adj,int dim_full,int tertiary_boundary)

{ int centre = Nova_GetMaxEvcNode(tribe_evc,tribe_size);
  int i,j;

  for (i = 0; i < tribe_size; i++)
     {
     Debug("(%d) %f - %s\n",i,tribe_evc[i],tribe_node[i].fullname);
     }
  
// Set distances, radii and illumination and horizons
  
// By design, the maximum horizon distance can be 6
  
}

/*****************************************************************************/

void Nova_DrawTribe(char *filename,int *tribe_id,struct CfGraphNode *tribe_node,double **tribe_adj,int tribe_size,double *tribe_evc,char **n,int topic,double **full_adj,int dim_full,int tertiary_boundary)

{ struct CfGraphNode neighbours1[CF_TRIBE_SIZE],neighbours2[CF_TRIBE_SIZE],neighbours3[CF_TRIBE_SIZE];
  int centre = Nova_GetMaxEvcNode(tribe_evc,tribe_size);
  int tribe_tops[CF_TRIBE_SIZE], num_tops,size1,size2,size3,i,j,k;
  double x,y,theta0,dtheta0,theta1,dtheta1,theta2,dtheta2,pi = 3.1416;
  double orbital_r1,orbital_r2,orbital_r3,orbital_r4,orbital_r5;
  int trail[CF_TRIBE_SIZE];
  struct CfDataView cfv;
  char s[CF_MAXVARSIZE];
  FILE *fout;

/* Initialize the tribe*/
  
num_tops = Nova_GetEvcTops(tribe_adj,tribe_size,tribe_evc,tribe_tops);

Nova_ClearTrail(trail);

for (i = 0; i < tribe_size; i++)
   {
   if (i == topic)
      {
      tribe_node[i].radius = 1.5*CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }
   else
      {
      tribe_node[i].radius = CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }
   }

/* Centre on most imporant topic */

Nova_AnchorTrail(trail,centre);

cfv.height = 750;
cfv.width = 1000;
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

// First orbit
size1 = Nova_SplayAdjacent(centre,tribe_adj,tribe_size,tribe_node,trail,neighbours1);

theta0 = 0.64; // this should be close to the adjacent tops
dtheta0 = 2 * pi / (double)size1;

for (i = 0; i < size1; i++)
   {
   int n = Nova_GetAdjacent(neighbours1[i].tribe_id,tribe_adj,tribe_size,tribe_node,neighbours2);

   orbital_r1 = tribe_node[centre].radius + 1.5 * neighbours1[i].radius;
   orbital_r2 = tribe_node[centre].radius + 4.0 * neighbours1[i].radius;

   orbital_r1 += 0.2 * orbital_r1 * Nova_SignPerturbation(i);
   orbital_r2 += 0.2 * orbital_r2 * Nova_SignPerturbation(i);
         
   if (n > 1)
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

   Nova_Line(cfv,0,0,x,y,LIGHTGREY);

   if (neighbours1[i].real_id == topic)
      {
      Nova_HotBall(cfv,x,y,neighbours1[i].radius,YELLOWS);
      Nova_Print(cfv,x,y,neighbours1[i].shortname,RED);
      }
   else
      {
      Nova_HotBall(cfv,x,y,neighbours1[i].radius,PINKS);
      Nova_Print(cfv,x,y,neighbours1[i].shortname,BLACK);
      }
   
   theta0 += dtheta0;
   }

// Centre-piece

Nova_HotBall(cfv,0,0,tribe_node[centre].radius,YELLOWS);
Nova_Print(cfv,0,0,tribe_node[centre].shortname,BLACK);

// Secondary orbits

Nova_TribeUnion(trail,neighbours1,tribe_size,size1);

for (i = 0; i < size1; i++)
   {
   size2 = Nova_SplayAdjacent(neighbours1[i].tribe_id,tribe_adj,tribe_size,tribe_node,trail,neighbours2);
   
   dtheta1 = 2 * pi / (double)(size2 + 1);
   theta1 = neighbours1[i].angle - pi + dtheta1;   
   
   for (j = 0; j < size2; j++)
      {
      size3 = Nova_SplayAdjacent(neighbours2[j].tribe_id,tribe_adj,tribe_size,tribe_node,trail,neighbours3);
      orbital_r3 = 2.5 * neighbours2[j].radius;
      orbital_r4 = 3.5 * neighbours2[j].radius;
      
      orbital_r3 += 0.2 * orbital_r3 * Nova_SignPerturbation(j);
      orbital_r4 += 0.2 * orbital_r4 * Nova_SignPerturbation(j);
      
      if (size3 > 1)
         {
         x = neighbours1[i].x + orbital_r4 * cos(theta1);
         y = neighbours1[i].y + orbital_r4 * sin(theta1);      
         }
      else
         {
         x = neighbours1[i].x + orbital_r3 * cos(theta1);
         y = neighbours1[i].y + orbital_r3 * sin(theta1);               
         }

      neighbours2[j].x = x;
      neighbours2[j].y = y;
      neighbours2[j].distance_from_centre = 2;
      neighbours2[j].angle = theta1;

      Nova_Line(cfv,neighbours1[i].x,neighbours1[i].y,x,y,LIGHTGREY);

      // Tertiary orbits
      
      dtheta2 = 2 * pi / (double)(size3 + 1);
      theta2 = neighbours2[j].angle - pi + dtheta2;   
      
      for (k = 0; k < size3; k++)
         {
         orbital_r5 = 2.0 * CF_MIN_RADIUS;
         orbital_r5 += 0.2 * orbital_r5 * Nova_SignPerturbation(k);
             
         x = neighbours2[j].x + orbital_r5 * cos(theta2);
         y = neighbours2[j].y + orbital_r5 * sin(theta2);      
         
         neighbours3[k].x = x;
         neighbours3[k].y = y;
         neighbours3[k].distance_from_centre = 3;
         neighbours3[k].angle = theta2;

         Nova_Line(cfv,neighbours2[j].x,neighbours2[j].y,x,y,LIGHTGREY);         

         if (neighbours3[k].real_id == topic)
            {
            Nova_HotBall(cfv,x,y,neighbours3[k].radius,YELLOWS);
            Nova_Print(cfv,x,y,neighbours3[k].shortname,RED);         
            }
         else
            {
            Nova_ColdBall(cfv,x,y,neighbours3[k].radius,GREYS);
            Nova_Print(cfv,x,y,neighbours3[k].shortname,BLACK);
            }
         
         theta2 += dtheta2;
         }      

      // Render 2ndary after

      if (neighbours2[j].real_id == topic)
         {
         Nova_HotBall(cfv,neighbours2[j].x,neighbours2[j].y,neighbours2[j].radius,YELLOWS);
         Nova_Print(cfv,neighbours2[j].x,neighbours2[j].y,neighbours2[j].shortname,RED);
         }
      else
         {
         Nova_HotBall(cfv,neighbours2[j].x,neighbours2[j].y,neighbours2[j].radius,BROWNS);
         Nova_Print(cfv,neighbours2[j].x,neighbours2[j].y,neighbours2[j].shortname,BLACK);     
         }
      
      theta1 += dtheta1;
      }

   Nova_TribeUnion(trail,neighbours2,tribe_size,size2);
   }


if ((fout = fopen(filename, "wb")) == NULL)
   {
   CfOut(cf_verbose,"fopen","Cannot write %s file\n",filename);
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",filename);
   }

gdImagePng(cfv.im, fout);
fclose(fout);
gdImageDestroy(cfv.im);
}

/*****************************************************************************/

int Nova_GetMaxEvcNode(double *evc,int tribe_size)

{ int i, imax = -1;
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

int Nova_SplayAdjacent(int i,double **adj,int tribe_size,struct CfGraphNode *tribe,int *trail,struct CfGraphNode *neighbours)

{ int j,k,l,m,counter = 0;
  int incr,max = 0,min = 999;
  int f[CF_TRIBE_SIZE],s1[CF_TRIBE_SIZE],s2[CF_TRIBE_SIZE];
  int n[CF_TRIBE_SIZE],n1[CF_TRIBE_SIZE],n2[CF_TRIBE_SIZE];
  struct CfGraphNode neighboursd[CF_TRIBE_SIZE];

/* Distribute arm weights evenly over the interval */

Debug("\nUNORDER ");

for (j = 0; j < tribe_size; j++)
   {
   Nova_InitVertex(neighbours,j);
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

      Debug(" %d",f[counter]);

      // n records the correct j for tribe[j]       
      n[counter++] = j;

      Debug(" ...... c=%d, @%d n=%s\n",counter-1,j,tribe[j].shortname);

      if (counter > tribe_size)
         {
         Debug("COUNTER out 0f control %s",counter);
         exit(1);
         }
      }
   }

Debug("\ncounter = %d\n",counter);

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

Debug("BETWEEN %d\n",counter);

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

int Nova_GetAdjacent(int i,double **adj,int tribe_size,struct CfGraphNode *tribe, struct CfGraphNode *neighbours)

{ int j,counter = 0;
 
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

{ int j;
  double x = -1.0;

for (j = 0; j < i; j++)
   {
   x *= -1.0;
   }

return x;
}

/*****************************************************************************/
/* Coords                                                                    */
/*****************************************************************************/

int Nova_X(struct CfDataView cfv,double x)

{
return (cfv.width/2) + (int)x;
}

/*****************************************************************************/

int Nova_Y(struct CfDataView cfv,double y)

{
return (cfv.height/2) - (int)y;
}

/*****************************************************************************/

void Nova_Line(struct CfDataView cfv,double x1,double y1,double x2,double y2,int colour)

{
//gdImageSetThickness(cfv.im,2);
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

void Nova_Print(struct CfDataView cfv,double x,double y,char *s,int colour)
    
{ char ps[CF_MAXVARSIZE];

if (strlen(s) > 10)
   {
   strncpy(ps,s,10);
   snprintf(ps+10,4,"..");
   }
else
   {
   snprintf(ps,CF_MAXVARSIZE,"%s",s);
   }

gdImageString(cfv.im,gdFontGetLarge(),Nova_X(cfv,x)-strlen(ps)*gdFontGetLarge()->w/2,Nova_Y(cfv,y)-10,ps,colour);
}
