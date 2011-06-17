
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

int Nova_ViewLongHistory(char *keyhash,enum observables obs,char *buffer,int bufsize)
    
{
struct CfDataView cfv;
  
strcpy(buffer,"[");

/* Done initialization */

if (!Nova_ReadLongHistory(&cfv,keyhash,obs))
   {
   Join(buffer,"]",bufsize);
   return false;
   }

Nova_PlotLongHFile(&cfv,buffer,bufsize);
Join(buffer,"]",bufsize);
return true;
}

/**********************************************************************/
/* Yrdata                                                             */
/**********************************************************************/

int Nova_ReadLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs)
/* DEPRECATED */
{ double ry,rq,rs;
  double max,min;
  int i,have_data = false;
  mongo_connection dbconn;
  double q[CF_YEAR_SLOTS] = {0},e[CF_YEAR_SLOTS] = {0},d[CF_YEAR_SLOTS]={0};

  CfOut(cf_error, "", "!! Nova_ReadLongHistory: DEPRECATED");
  return false;
  
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database on read long history");
   return false;
   }

//CFDB_QueryYearView(&dbconn,keyhash,obs,q,e,d);

CFDB_Close(&dbconn);

max = 0;
min = 99999;
cfv->error_scale = 0;      
  
for (i = 0; i < CF_YEAR_SLOTS; i++)
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

int Nova_ReadYearTimeSeries(mongo_connection *conn, struct CfDataView *cfv,char *keyhash,char *monId)

{ double ry,rq,rs;
  double max,min;
  int i,have_data = false;
  double q[CF_YEAR_SLOTS] = {0},e[CF_YEAR_SLOTS] = {0},d[CF_YEAR_SLOTS]={0};

CFDB_QueryMonView(conn,keyhash,monId,mon_rep_yr,q,e,d);

max = 0;
min = 99999;
cfv->error_scale = 0;      
  
for (i = 0; i < CF_YEAR_SLOTS; i++)
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
 
for (i = 0; i < CF_YEAR_SLOTS; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"[%d,%lf,%lf,%lf]",i,cfv->data_q[i],cfv->data_E[i],cfv->bars[i]);

   if (i < CF_YEAR_SLOTS-1)
      {
      strcat(work,",");
      }

   Join(buffer,work,bufsize);
   }
}

/***********************************************************/

void Nova_AnalyseLongHistory(char *keyhash,enum observables obs,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
  struct CfDataView cfv = {0};

*buffer = '\0';
  
if (!Nova_ReadTimeSeries(&cfv,keyhash,obs))
   {
   return;
   }

strcpy(buffer,"[");

snprintf(work,CF_BUFSIZE,"\"Maximum value: %.2lf %s\",",cfv.max,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Minimum value: %.2lf %s\",",cfv.min,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Average variability: %lf %s\"",cfv.error_scale,UNITS[obs]);
Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
}
