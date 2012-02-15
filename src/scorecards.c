/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "scorecards.h"

#ifdef HAVE_LIBMONGOC
/*****************************************************************************/
/*                                                                           */
/* File: scorecards.c                                                        */
/*                                                                           */
/*****************************************************************************/

const char *Nova_HostColourToString(HostColour colour)
{
   switch (colour)
   {
   case HOST_COLOUR_BLUE:
      return "blue";
   case HOST_COLOUR_RED:
      return "red";
   case HOST_COLOUR_YELLOW:
      return "yellow";
   case HOST_COLOUR_GREEN:
      return "green";
   default:
      return "unknown";
   }
}


void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize)

// Read the cached compliance summary (either from Hub or
// Constellation DB)

{ char work[CF_BUFSIZE];
  mongo_connection dbconn;
  HubCacheTotalCompliance *tc;
  HubQuery *hq;
  double kept, repaired, notkept, nodata;
  time_t now = time(NULL),start,one_week = (time_t)SECONDS_PER_WEEK;
  int i,slot,count;
  char buf[CF_MAXVARSIZE];

if (!CFDB_Open(&dbconn))
   {
   return;
   }

if (constellation)
   {
#ifdef HAVE_CONSTELLATION
   
   hq = CFDBCon_QueryComp(&dbconn, hubKeyHash, policy, now - SECONDS_PER_WEEK);
   
#else
   CfOut(cf_error, "", "!! Trying to get constellation data from data ComplianceSummaryGraph() without libcfconstellation");
   CFDB_Close(&dbconn);
   return;
#endif
   
   }
else  // Nova
   {
   hq = CFDB_QueryCachedTotalCompliance(&dbconn, policy, now - SECONDS_PER_WEEK);
   }

CFDB_Close(&dbconn);


snprintf(buffer,bufsize,"[");

start = GetShiftSlotStart(now - one_week);

if (start == -1)
   {
   CfOut(cf_error, "", "!! Could not get start of interval");
   DeleteHubQuery(hq,DeleteHubCacheTotalCompliance);
   return;
   }
  
for (i = 0; i < 28; start += CF_SHIFT_INTERVAL,i++)
   {
   slot = GetShiftSlot(start);
   
   tc = GetHubCacheTotalComplianceSlot(hq->records,slot);
   
   if (tc && tc->hostCount > 0)
      {
      kept = tc->kept;
      repaired = tc->repaired;
      notkept = tc->notkept;
      nodata = 0;
      count = tc->hostCount;
      }
   else
      {
      kept = 0;
      repaired = 0;
      notkept = 0;
      nodata = 100.0;
      count = 0;
      }
   
   UtcShiftInterval(start, buf, sizeof(buf));
   
   snprintf(work,CF_BUFSIZE,"{ \"title\": \"%s\", \"start\": %ld, \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf, \"nodata\": %lf, \"count\": %d },",
            buf, start, i, kept, repaired, notkept, nodata, count);
   
   if (!Join(buffer,work,bufsize))
      {
      break;
      }
   }

buffer[strlen(buffer)-1] = ']';

DeleteHubQuery(hq,DeleteHubCacheTotalCompliance);
}

/*****************************************************************************/

void Nova_Meter(bson *query,char *db,char *buffer,int bufsize)

{ double kept = 0,repaired = 0;
  double kept_week = 0,kept_day = 0,kept_hour = 0,kept_comms = 0,kept_anom = 0,kept_perf = 0,kept_other = 0;
  double rep_week = 0,rep_day = 0,rep_hour = 0,rep_comms = 0,rep_anom = 0,rep_perf = 0,rep_other = 0;
  double num_week = 0,num_day = 0,num_hour = 0,num_comms = 0,num_anom = 0,num_perf = 0,num_other = 0;
  HubMeter *hm;
  HubQuery *hq;
  mongo_connection dbconn;
  Rlist *rp;

 strcpy(buffer,"[");

 if (!CFDB_Open(&dbconn))
    {
    Join(buffer,"]",bufsize);
    return;
    }

 hq = CFDB_QueryMeter(&dbconn,query,db);

 CFDB_Close(&dbconn);

 for (rp = hq->records; rp != NULL; rp=rp->next)
    {
    hm = (HubMeter *)rp->item;
    CfDebug("Meter result: (%c) %lf,%lf,%lf\n",hm->type,hm->kept,hm->repaired,hm->notkept);
    CfDebug("found on (%s=%s=%s)\n",hm->hh->keyhash,hm->hh->hostname,hm->hh->ipaddr);

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
    
{ bson_buffer b,bb;
  bson field,query;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  double akept[meter_endmark] = {0},arepaired[meter_endmark] = {0};
  double rkept,rrepaired;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
  int result = -1,awol, foundMeter;
  mongo_connection conn;
  time_t now = time(NULL);
  unsigned long bluehost_threshold;
  
if (lkeyhash == NULL)
   {
   return -1;
   }

if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
   {
   return -1;
   }

if (!CFDB_Open(&conn))
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

cursor = mongo_find(&conn,MONGO_DATABASE,&query,&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&query);
bson_destroy(&field);

while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   keyhash[0] = '\0';
   hostnames[0] = '\0';
   addresses[0] = '\0';
   awol = true;
   foundMeter = false;
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */
      
      CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfr_day) == 0)
         {
         time_t then;
         then = (time_t)bson_iterator_int(&it1);
         
         if (now - bluehost_threshold < then)
            {
            awol = false;
            }
         }
      
      if (strcmp(bson_iterator_key(&it1),cfr_meter) == 0)
         {
         foundMeter = true;
         
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
                   akept[meter_perf_day]= rkept;
                   arepaired[meter_perf_day] = rrepaired;
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
            }
         }   
      }
   
   if (awol || !foundMeter)
      {
      result = CF_CODE_BLUE;
      }
   else
      {
      result = Nova_GetComplianceScore(HOST_RANK_METHOD_COMPLIANCE,akept,arepaired);
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

Item *Nova_RankHosts(HostRankMethod method,int max_return)

{ Item *ip,*hosts,*counted =  NULL;
  int num = 0;
 
hosts = Nova_ClassifyHostState(method);
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

Item *Nova_GreenHosts()

{ Item *ip,*hosts = NULL,*sorted = NULL;

hosts = Nova_ClassifyHostState(HOST_RANK_METHOD_DEFAULT);

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (Nova_HostScoreToColour(ip->counter) == HOST_COLOUR_GREEN)
      {
      AppendItem(&sorted,ip->name,ip->classes);
      }
   }

DeleteItemList(hosts);
sorted = SortItemListNames(sorted);
return sorted;
}

/*****************************************************************************/

Item *Nova_YellowHosts()

{ Item *ip,*hosts = NULL,*sorted = NULL;

hosts = Nova_ClassifyHostState(HOST_RANK_METHOD_DEFAULT);

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (Nova_HostScoreToColour(ip->counter) == HOST_COLOUR_YELLOW)
      {
      AppendItem(&sorted,ip->name,ip->classes);
      }
   }

DeleteItemList(hosts);
sorted = SortItemListNames(sorted);
return sorted;
}

/*****************************************************************************/

Item *Nova_RedHosts()

{ Item *ip,*hosts = NULL,*sorted = NULL;

 hosts = Nova_ClassifyHostState(HOST_RANK_METHOD_DEFAULT);

 for (ip = hosts; ip != NULL; ip=ip->next)
    {
    if (Nova_HostScoreToColour(ip->counter) == HOST_COLOUR_RED)
       {
       AppendItem(&sorted,ip->name,ip->classes);
       }
    }

 DeleteItemList(hosts);
 sorted = SortItemListNames(sorted);
 return sorted;
}

/*****************************************************************************/

Item *Nova_BlueHosts()

{ Item *ip,*hosts = NULL,*sorted = NULL;

hosts = Nova_ClassifyHostState(HOST_RANK_METHOD_DEFAULT);

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (Nova_HostScoreToColour(ip->counter) == HOST_COLOUR_BLUE)
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

Item *Nova_ClassifyHostState(HostRankMethod method)

/* note the similarities between this fn and GetHostColour() */
    
{ bson_buffer bb;
  bson qe,field;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  double akept[meter_endmark] = {0},arepaired[meter_endmark] = {0};
  double rkept,rrepaired;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
  int awol,foundMeter;
  mongo_connection conn;
  Item *list = NULL;
  time_t now = time(NULL);
  unsigned long bluehost_threshold;

if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
   {
   return NULL;
   }

if (!CFDB_Open(&conn))
   {
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

cursor = mongo_find(&conn,MONGO_DATABASE,bson_empty(&qe),&field,0,0,CF_MONGO_SLAVE_OK);
bson_destroy(&field);


while (mongo_cursor_next(cursor))  // loops over documents
   {
   bson_iterator_init(&it1,cursor->current.data);
   
   keyhash[0] = '\0';
   hostnames[0] = '\0';
   addresses[0] = '\0';
   awol = true;
   foundMeter = false;
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */
      
      CFDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
      if (strcmp(bson_iterator_key(&it1),cfr_day) == 0)
         {
         time_t then;
         then = (time_t)bson_iterator_int(&it1);
         
         if (now - bluehost_threshold < then)
            {
            awol = false;
            }
         }
      
      /* Query specific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfr_meter) == 0)
         {
         foundMeter = true;
         
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
                   akept[meter_perf_day]= rkept;
                   arepaired[meter_perf_day] = rrepaired;
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

            }
         }   
      }
   
   int score;
   
   if (awol || !foundMeter)
      {
      score = CF_CODE_BLUE;
      }
   else
      {
      score = Nova_GetComplianceScore(method,akept,arepaired);
      }
   
   PrependItem(&list,keyhash,hostnames);
   SetItemListCounter(list,keyhash,score);
   }


mongo_cursor_destroy(cursor);

if (!CFDB_Close(&conn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   } 

return list;
}

/*****************************************************************************/

int Nova_GetComplianceScore(HostRankMethod method,double *k,double *r)

{ int result = CF_GREEN;
 double kav,rav,notkept; 

 switch (method)
    {
    default:
    case HOST_RANK_METHOD_DEFAULT:
    case HOST_RANK_METHOD_COMPLIANCE:

        notkept = 100 - k[meter_compliance_hour] - r[meter_compliance_hour];
        
        if (notkept > 20)                   // 20% of promises are not kept => RED!
           {
           result = CF_RED_THRESHOLD + 100 + notkept; // Make red override all variations in amber/green
           }
       
        if (r[meter_compliance_hour] > 20)  // If more than 20% of promises were repaired => AMBER
           {
           result = CF_AMBER_THRESHOLD + 100 + r[meter_compliance_hour];
           }

        result -= k[meter_compliance_hour]; // Adjust the Green Value relative
        break;
       
    case HOST_RANK_METHOD_ANOMALY:

        notkept = 100 - k[meter_anomalies_day] - r[meter_anomalies_day];

        // If red or yellow, add the "badness" to the base threshold
        // to ensure the worst host is ranked on top. We also add
        // 100 in case k[meter_anomalies_day] pulls us back under the
        // threshold
        
        if (notkept > 20)
           {
           result = CF_RED_THRESHOLD + 100 + notkept;
           }
       
        if (r[meter_anomalies_day] > 20)
           {
           result = CF_AMBER_THRESHOLD + 100 + r[meter_anomalies_day];
           }

        // We want worse hosts to have higher scores so they appear on top
        // so subtract what is "good" about kept promises
        
        result -= k[meter_anomalies_day];
        break;
       
    case HOST_RANK_METHOD_PERFORMANCE:

        notkept = 100 - k[meter_perf_day] - r[meter_perf_day];
        
        if (notkept > 20)
           {
           result = CF_RED_THRESHOLD + 100 + notkept;
           }
       
        if (r[meter_perf_day] > 20)
           {
           result = CF_AMBER_THRESHOLD + 100 + r[meter_perf_day];
           }

        result -= k[meter_perf_day];
        break;
       
    case HOST_RANK_METHOD_LASTSEEN:

        notkept = 100 - k[meter_comms_hour] - r[meter_comms_hour];
            
        if (notkept > 20)
           {
           result = CF_RED_THRESHOLD + 100 + notkept;
           }
       
        if (r[meter_comms_hour] > 20)
           {
           result = CF_AMBER_THRESHOLD + 100 + r[meter_comms_hour];
           }

        result -= k[meter_comms_hour];
        break;
       
    case HOST_RANK_METHOD_MIXED:
        kav = (k[meter_comms_hour] + k[meter_compliance_hour] + k[meter_anomalies_day]) / 3;
        rav = (r[meter_comms_hour] + r[meter_compliance_hour] + r[meter_anomalies_day]) / 3;

        notkept = 100 - kav - rav;
            
        if (notkept > 20)
           {
           result = CF_RED_THRESHOLD + 100 + notkept;
           }
       
        if (rav > 20)
           {
           result = CF_AMBER_THRESHOLD + 100 + rav;
           }

        result -= kav;
        break;

        
    }

return result;
}

HostColour Nova_HostScoreToColour(int score)
{
if (score == CF_CODE_BLUE)
   {
   return HOST_COLOUR_BLUE;
   }
else if (score < CF_AMBER_THRESHOLD)
   {
   return HOST_COLOUR_GREEN;
   }
else if (score >= CF_AMBER_THRESHOLD && score < CF_RED_THRESHOLD)
   {
   return HOST_COLOUR_YELLOW;
   }
else
   {
   return HOST_COLOUR_RED;
   }

}

/*****************************************************************************/

void Nova_BarMeter(int pos,double kept,double rep,char *name,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
 
snprintf(work,CF_BUFSIZE,"{ \"title\": \"%s\", \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf }",name,pos,kept,rep,100-kept-rep);
Join(buffer,work,bufsize);
}

#endif
