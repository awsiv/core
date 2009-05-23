
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: promises.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

int Nova_EnterpriseExpiry(char *day,char *month,char *year)

/* This function is a convenience to commerical clients during testing */
    
{ struct stat sb;
  char name[CF_MAXVARSIZE];
  FILE *fp;
  
snprintf(name,CF_MAXVARSIZE,"%s/NEE",CFWORKDIR);
  
if (stat(name,&sb) == -1)
   {
   Debug("Y. %s > %s\nM. %s > %s\nD: %s > %s = %d\n",VYEAR,year,VMONTH,month,VDAY,day,cf_strcmp(VDAY,day));
   
   if ((cf_strcmp(VYEAR,year) >= 0) && (cf_strcmp(VMONTH,month) >= 0) && (cf_strcmp(VDAY,day) > 0))
      {
      if (fp = cf_fopen(name,"w"))
         {
         fprintf(fp,"enable\n");
         cf_fclose(fp);
         }
      return true;
      }

   
   return false;
   }
else
   {
   return true;
   }
}

/*****************************************************************************/

void Nova_Version()

{
printf("contains Nova extensions at version %s\n",VERSION);
}

/*****************************************************************************/

char *Nova_StrVersion()

{ static char buf[64];

snprintf(buf,63,"Contains Nova extensions at version %s\n",VERSION);
return buf;
}

/***************************************************************/

void Nova_NotePromiseCompliance(struct Promise *pp,double val)

{ DB *dbp;
  DBC *dbcp;
  char name[CF_BUFSIZE];
  struct Event e,newe;
  time_t now = time(NULL);
  double lastseen,delta2;
  double vtrue = 1.0;      /* end with a rough probability */

Debug("Note Promise Compliance\n");

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* First record the classes that are in use */

cf_strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (ReadDB(dbp,name,&e,sizeof(e)))
   {
   lastseen = now - e.t;
   newe.t = now;
   newe.Q.q = vtrue;
   newe.Q.expect = GAverage(vtrue,e.Q.expect,0.5);
   delta2 = (vtrue - e.Q.expect)*(vtrue - e.Q.expect);
   newe.Q.var = GAverage(delta2,e.Q.var,0.5);
   }
else
   {
   lastseen = 0;
   newe.t = now;
   newe.Q.q = 0.5*vtrue;
   newe.Q.expect = 0.5*vtrue;  /* With no data it's 50/50 what we can say */
   newe.Q.var = 0.000;
   }

WriteDB(dbp,name,&newe,sizeof(newe));

dbp->close(dbp,0);
}

/***************************************************************/

time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *lastseen)

{ DB *dbp;
  DBC *dbcp;
  char name[CF_MAXVARSIZE];
  struct Event e;
  double lsea = CF_WEEK * 52; /* expire after a year */
  
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
   return (time_t)0;
   }

cf_strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (ReadDB(dbp,name,&e,sizeof(e)))
   {
   *lastseen = e.t;
   *value = e.Q.q;
   *average = e.Q.expect;
   *var = e.Q.var;
   }
else
   {
   *lastseen = 0;
   *value = 0;
   *average = 0;
   *var = 0;
   }

if (*lastseen > lsea)
   {
   Debug("Promise record %s expired\n",name);
   DeleteDB(dbp,name);
   }

dbp->close(dbp,0);
return *lastseen;
}
