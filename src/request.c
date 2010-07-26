
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
 
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

// CFDB_ListEverything(&dbconn);

/* Example 1 **************************************************

hq = CFDB_QuerySoftware(&dbconn,bson_empty(&b),"zypper",NULL,NULL,false);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   printf("softwareresult: (%s,%s,%s) ",hs->name,hs->version,Nova_LongArch(hs->arch));
   printf("found on (%s=%s=%s)\n",hs->hh->keyhash,hs->hh->hostname,hs->hh->ipaddr);
   }

DeleteHubQuery(hq,DeleteHubSoftware);

*/

/* Example 2 **************************************************

hq = CFDB_QueryClasses(&dbconn,bson_empty(&b),"linux.*",true);

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

DeleteHubQuery(hq,DeleteHubClass);

*/
 
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
   hm = (struct HubMeter *)rp->item;
   
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


// graphs

/*
double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
CFDB_QueryMagView(&dbconn,"MD5=4a37e48645122312daf7862f2a0f0ef7",0,time(0) - 4*3600,q,e,d);
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

struct CfDataView cfv;
//Nova_BuildGraphs(&cfv);

strcpy(DOCROOT,"/home/mark/tmp");
int i;

for (i = 0; i < 20; i++)
   {
   Nova_ViewMag(&cfv,"MD5=4a37e48645122312daf7862f2a0f0ef7",i);
   Nova_ViewWeek(&cfv,"MD5=4a37e48645122312daf7862f2a0f0ef7",i);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

#endif
}

