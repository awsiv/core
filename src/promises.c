
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

#ifdef HAVE_ZONE_H
# include <zone.h>
#endif

char POLICY_SERVER[CF_BUFSIZE];
struct PromiseIdent *PROMISER_LIST[CF_HASHTABLESIZE] = { NULL };
struct PromiseIdent *PROMISER_REGEXES = NULL;

/*****************************************************************************/

void Nova_NewPromiser(struct Promise *pp)

{ int hash;
  char unique[CF_BUFSIZE];

if (strcmp(pp->agentsubtype,"methods") == 0)
   {
   return; // Method promisers are not real objects
   }

if (strcmp(pp->agentsubtype,"delete_lines") == 0)
   {
   return; // Deletion cannot confict
   }

if (IsNakedVar(pp->promiser,'$') || strcmp(pp->agentsubtype,"vars") == 0 || strcmp(pp->agentsubtype,"classes") == 0)
   {
   snprintf(unique,CF_BUFSIZE,"%s: %s (%s)",pp->agentsubtype,pp->promiser,pp->bundle);
   }
else
   {
   snprintf(unique,CF_BUFSIZE,"%s: %s",pp->agentsubtype,pp->promiser);
   }
  
hash = GetHash(unique);
  
if (IsRegex(pp->promiser) && (strchr(pp->promiser,'*') || strchr(pp->promiser,'+')))
   {
   PrependPromiserList(&PROMISER_REGEXES,unique,pp);
   }
else
   {
   PrependPromiserList(&(PROMISER_LIST[hash]),unique,pp);
   }
}

/*****************************************************************************/

void Nova_AnalyzePromiseConflicts()

{ int i;
  struct PromiseIdent *p1,*p2;
  struct Rlist *contexts1,*contexts2;
  struct Promise *pp = NULL;

// There should be no duplicate entries in these lists when we get here
  
for (i = 0; i < CF_HASHTABLESIZE; i++)
   {
   if (PROMISER_LIST[i])
      {
      if (PROMISER_LIST[i]->next)
         {
         for (p1 = PROMISER_LIST[i]; p1 != NULL; p1=p1->next)
            {
            int count = 0;
            
            for (p2 = PROMISER_LIST[i]; p2 != NULL; p2=p2->next)
               {
               if (p1 == p2)
                  {
                  continue;
                  }

               if (strcmp(p1->handle,p2->handle) == 0)
                  {
                  count++;
                  }
               }

            if (count < 1) // Could be a hashing ghost
               {
               continue;
               }

            contexts1 = SplitContextExpression(p1->classes,pp);

            count = 0;
            
            for (p2 = p1->next; p2 != NULL; p2=p2->next)
               {
               if (strcmp(p1->handle,p2->handle) != 0) // Can be hash collisions
                  {
                  continue;
                  }
               
               contexts2 = SplitContextExpression(p2->classes,pp);
               
               if (Nova_ClassesIntersect(contexts1,contexts2))
                  {                     
                  if (count++ == 0)
                     {
                     CfOut(cf_inform,"","I: The resource \"%s\" makes a number of possibly conflicting promises: ",p1->handle);
                     CfOut(cf_inform,"","\n %d: In %s\n    Near line %d\n    In the context of \"%s\"\n",count,p1->filename,p1->lineno,p1->classes);
                     count++;
                     }
                  CfOut(cf_inform,"","\n %d: In %s\n    Near line %d\n    In the context of \"%s\"\n", count,p2->filename,p2->lineno,p2->classes);
                  }
               
               DeleteRlist(contexts2);
               }

            DeleteRlist(contexts1);
            }
         }

      // Check if there are any patterns that might overlap
      
      for (p1 = PROMISER_REGEXES; p1 != NULL; p1=p1->next)
         {
         if (FullTextMatch(p1->handle,PROMISER_LIST[i]->handle))
            {
            CfOut(cf_inform,""," ! promiser \"%s\" might conflict with \"%s\" in file %s near line %d\n",p1->handle,PROMISER_LIST[i]->handle,PROMISER_LIST[i]->filename,PROMISER_LIST[i]->lineno);
            }
         }
      }
   }

/* Finally check if any of the patterns could overlap one another
   This is a much harder problem....

for (p1 = PROMISER_REGEXES; p1 != NULL; p1=p1->next)
   {
   for (p2 = PROMISER_REGEXES; p2 != NULL; p2=p2->next)
      {
      }
   }

*/
}

/*****************************************************************************/

void Nova_EnterpriseContext()

{
#ifdef HAVE_GETZONEID
 zoneid_t zid;
 char zone[ZONENAME_MAX];
 char vbuff[CF_BUFSIZE];

zid = getzoneid();
getzonenamebyid(zid,zone,ZONENAME_MAX);
CfOut(cf_verbose,""," -> Cfengine seems to be running inside a solaris zone of name \"%s\"",zone);

NewScalar("sys","zone",zone,cf_str);
snprintf(vbuff,CF_BUFSIZE-1,"zone_%s",zone);
NewClass(vbuff);
#endif
}

/*****************************************************************************/

void Nova_EnterpriseDiscovery()
{
if (IsDefinedClass("redhat"))
   {
   Nova_SetDocRoot("/var/www/html");
   }

if (IsDefinedClass("SuSE"))
   {
   Nova_SetDocRoot("/srv/www/htdocs");
   }

if (IsDefinedClass("debian"))
   {
   Nova_SetDocRoot("/var/www");
   }
}

/*****************************************************************************/

void Nova_PreSanitizePromise(struct Promise *pp)

{ struct Attributes a = {{0}};

 /* Attempt run-time-like analysis here before execution if possible */

if (strcmp("processes",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("storage",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("packages",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("files",pp->agentsubtype) == 0)
   {
   a = GetFilesAttributes(pp);
   FileSanityChecks(pp->promiser,a,pp);
   return;
   }

if (strcmp("commands",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("databases",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("methods",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("reports",pp->agentsubtype) == 0)
   {
   return;
   }
}

/*****************************************************************************/

void Nova_Version()

{

printf("This core uses commercial Nova extensions at version %s Copyright (C) Cfengine AS 2009-%s\n",NOVA_VERSION,VYEAR);

}

/*****************************************************************************/

char *Nova_GetVersion()

{
return NOVA_VERSION;
}

/*****************************************************************************/

char *Nova_StrVersion()

{ static char buf[64];

snprintf(buf,63,"Contains Nova extensions at version %s\n",NOVA_VERSION);
return buf;
}

/***************************************************************/

void Nova_NotePromiseCompliance(struct Promise *pp,double val,enum cf_status status,char *reason)

{ CF_DB *dbp;
  FILE *fp;
  char name[CF_BUFSIZE],id[CF_MAXVARSIZE];
  static char previous[CF_BUFSIZE];
  struct Event e,newe;
  time_t now = time(NULL);
  double lastseen,delta2;
  double vstatus;      /* end with a rough probability */
  
Debug("Note Promise Compliance\n");

cf_strncpy(id,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (strcmp(previous,id) == 0)
   {
   return;
   }

cf_strncpy(previous,Nova_PromiseID(pp),CF_MAXVARSIZE);

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

switch (status)
   {
   case cfn_kept:
   case cfn_nop:
       vstatus = 1.0;
       break;

   case cfn_repaired:
       vstatus = 0.5;
       break;

   case cfn_notkept:
       vstatus = 0;
       break;
   default:
       return;
   }

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* First record the classes that are in use */

if (ReadDB(dbp,id,&e,sizeof(e)))
   {
   lastseen = now - e.t;
   newe.t = now;
   newe.Q.q = vstatus;
   newe.Q.expect = GAverage(vstatus,e.Q.expect,0.5);
   delta2 = (vstatus - e.Q.expect)*(vstatus - e.Q.expect);
   newe.Q.var = GAverage(delta2,e.Q.var,0.5);
   }
else
   {
   lastseen = 0;
   newe.t = now;
   newe.Q.q = vstatus;
   newe.Q.expect = 0.5*vstatus;  /* With no data it's 50/50 what we can say */
   newe.Q.var = 0.000;
   }

WriteDB(dbp,id,&newe,sizeof(newe));
CloseDB(dbp);

/* Just noise to log variables and classes */

if (pp->agentsubtype == NULL)
   {
   return;
   }

/* Now keep the next log */

switch (status)
   {
   case cfn_repaired:
       snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_REPAIR_LOG);
       break;

   case cfn_notkept:
       snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_NOTKEPT_LOG);
       break;

   default:
       return;
   }

if ((fp = fopen(name,"a")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Could not open \"%s\"",name);
   return;
   }

fprintf(fp,"%ld,%s,%s",(long)now,id,reason);

if(reason[strlen(reason) - 1] != '\n')
  {
  fprintf(fp,"\n");
  }

fclose(fp);
chmod(name,0644);
}

/***************************************************************/

time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *lastseen)

{ CF_DB *dbp;
  char name[CF_MAXVARSIZE];
  struct Event e;
  double lsea = CF_WEEK * 52; /* expire after a year */
  time_t now = time(NULL);

snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

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

if (*lastseen > 0 && (now - *lastseen) > lsea)
   {
   CfOut(cf_verbose,"","Promise record \"%s\" expired ... removing\n",name);
   DeleteDB(dbp,name);
   }

CloseDB(dbp);
return *lastseen;
}

/***************************************************************/

void Nova_TrackValue(char *date,double kept,double repaired, double notkept)

{ char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],key[CF_SMALLBUF],name[CF_BUFSIZE];
  CF_DB *dbp;
  struct promise_value value,new_value;

// Strip out the date resolution so we keep only each day of the year
  
sscanf(date,"%*s %s %s %*s %s",month,day,year);
snprintf(key,CF_SMALLBUF-1,"%s %s %s",day,month,year);

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_VALUE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

if (ReadDB(dbp,key,&value,sizeof(struct promise_value)))
   {
   new_value.kept = value.kept + kept;
   new_value.repaired = value.repaired + repaired;
   new_value.notkept = value.notkept + notkept;
   }
else
   {
   new_value.kept = kept;
   new_value.repaired = repaired;
   new_value.notkept = notkept;
   }

CfOut(cf_verbose,""," -> recording value (%.4lf,%.4lf,%.4lf)",kept,repaired,notkept);
WriteDB(dbp,key,&new_value,sizeof(struct promise_value));
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_LastSawBundle(char *name)

{ char filename[CF_BUFSIZE];
  int lsea = LASTSEENEXPIREAFTER;
  double lastseen,delta2;
  struct QPoint q,newq;
  time_t now = time(NULL);
  CF_DB *dbp;
 
snprintf(filename,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_BUNDLE_LOG);
MapName(filename);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

if (ReadDB(dbp,name,&q,sizeof(q)))
   {
   lastseen = (double)now - q.q;
   newq.q = (double)now;                   /* Last seen is now-then */
   newq.expect = GAverage(lastseen,q.expect,0.3);
   delta2 = (lastseen - q.expect)*(lastseen - q.expect);
   newq.var = GAverage(delta2,q.var,0.3);
   }
else
   {
   lastseen = 0.0;
   newq.q = (double)now;
   newq.expect = 0.0;
   newq.var = 0.0;
   }

if (lastseen > (double)lsea)
   {
   CfOut(cf_verbose,""," -> Bundle %s expired\n",name);
   DeleteDB(dbp,name);
   }
else
   {
   char timebuffer[26];
   CfOut(cf_verbose,""," -> Bundle \"%s\" promise kept at %s\n",name,cf_strtimestamp_local(now,timebuffer));
   WriteDB(dbp,name,&newq,sizeof(newq));
   }

CloseDB(dbp);
}

/********************************************************************/

int Nova_ClassesIntersect(struct Rlist *contexts1,struct Rlist *contexts2)

{ struct Rlist *rp1, *rp2;

for (rp1 = contexts1; rp1 != NULL; rp1=rp1->next)
   {
   for (rp2 = contexts2; rp2 != NULL; rp2=rp2->next)
      {
      // Should strictly check the commutation on [.&] also
      
      if (strcmp(rp1->item,rp2->item) == 0)
         {
         return true;
         }
      }
   }

return false;
}
