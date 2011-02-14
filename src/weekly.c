/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: timeseries.c                                                        */
/*                                                                           */
/*****************************************************************************/

double Num(double x)
{
if (isnan(x) || isinf(x))
   {
   return 0;
   }

if (x > CF_BIGNUMBER)
   {
   return 0;
   }

if (x < 0)
   {
   return 0;
   }

return x;
}

/*****************************************************************************/

int Nova_ViewWeek(char *keyhash,enum observables obs,char *buffer,int bufsize)
    
{ int i,y,hint;
  FILE *fout;
  struct stat sb;
  char newfile[CF_BUFSIZE];
  time_t now = time(NULL);
  struct CfDataView cfv;
  
  /* Initialization */

  strcpy(buffer,"[");

/* Done initialization */

if (!Nova_ReadTimeSeries(&cfv,keyhash,obs))
   {
   Join(buffer,"]",bufsize);
   return false;
   }

Nova_PlotQFile(&cfv,buffer,bufsize);
Join(buffer,"]",bufsize);
return true;
}

/**********************************************************************/

int Nova_ReadTimeSeries(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double rx,ry,rs,rq;
  FILE *fp;
  char name[CF_BUFSIZE];
  int i,have_data = false;
  mongo_connection dbconn;
  double q[CF_TIMESERIESDATA],e[CF_TIMESERIESDATA],d[CF_TIMESERIESDATA];

if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database on reading time series");
   return false;
   }

CFDB_QueryWeekView(&dbconn,keyhash,obs,q,e,d);
CFDB_Close(&dbconn);

cfv->over = 0;
cfv->under = 0;
cfv->over_dev1 = 0;
cfv->under_dev1 = 0;
cfv->over_dev2 = 0;
cfv->under_dev2 = 0;
cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

Q_MEAN = 0;
Q_SIGMA = 0;

for (i = 0; i < CF_TIMESERIESDATA; i++)
   {
   ry = Num(e[i]);
   rs = Num(d[i]);
   rq = Num(q[i]);

   // Num() resets to zero negative numbers
   if (q[i] >= 0)
      {
      have_data++;
      }
   }

if (have_data)
   {
   for (i = 0; i < CF_TIMESERIESDATA; i++)
      {
      ry = Num(e[i]);
      rs = Num(d[i]);
      rq = Num(q[i]);
      
      if (ry > cfv->max)
         {
         cfv->max = ry;
         }
      
      cfv->error_scale = (cfv->error_scale+rs)/2;
      
      if (ry < cfv->min)
         {
         cfv->min = ry;
         }
      
      cfv->data_E[i] = ry;
      cfv->bars[i] = rs;
      cfv->data_q[i] = rq;
      
      if (rq > cfv->max)
         {
         cfv->max = rq;
         }
      
      cfv->error_scale = (cfv->error_scale+rs)/2;
      
      if (rq < cfv->min)
         {
         cfv->min = rq;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i])
         {
         cfv->over++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i])
         {
         cfv->under++;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i]+cfv->bars[i])
         {
         cfv->over_dev1++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i]-cfv->bars[i])
         {
         cfv->under_dev1++;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i]+2*cfv->bars[i])
         {
         cfv->over_dev2++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i]-2*cfv->bars[i])
         {
         cfv->under_dev2++;
         }
      
      if (cfv->data_E[i] != 0)
         {
         Q_MEAN = GAverage(Q_MEAN,cfv->data_E[i],0.5);
         }
      
      if (cfv->bars[i])
         {
         Q_SIGMA = GAverage(Q_SIGMA,cfv->bars[i],0.5);
         }
      }
   
   if (cfv->max > CF_MAX_LIMIT)
      {
      cfv->max = CF_MAX_LIMIT;
      }
   
   }

if (have_data > 10)
   {
   return have_data;
   }
else
   {
   return false;
   }
}

/*******************************************************************/

void Nova_PlotQFile(struct CfDataView *cfv,char *buffer,int bufsize)

{ int i,x,y,lx = 0,ly = 0;
  double rx,ry,rs,sx = 0,s;
  double low,high,av;
  time_t now = time(NULL);
  char work[CF_MAXVARSIZE];

// First plot average

strcpy(buffer,"[ ");
  
for (i = 0; i < CF_TIMESERIESDATA; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"[ %d,%lf,%lf,%lf ],",i,cfv->data_q[i],cfv->data_E[i],cfv->bars[i]);
   Join(buffer,work,bufsize);
   }

Join(buffer,"]",bufsize);
}

/***********************************************************/

void Nova_AnalyseWeek(char *keyhash,enum observables obs,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
  double x;
  struct CfDataView cfv = {0};

*buffer = '\0';

if (!Nova_ReadTimeSeries(&cfv,keyhash,obs))
   {
   return;
   }

strcpy(buffer,"[");

snprintf(work,CF_BUFSIZE-1,"\"Maximum value: %lf %s\",",cfv.max,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"\"Minimum value %lf %s\",",cfv.min,UNITS[obs]);
Join(buffer,work,bufsize);
x = 100*(double)cfv.over/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage over average/normal: %lf\%\",",x);
Join(buffer,work,bufsize);
x = 100*(double)cfv.under/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage under average/normal: %lf\%\",",x);
Join(buffer,work,bufsize);
x = 100*(double)cfv.over_dev1/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage 1 deviation over mean: %lf\%\",",x);
Join(buffer,work,bufsize);
x = 100*(double)cfv.under_dev1/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage 1 deviation under mean: %lf\%\",",x);
Join(buffer,work,bufsize);
x = 100*(double)cfv.over_dev2/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage 2 deviations over mean: %lf\%\",",x);
Join(buffer,work,bufsize);
x = 100*(double)cfv.under_dev2/(double)CF_TIMESERIESDATA;
snprintf(work,CF_BUFSIZE-1,"\"Percentage 2 deviations under mean: %lf\%\"",x);
Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
}
