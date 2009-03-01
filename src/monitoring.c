
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
  
if (!OpenDB(filename,&dbp))
   {
   return;
   }

CfOut(cf_error,"","Storing %s values\n",timekey);

WriteDB(dbp,timekey,&newvals,sizeof(newvals));

dbp->close(dbp,0);
}

/*****************************************************************************/

void Nova_UpdateShiftAverage(struct Averages *shift_value,struct Averages *newvals)

{ int i;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   double delta2;

   shift_value->Q[i].q = newvals->Q[i].q;       
   shift_value->Q[i].expect = NovaShiftAverage(newvals->Q[i].q,shift_value->Q[i].expect);
   delta2 = (newvals->Q[i].q - shift_value->Q[i].expect)*(newvals->Q[i].q - shift_value->Q[i].expect);
   shift_value->Q[i].var = NovaShiftAverage(delta2,shift_value->Q[i].var);
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

 if (strcmp(a.measure.stream_type,"file") == 0)
   {
   }
 else if (strcmp(a.measure.stream_type,"pipe") == 0)
   {
   }
}

/*****************************************************************************/

void Nova_LongHaul(char *day,char *month,char* lifecycle,char *shift)
    
{ int its,i,j,k, count = 0,err,this_lifecycle,ago, this;
  char timekey[CF_MAXVARSIZE],timekey_now[CF_MAXVARSIZE];
  char d[CF_TIME_SIZE],m[CF_TIME_SIZE],l[CF_TIME_SIZE],s[CF_TIME_SIZE];
  char filename[CF_BUFSIZE];
  struct Averages value;
  time_t now;
  FILE *fp[CF_OBSERVABLES];
  DB *dbp;

snprintf(filename,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_HISTORYDB);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

snprintf(timekey_now,CF_MAXVARSIZE-1,"%s_%.3s_%s_%s",day,month,lifecycle,shift);

/* Now we want 3 symmetrical graphs of the past 3 years */

ago = 2;
NovaOpenNewLifeCycle(ago,fp);
strncpy(l,lifecycle,31);

this_lifecycle = Str2Int(l+strlen("Lcycle_"));

this = (this_lifecycle + ago) %3;
snprintf(l,CF_TIME_SIZE-1,"Lcycle_%d",this);

strncpy(s,shift,31);
strncpy(d,day,31);
strncpy(m,month,31);

while (true)
   {
   snprintf(timekey,CF_MAXVARSIZE-1,"%s_%s_%s_%s",d,m,l,s);

   if (ReadDB(dbp,timekey,&value,sizeof(struct Averages)))
      {
      for (i = 0; i < CF_OBSERVABLES;i++)
         {
         fprintf(fp[i],"%d %f %f %f\n",count,value.Q[i].expect, sqrt(value.Q[i].var),value.Q[i].q);
         }

      count++;
      }
   
   NovaIncrementShift(d,m,l,s);

   Debug("TIMEKEY (%s)\n",timekey);
      
   if (NovaLifeCyclePassesGo(d,m,l,s,day,month,lifecycle,shift))
      {
      NovaCloseLifeCycle(ago,fp);

      if (ago--)
         {
         count = 0;
         NovaOpenNewLifeCycle(ago,fp);
         }
      else
         {
         break;
         }          
      }
   }

dbp->close(dbp,0);
}

/*****************************************************************************/

void NovaOpenNewLifeCycle(int age,FILE **fp)
    
{ int i;
  char filename[CF_MAXVARSIZE]; 

Debug("OPEN YEAR %d\n",age);
 
for (i = 0; i < CF_OBSERVABLES; i++)
   {
   sprintf(filename,"%s_%d.yr",OBS[i][0],age);
   
   if ((fp[i] = fopen(filename,"w")) == NULL)
      {
      perror("fopen");
      exit(1);
      }
   }
}

/*****************************************************************************/

void NovaCloseLifeCycle(int age,FILE **fp)
    
{ int i;

Debug("CLOSE YEAR %d\n",age);
 
for (i = 0; i < CF_OBSERVABLES; i++)
   {
   fclose(fp[i]);
   } 
}

/*****************************************************************************/
/* Clock handling                                                            */
/*****************************************************************************/

double NovaShiftAverage(double new,double old)

{ double av, forgetrate = 0.2;
  double wnew,wold;

wnew = forgetrate;
wold = (1.0-forgetrate);

av = (wnew*new + wold*old)/(wnew+wold); 

return av;
}

/*****************************************************************************/

void NovaIncrementShift(char *day,char *month,char* lifecycle,char *shift)

{ int i,new;
  int nday = 0, next_day, nyear = 0, lc = -1;

sscanf(day,"%d",&nday);

if (nday == 0)
   {
   CfOut(cf_error,"","Nova: Could not parse day string \"%s\"",day);
   exit(1);
   }

sscanf(VYEAR,"%d",&nyear);

if (nyear == 0)
   {
   CfOut(cf_error,"","Nova: Could not parse year \"%s\"",nyear);
   exit(1);
   }

sscanf(lifecycle,"Lcycle_%d",&lc);

if (lc == -1)
   {
   CfOut(cf_error,"","Nova: Could not parse lifecycle string \"%s\"",lifecycle);
   exit(1);
   }

/* Start with fastest changing first */

for (i = 0; true; i = (i+1)%4)
   {
   if (strcmp(SHIFT_TEXT[i],shift) == 0)
      {
      new = (i+1)%4;
      
      strncpy(shift,SHIFT_TEXT[new],CF_TIME_SIZE-1);

      if (new >= i)
         {
         return;
         }
      else
         {
         break;
         }
      }
   }

next_day = NovaGetNextDay(nday,month,nyear);

snprintf(day,CF_TIME_SIZE-1,"%d",next_day);
      
if (next_day > nday)
   {
   return;
   }

for (i = 0; true; i = (i+1)%12)
   {
   if (strncmp(MONTH_TEXT[i],month,strlen(month)) == 0)
      {
      new = (i+1)%12;

      snprintf(month,CF_TIME_SIZE,"%.3s",MONTH_TEXT[new]);
      
      if (new >= i)
         {
         return;
         }
      else
         {
         break;
         }
      }
   }

snprintf(lifecycle,CF_TIME_SIZE-1,"Lcycle_%d",(lc+1)%3);
}

/*****************************************************************************/

int NovaLifeCyclePassesGo(char *d,char *m,char *l,char *s,char *day,char *month,char* lifecycle,char *shift)

{ 
if (strcmp(d,day) != 0)
   {
   return false;
   }

if (strcmp(m,month) != 0)
   {
   return false;
   }

if (strcmp(s,shift) != 0)
   {
   return false;
   }

return true;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int NovaGetNextDay(int day,char *month,int year)

{ static struct month_days md[12] =
     {
     "January",31,
     "February",28,
     "March",31,
     "April",30,
     "May",31,
     "June",30,
     "July",31,
     "August",31,
     "September",30,
     "October",31,
     "November",30,
     "December",31
     };
 
 int ndays = 0,this_month;

for (this_month = 0; this_month < 12; this_month++)
   {
   if (strncmp(md[this_month].m,month,strlen(month)) == 0)
      {
      ndays = md[this_month].d;
      break;
      }
   }

if (ndays == 0)
   {
   FatalError("Nova - unrecognizable month");
   }

if ((strcmp(md[this_month].m,"February") == 0) && (year % 4 == 0))
   {
   ndays++; /* leap years */
   }

if (day < ndays)
   {
   return day+1;
   }
else
   {
   return 1;
   }
}
