
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,BACKGR,ORANGE,GREENS[];
extern char *UNITS[];

#ifdef HAVE_LIBMONGOC
/*****************************************************************************/
/*                                                                           */
/* File: scorecards.c                                                        */
/*                                                                           */
/*****************************************************************************/

void Nova_PerformancePage(char *docroot,char *hostkey,char *buffer,int bufsize)
    
{ char work[CF_BUFSIZE],hostname[CF_SMALLBUF],ipaddress[CF_SMALLBUF];
  char desc[CF_MAXVARSIZE],id[CF_MAXVARSIZE],lastsaw[CF_SMALLBUF];
  int i, have_week, have_mag, have_histo;
  struct CfDataView cfv;


Nova2PHP_hostinfo(hostkey,hostname,ipaddress,CF_MAXVARSIZE);

snprintf(buffer,bufsize,"<table><tr><th></th><th>Past 4 hours</th><th>Past week</th><th>Statistical complete history</th></tr>\n");

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   Nova_LookupAggregateClassName(i,id,desc);

   if (strcmp(id,"spare") == 0)
      {
      continue;
      }

   // Make the graphs

//   have_week = Nova_ViewWeek(&cfv,hostkey,i,false);
//   have_mag = Nova_ViewMag(&cfv,hostkey,i);
//   have_histo = Nova_ViewHisto(&cfv,hostkey,i,false);

   if (have_mag)
      {   
      Nova2PHP_getlastupdate(hostkey,lastsaw,CF_SMALLBUF);

      Join(buffer,"<tr>\n",bufsize);

      snprintf(work,CF_BUFSIZE,"<td><div id=\"ip\">%s</div><br><br>"
               "<a href=\"/visual/vitaldetail/hostkey/%s/obs/%d/nm/%s/view/type\">%s</a>"
               "<br><br><small>Latest data<br>%s</small></td>",hostname,hostkey,i,OBS[i][0],OBS[i][0],lastsaw);      
      Join(buffer,work,bufsize);
      
      
      if (have_mag)
         {
         snprintf(work,CF_BUFSIZE,"<td><a href=\"/visual/vitaldetail/hostkey/%s/obs/%d/nm/%s/view/mag\"><img src=\"/hub/%s/%s_mag.png\" width=\"300\" border=\"0\"></a></td>",hostkey,i,OBS[i][0],hostkey,OBS[i][0]);
         }
      else
         {
         snprintf(work,CF_BUFSIZE,"<td><img src=\"/images/monitoring_nodata.png\" width=\"300\" border=\"0\"></td>");
         }
      
      Join(buffer,work,bufsize);


      if (have_week)
         {
         snprintf(work,CF_BUFSIZE,"<td><a href=\"/visual/vitaldetail/hostkey/%s/obs/%d/nm/%s/view/week\"><img src=\"/hub/%s/%s_week.png\" width=\"300\" border=\"0\"></a></td>",hostkey,i,OBS[i][0],hostkey,OBS[i][0]);
         }
      else
         {
         snprintf(work,CF_BUFSIZE,"<td><img src=\"/images/monitoring_nodata.png\" width=\"300\" border=\"0\"></td>");
         }
      
      Join(buffer,work,bufsize);
      
      
      if(have_histo)
         {
         snprintf(work,CF_BUFSIZE,"<td><a href=\"/visual/vitaldetail/hostkey/%s/obs/%d/nm/%s/view/hist\"><img src=\"/hub/%s/%s_hist.png\" width=\"300\" border=\"0\"></a></td>",hostkey,i,OBS[i][0],hostkey,OBS[i][0]);
         }
      else
         {
         snprintf(work,CF_BUFSIZE,"<td><img src=\"/images/monitoring_nodata.png\" width=\"300\" border=\"0\"></td>");
         }
      
      Join(buffer,work,bufsize);
            
      Join(buffer,"</tr>\n",bufsize);
      }
   }

EndJoin(buffer,"</table>\n",bufsize);
}

/*****************************************************************************/

void Nova_ComplianceSummaryGraph(char *docroot)

{ char *report,buffer[CF_BUFSIZE];
  int count = 0, tmpsize;
  char newfile[CF_BUFSIZE],key[CF_MAXVARSIZE],value[CF_MAXVARSIZE];
  const int span = 7 * 4;
  const double scale = 4.0;
  double x,kept[span], repaired[span], notkept[span];
  double tkept,trepaired,tnotkept,total,y;
  double lkept,lrepaired,lnotkept,ltotal;
  time_t now = time(NULL),start,one_week = (time_t)CF_WEEK;
  int i,j,slot;

for (i = 0; i < (int)span; i++)
   {
   kept[i] = 0;
   repaired[i] = 0;
   notkept[i] = 0;
   }

ltotal = lkept = lrepaired = 0;
lnotkept = 1;

// Read the cached compliance summary

for (i = 0,start = now - one_week; start < now; start += CF_SHIFT_INTERVAL,i++)
   {
   slot = GetShiftSlot(start);
   snprintf(key,CF_MAXVARSIZE,"tc_%d",slot);
   CFDB_GetValue(key,value,CF_MAXVARSIZE);
   sscanf(value,"%lf,%lf,%lf",&(kept[i]),&(repaired[i]),&(notkept[i]));

   // print UTC(time), 
   }
}

/*****************************************************************************/

void Nova_SummaryMeter(char *docroot,char *search_string)

{  char filename[CF_BUFSIZE];
  int returnval = 0;
  double kept = 0,repaired = 0;
  double kept_week = 0,kept_day = 0,kept_hour = 0,kept_comms = 0,kept_anom = 0,kept_perf = 0,kept_other = 0;
  double rep_week = 0,rep_day = 0,rep_hour = 0,rep_comms = 0,rep_anom = 0,rep_perf = 0,rep_other = 0;
  double num_week = 0,num_day = 0,num_hour = 0,num_comms = 0,num_anom = 0,num_perf = 0,num_other = 0;
  struct stat sb;
  struct utimbuf t;
  struct HubMeter *hm;
  struct HubQuery *hq;
  mongo_connection dbconn;
  struct Rlist *rp;
  struct CfDataView cfv;

// get data

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database on meter summary");
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

Nova_BarMeter(&cfv,1,kept_week/num_week,rep_week/num_week,"Week");
Nova_BarMeter(&cfv,2,kept_day/num_day,rep_day/num_day,"Day");
Nova_BarMeter(&cfv,3,kept_hour/num_hour,rep_hour/num_hour,"Hour");
Nova_BarMeter(&cfv,4,kept_perf/num_perf,rep_perf/num_perf,"Perf");
Nova_BarMeter(&cfv,5,kept_other/num_other,rep_other/num_other,"Chng");
Nova_BarMeter(&cfv,6,kept_comms/num_comms,rep_comms/num_comms,"Seen");
Nova_BarMeter(&cfv,7,kept_anom/num_anom,rep_anom/num_anom,"Anom");

// Clean up

DeleteHubQuery(hq,DeleteHubMeter);
}

/*****************************************************************************/

int Nova_Meter(char *docroot,char *hostkey)

{ int returnval = 0;
  char filename[CF_BUFSIZE];
  double kept,repaired;
  struct stat sb;
  struct utimbuf t;
  struct HubMeter *hm;
  struct HubQuery *hq;
  mongo_connection dbconn;
  struct Rlist *rp;
  struct CfDataView cfv;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error,"", "!! Could not open connection to report database on meter query");
   return returnval;
   }

hq = CFDB_QueryMeter(&dbconn,hostkey);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   } 

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hm = (struct HubMeter *)rp->item;

   kept = hm->kept;
   repaired = hm->repaired;
   
   switch (hm->type)
      {
      case cfmeter_week:
          Nova_BarMeter(&cfv,1,kept,repaired,"Week");
          break;
      case cfmeter_hour:
          Nova_BarMeter(&cfv,3,kept,repaired,"Hour");
          break;          
      case cfmeter_day:
          Nova_BarMeter(&cfv,2,kept,repaired,"Day");
          break;
      case cfmeter_perf:
          Nova_BarMeter(&cfv,4,kept,repaired,"Perf");
          break;
      case cfmeter_comms:
          Nova_BarMeter(&cfv,6,kept,repaired,"Coms");
          break;
      case cfmeter_anomaly:
          Nova_BarMeter(&cfv,7,kept,repaired,"Anom");
          break;
      case cfmeter_other:
          Nova_BarMeter(&cfv,5,kept,repaired,"Chng");
          break;
      }
   }

// Clean up

DeleteHubQuery(hq,DeleteHubMeter);
return true;
}

/*****************************************************************************/

int Nova_GetHostColour(char *lkeyhash)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
  bson qe,field,query;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  double akept[meter_endmark],arepaired[meter_endmark];
  double rkept,rrepaired;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
  int num = 0,found = false,result = -1;
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
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
      result = Nova_GetComplianceScore(cfrank_default,akept,arepaired);
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
/* Level                                                                     */
/*****************************************************************************/

struct Item *Nova_ClassifyHostState(char *search_string,int regex,enum cf_rank_method method,int max_return)

{ bson_buffer b,bb,*sub1,*sub2,*sub3;
  bson qe,field,query;
  mongo_cursor *cursor;
  bson_iterator it1,it2,it3;
  struct HubHost *hh;
  struct Rlist *rp,*record_list = NULL, *host_list = NULL;
  double akept[meter_endmark],arepaired[meter_endmark];
  double rkept,rrepaired;
  char keyhash[CF_MAXVARSIZE],hostnames[CF_BUFSIZE],addresses[CF_BUFSIZE],rcolumn[CF_SMALLBUF];
  int num = 0,found = false;
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
   
   while (bson_iterator_next(&it1))
      {
      /* Extract the common HubHost data */

      CMDB_ScanHubHost(&it1,keyhash,addresses,hostnames);
      
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
         int score = Nova_GetComplianceScore(method,akept,arepaired);
         
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

#endif
