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
{
#ifdef HAVE_LIBMONGOC
char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c
 
if (false)
   {
   //   Nova2PHP_software_report(0,0,0,0,0,cfr_software,NULL,buffer,20);
   //   Nova2PHP_classes_report(NULL,NULL,0,NULL,buffer,1000);
   //   Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,NULL,buffer,1000);
   //   Nova2PHP_compliance_promises(NULL,NULL,"x",0,NULL,buffer,10000);
   //   Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,NULL,buffer,10000);
   //   Nova2PHP_bundles_report(NULL,NULL,0,NULL,buffer,10000);
   //   Nova2PHP_performance_report(NULL,NULL,0,NULL,buffer,10000);
   //   Nova2PHP_setuid_report(NULL,NULL,0,NULL,buffer,10000);
   //   Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",NULL,buffer,10000);
   //   Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",NULL,buffer,10000);
   //   Nova2PHP_value_report(NULL,NULL,NULL,NULL,NULL,buffer,1000);
   //   Nova2PHP_promiselog(NULL,NULL,1,0,0,NULL,buffer,1000);
   Nova2PHP_promises(NULL, NULL, NULL, 0);
   //   Nova2PHP_getlastupdate(NULL,buffer,10);

   CFDB_PutValue("one_two","three");
   CFDB_GetValue("newvar",buffer,120);
   Nova2PHP_count_hosts();
   Nova2PHP_count_red_hosts();
   Nova2PHP_count_yellow_hosts();
   Nova2PHP_count_green_hosts();
   Nova_Header("123","123",buffer,1000);
   Nova_Footer(buffer,1000);
   Nova2PHP_choose_hosts("123",2,buffer,1000);
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
   Nova2PHP_add_new_note(NULL,NULL, -1,NULL, -1, NULL,NULL,100);
   Nova2PHP_add_note(NULL,NULL,-1,NULL,NULL,100);
   Nova2PHP_get_notes(NULL,NULL, NULL,-1,-1,NULL,NULL,10000);
   Nova2PHP_get_host_noteid(NULL,NULL,4096);
   
   Nova2PHP_get_knowledge_view(0,NULL,NULL,999);
   }
#endif
}

#ifdef HAVE_LIBMONGOC

/*****************************************************************************/
/* Helper functions                                                          */
/****************************************************************************/

void Nova2PHP_get_knowledge_view(int pid,char *view,char *buffer,int bufsize)
    
{
Nova_PlotTopicCosmos(pid,view,buffer,bufsize);
}

/****************************************************************************/

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
/* Vitals functions                                                          */
/*****************************************************************************/

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 bool ret = false;
 char work[CF_MAXVARSIZE];
 time_t lastUpdate = 0;
 char hostName[CF_MAXVARSIZE], ipAddress[CF_MAXVARSIZE];
 struct HubVital *res, *hv;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 res = CFDB_QueryVitalsMeta(&dbconn, keyHash);

 strcpy(buffer,"{");

 Nova2PHP_hostinfo(keyHash, hostName, ipAddress, sizeof(hostName));
 CFDB_QueryLastUpdate(&dbconn, keyHash, &lastUpdate);

 CFDB_Close(&dbconn);
 
 
 snprintf(work, sizeof(work), "\"hostname\" : \"%s\", \"ip\" : \"%s\", \"ls\" : %ld, \n\"obs\" : [",
          hostName, ipAddress, lastUpdate);

 Join(buffer,work,bufsize);

 
 for(hv = res; hv != NULL; hv = hv->next)
    {
    snprintf(work, sizeof(work), "{\"id\":\"%s\", \"units\":\"%s\", \"desc\":\"%s\"},\n",
             hv->id, hv->units, hv->description);
    Join(buffer, work, bufsize);
    
    ret = true;
    }

 DeleteHubVital(res);
 
 int buflen = strlen(buffer);
 if(buffer[buflen - 2] == ',')
    {
    buffer[buflen - 2] = '\0';
    }
 
 Join(buffer, "]}", bufsize);
 
 return ret;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 struct CfDataView cfv = {0};
 char work[CF_MAXVARSIZE];
 bool haveData = false;
 int i;
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 haveData = Nova_ReadMagTimeSeries2(&dbconn, &cfv, keyHash, vitalId);

 CFDB_Close(&dbconn);

 strcpy(buffer,"[");
 
if (haveData)
   {
   for (i = 0; i < CF_MAGDATA; i++)
      {
      snprintf(work,sizeof(work)," [%d,%lf,%lf,%lf],",i, cfv.data_q[i], cfv.data_E[i],cfv.bars[i]);
      Join(buffer, work, bufsize);
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');
Join(buffer,"]",bufsize);

return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 struct CfDataView cfv = {0};
 char work[CF_MAXVARSIZE];
 bool haveData = false;
 int i;
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 haveData = Nova_ReadWeekTimeSeries2(&dbconn, &cfv, keyHash, vitalId);

 CFDB_Close(&dbconn);
 
 strcpy(buffer,"[");
 
if (haveData)
   {
   for (i = 0; i < CF_TIMESERIESDATA; i++)
      {
      snprintf(work,sizeof(work)," [%d,%lf,%lf,%lf],",i, cfv.data_q[i], cfv.data_E[i],cfv.bars[i]);
      Join(buffer, work, bufsize);
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');
Join(buffer,"]",bufsize);

return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 struct CfDataView cfv = {0};
 char work[CF_MAXVARSIZE];
 bool haveData = false;
 int i;
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 haveData = Nova_ReadYearTimeSeries(&dbconn, &cfv, keyHash, vitalId);

 CFDB_Close(&dbconn);
 
 strcpy(buffer,"[");
 
if (haveData)
   {
   for (i = 0; i < CF_MAGDATA; i++)
      {
      snprintf(work,sizeof(work)," [%d,%lf,%lf,%lf],",i, cfv.data_q[i], cfv.data_E[i],cfv.bars[i]);
      Join(buffer, work, bufsize);
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');
Join(buffer,"]",bufsize);

return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 struct CfDataView cfv = {0};
 char work[CF_MAXVARSIZE];
 bool haveData = false;
 int i;
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }
 
 haveData = Nova_ReadHistogram2(&dbconn, &cfv, keyHash, vitalId);

 CFDB_Close(&dbconn);

 strcpy(buffer,"[");

if (haveData)
   {
   for (i = 0; i < CF_GRAINS; i++)
      {
      snprintf(work,sizeof(work)," [%d,%d],", i, (int)round(cfv.data_E[i]));
      Join(buffer, work, bufsize);
      }
   }

ReplaceTrailingChar(buffer, ',', '\0');
Join(buffer,"]",bufsize);

return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
 mongo_connection dbconn;
 char work[CF_BUFSIZE];
 struct CfDataView cfv = {0};
  
 buffer[0] = '\0';

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 if (!Nova_ReadMagTimeSeries2(&dbconn,&cfv,hostkey,vitalId))
    {
    CFDB_Close(&dbconn);
    return false;
    }

 CFDB_Close(&dbconn);
  
 strcpy(buffer,"[");
  
 if (cfv.max - cfv.min < cfv.error_scale * 2)
    {
    snprintf(work,sizeof(work),"\"No significant variations\",");
    Join(buffer,work,bufsize);
    }
 else
    {
    snprintf(work,sizeof(work),"\"Significant variations\",");
    Join(buffer,work,bufsize);
    }
  
 snprintf(work,sizeof(work),"\"Maximum value: %lf\",", cfv.max);
 Join(buffer,work,bufsize);
 snprintf(work,sizeof(work),"\"Minimum value: %lf\",", cfv.min);
 Join(buffer,work,bufsize);
 snprintf(work,sizeof(work),"\"Average variability: %lf\",", cfv.error_scale);
 Join(buffer,work,bufsize);
 Join(buffer,"]",bufsize);
 
 return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize)

{ char work[CF_BUFSIZE];
 double x;
 struct CfDataView cfv = {0};
 mongo_connection dbconn;

 buffer[0] = '\0';

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 if (!Nova_ReadWeekTimeSeries2(&dbconn,&cfv,hostkey,vitalId))
    {
    CFDB_Close(&dbconn);
    return false;
    }

 CFDB_Close(&dbconn);
 
 strcpy(buffer,"[");

 snprintf(work,CF_BUFSIZE-1,"\"Maximum value: %lf\",",cfv.max);
 Join(buffer,work,bufsize);
 snprintf(work,CF_BUFSIZE-1,"\"Minimum value %lf\",",cfv.min);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.over/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage over average/normal: %lf%%\",",x);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.under/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage under average/normal: %lf%%\",",x);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.over_dev1/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage 1 deviation over mean: %lf%%\",",x);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.under_dev1/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage 1 deviation under mean: %lf%%\",",x);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.over_dev2/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage 2 deviations over mean: %lf%%\",",x);
 Join(buffer,work,bufsize);
 x = 100*(double)cfv.under_dev2/(double)CF_TIMESERIESDATA;
 snprintf(work,CF_BUFSIZE-1,"\"Percentage 2 deviations under mean: %lf%%\"",x);
 Join(buffer,work,bufsize);
 Join(buffer,"]",bufsize);

 return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
 char work[CF_BUFSIZE];
 struct CfDataView cfv = {0};
 mongo_connection dbconn;

 buffer[0] = '\0';

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }
 
 if (!Nova_ReadYearTimeSeries(&dbconn,&cfv,hostkey,vitalId))
    {
    CFDB_Close(&dbconn);
    return false;
    }

 CFDB_Close(&dbconn);

 strcpy(buffer,"[");

 snprintf(work,CF_BUFSIZE,"\"Maximum value: %.2lf\",",cfv.max);
 Join(buffer,work,bufsize);
 snprintf(work,CF_BUFSIZE,"\"Minimum value: %.2lf\",",cfv.min);
 Join(buffer,work,bufsize);
 snprintf(work,CF_BUFSIZE,"\"Average variability: %lf\"",cfv.error_scale);
 Join(buffer,work,bufsize);
 Join(buffer,"]",bufsize);


 return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char work[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct CfDataView cfv = {0};
  struct Item *spectrum;
  mongo_connection dbconn;

  buffer[0] = '\0';

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if (!Nova_ReadHistogram2(&dbconn,&cfv,hostkey,vitalId))
   {
   CFDB_Close(&dbconn);
   return false;
   }

CFDB_Close(&dbconn);

spectrum = Nova_MapHistogram(&cfv,hostkey);

strcpy(buffer,"[");

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

snprintf(work,CF_BUFSIZE-1,"Maximum observed %s = %.2lf\",",vitalId,cfv.max);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"Minimum observed %s = %.2lf\",",vitalId,cfv.min);
Join(buffer,work,bufsize);

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];

   above_noise = (delta*delta > sigma2) * sensitivity_factor;
   
   if (above_noise)
      {
      new_gradient = delta;

      if (new_gradient < 0 && past_gradient >= 0)
         {
         max++;

         snprintf(work,CF_BUFSIZE-1,"\"%d: Spectral mode with peak at %.0lf/%.0lf grains, ",max,sx-1,(double)CF_GRAINS);
         Join(buffer,work,bufsize);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            snprintf(work,CF_BUFSIZE-1,"red-shifted, e.g. a retardation process where usage is declining. "
                     "If the distribution is skewed, it has a long ramp, indicating "
                     "a possible resource ceiling, a well-utilized system. "
                     "Or there could be outliers of low value, because data are incomplete.\",");
            Join(buffer,work,bufsize);
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            snprintf(work,CF_BUFSIZE-1,"blue-shifted, e.g. an acceleration process where usage is increasing. "
                     "If the distribution is skewed, it has a long tail, indicating "
                    "plenty of resources, or an under-used system. "
                    "Or there could be outliers of low value, because data are incomplete.\",");
            Join(buffer,work,bufsize);            
            }
         }
      }
   
   past_gradient = new_gradient;
   }

Join(buffer,work,bufsize);
Join(buffer,"]",bufsize);
DeleteItemList(spectrum);

return true;
}


/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2PHP_promiselog(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE], canonifiedCause[CF_BUFSIZE]={0};
 struct HubPromiseLog *hp;  struct HubQuery *hq;
 struct Rlist *rp;
 int reportType;
 mongo_connection dbconn;
  
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,from,to,true,classreg);
 PageRecords(&(hq->records),page,DeleteHubPromiseLog);
 
 snprintf(buffer,sizeof(buffer), 
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\":{\"Host\":0,\"Promise Handle\":1,\"Report\":2,\"Time\":3,"
	  "\"Note\":{\"index\":4,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (struct HubPromiseLog *)rp->item;
    EscapeJson(hp->cause,canonifiedCause,sizeof(canonifiedCause));
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

int Nova2PHP_promiselog_summary(char *hostkey,char *handle,enum promiselog_rep type,time_t from, time_t to,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE],hostname[CF_MAXVARSIZE],report[CF_BUFSIZE]={0};
 struct HubPromiseLog *hp;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;
 struct Item *ip,*summary = NULL;
 int startIndex = 0, endIndex=0, i = 0;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPromiseLog(&dbconn,hostkey,type,handle,true,from,to,false,classreg);
 
 hostname[0] = '\0';

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (struct HubPromiseLog *)rp->item;
    ip = IdempPrependItem(&summary,hp->handle,hp->cause);
    ip->counter++;

    if (hostname[0] == '\0')
       {
       strncpy(hostname,hp->hh->hostname,CF_MAXVARSIZE);
       }
    }

 DeleteHubQuery(hq,DeleteHubPromiseLog);

 CFDB_Close(&dbconn);

 startIndex = page->resultsPerPage*(page->pageNum - 1);
 endIndex = (page->resultsPerPage*page->pageNum) - 1;

 if (summary == NULL)
    {
    snprintf(returnval,bufsize,"No data to report on");
    }
 else
    {     
    summary = SortItemListCounters(summary);
    snprintf(buffer,sizeof(buffer),
             "{\"meta\":{\"count\" : %d,"
             "\"header\":{\"Host\":0,\"Promise Handle\":1,\"Report\":2,\"Occurrences\":3"
             "}},\"data\":[",ListLen(summary));
     
    StartJoin(returnval,buffer,bufsize);
   
    for (ip = summary; ip != NULL; ip=ip->next, i++)
       {
       if(i>=startIndex && (i<=endIndex || endIndex < 0))
          {
          EscapeJson(ip->classes,report,sizeof(report));
          snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",\"%s\",%d],",
                   hostname,ip->name,report,ip->counter);
       
          if(!Join(returnval,buffer,bufsize))
             {
             break;
             }
          }
       }

    ReplaceTrailingChar(returnval, ',', '\0');

    EndJoin(returnval,"]}\n",bufsize);
    DeleteItemList(summary);
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_value_report(char *hostkey,char *day,char *month,char *year,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ struct HubValue *hp;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;
 char buffer[CF_BUFSIZE]={0};

/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryValueReport(&dbconn,hostkey,day,month,year,true,classreg);

 PageRecords(&(hq->records),page,DeleteHubValue);
 snprintf(buffer,sizeof(buffer),
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\":{\"Host\":0,\"Day\":1,\"Kept\":2,\"Repaired\":3,\"Not Kept\":4,"
	  "\"Note\":{\"index\":5,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (struct HubValue *)rp->item;

    if(strcmp(hp->nid,CF_NONOTE) == 0)
       {
       snprintf(buffer,sizeof(buffer),
		"[\"%s\",\"%s\",%.1lf,%.1lf,%.1lf,"
		"[\"add\",\"%s\",%d,\"%s\",\"\"]],",
		hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept,
		hp->hh->keyhash,CFREPORT_VALUE,hp->handle);
       }
    else
       {
       snprintf(buffer,sizeof(buffer),
		"[\"%s\",\"%s\",%.1lf,%.1lf,%.1lf,"
		"[\"show\",\"\",\"\",\"\",\"%s\"]],",
		hp->hh->hostname,hp->day,hp->kept,hp->repaired,hp->notkept,
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
 EndJoin(returnval,"]}\n",bufsize);

 DeleteHubQuery(hq,DeleteHubValue);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/
int Nova2PHP_get_value_graph(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize)

{ struct HubValue *hp;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;
 char buffer[CF_BUFSIZE]={0};
 
/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryValueGraph(&dbconn,hostkey,day,month,year,true,classreg);

 strcpy(returnval,"[");

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (struct HubValue *)rp->item;
    
    snprintf(buffer,sizeof(buffer),
	     "[%s,%.1lf,%.1lf,%.1lf],",
	     hp->day,hp->kept,hp->repaired,hp->notkept);

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }
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

{ char buffer[CF_BUFSIZE];
 struct HubSoftware *hs;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;


 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }


 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,classreg,true);
 PageRecords(&(hq->records),page,DeleteHubSoftware);

 snprintf(buffer,sizeof(buffer),
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\": {\"Host\":0,\"Name\":1,\"Version\":2,\"Architecture\":3"
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hs = (struct HubSoftware *)rp->item;
    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",\"%s\",\"%s\"],",hs->hh->hostname,hs->name,hs->version,Nova_LongArch(hs->arch));

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }
 EndJoin(returnval,"]}\n",bufsize);


 DeleteHubQuery(hq,DeleteHubSoftware);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_classes_report(char *hostkey,char *name,int regex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
 struct HubClass *hc;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;

/* BEGIN query document */
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }


 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);
 PageRecords(&(hq->records),page,DeleteHubClass);

 snprintf(buffer,sizeof(buffer),
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\": {\"Host\":0,\"Class Context\":1,\"Occurs with Probability\":2,\"Uncertainty\":3,\"Last seen\":4"
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hc = (struct HubClass *)rp->item;
   
    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%lf,%lf,%ld],",hc->hh->hostname,hc->class,hc->prob,hc->dev,hc->t);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }
 EndJoin(returnval,"]}\n",bufsize);
 DeleteHubQuery(hq,DeleteHubClass);

 CFDB_Close(&dbconn);

 return true;
}

/*****************************************************************************/

int Nova2PHP_listclasses_time(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  struct Rlist *rp, *rp2;
  mongo_connection dbconn;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

rp = CFDB_QueryDateTimeClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);

StartJoin(returnval,"[",bufsize);

for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
   {
     snprintf(buffer,sizeof(buffer),"\"%s\",",(char*)rp2->item);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   }

if (returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval)-1]='\0';
   }

EndJoin(returnval,"]\n",bufsize);

 DeleteRlist(rp);
CFDB_Close(&dbconn);

return true;
}

/*****************************************************************************/

int Nova2PHP_listclasses_soft(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
 struct Rlist *rp, *rp2;
 mongo_connection dbconn;

/* BEGIN query document */
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

rp = CFDB_QuerySoftClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);

StartJoin(returnval,"[",bufsize);

for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
   {
     snprintf(buffer,sizeof(buffer),"\"%s\",",(char *)rp2->item);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   }

if (returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval)-1]='\0';
   }

EndJoin(returnval,"]\n",bufsize);

 DeleteRlist(rp);
CFDB_Close(&dbconn);

return true;
}

/*****************************************************************************/

int Nova2PHP_listclasses_ip(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  struct Rlist *rp, *rp2;
  mongo_connection dbconn;

/* BEGIN query document */
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }


rp = CFDB_QueryIpClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);
 
StartJoin(returnval,"[",bufsize);

for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
   {
     snprintf(buffer,sizeof(buffer),"\"%s\",",(char *)rp2->item);
   
   if(!Join(returnval,buffer,bufsize))
      {
      break;
      }
   }

if(returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval)-1]='\0';
   }

EndJoin(returnval,"]\n",bufsize);

 DeleteRlist(rp);
CFDB_Close(&dbconn);

return true;
}

/*****************************************************************************/
int Nova2PHP_listclasses_host(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  struct Rlist *rp, *rp2;
  mongo_connection dbconn;

  /* BEGIN query document */
  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }

  rp = (void*)CFDB_QueryHostClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);
  StartJoin(returnval,"[",bufsize);
  for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
    {
      snprintf(buffer,sizeof(buffer),"\"%s\",",(char *)rp2->item);

      if(!Join(returnval,buffer,bufsize))
        {
          break;
        }
    }
  if (returnval[strlen(returnval)-1]==',')
    {
      returnval[strlen(returnval)-1]='\0';
    }
  EndJoin(returnval,"]\n",bufsize);

  DeleteRlist(rp);
  CFDB_Close(&dbconn);
  return true;
}
/*****************************************************************************/
int Nova2PHP_listclasses_all(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  struct Rlist *rp, *rp2;
  mongo_connection dbconn;
  /* BEGIN query document */

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
      CfOut(cf_verbose,"", "!! Could not open connection to report database");
      return false;
    }
  
  rp = CFDB_QueryAllClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,true);
  
  StartJoin(returnval,"[",bufsize);
  
  for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
    {
      snprintf(buffer,sizeof(buffer),"\"%s\",",(char*)rp2->item);

      if(!Join(returnval,buffer,bufsize))
        {
          break;
        }
    }

  if (returnval[strlen(returnval)-1]==',')
    {
      returnval[strlen(returnval)-1]='\0';
    }
  EndJoin(returnval,"]\n",bufsize);

  DeleteRlist(rp);
  CFDB_Close(&dbconn);

  return true;
}

/*****************************************************************************/

int Nova2PHP_classes_summary(char **classes, char *buf, int bufsize)

{ mongo_connection dbconn;
 struct HubQuery *hq;
 struct HubClassSum *hc;
 struct HubHost *hh;
 struct Rlist *rp;
 char work[CF_MAXVARSIZE];
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryClassSum(&dbconn,classes);

 StartJoin(buf, "{", bufsize);

 if(hq && hq->hosts)
    {

    Join(buf, "\"hosts\":[", bufsize);
    
    for(rp = hq->hosts; rp != NULL; rp = rp->next)
       {
       hh = (struct HubHost *)rp->item;
       snprintf(work, sizeof(work), "[\"%s\",\"%s\"]\n,", hh->hostname, hh->keyhash);
       
       if(!Join(buf, work, bufsize - 10))
          {
          break;
          }           
       }

    ReplaceTrailingChar(buf, ',', '\0');

    Join(buf, "]", bufsize);
    Join(buf, ",\n\"classes\":[", bufsize - 10);
    
    for(rp = hq->records; rp != NULL; rp = rp->next)
       {
       hc = (struct HubClassSum *)rp->item;
       snprintf(work, sizeof(work), "[\"%s\",%d]\n,", hc->class, hc->frequency);

       if(!Join(buf, work, bufsize - 10))
          {
          break;
          }
       }

    ReplaceTrailingChar(buf, ',', '\0');

    Join(buf, "]", bufsize);
    
    }

 EndJoin(buf, "}", bufsize);
 
 DeleteHubQuery(hq,DeleteHubClassSum);

 CFDB_Close(&dbconn);

 return true;
}

/*****************************************************************************/

int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE],lscope[CF_MAXVARSIZE],canonifiedValue[CF_BUFSIZE]={0};
 char rvalBuf[CF_MAXVARSIZE];
 struct HubVariable *hv;
 struct HubQuery *hq;

 struct Rlist *rp;
 mongo_connection dbconn;
  
 int first = true, countadded=false;
 int scope_record_count = 0,last_scope_record_count=0,first_scope_record_count=0, meta_len=0;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,rval,type,regex,classreg);

 CountMarginRecordsVars(&(hq->records),page,&first_scope_record_count,&last_scope_record_count);
 PageRecords(&(hq->records),page,DeleteHubVariable);

 lscope[0] = '\0';

 snprintf(buffer,sizeof(buffer),"{\"meta\":{\"count\":%d},",page->totalResultCount);
 meta_len=strlen(buffer);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    char typestr[CF_SMALLBUF];
   
    hv = (struct HubVariable *)rp->item;
    if (strcmp(lscope,hv->scope) != 0)
       {
       strcpy(lscope,hv->scope);
      
       if(strlen(buffer)>meta_len)
          {	  
	  returnval[strlen(returnval)-1] = '\0';
	  snprintf(buffer,CF_BUFSIZE,"],\"count\":%d},",first?first_scope_record_count:scope_record_count);
	  Join(returnval,buffer,bufsize); // end scope
	  scope_record_count=0;
	  first=false;
          }
     
       snprintf(buffer,CF_BUFSIZE,"\"%s\":{"
                "\"header\":{\"Host\":0,\"Type\":1,\"Name\":2,\"Value\":3},"
                "\"data\":[",hv->scope);
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

    if (strlen(hv->dtype) > 1) // list
       {
       PrintRlist(rvalBuf,sizeof(rvalBuf),hv->rval);
       }
    else
       {
       snprintf(rvalBuf,sizeof(rvalBuf),"%s",(char *)hv->rval);
       }

    EscapeJson(rvalBuf,canonifiedValue,sizeof(canonifiedValue));

    snprintf(buffer,CF_BUFSIZE,
             "[\"%s\",\"%s\",\"%s\",\"%s\",%ld],",
             hv->hh->hostname,typestr,hv->lval,canonifiedValue,hv->t);

    if(!Join(returnval,buffer,bufsize))
       {
       countadded=true;
       break;
       }
    scope_record_count++;
    }

 if (!countadded )
    {
    returnval[strlen(returnval)-1] = '\0';
    snprintf(buffer,CF_BUFSIZE,"],\"count\":%d}",last_scope_record_count);
    Join(returnval,buffer,bufsize); // end scope
    }else if(first)
    {
    returnval[strlen(returnval)-1] = ']';
    }
 else
    {
    returnval[strlen(returnval)-1] = '\0';
    }

 EndJoin(returnval,"}\n",bufsize);

 DeleteHubQuery(hq,DeleteHubVariable);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/
int Nova2PHP_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,struct PageInfo *page, char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubTotalCompliance *ht;
 struct HubQuery *hq;
 struct Rlist *rp;
 int icmp;
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
 PageRecords(&(hq->records),page,DeleteHubTotalCompliance);

 snprintf(buffer,sizeof(buffer),
	  "{\"meta\":{\"count\" : %d,"
	  "\"header\": {\"Host\":0,\"Policy\":1,\"Kept\":2,\"Repaired\":3,\"Not Kept\":4,\"Last seen\":5" 
	  "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    ht = (struct HubTotalCompliance *)rp->item;

    snprintf(buffer,sizeof(buffer),
             "[\"%s\",\"%s\",%d,%d,%d,%ld],",
             ht->hh->hostname,ht->version,ht->kept,ht->repaired,ht->notkept,ht->t);

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }
 EndJoin(returnval,"]}\n",bufsize);
 
 DeleteHubQuery(hq,DeleteHubTotalCompliance);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
} 

/*****************************************************************************/

int Nova2PHP_compliance_promises(char *hostkey,char *handle,char *status,int regex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubPromiseCompliance *hp;
 struct HubQuery *hq;
 struct Rlist *rp;
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
 PageRecords(&(hq->records),page,DeleteHubPromiseCompliance);

 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"Promise Handle\":1,\"Last Known State\":2,\"Probability Kept\":3,\"Uncertainty\":4,\"Last seen\":5"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hp = (struct HubPromiseCompliance *)rp->item;

    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",\"%s\",%.2lf,%.2lf,%ld],",
             hp->hh->hostname,hp->handle,Nova_LongState(hp->status),hp->e,hp->d,hp->t);
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1]='\0';
    }

 EndJoin(returnval,"]}\n",bufsize);
 DeleteHubQuery(hq,DeleteHubPromiseCompliance);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubLastSeen *hl;
 struct HubQuery *hq;
 struct Rlist *rp;
 int count = 0;
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
 PageRecords(&(hq->records),page,DeleteHubLastSeen);

 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"Initiated\":1,\"Remote host name\":2,\"Remote IP address\":3,\"Last seen\":4,\"Hours ago\":5,\"Avg interval\":6,\"Uncertainty\":7,\"Remote host key\":8"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 count += strlen(returnval);

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
   
    snprintf(buffer,sizeof(buffer),
             "[\"%s\",\"%s\",\"%s\",\"%s\",%ld,"
             "%.2lf,%.2lf,%.2lf,\"%s\"],",
             hl->hh->hostname,inout,hl->rhost->hostname,hl->rhost->ipaddr,hl->t,
             hl->hrsago,hl->hrsavg,hl->hrsdev,
             hl->rhost->keyhash);
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]}\n",bufsize);

 DeleteHubQuery(hq,DeleteHubLastSeen);

 CFDB_Close(&dbconn);

 return true;
}

/*****************************************************************************/
int Nova2PHP_performance_report(char *hostkey,char *job,int regex,char *classreg,struct PageInfo *page, char *returnval,int bufsize)

{ char *report,buffer[CF_BUFSIZE];
 struct HubPerformance *hP;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryPerformance(&dbconn,hostkey,job,regex,true,classreg);
 PageRecords(&(hq->records),page,DeleteHubPerformance);

 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"Event\":1,\"Last time\":2,\"Avg Time\":3,\"Uncertainty\":4,\"Last performed\":5,"
	  "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hP = ( struct HubPerformance *)rp->item;

    if(strcmp(hP->nid,CF_NONOTE) == 0)
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%.2lf,%.2lf,%.2lf,%ld,"
		"[\"add\",\"%s\",%d,\"%s\",\"\"]],",
		hP->hh->hostname,hP->event,hP->q,hP->e,hP->d,hP->t,
		hP->hh->keyhash,CFREPORT_PERFORMANCE,hP->handle);
       }
    else
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%.2lf,%.2lf,%.2lf,%ld,"
		"[\"show\",\"\",\"\",\"\",\"%s\"]],",
		hP->hh->hostname,hP->event,hP->q,hP->e,hP->d,hP->t,
		hP->nid);
       }
   
    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1] = '\0';
    }
 EndJoin(returnval,"]}\n",bufsize);

 DeleteHubQuery(hq,DeleteHubPerformance);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_report(char *hostkey,char *file,int regex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubSetUid *hS;   
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QuerySetuid(&dbconn,hostkey,file,regex,classreg);
 PageRecords(&(hq->records),page,DeleteHubSetUid);

 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"File\":1"
          "}},\"data\":[", page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hS = ( struct HubSetUid *)rp->item;

    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\"],",hS->hh->hostname,hS->path);

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1] = '\0';
    }
 EndJoin(returnval,"]}\n",bufsize);

 DeleteHubQuery(hq,DeleteHubSetUid);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}


/*****************************************************************************/

int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,char *classreg,struct PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubBundleSeen *hb;   
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;

/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryBundleSeen(&dbconn,hostkey,bundle,regex,classreg,true);
 PageRecords(&(hq->records),page,DeleteHubBundleSeen);
 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"Hours Ago\":3,\"Avg Interval\":4,\"Uncertainty\":5,"
	  "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hb = ( struct HubBundleSeen *)rp->item;

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

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval)-1] = '\0'; 
    }
 EndJoin(returnval,"]}",bufsize);

 DeleteHubQuery(hq,DeleteHubBundleSeen);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,struct PageInfo *page, int lookInArchive,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubFileChanges *hC;
 struct HubQuery *hq;
 struct Rlist *rp;
 int icmp;
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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,true,classreg, lookInArchive);
 PageRecords(&(hq->records),page,DeleteHubFileChanges);

 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"File\":1,\"Change Detected at\":2,"
	  "\"Note\":{\"index\":3,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hC = (struct HubFileChanges *)rp->item;
              
    if(strcmp(hC->nid,CF_NONOTE) == 0)
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,"
                "[\"add\",\"%s\",%d,\"%s\",\"\"]],",
                hC->hh->hostname,hC->path,hC->t,
                hC->hh->keyhash,CFREPORT_FILECHANGES,hC->handle);
       }
    else
       {
       snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,"
                "[\"show\",\"\",\"\",\"\",\"%s\"]],",
                hC->hh->hostname,hC->path,hC->t,
                hC->nid);
       }     

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }

 ReplaceTrailingChar(returnval, ',', '\0');
 EndJoin(returnval,"]}\n",bufsize);
   
 DeleteHubQuery(hq,DeleteHubFileChanges);
   
 CFDB_Close(&dbconn);
   
 return true;
}

/*****************************************************************************/

int Nova2PHP_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,struct PageInfo *page, int lookInArchive,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];   
 struct HubFileDiff *hd;
 struct HubQuery *hq;
 struct Rlist *rp;
 int icmp;
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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,true,classreg,lookInArchive);
 PageRecords(&(hq->records),page,DeleteHubFileDiff);
 
 snprintf(buffer,sizeof(buffer),
          "{\"meta\":{\"count\" : %d,"
          "\"header\": {\"Host\":0,\"File\":1,\"Change Detected at\":2,"
	  "\"Change\":{\"index\":3,\"subkeys\":{\"plusminus\":0,\"line\":1,\"diff\":2}}"
          "}},\"data\":[",page->totalResultCount);
 StartJoin(returnval,buffer,bufsize);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hd = (struct HubFileDiff *)rp->item;

    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,%s],",
             hd->hh->hostname,hd->path,hd->t,Nova_FormatDiff(hd->diff));

    if(!Join(returnval,buffer,bufsize))
       {
       break;
       }
    }

 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval) - 1] = '\0'; 
    }
 EndJoin(returnval,"]}\n",bufsize);

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

int Nova2PHP_hostinfo(char *hostkey,char *hostnameOut,char *ipaddrOut,int bufsize)

{ char buffer1[CF_BUFSIZE],buffer2[CF_BUFSIZE];
 struct HubHost *hh;
 struct HubQuery *hq;
 struct Rlist *rp;
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

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryHosts(&dbconn,&query);
 bson_destroy(&query);

 hostnameOut[0] = '\0';
 ipaddrOut[0] = '\0';

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;

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

int Nova2PHP_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubHost *hh;
 struct HubQuery *hq;
 struct Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;


 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QuerySoftware(&dbconn,hostkey,type,name,value,arch,regex,classreg,false);


 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;

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

 DeleteHubQuery(hq,DeleteHubSoftware);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}

/*****************************************************************************/

int Nova2PHP_classes_hosts(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubHost *hh;
 struct HubQuery *hq;
 struct Rlist *rp;
 int counter = 0, n = 180;
 mongo_connection dbconn;

/* BEGIN query document */
 
 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 hq = CFDB_QueryClasses(&dbconn,hostkey,name,regex,(time_t)CF_WEEK,classreg,false);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;

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

StartJoin(returnval,"[",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
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

if (returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval) - 1] = '\0';
   }

EndJoin(returnval,"]\n",bufsize);

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

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

if (!status)  // any
   {
   status = "x";
   }


hq = CFDB_QueryPromiseCompliance(&dbconn,hostkey,handle,*status,regex,false,classreg);

StartJoin(returnval,"[",bufsize);

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   hh = (struct HubHost *)rp->item;
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

if (returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval) - 1] = '\0';
   }

EndJoin(returnval,"]\n",bufsize);

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

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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


 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,false,classreg,false);


 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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
 EndJoin(returnval,"]\n",bufsize);

 DeleteHubQuery(hq,DeleteHubFileChanges);

 CFDB_Close(&dbconn);
 
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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,false,classreg,false);

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

 StartJoin(returnval,"[",bufsize);

 for (rp = hq->hosts; rp != NULL; rp=rp->next)
    {
    hh = (struct HubHost *)rp->item;
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

int Nova2PHP_get_classes_for_bundle(char *name,char *type,char *buffer,int bufsize)

{ mongo_connection dbconn;
 struct Rlist *classList,*rp;
 struct Item *ip,*list = NULL;
 char work[CF_MAXVARSIZE],context[CF_MAXVARSIZE];
 int pid;
 char jsonEscapedStr[CF_MAXVARSIZE] = {0};

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
    strcat(buffer,"]");

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
      struct Item *ip2,*glist = Nova_GetBusinessGoals(ip->name);
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
         strcat(goals,"]");
         snprintf(colour,CF_SMALLBUF,"green");
         }
      else if (type)
         {
         snprintf(goals,CF_MAXVARSIZE,"[[-1,\"Unknown\",\"Unknown\"]]");
         snprintf(colour,CF_SMALLBUF,"yellow");
         }

      if (type)
         {
         snprintf(work,CF_BUFSIZE,"[\"%s\",\"%s\",\"%s\",%s,\"%s\"],",ip->classes,ip->name,Nova_GetBundleComment(ip->name),goals,colour);
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

int Nova2PHP_get_bundle_type(char *name,char *buffer,int bufsize)

{ mongo_connection dbconn;
  char work[CF_BUFSIZE];
  int matched = false;

Nova_WebTopicMap_Initialize();

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
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
int Nova2PHP_list_all_goals(char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 struct Item *ip2,*glist;

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
   strcat(buffer,"]\n");
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

if ((pid = Nova_SearchTopicMap(search,buffer,bufsize)))
   {
   return true;
   }

return false;
}

/*****************************************************************************/

void Nova2PHP_show_topic(int id,char *buffer,int bufsize)

{ char topic_name[CF_BUFSIZE],topic_id[CF_BUFSIZE],topic_context[CF_BUFSIZE],topic_comment[CF_BUFSIZE];

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

void Nova2PHP_show_topic_leads(int id,char *buffer,int bufsize)

{ struct Item *ip;
  struct Item *list = Nova_ScanLeadsAssociations(id,NULL);
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

void Nova2PHP_show_topic_hits(int id,char *buffer,int bufsize)

{
buffer[0] = '\0';
Nova_ScanOccurrences(id,buffer,bufsize);
}

/*****************************************************************************/

void Nova2PHP_show_topic_category(int id,char *buffer,int bufsize)

{
buffer[0] = '\0';
Nova_ScanTheRest(id,buffer,bufsize);
}

/*****************************************************************************/
/* Hosts stats                                                               */
/*****************************************************************************/

void Nova2PHP_show_topN(char *policy,int n,struct PageInfo *page,char *buffer,int bufsize)

{ struct Item *ip,*clist;
 char work[CF_BUFSIZE] = {0};
 static char *policies[] = { "compliance", "anomaly", "performance", "lastseen", NULL};
 enum cf_rank_method pol;
 int startIndex=0,endIndex=0, i=0;
 
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
strcat(buffer,"{\"data\":[");

startIndex = page->resultsPerPage*(page->pageNum - 1);
endIndex = (page->resultsPerPage*page->pageNum) - 1;

for (ip = clist, i = 0; (ip !=  NULL); ip=ip->next,i++)
   {
   if (i >= startIndex && (i <= endIndex || endIndex < 0))
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
   }

ReplaceTrailingChar(buffer, ',', '\0');

snprintf(work,sizeof(work),"],\"meta\":{\"count\":%d}}",i);
EndJoin(buffer,work,bufsize);

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

int Nova2PHP_show_hosts(char *hostNameRegex,char *ipRegex,char *classRegex,struct PageInfo *page,char *buf,int bufsize)

{
 struct HubQuery *hq;
 struct HubHost *hh;
 struct Rlist *rp;
 mongo_connection dbconn;
 char work[CF_MAXVARSIZE];

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
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
       hh = (struct HubHost *)rp->item;
       
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

void Nova2PHP_show_col_hosts(char *colour,int n,struct PageInfo *page,char *buffer,int bufsize)

{ struct Item *ip,*clist;
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
             Nova2PHP_getlastupdate(ip->name,lastseen,sizeof(lastseen));
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

{
 mongo_connection dbconn;
 int count;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return 0;
    }

 count = CFDB_QueryHostCount(&dbconn);

 CFDB_Close(&dbconn);
 
 return count;
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
          "{\"hostkey\" : \"%s\", \"profiles\":[\"Bundles\",\"Classes\",\"Goals\",\"Promises\"]}",key);
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
       struct HubBodyAttr *ha; 
       Join(returnval,"\"attributes\":[",bufsize);
       for (ha = hb->attr; ha != NULL; ha = ha->next)
          {
          snprintf(work,CF_MAXVARSIZE-1,"{\"lval\":\"%s\",\"rval\":\"%s\",\"class_context\":\"%s\"},",ha->lval,ha->rval,ha->classContext);
          Join(returnval,work,bufsize);
          }
       ReplaceTrailingChar(returnval, ',', ']');
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
    snprintf(returnval,CF_MAXVARSIZE-1,"[");
    for (ip = all_bodies; ip != NULL; ip=ip->next)
       {
       snprintf(work,CF_MAXVARSIZE-1,"{\"body\":\"%s\",\"type\":\"%s\"},",ip->name,ip->classes);
       Join(returnval,work,bufsize);
       }
    
    ReplaceTrailingChar(returnval, ',', ']');
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

for (i = 0; BASIC_REPORTS[i][0] != NULL; i++)
   {
   snprintf(work,CF_MAXVARSIZE,"\"%s\"",BASIC_REPORTS[i][0]);
   if (BASIC_REPORTS[i+1] != NULL)
      {
      strcat(work,",");
      }
   Join(buffer,work,bufsize);
   }
 if(buffer[strlen(buffer)-1] == ',')
   {
     buffer[strlen(buffer)-1] = '\0';
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
    snprintf(returnval, bufsize, "This is a promise made internally by Cfengine, and is thus not part of your policy.");
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

 constText[0] = '\0';

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
       Join(returnval,"body content is implicit, no futher details",bufsize);
       }
    }
 ReplaceTrailingChar(returnval, ',', '\0');
 strcat(returnval,"]}");
    
 DeleteHubPromise(hp);

 if (!CFDB_Close(&dbconn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    }

 return true;
}
/*****************************************************************************/
int Nova2PHP_list_promise_handles_with_comments(char *bundle,char *btype,char *returnval,int bufsize)

{ mongo_connection dbconn;
 char work[CF_BUFSIZE];
 struct Rlist *rp;
 struct HubQuery *hq;
 struct HubPromise *hp;
  
/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
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
       hp = (struct HubPromise *)rp->item;
       snprintf(work,CF_BUFSIZE-1,"[\"%s\", \"%s\"],",(char*)hp->handle, (char*)hp->comment);
       Join(returnval,work,bufsize);
       }

    ReplaceTrailingChar(returnval, ',', '\0');
    
    EndJoin(returnval, "]", bufsize);
    
    DeleteHubQuery(hq,DeleteHubPromise);

    return true;
    }
 else  // no result
    {
    return false;
    }

}
/*****************************************************************************/

int Nova2PHP_list_promise_handles(char *promiser,char *ptype,char *bundle,char *btype,int regex,char *returnval,int bufsize)

{ mongo_connection dbconn;
 char promiseeText[CF_MAXVARSIZE],bArgText[CF_MAXVARSIZE];
 char commentText[CF_MAXVARSIZE], constText[CF_MAXVARSIZE];
 char work[CF_MAXVARSIZE];
 struct Rlist *rp;
 struct HubQuery *hq;
 struct HubPromise *hp;
 int i,count;
  
/* BEGIN query document */

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
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
       hp = (struct HubPromise *)rp->item;
       snprintf(work,CF_MAXVARSIZE-1,"\"%s\",",(char*)hp->handle);
       Join(returnval,work,bufsize);
       }

    ReplaceTrailingChar(returnval, ',', '\0');
    
    EndJoin(returnval, "]", bufsize);
    
    DeleteHubQuery(hq,DeleteHubPromise);

    return true;
    }
 else  // no result
    {
    return false;
    }

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

 snprintf(returnval,bufsize,"[");

/* Make a histogram of job vs number of hosts */

 order_results = SortItemListNames(order_results);

 for (ip = order_results; ip != NULL; ip = ip->next)
    {   
    snprintf(work,CF_BUFSIZE,"{\"name\":\"%s\",\"count\":%d},",ip->name+strlen(MONITOR_CLASS_PREFIX),ip->counter+1);
    Join(returnval,work,bufsize);
    count += ip->counter+1;
    }

 if (count == 0)
    {
    snprintf(work,CF_BUFSIZE,"{\"count\":0}");
    Join(returnval,work,bufsize);
    }
 
 ReplaceTrailingChar(returnval, ',', '\0');

 strcat(returnval,"]");
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
 ComplianceSummaryGraph(NULL,policy,false,buffer,bufsize);
}

/*****************************************************************************/

char *Nova_FormatDiff(char *s)

{ char *sp,work[CF_BUFSIZE],diff[CF_BUFSIZE],tline[CF_BUFSIZE];
 static char returnval[CF_BUFSIZE];
 char pm;
 int line = 0;

 strcpy(returnval,"[");

 for (sp = s; *sp != '\0'; sp += strlen(tline)+1)
    {
    sscanf(sp,"%c,%d,%2047[^\n]",&pm,&line,diff);
    sscanf(sp,"%2047[^\n]",tline);

    snprintf(work,sizeof(work),"[\"%c\",%d,\"%s\"],",pm,line,diff);
    if(!Join(returnval,work,sizeof(returnval)))
       {
       break;
       }
    }
 if(returnval[strlen(returnval)-1]==',')
    {
    returnval[strlen(returnval) - 1] = '\0';
    }

 EndJoin(returnval,"]\n",sizeof(returnval));
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

{ char *report,buffer[CF_BUFSIZE], date[CF_MAXVARSIZE];
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

    strncpy(date,cf_ctime(&(hb->t)),CF_MAXVARSIZE - 1);
    StripTrailingNewline(date);

    snprintf(buffer,sizeof(buffer),"%s<nc>%s<nc>%s<nc>%.2lf<nc>%.2lf<nc>%.2lf<nova_nl>",
             hb->hh->hostname,hb->bundle,date,
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

/*pid = Nova_GetTopicIdForTopic(typedName);

if (pid && Nova_GetTopicByTopicId(pid,topic_name,topic_id,topic_context))
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

 hq = CFDB_QueryFileChanges(&dbconn,hostkey,file,regex,t,icmp,true,classreg,false);

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

 hq = CFDB_QueryFileDiff(&dbconn,hostkey,file,diffs,regex,t,icmp,true,classreg,false);

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
 static char returnval[CF_BUFSIZE] = "";
 char pm;
 int line = 0;

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

for (i = 0; CDP_REPORTS[i][0] != NULL; i++)
   {
   snprintf(work,sizeof(work),"<option value=\"%s\">%s</option>\n",CDP_REPORTS[i][0],CDP_REPORTS[i][0]);
   
   if (!Join(buf,work,bufSz))
      {
      break;
      }
   }

EndJoin(buf,"\n</select>\n",bufSz);
}

/*****************************************************************************/

cdp_t CdpReportNameToType(char *reportName)

{ int i;

for (i = 0; i < cdp_unknown; i++)
   {
   if (strcmp(reportName,CDP_REPORTS[i][0]) == 0)
      {
      return (cdp_t)i;      
      }
   }

return cdp_unknown;
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

int Nova2PHP_delete_host(char *keyHash)

{ mongo_connection dbconn;
  
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
/* Multiple policy environments                                              */
/*****************************************************************************/

bool Nova2PHP_environments_list(struct EnvironmentsList **out)

{ mongo_connection dbconn;
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
   MongoCheckForError(&dbconn,"Nova2PHP_environments_list", "", NULL);
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

/*****************************************************************************/

bool Nova2PHP_environment_contents(const char *environment, struct HostsList **out)

{ mongo_connection dbconn;
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

/*****************************************************************************/

char *Nova2PHP_get_host_environment(const char *hostkey)

{ mongo_connection dbconn;
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

/*****************************************************************************/

void FreeEnvironmentsList(struct EnvironmentsList *list)
{
 while (list)
    {
    struct EnvironmentsList *next = list->next;
    free(list->name);
    free(list);
    list = next;
    }
}

/*****************************************************************************/

void FreeHostsList(struct HostsList *list)
{
 while (list)
    {
    struct HostsList *next = list->next;
    free(list->keyhash);
    free(list);
    list = next;
    }
}

/*****************************************************************************/
/* for commenting functionality */
/*****************************************************************************/

int Nova2PHP_add_note(char *noteid,char *username, time_t datetime,char* note,char *returnval, int bufsize)

{ struct Item *data = NULL;
  char msg[CF_BUFSIZE] = {0}, nid[CF_MAXVARSIZE] = {0};
  mongo_connection dbconn;
  bson_buffer bb;
  int ret = 0;

if (!noteid || strlen(noteid) == 0)
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

ret = CFDB_AddNote(&dbconn,NULL,0,nid,NULL,data);
CFDB_Close(&dbconn);
snprintf(returnval,bufsize,"%d",ret);

if (ret)
   {
   snprintf(returnval,bufsize,"%s",nid);
   }

return ret;
}

/*****************************************************************************/
/*for commenting functionality */
/*****************************************************************************/

int Nova2PHP_add_new_note(char *keyhash, char *repid, int reportType, char *username, time_t datetime, char *note,char *returnval, int bufsize)

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
        snprintf(row_name, sizeof(row_name), "%s",cfr_ip_array); // taking the IP addresses
        snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
        getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
        snprintf(row_add, sizeof(row_add),"%s",cfn_nid);
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
        level = 3;
        snprintf(row_name, sizeof(row_name), "%s.%s",cfr_performance,repid);
        snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
        getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
        snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
        break;

    case CFREPORT_VALUE: /*value report*/
        level = 3;
        snprintf(row_name, sizeof(row_name), "%s.%s",cfr_valuereport,repid);
        snprintf(db, sizeof(db), "%s",MONGO_DATABASE);
        getrow = CFDB_GetRow(&dbconn, db, &query, row_name, row, sizeof(row), level);
        snprintf(row_add, sizeof(row_add), "%s.%s",row_name,cfn_nid);
        break;
    case CFREPORT_FILECHANGES:  
        level = 3;
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
        level = 3;
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
 ret = CFDB_AddNote(&dbconn,keyhash, reportType, noteId, row, data);
 //add DBRef
 snprintf(returnval,bufsize,"%d",ret);
 if(strlen(noteId)>0 && ret)
    {
      CFDBRef_AddToRow(&dbconn, db, &query, row_add, noteId);
      snprintf(returnval,bufsize,"%s",noteId);
    }
 //TODO: delete comment if addtorow fails?
 bson_destroy(&query);
 CFDB_Close(&dbconn);
 return ret;
}

/*****************************************************************************/
/*  Commenting                                                               */
/*****************************************************************************/

int Nova2PHP_get_notes(char *keyhash, char *nid, char *username, time_t from, time_t to, struct PageInfo *page, char *returnval, int bufsize)

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

  char jsonEscapedReport[CF_BUFSIZE]={0};
  char jsonEscapedMsg[CF_BUFSIZE]={0};
  int startIndex=0,endIndex=0, count=0;
   
if (username)
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
StartJoin(returnval,"{\"data\":[",bufsize);
startIndex = page->resultsPerPage*(page->pageNum - 1);
endIndex = (page->resultsPerPage*page->pageNum) - 1;
for (rp = result; rp != NULL; rp=rp->next)
   {
   hni = ( struct HubNoteInfo *) rp->item;
   
   EscapeJson(hni->report,jsonEscapedReport,sizeof(jsonEscapedReport));
   
   for(hn = hni->note; hn != NULL; hn=hn->next, count++)
      {
      if(count>=startIndex && (count<=endIndex || endIndex < 0))
         {
         EscapeJson(hn->msg,jsonEscapedMsg,sizeof(jsonEscapedMsg));
         ReplaceTrailingChar(jsonEscapedMsg, '\n', '\0');
      
         snprintf(buffer,sizeof(buffer),"{\"user\":\"%s\",\"date\":%ld,\"message\":\"%s\",\"report\":\"%s\",\"report_type\":%d},",
                  hn->user, hn->t, jsonEscapedMsg, jsonEscapedReport, hni->reportType);
         if(!Join(returnval,buffer,bufsize))
            {
            break;
            }
         }
      }
   hni = NULL;
   }

if (returnval[strlen(returnval)-1]==',')
   {
   returnval[strlen(returnval) - 1] = '\0';
   }

snprintf(buffer,sizeof(buffer),"],\"meta\":{\"count\":%d}}\n",count);
EndJoin(returnval,buffer,bufsize);

DeleteRlist(result);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize)

{ char *report,buffer[CF_BUFSIZE];
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
#ifdef HAVE_CONSTELLATION
strlcpy(buf, Constellation_NameVersion(), bufsize);
return true;
#else
strlcpy(buf, Nova_NameVersion(), bufsize);
return true;
#endif
return false;
}

/*****************************************************************************/

int Nova2PHP_list_handles_policy_finder(char *handle,char *promiser,char *bundle,int escRegex,char *returnval,int bufsize)

{ mongo_connection dbconn;
  char promiseeText[CF_MAXVARSIZE],bArgText[CF_MAXVARSIZE];
  char commentText[CF_MAXVARSIZE], constText[CF_MAXVARSIZE];
  char work[CF_MAXVARSIZE] = {0};
  char bundleName[CF_BUFSIZE],promiserJson[CF_BUFSIZE];
  struct Rlist *rp;
  struct HubPromise *hp;
  struct HubQuery *hq;
  int i,count;

  /* BEGIN query document */

  if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
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
      hp = (struct HubPromise *)rp->item;
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



/*****************************************************************************/
/*                           Constellation                                   */
/*****************************************************************************/

void Con2PHP_ComplianceSummaryGraph(char *hubKeyHash, char *policy, char *buffer,int bufsize)
{
 char buf[CF_MAXVARSIZE];
 
#ifdef HAVE_CONSTELLATION
 
 ComplianceSummaryGraph(hubKeyHash,policy,true,buffer,bufsize);
 
#else
 
 snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_ComplianceSummaryGraph() in Nova-only environment\n");
 CfOut(cf_error, "", buf);
 
#endif
}

/*****************************************************************************/

int Con2PHP_count_hubs(char *classification, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubHost *hh;
 char buffer[CF_MAXVARSIZE];
 int result = false;
 int count = 0;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryHubs(&dbconn);

CFDB_Close(&dbconn);

count = RlistLen(hq->hosts);

snprintf(buf,bufsize,"{ hosts : %d}", count);

DeleteHubQuery(hq, NULL);

return true;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_count_hubs() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}

/*****************************************************************************/

int Con2PHP_show_hubs(char *classification, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubHost *hh;
 char row[CF_MAXVARSIZE];
 int result = false;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QueryHubs(&dbconn);

CFDB_Close(&dbconn);

StartJoin(buf,"<table>\n",bufsize);

if (hq)
   {
   for(rp = hq->hosts; rp != NULL; rp = rp->next)
      {
      hh = (struct HubHost *)rp->item;
      
      snprintf(row,sizeof(row),"<tr><td><a href=\"constellation.php?hkh=%s\">%s</a></td><td>%s</td></tr>\n",  // FIXME
               hh->keyhash, hh->keyhash, hh->hostname);
      
      Join(buf,row,bufsize);
      }
   result = true;
   }
else
   {
   result = false;
   }

EndJoin(buf,"</table>\n",bufsize);


DeleteHubQuery(hq,NULL);

return result;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_show_hubs() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}

/*****************************************************************************/

int Con2PHP_summarize_promiselog(char *hubKeyHash, enum promiselog_rep log_type, enum time_window tw, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
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

hq = CFDB_QuerySumPromiseLog(&dbconn,hubKeyHash,NULL,log_type,tw);

CFDB_Close(&dbconn);

if (!hq)
   {
   buf[0] = '\0';
   return false;
   }

StartJoin(buf,"<table>\n",bufsize);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hs = (struct HubPromiseSum *)rp->item;
   
   snprintf(buffer,sizeof(buffer),"<tr><td><a href=\"/promise/details/%s\">%s</a></td><td>%d</td><td>%d</td></tr>\n",
            hs->handle,hs->handle,hs->occurences,hs->hostOccurences);
   
   if (!Join(buf,buffer,bufsize))
      {
      break;
      }
   }

EndJoin(buf,"</table>\n",bufsize);

DeleteHubQuery(hq,DeleteHubPromiseSum);

return true;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_summarize_promiselog() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}

/*****************************************************************************/

int Con2PHP_count_promiselog(char *hubKeyHash, char *promiseHandle, enum promiselog_rep log_type, enum time_window tw, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;
 struct HubPromiseSum *hs;
 char buffer[CF_MAXVARSIZE];
 int result = false;
 int count = 0, hostCount = 0;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDB_QuerySumPromiseLog(&dbconn,hubKeyHash,promiseHandle,log_type,tw);

CFDB_Close(&dbconn);

if(hq)
   {
   for(rp = hq->records; rp != NULL; rp = rp->next)
      {
      // usually iterates once (a specific promise handle is supplied)
      hs = (struct HubPromiseSum *)rp->item;
      count += hs->occurences;
      hostCount += hs->hostOccurences;
      }
   result = true;
   }
else
   {
   result = false;
   }

snprintf(buf,bufsize,"{total : %d, hosts : %d}", count, hostCount);

DeleteHubQuery(hq,DeleteHubPromiseSum);

return result;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_count_notkept() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}

/*****************************************************************************/

int Con2PHP_reasons_promiselog(char *hubKeyHash, char *promiseHandle, enum promiselog_rep log_type, enum time_window tw, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
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

hq = CFDB_QueryReasonsPromiseLog(&dbconn,hubKeyHash,promiseHandle,log_type,tw);

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

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_reasons_promiselog() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;
#endif
}

/*****************************************************************************/

int Con2PHP_environments_list(char *hubKeyHash, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 mongo_connection dbconn;
 char buffer[CF_MAXVARSIZE];
 struct Item *environments, *ip;
 char work[CF_MAXVARSIZE];
 bson query;
 
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

if(EMPTY(hubKeyHash))
   {
   environments = CFDB_QueryDistinct(&dbconn, CONST_MONGO_BASE, "sum_compliance_wk", cfl_polname, bson_empty(&query));
   }
else
   {
   environments = CFDB_QueryDistinctStr(&dbconn, CONST_MONGO_BASE, "sum_compliance_wk", cfl_polname, cfl_hubkeyhash, hubKeyHash);
   }

CFDB_Close(&dbconn);

StartJoin(buf, "[ ", bufsize);

for (ip = environments; ip != NULL; ip = ip->next)
   {
   snprintf(work, sizeof(work), "\"%s\",", ip->name);
   Join(buf, work, bufsize);
   }

ReplaceTrailingChar(buf, ',', '\0');
EndJoin(buf, " ]", bufsize);
DeleteItemList(environments);
return true;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_environments_list() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;
#endif
}

/*****************************************************************************/

int Con2PHP_promise_popularity(char *promiseHandle, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 mongo_connection dbconn;
 char buffer[CF_MAXVARSIZE];
 int hostCount;
 double popularity = 0;
 bool res;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hostCount = CFDB_QueryHostCount(&dbconn);

if(hostCount < 0)
   {
   snprintf(buf,bufsize,"!! Error when couting hosts");
   CfOut(cf_error, "", buf);
   return false;
   }

res = CFDB_QueryPromisePopularity(&dbconn,promiseHandle,hostCount,&popularity);

CFDB_Close(&dbconn);

if(!res)
   {
   snprintf(buf,bufsize,"!! Error when couting promises");
   CfOut(cf_error, "", buf);
   return false;
   }

snprintf(buf, bufsize, "{popularity : %f}", popularity);

return true;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_promise_popularity() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}

/*****************************************************************************/

int Con2PHP_rank_promise_popularity(bool sortAscending, char *buf, int bufsize)

// TODO: Pageinfo

{

#ifdef HAVE_CONSTELLATION

 struct HubQuery *hq;
 struct HubPromise *hp;
 struct Rlist *rp;
 mongo_connection dbconn;
 char work[CF_MAXVARSIZE];
 void *sortFn;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to report database");
    return false;
    }

 if(sortAscending)
    {
    sortFn = SortPromisePopularAscending;
    }
 else
    {
    sortFn = SortPromisePopularDescending;
    }

 StartJoin(buf,"<table>\n",bufsize);
 
 hq = CFDB_RankPromisePopularity(&dbconn,false);

 CFDB_Close(&dbconn);
 
 if(hq && hq->records)
    {
    hq->records = SortRlist(hq->records,sortFn);
    
    for (rp = hq->records; rp != NULL; rp=rp->next)
       {
       hp = (struct HubPromise *)rp->item;
       
       snprintf(work, sizeof(work), "<tr><td><a href=\"/promise/details/%s\">%s</a></td><td>%f</td></tr>\n",
                hp->handle, hp->handle, hp->popularity);

       if(!Join(buf,work,bufsize))
          {
          break;
          }
       }
    
    DeleteHubQuery(hq,DeleteHubPromise);
    }

 EndJoin(buf,"</table>\n",bufsize);

 return true;

#else  /* NOT HAVE_CONSTELLATION */

 snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_promise_popularity() in Nova-only environment\n");
 CfOut(cf_error, "", buf);
 return false;

#endif

}

#endif  /* HAVE_LIBMONGOC */

/*****************************************************************************/

int Con2PHP_summarise_filechange(char *hubKeyHash, char *filePath, char *buf, int bufsize)

{
#ifdef HAVE_CONSTELLATION
 struct HubQuery *hq;
 conocs_t *ocs;
 mongo_connection dbconn;
 struct Rlist *rp;
 char work[CF_MAXVARSIZE];

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

hq = CFDBCon_QueryFileChange(&dbconn,hubKeyHash,filePath);

CFDB_Close(&dbconn);

if (!hq)
   {
   buf[0] = '\0';
   return false;
   }
snprintf(buf, bufsize, "{\"meta\":{\"count\" : %d,"
         "\"header\": {\"File name\":0,\"Total count\":1,\"Host count\":2,\"Hub count\":3}"
         "},\"data\":[", 999);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   ocs = (conocs_t *)rp->item;

   snprintf(work, sizeof(work), "[\"%s\",%d,%d,%d],\n",
            ocs->id, ocs->total, ocs->hosts, ocs->hubs);
   
   if (!Join(buf,work,bufsize))
      {
      break;
      }
   }

ReplaceTrailingStr(buf, ",\n", '\0');
EndJoin(buf,"]}\n",bufsize);

DeleteHubQuery(hq,DeleteConOccurences);

return true;

#else  /* NOT HAVE_CONSTELLATION */

snprintf(buf,bufsize,"!! Error: Use of Constellation function Con2PHP_summarise_filechange() in Nova-only environment\n");
CfOut(cf_error, "", buf);
return false;

#endif
}
