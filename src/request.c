
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
 
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

//CFDB_ListEverything(&dbconn);

/* Example 1 **************************************************

hq = CFDB_QuerySoftware(&dbconn,"zypper",NULL,NULL,false);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   printf("result: (%s,%s,%s) ",hs->name,hs->version,Nova_LongArch(hs->arch));
   printf("found on (%s=%s=%s)\n",hs->hh->keyhash,hs->hh->hostname,hs->hh->ipaddr);
   }

DeleteHubQuery(hq,DeleteHubSoftware);

****************************************************************/

/* Example 2 **************************************************

hq = CFDB_QueryClasses(&dbconn,"linux.*",true);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (struct HubClass *)rp->item;
   printf("result: %s,%lf,%lf,%s",hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));
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

****************************************************************/

/* Example 3 **************************************************

hq = CFDB_QueryTotalCompliance(&dbconn,NULL,-1,-1,-1,-1,CFDB_GREATERTHANEQ);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (struct HubTotalCompliance *)rp->item;
   printf("result: %s,%d,%d,%d at %s",ht->version,ht->kept,ht->repaired,ht->notkept,cf_ctime(&(ht->t)));
   printf("found on (%s=%s=%s)\n",ht->hh->keyhash,ht->hh->hostname,ht->hh->ipaddr);
   }

printf("Search returned matches from hosts: ");

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
   printf("%s  ",hh->hostname);
   }
printf("\n");

****************************************************************/

hq = CFDB_QueryVariables(&dbconn,NULL,NULL,NULL,NULL);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hv = (struct HubVariable *)rp->item;
   printf("result: (%s) %s,%s = ",hv->dtype,hv->scope,hv->lval);
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



if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

#endif
}

