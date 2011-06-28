
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC

/*****************************************************************************/
/*                                                                           */
/* File: magnify.c                                                           */
/*                                                                           */
/*****************************************************************************/

/**********************************************************************/
/* Magdata                                                            */
/**********************************************************************/

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


#endif  /* HAVE_LIBMONGOC */


