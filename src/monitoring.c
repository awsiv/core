
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: monitoring.c                                                        */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

struct CfMeasurement
   {
   char *path;
   struct Item *output;
   };

int MONITOR_RESTARTED = true;
char *MEASUREMENTS[CF_DUNBAR_WORK];
struct CfMeasurement NOVA_DATA[CF_DUNBAR_WORK];
static char SLOTS[CF_OBSERVABLES-ob_spare][2][CF_MAXVARSIZE];

/*****************************************************************************/

void NovaInitMeasurements()

{ int i;

for (i = 0; i < CF_DUNBAR_WORK; i++)
   {
   MEASUREMENTS[i] = NULL;
   NOVA_DATA[i].path = NULL;
   NOVA_DATA[i].output = NULL;
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   UNITS[i] = NULL;
   }

UNITS[ob_users] = "average users per 2.5 mins";
UNITS[ob_rootprocs] = "processes";
UNITS[ob_otherprocs] = "processes";
UNITS[ob_diskfree] = "percent";
UNITS[ob_loadavg] = "percent";
UNITS[ob_netbiosns_in] = "connections";
UNITS[ob_netbiosns_out] = "connections";
UNITS[ob_netbiosdgm_in] = "connections";
UNITS[ob_netbiosdgm_out] = "connections";
UNITS[ob_netbiosssn_in] = "connections";
UNITS[ob_netbiosssn_out] = "connections";
UNITS[ob_irc_in] = "connections";
UNITS[ob_irc_out] = "connections";
UNITS[ob_cfengine_in] = "connections";
UNITS[ob_cfengine_out] = "connections";
UNITS[ob_nfsd_in] = "connections";
UNITS[ob_nfsd_out] = "connections";
UNITS[ob_smtp_in] = "connections";
UNITS[ob_smtp_out] = "connections";
UNITS[ob_www_in] = "connections";
UNITS[ob_www_out] = "connections";
UNITS[ob_ftp_in] = "connections";
UNITS[ob_ftp_out] = "connections";
UNITS[ob_ssh_in] = "connections";
UNITS[ob_ssh_out] = "connections";
UNITS[ob_wwws_in] = "connections";
UNITS[ob_wwws_out] = "connections";
UNITS[ob_icmp_in] = "packets";
UNITS[ob_icmp_out] = "packets";
UNITS[ob_udp_in] = "packets";
UNITS[ob_udp_out] = "packets";
UNITS[ob_dns_in] = "packets";
UNITS[ob_dns_out] = "packets";
UNITS[ob_tcpsyn_in] = "packets";
UNITS[ob_tcpsyn_out] = "packets";
UNITS[ob_tcpack_in] = "packets";
UNITS[ob_tcpack_out] = "packets";
UNITS[ob_tcpfin_in] = "packets";
UNITS[ob_tcpfin_out] = "packets";
UNITS[ob_tcpmisc_in] = "packets";
UNITS[ob_tcpmisc_out] = "packets";
UNITS[ob_webaccess] = "packets";
UNITS[ob_weberrors] = "packets";
UNITS[ob_syslog] = "entries";
UNITS[ob_messages] = "entries";
UNITS[ob_temp0] = "Celcius";
UNITS[ob_temp1] = "Celcius";
UNITS[ob_temp2] = "Celcius";
UNITS[ob_temp3] = "Celcius";
UNITS[ob_cpuall] = "percent";
UNITS[ob_cpu0] = "percent";
UNITS[ob_cpu1] = "percent";
UNITS[ob_cpu2] = "percent";
UNITS[ob_cpu3] = "percent";
UNITS[ob_spare] = NULL;

for (i = ob_spare; i < CF_OBSERVABLES; i++)
   {
   UNITS[i] = NULL;
   }

}

/*****************************************************************************/

void Nova_LookupClassName(int n,char *name,char *desc)

{ FILE *fin;
  char filename[CF_BUFSIZE];
  int i;

if (SLOTS[0][0][0] != 0)
   {
   Nova_GetClassName(n,name,desc);
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"%s/state/ts_key",CFWORKDIR);

if ((fin = cf_fopen(filename,"r")) == NULL)
   {
   Nova_GetClassName(n,name,desc);
   return;
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (i < ob_spare)
      {
      fgets(filename,CF_BUFSIZE-1,fin);
      }
   else
      {
      fscanf(fin,"%*d,%[^,],%[^\n]",SLOTS[i-ob_spare][0],SLOTS[i-ob_spare][1]);
      }
   }

cf_fclose(fin);
Nova_GetClassName(n,name,desc);
return;
}

/*****************************************************************************/

void Nova_LookupAggregateClassName(int n,char *name,char *desc)

{ FILE *fin;
  char filename[CF_BUFSIZE];
  int i;

if (SLOTS[0][0][0] != 0)
   {
   Nova_GetClassName(n,name,desc);
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"ts_key");

if ((fin = cf_fopen(filename,"r")) == NULL)
   {
   Nova_GetClassName(n,name,desc);
   return;
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (i < ob_spare)
      {
      fgets(filename,CF_BUFSIZE-1,fin);
      }
   else
      {
      fscanf(fin,"%*d,%[^,],%[^\n]",SLOTS[i-ob_spare][0],SLOTS[i-ob_spare][1]);

      printf("READ %s,%s\n",SLOTS[i-ob_spare][0],SLOTS[i-ob_spare][1]);
      }
   }

cf_fclose(fin);
Nova_GetClassName(n,name,desc);
return;
}

/*****************************************************************************/

void Nova_GetClassName(int i,char *name,char *desc)

{
if (i < ob_spare)
   {
   strncpy(name,OBS[i][0],CF_MAXVARSIZE-1);
   strncpy(desc,OBS[i][1],CF_MAXVARSIZE-1);
   }
else
   {
   if (strlen(SLOTS[i-ob_spare][0]) > 0)
      {
      strncpy(name,SLOTS[i-ob_spare][0],CF_MAXVARSIZE-1);
      strncpy(desc,SLOTS[i-ob_spare][1],CF_MAXVARSIZE-1);
      }
   else
      {
      strncpy(name,OBS[i][0],CF_MAXVARSIZE-1);
      strncpy(desc,OBS[i][1],CF_MAXVARSIZE-1);
      }
   }
}

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

void Nova_VerifyMeasurement(double *this,struct Attributes a,struct Promise *pp)

{ char *handle = (char *)GetConstraint("handle",pp,CF_SCALAR);
  char filename[CF_BUFSIZE];
  struct Item *stream;
  int i,slot,count = 0;
  double new_value;
  FILE *fin,*fout;
  
if (!handle)
   {
   CfOut(cf_error,""," !! The promised measurement has no handle to register it by.");
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Considering promise \"%s\"",handle);
   }

switch (a.measure.data_type)
   {
   case cf_counter:
   case cf_int:
   case cf_real:

       /* First see if we can accommodate this measurement */

       CfOut(cf_verbose,""," -> Promise \"%s\" is numerical in nature",handle);
       
       
       stream = NovaGetMeasurementStream(a,pp);
       
       if (cf_strcmp(a.measure.history_type,"weekly") == 0)
          {                 
          if ((slot = NovaGetSlotHash(handle)) < 0)
             {
             return;
             }
          
          snprintf(SLOTS[slot][0],CF_MAXVARSIZE-1,"%s",handle);
          
          if (pp->ref)
             {
             snprintf(SLOTS[slot][1],CF_MAXVARSIZE-1,"%s",pp->ref);
             }
          else
             {
             snprintf(SLOTS[slot][1],CF_MAXVARSIZE-1,"User defined measure");
             }
          
          this[ob_spare+slot] = NovaExtractValueFromStream(handle,stream,a,pp);
          CfOut(cf_verbose,""," -> Setting Nova slot %d=%s to %lf\n",ob_spare+slot,handle,this[ob_spare+slot]);
          }
       else /* static */
          {
          new_value = NovaExtractValueFromStream(handle,stream,a,pp);
          NovaNamedEvent(handle,new_value,a,pp);
          }
       break;

   default:

       CfOut(cf_verbose,""," -> Promise \"%s\" is symbolic in nature",handle);
       stream = NovaGetMeasurementStream(a,pp);
       NovaLogSymbolicValue(handle,stream,a,pp);
       break;
   }

/* Dump the slot overview */

snprintf(filename,CF_BUFSIZE-1,"%s/state/ts_key",CFWORKDIR);

if ((fout = cf_fopen(filename,"w")) == NULL)
   {
   return;
   }

for (i = 0; i < ob_spare; i++)
   {
   fprintf(fout,"%d,%s,%s\n",i,OBS[i][0],OBS[i][1]);
   }

for (i = 0; i < CF_OBSERVABLES-ob_spare; i++)
   {
   if (strlen(SLOTS[i][0]) > 0)
      {
      fprintf(fout,"%d,%s,%s\n",i+ob_spare,SLOTS[i][0],SLOTS[i][1]);
      }
   else
      {
      fprintf(fout,"%d,spare,unused\n",i+ob_spare);
      }
   }

cf_fclose(fout);
chmod(filename,0600);
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
   
   if ((fp[i] = cf_fopen(filename,"w")) == NULL)
      {
      CfOut(cf_error,"cf_fopen","Could not open %s\n",filename);
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
   cf_fclose(fp[i]);
   } 
}

/*****************************************************************************/

void Nova_SetMeasurementPromises(struct Item **classlist)

{ DB *dbp;
  DBC *dbcp;
  char dbname[CF_MAXVARSIZE],eventname[CF_MAXVARSIZE],assignment[CF_BUFSIZE];
  struct Event entry;
  DBT key,stored;
  struct Scope *ptr;
  struct Rlist *rp;
  int i;

snprintf(dbname,CF_BUFSIZE-1,"%s/state/nova_measures.db",CFWORKDIR);
          
if (!OpenDB(dbname,&dbp))
   {
   return;
   }

if ((errno = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0)
   {
   dbp->err(dbp,errno,"DB->cursor");
   return;
   }

memset(&key, 0, sizeof(key));
memset(&stored, 0, sizeof(stored));

/* Get the database values, if any */
 
while (dbcp->c_get(dbcp,&key,&stored,DB_NEXT) == 0)
   {
   if (stored.data != NULL)
      {
      strcpy(eventname,(char *)key.data);
      memcpy(&entry,stored.data,sizeof(entry));

      CfOut(cf_verbose,""," -> Setting measurement event %s\n",eventname);

      snprintf(assignment,CF_BUFSIZE-1,"value_%s=%.2lf",eventname,entry.Q.q);
      AppendItem(classlist,assignment,NULL);
      snprintf(assignment,CF_BUFSIZE-1,"av_%s=%.2lf",eventname,entry.Q.expect);
      AppendItem(classlist,assignment,NULL);
      snprintf(assignment,CF_BUFSIZE-1,"dev_%s=%.2lf",eventname,sqrt(entry.Q.var));
      AppendItem(classlist,assignment,NULL);
      }
   }


dbp->close(dbp,0);

/* Get the directly discovered environment data from sys context 

for (ptr = VSCOPE; ptr != NULL; ptr=ptr->next)
   {
   if (cf_strcmp(ptr->scope,"mon") != 0)
      {
      continue;
      }
   
   if (ptr->hashtable)
      {
      for (i = 0; i < CF_HASHTABLESIZE; i++)
         {
         if (ptr->hashtable[i] != NULL)
            {
            switch (ptr->hashtable[i]->rtype)
               {
               case CF_SCALAR:

                   snprintf(assignment,CF_BUFSIZE-1,"value_%s=%s",ptr->hashtable[i]->lval,ptr->hashtable[i]->rval);
                   AppendItem(classlist,assignment,NULL);
                   break;

               case CF_LIST:

                   snprintf(assignment,CF_BUFSIZE-1,"value_%s=",ptr->hashtable[i]->lval);
                   
                   for (rp = ptr->hashtable[i]->rval; rp != NULL; rp=rp->next)
                      {
                      strcat(assignment,rp->item);
                      if (rp->next)
                         {
                         strcat(assignment,",");
                         }
                      }
                   
                   AppendItem(classlist,assignment,NULL);
                   break;
               default:
                   break;
               }
            }
         }      
      }
   }
*/
}

/*****************************************************************************/

void Nova_LoadSlowlyVaryingObservations()

{ DB *dbp;
  DBC *dbcp;
  DBT key,stored;
  char name[CF_BUFSIZE];

snprintf(name,CF_BUFSIZE-1,"%s/state/nova_static.db",CFWORKDIR);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if ((errno = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0)
   {
   dbp->err(dbp, errno, "DB->cursor");
   return;
   }

 /* Initialize the key/data return pair. */
 
memset(&key, 0, sizeof(key));
memset(&stored, 0, sizeof(stored));

while (dbcp->c_get(dbcp, &key, &stored, DB_NEXT) == 0)
   {
   char buf[CF_MAXVARSIZE],lval[CF_MAXVARSIZE],rval[CF_BUFSIZE];
   enum cfdatatype type;
   struct Rlist *list = NULL;
   
   strncpy(buf,(char *)key.data,CF_MAXVARSIZE-1);

   sscanf(buf,"%[^:]:%d",lval,&type);
   
   if (stored.data != NULL)
      {
      strncpy(rval,stored.data,CF_BUFSIZE-1);
      
      switch (type)
         {
         case cf_str:
         case cf_int:
         case cf_real:
             NewScalar("mon",lval,rval,type);
             break;
             
         case cf_slist:
             list = SplitStringAsRList(rval,',');
             NewList("mon",lval,list,cf_slist);
             DeleteRlist(list);
             break;
             
         case cf_counter:
             NewScalar("mon",lval,rval,cf_str);
             break;
         }
      }
   }

dbp->close(dbp,0);
}

/*****************************************************************************/

void Nova_DumpSlowlyVaryingObservations()

{ DB *dbp;
  DBC *dbcp;
  DBT key,stored;
  char name[CF_BUFSIZE];
  FILE *fout;
  
snprintf(name,CF_BUFSIZE-1,"%s/state/nova_static.db",CFWORKDIR);

if (!OpenDB(name,&dbp))
   {
   return;
   }

snprintf(name,CF_BUFSIZE-1,"%s/state/static_data",CFWORKDIR);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Unable to save discovery data in %s\n",name);
   return;
   }

/* Acquire a cursor for the database. */

if ((errno = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0)
   {
   dbp->err(dbp, errno, "DB->cursor");
   return;
   }

 /* Initialize the key/data return pair. */
 
memset(&key, 0, sizeof(key));
memset(&stored, 0, sizeof(stored));

while (dbcp->c_get(dbcp, &key, &stored, DB_NEXT) == 0)
   {
   char buf[CF_MAXVARSIZE],lval[CF_MAXVARSIZE],rval[CF_BUFSIZE];
   enum cfdatatype type;
   struct Rlist *list = NULL;
   
   strncpy(buf,(char *)key.data,CF_MAXVARSIZE-1);

   sscanf(buf,"%s:%d",lval,&type);
   
   if (stored.data != NULL)
      {
      strncpy(rval,stored.data,CF_BUFSIZE-1);
      fprintf(fout,"%s:%s\n",lval,rval);
      }
   }

dbp->close(dbp,0);
cf_fclose(fout);
}

/*****************************************************************************/
/* Clock handling                                                            */
/*****************************************************************************/

/* MB: We want to solve the geometric series problem to simulate an unbiased
   average over a grain size for long history aggregation at zero cost, i.e.
   we'd ideally like to have

  w = (1-w)^n for all n

  The true average is expensive to compute, so forget the brute force approach
  because this gives a pretty good result. The eqn above has no actual solution
  but we can approximate numerically to w = 0.01, see this test to show that
  the distribution is flat:

main ()

{ int i,j;
  double w = 0.01,wp;

for (i = 1; i < 20; i++)
   {
   printf("(");
   wp = w;
    
   for (j = 1; j < i; j++)
      {
      printf("%f,",wp);
      wp *= (1- w);
      }
   printf(")\n");
   } 
}
  
*/

double NovaShiftAverage(double new,double old)

{ double av, forgetrate = 0.01;
  double wnew,wold;

wnew = forgetrate;
wold = (1.0-forgetrate);

av = (wnew*new + wold*old)/(wnew+wold); 

if (av < 0)
   {
   av = 0;
   }
  
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
   if (cf_strcmp(SHIFT_TEXT[i],shift) == 0)
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
   if (cf_strncmp(MONTH_TEXT[i],month,strlen(month)) == 0)
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
if (cf_strcmp(d,day) != 0)
   {
   return false;
   }

if (cf_strcmp(m,month) != 0)
   {
   return false;
   }

if (cf_strcmp(s,shift) != 0)
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
   if (cf_strncmp(md[this_month].m,month,strlen(month)) == 0)
      {
      ndays = md[this_month].d;
      break;
      }
   }

if (ndays == 0)
   {
   FatalError("Nova - unrecognizable month");
   }

if ((cf_strcmp(md[this_month].m,"February") == 0) && (year % 4 == 0))
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

/*****************************************************************************/

int NovaGetSlotHash(char *name)

{ int i, slot = 0, count = 0;
  static int hashtablesize = 29; /* must be prime <= CF_DUNBAR_WORK */

for (i = 0; name[i] != '\0'; i++)
   {
   slot = (CF_MACROALPHABET * slot + name[i]) % hashtablesize;
   }
    
for (i = 0; i < CF_DUNBAR_WORK; i++)
   {
   if (MEASUREMENTS[i] != NULL)
      {
      count++;
      }
   }

if (MEASUREMENTS[slot] != NULL)
   {
   if (count >= CF_DUNBAR_WORK)
      {
      CfOut(cf_error,"","Measurement slots are all in use - it is not helpful to measure too much, you can't usefully follow this many variables");
      return -1;
      }
   else if (cf_strcmp(name,MEASUREMENTS[slot]) != 0)
      {
      CfOut(cf_error,"","Measurement slot is already in use");
      return -1;
      }
   }

CfOut(cf_error,""," -> Using slot spare+%d for %s\n",slot,name);

return slot;
}

/*****************************************************************************/

struct Item *NovaGetMeasurementStream(struct Attributes a,struct Promise *pp)
    
{ int i;

for (i = 0; i < CF_DUNBAR_WORK; i++)
   {
   if (NOVA_DATA[i].path == NULL)
      {
      break;
      }
   
   if (cf_strcmp (NOVA_DATA[i].path,pp->promiser) == 0)
      {
      NOVA_DATA[i].output = NovaReSample(i,a,pp);
      return NOVA_DATA[i].output;
      }
   }

NOVA_DATA[i].path = strdup(pp->promiser);
NOVA_DATA[i].output = NovaReSample(i,a,pp);
return NOVA_DATA[i].output;
}

/*****************************************************************************/

struct Item *NovaReSample(int slot,struct Attributes a,struct Promise *pp)

{ struct CfLock thislock;
  char line[CF_BUFSIZE],eventname[CF_BUFSIZE];
  char comm[20], *sp;
  struct timespec start;
  FILE *fin = NULL;
  mode_t maskval = 0;

if (a.measure.stream_type && cf_strcmp(a.measure.stream_type,"pipe") == 0)
   {
   if (!IsExecutable(GetArg0(pp->promiser)))
      {
      cfPS(cf_error,CF_FAIL,"",pp,a,"%s promises to be executable but isn't\n",pp->promiser);
      return NULL;
      }
   else
      {
      CfOut(cf_verbose,""," -> Promiser string contains a valid executable (%s) - ok\n",GetArg0(pp->promiser));
      }
   }

thislock = AcquireLock(pp->promiser,VUQNAME,CFSTARTTIME,a,pp);
   
if (!MONITOR_RESTARTED && thislock.lock == NULL)
   {
   /* If too soon or busy then use cache */
   return NOVA_DATA[slot].output;
   }
else
   {
   DeleteItemList(NOVA_DATA[slot].output);
   NOVA_DATA[slot].output = NULL;
   
   CfOut(cf_inform,""," -> Sampling \'%s\' ...(timeout=%d,owner=%d,group=%d)\n",pp->promiser,a.contain.timeout,a.contain.owner,a.contain.group);
   
   start = BeginMeasure();
   
   CommPrefix(pp->promiser,comm);
   
   if (a.contain.timeout != 0)
      {
      SetTimeOut(a.contain.timeout);
      }
   
   /* Stream types */
   
   if (cf_strcmp(a.measure.stream_type,"file") == 0)
      {
      fin = cf_fopen(pp->promiser,"r");
      }
   else if (cf_strcmp(a.measure.stream_type,"pipe") == 0)
      {
      CfOut(cf_verbose,""," -> (Setting umask to %o)\n",a.contain.umask);
      maskval = umask(a.contain.umask);
      
      if (a.contain.umask == 0)
         {
         CfOut(cf_verbose,""," !! Programming %s running with umask 0! Use umask= to set\n",pp->promiser);
         }

      if (a.contain.useshell)
         {
         fin = cf_popen_shsetuid(pp->promiser,"r",a.contain.owner,a.contain.group,a.contain.chdir,a.contain.chroot);
         }
      else
         {
         fin = cf_popensetuid(pp->promiser,"r",a.contain.owner,a.contain.group,a.contain.chdir,a.contain.chroot);
         }
      }

   /* generic file stream */
   
   if (fin == NULL)
      {
      cfPS(cf_error,CF_FAIL,"cf_popen",pp,a,"Couldn't open pipe to command %s\n",pp->promiser);
      if (!MONITOR_RESTARTED)
         {
         YieldCurrentLock(thislock);
         }

      MONITOR_RESTARTED = false;
      return NOVA_DATA[slot].output;
      }
   
   while (!feof(fin))
      {
      if (ferror(fin))  /* abortable */
         {
         cfPS(cf_error,CF_TIMEX,"ferror",pp,a,"Sample stream %s\n",pp->promiser);
         if (!MONITOR_RESTARTED)
            {
            YieldCurrentLock(thislock);
            }
         return NOVA_DATA[slot].output;
         }
         
      CfReadLine(line,CF_BUFSIZE-1,fin);
      AppendItemList(&(NOVA_DATA[slot].output),line);
      CfOut(cf_inform,"","Sampling => %s",line);
      
      if (ferror(fin))  /* abortable */
         {
         cfPS(cf_error,CF_TIMEX,"ferror",pp,a,"Sample stream %s\n",pp->promiser);
         if (!MONITOR_RESTARTED)
            {
            YieldCurrentLock(thislock);
            }
         return NOVA_DATA[slot].output;
         }      
      }
   
   cf_pclose_def(fin,a,pp);
   }

if (a.contain.timeout != 0)
   {
   alarm(0);
   signal(SIGALRM,SIG_DFL);
   }

cfPS(cf_inform,CF_CHG,"",pp,a," -> Collected sample of %s\n",pp->promiser);
umask(maskval);

if (!MONITOR_RESTARTED)
   {
   YieldCurrentLock(thislock);
   }

MONITOR_RESTARTED = false;

snprintf(eventname,CF_BUFSIZE-1,"Sample(%s)",pp->promiser);
EndMeasure(eventname,start);
return NOVA_DATA[slot].output;
}

/*****************************************************************************/

double NovaExtractValueFromStream(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp)

{ char *line,value[CF_MAXVARSIZE];
  int count = 1, found = false,match_count = 0;
  double real_val = 0;
  struct Item *ip,*match = NULL;
 
for (ip = stream; ip != NULL; ip = ip-> next)
   {
   if (count == a.measure.select_line_number)
      {
      found = true;
      match = ip;      
      match_count++;
      }

   if (FullTextMatch(a.measure.select_line_matching,ip->name))
      {
      found = true;
      match = ip;      
      match_count++;

      switch (a.measure.data_type)
         {
         case cf_int:
             
             strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
             real_val += Str2Double(value);
             break;
             
         case cf_real:
             
             strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
             real_val += Str2Double(value);
             break;
         }
      
      }

   count++;
   }

if (!found)
   {
   cfPS(cf_inform,CF_FAIL,"",pp,a,"Could not locate the line for promise \"%s\"",handle);
   return 0.0;
   }

switch (a.measure.data_type)
   {
   case cf_counter:
       
       real_val = (double)match_count;
       break;

   case cf_int:
       
       if (match_count > 1)
          {
          CfOut(cf_inform,""," !! Warning: %d lines matched the line_selection \"%s\"- making best average",match_count,a.measure.select_line_matching);
          }

       if (match_count > 0)
          {
          real_val /= match_count;
          }
       break;
       
   case cf_real:

       if (match_count > 1)
          {
          CfOut(cf_inform,""," !! Warning: %d lines matched the line_selection \"%s\"- making best average",match_count,a.measure.select_line_matching);
          }

       if (match_count > 0)
          {
          real_val /= match_count;
          }

       break;
   }

if (real_val == CF_NODOUBLE)
   {
   cfPS(cf_inform,CF_FAIL,"",pp,a," !! Unable to extract a value from the matched line \"%\"",match->name);
   PromiseRef(cf_inform,pp);
   real_val == 0.0;
   }

CfOut(cf_inform,"","Extracted value \"%f\" for promise \"%s\"",real_val,handle);
return real_val;
}

/*****************************************************************************/

void NovaLogSymbolicValue(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp)

{ char value[CF_BUFSIZE],sdate[CF_MAXVARSIZE],filename[CF_BUFSIZE],*v;
  int count = 1, found = false,match_count = 0;
  struct Item *ip,*match = NULL,*matches = NULL;
  time_t now = time(NULL);
  FILE *fout;
  struct CfLock thislock;
  struct timespec start;

CfOut(cf_verbose,""," -> Locate and log symbolic sample ...");

for (ip = stream; ip != NULL; ip = ip-> next)
   {
   CfOut(cf_verbose,"","Passing line %d...\n",count);
   
   if (count == a.measure.select_line_number)
      {
      CfOut(cf_verbose,"","Found line %d by number...\n",count);
      found = true;
      match_count = 1;
      match = ip;
      
      CfOut(cf_verbose,""," -> Now looking for a matching extractor \"%s\"",a.measure.extraction_regex);
      CfOut(cf_inform,"","Extracted value \"%s\" for promise \"%s\"",value,handle);
      strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
      AppendItem(&matches,value,NULL);
      break;
      }
   
   if (a.measure.select_line_matching && FullTextMatch(a.measure.select_line_matching,ip->name))
      {
      CfOut(cf_verbose,"","Found line %d by pattern...\n",count);
      found = true;
      match = ip;
      match_count++;

      CfOut(cf_verbose,""," -> Now looking for a matching extractor \"%s\"",a.measure.extraction_regex);
      CfOut(cf_inform,"","Extracted value \"%s\" for promise \"%s\"",value,handle);
      strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
      AppendItem(&matches,value,NULL);
      }

   count++;
   }

if (!found)
   {
   cfPS(cf_inform,CF_FAIL,"",pp,a,"Could not locate the line containing symbolic string for promiser \"%s\"",pp->promiser);
   return;
   }

if (match_count > 1)
   {
   CfOut(cf_inform,""," !! Warning: %d lines matched the line_selection \"%s\"- matching to last",match_count,a.measure.select_line_matching);
   }

switch (a.measure.data_type)    
   {
   case cf_counter:
       snprintf(value,CF_MAXVARSIZE,"%d",match_count);
       break;
       
   case cf_slist:       
       v = ItemList2CSV(matches);
       snprintf(value,CF_BUFSIZE,"%s",v);
       free(v);
       break;

   default:
       snprintf(value,CF_BUFSIZE,"%s",matches->name);
   }

if (cf_strcmp(a.measure.history_type,"log") == 0)  // weekly,scalar,static,log
   {
   snprintf(filename,CF_BUFSIZE,"%s/state/%s_measure.log",CFWORKDIR,handle);

   if ((fout = cf_fopen(filename,"a")) == NULL)
      {
      cfPS(cf_error,CF_FAIL,"",pp,a,"Unable to open the output log \"%s\"",filename);
      PromiseRef(cf_error,pp);
      return;
      }

   strncpy(sdate,ctime(&now),CF_MAXVARSIZE-1);
   Chop(sdate);
   
   fprintf(fout,"%s,%s\n",sdate,value);
   CfOut(cf_verbose,"","Logging: %s,%s to %s\n",sdate,value,filename);
   
   cf_fclose(fout);
   }
else // scalar or static
   {
   DB *dbp;
   char id[CF_MAXVARSIZE];
   
   snprintf(filename,CF_BUFSIZE-1,"%s/state/nova_static.db",CFWORKDIR);
   
   if (!OpenDB(filename,&dbp))
      {
      return;
      }

   snprintf(id,CF_MAXVARSIZE-1,"%s:%d",handle,a.measure.data_type);
   
   WriteDB(dbp,id,value,strlen(value)+1);
   
   dbp->close(dbp,0);
   }
}

/*****************************************************************************/

void NovaNamedEvent(char *eventname,double value,struct Attributes a,struct Promise *pp)

{ char dbname[CF_BUFSIZE];
  struct Event ev_new,ev_old;
  time_t lastseen, now = time(NULL);
  double delta2;
  DB *dbp;
   
snprintf(dbname,CF_BUFSIZE-1,"%s/state/nova_measures.db",CFWORKDIR);
          
if (!OpenDB(dbname,&dbp))
   {
   return;
   }

if (ReadDB(dbp,eventname,&ev_old,sizeof(ev_old)))
   {
   lastseen = now - ev_old.t;
   ev_new.t = now;
   ev_new.Q.q = value;
   ev_new.Q.expect = GAverage(value,ev_old.Q.expect,FORGETRATE);
   delta2 = (value - ev_new.Q.expect)*(value - ev_new.Q.expect);
   ev_new.Q.var = GAverage(delta2,ev_old.Q.var,FORGETRATE);
   }
else
   {
   ev_new.t = now;
   ev_new.Q.q = value;
   ev_new.Q.expect = value;
   ev_new.Q.var = 0.0;
   }

CfOut(cf_verbose,""," -> Wrote scalar named event %s = (%.2lf,%.2lf,%.2lf) -- no time-series slot needed",eventname,ev_new.Q.q,ev_new.Q.expect,sqrt(ev_new.Q.var));
WriteDB(dbp,eventname,&ev_new,sizeof(ev_new));

dbp->close(dbp,0);
}
