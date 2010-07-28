
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

if (false)
   {
   Nova2PHP_software_report(0,0,0,0,0,cfr_software,buffer,20);
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

if (hostkey)
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

if (hostkey)
   {
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

if (hostkey)
   {
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
   count += tmpsize;

   if (strlen(hv->dtype) > 1) // list
      {
      char buffer[CF_BUFSIZE];
      PrintRlist(buffer,CF_BUFSIZE,hv->rval);
      snprintf(buffer,CF_BUFSIZE,"<td>%s</td></tr>\n",buffer);
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

