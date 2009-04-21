
/*

 This file is (C) Cfengine AS. See LICENSE for details.

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
  double lsea = CF_WEEK * 52; /* expire after a year */
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

strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

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

if (lastseen > lsea)
   {
   Debug("Promise record %s expired\n",name);
   DeleteDB(dbp,name);   
   }
else
   {
   WriteDB(dbp,name,&newe,sizeof(newe));
   }

dbp->close(dbp,0);
}

/***************************************************************/

time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *lastseen)

{ DB *dbp;
  DBC *dbcp;
  char name[CF_MAXVARSIZE];
  struct Event e;
  
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
   return (time_t)0;
   }

strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

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

dbp->close(dbp,0);
return *lastseen;
}
