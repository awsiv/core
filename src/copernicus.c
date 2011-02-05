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

void Nova_DrawTribe(int *tribe_id,struct CfGraphNode *tribe_node,double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,double *tribe_evc,int topic,char *buffer,int bufsize)

{ int centre = Nova_GetMaxEvcNode(tribe_evc,tribe_size);
  int i,j;
  double radius;
  char work[CF_BUFSIZE];

  strcat(buffer,"ZUBBA");
  
for (i = 0; i < tribe_size; i++)
   {
   if (i == topic)
      {
      // The topic might not be central, so inflate its importance a little
      radius = 1.5 * CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }
   else
      {
      radius = CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      }

   /* FORMAT
      {
      "id": "graphnode0",
      "name": "Host Portal",
      "adjacencies": [ "nodeTo": "graphnode1", "nodeFrom": "graphnode3"  ],
      "data": {
      '$color':'#FFCC33',
      '$dim': 30,
      "$type": "gradientCircle"
      }
      },
   */
   
   snprintf(work,bufsize,
            "function init(){ var json = ["
            "{ "
            "\"id\": \"g%d\","
            "\"name\": \"%s\""
            " \"data\": { '$color':'#dddddd', '$dim': %.1lf, \"$type\": \"gradientCircle\""
            "\"adjacencies\": [ ",
            i,tribe_node[i].shortname,radius
            );

   Join(buffer,work,bufsize);

   for (j = 0; j < tribe_size; j++)
      {
      if (tribe_adj[i][j])
         {
         snprintf(work,CF_BUFSIZE,"{\"nodeTo\":\"g%d\"},",j);
         Join(buffer,work,bufsize);
         }
      }
   
   Join(buffer,"]}",bufsize);
   }

Join(buffer,"];",bufsize);

// Cleanup

for (i = 0; i < tribe_size; i++)
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


