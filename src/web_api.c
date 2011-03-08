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

char *CDP_REPORTS[] =
   {
   "ACLs",
   "Commands",
   "File Changes",
   "File Diffs",
   "Registry",
   "Services",
   NULL
   };

/*****************************************************************************/

void Nova_EnterpriseModuleTrick()

{ char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c
 
if (false)
   {
     //   Nova2PHP_software_report(0,0,0,0,0,cfr_software,NULL,buffer,20);
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
   //   Nova2PHP_promiselog(NULL,NULL,1,0,0,NULL,buffer,1000);
   Nova2PHP_promises(NULL, NULL, NULL, 0);
   //   Nova2PHP_getlastupdate(NULL,buffer,10);

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
   Nova2PHP_meter(NULL,NULL,0);
   /* pdf functions */
   Nova2PHP_compliance_report_pdf(NULL,NULL,0,0,0,0,0,NULL,buffer,10000);
   Nova2PHP_promiselog_pdf(NULL,NULL,1,0,0,NULL,buffer,1000); 
   Nova2PHP_bundle_report_pdf(NULL,NULL,0,NULL,buffer,10000);
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
   /*
    * commenting
    */
   Nova2PHP_add_new_note(NULL,NULL, -1,NULL, -1, NULL);
   Nova2PHP_add_note(NULL,NULL,-1,NULL);
   Nova2PHP_get_notes(NULL,NULL, NULL,-1,-1,NULL,10000);
   Nova2PHP_get_host_noteid(NULL,NULL,1000);

   Nova2PHP_get_knowledge_view(0,NULL,NULL,999);
   }
}

/*****************************************************************************/
/* Helper functions                                                          */
/****************************************************************************/

void Nova2PHP_get_knowledge_view(int pid,char *view,char *buffer,int bufsize)

{
Nova_WebTopicMap_Initialize();
Nova_PlotTopicCosmos(pid,view,buffer,bufsize);
}

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
	snprintf(buffer,bufsize,"%ld",then);
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

void Nova2PHP_get_magnified_view(char *keyhash,enum observables obs,char *buffer,int bufsize)

{
Nova_ViewMag(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_weekly_view(char *keyhash,enum observables obs,char *buffer,int bufsize)
{
Nova_ViewWeek(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_yearly_view(char *keyhash,enum observables obs,char *buffer,int bufsize)

{
Nova_ViewLongHistory(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_histogram_view(char *keyhash,enum observables obs,char *buffer,int bufsize)

{
Nova_ViewHisto(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_magnified_analysis(char *keyhash,enum observables obs,char *buffer,int bufsize)
{
Nova_AnalyseMag(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_weekly_analysis(char *keyhash,enum observables obs,char *buffer,int bufsize)
{
Nova_AnalyseWeek(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_yearly_analysis(char *keyhash,enum observables obs,char *buffer,int bufsize)

{
Nova_AnalyseLongHistory(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_get_histogram_analysis(char *keyhash,enum observables obs,char *buffer,int bufsize)

{
Nova_AnalyseHistogram(keyhash,obs,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_summary_meter(char *buffer,int bufsize)

{
Nova_Meter(NULL,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_meter(char *hostkey,char *buffer,int bufsize)

{
Nova_Meter(hostkey,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_performance_analysis(char *hostkey,char *buffer,int bufsize)

{
Nova_PerformancePage(DOCROOT,hostkey,buffer,bufsize);
}

/*****************************************************************************/

char *Nova2PHP_get_observable_name(int obs,char *buffer,int bufsize)
{
snprintf(buffer,bufsize,"[ \"%s\", \"%s\"]",OBS[obs][0],OBS[obs][1]);
return buffer;
}

/*****************************************************************************/

int Nova2PHP_get_observable_id(char *name)

{ int i;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (strcmp(name,OBS[i][0]) == 0)
      {
      return i;
      }
   }

return -1;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2PHP_promiselog(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE], note[CF_MAXVARSIZE], note_link[CF_BUFSIZE];
  struct HubPromiseLog *hp;  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp, reportType;
  mongo_connection dbconn;
  
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,from,to,true,classreg);
 PageRecords(&(hq->records),page,DeleteHubPromiseLog);
 //StartJoin(returnval,"<table>\n",bufsize);
strcpy(returnval,"[");

//snprintf(buffer,sizeof(buffer), "<tr><th>Host</th><th>Promise handle</th><th>Report</th><th>Time</th><th>Notes</th></tr>\n");
Join(returnval,buffer,bufsize);
         
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;
   if (strcmp(hp->nid,CF_NONOTE) == 0)
      {
	switch (type)
	  {
	  case plog_repaired:
	    reportType = CFREPORT_REPAIRED;
	    break;
	  case plog_notkept:
	  default:
	    reportType = CFREPORT_NOTKEPT;
	    break;
	  }
	//	snprintf(note_link,sizeof(note_link),"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hp->hh->keyhash,reportType,hp->oid);
	//	snprintf(note,sizeof(note),"%s",CF_ADDNOTE);

	snprintf(buffer,sizeof(buffer), 
		 "{ \"host\": \"%s\", \"handle\" : \"%s\", \"report\" : \"%s\", \"time\" : %ld, "
		 "\"note\": { \"action\" : \"add\", \"hostkey\" : \"%s\", \"type\" : %d, \"rid\" : \"%s\"} " /* rid = oid of report */
		 "},",
		 hp->hh->hostname,hp->handle,hp->cause,hp->t,
		 hp->hh->keyhash,reportType,hp->oid);
      }
   else
      {
	//      snprintf(note_link,sizeof(note_link),"\"/notes/show/noteid/%s\"",hp->nid);
	//      snprintf(note,sizeof(note),"%s",CF_SHOWNOTE);

      snprintf(buffer,sizeof(buffer), 
	       "{ \"host\": \"%s\", \"handle\" : \"%s\", \"report\" : \"%s\", \"time\" : %ld, "
	       "\"note\": { \"action\" : \"show\", \"nid\" : \"%s\"} "
	       "},",
	       hp->hh->hostname,hp->handle,hp->cause,hp->t,
	       hp->nid);
      }
   /*   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"/promise/details/%s\">%s</a></td><td>%s</td><td>%s</td><td><a href=%s>%s</a></td></tr>\n", 
	    hp->hh->hostname,hp->handle,hp->handle,hp->cause,cf_ctime(&(hp->t)),
	    note_link,note);
   */

   
   if(!Join(returnval,buffer,bufsize))
      {
     break;
     }
   }

 returnval[strlen(returnval)-1]='\0';
EndJoin(returnval,"]",bufsize);
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

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,0,0,false,classreg);

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
     //   StartJoin(returnval,"<table>\n",bufsize);   
     strcpy(returnval,"[");

   summary = SortItemListCounters(summary);
   //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Promise handle</th><th>Report</th><th>Occurrences</th></tr>\n");

   Join(returnval,buffer,bufsize);
   
   /*   snprintf(note_link,CF_MAXVARSIZE,"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hostkey,CFREPORT_PRSUMMARY,ip->name);
   snprintf(note,CF_MAXVARSIZE,"%s",CF_SHOWNOTE);
   */
   for (ip = summary; ip != NULL; ip=ip->next)
     {
       //snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"/promise/details/%s\">%s</a></td><td>%s</td><td>%d</td></tr>\n",hostname,ip->name,ip->name,ip->classes,ip->counter);
       snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"handle\" : \"%s\",\"report\" : \"%s\",\"count\" : %d},",hostname,ip->name,ip->classes,ip->counter);

      if(!Join(returnval,buffer,bufsize))
	{
	break;
	}
      }
   returnval[strlen(returnval)-1]='\0';
   //   EndJoin(returnval,"</table>\n",bufsize);
   EndJoin(returnval,"]\n",bufsize);
   DeleteItemList(summary);
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
  char note_link[CF_MAXVARSIZE],note[CF_MAXVARSIZE];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,true,classreg);

 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Day</th><th>Kept</th><th>Repaired</th><th>Not kept</th><th>Notes</th></tr>\n");
 //Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubValue *)rp->item;

   if(strcmp(hp->nid,CF_NONOTE) == 0)
     {
       //  snprintf(note_link,CF_MAXVARSIZE,"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hp->hh->keyhash,CFREPORT_VALUE,hp->handle);
       //snprintf(note,CF_MAXVARSIZE,"%s",CF_ADDNOTE);
       snprintf(buffer,sizeof(buffer),
		"{\"host\" : \"%s\",\"day\" : \"%s\",\"kept\" : %.1lf,\"repaired\":%.1lf,\"notkept\":%.1lf, "
		"\"note\": { \"action\":\"add\", \"hostkey\":\"%s\", \"type\" : %d,\"rid\" : \"%s\"}},",
		hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept,
		hp->hh->keyhash,CFREPORT_VALUE,hp->handle);
     }
   else
     {
       //       snprintf(note_link,CF_MAXVARSIZE,"\"/note/show/noteid/%s\"",hp->nid);
       //       snprintf(note,CF_MAXVARSIZE,"%s",CF_SHOWNOTE);
       snprintf(buffer,sizeof(buffer),
		"{\"host\" : \"%s\",\"day\" : \"%s\",\"kept\" : %.1lf,\"repaired\":%.1lf,\"notkept\":%.1lf, "
		"\"note\": { \"action\":\"show\", \"nid\" : \"%s\"}},",
		hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept,
		hp->nid);
     }

   /*   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%.1lf</td><td>%.1lf</td><td>%.1lf</td><td><a href=%s>%s</a></td></tr>\n",
            hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept,note_link,note);
   */
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }
 returnval[strlen(returnval)-1]='\0';
EndJoin(returnval,"]\n",bufsize);

DeleteHubQuery(hq,DeleteHubValue);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_software_report(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

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


 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,classreg,true);
 PageRecords(&(hq->records),page,DeleteHubSoftware);

 //StartJoin(returnval,"<table>\n",bufsize);
 snprintf(buffer,sizeof(buffer),"[ meta:{count : %d}, data:{", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Name</th><th>Version</th><th>Architcture</th></tr>\n");
 //Join(returnval,buffer,bufsize);
      
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"name\" : \"%s\",\"ver\" : \"%s\",\"arch\":\"%s\"},",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

 returnval[strlen(returnval)-1]='\0';
EndJoin(returnval,"} ]\n",bufsize);


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


 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);

 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Class context</th><th>Occurs with<br>Probability</th><th>Uncertainty</th><th>Last seen</th></tr>\n");
 //Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
   
   //   if (now - hc->t > 6*3600)
      {
	//      snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td><span class=\"amber\">%s</span></td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
	snprintf(buffer,sizeof(buffer),"{\"host\":\"%s\",\"context\":\"%s\",\"prob\":%lf,\"uncert\":%lf,\"lastseen\":%ld},",hc->hh->hostname,hc->class,hc->prob,hc->dev,hc->t);
      }
      //   else if (now - hc->t > (time_t)CF_WEEK)
      {
	//snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td><span class=\"red\">%s</span></td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
      //   else
      {
	//snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td>%s</td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
      }
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

 returnval[strlen(returnval)-1]='\0';
//EndJoin(returnval,"</table>\n",bufsize);
EndJoin(returnval,"]\n",bufsize);

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
  char rvalBuf[CF_MAXVARSIZE];
  struct HubVariable *hv,*hv2;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  mongo_connection dbconn;
  
  int first = true, endscope=false;

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,classreg);

lscope[0] = '\0';

//StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (struct HubVariable *)rp->item;

   if (strcmp(lscope,hv->scope) != 0)
      {
      strcpy(lscope,hv->scope);
      
      if(!first)
	{
	  returnval[strlen(returnval)-1] = '\0';
	  Join(returnval,"]},",bufsize); // end object
	}

      snprintf(buffer,CF_BUFSIZE,"{\"scope\" : \"%s\",\"data\" : [",hv->scope);
      Join(returnval,buffer,bufsize);
      //      snprintf(buffer,CF_BUFSIZE,"<tr><th>Host</th><th>Type</th><th>Name</th><th>Value</th><th>Last seen</th></tr>\n");
      //      Join(returnval,buffer,bufsize);
      first = false;
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

   if (strlen(hv->dtype) > 1) // list
      {
      PrintRlist(rvalBuf,sizeof(rvalBuf),hv->rval);
      }
   else
      {
      snprintf(rvalBuf,sizeof(rvalBuf),"%s",(char *)hv->rval);
      }
   
   snprintf(buffer,CF_BUFSIZE,
	    "{ \"host\" : \"%s\",\"type\" : \"%s\",\"name\" : \"%s\", \"value\" : \"%s\", \"dt\" : \"%ld\"},",
	    hv->hh->hostname,typestr,hv->lval,rvalBuf,hv->t);

   if(!Join(returnval,buffer,bufsize))
     {
       break;
     }
   }

if (!first )
  {
    returnval[strlen(returnval)-1] = '\0';
    Join(returnval,"]}",bufsize); // end scope object
  }
//returnval[strlen(returnval)-1] = '\0';
//EndJoin(returnval,"</table>\n",bufsize);
EndJoin(returnval,"]\n",bufsize);

DeleteHubQuery(hq,DeleteHubVariable);

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

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,true,classreg);


 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Policy</th><th>Kept</th><th>Repaired</th><th>Not kept</th><th>Last seen</th></tr>\n");
 //Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),
	    "{\"host\" : \"%s\",\"policy\" : \"%s\",\"kept\" : %d,\"repaired\" : %d,\"notkept\" : %d,\"lastseen\" : %ld},",
            ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,ht->t);

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

//EndJoin(returnval,"</table>\n",bufsize);
returnval[strlen(returnval)-1]='\0';
EndJoin(returnval,"]\n",bufsize);

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
 
 if(!status)  // any
   {
   status = "x";
   }

 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,true,classreg);


 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Promise handle</th><th>Last known<br>State</th><th>Probability<br>Kept</th><th>Uncertainty</th><th>Last seen</th></tr>\n");
 //Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;

   /*snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"/promise/details/%s\">%s</a></td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_ctime(&(hp->t)));
   */
   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"handle\" : \"%s\",\"state\" : \"%s\",\"kept\" : %.2lf,\"uncert\" : %.2lf,\"dt\" : %ld},",
            hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,hp->t);
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

//EndJoin(returnval,"</table>\n",bufsize);
 returnval[strlen(returnval)-1]='\0';
EndJoin(returnval,"]\n",bufsize);
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

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,true,classreg);

 //StartJoin(returnval,"<table>\n",bufsize);
StartJoin(returnval,"[",bufsize);

count += strlen(returnval);

/*snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Initiated</th><th>IP address</th><th>Remote host</th><th>Last seen</th>"
            "<th>Hours ago</th><th>Avg interval</th><th>Uncertainty</th><th>Remote host key</th></tr>\n");
Join(returnval,buffer,bufsize);
*/ 
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
   
   /*   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td><a href=\"host/%s\">%s</a></td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td><span class=\"keyhash\">%s</span></td></tr>\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->keyhash,hl->rhost->ipaddr,cf_ctime(&then),
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
   */     
   
   snprintf(buffer,sizeof(buffer),
	    "\"host\" : \"%s\",</td><td>%s</td><td>%s</td><td><a href=\"host/%s\">%s</a></td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td><span class=\"keyhash\">%s</span></td></tr>\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->keyhash,hl->rhost->ipaddr,then,
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }
// bishwa
EndJoin(returnval,"]\n",bufsize);

DeleteHubQuery(hq,DeleteHubLastSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_performance_report(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],note_link[CF_MAXVARSIZE],note[CF_MAXVARSIZE];
  struct HubPerformance *hP;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,true,classreg);


 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 //snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Event</th><th>Last time</th><th>Avg time</th><th>Uncertainty</th><th>Last performed</th><th>Notes</th></tr>\n");
 //Join(returnval,buffer,bufsize);            

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hP = ( struct HubPerformance *)rp->item;

   if(strcmp(hP->nid,CF_NONOTE) == 0)
     {
       //      snprintf(note_link,CF_MAXVARSIZE,"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hP->hh->keyhash,CFREPORT_PERFORMANCE,hP->handle);
       //snprintf(note,CF_MAXVARSIZE,"%s",CF_ADDNOTE);
       snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"event\" : \"%s\",\"last\" : %.2lf,\"avg\" : %.2lf,\"uncert\" : %.2lf,\"performed\" : %ld,"
		"\"note\" : { \"action\" : \"add\",\"hostkey\" : \"%s\",\"type\" : %d,\"rid\" : \"%s\" }},",
		hP->hh->hostname,hP->event,hP->q,hP->e,hP->d,hP->t,
		hP->hh->keyhash,CFREPORT_PERFORMANCE,hP->handle);
     }
   else
     {
       //       snprintf(note_link,CF_MAXVARSIZE,"\"/note/show/noteid/%s\"",hP->nid);
       //       snprintf(note,CF_MAXVARSIZE,"%s",CF_SHOWNOTE);
       snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"event\" : \"%s\",\"time\" : %.2lf,\"avg\" : %.2lf,\"uncert\" : %.2lf,\"performed\" : %ld,"
		"\"note\" : { \"action\" : \"show\",\"nid\" : \"%s\" }},",
		hP->hh->hostname,hP->event,hP->q,hP->e,hP->d,hP->t,
		hP->nid);
     }
   
/*   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td>%s</td><td><a href=%s>%s</a></td></tr>\n",
            hP->hh->hostname,
            hP->event,hP->q,hP->e,hP->d,cf_ctime(&(hP->t)),
	    note_link,note);*/
   if(!Join(returnval,buffer,bufsize))
     {
      break;
     }
   }
//EndJoin(returnval,"</table>\n",bufsize);
 returnval[strlen(returnval)-1] = '\0';
EndJoin(returnval,"]\n",bufsize);

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

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,classreg);

 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");

 /*snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Setuid/setgid root file</th></tr>\n");
Join(returnval,buffer,bufsize);
 */
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( struct HubSetUid *)rp->item;

   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"file\" : \"%s\"},",hS->hh->hostname,hS->path);

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }
//EndJoin(returnval,"</table>\n",bufsize);
 returnval[strlen(returnval)-1] = '\0';
EndJoin(returnval,"]\n",bufsize);

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubBundleSeen *hb;   
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  char note_link[CF_MAXVARSIZE],note[CF_MAXVARSIZE];

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classreg,true);

 //StartJoin(returnval,"<table>\n",bufsize);
 strcpy(returnval,"[");
   /*snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>Bundle</a></th><th>Last verified</th><th>Hours Ago</th><th>Avg interval</th><th>Uncertainty</th><th>Notes</th></tr>\n");
Join(returnval,buffer,bufsize);
   */
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = ( struct HubBundleSeen *)rp->item;

   if (strcmp(hb->bundle,"QUERY") == 0)
      {
      continue;
      }

   if(strcmp(hb->nid,CF_NONOTE) == 0)
     {
       //       snprintf(note_link,CF_MAXVARSIZE,"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hb->hh->keyhash,CFREPORT_BUNDLE,hb->bundle);
       //       snprintf(note,CF_MAXVARSIZE,"%s",CF_ADDNOTE);
       snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"bundle\" : \"%s\",\"verified\" : %ld,\"ago\" : %.2lf,\"avg\" : %.2lf,\"uncert\" : %.2lf,"
		"\"note\" : { \"action\" : \"add\",\"hostkey\" : \"%s\",\"type\" : %d,\"rid\" : \"%s\" }},",
		hb->hh->hostname,hb->bundle,hb->t,
		hb->hrsago,hb->hrsavg,hb->hrsdev,
		hb->hh->keyhash,CFREPORT_BUNDLE,hb->bundle);
     }
   else
     {
       //       snprintf(note_link,CF_MAXVARSIZE,"\"/note/show/noteid/%s\"",hb->nid);
       //       snprintf(note,CF_MAXVARSIZE,"%s",CF_SHOWNOTE);
       snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"bundle\" : \"%s\",\"verified\" : %ld,\"ago\" : %.2lf,\"avg\" : %.2lf,\"uncert\" : \"%.2lf\","
		"\"note\" : { \"action\" : \"show\",\"nid\" : \"%s\" }},",
		hb->hh->hostname,hb->bundle,hb->t,
		hb->hrsago,hb->hrsavg,hb->hrsdev,
		hb->hh->keyhash,hb->nid);
     }
   /*   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td><a href=\"/bundle/details/bundle/%s\">%s</a></td><td>%s</td>"
            "<td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td><a href=%s>%s</a></td></tr>\n",
            hb->hh->hostname,hb->bundle,hb->bundle,cf_ctime(&(hb->t)),
            hb->hrsago,hb->hrsavg,hb->hrsdev,
	    note_link,note);
   */
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

 //EndJoin(returnval,"</table>\n",bufsize);
returnval[strlen(returnval)-1] = '\0'; 
EndJoin(returnval,"]",bufsize);

DeleteHubQuery(hq,DeleteHubBundleSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubFileChanges *hC;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize,icmp;
  mongo_connection dbconn;
  char note_link[CF_MAXVARSIZE] = {0},note[CF_MAXVARSIZE] = {0};

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

   hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,true,classreg);
   
   
   //   StartJoin(returnval,"<table>\n",bufsize);
   StartJoin(returnval,"[",bufsize);

   /*   snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>File</th><th>Change detected at</th><th>Notes</th></tr>\n");
   Join(returnval,buffer,bufsize);
   */
   for (rp = hq->records; rp != NULL; rp=rp->next)
     {
       hC = (struct HubFileChanges *)rp->item;
              
       if(strcmp(hC->nid,CF_NONOTE) == 0)
	 {
	   //	   snprintf(note_link,CF_MAXVARSIZE,"\"/notes/add/hostkey/%s/report_type/%d/handle/%s\"",hC->hh->keyhash,CFREPORT_FILECHANGES,hC->handle);
	   //	   snprintf(note,CF_MAXVARSIZE,"%s",CF_ADDNOTE);
	   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"file\" : \"%s\",\"dt\" : %ld,"
		    "\"note\" : { \"action\" : \"add\",\"hostkey\" : \"%s\",\"type\" : %d,\"rid\" : \"%s\" }},",
		    hC->hh->hostname,hC->path,hC->t,
		    hC->hh->keyhash,CFREPORT_FILECHANGES,hC->handle);
	 }
       else
	 {
	   //	   snprintf(note_link,CF_MAXVARSIZE,"\"/note/show/noteid/%s\"",hC->nid);
	   //	   snprintf(note,CF_MAXVARSIZE,"%s",CF_SHOWNOTE);
	   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"file\" : \"%s\",\"dt\" : %ld,"
		    "\"note\" : { \"action\" : \"show\",\"nid\" : \"%s\" }},",
		    hC->hh->hostname,hC->path,cf_ctime(&(hC->t)),
		    hC->nid);
	 }     

       //       snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td><a href=%s>%s</a></td></tr>\n",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)),note_link,note);

       if(!Join(returnval,buffer,bufsize))
	 {
	 break;
	 }
     }
   
   //   EndJoin(returnval,"</table>\n",bufsize);
   returnval[strlen(returnval) - 1] = '\0';
   EndJoin(returnval,"]\n",bufsize);
   
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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,true,classreg);

StartJoin(returnval,"[",bufsize);

//snprintf(buffer,sizeof(buffer),"<tr><th>Host</th><th>File</th><th>Change detected at</th><th>Change</th></tr>\n");
//Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;

   //   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),Nova_FormatDiff(hd->diff));
   snprintf(buffer,sizeof(buffer),"{\"host\" : \"%s\",\"file\" : \"%s\",\"time\" : %ld,\"change\" : %s},",
	    hd->hh->hostname,hd->path,hd->t,Nova_FormatDiff(hd->diff));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

//EndJoin(returnval,"</table>\n",bufsize);
 if(strlen(returnval) > 2)
   {
     returnval[strlen(returnval) - 1] = '\0'; 
   }
EndJoin(returnval,"]\n",bufsize);

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

int Nova2PHP_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,classreg,false);


StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubSoftware);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_classes_hosts(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,false);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }

   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubClass);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_vars_hosts(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }

   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_hosts(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubTotalCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_promise_hosts(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize)

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

 if(!status)  // any
   {
   status = "x";
   }


 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_hosts(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubLastSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_performance_hosts(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubPerformance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_hosts(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_hosts(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classreg,false);


StartJoin(returnval,"<table>\n<tr>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</tr></table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubBundleSeen);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_hosts(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,false,classreg);


StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubFileChanges);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_filediffs_hosts(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);
DeleteHubQuery(hq,DeleteHubFileDiff);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_promiselog_hosts(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,char *returnval,int bufsize)
{
  struct HubHost *hh;
  struct HubQuery *hq;
  mongo_connection dbconn;
  struct Rlist *rp;
  int counter = 0, n = 180;
  char buffer[CF_BUFSIZE];

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,from,to,false,classreg);

StartJoin(returnval,"<table>\n",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;

   if (counter++ % 6 == 0)
      {
      snprintf(buffer,CF_MAXVARSIZE,"</tr></tr>");
      Join(returnval,buffer,bufsize);
      }
   
   snprintf(buffer,CF_MAXVARSIZE,"<td><a href=\"host/%s\">%s</a><br>%s</td>\n",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

EndJoin(returnval,"</table>\n",bufsize);
DeleteHubQuery(hq,DeleteHubFileDiff);


 CFDB_Close(&dbconn);

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

      snprintf(work,CF_MAXVARSIZE,"<li><a href=\"/welcome/knowledge/pid/%d\"><span class=\"classcontext\">%s</span></a></li>",pid,ip->name);

      if(!Join(buffer,work,bufsize))
	{
	break;
	}
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
      snprintf(work,CF_MAXVARSIZE,"<li><span class=\"args\">%s</span></li>",ip->name);

      if(!Join(buffer,work,bufsize))
	{
	break;
	}
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
      snprintf(buffer,bufsize,"<div id=\"bundles\"><table>\n<tr><th>Type</th><th>Service bundle name</th><th>Description</th><th>Contributing to goals</th><th></th></tr>\n");
      }
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
            snprintf(work,sizeof(work),"<tr><td><a href=\"/welcome/knowledge/pid/%d\">%s</a> </td><td> %s</td></tr>",ip2->counter,ip2->name,ip2->classes);
            
            if (!Join(goals,work,CF_BUFSIZE))
               {
               break;
               }
            }
         
         strcat(goals,"</table>");
         snprintf(colour,CF_SMALLBUF,"/images/green.png");
         }
      else if (type)
         {
         snprintf(goals,CF_MAXVARSIZE,"Unknown");
         snprintf(colour,CF_SMALLBUF,"/images/yellow.png");
         }

      if (type)
         {
	   snprintf(work,CF_BUFSIZE,"<tr><td><a href=\"/bundle/details/type/%s\"><span class=\"bundletype\">%s</span></a></td><td><a href=\"/bundle/details/bundle/%s/type/%s\"><span class=\"bundle\">%s</span></a></td><td>%s</td><td>%s</td><td><img src=\"%s\"></td></tr>",ip->classes,ip->classes,ip->name,ip->classes,ip->name,Nova_GetBundleComment(ip->name),goals,colour);
         }
      else
         {
	   snprintf(work,CF_BUFSIZE,"<li><a href=\"/bundle/details/type/%s\"><span class=\"bundletype\">%s</span></a> <a href=\"/bundle/details/bundle/%s/type/%s\"><span class=\"bundle\">%s</span></a></li>",ip->classes,ip->classes,ip->name,ip->classes,ip->name);
         }
      
      if(!Join(buffer,work,bufsize))
	{
	break;
	}

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
      snprintf(work,CF_MAXVARSIZE,"<li><a href=\"/bundle/details/type/%s\"><span class=\"bundletype\">%s</span></a> <a href=\"/bundle/details/bundle/%s/type/%s\"><span class=\"bundle\">%s</span></a></li>",ip->classes,ip->classes,ip->name,ip->classes,ip->name);
      
      if(!Join(buffer,work,bufsize))
         {
         break;
         }
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

{ int pid;

Nova_WebTopicMap_Initialize();

if (pid = Nova_SearchTopicMap(search,buffer,bufsize))
   {
   // If there's only one match, just show it
     snprintf(buffer,bufsize,"<meta HTTP-EQUIV=\"REFRESH\" content=\"0; url=/welcome/knowledge/pid/%d\">",pid);
   return true;
   }
else
   {
   return true;
   }
}

/*****************************************************************************/

void Nova2PHP_show_topic(int id,char *buffer,int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE],topic_comment[CF_BUFSIZE];

buffer[0] = '\0';
Nova_WebTopicMap_Initialize();

if (Nova_GetTopicByPid(id,topic_name,topic_id,topic_context))
   {
   snprintf(buffer,bufsize,"<div id=\"topic\">\n'<span class=\"subject\">%s</span>' in section `<span class=\"category\">%s</span>:</div>",topic_name,topic_context);
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
  char work[CF_BUFSIZE] = {0};
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
strcat(buffer,"[");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   if (Nova_IsGreen(ip->counter))
      {
      snprintf(work,sizeof(work),"{ \"colour\": \"green\", \"key\": \"%s\", \"id\": \"%s\"},",ip->name,ip->classes);
      }
   else if (Nova_IsYellow(ip->counter))
      {
      snprintf(work,sizeof(work),"{ \"colour\": \"yellow\", \"key\": \"%s\", \"id\": \"%s\"},",ip->name,ip->classes);
      }
   else if (Nova_IsRed(ip->counter))
      {
      snprintf(work,sizeof(work),"{ \"colour\": \"red\", \"key\": \"%s\", \"id\": \"%s\"},",ip->name,ip->classes);
      }
   else
      {
      snprintf(work,sizeof(work),"{ \"colour\": \"blue\", \"key\": \"%s\", \"id\": \"%s\"},",ip->name,ip->classes);      
      }
   
   if (!Join(buffer,work,bufsize))
      {
      break;
      }
   }

if (clist)
   {
   buffer[strlen(buffer)-1] = '\0';
   }

EndJoin(buffer,"]",bufsize);
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
strcat(buffer,"[");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   snprintf(work,CF_MAXVARSIZE,"{ \"key\": \"%s\", \"id\": \"%s\"}",ip->name,ip->classes);

   if (ip && ip->next != NULL)
      {
      strcat(work,",");
      }
   
   if (!Join(buffer,work,bufsize))
      {
      break;
      }
   }

EndJoin(buffer,"]",bufsize);
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
else if (strcmp(colour,"red") == 0)
   {
   clist = Nova_RedHosts();
   }
else
   {
   clist = Nova_BlueHosts();
   }

if (clist)
   {
   buffer[0] = '\0';
   strcat(buffer,"[");
   
   for (ip = clist; ip !=  NULL; ip=ip->next, counter++)
      {      
      snprintf(work,CF_MAXVARSIZE,"{ \"key\": \"%s\", \"id\": \"%s\"}",ip->name,ip->classes);
      
      if (ip && ip->next != NULL)
         {
         strcat(work,",");
         }
      
      if (!Join(buffer,work,bufsize))
         {
         break;
         }
      }

   EndJoin(buffer,"]",bufsize);
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
strcat(buffer,"[");

for (ip = clist; ip !=  NULL; ip=ip->next)
   {
   snprintf(work,CF_MAXVARSIZE,"{ \"key\": \"%s\" , \"id\": \"%s\"}",ip->name,ip->classes);

   if (ip && ip->next != NULL)
      {
      strcat(work,",");
      }
   
   if (!Join(buffer,work,bufsize))
     {
     break;
     }
   }

EndJoin(buffer,"]",bufsize);
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

long Nova2PHP_count_blue_hosts()

{ struct Item *all = Nova_BlueHosts();
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
         "<table><tr><td><a href=\"/bundle/index/%s\">Bundles</a></td><td><a href=\"/welcome/classes/host/%s\">Classes</a></td></tr>"
         "<tr><td><a href=\"/welcome/knowledge/topic/goals\">Goals</a></td><td><a href=\"/promise/index/%s\">Promises</a></td></tr></table>",key,key,key);

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

  if(strcmp(handle,"internal_promise") == 0)
    {
    return "";
    }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!CFDB_QueryPromiseAttr(&dbconn,handle,cfp_bundlename,buffer,CF_BUFSIZE))
  {
  *buffer = '\0';
  }

 CFDB_Close(&dbconn);

 return buffer;
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
   
   snprintf(work,CF_MAXVARSIZE-1,"<tr><td>Type</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\">%s</a></td><td></td></tr>\n",hb->bodyType,hb->bodyType);
   Join(returnval,work,bufsize);

   snprintf(work,CF_MAXVARSIZE-1,"<tr><td>Name</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\">%s</a></td><td></td></tr>\n",hb->bodyName,hb->bodyName);
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
	   snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"right\"><span class=\"lval\"><a href=\"/welcome/knowledge/topic/%s\">%s</a></span></td><td>=></td><td><span class=\"rval\">%s</span></td><td><a href=\"/welcome/knowledge/topic/%s\">%s</a></td></tr>",ha->lval,ha->lval,ha->rval,ha->classContext,ha->classContext);
	 Join(returnval,work,bufsize);
         }

      }
   
   DeleteHubBody(hb);
   EndJoin(returnval,"</table></div>\n",bufsize);
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
      snprintf(work,CF_MAXVARSIZE-1,"<li><a href=\"/welcome/knowledge/topic/%s\">%s</a> ",ip->classes,ip->classes);
      Join(returnval,work,bufsize);
      
      snprintf(work,CF_MAXVARSIZE-1,"<a href=\"/welcome/body/body/%s/type/%s\">%s</a></li>\n",ip->name,ip->classes,ip->name);
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

  if(strcmp(handle,"internal_promise") == 0)
    {
    return "";
    }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!CFDB_QueryPromiseAttr(&dbconn,handle,cfp_promisetype,buffer,CF_BUFSIZE))
   {
   *buffer = '\0';
   }


 CFDB_Close(&dbconn);

 return buffer;
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

if (!CFDB_QueryPromiseAttr(&dbconn,handle,cfp_promiser,buffer,CF_BUFSIZE))
   {
   *buffer = '\0';
   }

 CFDB_Close(&dbconn);

 return buffer;
}

/*****************************************************************************/

int Nova2PHP_get_variable(char *hostkey,char *scope,char *lval,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE],lscope[CF_MAXVARSIZE];
  struct HubVariable *hv,*hv2;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, n = 180;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,NULL,NULL,false,NULL);

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
strcat(buffer,"[");

for (i = 0; BASIC_REPORTS[i] != NULL; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"\"%s\"",BASIC_REPORTS[i]);
   if (BASIC_REPORTS[i+1] != NULL)
      {
      strcat(work,",");
      }
   Join(buffer,work,bufsize);
   }

Join(buffer,"]",bufsize);
}

/*****************************************************************************/

int Nova2PHP_summarize_promise(char *handle, char *returnval,int bufsize)

{ mongo_connection dbconn;
  struct HubPromise *hp;
  char promiseeText[CF_MAXVARSIZE],bArgText[CF_MAXVARSIZE];
  char commentText[CF_MAXVARSIZE], constText[CF_MAXVARSIZE];
  char work[CF_MAXVARSIZE];
  int i,count;
  
  if(strcmp(handle,"internal_promise") == 0)
    {
    snprintf(returnval, bufsize, "<br> This is a promise made internally by Cfengine, and is thus not part of your policy.");
    return true;
    }


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hp = CFDB_QueryPromise(&dbconn, handle, NULL, 0);

if (!hp)
   {
   snprintf(returnval, bufsize, "<br> Promise '%s' was not found in the database.", handle);
   CFDB_Close(&dbconn);
   return false;
   }

returnval[0] = '\0';

strcat(returnval,"<div id=\"promise\"><table>\n");

 snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\" width=\"20%\">Belonging to <span class=\"bundletype\">%s</span> bundle</td><td>:</td><td><a href=\"/bundle/details/bundle/%s/type/%s\"><span class=\"bundle\">%s</span></a><td></tr>",hp->bundleType,hp->bundleName,hp->bundleType,hp->bundleName);
Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Reference handle</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\"><span class=\"handle\">%s</span></a></td></tr>",hp->handle,hp->handle);
Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Affected object (promiser)</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\"><span class=\"promiser\">%s</span></a></td></tr>",hp->promiser,hp->promiser);
Join(returnval,work,bufsize);

if (EMPTY(hp->promisee))
   {
   strcpy(promiseeText,"None mentioned");
   }
else
   {
     snprintf(promiseeText,sizeof(promiseeText),"<a href=\"/welcome/knowledge/topic/%s\">%s</a>",hp->promisee,hp->promisee);     
   }

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Stakeholders (promisees)</td><td>:</td><td><span class=\"promisee\">%s</span></td></tr>",promiseeText);
Join(returnval,work,bufsize);

if (EMPTY(hp->comment))
   {
   strcpy(commentText,"No comment");
   }
else
   {
   snprintf(commentText, sizeof(commentText),"%s",hp->comment);
   }

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Comment on original intention</td><td>:</td><td><span class=\"promiser\">%s</span></td></tr>",commentText);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Promise is about</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\"><span class=\"subtype\">%s</span></a></td></tr>",hp->promiseType,hp->promiseType);
Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Applies in the class context</td><td>:</td><td><a href=\"/welcome/knowledge/topic/%s\"><span class=\"classcontext\">%s</span></a></td></tr>",hp->classContext,hp->classContext);
Join(returnval,work,bufsize);

snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"left\">Defined in file</td><td>:</td><td><span class=\"file\">%s</span> near line %d</td></tr>",hp->file,hp->lineNo);
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

      if (strcmp(lval,"usebundle") == 0)
         {
	   snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"right\"><span class=\"lval\"><a href=\"/welcome/knowledge/topic/%s\">%s</a></span></td><td>=></td><td><a href=\"/bundle/details/bundle/%s/type/%s\"><span class=\"bundlename\">%s</span>%s</a></td></tr>",lval,lval,rval,lval,rval,args);
         }
      else
         {
	   snprintf(work,CF_MAXVARSIZE-1,"<tr><td align=\"right\"><span class=\"lval\"><a href=\"/welcome/knowledge/topic/%s\">%s</a></span></td><td>=></td><td><a href=\"/welcome/body/body/%s/type/%s\"><span class=\"bodyname\">%s</span>%s</a></td></tr>",lval,lval,rval,lval,rval,args);
         }
      
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
     snprintf(work,CF_MAXVARSIZE-1,"<li><a href=\"/promise/details/%s\">%s</a></li>",(char*)rp->item,(char*)rp->item);
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

{ mongo_connection dbconn;
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


 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,CF_HUB_HORIZON,NULL,false);

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

char *Nova_LongStateWarn(char s)

{
 switch (s)
    {
    case 'c':
        return "Compliant";
    case 'r':
        return "Repaired";
    case 'n':
    default:
        return "<span class=\"amber\">Not Compliant</span>";
    }
}

/*****************************************************************************/

void Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz)
{

  if(now > then + threshold)
    {
    snprintf(outStr,outStrSz,"<span class=\"amber\">%s</span>",cf_ctime(&then));
    }
  else
    {
    snprintf(outStr,outStrSz,"%s",cf_ctime(&then));
    }
}

/*****************************************************************************/

void Nova2PHP_ComplianceSummaryGraph(char *policy, char *buffer,int bufsize)
{
Nova_ComplianceSummaryGraph(buffer,bufsize);
}

/*****************************************************************************/

char *Nova_FormatDiff(char *s)

{ char *sp,work[CF_BUFSIZE],diff[CF_BUFSIZE],tline[CF_BUFSIZE];
  static char returnval[CF_BUFSIZE];
  char pm;
  int line = 0;

  //snprintf(returnval,sizeof(returnval),"<table>");
  strcpy(returnval,"[");

for (sp = s; *sp != '\0'; sp += strlen(tline)+1)
   {
   sscanf(sp,"%c,%d,%2047[^\n]",&pm,&line,diff);
   sscanf(sp,"%2047[^\n]",tline);

   snprintf(work,sizeof(work),"{ \"pm\" : \"%c\",\"line\" : %d, \"diff\" : \"%s\"},",pm,line,diff);
   /*   switch (pm)
      {
      case '+':
	//          snprintf(work,sizeof(work),"<tr><td><span class=\"pm\">%c</span></td><td>%d</td><td><span class=\"plusline\">%s</span></td><tr>",pm,line,diff);
	
          break;
      case '-':
	snprintf(work,sizeof(work),"{ \"pm\" : \"%c\",\"line\" : %d,</td><td><span class=\"minusline\">%s</span></td><tr>",pm,line,diff);
          break;
      default:
          snprintf(work,sizeof(work),"<tr><td><span class=\"pm\">%c</span></td><td>%d</td><td>%s</td><tr>",pm,line,diff);
          break;
      }
   */
   Join(returnval,work,sizeof(returnval));
   }

 returnval[strlen(returnval) - 1] = ']';
 //strcat(returnval,"</table>");
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

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,true,classreg);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%d<nc>%d<nc>%d<nc>%s<nova_nl>",
            ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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

int Nova2PHP_promiselog_pdf(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,from,to,true,classreg);

returnval[0] = '\0';
         
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseLog *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",
            hp->hh->hostname,hp->handle,hp->cause,cf_ctime(&(hp->t)));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

DeleteHubQuery(hq,DeleteHubPromiseLog);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_bundle_report_pdf(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize)

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

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classreg,true);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = ( struct HubBundleSeen *)rp->item;

   if (strcmp(hb->bundle,"QUERY") == 0)
      {
      continue;
      }

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%.2lf<nova_nl>",
            hb->hh->hostname,hb->bundle,cf_ctime(&(hb->t)),
            hb->hrsago,hb->hrsavg,hb->hrsdev);

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

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

 hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,true,classreg);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubValue *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%.1lf<nc>%.1lf<nc>%.1lf<nova_nl>",
            hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept);

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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

 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);

returnval[0] = '\0';

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
   
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

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

 if(!status)  // any
   {
   status = "x";
   }
 hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,true,classreg);
 returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;
   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%s<nova_nl>",
            hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_ctime(&(hp->t)));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
   }

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

 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,true,classreg);

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
          
   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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

 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,classreg,true);


returnval[0] = '\0';

      
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,true,classreg);

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

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,0,0,false,classreg);

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
            
   for (ip = summary; ip != NULL; ip=ip->next)
      {
      snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%d<nova_nl>",hostname,ip->name,ip->classes,ip->counter);

      if(!Join(returnval,buffer,bufsize))
	{
	break;
	}
      }
   DeleteItemList(summary);
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

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,classreg);

lscope[0] = '\0';
returnval[0] = '\0';


for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (struct HubVariable *)rp->item;

   if (strcmp(lscope,hv->scope) != 0)
      {
      strcpy(lscope,hv->scope);
      snprintf(buffer,CF_BUFSIZE,"<nh>Bundle scope %s<nova_nl>",hv->scope);
      Join(returnval,buffer,bufsize);
      snprintf(buffer,CF_BUFSIZE,"<nh>Host<nc>Type<nc>Name<nc>Value<nova_nl>");
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


DeleteHubQuery(hq,DeleteHubVariable);

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
  char topic_context[CF_BUFSIZE] = {0};
  char topic_comment[CF_BUFSIZE] = {0};
  char typedName[CF_MAXVARSIZE] = {0};

snprintf(typedName,sizeof(typedName),"system_reports::%s",reportName);

/*pid = Nova_GetPidForTopic(typedName);

if (pid && Nova_GetTopicByPid(pid,topic_name,topic_id,topic_context))
   {
   if (EMPTY(topic_comment))
      {
      snprintf(returnval,bufsize,"(no comment for %s found in topic map)",reportName);
      return false;
      }
   
   snprintf(returnval,bufsize,"%s",topic_comment);
   
   return true;
   }

   FIX ME TO GET COMMENT..........
*/

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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,true,classreg);

returnval[0] = '\0';

   
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hC = (struct HubFileChanges *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nova_nl>",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,true,classreg);

returnval[0] = '\0';

snprintf(buffer,sizeof(buffer),"<nh>Host<nc>File<nc>Change detected at<nc>Change<nova_nl>");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%s<nova_nl>",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),Nova_FormatDiff_pdf(hd->diff));

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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
   
   if(!Join(returnval,work,sizeof(returnval)))
     {
     break;
     }
   }

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


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,classreg);

returnval[0] = '\0';

snprintf(buffer,sizeof(buffer),"<nh>Host<nc>Setuid/setgid root file<nova_nl>\n");
Join(returnval,buffer,bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( struct HubSetUid *)rp->item;

   snprintf(buffer,sizeof(buffer),"%s<nc>%s<nova_nl>",hS->hh->hostname,hS->path);

   if(!Join(returnval,buffer,bufsize))
     {
     break;
     }
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
/*                       Content-Driven Policies                             */
/*****************************************************************************/

void Nova2PHP_cdp_reportnames(char *buf,int bufSz)

{ int i;
  char work[CF_SMALLBUF];

buf[0] = '\0';
Join(buf,"<select name=\"cdp_report\">\n",bufSz);

for (i = 0; CDP_REPORTS[i] != NULL; i++)
   {
   snprintf(work,sizeof(work),"<option value=\"%s\">%s</option>\n",CDP_REPORTS[i],CDP_REPORTS[i]);

   if(!Join(buf,work,bufSz))
     {
     break;
     }
   }

EndJoin(buf,"\n</select>\n",bufSz);
}

/*****************************************************************************/

cdp_t CdpReportNameToType(char *reportName)
{
  
  if(strcmp(reportName,"ACLs") == 0)
    {
    return cdp_acls;
    }
  else if(strcmp(reportName,"Commands") == 0)
    {
    return cdp_commands;
    }
  else if(strcmp(reportName,"File Changes") == 0)
    {
    return cdp_filechanges;
    }
  else if(strcmp(reportName,"File Diffs") == 0)
    {
    return cdp_filediffs;
    }
  else if(strcmp(reportName,"Registry") == 0)
    {
    return cdp_registry;
    }
  else if(strcmp(reportName,"Services") == 0)
    {
    return cdp_services;
    }
  else
    {
    return cdp_unknown;
    }
}

/*****************************************************************************/

int Nova2PHP_cdp_report(char *hostkey, char *reportName, char *buf, int bufSz)
{
  struct Item *promises = {0}, *hosts = {0};
  struct Item *ip = {0}, *ip2 = {0};
  mongo_connection dbconn;
  char handle[CF_MAXVARSIZE] = {0};
  char action[CF_MAXVARSIZE] = {0};
  char host[CF_MAXVARSIZE] = {0};
  char hostKeyHash[CF_MAXVARSIZE] = {0};
  char thenStr[CF_SMALLBUF] = {0};
  char cause[CF_MAXVARSIZE] = {0};
  char statusStr[CF_SMALLBUF];
  char lastChangeStr[CF_SMALLBUF];
  char fileChangePath[CF_MAXVARSIZE];
  char fileChangePathUrl[CF_MAXVARSIZE];
  char *urlReportName = "";
  time_t then = 0, now;
  char attributes[CF_MAXVARSIZE] = {0};
  char attributesTmp[CF_MAXVARSIZE] = {0};
  char row[CF_MAXVARSIZE] = {0};
  int ret = false;
  cdp_t cdpType;

memset(buf,0,bufSz);
now = time(NULL);

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 cdpType = CdpReportNameToType(reportName);

 switch(cdpType)
   {
   case cdp_acls:
     promises = CFDB_QueryCdpAcls(&dbconn,"</td><td>");
     break;

   case cdp_commands:
     promises = CFDB_QueryCdpCommands(&dbconn,"</td><td>");
     break;

   case cdp_services:
     promises = CFDB_QueryCdpServices(&dbconn,"</td><td>");
     break;

   case cdp_filechanges:
     promises = CFDB_QueryCdpPromiser(&dbconn,"</td><td>",cfp_cdp_bundle_filechanges,"files");
     reportName = cfr_filechanges;
     urlReportName = "File change log";
     break;

   case cdp_filediffs:
     promises = CFDB_QueryCdpPromiser(&dbconn,"</td><td>",cfp_cdp_bundle_filediffs,"files");
     reportName = cfr_filediffs;
     urlReportName = "File change diffs";
     break;

   case cdp_registry:
     promises = CFDB_QueryCdpRegistry(&dbconn,"</td><td>");
     break;

   default:
     snprintf(buf,bufSz,"Unknown Content-Driven Policy Type");
     CFDB_Close(&dbconn);
     return false;
   }
   
if (promises)
   {
   snprintf(buf,bufSz,"\n<table>\n");
   Join(buf,GetCdpTableHeader(cdpType),bufSz);
   
   for(ip = promises; ip != NULL; ip = ip->next)  // join policy with host reports
      {
      sscanf(ip->name,"%128[^<]</td><td>%512[^$]",handle,attributes);
      
      hosts = CFDB_QueryCdpCompliance(&dbconn,handle);
      
      if (hosts)
         {	   
         for (ip2 = hosts; ip2 != NULL; ip2 = ip2->next)
            {	       
            sscanf(ip2->name,"%512[^;];%128[^;];%8[^;];%ld[^$]",hostKeyHash,host,statusStr,&then);
            
            Nova_TimeWarn(now,then,CF_HUB_HORIZON,thenStr,sizeof(thenStr));
            
	    
            switch(cdpType)  // include special fields
	       {
	       case cdp_filechanges:
	       case cdp_filediffs:

                   sscanf(attributes, "%512[^<]", fileChangePath);
                   
                   snprintf(fileChangePathUrl, sizeof(fileChangePathUrl), "<a href=\"/search/index/report/%s/hostkey/%s/search/%s\">%s</a>",
                            urlReportName, hostKeyHash, fileChangePath,fileChangePath);
                   
                   // insert url to detailed report
                   snprintf(attributesTmp,sizeof(attributesTmp),"%s",attributes);
                   ReplaceStr(attributesTmp, attributes, sizeof(attributes), fileChangePath, fileChangePathUrl);
                   
                   CFDB_QueryLastFileChange(&dbconn, hostKeyHash, reportName,fileChangePath, lastChangeStr, sizeof(lastChangeStr));
                   Join(attributes, "</td><td>", sizeof(attributes));
                   Join(attributes, lastChangeStr, sizeof(attributes));
                   
                   break;
	       }
            
            
            snprintf(row,sizeof(row),"<tr><td><a href=\"host/%s\">%s</a></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                     hostKeyHash,host,attributes,Nova_LongStateWarn(*statusStr),thenStr);
            
            if(!Join(buf,row,bufSz))
	       {
	       break;
	       }
            }
         
         DeleteItemList(hosts);
         }
      }
   
   EndJoin(buf,"\n</table>\n",bufSz);
   
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

char *GetCdpTableHeader(cdp_t cdpType)
{
  switch(cdpType)
    {
    case cdp_acls:
      return "<tr><th>Host</th><th>Path</th><th>Permission (ACL)</th><th>Owner</th><th>Action</th><th>Class expression</th><th>State</th><th>Last checked</th></tr>";
    case cdp_commands:
      return "<tr><th>Host</th><th>Command</th><th>Failclass</th><th>Action</th><th>Class expression</th><th>State</th><th>Last checked</th></tr>";
    case cdp_filechanges:
    case cdp_filediffs:
      return "<tr><th>Host</th><th>Path</th><th>Class expression</th><th>Last change detected</th><th>State</th><th>Last checked</th></tr>";
    case cdp_registry:
      return "<tr><th>Host</th><th>Key</th><th>Value</th><th>Action</th><th>Class expression</th><th>State</th><th>Last checked</th></tr>";
    case cdp_services:
      return "<tr><th>Host</th><th>Service name</th><th>Runstatus</th><th>Action</th><th>Class expression</th><th>State</th><th>Last checked</th></tr>";
    }


  return "<tr>Undefined Special Policy Header</tr>";
}

/*****************************************************************************/
int Nova2PHP_validate_policy(char *file,char *buffer,int bufsize)

{
   char cmd[CF_BUFSIZE];
   char tmp[CF_MAXVARSIZE];
   int retVal;
   FILE *pp;

   // NOTE: this is run as the web user

   snprintf(cmd,CF_BUFSIZE,"/var/cfengine/bin/cf-promises -n -f \"%s\"",file);

   if((pp = cf_popen(cmd,"r")) == NULL)
     {
       snprintf(buffer,bufsize,"Could not run command \"%s\": %s", cmd, GetErrorStr());
       return -1;
     }

   buffer[0] = '\0';

   while(!feof(pp))
     {
     if(fgets(tmp,sizeof(tmp),pp))
       {
       Join(buffer,tmp,bufsize);
       }
     }

   retVal = cf_pclose(pp);

   return retVal; // 0 on success
}

/*****************************************************************************/

int Nova2PHP_delete_host(char *keyHash)
{
  mongo_connection dbconn;
  

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }
  
  
  CFDB_PurgeHost(&dbconn, keyHash);


  CFDB_Close(&dbconn);

  return true;
}

/*****************************************************************************/

bool Nova2PHP_environments_list(struct EnvironmentsList **out)
{
    mongo_connection dbconn;
    bson_buffer bb;
    bson cmd;
    bson result;
    bson_iterator i;
    bson_iterator values;

    *out = NULL;

    if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
        CfOut(cf_verbose, "", " !! Could not open connection to report database");
        return false;
    }

    /* { distinct: 'hosts', key: 'env' } */
    bson_buffer_init(&bb);
    bson_append_string(&bb, "distinct", "hosts");
    bson_append_string(&bb, "key", "env");
    bson_from_buffer(&cmd, &bb);

    if (!mongo_run_command(&dbconn, MONGO_BASE, &cmd, &result))
    {
        MongoCheckForError(&dbconn,"Nova2PHP_environments_list", "");
        bson_buffer_destroy(&bb);
        bson_destroy(&cmd);
        CFDB_Close(&dbconn);
        return false;
    }

    bson_destroy(&cmd);

    if (!bson_find(&i, &result, "values"))
    {
        CfOut(cf_verbose, "", " Malformed query result in Nova2PHP_environments_list");
        bson_destroy(&result);
        CFDB_Close(&dbconn);
        return false;
    }

    if (bson_iterator_type(&i) != bson_array)
    {
        CfOut(cf_verbose, "", " Malformed query result in Nova2PHP_environments_list");
        bson_destroy(&result);
        CFDB_Close(&dbconn);
        return false;
    }

    bson_iterator_subiterator(&i, &values);

    while (bson_iterator_next(&values))
    {
        struct EnvironmentsList *node = malloc(sizeof(struct EnvironmentsList));
        node->next = *out;
        node->name = strdup(bson_iterator_string(&values));
        *out = node;
    }

    bson_destroy(&result);
    CFDB_Close(&dbconn);
    return true;
}

bool Nova2PHP_environment_contents(const char *environment, struct HostsList **out)
{
    mongo_connection dbconn;
    mongo_cursor *cursor;
    bson_buffer bb;
    bson query, fields;

    *out = NULL;

    if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
        CfOut(cf_verbose, "", " !! Could not open connection to report database");
        return false;
    }

    if (environment)
    {
        /* { env: $environment } */
        bson_buffer_init(&bb);
        bson_append_string(&bb, "env", environment);
    }
    else
    {
        /* { env: { $exists: 0 } } */
        bson_buffer_init(&bb);
        bson_append_start_object(&bb, "env");
        bson_append_int(&bb, "$exists", 0);
        bson_append_finish_object(&bb);
    }
    bson_from_buffer(&query, &bb);

    /* { kH: 1 } */
    bson_buffer_init(&bb);
    bson_append_int(&bb, "kH", 1);
    bson_from_buffer(&fields, &bb);

    cursor = mongo_find(&dbconn, MONGO_DATABASE, &query, &fields, 0, 0, 0);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor))
    {
        bson_iterator i;

        if (!bson_find(&i, &cursor->current, "kH"))
        {
            CfOut(cf_verbose, "", "Malformed query result in Nova2PHP_environment_contents");
            mongo_cursor_destroy(cursor);
            CFDB_Close(&dbconn);
            return false;
        }

        struct HostsList *node = malloc(sizeof(struct HostsList));
        node->next = *out;
        node->keyhash = strdup(bson_iterator_string(&i));
        *out = node;
    }

    mongo_cursor_destroy(cursor);
    return true;
}

char *Nova2PHP_get_host_environment(const char *hostkey)
{
    mongo_connection dbconn;
    bson_buffer bb;
    bson query, fields;
    bson result;
    bool res;
    bson_iterator i;
    char *environment = NULL;

    if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
        CfOut(cf_verbose, "", " !! Could not open connection to report database");
        return false;
    }

    /* { kH: $hostkey } */
    bson_buffer_init(&bb);
    bson_append_string(&bb, "kH", hostkey);
    bson_from_buffer(&query, &bb);

    /* { env: 1 } */
    bson_buffer_init(&bb);
    bson_append_int(&bb, "env", 1);
    bson_from_buffer(&fields, &bb);

    res = mongo_find_one(&dbconn, MONGO_DATABASE, &query, &fields, &result);

    bson_destroy(&query);
    bson_destroy(&fields);

    if (res)
    {
        if (bson_find(&i, &result, "env"))
        {
            environment = strdup(bson_iterator_string(&i));
        }
        bson_destroy(&result);
    }

    CFDB_Close(&dbconn);
    return environment;
}

void FreeEnvironmentsList(struct EnvironmentsList *list)
{
    if (list)
    {
        FreeEnvironmentsList(list->next);
        free(list->name);
        free(list);
    }
}

void FreeHostsList(struct HostsList *list)
{
    if (list)
    {
        FreeHostsList(list->next);
        free(list->keyhash);
        free(list);
    }
}


/*****************************************************************************/
/*for commenting functionality */
/*****************************************************************************/

int Nova2PHP_add_note(char *noteid,char *username, time_t datetime,char* note)

{ struct Item *data = NULL;
  char msg[CF_BUFSIZE] = {0}, nid[CF_MAXVARSIZE] = {0};
  mongo_connection dbconn;
  bson_buffer bb;
  int ret = 0;

  if(!noteid || strlen(noteid) == 0)
    {
      CfOut(cf_verbose,"", "!! Noteid is empty");
      return false;
    }

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }

  snprintf(nid, CF_MAXVARSIZE, "%s",noteid);
  snprintf(msg, CF_BUFSIZE, "%s,%s,%ld\n",username,note,datetime);
  AppendItem(&data, msg, NULL);

  ret = CFDB_AddNote(&dbconn,NULL,nid,NULL,data);

  CFDB_Close(&dbconn);
  return ret;
}

/*****************************************************************************/
/*for commenting functionality */
/*****************************************************************************/
int Nova2PHP_add_new_note(char *keyhash, char *repid, int reportType, char *username, time_t datetime, char *note)

{ struct Item *data = NULL, *ip = NULL, *report = NULL;
  char msg[CF_BUFSIZE] = {0};
  char reportText[CF_BUFSIZE] = {0}, row[CF_BUFSIZE] = {0};
  char noteId[CF_MAXVARSIZE] = {0}, objectId[CF_MAXVARSIZE] = {0};
  mongo_connection dbconn;
  char handle[CF_MAXVARSIZE]={0};
  bson query,b, result;
  bson_buffer bb;
  bson_oid_t oid;

  char row_name[CF_MAXVARSIZE] = {0}, row_add[CF_MAXVARSIZE] = {0}, db[CF_MAXVARSIZE] = {0};
  bson_iterator it1,it2;
  int level = 0, getrow=false, ret;
  
  snprintf(msg, CF_BUFSIZE, "%s,%s,%ld\n",username,note,datetime);
  AppendItem(&data, msg, NULL);

  if (!keyhash || strlen(keyhash) == 0 || !repid || strlen(repid)==0)
    {
      CfOut(cf_verbose,"", "!! Hostkey and report id not given. Nothing to look for");
      return false;
    }

  //  snprintf(objectId, CF_MAXVARSIZE, "%s",repid);

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }

  //create query
  bson_buffer_init(&bb);
  bson_append_string(&bb,cfr_keyhash,keyhash);
  bson_from_buffer(&query,&bb);
  
  //get report
  switch(reportType)
    {
    case CFREPORT_HOSTS:
      level = 2;
      snprintf(row_name, sizeof(row_name), "%s","ip"); // taking the IP addresses
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s",cfn_nid);
      break;

    case CFREPORT_REPAIRED:
      level = 1;
      snprintf(db, sizeof(db), "%s", MONGO_LOGS_REPAIRED);
      
      bson_oid_from_string(&oid,repid);
      bson_buffer_init(&bb);
      bson_append_oid(&bb,"_id",&oid);
      bson_from_buffer(&query,&bb);
      getrow = CFDB_GetRow(&dbconn, db, &query, "*", row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s",cfn_nid); 
      break;

    case CFREPORT_PERFORMANCE:
      snprintf(row_name, sizeof(row_name), "%s.%s",cfr_performance,repid);
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
      break;

    case CFREPORT_VALUE: /*value report*/
      snprintf(row_name, sizeof(row_name), "%s.%s",cfr_valuereport,repid);
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
      break;
    case CFREPORT_FILECHANGES:  
      snprintf(row_name, sizeof(row_name), "%s.%s",cfr_filechanges,repid);
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn,db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
      break;
/*
    case CFREPORT_FILEDIFFS:  
      snprintf(row_name, sizeof(row_name), "%s.%s",cfr_filediffs,repid);
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
      break;
*/
    case CFREPORT_BUNDLE:
      snprintf(row_name, sizeof(row_name), "%s.%s",cfr_bundles,repid);
      snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
      getrow = CFDB_GetRow(&dbconn,db, &query, row_name, row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
      break;
    case CFREPORT_NOTKEPT:
      level = 1;
      snprintf(db, sizeof(db), "%s", MONGO_LOGS_NOTKEPT);

      bson_oid_from_string(&oid,repid);
      bson_buffer_init(&bb);
      bson_append_oid(&bb,"_id",&oid);
      bson_from_buffer(&query,&bb);
      getrow = CFDB_GetRow(&dbconn, db, &query, "*", row, sizeof(row), level);
      snprintf(row_add, sizeof(row_add), "%s",cfn_nid);
      break;
    }

  if(!getrow)
    {
      CfOut(cf_verbose,"", "!! Could not find report for: hostkey = %s, report ID = %s", keyhash, repid);
      bson_destroy(&query);
      return false;
    }

  // add note
  ret = CFDB_AddNote(&dbconn,keyhash, noteId, row, data);

  //add DBRef
  if(strlen(noteId)>0 && ret)
    {
      CFDBRef_AddToRow(&dbconn, db, &query, row_add, noteId);
    }
  //TODO: delete comment if addtorow fails?
  bson_destroy(&query);
  CFDB_Close(&dbconn);
  return ret;
}

/*****************************************************************************/
/*commenting*/
int Nova2PHP_get_notes(char *keyhash, char *nid, char *username, time_t from, time_t to, char *returnval, int bufsize)

{ struct Item *data = NULL, *ip = NULL;
  char msg[CF_BUFSIZE] = {0};
  char buffer[CF_BUFSIZE] = {0};
  mongo_connection dbconn;
  struct HubNoteInfo *hni;
  struct HubNote *hn;
  struct Rlist *result, *rp;

  char fuser[CF_MAXVARSIZE] = {0};
  char kh[CF_MAXVARSIZE] = {0};
  char noteId[CF_MAXVARSIZE] = {0};

  if(username)
    {
      snprintf(fuser, CF_MAXVARSIZE,"%s", username);
    }
  if(keyhash)
    {
      snprintf(kh, CF_MAXVARSIZE,"%s", keyhash);
    }
  if(nid)
    {
      snprintf(noteId, CF_MAXVARSIZE,"%s", nid);
    }

  snprintf(msg, CF_BUFSIZE, "%s,%ld,%ld\n",fuser, from, to);

  AppendItem(&data, msg, NULL);

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }

  result = CFDB_QueryNotes(&dbconn, kh, noteId, data);

  returnval[0] = '\0';
  //  snprintf(buffer,sizeof(buffer),"<table><tr><td>User</td><td>Date </td><td>Comment</td></tr>\n");
  Join(returnval,"[",bufsize);
  
  for (rp = result; rp != NULL; rp=rp->next)
    {
      hni = ( struct HubNoteInfo *) rp->item;
     for(hn = hni->note; hn != NULL; hn=hn->next)
	{
	  //	  snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%ld</td><td>%s</td></tr>\n", hn->user, hn->t, hn->msg);
	  snprintf(buffer,sizeof(buffer),"[{\"user\":\"%s\"}, {\"date\":%ld}, {\"message\":\"%s\"}],", hn->user, hn->t, hn->msg);
	  if(!Join(returnval,buffer,bufsize))
	    {
	      break;
	    }
	}
      hni = NULL;
    }
  returnval[strlen(returnval)-1] = '\0'; /* remove (,) at the end*/
  EndJoin(returnval,"]\n",bufsize);
  DeleteRlist(result);
  if (!CFDB_Close(&dbconn))
    {
      CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

  return true;
}
/*****************************************************************************/
int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize)
{
  char *report,buffer[CF_BUFSIZE];
  struct Rlist *rp,*result;
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

result = CFDB_QueryNoteId(&dbconn,&query);

bson_destroy(&query);
returnval[0] = '\0';
if(!result)
  {
    return false;
  }

for (rp = result; rp != NULL; rp=rp->next)
   {
     snprintf(buffer,CF_MAXVARSIZE,"%s",(char *)rp->item);
     snprintf(returnval,CF_MAXVARSIZE,"%s ",buffer);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_enterprise_version(char *buf, int bufsize)
{
#ifdef HAVE_LIBCFCONSTELLATION

snprintf(buf, bufsize, "Constellation %s", Constellation_GetVersion());

return true;

#else

snprintf(buf, bufsize, "Nova %s", Nova_GetVersion());
 
return true;

#endif


return false;
}

/*****************************************************************************/
/*                           Constellation                                   */
/*****************************************************************************/


int Con2PHP_summarize_notkept(enum time_window tw, char *buf, int bufsize)

{

#ifdef HAVE_LIBCFCONSTELLATION


 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubPromiseSum *hs;
 char buffer[CF_MAXVARSIZE];


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QuerySumNotKept(&dbconn,NULL,tw);

StartJoin(buf,"<table>\n",bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubPromiseSum *)rp->item;

   
   snprintf(buffer,sizeof(buffer),"<tr><td><a href=\"/promise/details/%s\">%s</td><td>%d</td></tr>\n",
	    hs->handle,hs->handle,hs->occurences);
   
   if(!Join(buf,buffer,bufsize))
     {
     break;
     }
   }

 EndJoin(buf,"</table>\n",bufsize);

 DeleteHubQuery(hq,DeleteHubPromiseSum);

 CFDB_Close(&dbconn);

 return true;

#else  /* NOT HAVE_LIBCFCONSTELLATION */

 snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_summarize_notkept() in Nova-only environment\n");
 CfOut(cf_error, "", buf);
 return false;

#endif

}

/*****************************************************************************/

int Con2PHP_count_notkept(char *promiseHandle, enum time_window tw, char *buf, int bufsize)

{

#ifdef HAVE_LIBCFCONSTELLATION


 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubPromiseSum *hs;
 char buffer[CF_MAXVARSIZE];
 int result = false;


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QuerySumNotKept(&dbconn,promiseHandle,tw);

 if(hq && hq->records)
   {
   hs = (struct HubPromiseSum *)hq->records->item;
   snprintf(buf,bufsize,"%d",hs->occurences);
   result = true;
   }
 else
   {
   snprintf(buf,bufsize,"%d",0);
   result = false;
   }

 DeleteHubQuery(hq,DeleteHubPromiseSum);

 CFDB_Close(&dbconn);

 return result;

#else  /* NOT HAVE_LIBCFCONSTELLATION */

 snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_count_notkept() in Nova-only environment\n");
 CfOut(cf_error, "", buf);
 return false;

#endif

}

/*****************************************************************************/

int Con2PHP_reasons_notkept(char *promiseHandle, enum time_window tw, char *buf, int bufsize)

{

#ifdef HAVE_LIBCFCONSTELLATION


 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubPromiseSum *hs;
 char buffer[CF_MAXVARSIZE];


if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

 hq = CFDB_QueryReasonsNotKept(&dbconn,promiseHandle,tw);


StartJoin(buf,"<table>\n",bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubPromiseSum *)rp->item;

   snprintf(buffer,sizeof(buffer),"<tr><td>%s</td><td>%d</td></tr>\n",
	    hs->cause,hs->occurences);
   
   if(!Join(buf,buffer,bufsize))
     {
     break;
     }
   }

 EndJoin(buf,"</table>\n",bufsize);
 

 DeleteHubQuery(hq,DeleteHubPromiseSum);

 CFDB_Close(&dbconn);

 return true;

#else  /* NOT HAVE_LIBCFCONSTELLATION */

 snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_reasons_notkept() in Nova-only environment\n");
 CfOut(cf_error, "", buf);
 return false;

#endif

}



#endif  /* HAVE_LIBMONGOC */

