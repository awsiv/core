
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
   "Promises repaired log",
   "Promises repaired summary",
   "Promises not kept log",
   "Promises not kept summary",
   "Setuid/gid root programs",
   "Software installed",
   "Variables",
   NULL
   };

char *SPP_REPORTS[] =
   {
   "ACLs",
   "Commands",
   "File Changes",
   "File Diffs",
   "Registry",
   "Services",
   NULL };


/*****************************************************************************/

void Nova_EnterpriseModuleTrick()

{ char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c
 
if (false)
   {
   Nova2PHP_software_report(0,0,0,0,0,cfr_software,NULL,buffer,20);
   Nova2PHP_classes_report(NULL,NULL,0,NULL,buffer,1000);
   Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,NULL,buffer,1000);
   Nova2PHP_compliance_promises(NULL,NULL,"x",0,NULL,buffer,10000);
   Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,NULL,buffer,10000);
   Nova2PHP_bundles_report(NULL,NULL,0,NULL,buffer,10000);
   Nova2PHP_performance_report(NULL,NULL,0,NULL,buffer,10000);
   Nova2PHP_setuid_report(NULL,NULL,0,NULL,buffer,10000);
   Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",NULL,buffer,10000);
   Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",NULL,buffer,10000);
   Nova2PHP_value_report(NULL,NULL,NULL,NULL,NULL,buffer,1000);
   Nova2PHP_promiselog(NULL,NULL,1,NULL,buffer,1000);
   Nova2PHP_promises(NULL, NULL, NULL, 0);
   Nova2PHP_getlastupdate(NULL,buffer,10);

   Nova2PHP_AnalyseMag(NULL,5,buffer,10);
   Nova2PHP_AnalyseWeek(NULL,5,buffer,10);
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
   Nova_Header("sd","we",buffer,3); 
   Nova2PHP_get_network_speed("","",3);
   Nova2PHP_summary_meter(NULL);
   Nova2PHP_meter(NULL);
   Nova2PHP_ComplianceSummaryGraph();
   /* pdf functions */
   Nova2PHP_compliance_report_pdf(NULL,NULL,0,0,0,0,0,NULL,buffer,10000);
   Nova2PHP_promiselog_pdf(NULL,NULL,1,NULL,buffer,1000); 
   Nova2PHP_bundle_report_pdf(NULL,NULL,0,NULL,buffer,10000,NULL);
   Nova2PHP_value_report_pdf(NULL,NULL,NULL,NULL,NULL,buffer,1000);
   Nova2PHP_classes_report_pdf(NULL,NULL,0,NULL, buffer,1000);
   Nova2PHP_compliance_promises_pdf(NULL,NULL,"x",0,NULL, buffer,10000);
   Nova2PHP_filechanges_report_pdf(NULL,NULL,false,-1,">",NULL, buffer,10000);
   Nova2PHP_lastseen_report_pdf(NULL,NULL,NULL,NULL,-1,0,NULL, buffer,10000);
   Nova2PHP_software_report_pdf(0,0,0,0,0,cfr_software,NULL, buffer,20);
   Nova2PHP_performance_report_pdf(NULL,NULL,0,NULL, buffer,10000);
   Nova2PHP_vars_report_pdf(NULL,NULL,NULL,NULL,NULL,0,NULL, buffer,1000);
   Nova2PHP_filediffs_report_pdf(NULL,NULL,NULL,false,-1,">",NULL, buffer,10000);
   Nova2PHP_setuid_report_pdf(NULL,NULL,0,NULL, buffer,10000);
   
   /* svn helper */
      Nova2PHP_validate_policy(NULL,NULL,10000);
   }
}

/*****************************************************************************/
/* Helper functions                                                          */
/****************************************************************************/

void Nova2PHP_getlastupdate(char *hostkey,char *buffer,int bufsize)

{ time_t then, now = time(NULL);
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
      if (now > then + CF_HUB_HORIZON)
         {
         snprintf(buffer,bufsize,"<span id=\"amber\">%s</span>",cf_ctime(&then));
         }
      else if (now > then + CF_HUB_HORIZON*2)
         {
         snprintf(buffer,bufsize,"<span id=\"red\">%s</span>",cf_ctime(&then));
         }
      else
         {
         snprintf(buffer,bufsize,"%s",cf_ctime(&then));
         }
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

void Nova2PHP_summary_meter(char *hostkey)

{
Nova_WebTopicMap_Initialize();
Nova_SummaryMeter(DOCROOT,hostkey);
}

/*****************************************************************************/

void Nova2PHP_meter(char *hostkey)

{
Nova_WebTopicMap_Initialize();
Nova_Meter(DOCROOT,hostkey);
}

/*****************************************************************************/

void Nova2PHP_performance_analysis(char *hostkey,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
Nova_PerformancePage(DOCROOT,hostkey,buffer,bufsize);
}

/*****************************************************************************/

char *Nova2PHP_get_observable_name(int obs,char *buffer,int bufsize)
{
snprintf(buffer,bufsize,"%s",OBS[obs][0]);
return buffer;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2PHP_promiselog(char *hostkey,char *handle,enum promiselog_rep type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseLog *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,true,NULL);

returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Promise handle</th><th>Report</th><th>Time</th></tr>\n");
Join(returnval,buffer,bufsize);
         
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"promise.php?handle=%s\">%s</a></td><td>%s</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->handle,hp->cause,cf_ctime(&(hp->t)));
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_promiselog_summary(char *hostkey,char *handle,enum promiselog_rep type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],hostname[CF_MAXVARSIZE];
  struct HubPromiseLog *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  struct Item *ip,*summary = NULL;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,false,NULL);

hostname[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;
   IdempPrependItem(&summary,hp->handle,hp->cause);
   IncrementItemListCounter(summary,hp->handle);

   if (hostname[0] == '\0')
      {
      strncpy(hostname,hp->hh->hostname,CF_MAXVARSIZE);
      }
   }

DeleteHubQuery(hq,DeleteHubPromiseLog);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

if (summary == NULL)
   {
   snprintf(returnval,bufsize,"No data to report on");
   }
else
   {
   returnval[0] = '\0';
   strcat(returnval,"<table>\n");

   summary = SortItemListCounters(summary);
   snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Promise handle</th><th>Report</th><th>Occurrences</th></tr>\n");
   Join(returnval,buffer,bufsize);
            
   for (ip = summary; ip != NULL; ip=ip->next)
      {
      snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"promise.php?handle=%s\">%s</a></td><td>%s</td><td>%d</td></tr>\n",hostname,ip->name,ip->name,ip->classes,ip->counter);

      Join(returnval,buffer,bufsize);
      }
   
   strcat(returnval,"</table>\n");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_value_report(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize)

{ struct HubValue *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  char buffer[CF_BUFSIZE];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,NULL);

returnval[0] = '\0';

strcat(returnval,"<table>\n");
snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Day</th><th>Kept</th><th>Repaired</th><th>Not kept</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubValue *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%.1lf</td><td>%.1lf</td><td>%.1lf</td></tr>\n",
            hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept);
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_software_report(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubSoftware *hs;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }


 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,NULL);
   

returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Name</th><th>Version</th><th>Architcture</th></tr>\n");
Join(returnval,buffer,bufsize);
      
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_classes_report(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubClass *hc;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  time_t now = time(NULL);

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }


 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,NULL);

returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Class context</th><th>Occurs with<br>Probability</th><th>Uncertainty</th><th>Last seen</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
   
   if (now - hc->t > 6*3600)
      {
      snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td><span id=\"amber\">%s</span></td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   else if (now - hc->t > (time_t)CF_WEEK)
      {
      snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td><span id=\"red\">%s</span></td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   else
      {
      snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td>%s</td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],lscope[CF_MAXVARSIZE];
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

lscope[0] = '\0';
returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (struct HubVariable *)rp->item;

   if (strcmp(lscope,hv->scope) != 0)
      {
      strcpy(lscope,hv->scope);
      snprintf(buffer,CF_BUFSIZE,"<tr><th colspan=\"3\">Bundle scope <a href=\"bundle.php?bundle=%s\">%s</a><th></tr>\n",hv->scope,hv->scope);
      Join(returnval,buffer,bufsize);
      snprintf(buffer,CF_BUFSIZE,"<tr><th>host</th><th>type</th><th>name</th><th>value</th></tr>\n");
      Join(returnval,buffer,bufsize);
      }

   switch (*hv->dtype)
      {
      case 's':
          snprintf(typestr,CF_SMALLBUF,"string");
          break;
      case 'i':
          snprintf(typestr,CF_SMALLBUF,"int");
          break;
      case 'r':
          snprintf(typestr,CF_SMALLBUF,"real");
          break;
      case 'm':
          snprintf(typestr,CF_SMALLBUF,"menu");
          break;
      }

   if (strlen(hv->dtype) == 2)
      {
      strcat(typestr," list");
      }
   
   snprintf(buffer,CF_BUFSIZE,"<tr><td>%s</td><td>%s</td><td><a href=\"knowledge.php?topic=%s\">%s</td>\n",hv->hh->hostname,typestr,hv->lval,hv->lval);
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

   Join(returnval,buffer,bufsize);
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

int Nova2PHP_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubTotalCompliance *ht;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;


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

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,NULL);


returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>policy</th><th>kept</th><th>repaired</th><th>not kept</th><th>last seen</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%d %%</td><td>%d %%</td><td>%d %%</td><td>%s</td></tr>\n",
            ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_compliance_promises(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseCompliance *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,NULL);


returnval[0] = '\0';

strcat(returnval,"<table>\n");
snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>promise handle</th><th>Last known<br>state</th><th>probability<br>kept</th><th>uncertainty</th><th>last seen</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"promise.php?handle=%s\">%s</a></td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_ctime(&(hp->t)));
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubLastSeen *hl;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  char inout[CF_SMALLBUF];
  time_t then;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,classreg);


returnval[0] = '\0';

strcat(returnval,"<table>\n");
count += strlen(returnval);

snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>Initiated</th><th>IP address</th><th>remote host</th><th>last seen</th>"
            "<th>Hours ago</th><th>Avg interval</th><th>Uncertainty</th><th>Remote host key</th></tr>\n");
Join(returnval,buffer,bufsize);
   
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (struct HubLastSeen *)rp->item;

   switch (hl->io)
      {
      case '+':
          snprintf(inout,CF_SMALLBUF,"by us (+)");
          break;
      case '-':
          snprintf(inout,CF_SMALLBUF,"by them (-)");
          break;
      }

   then = hl->t;
   
   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td><a href=\"host.php?hostkey=%s\">%s</a></td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td><span id=\"keyhash\">%s</span></td></tr>\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->keyhash,hl->rhost->ipaddr,cf_ctime(&then),
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
          
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_performance_report(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPerformance *hP;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,NULL);


returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><td>host</td><td>repair</td><td>Last time</td><td>Avg time</td><td>Uncertainty</td><td>Last performed</td></tr>\n");
            
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

int Nova2PHP_setuid_report(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize)

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
snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>setuid/setgid root file</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( struct HubSetUid *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td></tr>\n",hS->hh->hostname,hS->path);
   Join(returnval,buffer,bufsize);
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

int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize,char *classRegex)

{ char *report,buffer[CF_BUFSIZE];
  struct HubBundleSeen *hb;   
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classRegex);

returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Bundle</a></th><th>Last verified</th><th>Hours Ago</th><th>Avg interval</th><th>Uncertainty</th></tr>\n");
Join(returnval,buffer,bufsize);
   
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = ( struct HubBundleSeen *)rp->item;

   if (strcmp(hb->bundle,"QUERY") == 0)
      {
      continue;
      }

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"bundle.php?bundle=%s\">%s</a></td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td></tr>\n",
            hb->hh->hostname,hb->bundle,hb->bundle,cf_ctime(&(hb->t)),
            hb->hrsago,hb->hrsavg,hb->hrsdev);

   Join(returnval,buffer,bufsize);
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

int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{    char *report,buffer[CF_BUFSIZE];
     struct HubFileChanges *hC;
     struct HubQuery *hq;
     struct Rlist *rp,*result;
     int count = 0, tmpsize,icmp;
     mongo_connection dbconn;

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

   hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,NULL);
   
   
   returnval[0] = '\0';
   
   strcat(returnval,"<table>\n");
   snprintf(buffer,sizeof(buffer),"<tr><td>host</td><td>file</td><td>time of change</td></tr>\n");
   Join(returnval,buffer,bufsize);
      
   for (rp = hq->records; rp != NULL; rp=rp->next)
     {
	   hC = (struct HubFileChanges *)rp->item;
	
	   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)));
	   Join(returnval,buffer,bufsize);
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

int Nova2PHP_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];   
  struct HubFileDiff *hd;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;


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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,NULL);

returnval[0] = '\0';

strcat(returnval,"<table>\n");

snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>file</th><th>change detected at</th><th>change</th></tr>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),Nova_FormatDiff(hd->diff));
   Join(returnval,buffer,bufsize);
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
  int counter = 0, n = 180;
  mongo_connection dbconn;


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,NULL);


snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180;
  mongo_connection dbconn;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180;
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

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;

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

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;
  char inout[CF_SMALLBUF];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
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

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_hosts(char *hostkey,char *bundle,int regex,char *returnval,int bufsize,char *classRegex)

{ char *report,buffer[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classRegex);


snprintf(returnval,bufsize,"<table><tr>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</tr></table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;

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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,NULL);


snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);

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
  int counter = 0, n = 180,icmp;
  mongo_connection dbconn;


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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,NULL);

snprintf(returnval,bufsize,"<table>");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host.php?hostkey=%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   Join(returnval,buffer,bufsize);
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

Join(returnval,"</table>\n",bufsize);
DeleteHubQuery(hq,DeleteHubFileDiff);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_get_classes_for_bundle(char *name,char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
  struct Rlist *classList,*rp;
  struct Item *ip,*list = NULL;
  char work[CF_MAXVARSIZE],context[CF_MAXVARSIZE];
  int pid;
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

classList = CFDB_QueryBundleClasses(&dbconn,type,name);

if (classList)
   {
   for (rp = classList; rp != NULL; rp=rp->next)
      {
      PrependItem(&list,rp->item,NULL);
      }
   DeleteRlist(classList);
   IdempPrependItem(&list,"any",NULL);
   }

if (list)
   {
   list = SortItemListNames(list);
   
   snprintf(buffer,bufsize,"<ul>\n");
   
   for (ip = list; ip != NULL; ip=ip->next)
      {
      snprintf(context,CF_MAXVARSIZE,"class_contexts::%s",ip->name);
      pid = Nova_GetPidForTopic(context);

      snprintf(work,CF_MAXVARSIZE,"<li><a href=\"knowledge.php?pid=%d\"><span id=\"classcontext\">%s</span></a></li>",pid,ip->name);
      Join(buffer,work,bufsize);
      }
   strcat(buffer,"</ul>");
   DeleteItemList(list);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_get_args_for_bundle(char *name,char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
  struct Rlist *classList;
  struct Item *matched,*ip;
  char work[CF_MAXVARSIZE];
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

matched = CFDB_QueryBundleArgs(&dbconn,type,name);

if (matched)
   {
   snprintf(buffer,bufsize,"<ul>\n");
   
   for (ip = matched; ip != NULL; ip=ip->next)
      {
      snprintf(work,CF_MAXVARSIZE,"<li><span id=\"args\">%s</span></li>",ip->name);
      Join(buffer,work,bufsize);
      }

   strcat(buffer,"</ul>\n");
   DeleteItemList(matched);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_list_all_bundles(char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
  char work[CF_BUFSIZE];
  struct Item *matched,*ip;

Nova_WebTopicMap_Initialize();

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

matched = CFDB_QueryBundles(&dbconn,type,NULL);
matched = SortItemListClasses(matched);

if (matched)
   {
   if (type)
      {
      snprintf(buffer,bufsize,"<div id=\"bundles\"><table>\n<tr><th>Type</th><th>Service bundle name</th><th>Contributing to goals</th><th></th></tr>\n");      }
   else
      {
      snprintf(buffer,bufsize,"<div id=\"bundles\"><ul>\n");
      }
   
   for (ip = matched; ip != NULL; ip=ip->next)
      {
      struct Item *ip2,*glist = Nova_GetBusinessGoals(ip->name);
      char goals[CF_BUFSIZE];
      char colour[CF_SMALLBUF];
      
      if (type && glist)
         {
         snprintf(goals,sizeof(goals),"<table>");
         for (ip2 = glist; ip2 != NULL; ip2=ip2->next)
            {
            snprintf(work,sizeof(work),"<tr><td><a href=\"knowledge.php?pid=%d\">%s</a> </td><td> %s</td></tr>",ip2->counter,ip2->name,ip2->classes);
            Join(goals,work,CF_BUFSIZE);
            }

         strcat(goals,"</table>");
         snprintf(colour,CF_SMALLBUF,"/img/green.png");
         }
      else if (type)
         {
         snprintf(goals,CF_MAXVARSIZE,"Does not promise to align with any goals");
         snprintf(colour,CF_SMALLBUF,"/img/yellow.png");
         }

      if (type)
         {
         snprintf(work,CF_BUFSIZE,"<tr><td><a href=\"bundle.php?type=%s\"><span id=\"bundletype\">%s</span></a></td><td><a href=\"bundle.php?bundle=%s&type=%s\"><span id=\"bundle\">%s</span></a></td><td>%s</td><td><img src=\"%s\"></td></tr>",ip->classes,ip->classes,ip->name,ip->classes,ip->name,goals,colour);
         }
      else
         {
         snprintf(work,CF_BUFSIZE,"<li><a href=\"bundle.php?type=%s\"><span id=\"bundletype\">%s</span></a> <a href=\"bundle.php?bundle=%s&type=%s\"><span id=\"bundle\">%s</span></a></li>",ip->classes,ip->classes,ip->name,ip->classes,ip->name);
         }
      
      Join(buffer,work,bufsize);
      DeleteItemList(glist);
      }

   if (type)
      {
      strcat(buffer,"</table></div>\n");
      }
   else
      {
      strcat(buffer,"</ul></div>\n");
      }
   DeleteItemList(matched);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_list_bundles_using(char *name,char *buffer,int bufsize)

{ mongo_connection dbconn;
  char work[CF_MAXVARSIZE];
  struct Item *matched,*ip;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

matched = CFDB_QueryBundlesUsing(&dbconn,name);

matched = SortItemListClasses(matched);

if (matched)
   {
   snprintf(buffer,bufsize,"<div id=\"bundles\"><ul>\n");
   
   for (ip = matched; ip != NULL; ip=ip->next)
      {
      snprintf(work,CF_MAXVARSIZE,"<li><a href=\"bundle.php?type=%s\"><span id=\"bundletype\">%s</span></a> <a href=\"bundle.php?bundle=%s&type=%s\"><span id=\"bundle\">%s</span></a></li>",ip->classes,ip->classes,ip->name,ip->classes,ip->name);
      Join(buffer,work,bufsize);
      }

   strcat(buffer,"</ul></div>\n");
   DeleteItemList(matched);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_get_bundle_count(char *buffer, int bufsize)

{ mongo_connection dbconn;
  int bundleCount = 0;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }
 
 bundleCount = CFDB_QueryBundleCount(&dbconn);


if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

 snprintf(buffer,bufsize,"%d",bundleCount);

 return true;
}

/*****************************************************************************/

int Nova2PHP_get_promise_count(char *buffer, int bufsize)

{ mongo_connection dbconn;
  int promiseCount = 0;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }
 
 promiseCount = CFDB_QueryPromiseCount(&dbconn);


if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

 snprintf(buffer,bufsize,"%d",promiseCount);

 return true;
}


/*****************************************************************************/
/* Topic Map                                                                 */
/*****************************************************************************/

int Nova2PHP_docroot(char *buffer,int bufsize)
{
Nova_WebTopicMap_Initialize();
snprintf(buffer,bufsize,"%s",DOCROOT);
return true;
}

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

buffer[0] = '\0';
Nova_WebTopicMap_Initialize();

if (Nova_GetTopicByPid(id,topic_name,topic_id,topic_type,topic_comment))
   {
   snprintf(buffer,bufsize,"<div id=\"topic\">\n'<span id=\"subject\">%s</span>' in section `<span id=\"category\">%s</span>:<p>\"%s\"</div>",topic_name,topic_type,topic_comment);
   }
else
   {
   snprintf(buffer,bufsize,"No such topic\n");
   }
}

/*****************************************************************************/

void Nova2PHP_show_topic_leads(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '\0';
Nova_ScanLeadsAssociations(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_show_topic_hits(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '\0';
Nova_ScanOccurrences(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_show_topic_category(int id,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
buffer[0] = '\0';
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

Nova_WebTopicMap_Initialize();
  
for (pol = 0; policies[pol] != NULL; pol++)
   {
   if (strcmp(policy,policies[pol]) == 0)
      {
      break;
      }
   }

clist = Nova_RankHosts(NULL,0,pol,n);

buffer[0] = '\0';
strcat(buffer,"<table>\n\n\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   Nova_Meter(DOCROOT,ip->name);

   if (Nova_IsGreen(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><a href=\"hosts.php?type=green\"><img src=\"green.png\"></a></td><td><a href=\"host.php?hostkey=%s\">%s</a></td><td><a href=\"host.php?hostkey=%s\"><img src=\"/hub/%s/meter.png\"></a></td><td>%s</td></a></td></tr>\n",ip->name,ip->classes,ip->name,ip->name,Nova_HostProfile(ip->name));
      }
   else if (Nova_IsYellow(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><a href=\"hosts.php?type=green\"><img src=\"yellow.png\"></a></td><td><a href=\"host.php?hostkey=%s\">%s</a></td><td><a href=\"host.php?hostkey=%s\"><img src=\"/hub/%s/meter.png\"></a></td><td>%s</td></a></td></tr>\n",ip->name,ip->classes,ip->name,ip->name,Nova_HostProfile(ip->name));
      }
   else // if (Nova_IsRed(ip->counter))
      {
      snprintf(work,CF_MAXVARSIZE,"<tr><td><a href=\"hosts.php?type=red\"><img src=\"red.png\"></a></td><td><a href=\"host.php?hostkey=%s\">%s</a></td><td><a href=\"host.php?hostkey=%s\"><img src=\"/hub/%s/meter.png\"></a></td><td>%s</td></a></td></tr>\n",ip->name,ip->classes,ip->name,ip->name,Nova_HostProfile(ip->name));
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

Nova_WebTopicMap_Initialize();  
clist = Nova_RankHosts(policy,1,cfrank_compliance,n);
clist = SortItemListClasses(clist);

buffer[0] = '\0';
strcat(buffer,"<table>\n\n\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   Nova_Meter(DOCROOT,ip->name);

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

if (clist)
   {
   buffer[0] = '\0';
   strcat(buffer,"<table>\n<tr>\n");
   
   for (ip = clist; ip !=  NULL; ip=ip->next, counter++)
      {
      if (counter > 0 && (counter % 6 == 0))  // new row
         {
         snprintf(work,CF_MAXVARSIZE,"</tr>\n<tr>\n");
         Join(buffer,work,bufsize);
         }
      
      snprintf(work,CF_MAXVARSIZE,"<td><img src=\"img/%s.png\"><a href=\"host.php?hostkey=%s\">%s</a></td>\n",
	       colour,ip->name,ip->classes,Nova_HostProfile(ip->name));
      
      Join(buffer,work,bufsize);
      
      if (counter > n && (counter % 6 == 0))
         {
         break;
         }

      }

   Join(buffer,"</tr>\n</table>\n",bufsize);
   DeleteItemList(clist);
   }
}

/*****************************************************************************/

void Nova2PHP_select_hosts(char *match,char *selected,int n,char *buffer,int bufsize)

{ struct Item *ip,*clist;
  char work[CF_MAXVARSIZE];

clist = Nova_RankHosts(match,1,cfrank_compliance,n);
clist = SortItemListClasses(clist);

buffer[0] = '\0';
strcat(buffer,"<select name=\"hostkey\" onselect>\n");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   if (strcmp(selected,ip->name) == 0)
      {
      snprintf(work,CF_MAXVARSIZE,"<option value=\"%s\" selected>%s</option>\n",ip->name,ip->classes);
      }
   else
      {
      snprintf(work,CF_MAXVARSIZE,"<option value=\"%s\">%s</option>\n",ip->name,ip->classes);
      }
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

{ int score = Nova_GetHostColour(hostkey);

if (Nova_IsYellow(score))
   {
   strncpy(buffer,"yellow",bufsize);
   }
else if (Nova_IsGreen(score))
   {
   strncpy(buffer,"green",bufsize);
   }
else
   {
   strncpy(buffer,"red",bufsize);
   }
}

/*****************************************************************************/

char *Nova_HostProfile(char *key)

{ static char buffer[CF_BUFSIZE];

snprintf(buffer,CF_BUFSIZE,
         "<table><tr><td><a href=\"bundles.php?host=%s\">Bundles</a></td><td><a href=\"classes.php?host=%s\">Classes</a></td></tr>"
         "<tr><td><a href=\"knowledge.php?topic=goals\">Goals</a></td><td><a href=\"promises.php?host=%s\">Promises</a></td></tr></table>",key,key,key);

return buffer;
}

/*****************************************************************************/

char *Nova2PHP_GetPromiseComment(char *handle)
    
{ static char buffer[CF_BUFSIZE];
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (CFDB_QueryPromiseAttr(&dbconn,handle,cfp_comment,buffer,CF_BUFSIZE))
   {
   return buffer;
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return "No comment";
}

/*****************************************************************************/

char *Nova2PHP_GetPromiseBundle(char *handle)
    
{ static char buffer[CF_BUFSIZE];
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (CFDB_QueryPromiseAttr(&dbconn,handle,cfp_bundlename,buffer,CF_BUFSIZE))
   {
   return buffer;
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return "No such promise";
}

/*****************************************************************************/

void Nova2PHP_GetPromiseBody(char *name,char *type,char *returnval,int bufsize)
    
{ char work[CF_BUFSIZE];
  mongo_connection dbconn;
  struct HubBody *hb;    
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return;
   }

hb = CFDB_QueryBody(&dbconn,type,name);

if (hb)
   {
   snprintf(returnval,CF_MAXVARSIZE-1,"<div id=\"showbody\"><table>\n");
   
   snprintf(work,CF_MAXVARSIZE-1,"<tr><td>Type</td><td>:</td><td><a href=\"knowledge.php?topic=%s\">%s</a></td><td></td></tr>\n",hb->bodyType,hb->bodyType);
   Join(returnval,work,bufsize);

   snprintf(work,CF_MAXVARSIZE-1,"<tr><td>Name</td><td>:</td><td><a href=\"knowledge.php?topic=%s\">%s</a></td><td></td></tr>\n",hb->bodyName,hb->bodyName);
   Join(returnval,work,bufsize);
         
   if (hb->bodyArgs)
      {
      snprintf(work,CF_MAXVARSIZE-1,"<tr><td>Arguments</td><td>:</td><td>%s</td><td></td></tr>\n",hb->bodyArgs);
      Join(returnval,work,bufsize);
      }
   
   if (hb->attr)
      {
      struct HubBodyAttr *ha; 
      
      for (ha = hb->attr; ha != NULL; ha = ha->next)
         {
         snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"right\"><span id=\"lval\"><a href=\"knowledge.php?topic=%s\">%s</a></span></td><td>=></td><td><span id=\"rval\">%s</span></td><td><a href=\"knowledge.php?topic=%s\">%s</a></td></tr>",ha->lval,ha->lval,ha->rval,ha->classContext,ha->classContext);
         }
      Join(returnval,work,bufsize);      
      }
   
   DeleteHubBody(hb);
   strcat(returnval,"</table></div>\n");
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }
}

/*****************************************************************************/

int Nova2PHP_list_bodies(char *name,char *type,char *returnval,int bufsize)

{ mongo_connection dbconn;
  char work[CF_MAXVARSIZE];
  struct Item *all_bodies,*ip;    

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

all_bodies = CFDB_QueryAllBodies(&dbconn,type,name);
all_bodies = SortItemListNames(all_bodies);

if (all_bodies)
   {
   snprintf(returnval,CF_MAXVARSIZE-1,"<div id=\"bodies\"><ul>\n");
      
   for (ip = all_bodies; ip != NULL; ip=ip->next)
      {
      snprintf(work,CF_MAXVARSIZE-1,"<li><a href=\"knowledge.php?topic=%s\">%s</a> ",ip->classes,ip->classes);
      Join(returnval,work,bufsize);
      
      snprintf(work,CF_MAXVARSIZE-1,"<a href=\"body.php?body=%s&type=%s\">%s</a></li>\n",ip->name,ip->classes,ip->name);
      Join(returnval,work,bufsize);
      }
   
   strcat(returnval,"</ul></div>\n");
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

char *Nova2PHP_GetPromiseType(char *handle)
    
{ static char buffer[CF_BUFSIZE];
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (CFDB_QueryPromiseAttr(&dbconn,handle,cfp_promisetype,buffer,CF_BUFSIZE))
   {
   return buffer;
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return "No such promise";
}

/*****************************************************************************/

char *Nova2PHP_GetPromiser(char *handle)
    
{ static char buffer[CF_BUFSIZE];
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (CFDB_QueryPromiseAttr(&dbconn,handle,cfp_promiser,buffer,CF_BUFSIZE))
   {
   return buffer;
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return "No such promise";
}

/*****************************************************************************/

int Nova2PHP_get_variable(char *hostkey,char *scope,char *lval,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],lscope[CF_MAXVARSIZE];
  struct HubVariable *hv,*hv2;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, n = 180;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,hostkey);
bson_from_buffer(&query,&bb);
hq = CFDB_QueryVariables(&dbconn,&query,scope,lval,NULL,NULL,false);
bson_destroy(&query);

lscope[0] = '\0';
returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (struct HubVariable *)rp->item;

   if (strlen(hv->dtype) > 1) // list
      {
      char b[CF_BUFSIZE];
      b[0] = '\0';
      PrintRlist(b,CF_BUFSIZE,hv->rval);
      snprintf(returnval,bufsize-1,"%s",b);
      }
   else
      {
      snprintf(returnval,bufsize-1,"%s",(char *)hv->rval);
      }
   }

if (hq->records == NULL)
   {
   snprintf(buffer,sizeof(buffer),"Unknown value");
   }

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
/* Reports                                                                   */
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
  char work[CF_MAXVARSIZE];
  int i,count;
  

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hp = CFDB_QueryPromise(&dbconn, handle);
 
if (!hp)
   {
   snprintf(returnval, bufsize, "<br>Promise '%s' not found in database", handle);
   return false;
   }

returnval[0] = '\0';

strcat(returnval,"<div id=\"promise\"><table>\n");

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\" width=\"20%\">Belonging to <span id=\"bundletype\">%s</span> bundle</td><td>:</td><td><a href=\"bundle.php?bundle=%s&type=%s\"><span id=\"bundle\">%s</span></a><td></tr>",hp->bundleType,hp->bundleName,hp->bundleType,hp->bundleName);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Reference handle</td><td>:</td><td><a href=\"knowledge.php?topic=%s\"><span id=\"handle\">%s</span></a></td></tr>",hp->handle,hp->handle);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Affected object (promiser)</td><td>:</td><td><a href=\"knowledge.php?topic=%s\"><span id=\"promiser\">%s</span></a></td></tr>",hp->promiser,hp->promiser);
Join(returnval,work,bufsize);

if (EMPTY(hp->promisee))
   {
   strcpy(promiseeText,"None mentioned");
   }
else
   {
   snprintf(promiseeText,sizeof(promiseeText),"<a href=\"knowledge.php?topic=%s\">%s</a>",hp->promisee,hp->promisee);     
   }

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Stakeholders (promisees)</td><td>:</td><td><span id=\"promisee\">%s</span></td></tr>",promiseeText);
Join(returnval,work,bufsize);

if (EMPTY(hp->comment))
   {
   strcpy(commentText,"No comment");
   }
else
   {
   snprintf(commentText, sizeof(commentText),"%s",hp->comment);
   }

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Comment on original intention</td><td>:</td><td><span id=\"promiser\">%s</span></td></tr>",commentText);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Promise is about</td><td>:</td><td><a href=\"knowledge.php?topic=%s\"><span id=\"subtype\">%s</span></a></td></tr>",hp->promiseType,hp->promiseType);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Applies in the class context</td><td>:</td><td><a href=\"knowledge.php?topic=%s\"><span id=\"classcontext\">%s</span></a></td></tr>",hp->classContext,hp->classContext);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Defined in file</td><td>:</td><td><span id=\"file\">%s</span> near line %d</td></tr>",hp->file,hp->lineNo);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Body of the promise</td><td>:</td><td></td></tr>");
Join(returnval,work,bufsize);

constText[0] = '\0';

if (hp->constraints)
   {
   for(i = 0; hp->constraints[i] != NULL; i++)
      {
      char lval[CF_MAXVARSIZE],rval[CF_MAXVARSIZE],args[CF_MAXVARSIZE];

      args[0] = '\0';
      sscanf(hp->constraints[i],"%255s => %1023[^(,;]%[^\n]",lval,rval,args);

      
      snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"right\"><span id=\"lval\"><a href=\"knowledge.php?topic=%s\">%s</a></span></td><td>=></td><td><a href=\"body.php?body=%s&type=%s\"><span id=\"bodyname\">%s</span>%s</a></td></tr>",lval,lval,rval,lval,rval,args);
      
      Join(returnval,work,bufsize);   
      }

   if (i == 0)
      {
      Join(returnval,"body content is implicit, no futher details",bufsize);
      }
   }

strcat(returnval,"</table></div>\n");
    
DeleteHubPromise(hp);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_list_promise_handles(char *promiser,char *ptype,char *bundle,char *btype,int regex,char *returnval,int bufsize)

{ mongo_connection dbconn;
  char promiseeText[CF_MAXVARSIZE],bArgText[CF_MAXVARSIZE];
  char commentText[CF_MAXVARSIZE], constText[CF_MAXVARSIZE];
  char work[CF_MAXVARSIZE];
  struct Rlist *rp,*handles;
  int i,count;
  
/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

handles = CFDB_QueryPromiseHandles(&dbconn,promiser,ptype,btype,bundle,regex);

returnval[0] = '\0';

strcat(returnval,"<div id=\"promise\"><ul>\n");

for (rp = handles; rp != NULL; rp = rp->next)
   {
   snprintf(work,CF_MAXVARSIZE-1,"<li><a href=\"promise.php?handle=%s\">%s</a></li>",rp->item,rp->item);
   Join(returnval,work,bufsize);
   }

strcat(returnval,"</ul></div>\n");

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

DeleteRlist(handles);

return true;
}

/*****************************************************************************/

void Nova2PHP_get_network_speed(char *hostkey,char *buffer, int bufsize)

{ 
#ifdef HAVE_LIBMONGOC

  mongo_connection dbconn;
  mongo_cursor *cursor;
  bson_iterator it1;
  bson_buffer bb;
  bson query,field;
  int found = false;
  struct Event e;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to database (read network measurements)");
   return;
   }

// query

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,hostkey);
bson_from_buffer(&query,&bb);

// returned value

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_netmeasure,1);
bson_from_buffer(&field,&bb);

cursor = mongo_find(&dbconn,MONGO_DATABASE,&query,&field,0,0,0);
bson_destroy(&query);
bson_destroy(&field);

if (mongo_cursor_next(cursor))  // not more than one record
   {
   bson_iterator it;
   bson_iterator_init(&it, cursor->current.data);
   
   while(bson_iterator_next(&it))
      {
      if (strcmp(bson_iterator_key(&it),cfr_netmeasure) == 0)
	 {
         if (bson_iterator_bin_len(&it) == sizeof(e))
            {
            memcpy(&e,bson_iterator_bin_data(&it),sizeof(e));
            found = true;
            }
         else
            {
            CfOut(cf_verbose, "", "!! Existing network measurement incorrect - ignoring");
            }
	 }
      }
   }
 
mongo_cursor_destroy(cursor);

CFDB_Close(&dbconn);

if (found)
   {
   snprintf(buffer,bufsize,"%.2lf &Delta; %.2lf bytes/s",e.Q.expect,sqrt(e.Q.var));
   }
else
   {
   snprintf(buffer,bufsize,"Insufficient data");
   }

#endif /* HAVE_LIBMONGOC */
}

/*****************************************************************************/

int Nova2PHP_countclasses(char *hostkey,char *name,int regex,char *returnval,int bufsize)

{ char *report,work[CF_BUFSIZE];
  struct HubHost *hh;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  struct Item *order_results = NULL,*ip;
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


 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,CF_HUB_HORIZON,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   struct HubClass *hc = (struct HubClass *)rp->item;
   IdempItemCount(&order_results,hc->class,NULL);
   }

snprintf(returnval,bufsize,"<table>");

/* Make a histogram of job vs number of hosts */

order_results = SortItemListNames(order_results);

for (ip = order_results; ip != NULL; ip = ip->next)
   {   
   snprintf(work,CF_BUFSIZE,"<tr><td>%s</td><td>%d</td></tr>",ip->name+strlen(MONITOR_CLASS_PREFIX),ip->counter+1);
   Join(returnval,work,bufsize);
   count += ip->counter+1;
   }

if (count == 0)
   {
   snprintf(work,CF_BUFSIZE,"<tr><td>No occurrences</td></tr>");
   Join(returnval,work,bufsize);
   }

strcat(returnval,"</table>");
DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return count;
}

/*****************************************************************************/

char *Nova_LongState(char s)

{
 switch (s)
    {
    case 'c':
        return "Compliant";
    case 'r':
        return "Repaired";
    case 'n':
    default:
        return "Not Compliant";
    }
}

/*****************************************************************************/

void Nova2PHP_AnalyseMag(char *hostkey,enum observables obs,char *buffer,int bufsize)
{
Nova_WebTopicMap_Initialize();
Nova_AnalyseMag(DOCROOT,hostkey,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_AnalyseWeek(char *keyhash,enum observables obs,char *buffer,int bufsize)
{
Nova_WebTopicMap_Initialize();
Nova_AnalyseWeek(DOCROOT,keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_ComplianceSummaryGraph()
{
Nova_WebTopicMap_Initialize();
Nova_ComplianceSummaryGraph(DOCROOT);
}

/*****************************************************************************/

void Nova2PHP_AnalyseHistogram(char *keyhash,int obs,char *buffer,int bufsize)
{
Nova_WebTopicMap_Initialize();
Nova_AnalyseHistogram(DOCROOT,keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

char *Nova_FormatDiff(char *s)

{ char *sp,work[CF_BUFSIZE],diff[CF_BUFSIZE],tline[CF_BUFSIZE];
  static char returnval[CF_BUFSIZE];
  char pm;
  int line = 0;

snprintf(returnval,sizeof(returnval),"<table>");

for (sp = s; *sp != '\0'; sp += strlen(tline)+1)
   {
   sscanf(sp,"%c,%d,%2047[^\n]",&pm,&line,diff);
   sscanf(sp,"%2047[^\n]",tline);
   
   switch (pm)
      {
      case '+':
          snprintf(work,sizeof(work),"<tr><td><span id=\"pm\">%c</span></td><td>%d</td><td><span id=\"plusline\">%s</span></td><tr>",pm,line,diff);
          break;
      case '-':
          snprintf(work,sizeof(work),"<tr><td><span id=\"pm\">%c</span></td><td>%d</td><td><span id=\"minusline\">%s</span></td><tr>",pm,line,diff);
          break;
      default:
          snprintf(work,sizeof(work),"<tr><td><span id=\"pm\">%c</span></td><td>%d</td><td>%s</td><tr>",pm,line,diff);
          break;
      }
   
   Join(returnval,work,sizeof(returnval));
   }

strcat(returnval,"</table>");
return returnval;
}
/*****************************************************************************/
/* pdf sub routines */
/*****************************************************************************/
int Nova2PHP_compliance_report_pdf(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubTotalCompliance *ht;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;


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


 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%d<nc>%d<nc>%d<nc>%s<nova_nl>",
            ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubTotalCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}
/*****************************************************************************/
/*  */
/*****************************************************************************/

int Nova2PHP_promiselog_pdf(char *hostkey,char *handle,enum promiselog_rep type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseLog *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,true,NULL);

returnval[0] = '\0';
         
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",
            hp->hh->hostname,hp->handle,hp->cause,cf_ctime(&(hp->t)));
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubPromiseLog);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
int Nova2PHP_bundle_report_pdf(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize,char *classRegex)

{ char *report,buffer[CF_BUFSIZE];
  struct HubBundleSeen *hb;   
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classRegex);

returnval[0] = '\0';

//strcat(returnval,"<table>\n");

//snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Bundle</a></th><th>Last verified</th><th>Hours Ago</th><th>Avg interval</th><th>Uncertainty</th></tr>\n");
//Join(returnval,buffer,bufsize);
   
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = ( struct HubBundleSeen *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%.2lf<nova_nl>",
            hb->hh->hostname,hb->bundle,cf_ctime(&(hb->t)),
            hb->hrsago,hb->hrsavg,hb->hrsdev);

   Join(returnval,buffer,bufsize);
   }

//strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubBundleSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}
/*****************************************************************************/

int Nova2PHP_value_report_pdf(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubValue *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%.1lf<nc>%.1lf<nc>%.1lf<nova_nl>",
            hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept);
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubValue);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
int Nova2PHP_classes_report_pdf(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubClass *hc;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;
  time_t now = time(NULL);

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,NULL);

returnval[0] = '\0';

//strcat(returnval,"<table>\n");

//snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Class context</th><th>Occurs with<br>Probability</th><th>Uncertainty</th><th>Last seen</th></tr>\n");
//Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
  // TODO: this if else was used to give different colors to the 3rd col 
   if (now - hc->t > 6*3600)
      {
      snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%lf<nc>%lf<nc>%s<nova_nl>",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   else if (now - hc->t > (time_t)CF_WEEK)
      {
      snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%lf<nc>%lf<nc>%s<nova_nl>",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   else
      {
      snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%lf<nc>%lf<nc>%s<nova_nl>",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   
   Join(returnval,buffer,bufsize);
   }

//strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_promises_pdf(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPromiseCompliance *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,NULL);


returnval[0] = '\0';

//strcat(returnval,"<table>\n");
//snprintf(buffer,sizeof(buffer),"<tr><th>host</th><th>promise handle</th><th>Last known<br>state</th><th>probability<br>kept</th><th>uncertainty</th><th>last seen</th></tr>\n");
//Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;
   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%s<nova_nl>",
            hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_ctime(&(hp->t)));
   Join(returnval,buffer,bufsize);
   }

//strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_report_pdf(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubLastSeen *hl;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int tmpsize,icmp;
  mongo_connection dbconn;
  char inout[CF_SMALLBUF];
  time_t then;

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (struct HubLastSeen *)rp->item;

   switch (hl->io)
      {
      case '+':
          snprintf(inout,CF_SMALLBUF,"by us (+)");
          break;
      case '-':
          snprintf(inout,CF_SMALLBUF,"by them (-)");
          break;
      }

   then = hl->t;
   
   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%.2lf<nc>%s<nova_nl>",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,cf_ctime(&then),
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
          
   Join(returnval,buffer,bufsize);
   }


DeleteHubQuery(hq,DeleteHubLastSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_software_report_pdf(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubSoftware *hs;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,NULL);


returnval[0] = '\0';

      
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));
   Join(returnval,buffer,bufsize);
   }


DeleteHubQuery(hq,DeleteHubSoftware);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}
/*****************************************************************************/

int Nova2PHP_performance_report_pdf(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubPerformance *hP;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hP = ( struct HubPerformance *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%.2lf<nc>%s<nova_nl>",
            hP->hh->hostname,
            hP->event,hP->q,hP->e,hP->d,cf_ctime(&(hP->t)));
   
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

int Nova2PHP_promiselog_summary_pdf(char *hostkey,char *handle,enum promiselog_rep type,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],hostname[CF_MAXVARSIZE];
  struct HubPromiseLog *hp;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  struct Item *ip,*summary = NULL;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,false,NULL);

hostname[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;
   IdempPrependItem(&summary,hp->handle,hp->cause);
   IncrementItemListCounter(summary,hp->handle);

   if (hostname[0] == '\0')
      {
      strncpy(hostname,hp->hh->hostname,CF_MAXVARSIZE);
      }
   }

DeleteHubQuery(hq,DeleteHubPromiseLog);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

if (summary == NULL)
   {
   snprintf(returnval,bufsize,"No data to report on");
   }
else
   {
   returnval[0] = '\0';
   strcat(returnval,"<table>\n");

   summary = SortItemListCounters(summary);
  // snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Promise handle</th><th>Report</th><th>Occurrences</th></tr>\n");
//   Join(returnval,buffer,bufsize);
            
   for (ip = summary; ip != NULL; ip=ip->next)
      {
      snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%d<nova_nl>",hostname,ip->name,ip->classes,ip->counter);

      Join(returnval,buffer,bufsize);
      }
   
//   strcat(returnval,"</table>\n");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_vars_report_pdf(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],lscope[CF_MAXVARSIZE];
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

lscope[0] = '\0';
returnval[0] = '\0';

//strcat(returnval,"<table>\n");
count += strlen(returnval);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (struct HubVariable *)rp->item;

   if (strcmp(lscope,hv->scope) != 0)
      {
      strcpy(lscope,hv->scope);
      snprintf(buffer,CF_BUFSIZE,"<nh>Bundle scope %s<nova_nl>",hv->scope);
      Join(returnval,buffer,bufsize);
      snprintf(buffer,CF_BUFSIZE,"<nh>host<nc>type<nc>name<nc>value<nova_nl>");
      Join(returnval,buffer,bufsize);
      }

   switch (*hv->dtype)
      {
      case 's':
          snprintf(typestr,CF_SMALLBUF,"string");
          break;
      case 'i':
          snprintf(typestr,CF_SMALLBUF,"int");
          break;
      case 'r':
          snprintf(typestr,CF_SMALLBUF,"real");
          break;
      case 'm':
          snprintf(typestr,CF_SMALLBUF,"menu");
          break;
      }

   if (strlen(hv->dtype) == 2)
      {
      strcat(typestr," list");
      }
   
   snprintf(buffer,CF_BUFSIZE,"%s<nc>%s<nc>%s<nc>",hv->hh->hostname,typestr,hv->lval);
   strcat(returnval,buffer);
   count += strlen(buffer);

   if (strlen(hv->dtype) > 1) // list
      {
      char b[CF_BUFSIZE];
      b[0] = '\0';
      PrintRlist(b,CF_BUFSIZE,hv->rval);
      snprintf(buffer,sizeof(buffer),"%s<nova_nl>",b);
      }
   else
      {
      snprintf(buffer,sizeof(buffer),"%s<nova_nl>",(char *)hv->rval);
      }

   Join(returnval,buffer,bufsize);
   }

//strcat(returnval,"</table>\n");

//DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_report_description(char *reportName,char *returnval,int bufsize)
{
  int pid;
  char topic_name[CF_BUFSIZE] = {0};
  char topic_id[CF_BUFSIZE] = {0};
  char topic_type[CF_BUFSIZE] = {0};
  char topic_comment[CF_BUFSIZE] = {0};
  char typedName[CF_MAXVARSIZE] = {0};

  snprintf(typedName,sizeof(typedName),"system_reports::%s",reportName);

  pid = Nova_GetPidForTopic(typedName);

  if(pid && Nova_GetTopicByPid(pid,topic_name,topic_id,topic_type,topic_comment))
    {
      if(EMPTY(topic_comment))
	{
	  snprintf(returnval,bufsize,"(no comment for %s found in topic map)",reportName);
	  return false;
	}

      snprintf(returnval,bufsize,"%s",topic_comment);
      
      return true;
    }


  snprintf(returnval,bufsize,"(no description for %s found in topic map)",reportName);
  return false;

}


/*****************************************************************************/

int Nova2PHP_filechanges_report_pdf(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubFileChanges *hC;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,NULL);

returnval[0] = '\0';

   
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hC = (struct HubFileChanges *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nova_nl>",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)));
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubFileChanges);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filediffs_report_pdf(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];   
  struct HubFileDiff *hd;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;

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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,NULL);

returnval[0] = '\0';

snprintf(buffer,sizeof(buffer),"<nh>host<nc>file<nc>change detected at<nc>change<nova_nl>");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),Nova_FormatDiff_pdf(hd->diff));
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubFileDiff);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

char *Nova_FormatDiff_pdf(char *s)

{ char *sp,work[CF_BUFSIZE],diff[CF_BUFSIZE],tline[CF_BUFSIZE];
  static char returnval[CF_BUFSIZE];
  char pm;
  int line = 0;

snprintf(returnval,sizeof(returnval),"");

for (sp = s; *sp != '\0'; sp += strlen(tline)+1)
   {
   sscanf(sp,"%c,%d,%2047[^\n]",&pm,&line,diff);
   sscanf(sp,"%2047[^\n]",tline);
   
   switch (pm)
      {
      case '+':
          snprintf(work,sizeof(work),"%c  %d  %s\n",pm,line,diff);
          break;
      case '-':
          snprintf(work,sizeof(work),"%c  %d  %s\n",pm,line,diff);
          break;
      default:
          snprintf(work,sizeof(work),"%c  %d  %s\n",pm,line,diff);
          break;
      }
   
   Join(returnval,work,sizeof(returnval));
   }

//strcat(returnval,"</table>");
return returnval;
}
/*****************************************************************************/
int Nova2PHP_setuid_report_pdf(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize)

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

snprintf(buffer,sizeof(buffer),"<nh>host<nc>setuid/setgid root file<nova_nl>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( struct HubSetUid *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nova_nl>",hS->hh->hostname,hS->path);
   Join(returnval,buffer,bufsize);
   }

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}
/* pdf reports end*/

/*****************************************************************************/
/*                       Special Purpose Policies                            */
/*****************************************************************************/

void Nova2PHP_spp_reportnames(char *buf,int bufSz)

{ int i;
  char work[CF_SMALLBUF];

buf[0] = '\0';
Join(buf,"<select name=\"spp_report\">\n",bufSz);

for (i = 0; SPP_REPORTS[i] != NULL; i++)
   {
   snprintf(work,sizeof(work),"<option value=\"%s\">%s</option>\n",SPP_REPORTS[i],SPP_REPORTS[i]);
   Join(buf,work,bufSz);
   }

Join(buf,"\n</select>\n",bufSz);
}

/*****************************************************************************/

spp_t SppReportNameToType(char *reportName)
{
  
  if(strcmp(reportName,"ACLs") == 0)
    {
    return spp_acls;
    }
  else if(strcmp(reportName,"Commands") == 0)
    {
    return spp_commands;
    }
  else if(strcmp(reportName,"File Changes") == 0)
    {
    return spp_filechanges;
    }
  else if(strcmp(reportName,"File Diffs") == 0)
    {
    return spp_filediffs;
    }
  else if(strcmp(reportName,"Registry") == 0)
    {
    return spp_registry;
    }
  else if(strcmp(reportName,"Services") == 0)
    {
    return spp_services;
    }
  else
    {
    return spp_unknown;
    }
}

/*****************************************************************************/

int Nova2PHP_spp_report(char *hostkey, char *reportName, char *buf, int bufSz)
{
  struct Item *promises = {0}, *hosts = {0};
  struct Item *ip = {0}, *ip2 = {0};
  mongo_connection dbconn;
  char handle[CF_MAXVARSIZE] = {0};
  char action[CF_MAXVARSIZE] = {0};
  char host[CF_MAXVARSIZE] = {0};
  char hostKeyHash[CF_MAXVARSIZE] = {0};
  char cause[CF_MAXVARSIZE] = {0};
  char statusStr[CF_SMALLBUF];
  char time[CF_MAXVARSIZE] = {0};
  char attributes[CF_MAXVARSIZE] = {0};
  char row[CF_MAXVARSIZE] = {0};
  int ret = false;
  spp_t sppType;

  memset(buf,0,bufSz);


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 sppType = SppReportNameToType(reportName);

 switch(sppType)
   {
   case spp_acls:
     promises = CFDB_QuerySppAcls(&dbconn,"</td><td>");
     break;

   case spp_commands:
     promises = CFDB_QuerySppCommands(&dbconn,"</td><td>");
     break;

   case spp_services:
     promises = CFDB_QuerySppServices(&dbconn,"</td><td>");
     break;

   case spp_filechanges:
     promises = CFDB_QuerySppPromiser(&dbconn,"</td><td>",cfp_spp_bundle_filechanges,"files");
     break;

   case spp_filediffs:
     promises = CFDB_QuerySppPromiser(&dbconn,"</td><td>",cfp_spp_bundle_filediffs,"files");
     break;

   case spp_registry:
     promises = CFDB_QuerySppRegistry(&dbconn,"</td><td>");
     break;

   default:
     snprintf(buf,bufSz,"Unknown Special Purpose Policy Type");
     CFDB_Close(&dbconn);
     return false;
   }
   
 if(promises)
   {
     snprintf(buf,bufSz,"\n<table>\n");
     Join(buf,GetSppTableHeader(sppType),bufSz);
     
     for(ip = promises; ip != NULL; ip = ip->next)
       {
	 sscanf(ip->name,"%128[^<]</td><td>%512[^$]",handle,attributes);

	 hosts = CFDB_QuerySppCompliance(&dbconn,handle);

	 if(hosts)
	   {
	   
	   for(ip2 = hosts; ip2 != NULL; ip2 = ip2->next)
	     {
	       
	     sscanf(ip2->name,"%512[^;];%128[^;];%8[^;];%128[^$]",hostKeyHash,host,statusStr,time);

	     //CFDB_QueryStausCause(&dbconn,hostKeyHash,handle,*statusStr,cause,sizeof(cause));

	     snprintf(row,sizeof(row),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
		      host,attributes,Nova_LongState(*statusStr),time);
		   
	     Join(buf,row,bufSz);
	     }
	     
	   DeleteItemList(hosts);
	   }
       }

     Join(buf,"\n</table>\n",bufSz);
     
     DeleteItemList(promises);

     ret = true;
   }

 if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }


  return ret;
}

/*****************************************************************************/

char *GetSppTableHeader(spp_t sppType)
{
  switch(sppType)
    {
    case spp_acls:
      return "<tr><th>Host</th><th>Path</th><th>Permission (ACL)</th><th>Owner</th><th>Action</th><th>Class expression</th><th>State</th><th>Time checked</th></tr>";
    case spp_commands:
      return "<tr><th>Host</th><th>Command</th><th>Failclass</th><th>Action</th><th>Class expression</th><th>State</th><th>Time checked</th></tr>";
    case spp_filechanges:
    case spp_filediffs:
      return "<tr><th>Host</th><th>Path</th><th>Action</th><th>Class expression</th><th>State</th><th>Time checked</th></tr>";
    case spp_registry:
      return "<tr><th>Host</th><th>Key</th><th>Value</th><th>Action</th><th>Class expression</th><th>State</th><th>Time checked</th></tr>";
    case spp_services:
      return "<tr><th>Host</th><th>Service name</th><th>Runstatus</th><th>Action</th><th>Class expression</th><th>State</th><th>Time checked</th></tr>";
    }


  return "<tr>Undefined Special Policy Header</tr>";
}

/*****************************************************************************/
int Nova2PHP_validate_policy(char *file,char *buffer,int bufsize)

{
   char cmd[CF_BUFSIZE];
   char output[CF_EXPANDSIZE];

   snprintf(cmd,CF_BUFSIZE-1,"/var/cfengine/bin/cf-promises -f %s",file);
   output[0] = '\0';

   if(GetExecOutput(cmd,output,false))
     {
	if(output == NULL)
	  {
	     return 0;
	  }	
	else
	  {
	     snprintf(buffer,bufsize,"%s",output);
	     return 1;
	  }
     }
   else
     {
	return -1;
     }   
}
/*****************************************************************************/
#endif
