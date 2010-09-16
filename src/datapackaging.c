
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: datapackaging.c                                                     */
/*                                                                           */
/* Created: Wed Jun 16 14:26:50 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

struct CEnt /* For sorting */
   {
   char name[256];
   time_t date;
   double q;
   double d;
   };

/*****************************************************************************/

void Nova_PackPerformance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  time_t now = time(NULL);
  double ticksperminute = 60.0,average = 0, var = 0;
  char name[CF_BUFSIZE],eventname[CF_BUFSIZE],buffer[CF_BUFSIZE];
  struct Event entry;
  int ret,ksize,vsize,first = true,kept = 0, repaired = 0,not_kept = 0;

CfOut(cf_verbose,""," -> Packing performance data");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_PERFORMANCE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   CfOut(cf_inform,""," !! Unable to open performance database %s",name);
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan performance database");
   return;
   }

 /* Initialize the key/data return pair. */

memset(&key, 0, sizeof(key));
memset(&value, 0, sizeof(value));
memset(&entry, 0, sizeof(entry));

 /* Walk through the database and print out the key/data pairs. */

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   double measure;
   time_t then;
   char tbuf[CF_BUFSIZE],addr[CF_BUFSIZE];

   memcpy(&then,value,sizeof(then));
   strncpy(eventname,(char *)key,ksize);

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));

      then    = entry.t;
      measure = entry.Q.q/ticksperminute;
      average = entry.Q.expect/ticksperminute;
      var     = entry.Q.var;

      // Promise: reply with data only after the "from" time

      if (now - then > (int)(average+2.0*sqrt(var)+0.5))
         {
         not_kept++;
         }
      else if (now - then > (int)(average))
         {
         repaired++;
         }
      else
         {
         kept++;
         }

      if (then < from)
         {
         continue;
         }
          
      snprintf(tbuf,CF_BUFSIZE-1,"%s",cf_ctime(&then));
      tbuf[strlen(tbuf)-9] = '\0';                     /* Chop off second and year*/

      if (PURGE == 'y')
         {
         if (now - then > CF_WEEK)
            {
            DeleteDB(dbp,key);
            }

         CfOut(cf_inform,"","Deleting expired entry for %s\n",eventname);

         if (measure < 0 || average < 0 || measure > 4*CF_WEEK)
            {
            DeleteDB(dbp,key);
            }

         CfOut(cf_inform,""," -> Deleting entry for %s because it seems to take longer than 4 weeks to complete\n",eventname);

         continue;
         }

      // Format: time_t entry.t; char * "name"; double q; double E(q); double sigma

      if (strlen(eventname) == 0 || strlen(eventname) > CF_MAXVARSIZE/2)
         {
         CfOut(cf_inform,""," !! Corrupt entry in database");
         continue;
         }

      if (first)
         {
         first = false;
         AppendItem(reply,header,NULL);
         }
      
      snprintf(buffer,CF_MAXVARSIZE-1,"%ld,%7.4lf,%7.4lf,%7.4lf,%s\n",entry.t,measure,average,sqrt(var)/ticksperminute,eventname);
      AppendItem(reply,buffer,NULL);
      }
   else
      {
      continue;
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);

METER_KEPT[meter_perf_day] = 100.0;
METER_REPAIRED[meter_perf_day] = 0;
}

/*****************************************************************************/

void Nova_PackClasses(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  struct Item *ip;
  double now = (double)time(NULL),average = 0, var = 0;
  char name[CF_BUFSIZE],eventname[CF_BUFSIZE],buffer[CF_MAXVARSIZE];
  struct Event entry;
  struct CEnt array[1024];
  int ret,i,ksize,vsize,first = true;

CfOut(cf_verbose,""," -> Packing class data");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_CLASSUSAGE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

/* Initialize the key/data return pair. */

memset(&entry, 0, sizeof(entry));

 /* Walk through the database and print out the key/data pairs. */

for (i = 0; i < 1024; i++)
   {
   *(array[i].name) = '\0';
   array[i].q = -1;
   }

i = 0;

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   double measure;
   time_t then;

   memcpy(&then,value,sizeof(then));
   strncpy(eventname,(char *)key,CF_BUFSIZE-1);

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));

      then    = entry.t;
      measure = entry.Q.q;
      average = entry.Q.expect;
      var     = entry.Q.var;

      if (now - then > (time_t)CF_WEEK)
         {
         DeleteDB(dbp,key);
         CfOut(cf_inform,""," -> Deleting expired entry for %s\n",eventname);
         continue;
         }

      if (then < from)
         {
         continue;
         }
          
      if (i++ < 1024)
         {
         array[i].date = then;
         strncpy(array[i].name,eventname,254);
         array[i].q = average;
         array[i].d = var;
         }
      else
         {
         break;
         }
      }
   }

// Now check the current heap

for (ip = VHEAP; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"Min",3) == 0 || strncmp(ip->name,"Hr",2) == 0 || strncmp(ip->name,"Q",1) == 0
       || strncmp(ip->name,"Yr",2) == 0 || strncmp(ip->name,"Day",1) == 0 || strncmp(ip->name,"Morning",1) == 0
       || strncmp(ip->name,"Afternoon",1) == 0 || strncmp(ip->name,"Evening",1) == 0 || strncmp(ip->name,"Night",1) == 0)
      {
      continue;
      }
   }

for (i = 0; i < 1024; i++)
   {
   if (array[i].q <= 0.00001)
      {
      continue;
      }

   if (IsItemIn(VHEAP,array[i].name))
      {
      array[i].date = now;
      }

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,%ld,%7.4lf,%7.4lf\n",array[i].name,array[i].date,array[i].q,sqrt(array[i].d));
   AppendItem(reply,buffer,NULL);
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackSetuid(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int first = true;

CfOut(cf_verbose,""," -> Packing setuid data");

snprintf(name,CF_BUFSIZE,"%s/cfagent.%s.log",CFWORKDIR,VSYSNAME.nodename);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

cf_fclose(fin);

file = SortItemListNames(file);

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(start,0,32);
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   /* Promise: Keep a small time-key enabling further compression by delta elimination */

   AppendItem(reply,ip->name,NULL);
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackFileChanges(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],datestr[CF_MAXVARSIZE],size[CF_MAXVARSIZE];
  char no[CF_SMALLBUF],change[CF_BUFSIZE],reformat[CF_BUFSIZE],output[2*CF_BUFSIZE],aggregate[CF_BUFSIZE];
  char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],ref[CF_SMALLBUF],key[CF_SMALLBUF];
  struct Item *ip,*file = NULL;
  char pm,start[32];
  long lthen;
  time_t then, now = time(NULL);
  int i = 0,truncate,first = true,kept = CF_CHANGE_HORIZON,repaired = 0,not_kept = 0;

CfOut(cf_verbose,""," -> Packing file change data");
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,CF_FILECHANGE);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   sscanf(line,"%ld",&lthen);
   then = (time_t)lthen;

   if (now - then < 3600*24)
      {
      repaired++;
      }
   
   if (then < from)
      {
      continue;
      }

   PrependItem(&file,line,NULL);
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(start,0,32);
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   AppendItem(reply,ip->name,NULL);

   if (++i > 12*24*7)
      {
      break;
      }
   }

DeleteItemList(file);
METER_KEPT[meter_other_day] = 100.0*kept/(kept+repaired+not_kept);
METER_REPAIRED[meter_other_day] = 100.0*repaired/(kept+repaired+not_kept);
}

/*****************************************************************************/

void Nova_PackDiffs(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],size[CF_MAXVARSIZE];
  char no[CF_SMALLBUF],change[CF_BUFSIZE],reformat[CF_BUFSIZE],output[2*CF_BUFSIZE],aggregate[CF_BUFSIZE];
  char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],ref[CF_SMALLBUF],key[CF_SMALLBUF];
  struct Item *ip,*file = NULL;
  char pm,start[32];
  int i = 0,truncate,first = true;
  time_t then;
  long lthen;

CfOut(cf_verbose,""," -> Packing diff data");
 
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_DIFF_LOG);
MapName(name);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   if (strncmp(line,"CHANGE",strlen("CHANGE")) != 0)
      {
      continue;
      }

   name[0] = '\0';
   sscanf(line,"CHANGE %[^\n]",name);

   fgets(line,CF_BUFSIZE-1,fin);
   sscanf(line,"%ld;%[^\n]",&lthen,size);

   then = (time_t)lthen;
   
   if (strncmp(line,"END",strlen("END")) == 0)
      {
      continue;
      }

   memset(aggregate,0,CF_BUFSIZE);
   output[0] = '\0';

   truncate = false;

   while (!feof(fin))
      {
      line[0] = '\0';
      fgets(line,CF_BUFSIZE-1,fin);

      if (strncmp(line,"END",strlen("END")) == 0)
         {
         break;
         }

      no[0] = '\0';
      change[0] = '\0';
      sscanf(line,"%c,%[^,],%1024[^\n]",&pm,no,change);

      if (!truncate)
         {
         Chop(line);
         snprintf(reformat,CF_BUFSIZE-1,"%c,%s,%s%c",pm,no,change,CF_N_CODE);

         if (!Join(aggregate,reformat,CF_BUFSIZE))
            {
            }
         }
      }
   
   snprintf(output,CF_BUFSIZE-1,"%ld|%s|%s\n",then,name,aggregate);

   if (from > then)
      {
      continue;
      }

   if (strlen(aggregate) > 0)
      {
      PrependItem(&file,output,NULL);
      aggregate[0] = '\0';
      }
   }

cf_fclose(fin);

for (i = 0,ip = file; ip != NULL; ip = ip->next)
   {
   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,ip->name,NULL);
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackMonitorWeek(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int its,i,j,k, count = 0,err,first = true,slot = 0;
  double kept = 0, not_kept = 0, repaired = 0, nonzero;
  struct stat statbuf;
  struct Averages entry,det;
  char timekey[CF_MAXVARSIZE],filename[CF_MAXVARSIZE],buffer[CF_BUFSIZE];
  time_t now;
  CF_DB *dbp;

CfOut(cf_verbose,""," -> Monitor week data");
  
snprintf(filename,CF_MAXVARSIZE,"%s/state/%s",CFWORKDIR,CF_AVDB_FILE);
MapName(filename);
  
if (!OpenDB(filename,&dbp))
   {
   CfOut(cf_verbose,"","Couldn't open average database %s\n",filename);
   return;
   }

its = 12; // 1 hour coarse graining resolution

now = CF_MONDAY_MORNING;

while (now < CF_MONDAY_MORNING + CF_WEEK)
   {
   memset(&entry,0,sizeof(entry));

   for (j = 0; j < its; j++)
      {
      strcpy(timekey,GenTimeKey(now));

      if (ReadDB(dbp,timekey,&det,sizeof(struct Averages)))
         {
         for (i = 0; i < CF_OBSERVABLES; i++)
            {
            entry.Q[i].expect += det.Q[i].expect/(double)its;
            entry.Q[i].var += det.Q[i].var/(double)its;
            entry.Q[i].q += det.Q[i].q/(double)its;
            }
         }

      now += CF_MEASURE_INTERVAL;
      slot++;
      count++;
      }

   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      nonzero = 0;
      
      if (entry.Q[i].q > entry.Q[i].expect + 2.0*sqrt(entry.Q[i].var))
         {
         not_kept++;
         nonzero++;
         continue;
         }

      if (entry.Q[i].q > entry.Q[i].expect + sqrt(entry.Q[i].var))
         {
         repaired++;
         nonzero++;
         continue;
         }

      if (entry.Q[i].q < entry.Q[i].expect - 2.0*sqrt(entry.Q[i].var))
         {
         not_kept++;
         nonzero++;
         continue;
         }

      if (entry.Q[i].q < entry.Q[i].expect - sqrt(entry.Q[i].var))
         {
         repaired++;
         nonzero++;
         continue;
         }

      kept++;
      }

   /* Promise: only print header if we intend to transmit some data */
   
   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   /* Promise: Keep a small time-key enabling further compression by delta elimination */

   snprintf(buffer,CF_BUFSIZE,"T: %s,%d\n",timekey,slot);
   AppendItem(reply,buffer,NULL);
   
   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      if (entry.Q[i].expect > 0 || entry.Q[i].var > 0 || entry.Q[i].q > 0)
         {
         /* Promise: Keep the integer observable label so that we can eliminate zero entries */
         
         snprintf(buffer,CF_BUFSIZE-1,"%d %.4lf %.4lf %.4lf\n",i,entry.Q[i].q,entry.Q[i].expect,sqrt(entry.Q[i].var));
         AppendItem(reply,buffer,NULL);
         }
      }
   }

// Promise: need to reproduce this for the monitoring, possibly on the far side

METER_KEPT[meter_anomalies_day] = 100.0*kept/(kept+repaired+not_kept);
METER_REPAIRED[meter_anomalies_day] = 100.0*repaired/(kept+repaired+not_kept);

CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackMonitorMag(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int its,i,j,k,err,ok[CF_OBSERVABLES],first = true,slot;
  struct Averages entry,det;
  time_t now,here_and_now;
  double nonzero;
  char timekey[CF_MAXVARSIZE],filename[CF_MAXVARSIZE],buffer[CF_BUFSIZE];
  CF_DB *dbp;

CfOut(cf_verbose,""," -> Packing monitor magnified data");
  
snprintf(filename,CF_MAXVARSIZE,"%s/state/%s",CFWORKDIR,CF_AVDB_FILE);
MapName(filename);
  
if (!OpenDB(filename,&dbp))
   {
   CfOut(cf_verbose,"","Couldn't open average database %s\n",filename);
   return;
   }

now = time(NULL);
here_and_now = now - (time_t)(4 * CF_TICKS_PER_HOUR);

strcpy(timekey,GenTimeKey(here_and_now));
slot = GetTimeSlot(here_and_now);

// if from > here_and_now just send the delta

while (here_and_now < now)
   {
   nonzero = 0;   
   memset(&entry,0,sizeof(entry));

   if (from > here_and_now)
      {
      here_and_now += CF_MEASURE_INTERVAL;
      strcpy(timekey,GenTimeKey(here_and_now));
      slot++;
      continue;
      }
   
   if (ReadDB(dbp,timekey,&det,sizeof(struct Averages)))
      {
      for (i = 0; i < CF_OBSERVABLES; i++)
         {
         entry.Q[i].expect += det.Q[i].expect;
         entry.Q[i].var += det.Q[i].var;
         entry.Q[i].q += det.Q[i].q;
         nonzero += entry.Q[i].expect;
         nonzero += entry.Q[i].var;
         nonzero += entry.Q[i].q;
         }
      }
   
   /* Promise: only print header if we intend to transmit some data */
   
   if (first && nonzero != 0)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   /* Promise: Keep a small time-key enabling further compression by delta elimination */

   snprintf(buffer,CF_BUFSIZE,"T: %d\n",slot);
   AppendItem(reply,buffer,NULL);
   
   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      if (entry.Q[i].expect > 0 || entry.Q[i].var > 0 || entry.Q[i].q > 0)
         {
         /* Promise: Keep the integer observable label so that we can eliminate zero entries */      
         snprintf(buffer,CF_BUFSIZE-1,"%d %.4lf %.4lf %.4lf\n",i,entry.Q[i].q,entry.Q[i].expect, sqrt(entry.Q[i].var));
         AppendItem(reply,buffer,NULL);
         }
      }

   here_and_now += CF_MEASURE_INTERVAL;
   strcpy(timekey,GenTimeKey(here_and_now));
   slot++;
   }

CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackMonitorHist(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int i,j,k,day,position;
  int ok[CF_OBSERVABLES];
  char filename[CF_BUFSIZE],name[CF_MAXVARSIZE];
  char buffer[CF_BUFSIZE],val[CF_SMALLBUF];
  double weekly[CF_OBSERVABLES][CF_GRAINS];
  double histogram[CF_OBSERVABLES][7][CF_GRAINS],smoothhistogram[CF_OBSERVABLES][7][CF_GRAINS];
  FILE *fp;

CfOut(cf_verbose,""," -> Packing histograms");
  
for (i = 0; i < 7; i++)
   {
   for (j = 0; j < CF_OBSERVABLES; j++)
      {
      for (k = 0; k < CF_GRAINS; k++)
         {
         histogram[j][i][k] = 0;
         }
      }
    }

snprintf(filename,CF_BUFSIZE,"%s/state/histograms",CFWORKDIR);

if ((fp = fopen(filename,"r")) == NULL)
   {
   CfOut(cf_verbose,"","Unable to load histogram data\n");
   return;
   }

for (position = 0; position < CF_GRAINS; position++)
   {
   fscanf(fp,"%d ",&position);

   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      for (day = 0; day < 7; day++)
         {
         fscanf(fp,"%lf ",&(histogram[i][day][position]));
         }

      weekly[i][position] = 0;
      }
   }

fclose(fp);

for (k = 1; k < CF_GRAINS-1; k++)
   {
   for (j = 0; j < CF_OBSERVABLES; j++)
      {
      for (i = 0; i < 7; i++)
         {
         smoothhistogram[j][i][k] = (histogram[j][i][k-1] + histogram[j][i][k] + histogram[j][i][k+1])/3.0;
         }
      }
   }

AppendItem(reply,header,NULL);

for (j = 0; j < CF_OBSERVABLES; j++)
   {
   ok[j] = false;
   
   for (k = 0; k < CF_GRAINS; k++)
      {
      for (i = 0; i < 7; i++)
         {
         weekly[j][k] += (smoothhistogram[j][i][k]+0.5);

         if (weekly[j][k] > 0)
            {
            ok[j] = true;
            }
         
         if (weekly[j][k] < 0 || weekly[j][k] > CF_BIGNUMBER)
            {
            weekly[j][k] = 0;
            }
         }
      }
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (ok[i])
      {
      snprintf(buffer,CF_BUFSIZE,"%d:",i);
      
      for (k = 0; k < CF_GRAINS; k++)
         {      
         snprintf(val,CF_SMALLBUF,"%.0lf:",weekly[i][k]);
         strcat(buffer,val);
         }

      AppendItem(reply,buffer,NULL);
      }
   }
}

/*****************************************************************************/

void Nova_PackMonitorYear(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int its,i,j,k, count = 0,err,this_lifecycle,ago, this,first = true,nodate,showtime,slot;
  char timekey[CF_MAXVARSIZE],timekey_now[CF_MAXVARSIZE],buffer[CF_BUFSIZE];
  char d[CF_TIME_SIZE],m[CF_TIME_SIZE],l[CF_TIME_SIZE],s[CF_TIME_SIZE],om[CF_TIME_SIZE];
  char *day = VDAY,*month=VMONTH,*lifecycle=VLIFECYCLE,*shift=VSHIFT;
  double num[CF_OBSERVABLES],qav[CF_OBSERVABLES],varav[CF_OBSERVABLES],eav[CF_OBSERVABLES];
  char filename[CF_BUFSIZE],coarse_cycle[CF_SMALLBUF];
  struct Averages value;
  time_t now;
  CF_DB *dbp;
     
CfOut(cf_verbose,""," -> Packing and compressing monitor 3 year data");

snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_HISTORYDB);
MapName(filename);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

snprintf(timekey_now,CF_MAXVARSIZE-1,"%s_%.3s_%s_%s",day,month,lifecycle,shift);

/* Now we graphs of the past 3 years, in order from 2 years ago to now */

ago = 2;

//NovaOpenNewLifeCycle(ago,fp);
strncpy(l,lifecycle,31);

this_lifecycle = Str2Int(l+strlen("Lcycle_"));
this = (this_lifecycle + 2 - ago) %3;
snprintf(l,CF_TIME_SIZE-1,"Lcycle_%d",this);

strncpy(s,shift,31);
strncpy(d,day,31);
strncpy(m,month,31);
strncpy(om,month,31);

NovaIncrementShift(d,m,l,s);

for (i = 0; i < CF_OBSERVABLES;i++)
   {
   num[i] = 0;
   qav[i] = 0;
   eav[i] = 0;
   varav[i] = 0;
   }

while(true)
   {
   snprintf(timekey,CF_MAXVARSIZE-1,"%s_%s_%s_%s",d,m,l,s);
   snprintf(coarse_cycle,CF_SMALLBUF,"%s_%s",m,l);
   nodate = true;
   
   if (slot = Nova_LifeCycleLater(coarse_cycle,from))
      {
      if (ReadDB(dbp,timekey,&value,sizeof(struct Averages)))
         {
         if (strcmp(m,om) != 0)
            {
            showtime = true;
            
            // Only print header if there are data and we have not already done it
            
            if (first)
               {
               first = false;
               AppendItem(reply,header,NULL);
               }
            
            if (nodate)
               {
               snprintf(buffer,CF_BUFSIZE,"T: %d\n",slot);
               AppendItem(reply,buffer,NULL);
               nodate = false;
               }  
            }
         else
            {
            showtime = false;
            }
         
         for (i = 0; i < CF_OBSERVABLES;i++)
            {
            /* Check for out of bounds data */
            
            if (value.Q[i].q < 0 || value.Q[i].q > CF_BIGNUMBER)
               {
               value.Q[i].q = 0;
               }
            
            if (value.Q[i].var < 0 || value.Q[i].var > CF_BIGNUMBER)
               {
               value.Q[i].var = value.Q[i].q;
               }
            
            if (value.Q[i].expect < 0 || value.Q[i].expect > CF_BIGNUMBER)
               {
               value.Q[i].expect = value.Q[i].q;
               }
            
            if (showtime)
               {
               num[i]++;
               qav[i] += value.Q[i].q;
               eav[i] += value.Q[i].expect;
               varav[i] += value.Q[i].var;
               
               qav[i] /= num[i];
               eav[i] /= num[i];
               varav[i] /= num[i];
               
               if (value.Q[i].q > 0 && value.Q[i].expect > 0 && value.Q[i].var > 0)
                  {
                  snprintf(buffer,CF_BUFSIZE-1,"%d %.2lf %.2lf %.2lf\n",i,qav[i],eav[i],sqrt(varav[i]));
                  AppendItem(reply,buffer,NULL);
                  strcpy(om,m);
                  }
               
               qav[i] = 0;
               eav[i] = 0;
               varav[i] = 0;
               num[i] = 0;
               }
            else
               {
               qav[i] += value.Q[i].q;
               eav[i] += value.Q[i].expect;
               varav[i] += value.Q[i].var;
               num[i]++;
               }         
            }
         }
      
      count++;
      }
   
   NovaIncrementShift(d,m,l,s);

   if (NovaLifeCyclePassesGo(d,m,l,s,day,month,lifecycle,shift))
      {
      if (ago--)
         {
         count = 0;
         }
      else
         {
         break;
         }
      }
   }

CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackCompliance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE];
  double lsea = CF_WEEK * 52; /* expire after a year */
  struct Event entry,e,newe;
  int i = 0,ksize,vsize,first = true;
  CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *stored;
 
CfOut(cf_verbose,""," -> Packing sum compliance data");

/* Open the db */

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   CfOut(cf_verbose, "", "!! Could not open database \"%s\"", name);
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

/* Initialize the key/data return pair. */

while(NextDB(dbp,dbcp,&key,&ksize,&stored,&vsize))
   {
   double measure,av,var;
   time_t then,lastseen,now = time(NULL);
   char tbuf[CF_BUFSIZE],eventname[CF_BUFSIZE];

   name[0] = '\0';
   cf_strcpy(eventname,(char *)key);
   memcpy(&entry,stored,sizeof(entry));

   then    = entry.t;
   measure = entry.Q.q;
   av = entry.Q.expect;
   var = entry.Q.var;
   lastseen = now - then;

   if (then > 0 && lastseen > lsea)
      {
      CfOut(cf_verbose,""," -> Promise usage record \"%s\" expired, removing...\n",eventname);
      DeleteDB(dbp,eventname);
      }
   else
      {
      if (measure == 1.0)
         {
         // Compliant
         snprintf(name,CF_BUFSIZE-1,"%ld,%s,c,%.1lf,%.1lf\n",then,eventname,av*100.0,sqrt(var)*100.0);
         }
      else if (measure == 0.5)
         {
         // Repaired
         snprintf(name,CF_BUFSIZE-1,"%ld,%s,r,%.1lf,%.1lf\n",then,eventname,av*100.0,sqrt(var)*100.0);
         }
      else if (measure == 0.0)
         {
         // Non-compliant
         snprintf(name,CF_BUFSIZE-1,"%ld,%,n,%.1lf,%.1lf\n",then,eventname,av*100.0,sqrt(var)*100.0);
         }

      if (first && strlen(name) > 0)
         {
         first = false;
         AppendItem(reply,header,NULL);
         }

      if (strlen(name) > 0)
         {
         AppendItem(reply,name,NULL);
         }
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackSoftware(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  char buffer[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0,first = true;

CfOut(cf_verbose,""," -> Packing software data");
   
snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_SOFTWARE_INSTALLED);
MapName(name);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   snprintf(buffer,CF_BUFSIZE-1,"%s,%s,%s\n",name,version,Nova_ShortArch(arch));

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,buffer,NULL);
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackAvailPatches(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int i = 0, first = true;
  FILE *fin;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE];
  char buffer[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;

CfOut(cf_verbose,""," -> Packing available patch report...\n");

snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_PATCHES_AVAIL);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   snprintf(buffer,CF_BUFSIZE-1,"%s,%s,%s\n",name,version,Nova_ShortArch(arch));

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,buffer,NULL);
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackPatchStatus(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int i = 0,first = true,count = 0;
  FILE *fin;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE];
  char buffer[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;

CfOut(cf_verbose,""," -> Packing patch installed data");
 
snprintf(name,sizeof(name),"%s/state/%s",CFWORKDIR,NOVA_PATCHES_INSTALLED);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

cf_fclose(fin);


for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   snprintf(buffer,CF_BUFSIZE-1,"%s,%s,%s\n",name,version,Nova_ShortArch(arch));

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,buffer,NULL);
   count++;
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_Pack_promise_output_common(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
CfOut(cf_verbose,""," -> Packing promise data (deprecated)");
// Do we still want this?
}

/*****************************************************************************/

void Nova_PackValueReport(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],name[CF_BUFSIZE];
  CF_DB *dbp;
  CF_DBC *dbcp;
  int ksize,vsize,first = true;
  void *value;
  char *key;
  FILE *fout;
  time_t now = time(NULL);
  struct promise_value pt;
  struct Item *ip,*data = NULL;
  char ref[CF_SMALLBUF];

// Strip out the date resolution so we keep only each day of the year

CfOut(cf_verbose,""," -> Packing value data");
  
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_VALUE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

snprintf(name,CF_SMALLBUF,"%s",cf_ctime(&from));
sscanf(name,"%*s %s %s %*s %s",month,day,year);
snprintf(ref,CF_SMALLBUF-1,"%s %s %s",day,month,year);

if (NewDBCursor(dbp,&dbcp))
   {
   while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
      {
      if (value == NULL)
         {
         continue;
         }

      if (!Nova_CoarseLaterThan(key,ref))
         {
         continue;
         }

      memcpy(&pt,value,sizeof(pt));
      snprintf(name,CF_BUFSIZE,"%s,%.4lf,%.4lf,%.4lf\n",key,pt.kept,pt.repaired,pt.notkept);

      if (first)
         {
         first = false;
         AppendItem(reply,header,NULL);
         }

      AppendItem(reply,name,NULL);
      }
   }

CloseDB(dbp);
DeleteItemList(data);
}

/*****************************************************************************/

void Nova_PackVariables(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ char name[CF_BUFSIZE],line[CF_BUFSIZE],scope[CF_MAXVARSIZE],cache[CF_MAXVARSIZE];
  FILE *fin;
  int first = true;

CfOut(cf_verbose,""," -> Packing variable data");

snprintf(cache,CF_MAXVARSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);
MapName(cache);

snprintf(name,CF_BUFSIZE-1,"%s/state/vars.out",CFWORKDIR);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_verbose,""," -> No variable data yet");
   return;
   }

while (!feof(fin))
   {
   char lval[CF_MAXVARSIZE],type[CF_MAXVARSIZE],rval[CF_MAXVARSIZE];

   lval[0] = '\0';
   rval[0] = '\0';
   type[0] = '\0';   
   line[0] = '\0';
   
   fgets(line,CF_BUFSIZE-1,fin);

   if (strncmp(line,"Scope",strlen("Scope")) == 0)
      {
      // Skip the constant variables

      if (strncmp(line,"Scope const",strlen("Scope const")) == 0)
         {
         while (!feof(fin))
            {
            line[0] = '\0';
            fgets(line,CF_BUFSIZE-1,fin);
            
            if (strncmp(line,"Scope",strlen("Scope")) == 0)
               {
               }
            }

         if (feof(fin))
            {
            break;
            }
         }
      
      if (first)
         {
         first = false;
         AppendItem(reply,header,NULL);
         }

      sscanf(line+strlen("scope "),"%254[^:]",scope);
      snprintf(name,CF_BUFSIZE-1,"S: %s",scope);
      AppendItem(reply,name,NULL);
      continue;                              
      }
   else if (strncmp(line,"<tr><td>",strlen("<tr><td>")) == 0)
      {
      sscanf(line,"<tr><td>%*[^<]</td><th>%[^<]</th><td>%*[^<]</td><td>%512[^<]</td><td>%1023[^<]</td></tr>",type,lval,rval);
      }
   else
      {
      continue;
      }

   if (strcmp(lval,cache) == 0) // Skip the internal cachce
      {
      continue;
      }
   
   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   if (strlen(lval) > 0)
      {
      if (strstr(type,"string"))
         {
         snprintf(name,CF_BUFSIZE,"s,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"slist"))
         {
         snprintf(name,CF_BUFSIZE,"sl,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"int"))
         {
         snprintf(name,CF_BUFSIZE,"i,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"ilist"))
         {
         snprintf(name,CF_BUFSIZE,"il,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"real"))
         {
         snprintf(name,CF_BUFSIZE,"r,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"rlist"))
         {
         snprintf(name,CF_BUFSIZE,"rl,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"(menu option)"))
         {
         snprintf(name,CF_BUFSIZE,"m,%s,%s\n",lval,rval);
         }
      else if (strstr(type,"(option list)"))
         {
         snprintf(name,CF_BUFSIZE,"ml,%s,%s\n",lval,rval);
         }
      else
         {
         snprintf(name,CF_BUFSIZE,"%s,%s,%s\n",type,lval,rval);
         }
      AppendItem(reply,name,NULL);
      }
   }

fclose(fin);
}

/*****************************************************************************/

void Nova_PackLastSeen(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  time_t tid = time(NULL);
  double now = (double)tid,average = 0, var = 0;
  double ticksperhr = (double)CF_TICKS_PER_HOUR;
  char name[CF_BUFSIZE],hostkey[CF_BUFSIZE],buffer[CF_BUFSIZE];
  struct CfKeyHostSeen entry;
  int ret,ksize,vsize,first = true;

CfOut(cf_verbose,""," -> Packing last-seen data");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_LASTDB_FILE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan last-seen database");
   return;
   }

 /* Initialize the key/data return pair. */

memset(&entry, 0, sizeof(entry));

 /* Walk through the database and print out the key/data pairs. */

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   double then;
   time_t fthen;
   char tbuf[CF_BUFSIZE],addr[CF_BUFSIZE];

   strncpy(hostkey,(char *)key,ksize);

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));

      then = entry.Q.q;
      average = (double)entry.Q.expect;
      var = (double)entry.Q.var;
      strncpy(addr,entry.address,CF_MAXVARSIZE);
      }
   else
      {
      continue;
      }

   if (now - then > (double)LASTSEENEXPIREAFTER)
      {
      DeleteDB(dbp,key);
      CfOut(cf_inform,""," -> Deleting expired entry for %s\n",hostkey);
      continue;
      }

   fthen = (time_t)then;                            /* format date */

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   snprintf(buffer,CF_BUFSIZE-1,"%c %s %s %s %ld %.2lf %.2lf %.2lf\n",
           *hostkey,
           hostkey+1,
           IPString2Hostname(addr),
           addr,
           (long)fthen,
           ((double)(now-then))/ticksperhr,
           average/ticksperhr,
           sqrt(var)/ticksperhr);

   AppendItem(reply,buffer,NULL);
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackTotalCompliance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],buffer[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char *sp;
  time_t start,end;
  char version[CF_MAXVARSIZE];
  int kept,repaired,notrepaired;
  int i = 0,today = false,first = true;
  double av_day_kept = 0, av_day_repaired = 0;
  double av_week_kept = 0, av_week_repaired = 0;
  double av_hour_kept = 0, av_hour_repaired = 0;
  char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],key[CF_SMALLBUF],ref[CF_SMALLBUF];
  long t;
  time_t then;
  
CfOut(cf_verbose,""," -> Packing total compliance data");
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_PROMISE_LOG);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

snprintf(ref,CF_SMALLBUF,"%s",cf_ctime(&from));

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   sscanf(line,"%ld",&t);
   then = (time_t)t;

   if (then < from)
      {
      continue;
      }

   PrependItem(&file,line,NULL);
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   kept = repaired = notrepaired = 0;
   memset(version,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   // Complex parsing/extraction

   sscanf(ip->name,"%ld,%ld",&start,&end);
   sscanf(strstr(ip->name,"Outcome of version")+strlen("Outcome of version"),"%31[^:]",version);
   sscanf(strstr(ip->name,"to be kept")+strlen("to be kept"), "%d%*[^0-9]%d%*[^0-9]%d",&kept,&repaired,&notrepaired);

   if (i < 12*24)
      {
      av_day_kept = GAverage((double)kept,av_day_kept,0.5);
      av_day_repaired = GAverage((double)repaired,av_day_repaired,0.5);
      }

   if (i < 12*2)
      {
      av_hour_kept = GAverage((double)kept,av_hour_kept,0.5);
      av_hour_repaired = GAverage((double)repaired,av_hour_repaired,0.5);
      }

   av_week_kept = GAverage((double)kept,av_week_kept,0.1);
   av_week_repaired = GAverage((double)repaired,av_week_repaired,0.1);

   snprintf(buffer,CF_BUFSIZE-1,"%ld,%s,%d,%d,%d\n",start,version,kept,repaired,notrepaired);
   
   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,buffer,NULL);
   
   if (++i > 12*24*7)
      {
      break;
      }
   }

DeleteItemList(file);

METER_KEPT[meter_compliance_week] = av_week_kept;
METER_REPAIRED[meter_compliance_week] = av_week_repaired;
METER_KEPT[meter_compliance_day] = av_day_kept;
METER_REPAIRED[meter_compliance_day] = av_day_repaired;
METER_KEPT[meter_compliance_hour] = av_hour_kept;
METER_REPAIRED[meter_compliance_hour] = av_hour_repaired;
}

/*****************************************************************************/

void Nova_PackRepairLog(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0,first = true;
  long then;
  
CfOut(cf_verbose,""," -> Packing repair data");

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_REPAIR_LOG);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   sscanf(line,"%ld",&then);
   
   if ((time_t)then < from)
      {
      continue;
      }

   PrependItem(&file,line,NULL);
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }
   
   AppendItem(reply,ip->name,NULL);

   if (++i > 12*24*7)
      {
      break;
      }
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackNotKeptLog(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],handle[CF_MAXVARSIZE];
  struct Item *ip,*file = NULL;
  int i = 0,first = true;
  long then;
  char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF];

CfOut(cf_verbose,""," -> Packing promise not-kept data");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_NOTKEPT_LOG);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   
   sscanf(line,"%ld",&then);
   
   if ((time_t)then < from)
      {
      continue;
      }

   PrependItem(&file,line,NULL);
   }

cf_fclose(fin);

for (ip = file; ip != NULL; ip = ip->next)
   {
   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   AppendItem(reply,ip->name,NULL);

   if (++i > 12*24*7)
      {
      break;
      }
   }

DeleteItemList(file);
}

/*****************************************************************************/

void Nova_PackMeter(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ char line[CF_BUFSIZE];

CfOut(cf_verbose,""," -> Packing meter");

AppendItem(reply,header,NULL);

if (METER_KEPT[meter_compliance_week] > 0 || METER_REPAIRED[meter_compliance_week] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"W: %.4lf %.4lf\n",METER_KEPT[meter_compliance_week],METER_REPAIRED[meter_compliance_week]);
   AppendItem(reply,line,NULL);
   }

if (METER_KEPT[meter_compliance_day] > 0 || METER_REPAIRED[meter_compliance_day] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"D: %.4lf %.4lf\n",METER_KEPT[meter_compliance_day],METER_REPAIRED[meter_compliance_day]);
   AppendItem(reply,line,NULL);
   }

if (METER_KEPT[meter_compliance_hour] > 0 && METER_REPAIRED[meter_compliance_hour] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"H: %.4lf %.4lf\n",METER_KEPT[meter_compliance_hour],METER_REPAIRED[meter_compliance_hour]);
   AppendItem(reply,line,NULL);
   }

if (METER_KEPT[meter_perf_day] > 0 || METER_REPAIRED[meter_perf_day] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"P: %.4lf %.4lf\n",METER_KEPT[meter_perf_day],METER_REPAIRED[meter_perf_day]);
   AppendItem(reply,line,NULL);
   }

if (METER_KEPT[meter_other_day] > 0 || METER_REPAIRED[meter_other_day] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"S: %.4lf %.4lf\n",METER_KEPT[meter_other_day],METER_REPAIRED[meter_other_day]);
   AppendItem(reply,line,NULL);
   }

Nova_SummarizeComms();

if (METER_KEPT[meter_comms_hour] > 0 || METER_REPAIRED[meter_comms_hour] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"C: %.4lf %.4lf\n",METER_KEPT[meter_comms_hour],METER_REPAIRED[meter_comms_hour]);
   AppendItem(reply,line,NULL);
   }

if (METER_KEPT[meter_anomalies_day] > 0 || METER_REPAIRED[meter_anomalies_day] > 0)
   {
   snprintf(line,CF_BUFSIZE-1,"A: %.4lf %.4lf\n",METER_KEPT[meter_anomalies_day],METER_REPAIRED[meter_anomalies_day]);
   AppendItem(reply,line,NULL);
   }
}

/*****************************************************************************/

void Nova_PackBundles(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  char bundle[CF_MAXVARSIZE];
  struct Item *ip,*file = NULL;
  int first = true,ksize,vsize;
  time_t tid = time(NULL);
  double now = (double)tid,average = 0, var = 0;
  double ticksperhr = (double)CF_TICKS_PER_HOUR;
  time_t then;
  CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  struct QPoint entry;
 
CfOut(cf_verbose,""," -> Packing bundle log");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_BUNDLE_LOG);
MapName(name);

AppendItem(reply,header,NULL);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan last-seen database");
   return;
   }

 /* Initialize the key/data return pair. */

memset(&entry, 0, sizeof(entry));

 /* Walk through the database and print out the key/data pairs. */

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   double then;
   time_t fthen;
   char tbuf[CF_BUFSIZE],addr[CF_BUFSIZE];

   memcpy(&then,value,sizeof(then));
   strncpy(bundle,(char *)key,ksize);

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));

      then = entry.q;
      average = (double)entry.expect;
      var = (double)entry.var;
      }
   else
      {
      continue;
      }

   if (now - then > (double)LASTSEENEXPIREAFTER)
      {
      DeleteDB(dbp,key);
      CfOut(cf_inform,""," -> Deleting expired entry for %s\n",bundle);
      continue;
      }

   fthen = (time_t)then;                            /* format date */

   if (first)
      {
      first = false;
      AppendItem(reply,header,NULL);
      }

   snprintf(line,CF_BUFSIZE-1,"%s %ld %.2lf %.2lf %.2lf\n",
           bundle,
           (long)fthen,
           ((double)(now-then))/ticksperhr,
           average/ticksperhr,
            sqrt(var)/ticksperhr);

   AppendItem(reply,line,NULL);
   }

DeleteItemList(file);
CloseDB(dbp);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_CoarseLaterThan(char *bigger,char *smaller)

{ char month_small[CF_SMALLBUF];
  char month_big[CF_SMALLBUF];
  int m_small,day_small,year_small,m_big,year_big,day_big;

sscanf(smaller,"%d %s %d",&day_small,month_small,&year_small);
sscanf(bigger,"%d %s %d",&day_big,month_big,&year_big);

if (year_big < year_small)
   {
   return false;
   }

m_small = Month2Int(month_small);
m_big = Month2Int(month_big);

if (m_big < m_small)
   {
   return false;
   }

if (day_big < day_small && m_big == m_small && year_big == year_small)
   {
   return false;
   }

return true;
}

/*****************************************************************************/

int Nova_LaterThan(char *bigger,char *smaller)

{ char month_small[CF_SMALLBUF];
  char month_big[CF_SMALLBUF];
  int m_small,day_small,year_small,m_big,year_big,day_big;
  int min_small,min_big,hour_small,hour_big;

// Format: Fri Mar 27 15:45:52 2009

month_small[0] = '\0';
month_big[0] = '\0';
  
sscanf(smaller,"%*s %s %d %d:%d:%*d %d",month_small,&day_small,&hour_small,&min_small,&year_small);
sscanf(bigger,"%*s %s %d %d:%d:%*d %d",month_big,&day_big,&hour_big,&min_big,&year_big);

if (year_big < year_small)
   {
   return false;
   }

m_small = Month2Int(month_small);
m_big = Month2Int(month_big);

if (m_big < m_small)
   {
   return false;
   }

if (day_big < day_small && m_big == m_small && year_big == year_small)
   {
   return false;
   }

if (hour_big < hour_small && day_big == day_small && m_big == m_small && year_big == year_small)
   {
   return false;
   }

if (min_big < min_small && hour_big == hour_small && day_big == day_small
    && m_big == m_small && year_big == year_small)
   {
   return false;
   }

return true;
}

/*****************************************************************************/

int Nova_LifeCycleLater(char *coarse_cycle,time_t from)

{ char now[CF_MAXVARSIZE],nm[CF_MAXVARSIZE],om[CF_MAXVARSIZE];
  int year,nlc,olc,o_m,n_m;

/* Because cycles are endless, this will work up to a year */
  
snprintf(now,CF_MAXVARSIZE-1,"%s",cf_ctime(&from));

sscanf(now,"%*s %s %*s %*s %d",nm,&year);
nlc = year % 3;

sscanf(coarse_cycle,"%[^_]_%*[^_]_%d",om,&olc);

o_m = Month2Int(om);
n_m = Month2Int(nm);

if ((nlc == olc) && (n_m <= o_m))
   {
   return o_m + 12*olc; // hash slot for this month/year
   }

return false;
}

/*****************************************************************************/

char *Nova_ShortArch(char *arch)
    
{
if (strcmp(arch,"i386") == 0)
   {
   return "3";
   }

if (strcmp(arch,"i486") == 0)
   {
   return "4";
   }

if (strcmp(arch,"i586") == 0)
   {
   return "5";
   }

if (strcmp(arch,"i686") == 0)
   {
   return "6";
   }

if (strcmp(arch,"noarch") == 0)
   {
   return "";
   }

if (strcmp(arch,"x86_64") == 0)
   {
   return "x";
   }

if (strcmp(arch,"default") == 0)
   {
   return "d";
   }

return arch;
}
