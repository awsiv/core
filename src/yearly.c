
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: yearly.c                                                           */
/*                                                                           */
/*****************************************************************************/

extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY;
extern int GREYS[CF_SHADES];
extern int BLUES[CF_SHADES];
extern time_t DATESTAMPS[CF_OBSERVABLES];
extern char *UNITS[];

/*****************************************************************************/

int Nova_ViewLongHistory(char *keyhash,enum observables obs,char *buffer,int bufsize)
    
{ int i,y,hint,yr,num = 0,ago,lc;
  double max=0,min=9999,range;
  struct CfDataView cfv;
  
cfv.title = OBS[obs][1];

/* Done initialization */

if (!Nova_ReadLongHistory(&cfv,keyhash,obs))
   {
   return false;
   }

Nova_PlotLongHFile(&cfv,buffer,bufsize);
return true;
}

/**********************************************************************/
/* Yrdata                                                             */
/**********************************************************************/

int Nova_ReadLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double range,rx,ry,rq,rs;
  double max,min;
  int count = 0,i,have_data = false;
  mongo_connection dbconn;
  double q[CF_LHISTORYDATA],e[CF_LHISTORYDATA],d[CF_LHISTORYDATA];

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database on read long history");
   return false;
   }

CFDB_QueryYearView(&dbconn,keyhash,obs,q,e,d);

CFDB_Close(&dbconn);

max = 0;
min = 99999;
cfv->error_scale = 0;      
  
for (i = 0; i < CF_LHISTORYDATA; i++)
   {
   ry = e[i];
   rq = q[i];
   rs = d[i];

   if (rq > 0)
      {
      have_data = true;
      }

   if (ry > max)
      {
      max = ry;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (ry < min)
      {
      min = ry;
      }

   cfv->data_E[i] = ry;
   cfv->data_q[i] = rq;
   cfv->bars[i] = rs;
   }


return have_data;
}

/*******************************************************************/

void Nova_PlotLongHFile(struct CfDataView *cfv,char *buffer,int bufsize)

{ int i;
 char work[CF_MAXVARSIZE];
 
strcpy(buffer,"[");
 
for (i = 0; i < CF_LHISTORYDATA; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"[%d,%lf,%lf,%lf],",i,cfv->data_q[i],cfv->data_E[i],cfv->bars[i]);
   Join(buffer,work,bufsize);
   }

Join(buffer,"]",bufsize);
}

/***********************************************************/

void Nova_AnalyseLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ char work[CF_BUFSIZE];
  struct stat sb;
  int yr,this_lifecycle,this;

  /* First find the variance sigma2 */

snprintf(work,CF_BUFSIZE,"<div id=\"longhistory\">\n");

snprintf(work,CF_BUFSIZE,"<table>\n");
snprintf(work,CF_BUFSIZE,"<tr><td>Maximum value </td><td>%.2lf</td><td>%s</td></tr>\n",cfv->max,UNITS[obs]);
snprintf(work,CF_BUFSIZE,"<tr><td>Minimum value </td><td>%.2lf</td><td>%s</td></tr>\n",cfv->min,UNITS[obs]);
snprintf(work,CF_BUFSIZE,"<tr><td>Average variability </td><td>+/- %lf</td><td>%s</td></tr>\n",cfv->error_scale,UNITS[obs]);
snprintf(work,CF_BUFSIZE,"</table>\n");

snprintf(work,CF_BUFSIZE,"</div></div>\n");
}
