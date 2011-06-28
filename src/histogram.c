
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: histogram.c                                                         */
/*                                                                           */
/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

int Nova_ReadHistogram2(mongo_connection *conn, struct CfDataView *cfv,char *hostkey,char *monId)

{ double ry;
 int i,have_data = false;
  double histo[CF_GRAINS];

CFDB_QueryHistogram(conn,hostkey,monId,histo);

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

for (i = 0; i < CF_GRAINS; i++)
   {
   ry = histo[i];

   if (ry > 1)
      {
      have_data = true;
      }
   
   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }

   cfv->data_E[i] = ry;
   }

if (cfv->max == cfv->min)
   {
   have_data = false;
   }

if (cfv->max > CF_MAX_LIMIT)
   {
   cfv->max = CF_MAX_LIMIT;
   }

return have_data;
}

/*******************************************************************/

struct Item *Nova_MapHistogram(struct CfDataView *cfv,char *keyhash)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char output[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct Item *maxima = NULL;

  /* First find the variance sigma2 */

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv->data_E[(int)sx];
   delta = cfv->data_E[(int)sx] - cfv->data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv->data_E[(int)sx];
   delta = cfv->data_E[(int)sx] - cfv->data_E[(int)(sx-1)];

   above_noise = (delta*delta > sigma2) * sensitivity_factor;
   
   if (above_noise)
      {
      new_gradient = delta;

      if (new_gradient < 0 && past_gradient >= 0)
         {
         max++;

         snprintf(output,CF_BUFSIZE-1,"key-%f",sx);
         AppendItem(&maxima,output,NULL);
         SetItemListCounter(maxima,output,sx-1);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            }
         }
      }
   
   past_gradient = new_gradient;
   }
return maxima;
}

/*******************************************************************/


#endif  /* HAVE_LIBMONGOC */

