
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: monitoring.c                                                        */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_HistoryUpdate(char *timekey,struct Averages newvals)

{ int err_no;
  DBT key,value;
  DB *dbp;
  char filename[CF_BUFSIZE];

  snprintf(filename,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_HISTORYDB);
  
if ((err_no = db_create(&dbp,NULL,0)) != 0)
   {
   CfOut(cf_error,"db_open","Couldn't open history database %s\n",filename);
   return;
   }

#ifdef CF_OLD_DB 
if ((err_no = (dbp->open)(dbp,filename,NULL,DB_BTREE,DB_CREATE,0644)) != 0)
#else
if ((err_no = (dbp->open)(dbp,NULL,filename,NULL,DB_BTREE,DB_CREATE,0644)) != 0)    
#endif
   {
   CfOut(cf_error,"db_open","Couldn't open history database %s\n",filename);
   return;
   }

memset(&key,0,sizeof(key));       
memset(&value,0,sizeof(value));
      
key.data = timekey;
key.size = strlen(timekey)+1;

CfOut(cf_error,"","Storing %s values\n",timekey);

value.data = &newvals;
value.size = sizeof(newvals);
 
if ((err_no = dbp->put(dbp,NULL,&key,&value,0)) != 0)
   {
   dbp->err(dbp,err_no,NULL);
   dbp->close(dbp,0);
   return;
   } 

dbp->close(dbp,0);
}

/*****************************************************************************/

void Nova_UpdateShiftAverage(struct Averages *shift_value,struct Averages *newvals)

{ int i;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   double delta2;

   shift_value->Q[i].q = newvals->Q[i].q;       
   shift_value->Q[i].expect = ShiftAverage(newvals->Q[i].q,shift_value->Q[i].expect);
   delta2 = (newvals->Q[i].q - shift_value->Q[i].expect)*(newvals->Q[i].q - shift_value->Q[i].expect);
   shift_value->Q[i].var = ShiftAverage(delta2,shift_value->Q[i].var);
   } 
}

/*****************************************************************************/

void Nova_ResetShiftAverage(struct Averages *shift_value)

{ int i;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   shift_value->Q[i].q = 0.0;
   shift_value->Q[i].var = 0.0;
   shift_value->Q[i].expect = 0.0;
   }
}

/*****************************************************************************/

void Nova_VerifyMeasurement(struct Attributes a,struct Promise *pp)

{
//Slot numbers

}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

double ShiftAverage(double new,double old)

{ double av, forgetrate = 0.2;
  double wnew,wold;

wnew = forgetrate;
wold = (1.0-forgetrate);

av = (wnew*new + wold*old)/(wnew+wold); 

return av;
}
