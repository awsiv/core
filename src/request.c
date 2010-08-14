
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: request.c                                                           */
/*                                                                           */
/* Created: Sun Jul 18 16:45:02 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

void Nova_CfQueryCFDB(char *querystr)
{
#ifdef HAVE_LIBMONGOC
 mongo_connection dbconn;
 bson query,b;
 bson_buffer bb;
 struct Rlist *rp,*list;
 struct HubHost *hh;
 struct HubSoftware *hs;
 struct HubClass *hc;
 struct HubQuery *hq;
 struct HubTotalCompliance *ht;
 struct HubVariable *hv;
 struct HubPromiseCompliance *hp;
 struct HubLastSeen *hl;
 struct HubMeter *hm;
 struct HubPerformance *hP;
 struct HubSetUid *hS;
 struct HubBundleSeen *hb;
 struct HubFileChanges *hC;
 struct HubFileDiff *hd;
char buffer[100000];
 
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_verbose, "", "!! Could not open connection to report database");
   }

// CFDB_ListEverything(&dbconn);

/* Example 1 **************************************************

hq = CFDB_QuerySoftware(&dbconn,bson_empty(&b),cfr_software,"zypper",NULL,NULL,false);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   printf("softwareresult: (%s,%s,%s) ",hs->name,hs->version,Nova_LongArch(hs->arch));
   printf("found on (%s=%s=%s)\n",hs->hh->keyhash,hs->hh->hostname,hs->hh->ipaddr);
   }

DeleteHubQuery(hq,DeleteHubSoftware);

*/

/* Example 2 *************************************************

hq = CFDB_QueryClasses(&dbconn,bson_empty(&b),NULL,false);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
   printf("classresult: \"%s\" %lf,%lf,%s",hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
   printf("found on (%s=%s=%s)\n",hc->hh->keyhash,hc->hh->hostname,hc->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

DeleteHubQuery(hq,DeleteHubClass);*/

 
/* Example 3 **************************************************

hq = CFDB_QueryTotalCompliance(&dbconn,bson_empty(&b),NULL,-1,-1,-1,-1,CFDB_GREATERTHANEQ);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;
   printf("Tcomp result: %s,%d,%d,%d at %s",ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));
   printf("found on (%s=%s=%s)\n",ht->hh->keyhash,ht->hh->hostname,ht->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/
/* Example 4 **************************************************

hq = CFDB_QueryVariables(&dbconn,bson_empty(&b),"we.*",NULL,NULL,NULL,true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hv = (struct HubVariable *)rp->item;
   printf("Var result: (%s) %s,%s = ",hv->dtype,hv->scope,hv->lval);
   ShowRval(stdout,hv->rval,hv->rtype);
   printf("found on (%s=%s=%s)\n",hv->hh->keyhash,hv->hh->hostname,hv->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/* Example 5 **************************************************

hq = CFDB_QueryPromiseCompliance(&dbconn,bson_empty(&b),NULL,'x',true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (struct HubPromiseCompliance *)rp->item;
   printf("Pcomp result: (%s) %lf,%lf at %s",hp->handle,hp->e,hp->d,cf_ctime(&(hp->t)));
   printf("found on (%s=%s=%s)\n",hp->hh->keyhash,hp->hh->hostname,hp->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/* Example 6 **************************************************
   
hq = CFDB_QueryLastSeen(&dbconn,bson_empty(&b),NULL,NULL,NULL,0,true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (struct HubLastSeen *)rp->item;
   
   printf("Lastseen result: (%c) %s=%s (%s) %.2lf,%.2lfpm%.2lf\n",hl->io,hl->rhost->keyhash,hl->rhost->hostname,hl->rhost->ipaddr,hl->hrsago,hl->hrsavg,hl->hrsdev);
   printf("found on (%s=%s=%s)\n",hl->hh->keyhash,hl->hh->hostname,hl->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/* Example 7 **************************************************
   
hq = CFDB_QueryMeter(&dbconn,bson_empty(&b));

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hm = (struct HubMeter *)rp->item;
   
   printf("Meter result: (%c) %lf,%lf,%lf\n",hm->type,hm->kept,hm->notkept);
   printf("found on (%s=%s=%s)\n",hm->hh->keyhash,hm->hh->hostname,hm->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/


/* Example 8 **************************************************
   
hq = CFDB_QueryPerformance(&dbconn,bson_empty(&b),NULL,true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hP = (struct HubPerformance *)rp->item;
   
   printf("Perf result: \"%s\" %lf,%lf,%lf at %s",hP->event,hP->q,hP->e,hP->d,cf_ctime(&(hP->t)));
   printf("found on (%s=%s=%s)\n",hP->hh->keyhash,hP->hh->hostname,hP->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/* Example 9 ************************************************
   
hq = CFDB_QuerySetuid(&dbconn,bson_empty(&b),NULL,true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = (struct HubSetUid *)rp->item;
   
   printf("Setuid result: %s\n",hS->path);
   printf("found on (%s=%s=%s)\n",hS->hh->keyhash,hS->hh->hostname,hS->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/* Example 10 ***********************************************

hq = CFDB_QueryBundleSeen(&dbconn,bson_empty(&b),NULL,true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hb = (struct HubBundleSeen *)rp->item;
   
   printf("Bundle result: \"%s\" %.2lf,%.2lf,%.2lf\n",hb->bundle,hb->hrsago,hb->hrsavg,hb->hrsdev);
   printf("found on (%s=%s=%s)\n",hb->hh->keyhash,hb->hh->hostname,hb->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/

/*
hq = CFDB_QueryFileChanges(&dbconn,bson_empty(&b),NULL,false,-1,CFDB_GREATERTHANEQ);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hC = (struct HubFileChanges *)rp->item;
   
   printf("File: \"%s\" at %s\n",hC->path,cf_ctime(&(hC->t)));
   printf("found on (%s=%s=%s)\n",hC->hh->keyhash,hC->hh->hostname,hC->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

hq = CFDB_QueryFileDiff(&dbconn,bson_empty(&b),NULL,NULL,false,-1,CFDB_GREATERTHANEQ);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (struct HubFileDiff *)rp->item;
   
   printf("File: \"%s\" at %s\n",hd->path,cf_ctime(&(hd->t)));
   printf("Diff: %s",hd->diff);
   printf("found on (%s=%s=%s)\n",hd->hh->keyhash,hd->hh->hostname,hd->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

*/


// graphs

/*
double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
CFDB_QueryMagView(&dbconn,"SHA=0adede6fc8115004e77550d0c99da03ad8f502f1e48683d924e97047a76e1b21",2,time(0) - 4*3600,q,e,d);

int i;
for (i = 0; i < CF_MAGDATA; i++)
   {
   printf("%d (%.2lf,%.2lf,%.2lf)\n",i,q[i],e[i],d[i]);
   }

struct CfDataView cfv;

Nova_ReadMagTimeSeries(&cfv,"SHA=0adede6fc8115004e77550d0c99da03ad8f502f1e48683d924e97047a76e1b21",2);

for (i = 0; i < CF_MAGDATA; i++)
   {
   printf("CFV %d (%.2lf,%.2lf,%.2lf)\n",i,cfv.data_q[i],cfv.data_E[i],cfv.bars[i]);
   }
*/

/*
double q[CF_TIMESERIESDATA],e[CF_TIMESERIESDATA],d[CF_TIMESERIESDATA];
CFDB_QueryWeekView(&dbconn,"MD5=4a37e48645122312daf7862f2a0f0ef7",0,q,e,d);
*/


/*
double q[CF_TIMESERIESDATA],e[CF_TIMESERIESDATA],d[CF_TIMESERIESDATA];
CFDB_QueryYearView(&dbconn,"MD5=4a37e48645122312daf7862f2a0f0ef7",0,q,e,d);
*/


/*
double histo[CF_GRAINS];
CFDB_QueryHistogram(&dbconn,"MD5=4a37e48645122312daf7862f2a0f0ef7",0,histo);

int i;
for (i = 0; i < CF_GRAINS; i++)
   {
   printf("%.2lf ",histo[i]);
   }
printf("\n");
*/

/*
printf("Making gfraphs\n");
strcpy(DOCROOT,"/home/mark/tmp");

Nova_Meter("MD5=4a37e48645122312daf7862f2a0f0ef7");

struct Item *ip,*result = Nova_SummaryMeter(NULL);

for (ip = result; ip !=  NULL; ip=ip->next)
   {
   printf("HOST: %s\n",ip->name);
   }

//Nova_PerformancePage("MD5=4a37e48645122312daf7862f2a0f0ef7");
*/

char buff[100000];
//Nova2PHP_software_report(NULL,NULL,NULL,NULL,0,cfr_software,buff,100000);
//printf("SW: %s\n",buff);

//Nova2PHP_classes_report(NULL,NULL,0,buff,100000);
//printf("CLASSES: %s\nCLASSES\n\n",buff);

//Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,buff,100000);
//printf("VARS: (%s)\nVARS\n\n",buff);

//Nova2PHP_compliance_report(NULL,NULL,-1,-1,-1,-1,">",buff,100000);
//printf("TC: (%s)\nTC\n\n",buff);

//Nova2PHP_compliance_promises(NULL,NULL,"x",0,buff,10000);
//printf("PR: (%s)\nPR\n\n",buff);

//Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,buff,10000);
//printf("LS: (%s)\nLS\n\n",buff);

/*
Nova2PHP_performance_report(NULL,NULL,0,buff,10000);
printf("PF: (%s)\nPF\n\n",buff);

Nova2PHP_setuid_report(NULL,NULL,0,buff,10000);
printf("SETUID: (%s)\nSETUID\n\n",buff);

Nova2PHP_bundle_report(NULL,NULL,0,buff,10000);
printf("BUNDLE: (%s)\nBUNDLE\n\n",buff);
*/
//Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",buff,10000);
//printf("CHANGE: (%s)\nCHANGE\n\n",buff);

//Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",buff,10000);
//printf("CHANGE: (%s)\nCHANGE\n\n",buff);

//Nova2PHP_bundle_hosts(NULL,NULL,0,buff,10000);
//printf("HOSTS WITH: (%s)\nHOSTSWITH\n\n",buff);

//Nova_PerformancePage("MD5=4a37e48645122312daf7862f2a0f0ef7");
//Nova_PerformancePage("/srv/www/htdocs","MD5=4dda03e4f9b7547b3dc52aa463059d90",buffer,200000);

/*
printf("Search\n");
Nova2PHP_search_topics("sd","topic",1,buffer,2000);
printf("\n%s\n",buffer);
printf("Sjow\n");
Nova2PHP_show_topic(18,buffer,2000);
printf("\n%s\n",buffer);
printf("Leads\n");
Nova2PHP_show_topic_leads(18,buffer,2000);
printf("\n%s\n",buffer);
printf("Hits\n");
Nova2PHP_show_topic_hits(18,buffer,2000);
printf("\n%s\n",buffer);
printf("Category\n");
Nova2PHP_show_topic_category(18,buffer,2000);
printf("\n%s\n",buffer);
*/

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose, "", "!! Could not close connection to report database");
   }
/*
long n = Nova2PHP_count_hosts();
printf("NUMBER = %ld\n",n);

n = Nova2PHP_count_green_hosts();
printf("NUMBER GR = %ld\n",n);

n = Nova2PHP_count_red_hosts();
printf("NUMBER R = %ld\n",n);

n = Nova2PHP_count_yellow_hosts();
printf("NUMBER Y = %ld\n",n);

Nova2PHP_value_report(NULL,NULL,NULL,NULL,buffer,20000);
printf("NLOTKEPT = %s\n",buffer);

Nova2PHP_promiselog(NULL,NULL,plog_notkept,buffer,20000);
printf("AVLU = %s\n",buffer);
*/

//Nova2PHP_getlastupdate("SHA_852ffe9132338a8d76529ad522930933a111782959f028d8b63d35142531dc70",buffer,1000);
//printf("UPDATE = %s\n",buffer);

//Nova_ComplianceSummaryGraph("/srv/www/htdocs",buffer,10);

Nova2PHP_show_col_hosts("green",200,buffer,1000);
printf("HOSTS = %s\n",buffer);

Nova_WebTopicMap_Initialize();
printf("PID %d\n",Nova_GetPidForTopic("system_knowledge::views"));

#else
printf("For Cfengine internal only\n");
#endif
}

