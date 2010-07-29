
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
   }
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   hq = CFDB_QuerySoftware(&dbconn,&query,type,name,value,arch,regex);
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
   snprintf(buffer,CF_BUFSIZE,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));

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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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
 
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);

   hq = CFDB_QueryClasses(&dbconn,&query,name,regex);
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
   snprintf(buffer,CF_BUFSIZE,"<tr><td>%s</td><td>%s</td><td>%lf</td><td>%lf</td><td>%s</td></tr>\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,cf_ctime(&(hc->t)));

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
   CfOut(cf_error, "", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
  struct HubVariable *hv;
  struct HubQuery *hq;
  struct Rlist *rp,*result;
  int count = 0, tmpsize = 0;
  mongo_connection dbconn;
  bson query,b;
  bson_buffer bb;

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryVariables(&dbconn,&query,scope,lval,rval,type,regex);
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
      PrintRlist(b,CF_BUFSIZE,hv->rval);
      snprintf(buffer,CF_BUFSIZE,"<td>%s</td></tr>\n",b);
      }
   else
      {
      snprintf(buffer,CF_BUFSIZE,"<td>%s</td></tr>\n",hv->rval);
      }

   tmpsize = strlen(buffer);
   
   if (count + tmpsize > bufsize - strlen("</table>\n") - 1)
      {
      break;
      }
   
   strcat(returnval,buffer);
   count += tmpsize;
   }

strcat(returnval,"</table>\n");

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
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
  
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryTotalCompliance(&dbconn,&query,version,t,k,nk,rep,icmp);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>\n",
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPromiseCompliance(&dbconn,&query,handle,*status,regex);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hp->hh->hostname,hp->handle,hp->e,hp->d,cf_ctime(&(hp->t)));
          
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

/* BEGIN query document */

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryLastSeen(&dbconn,&query,lhash,lhost,laddress,lago,lregex);
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
   hl = ( struct HubLastSeen *)rp->item;

   switch (hl->io)
      {
      case '+':
          snprintf(inout,CF_SMALLBUF,"Hailed");
          break;
      case '-':
          snprintf(inout,CF_SMALLBUF,"Hailed by");
          break;
      }

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryPerformance(&dbconn,&query,job,regex);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%.2lf</td><td>%s</td></tr>\n",
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QuerySetuid(&dbconn,&query,file,regex);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td></tr>\n",hS->hh->hostname,hS->path);
   
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryBundleSeen(&dbconn,&query,bundle,regex);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%.2lf</td><td>%.2lf</td><td>%.2lf</td></tr>\n",hb->hh->hostname,hb->bundle,hb->hrsago,hb->hrsavg,hb->hrsdev);
   
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileChanges(&dbconn,&query,file,regex,t,icmp);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td></tr>\n",hC->hh->hostname,hC->path,cf_ctime(&(hC->t)));
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
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

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   return false;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   bson_buffer_init(&bb);
   bson_append_string(&bb,cfr_keyhash,hostkey);
   bson_from_buffer(&query,&bb);
   hq = CFDB_QueryFileDiff(&dbconn,&query,file,diffs,regex,t,icmp);
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

   snprintf(buffer,CF_BUFSIZE-1,"<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",hd->hh->hostname,hd->path,cf_ctime(&(hd->t)),hd->diff);
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
   CfOut(cf_error, "", "!! Could not close connection to report database");
   }

return true;
}



