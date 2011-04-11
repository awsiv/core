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

{ int i,j;
  double radius;
  char work[CF_BUFSIZE],url[CF_MAXVARSIZE];
  char *colour;

  Nova_GetMaxEvcNode(tribe_evc,tribe_size); /* FIXME: unused? */

for (i = 0; i < tribe_size; i++)
   {
   if (i == topic)
      {
      // The topic might not be central, so inflate its importance a little
      radius = 1.5 * CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      colour = "#ffcc33";
      }
   else
      {
      radius = CF_MIN_RADIUS + CF_RADIUS_SCALE*tribe_evc[i];
      switch (tribe_node[i].distance_from_centre)
         {
         case 0:
             colour = "#ffcc33";
             break;
         case 1:
             colour = "#d2bfa5";
             break;
         case 2:
             colour = "#CCB99F";
             break;
         default:
             colour= "#787878";
             break;
         }
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

   snprintf(url,CF_MAXVARSIZE,"/welcome/knowledge/pid/%d",tribe_node[i].real_id);
   
   snprintf(work,CF_BUFSIZE,
            "{ "
            "\"id\": \"g%d\",\n"
            "\"name\": \"%s\",\n"
            " \"data\": "
	              "{ " 
	              "'$color':'%s', "
	              "'$dim': %.1lf, "
	              "\"$type\": \"gradientCircle\","
	              "\"link\": \"%s\""
	              "},\n"
            "\"adjacencies\": [ ",
            i,tribe_node[i].shortname,colour,radius,url
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
   
   Join(buffer,"],\n},\n",bufsize);
   }

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


