/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: cmd_api.c                                                           */
/*                                                                           */
/* Created: Mon Aug 8 10:44:14 2011                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

/****************************************************************************/

void Nova2Txt_getlastupdate(char *hostkey,char *buffer,int bufsize)

{ time_t then;
  mongo_connection dbconn;

/* BEGIN query document */

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return;
   }

if (hostkey && strlen(hostkey) > 0)
   {
   then = 0;
   CFDB_QueryLastUpdate(&dbconn,MONGO_DATABASE,cfr_keyhash,hostkey,&then);

   if (CSV)
      {
      if (then > 0)
         {
         printf("%s,%s",hostkey,cf_ctime(&then));
         }
      else
         {
         printf("%s,never",hostkey);
         }
      }
   else
      {
      if (then > 0)
         {
         printf("Data from %s last updated %s",hostkey,cf_ctime(&then));
         }
      else
         {
         printf("Data from %s have never been collected",hostkey);
         }
      }
   }
else
   {
   printf("No hostkey specified");
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }
}

/****************************************************************************/

void Nova2Txt_getlicense(char *buffer,int bufsize)

{
NewClass("am_policy_hub");
CFDB_GetValue("license_report",buffer,bufsize);
}

/*****************************************************************************/

int Nova2Txt_summary_report(char *hostkey,char *handle,char *status,int regex,char *classreg)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{ char buf1[CF_SMALLBUF],buf2[CF_SMALLBUF];
  HubPromiseCompliance *hp;
  HubQuery *hq;
  Rlist *rp;
  mongo_connection dbconn;
  time_t now = time(NULL),from=now,to=now-SECONDS_PER_WEEK;
  int code_blue = 0,tot_hosts;
  double n,r,k,n_av,k_av,r_av,tot_promises;
  char *current_host = "x";

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!status)  // any
   {
   status = "x";
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,0,false,filter);
DeleteHostClassFilter(filter);

n = k = r = 0;
n_av = k_av = r_av = 0;
tot_promises = 1;
tot_hosts = 0;

// For each promise record (from multiple matching hosts)

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (HubPromiseCompliance *)rp->item;

   // If data have passed the time horizon, we should not claim to know their state
   // The system might or might not be compliant, hp->e is the expected schedule for this promise
   
   if (hp->t < now - CF_HUB_HORIZON - hp->e)
      {
      if (current_host && strcmp(hp->hh->keyhash,current_host) != 0) // New host
         {
         code_blue++;
         current_host = hp->hh->keyhash;
         }
      continue;
      }
   
   // How do we know how many promises are supposed to be kept on a given host?
   // compute the average number per host      
   if (current_host && strcmp(hp->hh->keyhash,current_host) != 0) // New host
      {
      n_av += n / tot_promises; // Average not kept on available hosts in class
      r_av += r / tot_promises; // Average repaired on available hosts in class
      k_av += k / tot_promises; // Average compliant/kept on available hosts in class

      n = k = r = 0;
      tot_promises = 1;
      tot_hosts++;              // The number of hosts in the class that have fresh data
      current_host = hp->hh->keyhash;
      }
   else
      {
      tot_promises++;
      }
   
   // Get minimax boundary of last measured time region so we can compute uncertainty

   if (hp->t < from)
      {
      from = hp->t;
      }

   if (hp->t > to)
      {
      to = hp->t;
      }

   // Get status of this matching promise (n,r,k = 1 if unique handle)
  
   switch (hp->status)
      {
      case PROMISE_STATE_KEPT:
          k++;
          break;
      case PROMISE_STATE_REPAIRED:
          r++;
          break;
      case PROMISE_STATE_NOTKEPT:
      default:
          n++;
          break;
      }
   }

n_av += n / tot_promises; // Average not kept on available hosts in class
r_av += r / tot_promises; // Average repaired on available hosts in class
k_av += k / tot_promises; // Average compliant/kept on available hosts in class

if (tot_hosts == 0)
   {
   from = now - CF_HUB_HORIZON;
   to = now;
   }
// Return current best-knowledge of average compliance for the class of hosts and promises selected

if (CSV)
   {
   printf("%.2lf,"
          "%.2lf,"
          "%.2lf,"
          "%d,"
          "%d,"
          "%s,"
          "%s,"
          "%s\n",
          k_av,n_av,r_av,tot_hosts,code_blue,classreg,
          cf_strtimestamp_local(from,buf1),
          cf_strtimestamp_local(to,buf2));
   }
else
   {
   printf("Hosts with promises kept: %.2lf\n"
          "Hosts with promises not kept: %.2lf\n"
          "Hosts with promises repaired %.2lf\n"
          "Total number of hosts: %d\n"
          "Hosts that didn't report status: %d\n"
          "Search class: %s\n"
          "First reports at: %s\n"
          "Last reports at: %s\n",
          k_av,n_av,r_av,tot_hosts,code_blue,classreg,
          cf_strtimestamp_local(from,buf1),
          cf_strtimestamp_local(to,buf2));
   }

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

bool Nova2Txt_vitals_list(char *keyHash, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 bool ret = false;
 char work[CF_MAXVARSIZE];
 time_t lastUpdate = 0;
 char hostName[CF_MAXVARSIZE], ipAddress[CF_MAXVARSIZE];
 HubVital *res, *hv;

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 res = CFDB_QueryVitalsMeta(&dbconn, keyHash);

 strcpy(buffer,"{");

 Nova2Txt_hostinfo(keyHash, hostName, ipAddress, sizeof(hostName));
 CFDB_QueryLastUpdate(&dbconn,MONGO_DATABASE,cfr_keyhash, keyHash, &lastUpdate);

 CFDB_Close(&dbconn);
 
 snprintf(work, sizeof(work), "\"hostname\" : \"%s\", \"ip\" : \"%s\", \"ls\" : %ld, \n\"obs\" : [",
          hostName, ipAddress, lastUpdate);

 Join(buffer,work,bufsize);

 
 for(hv = res; hv != NULL; hv = hv->next)
    {
    snprintf(work, sizeof(work), "{\"id\":\"%s\", \"units\":\"%s\", \"desc\":\"%s\"},",
             hv->id, hv->units, hv->description);
    Join(buffer, work, bufsize);
    
    ret = true;
    }

 DeleteHubVital(res);
 
 ReplaceTrailingChar(buffer, ',', '\0');
 EndJoin(buffer, "]}", bufsize);
 
 return ret;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2Txt_promiselog(char *hostkey,char *handle, PromiseLogState state,time_t from,time_t to,char *classreg,PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE], canonifiedCause[CF_BUFSIZE]={0};
 HubPromiseLog *hp;  HubQuery *hq;
 Rlist *rp;
 int reportType;
 mongo_connection dbconn;
  
 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,state,handle,true,from,to,true,classreg);
 PageRecords(&(hq->records),page,DeleteHubPromiseLog);
 
 snprintf(buffer,sizeof(buffer), 
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\":{\"Host\":0,\"Promise Handle\":1,\"Report\":2,\"Time\":3,"
	  "\"Note\":{\"index\":4,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (HubPromiseLog *)rp->item;
    EscapeJson(hp->cause,canonifiedCause,sizeof(canonifiedCause));
    if (strcmp(hp->nid,CF_NONOTE) == 0)
       {
       switch (state)
	  {
          case PROMISE_LOG_STATE_REPAIRED:
              reportType = CFREPORT_REPAIRED;
              break;
          case PROMISE_LOG_STATE_NOTKEPT:
	  default:
              reportType = CFREPORT_NOTKEPT;
              break;
	  }
       snprintf(buffer,sizeof(buffer), 
                "[ \"%s\",\"%s\",\"%s\",%ld,"
                "[ \"add\",\"%s\",%d,\"%s\",\"\"]"
                "],",
                hp->hh->hostname,hp->handle,canonifiedCause,hp->t,
                hp->hh->keyhash,reportType,hp->oid);
       }
    else
       {
       snprintf(buffer,sizeof(buffer), 
                "[ \"%s\",\"%s\",\"%s\",%ld,"
                "[ \"show\",\"\",\"\",\"\",\"%s\"]"
                "],",
                hp->hh->hostname,hp->handle,canonifiedCause,hp->t,
                hp->nid);
       }
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }
 EndJoin(returnval,"]}",bufsize);
 DeleteHubQuery(hq,DeleteHubPromiseLog);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_promiselog_summary(char *hostkey,char *handle, PromiseLogState state,time_t from, time_t to,char *classreg)

{
 HubPromiseLog *hp;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;
 Item *ip,*summary = NULL;
 int i = 0;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,state,handle,true,from,to,false,classreg);
 
 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (HubPromiseLog *)rp->item;
    ip = IdempPrependItem(&summary,hp->handle,hp->cause);
    ip->counter++;
    }

 DeleteHubQuery(hq,DeleteHubPromiseLog);

 CFDB_Close(&dbconn);

 if (summary == NULL)
    {
    printf("No data to report\n");
    }
 else
    {     
    summary = SortItemListCounters(summary);

    printf("{\"meta\":{\"count\" : %d,"
             "\"header\":{\"Promise Handle\":0,\"Report\":1,\"Occurrences\":2"
             "}},\"data\":[",ListLen(summary));

    printf("Promise Handle       Frequency  Report");
    
    for (ip = summary; ip != NULL; ip=ip->next, i++)
       {
       printf("%20.20s %10d %s\n",ip->name,ip->counter,ip->classes);
       }

    DeleteItemList(summary);
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_software_report(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg)

{
 HubSoftware *hs;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }
HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,filter,true);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %30s %20s %s\n","Hostname","Package","Version","Arch");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (HubSoftware *)rp->item;
   if (CSV)
      {
      printf("%s,%s,%s,%s\n",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));
      }
   else
      {
      printf("%25s %30s %20s %s\n",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));
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

int Nova2Txt_classes_report(char *hostkey,char *name,int regex,char *classreg)

{
  HubClass *hc;
  HubQuery *hq;
  Rlist *rp;
  mongo_connection dbconn;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)SECONDS_PER_WEEK,filter,true);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("Host  Class/Context  Probability  Uncertainty Last-Defined\n");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (HubClass *)rp->item;

   if (CSV)
      {
      printf("%s,%s,%lf,%lf,%ld\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,hc->t);
      }
   else
      {
      printf("%s %s %lf %lf %ld\n",hc->hh->hostname,hc->class,hc->prob,hc->dev,hc->t);
      }
   }

DeleteHubQuery(hq,DeleteHubClass);

CFDB_Close(&dbconn);
return true;
}

/*****************************************************************************/

int Nova2Txt_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg)

{
 char rvalBuf[CF_MAXVARSIZE];
 HubVariable *hv;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,filter);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %14s %25s %s\n","Host","Type","lval","rval");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   char typestr[CF_SMALLBUF];
   
   hv = (HubVariable *)rp->item;
   
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

   if (CSV)
      {
      printf("%s,%s,%s.%s,%s\n",hv->hh->hostname,typestr,hv->scope,hv->lval,rvalBuf);
      }
   else
      {
      printf("%25s %14s %s.%-25s %s\n",hv->hh->hostname,typestr,hv->scope,hv->lval,rvalBuf);
      }
   }

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2Txt_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg)

{ char buffer[CF_BUFSIZE];
 HubTotalCompliance *ht;
 HubQuery *hq;
 Rlist *rp;
 int icmp;
 mongo_connection dbconn;

 switch (*cmp)
    {
    case '<': icmp = CFDB_LESSTHANEQ;
        break;
    default: icmp = CFDB_GREATERTHANEQ;
        break;
    }
  
 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,true,classreg);

if (!CSV)
   {
   printf("%25s %7s %10s %10s %s\n","Host","%Kept", "%Repaired", "%Not-Kept","Last-verified");
   }
 
for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ht = (HubTotalCompliance *)rp->item;

   if (CSV)
      {
      printf("%s,%d%%,%d%%,%d%%,%s\n",ht->hh->hostname,ht->kept,ht->repaired,ht->notkept,cf_strtimestamp_local(ht->t,buffer));
      }
   else
      {
      printf("%25s %6d%% %9d%% %9d%% %s\n",ht->hh->hostname,ht->kept,ht->repaired,ht->notkept,cf_strtimestamp_local(ht->t,buffer));
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

int Nova2Txt_compliance_promises(char *hostkey,char *handle,char *status,int regex,char *classreg)

{ char buffer[CF_BUFSIZE];
 HubPromiseCompliance *hp;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;
 
if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!status)  // any
   {
   status = "x";
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,0,true,filter);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %50s %10s %s %s %s\n","Host","Promise-handle","Last state","E(Q)", "Sigma","Last-data");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hp = (HubPromiseCompliance *)rp->item;

   if (CSV)
      {
      printf("%s,%s,%s,%.2lf,%.2lf,%s\n",hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_strtimestamp_local(hp->t,buffer));
      }
   else
      {
      printf("%25s %50s %10s %.2lf %.2lf %s\n",hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,cf_strtimestamp_local(hp->t,buffer));
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

int Nova2Txt_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg)

{
 HubLastSeen *hl;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;
 char inout[CF_SMALLBUF];

/* BEGIN query document */

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
    }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,true,filter);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %11s %25s %10s %7s %12s %10s %s\n","Seen-on-host","Direction","Remote-host", "Remote-IP", "Hrs-ago", "Avg-interval", "Uncertainty", "Remote-key");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (HubLastSeen *)rp->item;
   
   switch (hl->direction)
      {
      case LAST_SEEN_DIRECTION_OUTGOING:
          snprintf(inout,CF_SMALLBUF,"by us (+)");
          break;
      case LAST_SEEN_DIRECTION_INCOMING:
          snprintf(inout,CF_SMALLBUF,"by them (-)");
          break;
      }
   
   if (CSV)
      {
      printf("%s,%s,%s,%s,%.2lf,%.2lf,%.2lf,%s\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);

      }
   else
      {
      printf("%25s %11s %25s %10s %7.2lf %12.2lf %10.2lf %s\n",
            hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,
            hl->hrsago,hl->hrsavg,hl->hrsdev,
            hl->rhost->keyhash);
      }
   }

DeleteHubQuery(hq,DeleteHubLastSeen);
CFDB_Close(&dbconn);
return true;
}

/*****************************************************************************/

int Nova2Txt_deadclient_report(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg)

{
 HubLastSeen *hl;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;
 time_t then;
 time_t now = time(NULL);
  
/* BEGIN query document */

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
    }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,true,filter);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %25s %10s %7s %12s %10s %s\n","Seen-on-host","Remote-host", "Remote-IP", "Hrs-ago", "Avg-interval", "Uncertainty", "Remote-key");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hl = (HubLastSeen *)rp->item;
   
   switch (hl->direction)
      {
      case LAST_SEEN_DIRECTION_OUTGOING:
         break;

      case LAST_SEEN_DIRECTION_INCOMING:
         continue;
      }
   
   then = hl->t;

   if (then < now - CF_HUB_HORIZON)
      {
      if (CSV)
         {
         printf("%s,%s,%s,%.2lf,%.2lf,%.2lf,%s\n",
             hl->hh->hostname,hl->rhost->hostname,hl->rhost->ipaddr,
             hl->hrsago,hl->hrsavg,hl->hrsdev,
             hl->rhost->keyhash);

         }
      else
         {
         printf("%25s %25s %10s %7.2lf %12.2lf %10.2lf %s\n",
             hl->hh->hostname,hl->rhost->hostname,hl->rhost->ipaddr,
             hl->hrsago,hl->hrsavg,hl->hrsdev,
             hl->rhost->keyhash);
         }
      }
   }

DeleteHubQuery(hq,DeleteHubLastSeen);
CFDB_Close(&dbconn);
return true;
}

/*****************************************************************************/

int Nova2Txt_setuid_report(char *hostkey,char *file,int regex,char *classreg)

{
 HubSetUid *hS;   
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,filter);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%25s %s\n","Host","File");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hS = ( HubSetUid *)rp->item;

   if (CSV)
      {
      printf("%s,%s\n",hS->hh->hostname,hS->path);
      }
   else
      {
      printf("%25s %s\n",hS->hh->hostname,hS->path);
      }
   }

DeleteHubQuery(hq,DeleteHubSetUid);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

 return true;
}


/*****************************************************************************/

int Nova2Txt_bundle_report(char *hostkey,char *bundle,int regex,char *classreg,PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
 HubBundleSeen *hb;   
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;
 char header[CF_BUFSIZE]={0};
 int margin = 0,headerLen=0,noticeLen=0;
 int truncated = false;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,filter,true);
 DeleteHostClassFilter(filter);
 
 PageRecords(&(hq->records),page,DeleteHubBundleSeen);
 snprintf(header,sizeof(header),
          "\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"Hours Ago\":3,\"Avg Interval\":4,\"Uncertainty\":5,"
	  "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
          "}",page->totalResultCount);

 headerLen = strlen(header);
 noticeLen = strlen(CF_NOTICE_TRUNCATED);
 StartJoin(returnval,"{\"data\":[",bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hb = ( HubBundleSeen *)rp->item;

    if (strcmp(hb->bundle,"QUERY") == 0)
       {
       continue;
       }

    if(strcmp(hb->nid,CF_NONOTE) == 0)
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
		"[\"add\",\"%s\",%d,\"%s\",\"\"]],",
		hb->hh->hostname,hb->bundle,hb->t,
		hb->hrsago,hb->hrsavg,hb->hrsdev,
		hb->hh->keyhash,CFREPORT_BUNDLE,hb->bundle);
       }
    else
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
		"[\"show\",\"\",\"\",\"\",\"%s\"]],",
		hb->hh->hostname,hb->bundle,hb->t,
		hb->hrsago,hb->hrsavg,hb->hrsdev,
		hb->nid);
       }
    margin = headerLen + noticeLen + strlen(buffer);
    if(!JoinMargin(returnval,buffer,NULL,bufsize,margin))
       {
       truncated = true;
       break;
       }
    }

 ReplaceTrailingChar(returnval,',','\0');
 EndJoin(returnval,"]",bufsize);

 Nova_AddReportHeader(header,truncated,buffer,sizeof(buffer)-1);
 Join(returnval,buffer,bufsize);
 EndJoin(returnval,"}}\n",bufsize);
 
 DeleteHubQuery(hq,DeleteHubBundleSeen);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg)

{ char buffer[CF_SMALLBUF];
  HubFileChanges *hC;
  HubQuery *hq;
  Rlist *rp;
  int icmp;
  mongo_connection dbconn;
 
switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,true,filter,false);
DeleteHostClassFilter(filter);

if (!CSV)
   {
   printf("%s,%s,%s,%s\n","Host","File", "Changed-on", "Note");
   }

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hC = (HubFileChanges *)rp->item;

   if (CSV)
      {
      if (strcmp(hC->nid,CF_NONOTE) == 0)
         {
         printf("%s,%s,%s\n",hC->hh->hostname,hC->path,cf_strtimestamp_local(hC->t,buffer));
         }
      else
         {
         printf("%s,%s,%s,%s\n",hC->hh->hostname,hC->path,cf_strtimestamp_local(hC->t,buffer),hC->nid);
         }
      }
   else
      {
      if (strcmp(hC->nid,CF_NONOTE) == 0)
         {
         printf("%25s %40s  %20s\n",hC->hh->hostname,hC->path,cf_strtimestamp_local(hC->t,buffer));
         }
      else
         {
         printf("%25s %40s %20s %s\n",hC->hh->hostname,hC->path,cf_strtimestamp_local(hC->t,buffer),hC->nid);
         }
      }
   }

DeleteHubQuery(hq,DeleteHubFileChanges);   
CFDB_Close(&dbconn);
return true;
}

/*****************************************************************************/

int Nova2Txt_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg)

{
  HubFileDiff *hd;
  HubQuery *hq;
  Rlist *rp;
  int icmp;
  mongo_connection dbconn;

switch (*cmp)
   {
   case '<': icmp = CFDB_LESSTHANEQ;
       break;
   default: icmp = CFDB_GREATERTHANEQ;
       break;
   }

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,true,filter,false);
DeleteHostClassFilter(filter);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hd = (HubFileDiff *)rp->item;

   if (CSV)
      {
      char *sp;
      // If the formatted difference contains new lines, we will break csv formatting, so purge

      for (sp = hd->diff; *sp != '\0'; sp++)
         {
         if (*sp == '\n')
            {
            *sp = ';';
            }
         }
      
      printf("%s,%s,%ld,%s\n",hd->hh->hostname,hd->path,hd->t,Nova_FormatDiff(hd->diff));
      }
   else
      {
      printf("%s file %s changed on %ld\n%s\n",hd->hh->hostname,hd->path,hd->t,Nova_FormatDiff(hd->diff));
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
/* Search for hosts with property X,Y,Z                                      */
/*****************************************************************************/

int Nova2Txt_hostinfo(char *hostkey,char *hostnameOut,char *ipaddrOut,int bufsize)

{ char buffer1[CF_BUFSIZE],buffer2[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int count1 = 0,count2 = 0,tmpsize1,tmpsize2;
 mongo_connection dbconn;
 bson query;
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

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryHosts(&dbconn,MONGO_DATABASE,cfr_keyhash,&query);
bson_destroy(&query);

hostnameOut[0] = '\0';
ipaddrOut[0] = '\0';

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (HubHost *)rp->item;
   
   snprintf(buffer1,CF_MAXVARSIZE-1,"%s ",hh->hostname);
   snprintf(buffer2,CF_MAXVARSIZE-1,"%s ",hh->ipaddr);
   
   tmpsize1 = strlen(buffer1);
   tmpsize2 = strlen(buffer2);
   
   if (count1 + tmpsize1 <= bufsize - 1)
      {
      strcat(hostnameOut,buffer1);
      count1 += tmpsize1;
      }
   
   if (count2 + tmpsize2 <= bufsize - 1)
      {
      strcat(ipaddrOut,buffer2);
      count2 += tmpsize2;
      }
   }

DeleteHubQuery(hq,NULL);

ReplaceTrailingChar(hostnameOut, ' ', '\0');
ReplaceTrailingChar(ipaddrOut, ' ', '\0');

CFDB_Close(&dbconn);

return true;
}
/*****************************************************************************/

int Nova2Txt_value_hosts(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize)

{
  HubQuery *hq;
  Rlist *rp;
  mongo_connection dbconn;
  char buffer[CF_BUFSIZE]={0};
  int counter = 0, n = 180;
  HubHost *hh;

/* BEGIN query document */

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,true,filter);
DeleteHostClassFilter(filter);

StartJoin(returnval,"[",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (HubHost *)rp->item;
   
   counter++;
   snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]",bufsize);

DeleteHubQuery(hq,DeleteHubValue);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2Txt_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;


 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,filter,false);
 DeleteHostClassFilter(filter);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;

    counter++;
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }
 
 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubSoftware);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_classes_hosts(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

/* BEGIN query document */
 
 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)SECONDS_PER_WEEK,filter,false);
 DeleteHostClassFilter(filter);
 
 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;

    counter++;
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }

    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubClass);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_vars_hosts(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
  HubHost *hh;
  HubQuery *hq;
  Rlist *rp;
  int counter = 0, n = 180;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,filter);
DeleteHostClassFilter(filter);

StartJoin(returnval,"[",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (HubHost *)rp->item;
   counter++;
   snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]",bufsize);

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2Txt_compliance_hosts(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180,icmp;
 mongo_connection dbconn;

 switch (*cmp)
    {
    case '<': icmp = CFDB_LESSTHANEQ;
        break;
    default: icmp = CFDB_GREATERTHANEQ;
        break;
    }
  
 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryTotalCompliance(&dbconn,hostkey,version,t,k,nk,rep,icmp,false,classreg);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;   
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);
 
 DeleteHubQuery(hq,DeleteHubTotalCompliance);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_promise_hosts(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
  HubHost *hh;
  HubQuery *hq;
  Rlist *rp;
  int counter = 0, n = 180;
  mongo_connection dbconn;


if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!status)  // any
   {
   status = "x";
   }


HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,0,false,filter);
DeleteHostClassFilter(filter);

StartJoin(returnval,"[",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (HubHost *)rp->item;
   counter++;   
   snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   
   if (counter > n && counter % 6 == 0)
      {
      break;
      }
   }

ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]",bufsize);

DeleteHubQuery(hq,DeleteHubPromiseCompliance);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2Txt_lastseen_hosts(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryLastSeen(&dbconn,hostkey,lhash,lhost,laddress,lago,lregex,false,filter);
 DeleteHostClassFilter(filter);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;   
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubLastSeen);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_performance_hosts(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,false,classreg);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;   
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubPerformance);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_setuid_hosts(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,filter);
 DeleteHostClassFilter(filter);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;   
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubSetUid);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}


/*****************************************************************************/

int Nova2Txt_bundle_hosts(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,filter,false);
 DeleteHostClassFilter(filter);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;   
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubBundleSeen);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_filechanges_hosts(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
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

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,false,filter,false);
 DeleteHostClassFilter(filter);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]",bufsize);

 DeleteHubQuery(hq,DeleteHubFileChanges);

 CFDB_Close(&dbconn);
 
 return true;
}

/*****************************************************************************/

int Nova2Txt_filediffs_hosts(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubHost *hh;
 HubQuery *hq;
 Rlist *rp;
 int counter = 0, n = 180,icmp;
 mongo_connection dbconn;


 switch (*cmp)
    {
    case '<': icmp = CFDB_LESSTHANEQ;
        break;
    default: icmp = CFDB_GREATERTHANEQ;
        break;
    }

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 HostClassFilter *filter = NewHostClassFilter(classreg, NULL);
 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,false,filter,false);
 DeleteHostClassFilter(filter);
 
 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
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

int Nova2Txt_promiselog_hosts(char *hostkey,char *handle, PromiseLogState state,time_t from,time_t to,char *classreg,char *returnval,int bufsize)
{
 HubHost *hh;
 HubQuery *hq;
 mongo_connection dbconn;
 Rlist *rp;
 int counter = 0, n = 180;
 char buffer[CF_BUFSIZE];

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,state,handle,true,from,to,false,classreg);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (HubHost *)rp->item;
    counter++;
    snprintf(buffer,CF_MAXVARSIZE,"{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},",hh->keyhash,hh->hostname,hh->ipaddr);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
   
    if (counter > n && counter % 6 == 0)
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval) - 1] = '\0';
    }

 EndJoin(returnval,"]\n",bufsize);
 DeleteHubQuery(hq,DeleteHubFileDiff);


 CFDB_Close(&dbconn);

 return true;
}

/*****************************************************************************/

int Nova2Txt_get_classes_for_bundle(char *name,char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
 Rlist *classList,*rp;
 Item *ip,*list = NULL;
 char work[CF_MAXVARSIZE],context[CF_MAXVARSIZE];
 int pid;
 char jsonEscapedStr[CF_MAXVARSIZE] = {0};

 if (!CFDB_Open(&dbconn))
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
   
    snprintf(buffer,bufsize,"[");
   
    for (ip = list; ip != NULL; ip=ip->next)
       {
       snprintf(context,CF_MAXVARSIZE,"class_contexts::%s",ip->name);
       pid = Nova_GetTopicIdForTopic(context);

       EscapeJson(ip->name,jsonEscapedStr,sizeof(jsonEscapedStr));
       snprintf(work,CF_MAXVARSIZE,"[%d,\"%s\"],",pid,jsonEscapedStr);

       if(!Join(buffer,work,bufsize))
          {
          break;
          }
       }
    
    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer,"]",bufsize);

    DeleteItemList(list);
    }

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2Txt_get_args_for_bundle(char *name,char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
  Item *matched,*ip;
  char work[CF_MAXVARSIZE];
  
if (!CFDB_Open(&dbconn))
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

int Nova2Txt_list_all_bundles(char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
  char work[CF_BUFSIZE];
  Item *matched,*ip;

Nova_WebTopicMap_Initialize();

if (!CFDB_Open(&dbconn))
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
      snprintf(buffer,bufsize,
               "{\"meta\":{\"header\": {\"Type\":0,\"Service bundle name\":1,\"Description\":2,"
               "\"Contributing to Goals\":{\"index\":3,\"subkeys\":{\"pid\":0,\"name\":1,\"description\":2}},"
               "\"\":4"
               "}},\"data\":[");
      }
   else
      {
      snprintf(buffer,bufsize,"[");
      }
   
   for (ip = matched; ip != NULL; ip=ip->next)
      {
      Item *ip2,*glist = Nova_GetBusinessGoals(ip->name);
      char goals[CF_BUFSIZE];
      char colour[CF_SMALLBUF];
      
      if (type && glist)
         {
         snprintf(goals,sizeof(goals),"[");

         for (ip2 = glist; ip2 != NULL; ip2=ip2->next)
            {
            snprintf(work,sizeof(work),"[%d,\"%s\",\"%s\"],",ip2->counter,ip2->name,ip2->classes);
            
            if (!Join(goals,work,CF_BUFSIZE))
               {
               break;
               }
            }
         
         ReplaceTrailingChar(goals, ',', '\0');
         EndJoin(goals,"]",sizeof(goals));

         snprintf(colour,CF_SMALLBUF,"green");
         }
      else if (type)
         {
         snprintf(goals,CF_MAXVARSIZE,"[[-1,\"Unknown\",\"Unknown\"]]");
         snprintf(colour,CF_SMALLBUF,"yellow");
         }

      if (type)
         {
         snprintf(work,CF_BUFSIZE,"[\"%s\",\"%s\",\"%s\",%s,\"%s\"],",ip->classes,ip->name,Nova_GetBundleComment(ToLowerStr(ip->name)),goals,colour);
         }
      else
         {
         snprintf(work,CF_BUFSIZE,"[\"%s\",\"%s\"],",ip->classes,ip->name);
         }
      
      if(!Join(buffer,work,bufsize))
         {
         break;
         }
      DeleteItemList(glist);
      }

   ReplaceTrailingChar(buffer, ',', '\0');
   if (type)
      {
      strcat(buffer,"]}\n");
      }
   else
      {
      strcat(buffer,"]\n");
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

int Nova2Txt_get_bundle_type(char *name,char *buffer,int bufsize)

{ mongo_connection dbconn;
  int matched = false;

Nova_WebTopicMap_Initialize();

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

matched = CFDB_QueryBundleType(&dbconn,name,buffer,bufsize);
 
if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

if (matched)
   {
   return true;
   }
else
   {
   return false;
   }
}
/*****************************************************************************/
int Nova2Txt_list_all_goals(char *buffer,int bufsize)

{
 if (Nova_GetUniqueBusinessGoals(buffer,bufsize))
    {
         return true;
    }
 else
    {
    return false;
    }

}

/*****************************************************************************/

int Nova2Txt_list_bundles_using(char *name,char *buffer,int bufsize)

{ mongo_connection dbconn;
  char work[CF_MAXVARSIZE];
  Item *matched,*ip;

if (!CFDB_Open(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return -1;
   }

matched = CFDB_QueryBundlesUsing(&dbconn,name);

matched = SortItemListClasses(matched);

if (matched)
   {
   snprintf(buffer,bufsize,"[");
   
   for (ip = matched; ip != NULL; ip=ip->next)
      {
      snprintf(work,CF_MAXVARSIZE,"{\"bundletype\":\"%s\",\"bundlename\":\"%s\"},",ip->classes,ip->name);
      
      if(!Join(buffer,work,bufsize))
         {
         break;
         }
      }

   ReplaceTrailingChar(buffer, ',', '\0');
   EndJoin(buffer,"]",bufsize);
   DeleteItemList(matched);
   }

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2Txt_get_bundle_count(char *buffer, int bufsize)

{ mongo_connection dbconn;
 int bundleCount = 0;

 if (!CFDB_Open(&dbconn))
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

int Nova2Txt_get_promise_count(char *buffer, int bufsize)

{ mongo_connection dbconn;
 int promiseCount = 0;

 if (!CFDB_Open(&dbconn))
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

int Nova2Txt_docroot(char *buffer,int bufsize)
{
 Nova_WebTopicMap_Initialize();
 snprintf(buffer,bufsize,"%s",DOCROOT);
 return true;
}

/*****************************************************************************/

int Nova2Txt_search_topics(char *search,int regex,char *buffer,int bufsize)

{ int pid;

if ((pid = Nova_SearchTopicMap(search,buffer,bufsize)))
   {
   return true;
   }

return false;
}

/*****************************************************************************/

void Nova2Txt_show_topic(int id,char *buffer,int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE];

buffer[0] = '\0';

if (Nova_GetTopicByTopicId(id,topic_name,topic_id,topic_context))
   {
   snprintf(buffer,bufsize,"{\"topic\":\"%s\",\"context\":\"%s\"}",topic_name,topic_context);
   }
else
   {
   snprintf(buffer,bufsize,"No such topic\n");
   }
}

/*****************************************************************************/

void Nova2Txt_show_topic_leads(int id,char *buffer,int bufsize)

{ Item *ip;
  Item *list = Nova_ScanLeadsAssociations(id,NULL);
  char work[CF_BUFSIZE];

  DebugListItemList(list);
  
buffer[0] = '\0';
strcpy(buffer,"[ ");

// name contains the association
// classes contains the related topic
// counter contains the topic id

for (ip = list; ip != NULL; ip=ip->next)
   {
   if (ip == list)
      {
      snprintf(work,CF_BUFSIZE,"{ \"assoc\": \"%s\", \"topics\": [",ip->name);
      Join(buffer,work,bufsize);
      }

   snprintf(work,CF_BUFSIZE,"{ \"topic\": \"%s\", \"id\": %d },",ip->classes,ip->counter);
   Join(buffer,work,bufsize);
   
   if (ip->next && strcmp(ip->name,ip->next->name) != 0)
      {
      strcpy(buffer+strlen(buffer)-1,"]},");
      snprintf(work,CF_BUFSIZE,"{ \"assoc\": \"%s\", \"topics\": [",ip->next->name);
      Join(buffer,work,bufsize);
      }
   }

strcpy(buffer+strlen(buffer)-1,"]} ]"); 
}

/*****************************************************************************/

void Nova2Txt_show_topic_hits(int id,char *buffer,int bufsize)

{
buffer[0] = '\0';
Nova_ScanOccurrences(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2Txt_show_topic_category(int id,char *buffer,int bufsize)

{
buffer[0] = '\0';
Nova_ScanTheRest(id,buffer,bufsize);
}

/*****************************************************************************/
/* Hosts stats                                                               */
/*****************************************************************************/

void Nova2Txt_show_topN(char *policy,int n,PageInfo *page,char *buffer,int bufsize)

{ Item *ip,*clist;
 char work[CF_BUFSIZE] = {0};
 static char *policies[] = { "compliance", "anomaly", "performance", "lastseen", NULL};
 enum cf_rank_method pol;
 int startIndex=0,endIndex=0, total=0, count = -1;
 
Nova_WebTopicMap_Initialize();

for (pol = 0; policies[pol] != NULL; pol++)
   {
   if (strcmp(policy,policies[pol]) == 0)
      {
      break;
      }
   }

clist = Nova_RankHosts(NULL,0,pol,-1);

buffer[0] = '\0';
strcat(buffer,"{\"data\":[");

startIndex = page->resultsPerPage*(page->pageNum - 1);
endIndex = (page->resultsPerPage*page->pageNum) - 1;

for (ip = clist; (ip !=  NULL); ip=ip->next)
   {
   if(Nova_IsBlue(ip->counter))
      {
      continue;
      }
   else
      {
      count++; // starts from 0
      total++; 
      }
   
   if (count >= startIndex && (count <= endIndex || endIndex <= 0))
      {
      work[0] = '\0';
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
      
      if (strlen(work) > 0)
         {
         if (!Join(buffer,work,bufsize))
            {
            break;
            }
         }
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');

snprintf(work,sizeof(work),"],\"meta\":{\"count\":%d}}",total);
EndJoin(buffer,work,bufsize);

DeleteItemList(clist);
}

/*****************************************************************************/

void Nova2Txt_show_all_hosts(char *policy,int n,char *buffer,int bufsize)

{ Item *ip,*clist;
 char work[CF_MAXVARSIZE];

 Nova_WebTopicMap_Initialize();  
 clist = Nova_RankHosts(policy,1,cfrank_compliance,n);
 clist = SortItemListClasses(clist);

 buffer[0] = '\0';
 strcat(buffer,"[");

 for (ip = clist; ip != NULL; ip=ip->next)
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

int Nova2Txt_show_hosts(char *hostNameRegex,char *ipRegex,char *classRegex,PageInfo *page,char *buf,int bufsize)

{
 HubQuery *hq;
 HubHost *hh;
 Rlist *rp;
 mongo_connection dbconn;
 char work[CF_MAXVARSIZE];

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryHostsByAddress(&dbconn,hostNameRegex,ipRegex,classRegex);

 CFDB_Close(&dbconn);
 
 if(!(hq && hq->hosts))
    {
    StartJoin(buf, "{}", bufsize);
    return true;
    }
    
 PageRecords(&(hq->hosts),page,DeleteHubHost);
 
 snprintf(work,sizeof(work),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Key Hash\":0,\"Host name\":1,\"IP address\":2"
          "}},\n\"data\":[",page->totalResultCount);

 StartJoin(buf,work,bufsize);
 
 for (rp = hq->hosts; rp != NULL; rp=rp->next)
       {
       hh = (HubHost *)rp->item;
       
       snprintf(work, sizeof(work), "[\"%s\", \"%s\", \"%s\"]\n,",
                hh->hostname, hh->ipaddr, hh->keyhash);

       if(!Join(buf,work,bufsize))
          {
          break;
          }
       }
    
 DeleteHubQuery(hq,NULL);
    
 ReplaceTrailingChar(buf, ',', '\0');

 EndJoin(buf,"]}",bufsize);

 return true;

}

/*****************************************************************************/

void Nova2Txt_show_col_hosts(char *colour,int n,PageInfo *page,char *buffer,int bufsize)

{ Item *ip,*clist;
 char work[CF_MAXVARSIZE],lastseen[CF_MAXVARSIZE]={0};
 int counter = 0, startIndex, endIndex;
  
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
    startIndex = page->resultsPerPage*(page->pageNum - 1);
    endIndex = (page->resultsPerPage*page->pageNum) - 1;
    
    buffer[0] = '\0';
    strcat(buffer,"{ \"data\":[");

    for (ip = clist,counter=0; ip !=  NULL; ip=ip->next, counter++)
       {
       if(counter>=startIndex && (counter<=endIndex || endIndex < 0))
          {
          if (strcmp(colour,"blue") == 0)
             {
             Nova2Txt_getlastupdate(ip->name,lastseen,sizeof(lastseen));
             snprintf(work,CF_MAXVARSIZE,"{ \"key\": \"%s\", \"id\": \"%s\",\"lastseen\": \"%s\"},",ip->name,ip->classes,lastseen);
             }
          else
             {
             snprintf(work,CF_MAXVARSIZE,"{ \"key\": \"%s\", \"id\": \"%s\"},",ip->name,ip->classes);
             }

          if (!Join(buffer,work,bufsize))
             {
             break;
             }
          }
       }
    
    ReplaceTrailingChar(buffer, ',', '\0');

    snprintf(work,sizeof(work),"],\"meta\":{\"count\":%d}}",counter);
    Join(buffer,work,bufsize);
    
    DeleteItemList(clist);
    }
}

/*****************************************************************************/

char *Nova2Txt_GetPromiseComment(char *handle)
    
{ static char buffer[CF_BUFSIZE];
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn))
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

char *Nova2Txt_GetPromiseBundle(char *handle)
    
{ static char buffer[CF_BUFSIZE];
 mongo_connection dbconn;

 if(strcmp(handle,"internal_promise") == 0)
    {
    return "";
    }

 if (!CFDB_Open(&dbconn))
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

void Nova2Txt_GetPromiseBody(char *name,char *type,char *returnval,int bufsize)
    
{ char work[CF_BUFSIZE];
 mongo_connection dbconn;
 HubBody *hb;    

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return;
    }

 hb = CFDB_QueryBody(&dbconn,type,name);

 if (hb)
    {
    snprintf(returnval,CF_MAXVARSIZE-1,"{");
   
    snprintf(work,CF_MAXVARSIZE-1,"\"Type\":\"%s\",",hb->bodyType);
    Join(returnval,work,bufsize);

    snprintf(work,CF_MAXVARSIZE-1,"\"Name\":\"%s\",",hb->bodyName);
    Join(returnval,work,bufsize);
         
    if (hb->bodyArgs)
       {
       snprintf(work,CF_MAXVARSIZE-1,"\"Arguments\":\"%s\",",hb->bodyArgs);
       Join(returnval,work,bufsize);
       }
   
    if (hb->attr)
       {
       HubBodyAttr *ha; 
       Join(returnval,"\"attributes\":[",bufsize);
       for (ha = hb->attr; ha != NULL; ha = ha->next)
          {
          snprintf(work,CF_MAXVARSIZE-1,"{\"lval\":\"%s\",\"rval\":\"%s\",\"class_context\":\"%s\"},",ha->lval,ha->rval,ha->classContext);
          Join(returnval,work,bufsize);
          }

       ReplaceTrailingChar(returnval, ',', '\0');
       EndJoin(returnval,"]",bufsize);
       }
   
    DeleteHubBody(hb);
    EndJoin(returnval,"}",bufsize);
    }

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }
}

/*****************************************************************************/

int Nova2Txt_list_bodies(char *name,char *type,char *returnval,int bufsize)

{ mongo_connection dbconn;
 char work[CF_MAXVARSIZE];
 Item *all_bodies,*ip;    

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return -1;
    }

 all_bodies = CFDB_QueryAllBodies(&dbconn,type,name);
 all_bodies = SortItemListNames(all_bodies);

 if (all_bodies)
    {
    snprintf(returnval,CF_MAXVARSIZE-1,"[");
    for (ip = all_bodies; ip != NULL; ip=ip->next)
       {
       snprintf(work,CF_MAXVARSIZE-1,"{\"body\":\"%s\",\"type\":\"%s\"},",ip->name,ip->classes);
       Join(returnval,work,bufsize);
       }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval,"]",bufsize);
    }

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

char *Nova2Txt_GetPromiseType(char *handle)
    
{ static char buffer[CF_BUFSIZE];
 mongo_connection dbconn;

 if(strcmp(handle,"internal_promise") == 0)
    {
    return "";
    }

 if (!CFDB_Open(&dbconn))
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

char *Nova2Txt_GetPromiser(char *handle)
    
{ static char buffer[CF_BUFSIZE];
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn))
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

int Nova2Txt_get_variable(char *hostkey,char *scope,char *lval,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 HubVariable *hv;
 HubQuery *hq;
 Rlist *rp;
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,NULL,NULL,false,NULL);

 returnval[0] = '\0';

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hv = (HubVariable *)rp->item;

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
void Nova2Txt_select_reports(char *buffer,int bufsize)

{ char work[CF_MAXVARSIZE];
  int i;

buffer[0] = '\0';
strcat(buffer,"[");

for (i = 0; i < cfrep_unknown; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"[\"%s\",\"%s\"]",BASIC_REPORTS[i].category,BASIC_REPORTS[i].name);
   if (i != cfrep_unknown - 1)
      {
      strcat(work,",");
      }
   Join(buffer,work,bufsize);
   }

ReplaceTrailingChar(buffer, ',', '\0');
EndJoin(buffer,"]",bufsize);
}

/*****************************************************************************/

int Nova2Txt_summarize_promise(char *handle, char *returnval,int bufsize)

{ mongo_connection dbconn;
 HubPromise *hp;
 char promiseeText[CF_MAXVARSIZE];
 char commentText[CF_MAXVARSIZE];
 char work[CF_MAXVARSIZE];
 int i;
  
 if(strcmp(handle,"internal_promise") == 0)
    {
    snprintf(returnval, bufsize, "This is a promise made internally by Cfengine, and is thus not part of your policy.");
    return true;
    }


 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hp = CFDB_QueryPromise(&dbconn, handle, NULL, 0);

 if (!hp)
    {
    snprintf(returnval, bufsize, " Promise '%s' was not found in the database.", handle);
    CFDB_Close(&dbconn);
    return false;
    }

 returnval[0] = '\0';

 strcat(returnval,"{");

 snprintf(work,CF_MAXVARSIZE-1,"\"bundletype\":\"%s\",\"bundlename\":\"%s\",",hp->bundleType,hp->bundleName);
 Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"\"handle\":\"%s\",",hp->handle);
 Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"\"promiser\":\"%s\",",hp->promiser);
 Join(returnval,work,bufsize);

 if (EMPTY(hp->promisee))
    {
    strcpy(promiseeText,"None mentioned");
    }
 else
    {
    snprintf(promiseeText,sizeof(promiseeText),"%s",hp->promisee);     
    }

 snprintf(work,CF_MAXVARSIZE-1,"\"promisee\":\"%s\",",promiseeText);
 Join(returnval,work,bufsize);

 if (EMPTY(hp->comment))
    {
    strcpy(commentText,"No comment");
    }
 else
    {
    snprintf(commentText, sizeof(commentText),"%s",hp->comment);
    }

 snprintf(work,CF_MAXVARSIZE-1,"\"comment\":\"%s\",",commentText);
 Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"\"promise_type\":\"%s\",",hp->promiseType);
 Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"\"class_context\":\"%s\",",hp->classContext);
 Join(returnval,work,bufsize);

 snprintf(work,CF_MAXVARSIZE-1,"\"file\":\"%s\",\"line_num\":%d,",hp->file,hp->lineNo);
 Join(returnval,work,bufsize);

 if (hp->constraints)
    {
     snprintf(work,CF_MAXVARSIZE-1,"\"body\":[");
     Join(returnval,work,bufsize);
    for(i = 0; hp->constraints[i] != NULL; i++)
       {
       char lval[CF_MAXVARSIZE],rval[CF_MAXVARSIZE],args[CF_MAXVARSIZE];

       args[0] = '\0';
       sscanf(hp->constraints[i],"%255s => %1023[^(,;]%[^\n]",lval,rval,args);

       if (strcmp(lval,"usebundle") == 0)
          {
          snprintf(work,CF_MAXVARSIZE-1,"{\"constraint_type\":\"bundle\",\"type\":\"%s\",\"name\":\"%s\",\"args\":\"%s\"},",lval,rval,args);
          }
       else
          {
          snprintf(work,CF_MAXVARSIZE-1,"{\"constraint_type\":\"body\",\"type\":\"%s\",\"name\":\"%s\",\"args\":\"%s\"},",lval,rval,args);
          }
      
       Join(returnval,work,bufsize);   
       }
    
    if (i == 0)
       {
       Join(returnval,"\"body content is implicit, no futher details\"",bufsize);
       }
    
    ReplaceTrailingChar(returnval, ',', '\0');
    strcat(returnval,"]");
    }
 
 ReplaceTrailingChar(returnval, ',', '\0');
 strcat(returnval,"}");
    
 DeleteHubPromise(hp);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}
/*****************************************************************************/
int Nova2Txt_list_promise_handles_with_comments(char *bundle,char *btype,char *returnval,int bufsize)

{ mongo_connection dbconn;
 char work[CF_BUFSIZE];
 Rlist *rp;
 HubQuery *hq;
 HubPromise *hp;
  
/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryHandlesForBundlesWithComments(&dbconn,btype,bundle);

 CFDB_Close(&dbconn);

 returnval[0] = '\0';

 if(hq)
    {
    StartJoin(returnval, "[", bufsize);
        
    for (rp = hq->records; rp != NULL; rp=rp->next)
       {
       hp = (HubPromise *)rp->item;
       snprintf(work,CF_BUFSIZE-1,"[\"%s\", \"%s\"],",(char*)hp->handle, (char*)hp->comment);
       Join(returnval,work,bufsize);
       }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval,"]",bufsize);
    
    DeleteHubQuery(hq,DeleteHubPromise);

    return true;
    }
 else  // no result
    {
    return false;
    }

}
/*****************************************************************************/

int Nova2Txt_list_promise_handles(char *promiser,char *ptype,char *bundle,char *btype,int regex,char *returnval,int bufsize)

{ mongo_connection dbconn;
 char work[CF_MAXVARSIZE];
 Rlist *rp;
 HubQuery *hq;
 HubPromise *hp;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseHandles(&dbconn,promiser,ptype,btype,bundle,regex,false);

 CFDB_Close(&dbconn);

 
 returnval[0] = '\0';

 if(hq)
    {
    StartJoin(returnval, "[", bufsize);
        
    for (rp = hq->records; rp != NULL; rp=rp->next)
       {
       hp = (HubPromise *)rp->item;
       snprintf(work,CF_MAXVARSIZE-1,"\"%s\",",(char*)hp->handle);
       Join(returnval,work,bufsize);
       }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval,"]",bufsize);
    
    DeleteHubQuery(hq,DeleteHubPromise);

    return true;
    }
 else  // no result
    {
    return false;
    }

}

/*****************************************************************************/

void Nova2Txt_get_network_speed(char *hostkey,char *buffer, int bufsize)

{ mongo_connection dbconn;
 mongo_cursor *cursor;
 bson_buffer bb;
 bson query,field;
 int found = false;
 Event e;

 if (!CFDB_Open(&dbconn))
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

 cursor = mongo_find(&dbconn,MONGO_DATABASE,&query,&field,0,0,CF_MONGO_SLAVE_OK);
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

void Nova2Txt_ComplianceSummaryGraph(char *policy, char *buffer,int bufsize)

{
ComplianceSummaryGraph(NULL,policy,false,buffer,bufsize);
}

/*****************************************************************************/

int Nova2Txt_validate_policy(char *file,char *buffer,int bufsize)

{ char cmd[CF_BUFSIZE];
  char tmp[CF_MAXVARSIZE];
  int retVal;
  FILE *pp;

 // NOTE: this is run as the web user

snprintf(cmd,CF_BUFSIZE,"/var/cfengine/bin/cf-promises -n -f \"%s\"",file);

if ((pp = cf_popen(cmd,"r")) == NULL)
   {
   snprintf(buffer,bufsize,"Could not run command \"%s\": %s", cmd, GetErrorStr());
   return -1;
   }

buffer[0] = '\0';

while(!feof(pp))
   {
   if (fgets(tmp,sizeof(tmp),pp))
      {
      Join(buffer,tmp,bufsize);
      }
   }

retVal = cf_pclose(pp);
return retVal; // 0 on success
}

/*****************************************************************************/

int Nova2Txt_list_handles_policy_finder(char *handle,char *promiser,char *bundle,int escRegex,char *returnval,int bufsize)

{ mongo_connection dbconn;
  char work[CF_MAXVARSIZE] = {0};
  char promiserJson[CF_BUFSIZE];
  Rlist *rp;
  HubPromise *hp;
  HubQuery *hq;

  /* BEGIN query document */

  if (!CFDB_Open(&dbconn))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }

  hq = CFDB_QueryPolicyFinderData(&dbconn,handle,promiser,bundle,escRegex);
  returnval[0] = '\0';

  if(hq)
    {
    StartJoin(returnval, "[", bufsize);
    for (rp = hq->records; rp != NULL; rp=rp->next)
      {
      hp = (HubPromise *)rp->item;
      snprintf(work,sizeof(work),"[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"],",
               (char*)hp->handle,
               (char*)hp->promiseType,
               (char *)hp->bundleName,
               (char*)hp->bundleType,
               (char*)EscapeJson(hp->promiser,promiserJson,CF_BUFSIZE-1));
      Join(returnval,work,bufsize);
      }

    CFDB_Close(&dbconn);
    
    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);
    
    DeleteHubQuery(hq,DeleteHubPromise);
    return true;
    }
  else  // no result
    {
    CFDB_Close(&dbconn);
    return false;
    }
}

#endif
