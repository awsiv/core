/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC
/*****************************************************************************/
/*                                                                           */
/* File: scorecards.c                                                        */
/*                                                                           */
/*****************************************************************************/

void Nova_PerformancePage(char *docroot,char *hostkey,char *buffer,int bufsize)
    
{ char work[CF_BUFSIZE],lastsaw[CF_SMALLBUF], hostname[CF_SMALLBUF],ipaddress[CF_SMALLBUF];
 char desc[CF_BUFSIZE],id[CF_BUFSIZE];
  
 int i, havedata=false;
 struct CfDataView cfv;

 strcpy(buffer,"{ ");

 Nova2PHP_hostinfo(hostkey,hostname,ipaddress,CF_MAXVARSIZE);
 Nova2PHP_getlastupdate(hostkey,lastsaw,CF_SMALLBUF);

 snprintf(work,CF_BUFSIZE, "\"hostname\" : \"%s\", \"ip\" : \"%s\", \"ls\" : \"%s\", \"obs\" : [",
          hostname,ipaddress,lastsaw);

 Join(buffer,work,bufsize);

 for (i = 0; i < CF_OBSERVABLES; i++)
    {     
    Nova_LookupAggregateClassName(i,id,desc);
     
    if (strcmp(id,"spare") == 0)
       {
       continue;
       }
     
    if (!Nova_ReadMagTimeSeries(&cfv,hostkey,i))
       {
       continue;
       }   
     
    snprintf(work,CF_BUFSIZE, "{ \"id\" : \"%s\", \"desc\" : \"%s\", \"i\" : %d },",
             id,desc,i);

    Join(buffer,work,bufsize);
    havedata=true;
    }

 if(havedata)
    {
    buffer[strlen(buffer)-1] = ']';
    }
 else
    {
    strcpy(buffer,"{");
    }
 Join(buffer,"}",bufsize);
}

/*****************************************************************************/

void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize)

// Read the cached compliance summary (either from Hub or
// Constellation DB)

{ char key[CF_MAXVARSIZE],value[CF_MAXVARSIZE],work[CF_BUFSIZE];
 mongo_connection dbconn;
 struct HubCacheTotalCompliance *tc;
 struct HubQuery *hq;
 struct Rlist *rp;
 double kept, repaired, notkept, nodata;
 time_t now = time(NULL),start,one_week = (time_t)CF_WEEK;
 int i,slot,count;

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_verbose,"", "!! Could not open connection to cache database");
    return;
    }

 if(constellation)
    {
#ifdef HAVE_CONSTELLATION
     
    hq = CFDB_QuerySumComp(&dbconn, hubKeyHash, policy, now - CF_WEEK);

#else
    CfOut(cf_error, "", "!! Trying to get constellation data from data ComplianceSummaryGraph() without libcfconstellation");
    CFDB_Close(&dbconn);
    return;
#endif
     
    }
 else  // Nova
    {
    hq = CFDB_QueryCachedTotalCompliance(&dbconn, policy, now - CF_WEEK);
    }

 CFDB_Close(&dbconn);


 snprintf(buffer,bufsize,"[");

 start = GetShiftSlotStart(now - one_week);

 if(start == -1)
    {
    CfOut(cf_error, "", "!! Could not get start of interval");
    DeleteHubQuery(hq,DeleteHubCacheTotalCompliance);
    return;
    }
  
 for (i = 0; i < 28; start += CF_SHIFT_INTERVAL,i++)
    {
    
    start = GetShiftSlotStart(start);  // in case of daylight saving time
    slot = GetShiftSlot(start);
     
    tc = GetHubCacheTotalComplianceSlot(hq->records,slot);

     
    if(tc)
       {
       kept = tc->kept;
       repaired = tc->repaired;
       notkept = tc->notkept;
       nodata = 0;
       count = tc->count;
       }
    else
       {
       kept = 0;
       repaired = 0;
       notkept = 0;
       nodata = 100.0;
       count = 0;
       }

    
    snprintf(work,CF_BUFSIZE,"{ \"start\": %d, \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf, \"nodata\": %lf, \"count\": %d },",
             start, i, kept, repaired, notkept, nodata, count);

    if(!Join(buffer,work,bufsize))
       {
       break;
       }
    }

 buffer[strlen(buffer)-1] = ']';

 DeleteHubQuery(hq,DeleteHubCacheTotalCompliance);
}

/*****************************************************************************/

void Nova_Meter(char *search_string,char *buffer,int bufsize)

{ double kept = 0,repaired = 0;
 double kept_week = 0,kept_day = 0,kept_hour = 0,kept_comms = 0,kept_anom = 0,kept_perf = 0,kept_other = 0;
 double rep_week = 0,rep_day = 0,rep_hour = 0,rep_comms = 0,rep_anom = 0,rep_perf = 0,rep_other = 0;
 double num_week = 0,num_day = 0,num_hour = 0,num_comms = 0,num_anom = 0,num_perf = 0,num_other = 0;
 struct stat sb;
 struct utimbuf t;
 struct HubMeter *hm;
 struct HubQuery *hq;
 mongo_connection dbconn;
 struct Rlist *rp;

 strcpy(buffer,"[");

 if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_error,"", "!! Could not open connection to report database on meter summary");
    Join(buffer,"]",bufsize);
    return;
    }

 hq = CFDB_QueryMeter(&dbconn,search_string);

 CFDB_Close(&dbconn);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hm = (struct HubMeter *)rp->item;
    Debug("Meter result: (%c) %lf,%lf,%lf\n",hm->type,hm->kept,hm->repaired,hm->notkept);
    Debug("found on (%s=%s=%s)\n",hm->hh->keyhash,hm->hh->hostname,hm->hh->ipaddr);

    kept = hm->kept;
    repaired = hm->repaired;

    switch (hm->type)
       {
       case cfmeter_week:
           kept_week += kept;
           rep_week += repaired;
           num_week++;
           break;
          
       case cfmeter_hour:
           kept_hour += kept;
           rep_hour += repaired;
           num_hour++;
           break;
          
       case cfmeter_day:
           kept_day += kept;
           rep_day += repaired;
           num_day++;
           break;
          
       case cfmeter_perf:
           kept_perf += kept;
           rep_perf += repaired;
           num_perf++;
           break;
          
       case cfmeter_comms:
           kept_comms += kept;
           rep_comms += repaired;
           num_comms++;
           break;
          
       case cfmeter_anomaly:
           kept_anom += kept;
           rep_anom += repaired;
           num_anom++;
           break;
          
       case cfmeter_other:
           kept_other += kept;
           rep_other += repaired;
           num_other++;
           break;
       }
    }

 if (hq->records != NULL)
    {
    if (num_week>0)
       {
       Nova_BarMeter(1,kept_week/num_week,rep_week/num_week,"Week",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_day>0)
       {
       Nova_BarMeter(2,kept_day/num_day,rep_day/num_day,"Day",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_hour>0)
       {
       Nova_BarMeter(3,kept_hour/num_hour,rep_hour/num_hour,"Hour",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_perf>0)
       {
       Nova_BarMeter(4,kept_perf/num_perf,rep_perf/num_perf,"Perf",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_other>0)
       {
       Nova_BarMeter(5,kept_other/num_other,rep_other/num_other,"Chng",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_comms>0)
       {
       Nova_BarMeter(6,kept_comms/num_comms,rep_comms/num_comms,"Seen",buffer,bufsize);
       strcat(buffer,",");
       }
    if (num_anom>0)
       {
       Nova_BarMeter(7,kept_anom/num_anom,rep_anom/num_anom,"Anom",buffer,bufsize);
       }
    else
       {
       buffer[strlen(buffer)-1] = '\0';
       }
    }

// Clean up

 DeleteHubQuery(hq,DeleteHubMeter);
 Join(buffer,"]",bufsize);
}

/*****************************************************************************/

int Nova_GetHostColour(char *lkeyhash)

/* note the similarities between this fn and ClassifyHostState() */
    
{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qe,field,query;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp,*record_list = NULL, *host_list = NULL;
 double akept[meter_endmark] = {0},arepaired[meter_endmark] = {0};
 double rkept,rrepaired;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
 int num = 0,found = false,result = -1,awol = false;
 mongo_connection conn;
 struct Item *list = NULL;

 if (!CFDB_Open(&conn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_error,"", "!! Could not open connection to report database on host colour query");
    return -1;
    }

 if (lkeyhash == NULL)
    {
    return -1;
    }

 bson_buffer_init(&b);
 bson_append_string(&b,cfr_keyhash,lkeyhash);
 bson_from_buffer(&query,&b);

/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_meter,1);
 bson_append_int(&bb,cfr_day,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(&conn,MONGO_DATABASE,&query,&field,0,0,0);
 bson_destroy(&query);
 bson_destroy(&field);

 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
    awol = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_day) == 0)
          {
          time_t then,now = time(NULL);
          then = (time_t)bson_iterator_int(&it1);

          if (now > then + CF_HUB_HORIZON)
             {
             awol = true;
             break; // Machine is officially AWOL
             }
          }

       if (strcmp(bson_iterator_key(&it1),cfr_meter) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rcolumn,bson_iterator_key(&it2),CF_SMALLBUF-1);

             rkept = 0;
             rrepaired = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_meterkept) == 0)
                   {
                   rkept = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_meterrepaired) == 0)
                   {
                   rrepaired = bson_iterator_double(&it3);
                   }
                else
                   {
                   CfOut(cf_error,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                   }
                }

             switch (*rcolumn)
                {
                case cfmeter_week:
                    akept[meter_compliance_week]= rkept;
                    arepaired[meter_compliance_week] = rrepaired;
                    break;

                case cfmeter_hour:
                    akept[meter_compliance_hour] = rkept;
                    arepaired[meter_compliance_hour] = rrepaired;
                    break;
                   
                case cfmeter_day:
                    akept[meter_compliance_day]= rkept;
                    arepaired[meter_compliance_day] = rrepaired;
                    break;
                   
                case cfmeter_perf:
                    akept[meter_compliance_week]= rkept;
                    arepaired[meter_compliance_week] = rrepaired;
                    break;

                case cfmeter_comms:
                    akept[meter_comms_hour]= rkept;
                    arepaired[meter_comms_hour] = rrepaired;
                    break;
                   
                case cfmeter_anomaly:
                    akept[meter_anomalies_day] = rkept;
                    arepaired[meter_anomalies_day] = rrepaired;
                    break;
                   
                case cfmeter_other:
                    akept[meter_other_day]= rkept;
                    arepaired[meter_other_day] = rrepaired;
                    break;
                }
            
             found = true;           
             }
          }   
       }

    if (found)
       {
       if (awol)
          {
          result = CF_CODE_BLUE;
          }
       else
          {
          result = Nova_GetComplianceScore(cfrank_default,akept,arepaired);
          }
       }
    }

 mongo_cursor_destroy(cursor);

 if (!CFDB_Close(&conn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    } 

 return result;
}

/*****************************************************************************/

struct Item *Nova_RankHosts(char *search_string,int regex,enum cf_rank_method method,int max_return)

{ struct Item *ip,*hosts,*counted =  NULL;
 int num = 0;
 
 hosts = Nova_ClassifyHostState(search_string,regex,method,0);
 hosts = SortItemListCounters(hosts);

 if (max_return > 0)
    {
    for (ip = hosts; ip != NULL; ip = ip->next)
       {
       if (num++ > max_return)
          {
          break;
          }

       AppendItem(&counted,ip->name,ip->classes);
       SetItemListCounter(counted,ip->name,ip->counter);
       }

    DeleteItemList(hosts);
    return counted;
    }
 else
    {
    return hosts;
    }
}

/*****************************************************************************/

struct Item *Nova_GreenHosts()

{ struct Item *ip,*hosts = NULL,*sorted = NULL;

 hosts = Nova_ClassifyHostState(NULL,false,cfrank_default,1000);

 for (ip = hosts; ip != NULL; ip=ip->next)
    {
    if (Nova_IsGreen(ip->counter))
       {
       AppendItem(&sorted,ip->name,ip->classes);
       }
    }

 DeleteItemList(hosts);
 sorted = SortItemListNames(sorted);
 return sorted;
}

/*****************************************************************************/

struct Item *Nova_YellowHosts()

{ struct Item *ip,*hosts = NULL,*sorted = NULL;

 hosts = Nova_ClassifyHostState(NULL,false,cfrank_default,1000);

 for (ip = hosts; ip != NULL; ip=ip->next)
    {
    if (Nova_IsYellow(ip->counter))
       {
       AppendItem(&sorted,ip->name,ip->classes);
       }
    }

 DeleteItemList(hosts);
 sorted = SortItemListNames(sorted);
 return sorted;
}

/*****************************************************************************/

struct Item *Nova_RedHosts()

{ struct Item *ip,*hosts = NULL,*sorted = NULL;

 hosts = Nova_ClassifyHostState(NULL,false,cfrank_default,1000);

 for (ip = hosts; ip != NULL; ip=ip->next)
    {
    if (Nova_IsRed(ip->counter))
       {
       AppendItem(&sorted,ip->name,ip->classes);
       }
    }

 DeleteItemList(hosts);
 sorted = SortItemListNames(sorted);
 return sorted;
}

/*****************************************************************************/

struct Item *Nova_BlueHosts()

{ struct Item *ip,*hosts = NULL,*sorted = NULL;

 hosts = Nova_ClassifyHostState(NULL,false,cfrank_default,1000);

 for (ip = hosts; ip != NULL; ip=ip->next)
    {
    if (Nova_IsBlue(ip->counter))
       {
       AppendItem(&sorted,ip->name,ip->classes);
       }
    }

 DeleteItemList(hosts);
 sorted = SortItemListNames(sorted);
 return sorted;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

struct Item *Nova_ClassifyHostState(char *search_string,int regex,enum cf_rank_method method,int max_return)

/* note the similarities between this fn and GetHostColour() */
    
{ bson_buffer b,bb,*sub1,*sub2,*sub3;
 bson qe,field,query;
 mongo_cursor *cursor;
 bson_iterator it1,it2,it3;
 struct HubHost *hh;
 struct Rlist *rp,*record_list = NULL, *host_list = NULL;
 double akept[meter_endmark],arepaired[meter_endmark];
 double rkept,rrepaired;
 char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
 int num = 0,found = false,awol;
 mongo_connection conn;
 struct Item *list = NULL;

 if (!CFDB_Open(&conn, "127.0.0.1", CFDB_PORT))
    {
    CfOut(cf_error,"", "!! Could not open connection to report database on classify host state");
    return NULL;
    }

/* BEGIN RESULT DOCUMENT */

 bson_buffer_init(&bb);
 bson_append_int(&bb,cfr_keyhash,1);
 bson_append_int(&bb,cfr_ip_array,1);
 bson_append_int(&bb,cfr_host_array,1);
 bson_append_int(&bb,cfr_meter,1);
 bson_append_int(&bb,cfr_day,1);
 bson_from_buffer(&field, &bb);

/* BEGIN SEARCH */

 hostnames[0] = '\0';
 addresses[0] = '\0';

 cursor = mongo_find(&conn,MONGO_DATABASE,bson_empty(&qe),&field,0,0,0);
 bson_destroy(&field);


 while (mongo_cursor_next(cursor))  // loops over documents
    {
    bson_iterator_init(&it1,cursor->current.data);

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';
    found = false;
    awol = false;
   
    while (bson_iterator_next(&it1))
       {
       /* Extract the common HubHost data */

       CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);

       if (strcmp(bson_iterator_key(&it1),cfr_day) == 0)
          {
          time_t then,now = time(NULL);
          then = (time_t)bson_iterator_int(&it1);

          if (now > then + CF_HUB_HORIZON)
             {
             awol = true;
             break; // Machine is officially AWOL
             }
          }
      
       /* Query specific search/marshalling */

       if (strcmp(bson_iterator_key(&it1),cfr_meter) == 0)
          {
          bson_iterator_init(&it2,bson_iterator_value(&it1));

          while (bson_iterator_next(&it2))
             {
             bson_iterator_init(&it3, bson_iterator_value(&it2));
             strncpy(rcolumn,bson_iterator_key(&it2),CF_SMALLBUF-1);

             rkept = 0;
             rrepaired = 0;
            
             while (bson_iterator_next(&it3))
                {
                if (strcmp(bson_iterator_key(&it3),cfr_meterkept) == 0)
                   {
                   rkept = bson_iterator_double(&it3);
                   }
                else if (strcmp(bson_iterator_key(&it3),cfr_meterrepaired) == 0)
                   {
                   rrepaired = bson_iterator_double(&it3);
                   }
                else
                   {
                   CfOut(cf_error,"", " !! Unknown key \"%s\" in last seen",bson_iterator_key(&it3));
                   }
                }

             switch (*rcolumn)
                {
                case cfmeter_week:
                    akept[meter_compliance_week]= rkept;
                    arepaired[meter_compliance_week] = rrepaired;
                    break;

                case cfmeter_hour:
                    akept[meter_compliance_hour] = rkept;
                    arepaired[meter_compliance_hour] = rrepaired;
                    break;
                   
                case cfmeter_day:
                    akept[meter_compliance_day]= rkept;
                    arepaired[meter_compliance_day] = rrepaired;
                    break;
                   
                case cfmeter_perf:
                    akept[meter_compliance_week]= rkept;
                    arepaired[meter_compliance_week] = rrepaired;
                    break;

                case cfmeter_comms:
                    akept[meter_comms_hour]= rkept;
                    arepaired[meter_comms_hour] = rrepaired;
                    break;
                   
                case cfmeter_anomaly:
                    akept[meter_anomalies_day] = rkept;
                    arepaired[meter_anomalies_day] = rrepaired;
                    break;
                   
                case cfmeter_other:
                    akept[meter_other_day]= rkept;
                    arepaired[meter_other_day] = rrepaired;
                    break;
                }
            
             found = true;           
             }
          }   
       }
   
    if (found)
       {
       if (search_string == NULL || FullTextMatch(search_string,hostnames))
          {
          int score;
         
          if (awol)
             {
             score = CF_CODE_BLUE;
             }
          else
             {
             score = Nova_GetComplianceScore(method,akept,arepaired);
             }
         
          PrependItem(&list,keyhash,hostnames);
          SetItemListCounter(list,keyhash,score);
         
          if (max_return && (num++ >= max_return))
             {
             break;
             }
          }
       }
    }

 mongo_cursor_destroy(cursor);

 if (!CFDB_Close(&conn))
    {
    CfOut(cf_verbose,"", "!! Could not close connection to report database");
    } 

 return list;
}

/*****************************************************************************/

int Nova_GetComplianceScore(enum cf_rank_method method,double *k,double *r)

{ int result = CF_GREEN;
 double kav,rav; 

 switch (method)
    {
    case cfrank_compliance:
        if (k[meter_compliance_hour] < 80)
           {
           result = CF_RED_THRESHOLD + 100;
           }
       
        if (r[meter_compliance_hour] > 20)
           {
           result = CF_AMBER_THRESHOLD + r[meter_compliance_hour];
           }

        result -= k[meter_compliance_hour];
        break;
       
    case cfrank_anomaly:
        if (k[meter_anomalies_day] < 80)
           {
           result = CF_RED_THRESHOLD + 100;
           }
       
        if (r[meter_anomalies_day] > 20)
           {
           result = CF_AMBER_THRESHOLD + r[meter_anomalies_day];
           }

        result -= k[meter_anomalies_day];
        break;
       
    case cfrank_performance:
        if (k[meter_perf_day] < 80)
           {
           result = CF_RED_THRESHOLD + 100;
           }
       
        if (r[meter_perf_day] > 20)
           {
           result = CF_AMBER_THRESHOLD + r[meter_perf_day];
           }

        result -= k[meter_perf_day];
        break;
       
    case cfrank_lastseen:
        if (k[meter_comms_hour] < 80)
           {
           result = CF_RED_THRESHOLD + 100;
           }
       
        if (r[meter_comms_hour] > 20)
           {
           result = CF_AMBER_THRESHOLD + r[meter_comms_hour];
           }

        result -= k[meter_comms_hour];
        break;
       
    default:
        kav = (k[meter_comms_hour] + k[meter_compliance_hour] + k[meter_anomalies_day]) / 3;
        rav = (r[meter_comms_hour] + r[meter_compliance_hour] + r[meter_anomalies_day]) / 3;

        if (kav < 80)
           {
           result = CF_RED_THRESHOLD + 100;
           }
       
        if (rav > 20)
           {
           result = CF_AMBER_THRESHOLD + rav;
           }

        result -= kav;
        break;
    }

 return result;
}

/*****************************************************************************/

int Nova_IsGreen(int level)

{
 if (level < CF_AMBER_THRESHOLD)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

int Nova_IsYellow(int level)

{
 if (level >= CF_AMBER_THRESHOLD && level < CF_RED_THRESHOLD)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

int Nova_IsRed(int level)

{
 if (level >= CF_RED_THRESHOLD)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

int Nova_IsBlue(int level)

{
 if (level == CF_CODE_BLUE)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

void Nova_BarMeter(int pos,double kept,double rep,char *name,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 
 snprintf(work,CF_BUFSIZE,"{ \"title\": \"%s\", \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf }",name,pos,kept,rep,100-kept-rep);
 Join(buffer,work,bufsize);
}

#endif
