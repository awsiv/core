
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: web_api.c                                                           */
/*                                                                           */
/* Created: Tue Jul 27 15:44:14 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC

char *BASIC_REPORTS[] =
   {
   "Bundle profile",
   "Business value report",
   "Class profile",
   "Compliance by promise",
   "Compliance summary",
   "File change log",
   "File change diffs",
   "Last saw hosts",
   "Patches available",
   "Patch status",
   "Performance",
   "Promises repaired",
   "Promises not kept",
   "Setuid/gid root programs",
   "Software installed",
   "Variables",
   NULL
   };

/*****************************************************************************/

void Nova_EnterpriseModuleTrick()

{ char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c
 
if (false)
   {
   Nova2PHP_software_report(0,0,0,0,0,cfr_software,buffer,20);
   Nova2PHP_classes_report(NULL,NULL,0,buffer,1000);
   Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,buffer,1000);
   Nova2PHP_compliance_promises(NULL,NULL,"x",0,buffer,10000);
   Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,buffer,10000);
   Nova2PHP_bundles_report(NULL,NULL,0,buffer,10000);
   Nova2PHP_performance_report(NULL,NULL,0,buffer,10000);
   Nova2PHP_setuid_report(NULL,NULL,0,buffer,10000);
   Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",buffer,10000);
   Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",buffer,10000);
   Nova2PHP_value_report(NULL,NULL,NULL,NULL,buffer,1000);
   Nova2PHP_promiselog(NULL,NULL,1,buffer,1000);
   Nova2PHP_promises(NULL, NULL, NULL, 0);
   Nova2PHP_getlastupdate(NULL,buffer,10);

   Nova_AnalyseMag(NULL,NULL,5,buffer,10);
   Nova_AnalyseWeek(NULL,NULL,5,buffer,10);
   Nova_AnalyseHist(NULL,NULL,5,buffer,10);
   
   CFDB_PutValue("one_two","three");
   CFDB_GetValue("newvar",buffer,120);
   Nova2PHP_count_hosts();
   Nova2PHP_count_red_hosts();
   Nova2PHP_count_yellow_hosts();
   Nova2PHP_count_green_hosts();
   Nova_Header("123","123",buffer,1000);
   Nova_Footer(buffer,1000);
   Nova2PHP_choose_hosts("123",2,buffer,1000);
   NovaInitMeasurements();
   Nova2PHP_get_host_colour(NULL,buffer,3);
   }
}

/*****************************************************************************/
/* Helper functions                                                          */
/*****************************************************************************/

void Nova2PHP_getlastupdate(char *hostkey,char *buffer,int bufsize)

{ time_t then;
  mongo_connection dbconn;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   then = 0;
   CFDB_QueryLastUpdate(&dbconn,hostkey,&then);
   if (then > 0)
      {
      snprintf(buffer,bufsize,"%s",cf_ctime(&then));
      }
   else
      {
      snprintf(buffer,bufsize,"never");
      }
   }
else
   {
   snprintf(buffer,bufsize,"never");
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }
}

/*****************************************************************************/

void Nova2PHP_summary_meter(char *docroot,char *hostkey)

{
Nova_SummaryMeter(docroot,hostkey);
}

/*****************************************************************************/

void Nova2PHP_performance_analysis(char *docroot,char *hostkey,char *buffer,int bufsize)

{
Nova_PerformancePage(docroot,hostkey,buffer,bufsize);
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2PHP_promiselog(char *hostkey,char *handle,enum promiselog_rep type,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseLog *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPromiseLog(&dbconn,&query,type,handle,true);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryPromiseLog(&dbconn,bson_empty(&b),type,handle,true);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->cause,cf_ctime(&(hp->t)));
          
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubPromiseLog);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_value_report(char *hostkey,char *day,char *month,char *year,char *returnval,int bufsize)

{ struct HubValue *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;
  char buffer[CF_BUFSIZE];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryValueReport(&dbconn,&query,day,month,year);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryValueReport(&dbconn,bson_empty(&b),day,month,year);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubValue *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%.1lf</td><td>%.1lf</td><td>%.1lf</td></tr>\n",
            hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept);
          
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubValue);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;


}

/*****************************************************************************/

int Nova2PHP_software_report(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubSoftware *hs;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (hostkey && strlen(hostkey) != 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   hq = CFDB_QuerySoftware(&dbconn,&query,type,name,value,arch,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QuerySoftware(&dbconn,bson_empty(&b),type,name,value,arch,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));

   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubSoftware);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_classes_report(char *hostkey,char *name,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubClass *hc;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);

   hq = CFDB_QueryClasses(&dbconn,&query,name,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryClasses(&dbconn,bson_empty(&b),name,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td>%s</td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));

   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
 struct HubVariable *hv,*hv2;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize = 0;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryVariables(&dbconn,&query,scope,lval,rval,type,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryVariables(&dbconn,bson_empty(&b),scope,lval,rval,type,regex);
   }


returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hv = (struct HubVariable *)rp->item;
   snprintf(buffer,CF_BUFSIZE,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td>\n",hv->hh->hostname,hv->dtype,hv->scope,hv->lval);
   strcat(returnval,buffer);
   count += strlen(buffer);

   if (strlen(hv->dtype) > 1) // list
      {
      char b[CF_BUFSIZE];
      b[0] = '\0';
      PrintRlist(b,CF_BUFSIZE,hv->rval);
      snprintf(buffer,sizeof(buffer),"<td>%s</td></tr>\n",b);
      }
   else
      {
      snprintf(buffer,sizeof(buffer),"<td>%s</td></tr>\n",(char *)hv->rval);
      }

   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      snprintf(buffer,sizeof(buffer),"<td>too big</td></tr>\n");
      break;
      }
   
   Join(returnval,buffer,bufsize);
   count += tmpsize;

   // Split tables per scope
   
   if (rp->next)
      {
      hv2 = (struct HubVariable *)rp->next->item;

      if (strcmp(hv->scope,hv2->scope) != 0)
         {
         Join(returnval,"</table><table>\n",bufsize);
         }
      }
   }

strcat(returnval,"</table>\n");

//DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubTotalCompliance *ht;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryTotalCompliance(&dbconn,&query,version,t,k,nk,rep,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryTotalCompliance(&dbconn,bson_empty(&b),version,t,k,nk,rep,icmp);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>\n",
            ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));
          
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubTotalCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_promises(char *hostkey,char *handle,char *status,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseCompliance *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPromiseCompliance(&dbconn,&query,handle,*status,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryPromiseCompliance(&dbconn,bson_empty(&b),handle,*status,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"promises.php?handle=%s\">%s</a></td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->handle,hp->e,hp->d,cf_ctime(&(hp->t)));
          
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubLastSeen *hl;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;
  char inout[CF_SMALLBUF];
  time_t then;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryLastSeen(&dbconn,&query,lhash,lhost,laddress,lago,lregex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryLastSeen(&dbconn,bson_empty(&b),lhash,lhost,laddress,lago,lregex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (struct HubLastSeen *)rp->item;

   switch (hl->io)
      {
      case '+':
          snprintf(inout,CF_SMALLBUF,"out(+)");
          break;
      case '-':
          snprintf(inout,CF_SMALLBUF,"in(-)");
          break;
      }

   then = hl->t;
   
   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,cf_ctime(&then),
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
          
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubLastSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_performance_report(char *hostkey,char *job,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPerformance *hP;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPerformance(&dbconn,&query,job,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryPerformance(&dbconn,bson_empty(&b),job,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hP = ( struct HubPerformance *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hP->hh->hostname,
            hP->event,hP->q,hP->e,hP->d,cf_ctime(&(hP->t)));
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubPerformance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_report(char *hostkey,char *file,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubSetUid *hS;   
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QuerySetuid(&dbconn,&query,file,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QuerySetuid(&dbconn,bson_empty(&b),file,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( struct HubSetUid *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td></tr>\n",hS->hh->hostname,hS->path);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubBundleSeen *hb;   
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryBundleSeen(&dbconn,&query,bundle,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryBundleSeen(&dbconn,bson_empty(&b),bundle,regex);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = ( struct HubBundleSeen *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td></tr>\n",
            hb->hh->hostname,hb->bundle,cf_ctime(&(hb->t)),
            hb->hrsago,hb->hrsavg,hb->hrsdev);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubBundleSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubFileChanges *hC;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileChanges(&dbconn,&query,file,regex,t,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryFileChanges(&dbconn,bson_empty(&b),file,regex,t,icmp);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hC = (struct HubFileChanges *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)));
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubFileChanges);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];   
  struct HubFileDiff *hd;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileDiff(&dbconn,&query,file,diffs,regex,t,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryFileDiff(&dbconn,bson_empty(&b),file,diffs,regex,t,icmp);
   }

returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),hd->diff);
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubFileDiff);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
/* Search for hosts with property X,Y,Z                                      */
/*****************************************************************************/

int Nova2PHP_hostinfo(char *hostkey,char *returnval1,char *returnval2,int bufsize)

{ char *report,buffer1[CF_BUFSIZE],buffer2[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count1 = 0,count2 = 0,tmpsize1,tmpsize2;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (hostkey && strlen(hostkey) != 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   }
else
   {
   return false;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryHosts(&dbconn,&query);
bson_destroy(&query);

returnval1[0] = '\0';
returnval2[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer1,CF_MAXVARSIZE-1,"%s ",hh->hostname);
   snprintf(buffer2,CF_MAXVARSIZE-1,"%s ",hh->ipaddr);

   tmpsize1 = strlen(buffer1);
   tmpsize2 = strlen(buffer2);
   
   if (count1 + tmpsize1 <= bufsize - 1)
      {
      strcat(returnval1,buffer1);
      count1 += tmpsize1;
      }

   if (count2 + tmpsize2 <= bufsize - 1)
      {
      strcat(returnval2,buffer2);
      count2 += tmpsize2;
      }
   }

DeleteHubQuery(hq,NULL);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (hostkey && strlen(hostkey) != 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   hq = CFDB_QuerySoftware(&dbconn,&query,type,name,value,arch,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QuerySoftware(&dbconn,bson_empty(&b),type,name,value,arch,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubSoftware);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_classes_hosts(char *hostkey,char *name,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);

   hq = CFDB_QueryClasses(&dbconn,&query,name,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryClasses(&dbconn,bson_empty(&b),name,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_vars_hosts(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize = 0;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryVariables(&dbconn,&query,scope,lval,rval,type,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryVariables(&dbconn,bson_empty(&b),scope,lval,rval,type,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_hosts(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryTotalCompliance(&dbconn,&query,version,t,k,nk,rep,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryTotalCompliance(&dbconn,bson_empty(&b),version,t,k,nk,rep,icmp);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubTotalCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_promise_hosts(char *hostkey,char *handle,char *status,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPromiseCompliance(&dbconn,&query,handle,*status,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryPromiseCompliance(&dbconn,bson_empty(&b),handle,*status,regex);
   }


returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_hosts(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;
  char inout[CF_SMALLBUF];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryLastSeen(&dbconn,&query,lhash,lhost,laddress,lago,lregex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryLastSeen(&dbconn,bson_empty(&b),lhash,lhost,laddress,lago,lregex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubLastSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_performance_hosts(char *hostkey,char *job,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPerformance(&dbconn,&query,job,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryPerformance(&dbconn,bson_empty(&b),job,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubPerformance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_hosts(char *hostkey,char *file,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QuerySetuid(&dbconn,&query,file,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QuerySetuid(&dbconn,bson_empty(&b),file,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_hosts(char *hostkey,char *bundle,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryBundleSeen(&dbconn,&query,bundle,regex);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryBundleSeen(&dbconn,bson_empty(&b),bundle,regex);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubBundleSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_hosts(char *hostkey,char *file,int regex,time_t t,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];   
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */
switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileChanges(&dbconn,&query,file,regex,t,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryFileChanges(&dbconn,bson_empty(&b),file,regex,t,icmp);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubFileChanges);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_filediffs_hosts(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];   
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

/* BEGIN query document */

switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileDiff(&dbconn,&query,file,diffs,regex,t,icmp);
   bson_destroy(&query);
   }
else
   {
   hq = CFDB_QueryFileDiff(&dbconn,bson_empty(&b),file,diffs,regex,t,icmp);
   }

returnval[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   snprintf(buffer,CF_MAXVARSIZE-1,"%s,",hh->keyhash);
   
   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

DeleteHubQuery(hq,DeleteHubFileDiff);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
/* Topic Map                                                                 */
/*****************************************************************************/

int Nova2PHP_search_topics(char *search,int regex,char *buffer,int bufsize)
{
Nova_WebTopicMap_Initialize();

Nova_SearchTopicMap(search,buffer,bufsize);
return true;
}

/*****************************************************************************/

void Nova2PHP_show_topic(int id,char *buffer,int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_type[CF_BUFSIZE],topic_comment[CF_BUFSIZE];

buffer[0] = '0';
Nova_WebTopicMap_Initialize();

if (Nova_GetTopicByPid(id,topic_name,topic_id,topic_type,topic_comment))
   {
   snprintf(buffer,bufsize,"<div id=\"topic\">\n'<span id=\"subject\">%s</span>' in section `<span id=\"category\">%s</span>:<p>\"%s\"</div>",topic_name,topic_type,topic_comment);
   }
else
   {
   printf("No such topic\n");
   }
}

/*****************************************************************************/

void Nova2PHP_show_topic_leads(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '0';
Nova_ScanLeadsAssociations(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_show_topic_hits(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '0';
Nova_ScanOccurrences(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_show_topic_category(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '0';
Nova_ScanTheRest(id,buffer,bufsize);
}

/*****************************************************************************/
/* Hosts stats                                                               */
/*****************************************************************************/

void Nova2PHP_show_topN(char *policy,int n,char *buffer,int bufsize)

{ struct Item *ip,*clist;
  char work[CF_MAXVARSIZE];
  static char *policies[] = { "compliance", "anomaly", "performance", "lastseen", NULL};
  enum cf_rank_method pol;

for (pol = 0; policies[pol] != NULL; pol++)
   {
   if (strcmp(policy,policies[pol]) == 0)
      {
      break;
      }
   }

clist = Nova_RankHosts(NULL,0,pol,n);

buffer[0] = '0';
strcat(buffer,"<table>\n\n\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   Nova_Meter("/srv/www/htdocs",ip->name);

   if (Nova_IsGreen(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><img src=\"green.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></a></td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }
   else if (Nova_IsYellow(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><img src=\"yellow.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></a></td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }
   else // if (Nova_IsRed(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><img src=\"red.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }

   Join(buffer,work,bufsize);
   }

Join(buffer,"\n</table>\n",bufsize);
DeleteItemList(clist);
}

/*****************************************************************************/

void Nova2PHP_show_all_hosts(char *policy,int n,char *buffer,int bufsize)

{ struct Item *ip,*clist;
  char work[CF_MAXVARSIZE];

clist = Nova_RankHosts(policy,1,cfrank_compliance,n);
clist = SortItemListClasses(clist);

buffer[0] = '0';
strcat(buffer,"<table>\n\n\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   Nova_Meter("/srv/www/htdocs",ip->name);

   if (Nova_IsGreen(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<td><img src=\"green.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></a></td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }
   else if (Nova_IsYellow(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><img src=\"yellow.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></a></td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }
   else // if (Nova_IsRed(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><img src=\"red.png\"></td><td>%s</td><td><img src=\"/hub/%s/meter.png\"></td><td>%s</td></a></td></tr>\n",ip->classes,ip->name,Nova_HostProfile(ip->name));
      }

   Join(buffer,work,bufsize);
   }

Join(buffer,"\n</table>\n",bufsize);
DeleteItemList(clist);
}

/*****************************************************************************/

void Nova2PHP_show_col_hosts(char *colour,int n,char *buffer,int bufsize)

{ struct Item *ip,*clist;
  char work[CF_MAXVARSIZE];
  int counter = 0;
  
if (strcmp(colour,"green") == 0)
   {
   clist = Nova_GreenHosts();
   }
else if (strcmp(colour,"yellow") == 0)
   {
   clist = Nova_YellowHosts();
   }
else
   {
   clist = Nova_RedHosts();
   }

buffer[0] = '0';
strcat(buffer,"<table>\n<tr>\n\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   if (counter++ % 6 == 0)
      {
      snprintf(work,CF_MAXVARSIZE,"</tr></tr>");
      Join(buffer,work,bufsize);
      }
   
   snprintf(work,CF_MAXVARSIZE,"<td><img src=\"%s.png\"> &nbsp;<a href=\"host.php?hostkey=%s\">%s</a></td></a></td>\n",colour,ip->name,ip->classes,Nova_HostProfile(ip->name));
   
   Join(buffer,work,bufsize);

   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(buffer,"</tr>\n</table>\n",bufsize);
DeleteItemList(clist);
}

/*****************************************************************************/

void Nova2PHP_select_hosts(char *match,int n,char *buffer,int bufsize)

{ struct Item *ip,*clist;
  char work[CF_MAXVARSIZE];

clist = Nova_RankHosts(match,1,cfrank_compliance,n);
clist = SortItemListClasses(clist);

buffer[0] = '\0';
strcat(buffer,"<select name=\"hostkey\">\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   snprintf(work,CF_MAXVARSIZE,"<option value=\"%s\">%s</option>\n",ip->name,ip->classes);
   Join(buffer,work,bufsize);
   }

Join(buffer,"\n</select>\n",bufsize);
DeleteItemList(clist);
}

/*****************************************************************************/

long Nova2PHP_count_hosts()

{ struct Item *all = Nova_ClassifyHostState(NULL,false,-1,0);
  int len = ListLen(all);

DeleteItemList(all);
return (long)len;
}

/*****************************************************************************/

long Nova2PHP_count_red_hosts()

{ struct Item *all = Nova_RedHosts();
  int len = ListLen(all);

DeleteItemList(all);
return (long)len;
}

/*****************************************************************************/

long Nova2PHP_count_yellow_hosts()

{ struct Item *all = Nova_YellowHosts();
  int len = ListLen(all);

DeleteItemList(all);
return (long)len; 
}

/*****************************************************************************/

long Nova2PHP_count_green_hosts()

{ struct Item *all = Nova_GreenHosts();
  int len = ListLen(all);

DeleteItemList(all);
return (long)len;
}

/*****************************************************************************/

void Nova2PHP_get_host_colour(char *hostkey,char *buffer,int bufsize)

{
switch(Nova_GetHostColour(hostkey))
   {
   case CF_RED_THRESHOLD:
       strncpy(buffer,"red",bufsize);
       break;
   case CF_AMBER_THRESHOLD:
       strncpy(buffer,"yellow",bufsize);
       break;
   case CF_GREEN :
       strncpy(buffer,"green",bufsize);
       break;
   default:
       strncpy(buffer,"unknown",bufsize);
       break;
   }
}

/*****************************************************************************/

char *Nova_HostProfile(char *key)

{ static char buffer[CF_BUFSIZE];

snprintf(buffer,CF_BUFSIZE,
         "<table><tr><td><a href=\"bundles.php?host=%s\">Bundles</a></td><td><a href=\"classes.php?host=%s\">Classes</a></td></tr>"
         "<tr><td><a href=\"intentions.php?host=%s\">Intentions</a></td><td><a href=\"promises.php?host=%s\">Promises</a></td></tr></table>",key,key,key,key);

return buffer;
}

/*****************************************************************************/

void Nova2PHP_select_reports(char *buffer,int bufsize)

{ char work[CF_MAXVARSIZE];
  int i;

buffer[0] = '\0';
strcat(buffer,"<select name=\"report\">\n");

for (i = 0; BASIC_REPORTS[i] != NULL; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"<option value=\"%s\">%s</option>\n",BASIC_REPORTS[i],BASIC_REPORTS[i]);
   Join(buffer,work,bufsize);
   }

Join(buffer,"\n</select>\n",bufsize);
}

/*****************************************************************************/

int Nova2PHP_summarize_promise(char *handle, char *returnval,int bufsize)

{ mongo_connection dbconn;
  struct HubPromise *hp;
  char promiseeText[CF_MAXVARSIZE],bArgText[CF_MAXVARSIZE];
  char commentText[CF_MAXVARSIZE], constText[CF_MAXVARSIZE];
  int i,count;
  

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hp = CFDB_QueryPromise(&dbconn, handle);
 
 if(!hp)
   {
     snprintf(returnval, bufsize, "<br>Promise '%s' not found in database", handle);
     return false;
   }

 returnval[0] = '\0';


 if(EMPTY(hp->promisee))
   {
     promiseeText[0] = '\0';
   }
 else
   {
     snprintf(promiseeText, sizeof(promiseeText), " promises '%s'", hp->promisee);     
   }


 if(EMPTY(hp->bundleArgs))
   {
     bArgText[0] = '\0';
   }
 else
   {
     snprintf(bArgText, sizeof(bArgText), " with args '%s'", hp->bundleArgs);
   }

 if(EMPTY(hp->comment))
   {
     commentText[0] = '\0';
   }
 else
   {
     snprintf(commentText, sizeof(commentText), "<br>     comment => %s", hp->comment);
   }


 constText[0] = '\0';

 if(hp->constraints)
   {
     count = 0;
     
     for(i = 0; hp->constraints[i] != NULL; i++)
       {
	 count += strlen("<br>     ") + strlen(hp->constraints[i]);
	 
	 if(count < sizeof(constText))
	   {
	     strcat(constText, "<br>     ");
	     strcat(constText, hp->constraints[i]);
	   }
	 else
	   {
	     break;
	   }

       }
   }

 snprintf(returnval, bufsize, "<br><br>Resource object '%s' of type %s%s%s<br>     context => %s<br>     handle => %s%s<br>Promised in file '%s' near line %d.<br>Part of bundle '%s' (type %s)%s.", hp->promiser, hp->promiseType, promiseeText, commentText, hp->classContext, hp->handle, constText, hp->file, hp->lineNo, hp->bundleName, hp->bundleType, bArgText);
 

 DeleteHubPromise(hp);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


#endif
