
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: magnify.c                                                           */
/*                                                                           */
/*****************************************************************************/

int Nova_ViewMag(char *keyhash,enum observables obs,char *buffer,int bufsize)
    
{ int i,y,hint;
  struct CfDataView cfv;
 
cfv.title = OBS[obs][1];

/* Done initialization */

if (!Nova_ReadMagTimeSeries(&cfv,keyhash,obs))
   {
   return false;
   }

Nova_PlotMagQFile(&cfv,buffer,bufsize);
return true;
}

/**********************************************************************/
/* Magdata                                                            */
/**********************************************************************/

int Nova_ReadMagTimeSeries(struct CfDataView *cfv,char *hostkey,enum observables obs)

{ double range,rx,ry,rq,rs;
  double ly = 1,lq = 1,ls = 1;
  double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
  int i,have_data = false;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_error,"","!! Could not open connection to report database to read mag time series");
   return false;
   }

CFDB_QueryMagView(&dbconn,hostkey,obs,time(NULL) - 4*3600,q,e,d);

CFDB_Close(&dbconn);

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

for (i = 0; i < CF_MAGDATA; i++)
   {
   cfv->data_E[i] = 0;
   cfv->data_q[i] = 0;
   cfv->bars[i] = 0;
   }

for (i = 0; i < CF_MAGDATA; i++)
   {
   ry = Num(e[i]);
   rq = Num(q[i]);
   rs = Num(d[i]);

   // Num() resets to zero negative numbers
   if (q[i] >= 0)
      {
      have_data++;
      }
   
   if (rq > cfv->max)
      {
      cfv->max = rq;
      }

   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   if (rq < cfv->min)
      {
      cfv->min = rq;
      }

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }
   
   ly = cfv->data_E[i] = ry;
   lq = cfv->data_q[i] = rq;
   ls = cfv->bars[i] = rs;
   }

if (have_data > 1)
   {
   return have_data;
   }
else
   {
   return false;
   }
}

/*******************************************************************/

void Nova_PlotMagQFile(struct CfDataView *cfv,char *buffer,int bufsize)

{ char work[CF_MAXVARSIZE];
 int i; 

strcpy(buffer,"[");
 
for (i = 0; i < CF_MAGDATA; i++)
   {
   snprintf(work,CF_MAXVARSIZE," [%d,%lf,%lf,%lf],",i, cfv->data_q[i], cfv->data_E[i],cfv->bars[i]);
   Join(buffer,work,bufsize);
   }

Join(buffer,"]",bufsize);
}

/***********************************************************/

void Nova_AnalyseMag(char *docroot,char *keyhash,enum observables obs,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
  struct CfDataView cfv;
  char newfile[CF_BUFSIZE];
  double y;
  
/* Done initialization */

if (!Nova_ReadMagTimeSeries(&cfv,keyhash,obs))
   {
   return;
   }

*buffer = '\0';

snprintf(work,CF_BUFSIZE,"<div id=\"maganalysis\">\n");
Join(buffer,work,bufsize);
   
if (cfv.max - cfv.min < cfv.error_scale * 2)
   {
   snprintf(work,CF_BUFSIZE,"<p><font color=red>The variations you are seeing are not significant.</p>");
   Join(buffer,work,bufsize);
   }

snprintf(work,CF_BUFSIZE,"<table>\n");
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Maximum value </td><td>%lf</td><td>%s</td></tr>\n",cfv.max,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Minimum value </td><td>%lf</td><td>%s</td></tr>\n",cfv.min,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Average variability </td><td>+/- %lf</td><td>%s</td></tr>\n",cfv.error_scale,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"</table>\n");
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"</div>\n");
Join(buffer,work,bufsize);
}

