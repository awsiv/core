
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
/* DEPRECATED */
{
  struct CfDataView cfv;

  strcpy(buffer, "DEPRECATED");
  return false;
  
strcpy(buffer,"[");

/* Done initialization */

if (!Nova_ReadMagTimeSeries(&cfv,keyhash,obs))
   {
   Join(buffer,"]",bufsize);
   return false;
   }

Nova_PlotMagQFile(&cfv,buffer,bufsize);
Join(buffer,"]",bufsize);
return true;
}

/**********************************************************************/
/* Magdata                                                            */
/**********************************************************************/

int Nova_ReadMagTimeSeries(struct CfDataView *cfv,char *hostkey,enum observables obs)
/* DEPRECATED */
{ double ry,rq,rs;
  double ly = 1,lq = 1,ls = 1;
  double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
  int i,have_data = false;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_error,"","!! Could not open connection to report database to read mag time series");
   return false;
   }

//CFDB_QueryMagView(&dbconn,hostkey,obs,time(NULL) - 4*3600,q,e,d);

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

bool Nova_ReadMagTimeSeries2(mongo_connection *conn, struct CfDataView *cfv,char *hostkey,char *vitalId)

{ double ry,rq,rs;
  double ly = 1,lq = 1,ls = 1;
  double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
  int i,have_data = false;

CFDB_QueryMagView2(conn,hostkey,vitalId,time(NULL) - 4*3600,q,e,d);

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
   return true;
   }
else
   {
   return false;
   }
}

/*******************************************************************/

void Nova_PlotMagQFile(struct CfDataView *cfv,char *buffer,int bufsize)
/* DEPRECATED */
{ char work[CF_MAXVARSIZE];
  int i; 

for (i = 0; i < CF_MAGDATA; i++)
   {
   snprintf(work,CF_MAXVARSIZE," [%d,%lf,%lf,%lf]",i, cfv->data_q[i], cfv->data_E[i],cfv->bars[i]);

   if (i < CF_MAGDATA-1)
      {
      strcat(work,",");
      }
   
   Join(buffer,work,bufsize);
   }
}

/***********************************************************/

void Nova_AnalyseMag(char *keyhash,enum observables obs,char *buffer,int bufsize)
/* TODO: new protocol adjustments? */
{ char work[CF_BUFSIZE];
  struct CfDataView cfv = {0};
  
*buffer = '\0';

if (!Nova_ReadMagTimeSeries(&cfv,keyhash,obs))
   {
   return;
   }

strcpy(buffer,"[");
   
if (cfv.max - cfv.min < cfv.error_scale * 2)
   {
   snprintf(work,CF_BUFSIZE,"\"No significant variations\",");
   Join(buffer,work,bufsize);
   }
else
   {
   snprintf(work,CF_BUFSIZE,"\"Significant variations\",");
   Join(buffer,work,bufsize);
   }

snprintf(work,CF_BUFSIZE,"\"Maximum value_ %lf %s\",",cfv.max,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Minimum value: %lf %s\",",cfv.min,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Average variability: %lf %s\",",cfv.error_scale,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
}

