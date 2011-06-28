
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

#ifdef HAVE_LIBMONGOC

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

/***********************************************************/

void Nova_AnalyseLongHistory(char *keyhash,enum observables obs,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
  struct CfDataView cfv = {0};

*buffer = '\0';
  
if (1)//!Nova_ReadYearTimeSeries(&cfv,keyhash,obs))
   {
   return;
   }

strcpy(buffer,"[");

snprintf(work,CF_BUFSIZE,"\"Maximum value: %.2lf %s\",",cfv.max,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Minimum value: %.2lf %s\",",cfv.min,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"\"Average variability: %lf %s\"",cfv.error_scale,NovaGetSlotUnits(obs));
Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
}


#endif  /* HAVE_LIBMONGOC */


