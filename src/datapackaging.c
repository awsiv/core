/*****************************************************************************/
/*                                                                           */
/* File: datapackaging.c                                                     */
/*                                                                           */
/* Created: Wed Jun 16 14:26:50 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h";

/*****************************************************************************/

void Nova_PackPerformance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  FILE *fout;
  double now = (double)time(NULL),average = 0, var = 0;
  double ticksperminute = 60.0;
  char name[CF_BUFSIZE],eventname[CF_BUFSIZE],buffer[CF_BUFSIZE];
  struct Event entry;
  int ret,ksize,vsize,first = true;


CfOut(cf_verbose,""," -> Packing performance data");
  
snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_PERFORMANCE);

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
      measure = entry.Q.q/ticksperminute;;
      average = entry.Q.expect/ticksperminute;;
      var     = entry.Q.var;

      // Promise: reply with data only after the "from" time
      
      if (then < from)
         {
         printf("Skipping time %s",ctime(&then));
         continue;
         }
          
      snprintf(tbuf,CF_BUFSIZE-1,"%s",cf_ctime(&then));
      tbuf[strlen(tbuf)-9] = '\0';                     /* Chop off second and year */

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
      
      snprintf(buffer,CF_MAXVARSIZE-1,"%ld;%s;%7.4lf;%7.4lf",entry.t,eventname,measure,average,sqrt(var)/ticksperminute);
      AppendItem(reply,buffer,NULL);
      }
   else
      {
      continue;
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackClasses(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackSetuid(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackFileChanges(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackDiffs(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackMonitorWeek(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackMonitorMag(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackMonitorHist(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{ int i,j,k;
  int position,day;
  int weekly[CF_OBSERVABLES][CF_GRAINS],ok[CF_OBSERVABLES];
  char filename[CF_BUFSIZE],name[CF_MAXVARSIZE];
  unsigned int histogram[CF_OBSERVABLES][7][CF_GRAINS];
  unsigned int smoothhistogram[CF_OBSERVABLES][7][CF_GRAINS];
  char buffer[CF_BUFSIZE],val[CF_SMALLBUF];
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
         fscanf(fp,"%d ",&(histogram[i][day][position]));
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
         smoothhistogram[j][i][k] = ((double)(histogram[j][i][k-1] + histogram[j][i][k] + histogram[j][i][k+1]))/3.0;
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
         weekly[j][k] += (int)(smoothhistogram[j][i][k]+0.5);

         if (weekly[j][k] > 0)
            {
            ok[j] = true;
            }
         }
      }
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (ok[i])
      {
      LookUpClassName(i,name);
      
      snprintf(buffer,CF_BUFSIZE,"%d,%s:",i,name);
      
      for (k = 0; k < CF_GRAINS; k++)
         {      
         snprintf(val,CF_SMALLBUF,"%d ",weekly[i][k]);
         strcat(buffer,val);
         }
      
      AppendItem(reply,buffer,NULL);
      }
   }
}

/*****************************************************************************/

void Nova_PackMonitorYear(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackCompliance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackSoftware(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackAvailPatches(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackPatchStatus(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_Pack_promise_output_common(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackValueReport(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackVariables(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackLastSeen(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackTotalCompliance(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackRepairLog(struct Item **reply,char *header,time_t from,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackNotKeptLog(struct Item **reply,char *header,time_t from,enum cfd_menu type)

{
}


