
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
#include "probes.h"

/* Structs */

struct CfMeasurement
   {
   char *path;
   struct Item *output;
   };

typedef struct MonitoringSlot
   {
   char *name;
   char *description;
   char *units;
   double expected_minimum;
   double expected_maximum;
   bool consolidable;
   } MonitoringSlot;

/* Constants */

int MONITOR_RESTARTED = true;
char *MEASUREMENTS[CF_DUNBAR_WORK];
struct CfMeasurement NOVA_DATA[CF_DUNBAR_WORK];

static bool slots_loaded;
static MonitoringSlot *SLOTS[CF_OBSERVABLES - ob_spare];

/*****************************************************************************/

void NovaInitMeasurements()

{
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
}

/*****************************************************************************/

static void Nova_FreeSlot(MonitoringSlot *slot)
{
if (slot)
   {
   free(slot->name);
   free(slot->description);
   free(slot->units);
   free(slot);
   }
}

static MonitoringSlot *Nova_MakeSlot(const char *name, const char *description,
                                     const char *units,
                                     double expected_minimum, double expected_maximum,
                                     bool consolidable)
{
MonitoringSlot *slot = malloc(sizeof(MonitoringSlot));
slot->name = strdup(name);
slot->description = strdup(description);
slot->units = strdup(units);
slot->expected_minimum = expected_minimum;
slot->expected_maximum = expected_maximum;
slot->consolidable = consolidable;
return slot;
}

static void Nova_LoadSlots(void)
{
FILE *f;
char filename[CF_BUFSIZE];
int i;

if (slots_loaded)
   {
   return;
   }

slots_loaded = true;

snprintf(filename,CF_BUFSIZE-1,"%s%cstate%cts_key",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR);

if ((f = fopen(filename, "r")) == NULL)
   {
   return;
   }

for (i = 0; i < CF_OBSERVABLES; ++i)
   {
   if (i < ob_spare)
      {
      fscanf(f, "%*[^\n]\n");
      }
   else
      {
      char line[CF_MAXVARSIZE];

      char name[CF_MAXVARSIZE], desc[CF_MAXVARSIZE];
      char units[CF_MAXVARSIZE] = "unknown";
      double expected_min = 0.0;
      double expected_max = 100.0;
      int consolidable = true;

      if (fgets(line, CF_MAXVARSIZE, f) == NULL)
         {
         CfOut(cf_error, "fgets", "Error trying to read ts_key");
         continue;
         }

      int fields = sscanf(line, "%*d,%1023[^,],%1023[^,],%1023[^,],%lf,%lf,%d",
                          name, desc, units, &expected_min, &expected_max, &consolidable);
      if (fields == 2)
         {
         /* Old-style ts_key with name and description */
         }
      else if(fields == 6)
         {
         /* New-style ts_key with additional parameters */
         }
      else
         {
         CfOut(cf_error, "", "Wrong line format in ts_key: %s", line);
         }

      if (strcmp(name, "spare") != 0)
         {
         Nova_FreeSlot(SLOTS[i - ob_spare]);
         SLOTS[i - ob_spare] = Nova_MakeSlot(name, desc, units,
                                             expected_min, expected_max,
                                             consolidable);
         }
      }
   }
fclose(f);
}

/*****************************************************************************/

static void Nova_DumpSlots(void)
{
FILE *fout;
char filename[CF_BUFSIZE];
int i;

snprintf(filename,CF_BUFSIZE-1,"%s%cstate%cts_key",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR);

if ((fout = fopen(filename,"w")) == NULL)
   {
   return;
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (NovaHasSlot(i))
      {
      fprintf(fout, "%d,%s,%s,%s,%.3lf,%.3lf,%d\n",
              i,
              NovaGetSlotName(i),
              NovaGetSlotDescription(i),
              NovaGetSlotUnits(i),
              NovaGetSlotExpectedMinimum(i),
              NovaGetSlotExpectedMaximum(i),
              NovaIsSlotConsolidable(i) ? 1 : 0);
      }
   else
      {
      fprintf(fout, "%d,spare,unused\n", i);
      }
   }

fclose(fout);
chmod(filename,0600);
}

/*****************************************************************************/

void Nova_LookupClassName(int n,char *name,char *desc)

{
Nova_LoadSlots();
Nova_GetClassName(n, name, desc);
}

/*****************************************************************************/

void Nova_LookupAggregateClassName(int n,char *name,char *desc)

{
Nova_LoadSlots();
Nova_GetClassName(n,name,desc);
}

/*****************************************************************************/

void Nova_GetClassName(int i,char *name,char *desc)

{
Nova_LoadSlots();

if (i < ob_spare)
   {
   strncpy(name,OBS[i][0],CF_MAXVARSIZE-1);
   strncpy(desc,OBS[i][1],CF_MAXVARSIZE-1);
   }
else
   {
   if (SLOTS[i - ob_spare])
      {
      strncpy(name,SLOTS[i-ob_spare]->name,CF_MAXVARSIZE-1);
      strncpy(desc,SLOTS[i-ob_spare]->description,CF_MAXVARSIZE-1);
      }
   else
      {
      strncpy(name,OBS[i][0],CF_MAXVARSIZE-1);
      strncpy(desc,OBS[i][1],CF_MAXVARSIZE-1);
      }
   }
}

/*****************************************************************************/

void Nova_HistoryUpdate(time_t time, const struct Averages *newvals)
{
CF_DB *dbp;
char filename[CF_BUFSIZE];

if (LICENSES == 0)
   {
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_HISTORYDB);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

//CfOut(cf_verbose,"","Storing %s values\n",);

PutRecordForTime(dbp, time, newvals);

CloseDB(dbp);
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
  struct Item *stream = NULL;
  int slot = 0;
  double new_value;

if (!handle)
   {
   CfOut(cf_error,""," !! The promised measurement has no handle to register it by.");
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Considering promise \"%s\"",handle);
   }

Nova_LoadSlots();

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
          if ((slot = NovaRegisterSlot(handle, pp->ref ? pp->ref : "User defined measure",
                                       a.measure.units ? a.measure.units : "unknown",
                                       0.0f, 100.0f, true)) < 0)
             {
             return;
             }

          this[slot] = NovaExtractValueFromStream(handle,stream,a,pp);
          CfOut(cf_verbose,""," -> Setting Nova slot %d=%s to %lf\n",slot,handle,this[slot]);
          }
       else if (cf_strcmp(a.measure.history_type,"log") == 0)
          {
          CfOut(cf_verbose,""," -> Promise to log a numerical value");
          NovaLogSymbolicValue(handle,stream,a,pp);
          }
       else /* static */
          {
          CfOut(cf_verbose,""," -> Promise to store a static numerical value");
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

// stream gets de-allocated in ReSample

Nova_DumpSlots();
}

/*****************************************************************************/

static void NovaOpenNewLifeCycle(int age, FILE **fp)
{
int i;
char filename[CF_MAXVARSIZE];

Debug("OPEN YEAR %d\n",age);

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   sprintf(filename, "%s_%d.yr", OBS[i][0], age);

   if ((fp[i] = fopen(filename,"w")) == NULL)
      {
      CfOut(cf_error,"fopen","Could not open %s\n",filename);
      exit(1);
      }
   }
}

/*****************************************************************************/

static void NovaCloseLifeCycle(int age, FILE **fp)
{
int i;

Debug("CLOSE YEAR %d\n",age);

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   fclose(fp[i]);
   }
}

/*****************************************************************************/

void Nova_LongHaul(time_t current)
{
int y,i,j,k;
char filename[CF_BUFSIZE];
CF_DB *dbp;

if (LICENSES == 0)
   {
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_HISTORYDB);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

time_t now = CFSTARTTIME;
time_t w = SubtractWeeks(WeekBegin(now), MONITORING_HISTORY_LENGTH_WEEKS - 1);

/* Graphs of the past MONITORING_HISTORY_LENGTH_YEARS */

for (y = 0; y < MONITORING_HISTORY_LENGTH_YEARS; ++y)
   {
   FILE *fp[CF_OBSERVABLES];
   NovaOpenNewLifeCycle(MONITORING_HISTORY_LENGTH_YEARS - 1 - y, fp);

   for (i = 0; i < MONITORING_WEEKS_PER_YEAR; ++i)
      {
      for (j = 0; j < SHIFTS_PER_WEEK && w <= now; ++j, w = NextShift(w))
         {
         struct Averages av;
         if (GetRecordForTime(dbp, w, &av))
            {
            for (k = 0; k < CF_OBSERVABLES; ++k)
               {
               double q = BoundedValue(av.Q[k].q, 0);
               double var = BoundedValue(av.Q[k].var, q*q);
               double expect = BoundedValue(av.Q[k].expect, q);
               fprintf(fp[k], "%d %.2lf %.2lf %.2lf\n", j + SHIFTS_PER_WEEK*i,
                       expect, sqrt(var), q);
               }
            }
         else
            {
            for (k = 0; k < CF_OBSERVABLES; ++k)
               {
               fprintf(fp[k], "%d 0.00 0.00 0.00\n", j + SHIFTS_PER_WEEK*i);
               }
            }
         }
      }

   NovaCloseLifeCycle(MONITORING_WEEKS_PER_YEAR - 1 - y, fp);
   }

CloseDB(dbp);
}

/*****************************************************************************/

void Nova_SetMeasurementPromises(struct Item **classlist)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char dbname[CF_MAXVARSIZE],eventname[CF_MAXVARSIZE],assignment[CF_BUFSIZE];
  struct Event entry;
  char *key;
  void *stored;
  int ksize,vsize;

snprintf(dbname,CF_MAXVARSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_MEASUREDB);
MapName(dbname);

if (!OpenDB(dbname,&dbp))
   {
   return;
   }

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

memset(&entry, 0, sizeof(entry)); 

while(NextDB(dbp,dbcp,&key,&ksize,&stored,&vsize))
   {
   if (stored != NULL)
      {
      strcpy(eventname,(char *)key);
      memcpy(&entry,stored,sizeof(entry));

      CfOut(cf_verbose,""," -> Setting measurement event %s\n",eventname);

      // a.measure.data_type is not longer known here, so look for zero decimals

      if ((int)(entry.Q.q * 10) % 10 == 0)
         {
         snprintf(assignment,CF_BUFSIZE-1,"value_%s=%.0lf",eventname,entry.Q.q);
         }
      else
         {
         snprintf(assignment,CF_BUFSIZE-1,"value_%s=%.2lf",eventname,entry.Q.q);
         }
      
      AppendItem(classlist,assignment,NULL);

      snprintf(assignment,CF_BUFSIZE-1,"av_%s=%.2lf",eventname,entry.Q.expect);
      AppendItem(classlist,assignment,NULL);
      snprintf(assignment,CF_BUFSIZE-1,"dev_%s=%.2lf",eventname,sqrt(entry.Q.var));
      AppendItem(classlist,assignment,NULL);
      }
   }

CloseDB(dbp);
}

/*****************************************************************************/

void Nova_LoadSlowlyVaryingObservations()

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *stored;
  int ksize,vsize;
  char name[CF_BUFSIZE];

if (THIS_AGENT_TYPE == cf_executor || LOOKUP)
   {
   return;
   }
  
snprintf(name,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

while(NextDB(dbp,dbcp,&key,&ksize,&stored,&vsize))
   {
   char buf[CF_MAXVARSIZE],lval[CF_MAXVARSIZE],rval[CF_BUFSIZE];
   enum cfdatatype type;
   struct Rlist *list = NULL;

   strncpy(buf,key,CF_MAXVARSIZE-1);

   sscanf(buf,"%[^:]:%d",lval,(int *)&type);

   if (stored != NULL)
      {
      strncpy(rval,stored,CF_BUFSIZE-1);

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

         default:
            CfOut(cf_error,"","Unexpected value type reading from database: %d\n",(int)type);
         }
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_DumpSlowlyVaryingObservations()

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char name[CF_BUFSIZE];
  FILE *fout;
  char *key;
  void *stored;
  int ksize,vsize;

if (LICENSES == 0)
   {
   return;
   }

snprintf(name,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);

if (!OpenDB(name,&dbp))
   {
   return;
   }

snprintf(name,CF_BUFSIZE-1,"%s%cstate%cstatic_data",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Unable to save discovery data in %s\n",name);
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

while(NextDB(dbp,dbcp,&key,&ksize,&stored,&vsize))
   {
   char buf[CF_MAXVARSIZE],lval[CF_MAXVARSIZE],rval[CF_BUFSIZE];
   enum cfdatatype type;

   strncpy(buf,key,CF_MAXVARSIZE-1);

   sscanf(buf,"%s:%d",lval,(int *)&type);

   if (stored != NULL)
      {
      strncpy(rval,stored,CF_BUFSIZE-1);
      fprintf(fout,"%s:%s\n",lval,rval);
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
fclose(fout);
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
/* Level                                                                     */
/*****************************************************************************/

static int NovaGetSlot(const char *name)
{
int i;

Nova_LoadSlots();

/* First try to find existing slot */
for (i = 0; i < CF_OBSERVABLES - ob_spare; ++i)
   {
   if (SLOTS[i] && !strcmp(SLOTS[i]->name, name))
      {
      CfOut(cf_verbose, "", " -> Using slot ob_spare+%d (%d) for %s\n", i, i + ob_spare, name);
      return i + ob_spare;
      }
   }

/* Then find the spare one */
for (i = 0; i < CF_OBSERVABLES - ob_spare; ++i)
   {
   if (!SLOTS[i])
      {
      CfOut(cf_verbose, "", " -> Using empty slot ob_spare+%d (%d) for %s\n", i, i + ob_spare, name);
      return i + ob_spare;
      }
   }

CfOut(cf_error, "", "Measurement slots are all in use - it is not helpful to measure too much, you can't usefully follow this many variables");

return -1;
}

/*****************************************************************************/

int NovaRegisterSlot(const char *name, const char *description,
                     const char *units, double expected_minimum, double expected_maximum,
                     bool consolidable)
{
int slot = NovaGetSlot(name);
if (slot == -1)
   {
   return -1;
   }

Nova_FreeSlot(SLOTS[slot - ob_spare]);
SLOTS[slot - ob_spare] = Nova_MakeSlot(name, description, units,
                                       expected_minimum, expected_maximum,
                                       consolidable);
Nova_DumpSlots();

return slot;
}

bool NovaHasSlot(int idx)
{
Nova_LoadSlots();

return idx < ob_spare || SLOTS[idx - ob_spare];
}

const char *NovaGetSlotName(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? OBS[idx][0] : SLOTS[idx - ob_spare]->name;
}

const char *NovaGetSlotDescription(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? OBS[idx][1] : SLOTS[idx - ob_spare]->description;
}

const char *NovaGetSlotUnits(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? UNITS[idx] : SLOTS[idx - ob_spare]->units;
}

// TODO: real expected minimum/maximum/consolidable for core slots

double NovaGetSlotExpectedMinimum(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? 0.0f : SLOTS[idx - ob_spare]->expected_minimum;
}

double NovaGetSlotExpectedMaximum(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? 100.0f : SLOTS[idx - ob_spare]->expected_maximum;
}

bool NovaIsSlotConsolidable(int idx)
{
Nova_LoadSlots();

return idx < ob_spare ? true : SLOTS[idx - ob_spare]->consolidable;
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
  char comm[20];
  struct timespec start;
  FILE *fin = NULL;
  mode_t maskval = 0;
  struct Attributes at = {{0}};

if (LICENSES == 0)
   {
   CfOut(cf_verbose,""," !! No valid commercial license");
   return NULL;
   }
  
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

if (MONITOR_RESTARTED)
   {
   // Force a measurement if restarted
   at = a;
   a.transaction.ifelapsed = 0;
   }
else
   {
   at = a;
   }

CFSTARTTIME = time(NULL);

thislock = AcquireLock(pp->promiser,VUQNAME,CFSTARTTIME,at,pp,false);

if (thislock.lock == NULL)
   {
   if (cf_strcmp(a.measure.history_type,"log") == 0)
      {
      DeleteItemList(NOVA_DATA[slot].output);
      NOVA_DATA[slot].output = NULL;
      }
   else
      {
      /* If static or time-series, and too soon or busy then use a cached value
         to avoid artificial gaps in the history */
      }

   MONITOR_RESTARTED = false;
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

   if (a.measure.stream_type && cf_strcmp(a.measure.stream_type,"file") == 0)
      {
      long filepos = 0;
      struct stat sb;

      CfOut(cf_verbose,""," -> Stream \"%s\" is a plain file\n",pp->promiser);

      if (cfstat(pp->promiser,&sb) == -1)
         {
         CfOut(cf_inform,"stat"," !! Unable to find stream %s\n",pp->promiser);
         YieldCurrentLock(thislock);         
         MONITOR_RESTARTED = false;
         
         return NULL;
         }
      
      fin = fopen(pp->promiser,"r");

      if (a.measure.growing)
         {
         filepos = Nova_RestoreFilePosition(pp->promiser);
         
         if (sb.st_size >= filepos)
            {
            fseek(fin,filepos,SEEK_SET);
            }
         }
      }
   else if (a.measure.stream_type && cf_strcmp(a.measure.stream_type,"pipe") == 0)
      {
      CfOut(cf_verbose,""," -> (Setting pipe umask to %o)\n",a.contain.umask);
      maskval = umask(a.contain.umask);

      if (a.contain.umask == 0)
         {
         CfOut(cf_verbose,""," !! Programming %s running with umask 0! Use umask= to set\n",pp->promiser);
         }

      if (a.contain.useshell)
         {
         fin = cf_popen_shsetuid(pp->promiser,"r",a.contain.owner,a.contain.group,a.contain.chdir,a.contain.chroot,false);
         }
      else
         {
	 fin = cf_popensetuid(pp->promiser,"r",a.contain.owner,a.contain.group,a.contain.chdir,a.contain.chroot,false);
         }
      }

   /* generic file stream */

   if (fin == NULL)
      {
      cfPS(cf_error,CF_FAIL,"cf_popen",pp,a,"Couldn't open pipe to command %s\n",pp->promiser);
      YieldCurrentLock(thislock);
      MONITOR_RESTARTED = false;
      return NOVA_DATA[slot].output;
      }

   while (!feof(fin))
      {
      if (ferror(fin))  /* abortable */
         {
         cfPS(cf_error,CF_TIMEX,"ferror",pp,a,"Sample stream %s\n",pp->promiser);
         YieldCurrentLock(thislock);
         return NOVA_DATA[slot].output;
         }

      CfReadLine(line,CF_BUFSIZE-1,fin);
      AppendItem(&(NOVA_DATA[slot].output),line,NULL);
      CfOut(cf_inform,"","Sampling => %s",line);

      if (ferror(fin))  /* abortable */
         {
         cfPS(cf_error,CF_TIMEX,"ferror",pp,a,"Sample stream %s\n",pp->promiser);
         YieldCurrentLock(thislock);
         return NOVA_DATA[slot].output;
         }
      }
   
   if (a.measure.stream_type && cf_strcmp(a.measure.stream_type,"file") == 0)
      {
      long fileptr = ftell(fin);
      fclose(fin);
      Nova_SaveFilePosition(pp->promiser,fileptr);
      }
   else if (a.measure.stream_type && cf_strcmp(a.measure.stream_type,"pipe") == 0)
      {
      cf_pclose_def(fin,a,pp);
      }
   }

if (a.contain.timeout != 0)
   {
   alarm(0);
   signal(SIGALRM,SIG_DFL);
   }

CfOut(cf_inform,""," -> Collected sample of %s\n",pp->promiser);
umask(maskval);
YieldCurrentLock(thislock);
MONITOR_RESTARTED = false;

snprintf(eventname,CF_BUFSIZE-1,"Sample(%s)",pp->promiser);
EndMeasure(eventname,start);
return NOVA_DATA[slot].output;
}

/*****************************************************************************/

double NovaExtractValueFromStream(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp)

{ char value[CF_MAXVARSIZE];
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

   if (a.measure.select_line_matching && FullTextMatch(a.measure.select_line_matching,ip->name))
      {
      CfOut(cf_verbose,""," ?? Look for %s regex %s\n",handle,a.measure.select_line_matching);
      found = true;
      match = ip;
      match_count++;

      if (a.measure.extraction_regex)
         {
         switch (a.measure.data_type)
            {
            case cf_int:
            case cf_real:
                
                strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
                
                if(strcmp(value, "CF_NOMATCH") == 0)
                   {
                   real_val = CF_NODOUBLE;
                   }
                else
                   {
                   real_val += Str2Double(value);
                   }
                break;

            default:
               CfOut(cf_error,"","Unexpected data type in data_type attribute: %d\n",a.measure.data_type);
            }
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

   default:
      CfOut(cf_error, "", "Unexpected data type in data_type attribute: %d", a.measure.data_type);
   }

if (real_val == CF_NODOUBLE)
   {
   cfPS(cf_inform,CF_FAIL,"",pp,a," !! Unable to extract a value from the matched line \"%\"",match->name);
   PromiseRef(cf_inform,pp);
   real_val = 0.0;
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

if (stream == NULL)
   {
   CfOut(cf_verbose,""," -> No stream to measure");
   return;
   }

if (LICENSES == 0)
   {
   CfOut(cf_verbose,""," !! No valid commercial license");
   return;
   }

CfOut(cf_verbose,""," -> Locate and log sample ...");

for (ip = stream; ip != NULL; ip = ip->next)
   {
   if (ip->name == NULL)
      {
      continue;
      }
   
   if (count == a.measure.select_line_number)
      {
      CfOut(cf_verbose,"","Found line %d by number...\n",count);
      found = true;
      match_count = 1;
      match = ip;

      if (a.measure.extraction_regex)
         {
         CfOut(cf_verbose,""," -> Now looking for a matching extractor \"%s\"",a.measure.extraction_regex);
         CfOut(cf_inform,"","Extracted value \"%s\" for promise \"%s\"",value,handle);
         strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
         AppendItem(&matches,value,NULL);
         }
      break;
      }

   if (a.measure.select_line_matching && FullTextMatch(a.measure.select_line_matching,ip->name))
      {
      CfOut(cf_verbose,"","Found line %d by pattern...\n",count);
      found = true;
      match = ip;
      match_count++;

      if (a.measure.extraction_regex)
         {
         CfOut(cf_verbose,""," -> Now looking for a matching extractor \"%s\"",a.measure.extraction_regex);
         CfOut(cf_inform,"","Extracted value \"%s\" for promise \"%s\"",value,handle);
         strncpy(value,ExtractFirstReference(a.measure.extraction_regex,match->name),CF_MAXVARSIZE-1);
         AppendItem(&matches,value,NULL);
         }
      }

   count++;
   }

if (!found)
   {
   cfPS(cf_inform,CF_FAIL,"",pp,a,"Promiser \"%s\" found no matching line",pp->promiser);
   return;
   }

if (match_count > 1)
   {
   CfOut(cf_inform,""," !! Warning: %d lines matched the line_selection \"%s\"- matching to last",match_count,a.measure.select_line_matching);
   }

switch (a.measure.data_type)
   {
   case cf_counter:
       CfOut(cf_verbose,""," -> Counted %d for %s\n",match_count,handle);
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

if (a.measure.history_type && cf_strcmp(a.measure.history_type,"log") == 0)
   {
   snprintf(filename,CF_BUFSIZE,"%s%cstate%c%s_measure.log",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,handle);

   if ((fout = fopen(filename,"a")) == NULL)
      {
      cfPS(cf_error,CF_FAIL,"",pp,a,"Unable to open the output log \"%s\"",filename);
      PromiseRef(cf_error,pp);
      return;
      }

   strncpy(sdate,cf_ctime(&now),CF_MAXVARSIZE-1);
   Chop(sdate);

   fprintf(fout,"%s,%ld,%s\n",sdate,(long)now,value);
   CfOut(cf_verbose,"","Logging: %s,%s to %s\n",sdate,value,filename);

   fclose(fout);
   }
else // scalar or static
   {
   CF_DB *dbp;
   char id[CF_MAXVARSIZE];

   snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);

   if (!OpenDB(filename,&dbp))
      {
      return;
      }

   snprintf(id,CF_MAXVARSIZE-1,"%s:%d",handle,a.measure.data_type);
   WriteDB(dbp,id,value,strlen(value)+1);
   CloseDB(dbp);
   }
}

/*****************************************************************************/

void NovaNamedEvent(char *eventname,double value,struct Attributes a,struct Promise *pp)

{ char dbname[CF_BUFSIZE];
  struct Event ev_new,ev_old;
  time_t lastseen, now = time(NULL);
  double delta2;
  CF_DB *dbp;

snprintf(dbname,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_MEASUREDB);

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

CfOut(cf_verbose,""," -> Wrote scalar named event \"%s\" = (%.2lf,%.2lf,%.2lf)",eventname,ev_new.Q.q,ev_new.Q.expect,sqrt(ev_new.Q.var));
WriteDB(dbp,eventname,&ev_new,sizeof(ev_new));

CloseDB(dbp);
}


/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_SaveFilePosition(char *name,long fileptr)

{ char filename[CF_BUFSIZE];
  CF_DB *dbp;
 
snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

CfOut(cf_verbose,"","Saving state for %s at %ld\n",filename,fileptr);
WriteDB(dbp,filename,&fileptr,sizeof(long));
CloseDB(dbp);
}

/*****************************************************************************/

long Nova_RestoreFilePosition(char *name)

{ char filename[CF_BUFSIZE];
  CF_DB *dbp;
  long fileptr;
 
snprintf(filename,CF_BUFSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_STATICDB);

if (!OpenDB(filename,&dbp))
   {
   return 0L;
   }

ReadDB(dbp,filename,&fileptr,sizeof(long));
CfOut(cf_verbose,"","Resuming state for %s at %ld\n",filename,fileptr);
CloseDB(dbp);
return fileptr;
}

/****************************************************************************/

/*
 * This function returns beginning of last Monday relative to 'time'. If 'time'
 * is Monday, beginning of the same day is returned.
 */
time_t WeekBegin(time_t time)
{
struct tm tm;
gmtime_r(&time, &tm);

/* Move back in time to reach Monday. */

time -= ((tm.tm_wday == 0 ? 6 : tm.tm_wday - 1) * SECONDS_PER_DAY);

/* Move to the beginning of day */

time -= tm.tm_hour * SECONDS_PER_HOUR;
time -= tm.tm_min * SECONDS_PER_MINUTE;
time -= tm.tm_sec;

return time;
}

/****************************************************************************/

time_t SubtractWeeks(time_t time, int weeks)
{
return time - weeks * SECONDS_PER_WEEK;
}

/****************************************************************************/

time_t NextShift(time_t time)
{
return time + SECONDS_PER_SHIFT;
}

/****************************************************************************/

static void MakeTimekey(time_t time, char *result)
{
/* Generate timekey for database */
struct tm tm;
gmtime_r(&time, &tm);

snprintf(result, 64, "%d_%.3s_Lcycle_%d_%s",
         tm.tm_mday,
         MONTH_TEXT[tm.tm_mon],
         (tm.tm_year + 1900) % 3,
         SHIFT_TEXT[tm.tm_hour / 6]);
}

/****************************************************************************/

/* Returns true if entry was found, false otherwise */
bool GetRecordForTime(CF_DB *db, time_t time, struct Averages *result)
{
char timekey[CF_MAXVARSIZE];
MakeTimekey(time, timekey);

return ReadDB(db, timekey, result, sizeof(struct Averages));
}


/****************************************************************************/

void PutRecordForTime(CF_DB *db, time_t time, const struct Averages *values)
{
char timekey[CF_MAXVARSIZE];
MakeTimekey(time, timekey);

WriteDB(db, timekey, values, sizeof(struct Averages));
}

/****************************************************************************/

double BoundedValue(double value, double defval)
{
if (value < 0 || value > CF_BIGNUMBER)
   {
   return defval;
   }
else
   {
   return value;
   }
}
