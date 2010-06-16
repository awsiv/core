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


void Nova_PackPerformance(time_t date,enum cfd_menu type)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *value;
  FILE *fout;
  double now = (double)time(NULL),average = 0, var = 0;
  double ticksperminute = 60.0;
  char name[CF_BUFSIZE],eventname[CF_BUFSIZE];
  struct Event entry;
  int ret,ksize,vsize;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_PERFORMANCE);

if (!OpenDB(name,&dbp))
   {
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
      snprintf(buffer,CF_MAXVARSIZE-1,"%s;%7.4lf;%7.4lf,%7.4lf",entry.t,eventname,measure,average,sqrt(var)/ticksperminute);

      // ADD TO LIST?
      }
   else
      {
      continue;
      }
   }

// Send data

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_PackClasses(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackSetuid(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackFileChanges(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackDiffs(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackMonitor(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackCompliance(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackSoftware(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackAvailPatches(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackPatchStatus(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_Pack_promise_output_common(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackValueReport(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackVariables(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackLastSeen(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackTotalCompliance(time_t date,enum cfd_menu type)

{
}

/*****************************************************************************/

void Nova_PackRepairLog(time_t date,enum cfd_menu type)
{
}

/*****************************************************************************/

void Nova_PackNotKeptLog(time_t date,enum cfd_menu type)

{
}


