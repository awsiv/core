
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,BACKGR,ORANGE;
extern char *UNITS[];


/*****************************************************************************/
/*                                                                           */
/* File: scorecards.c                                                        */
/*                                                                           */
/*****************************************************************************/

void Nova_PerformancePage(char *hostkey)
    
{ FILE *fout;
  char name[CF_BUFSIZE],exist[CF_BUFSIZE];
  char id[CF_BUFSIZE],desc[CF_BUFSIZE];
  struct stat s1,s2;
  struct CfDataView cfv;
  int i;

if (LICENSES == 0)
   {

printf("HERE.......\n");
//   return;
   }

// Make common resources
  
cfv.height = 300;
cfv.width = 700; //(7*24*2)*2; // halfhour
cfv.margin = 50;
  
for (i = 0; i < CF_OBSERVABLES; i++)
   {
   Nova_LookupAggregateClassName(i,id,desc);

   if (strcmp(id,"spare") == 0)
      {
      continue;
      }

   // Make the graphs
   Nova_ViewMag(&cfv,hostkey,i);
   Nova_ViewWeek(&cfv,hostkey,i);
   Nova_ViewHisto(&cfv,hostkey,i);

   // Create the table (return iplist of printf)
   }
}

/*****************************************************************************/

struct Item *Nova_SummaryMeter(char *search_string)

{ FILE *fout;
  char filename[CF_BUFSIZE];
  int returnval = 0;
  double kept = 0,repaired = 0;
  double kept_week = 0,kept_day = 0,kept_hour = 0,kept_comms = 0,kept_anom = 0,kept_perf = 0,kept_other = 0;
  double rep_week = 0,rep_day = 0,rep_hour = 0,rep_comms = 0,rep_anom = 0,rep_perf = 0,rep_other = 0;
  double num = 0;
  struct stat sb;
  struct utimbuf t;
  struct HubMeter *hm;
  struct HubQuery *hq;
  mongo_connection dbconn;
  struct Rlist *rp;
  struct Item *matches = NULL;
  struct CfDataView cfv;

if (LICENSES == 0)
   {
//   return NULL;
   printf("HERE\n");
   }

cfv.height = CF_METER_HEIGHT;
cfv.width = CF_METER_WIDTH;
cfv.margin = CF_METER_MARGIN;
cfv.title = "System state";
cfv.im = gdImageCreate(cfv.width+2*cfv.margin,cfv.height+2*cfv.margin);
Nova_MakePalette(&cfv);

gdImageSetThickness(cfv.im,2);
gdImageFilledRectangle(cfv.im,0,0,cfv.width+2*cfv.margin,cfv.height+2*cfv.margin,LIGHTGREY);
gdImageRectangle(cfv.im,0,0,cfv.width+2*cfv.margin,cfv.height+2*cfv.margin,BLACK);

// get data

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

hq = CFDB_QueryMeter(&dbconn,search_string);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hm = (struct HubMeter *)rp->item;
   Debug("Meter result: (%c) %lf,%lf,%lf\n",hm->type,hm->kept,hm->notkept);
   Debug("found on (%s=%s=%s)\n",hm->hh->keyhash,hm->hh->hostname,hm->hh->ipaddr);

   kept = hm->kept;
   repaired = hm->repaired;

   num++;
   IdempPrependItem(&matches,hm->hh->hostname,hm->hh->ipaddr);
   
   switch (hm->type)
      {
      case cfmeter_week:
          kept_week += kept;
          rep_week += repaired;
          break;
          
      case cfmeter_hour:
          kept_hour += kept;
          rep_hour += repaired;
          break;
          
      case cfmeter_day:
          kept_day += kept;
          rep_day += repaired;
          break;
          
      case cfmeter_perf:
          kept_perf += kept;
          rep_perf += repaired;
          break;
          
      case cfmeter_comms:
          kept_comms += kept;
          rep_comms += repaired;
          break;
          
      case cfmeter_anomaly:
          kept_anom += kept;
          rep_anom += repaired;
          break;
          
      case cfmeter_other:
          kept_other += kept;
          rep_other += repaired;
          break;
      }
   }

kept /= num;
repaired /= num;

Nova_BarMeter(&cfv,1,kept_week,rep_week,"Week");
Nova_BarMeter(&cfv,2,kept_day,rep_day,"Day");
Nova_BarMeter(&cfv,3,kept_hour,rep_hour,"Hour");
Nova_BarMeter(&cfv,4,kept_perf,rep_perf,"Perf");
Nova_BarMeter(&cfv,6,kept_comms,rep_comms,"Coms");
Nova_BarMeter(&cfv,7,kept_anom,rep_anom,"Anom");
Nova_BarMeter(&cfv,5,kept_other,rep_other,"Core");

// Clean up

DeleteHubQuery(hq,DeleteHubMeter);

// Write graph

snprintf(filename,CF_BUFSIZE,"%s/hub/common/meter.png",DOCROOT);
MakeParentDirectory(filename,true);

if ((fout = fopen(filename, "wb")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Couldn't draw meter %s",filename);
   return matches;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",filename);
   }
    
gdImagePng(cfv.im, fout);
fclose(fout);
gdImageDestroy(cfv.im);
return matches;
}

/*****************************************************************************/

int Nova_Meter(char *hostkey)

{ FILE *fout;
  char filename[CF_BUFSIZE];
  int returnval = 0;
  double kept,repaired;
  struct stat sb;
  struct utimbuf t;
  struct HubMeter *hm;
  struct HubQuery *hq;
  mongo_connection dbconn;
  struct Rlist *rp;
  struct CfDataView cfv;
  
if (LICENSES == 0)
   {
//   return 0;
     printf(" - HERE \n");
 }

cfv.height = CF_METER_HEIGHT;
cfv.width = CF_METER_WIDTH;
cfv.margin = CF_METER_MARGIN;
cfv.title = "System state";
cfv.im = gdImageCreate(cfv.width+2*cfv.margin,cfv.height+2*cfv.margin);
Nova_MakePalette(&cfv);

gdImageSetThickness(cfv.im,2);
gdImageFilledRectangle(cfv.im,0,0,cfv.width+2*cfv.margin,cfv.height+2*cfv.margin,LIGHTGREY);
gdImageRectangle(cfv.im,0,0,cfv.width+2*cfv.margin,cfv.height+2*cfv.margin,BLACK);

// get data

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

hq = CFDB_QueryMeter(&dbconn,hostkey);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hm = (struct HubMeter *)rp->item;
   printf("Meter result: (%c) %lf,%lf,%lf\n",hm->type,hm->kept,hm->notkept);
   printf("found on (%s=%s=%s)\n",hm->hh->keyhash,hm->hh->hostname,hm->hh->ipaddr);

   kept = hm->kept;
   repaired = hm->repaired;
   
   switch (hm->type)
      {
      case cfmeter_week:
          Nova_BarMeter(&cfv,1,kept,repaired,"Week");
          break;
      case cfmeter_hour:
          Nova_BarMeter(&cfv,3,kept,repaired,"Hour");
          returnval = kept+repaired;
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
          Nova_BarMeter(&cfv,5,kept,repaired,"Lics");
          break;
      }
   }

// Clean up

DeleteHubQuery(hq,DeleteHubMeter);

// Write graph

snprintf(filename,CF_BUFSIZE,"%s/hub/%s/meter.png",DOCROOT,hostkey);

if ((fout = fopen(filename, "wb")) == NULL)
   {
   return returnval;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",filename);
   }
    
gdImagePng(cfv.im, fout);
fclose(fout);
gdImageDestroy(cfv.im);
return returnval;
}

/*****************************************************************************/

struct Item *Nova_RankHosts(char *search_string,int regex,enum cf_rank_method method,int max_return)

{ struct Item *hosts;
 
if (LICENSES == 0)
   {
   return NULL;
   }

hosts = Nova_ClassifyHostState(search_string,regex,method,max_return);
hosts = SortItemListCounters(hosts);

return hosts;
}

/*****************************************************************************/

struct Item *Nova_GreenHosts(struct Item *master)

{ struct Item *ip,*hosts = NULL;

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (ip->counter < CF_AMBER_THRESHOLD)
      {
      AppendItem(&hosts,ip->name,ip->classes);
      }
   }

return hosts;
}

/*****************************************************************************/

struct Item *Nova_YellowHosts(struct Item *master)

{ struct Item *ip,*hosts = NULL;

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (ip->counter >= CF_AMBER_THRESHOLD && ip->counter < CF_RED_THRESHOLD)
      {
      AppendItem(&hosts,ip->name,ip->classes);
      }
   }

return hosts;
}

/*****************************************************************************/

struct Item *Nova_RedHosts(struct Item *master)

{ struct Item *ip,*hosts = NULL;

for (ip = hosts; ip != NULL; ip=ip->next)
   {
   if (ip->counter >= CF_RED_THRESHOLD)
      {
      AppendItem(&hosts,ip->name,ip->classes);
      }
   }

return hosts;
}

/*****************************************************************************/

void Nova_BarMeter(struct CfDataView *cfv,int number,double kept,double repaired,char *s)

{ int n = number;
  int m = number - 1;
  int v_offset = 35,bar_height = CF_METER_HEIGHT-v_offset;
  int width = 35,h_offset = 15;
  int thickness = bar_height/20;
  int colour,x,y;
  double count;
  char ss[CF_MAXVARSIZE];

count = 0;

for (y = v_offset+bar_height; y > v_offset; y -= thickness)
   {
   gdImageSetThickness(cfv->im,thickness);

   // 20 bars in the given height
   
   if (count/20 <= kept/100)
      {
      colour = GREEN;
      }
   else if (count/20 <= (kept+repaired)/100)
      {
      colour = YELLOW;
      }
   else
      {
      colour = RED;
      }

   gdImageLine(cfv->im,n*h_offset+m*width,y,n*h_offset+n*width,y,colour);
   count++;
   }

snprintf(ss,CF_MAXVARSIZE-1,"%.1lf",kept);

Nova_Font(cfv,n*h_offset+m*width+5,15,s,WHITE);
Nova_Font(cfv,n*h_offset+m*width+5,27,ss,WHITE);
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

if (!CFDB_Open(&conn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
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
                   akept[meter_anomalies_day]= rkept;
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
         
         PrependItem(&list,hostnames,addresses);
         SetItemListCounter(list,hostnames,score);
         
         if (num++ >= max_return)
            {
            break;
            }
         }
      }
   }

bson_destroy(&field);
mongo_cursor_destroy(cursor);

if (!CFDB_Close(&conn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

return list;
}

/*****************************************************************************/

int Nova_GetComplianceScore(enum cf_rank_method method,double *k,double *r)

{ int result = CF_GREEN;
 
switch (method)
   {
   case cfrank_compliance:
       if (k[meter_compliance_hour] < 80)
          {
          result = CF_RED_THRESHOLD;
          }
       
       if (r[meter_compliance_hour] > 20)
          {
          result = CF_AMBER_THRESHOLD;
          }
       
       break;
       
   case cfrank_anomaly:
       break;
       
   case cfrank_performance:
       break;
       
   case cfrank_lastseen:

       break;
       
   default:
       break;
   }

return result;
}
