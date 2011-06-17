
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

int Nova_ViewHisto(char *keyhash,enum observables obs,char *buffer,int bufsize)
    
{
  struct Item *spectrum;
  struct CfDataView cfv;
  
strcpy(buffer,"[");

if (!Nova_ReadHistogram(&cfv,keyhash,obs))
   {
   Join(buffer,"]",bufsize);
   return false;
   }

spectrum = Nova_MapHistogram(&cfv,keyhash,obs);
Nova_PlotHistogram(&cfv,buffer,bufsize);
DeleteItemList(spectrum);
Join(buffer,"]",bufsize);
return true;
}

/**********************************************************************/

int Nova_ReadHistogram(struct CfDataView *cfv,char *hostkey,enum observables obs)
/* DEPRECATED */
{ double ry;
 int i,have_data = false;
  mongo_connection dbconn;
  double histo[CF_GRAINS];

  CfOut(cf_error, "", "!! Nova_ReadHistogram: DEPRECATED");
  return false;
  
if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

//CFDB_QueryHistogram(&dbconn,hostkey,obs,histo);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   } 

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

void Nova_PlotHistogram(struct CfDataView *cfv,char *buffer,int bufsize)

{ int i;
  char work[CF_MAXVARSIZE];

for (i = 0; i < CF_GRAINS; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"[%d,%lf]",i,cfv->data_E[i]);

   if (i < CF_GRAINS-1)
      {
      strcat(work,",");
      }

   Join(buffer,work,bufsize);
   }
}

/*******************************************************************/

struct Item *Nova_MapHistogram(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char output[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct Item *maxima = NULL;

  /* First find the variance sigma2 */

CfOut(cf_verbose,""," -> Looking for maxima in %s\n",OBS[obs][0]);

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

void Nova_AnalyseHistogram(char *keyhash,enum observables obs,char *buffer,int bufsize)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char work[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct CfDataView cfv = {0};
  struct Item *spectrum;

*buffer = '\0';

if (!Nova_ReadHistogram(&cfv,keyhash,obs))
   {
   return;
   }

spectrum = Nova_MapHistogram(&cfv,keyhash,obs);

strcpy(buffer,"[");

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

snprintf(work,CF_BUFSIZE-1,"Maximum observed %s = %.2lf\",",OBS[obs][0],cfv.max);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"Minimum observed %s = %.2lf\",",OBS[obs][0],cfv.min);
Join(buffer,work,bufsize);

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];

   above_noise = (delta*delta > sigma2) * sensitivity_factor;
   
   if (above_noise)
      {
      new_gradient = delta;

      if (new_gradient < 0 && past_gradient >= 0)
         {
         max++;

         snprintf(work,CF_BUFSIZE-1,"\"%d: Spectral mode with peak at %.0lf/%.0lf grains, ",max,sx-1,(double)CF_GRAINS);
         Join(buffer,work,bufsize);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            snprintf(work,CF_BUFSIZE-1,"red-shifted, e.g. a retardation process where usage is declining. "
                     "If the distribution is skewed, it has a long ramp, indicating "
                     "a possible resource ceiling, a well-utilized system. "
                     "Or there could be outliers of low value, because data are incomplete.\",");
            Join(buffer,work,bufsize);
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            snprintf(work,CF_BUFSIZE-1,"blue-shifted, e.g. an acceleration process where usage is increasing. "
                     "If the distribution is skewed, it has a long tail, indicating "
                    "plenty of resources, or an under-used system. "
                    "Or there could be outliers of low value, because data are incomplete.\",");
            Join(buffer,work,bufsize);            
            }
         }
      }
   
   past_gradient = new_gradient;
   }

Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
DeleteItemList(spectrum);
}

