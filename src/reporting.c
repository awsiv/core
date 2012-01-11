
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: reporting.c                                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*******************************************************************/

 /* These should be coordinated with cfreport.c - but not copied */

enum cf_format
   {
   cfx_entry,
   cfx_event,
   cfx_host,
   cfx_pm,
   cfx_ip,
   cfx_date,
   cfx_q,
   cfx_av,
   cfx_dev,
   cfx_version,
   cfx_ref,
   cfx_filename,
   cfx_index,
   cfx_min,
   cfx_max,
   cfx_end,
   cfx_kept,
   cfx_repaired,
   cfx_notrepaired
   };

char *NRX[][2] =
   {
    {"<entry>\n","\n</entry>\n"},
    {"<event>\n","\n</event>\n"},
    {"<hostname>\n","\n</hostname>\n"},
    {"<pm>\n","\n</pm>\n"},
    {"<ip>\n","\n</ip>\n"},
    {"<date>\n","\n</date>\n"},
    {"<q>\n","\n</q>\n"},
    {"<expect>\n","\n</expect>\n"},
    {"<sigma>\n","\n</sigma>\n"},
    {"<version>\n","\n</version>\n"},
    {"<ref>\n","\n</ref>\n"},
    {"<filename>\n","\n</filename>\n"},
    {"<index>\n","\n</index>\n"},
    {"<min>\n","\n</min>\n"},
    {"<max>\n","\n</max>\n"},
    {"<end>\n","\n</end>\n"},
    {"<kept>\n","\n</kept>\n"},
    {"<repaired>\n","\n</repaired>\n"},
    {"<notrepaired>\n","\n</notrepaired>\n"},
    {NULL,NULL}
   };

char *NRH[][2] =
   {
    {"<tr>","</tr>\n\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td bgcolor=#add8e6>","</td>\n"},
    {"<td bgcolor=#e0ffff>","</td>\n"},
    {"<td bgcolor=#f0f8ff>","</td>\n"},
    {"<td bgcolor=#fafafa>","</td>\n"},
    {"<td bgcolor=#ededed>","</td>\n"},
    {"<td bgcolor=#e0e0e0>","</td>\n"},
    {"<td bgcolor=#add8e6>","</td>\n"},
    {"<td bgcolor=#e0ffff>","</td>\n"},
    {"<td bgcolor=#fafafa><small>","</small></td>\n"},
    {"<td bgcolor=#fafafa><small>","</small></td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {"<td>","</td>\n"},
    {NULL,NULL}
   };

char *NUMBER_TXT[] =
   {
   "zero",
   "one",
   "two",
   "three",
   "four",
   "five",
   "six",
   "seven",
   "eight",
   "nine",
   "ten",
   "eleven",
   "twelve",
   "thirteen",
   "fourteen",
   "fifteen",
   "sixteen",
   NULL
   };

struct ReportInfo BASIC_REPORTS[] =
   {
   {"bundle-profile", "Policy","Themes by bundle","Bundle profile","Status of promise bundles and when they were last verified"},
   {"promise-compliance", "Policy","View compliance","Compliance by promise","Compliance of each promise individually"},
   {"promises-repaired-log","Policy","Promises repaired (log)","Promises repaired log","Log of actual repairs made to the system"},
   {"promises-repaired-summary","Policy","Promises repaired (summary)","Promises repaired summary","Cumulative (histogram) summary of promises repaired"},
   {"promises-not-kept-log","Policy","Promises not kept (log)","Promises not kept log","Log of promises that could not or would not be kept"},
   {"promises-not-kept-summary","Policy","Promises not kept (summary)","Promises not kept summary","Cumulative (histogram) summary of promises not kept"},


   {"contexts","System","Context classes","Class profile","User defined classes observed on the system"},
   {"neighbors","System","Neighbors","Last seen hosts","Log of when neighbouring hosts were last observed online"},
   {"values","System","Values","Variables","Table of variable values last observed"},

   {"patches-available","Software","Patches available","Patches available","A list of patches currently claimed to be available by the local package manager"},
   {"patches-installed","Software","Patches installed","Patch status","A list of (un)applied patches according to the local package manager"},


   {"software-installed","Software","Installed","Software installed","List of software packages claimed to be installed according to the local package manager"},

   {"file-change-log","File watch","Change summary","File change log","Log of all detected changes to files from changes promises"},
   {"file-change-diffs","File watch","Text changes","File change diffs","Delta/difference comparison showing file changes"},
   {"setuid-programs","File watch","Setuid","Setuid/gid root programs","Current list of observed setuid/setgid root programs"},


   {"business-value","Accounting","Business value","Business value report","Accumulated value of promises kept"},
   {"benchmarks","Accounting","Benchmarks","Performance","Execution time used to verify selected promises"},
   {"compliance-summary","Accounting","Compliance summary","Compliance summary","Total summary of host compliance"},
   {NULL,NULL,NULL,NULL,NULL}
   };

/*****************************************************************************/

void CSV2XML(Rlist *list)

{ Rlist *rp,*rline,*rl;
  int i;

for (rp = list; rp != NULL; rp = rp->next)
    {
    FILE *fin,*fout;
    char *sp,name[CF_MAXVARSIZE],line[CF_BUFSIZE];

    if ((fin = fopen(rp->item,"r")) == NULL)
       {
       CfOut(cf_inform,"fopen","Cannot open CSV file %s",rp->item);
       continue;
       }

    strncpy(name,rp->item,CF_MAXVARSIZE-1);

    for (sp = name; *sp != '.' && *sp != '\0'; sp++)
       {
       }

    *sp = '\0';

    strcat(name,".xml");

    if ((fout = fopen(name,"w")) == NULL)
       {
       CfOut(cf_inform,"fopen","Cannot open XML file %s",rp->item);
       continue;
       }

    CfOut(cf_verbose,"","Converting %s to %s\n",rp->item,name);

    fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");

    while (!feof(fin))
       {
       line[0] = '\0';
       fgets(line,CF_BUFSIZE-1,fin);
       rline = SplitStringAsRList(line,',');

       fprintf(fout," <line>\n");

       i = 1;

       for (rl = rline; rl != NULL; rl = rl->next)
          {
          if (NUMBER_TXT[i])
             {
             fprintf(fout,"   <%s>%s</%s>\n",NUMBER_TXT[i],(char*)rl->item,NUMBER_TXT[i]);
             i++;
             }
          else
             {
             CfOut(cf_error,"","Too many fields in csv file %s\n",rp->item);
             break;
             }
          }

       fprintf(fout," </line>\n");

       DeleteRlist(rline);
       }

    fprintf(fout,"</output>\n");

    fclose(fin);
    fclose(fout);
    }
}

/*****************************************************************************/

void SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

/* /var/cfengine/promise.log */

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  Item *ip,*file = NULL;
  char start[32],end[32];
  char version[CF_MAXVARSIZE];
  int kept,repaired,notrepaired;
  int i = 0;
  double av_day_kept = 0, av_day_repaired = 0;
  double av_week_kept = 0, av_week_repaired = 0;
  double av_hour_kept = 0, av_hour_repaired = 0;


snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_PROMISE_LOG);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the source log %s",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"compliance.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"compliance.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"compliance.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"compliance.txt");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Policy compliance summary on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Start check",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_end][cfb],"End check",NRH[cfx_end][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],"Policy version",NRH[cfx_version][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_kept][cfb],"%% scheduled promises kept",NRH[cfx_kept][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_repaired][cfb],"%% scheduled promises repaired",NRH[cfx_repaired][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_notrepaired][cfb],"%% scheduled promises ignored",NRH[cfx_notrepaired][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   long startt,endt;
   time_t st,et;
   char timebuffer[26];

   kept = repaired = notrepaired = 0;
   memset(start,0,32);
   memset(end,0,32);
   memset(version,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   // Complex parsing/extraction

   sscanf(ip->name,"%ld,%ld",&startt,&endt);
   st = (time_t)startt;
   et = (time_t)endt;
   snprintf(start,31,"%s",cf_strtimestamp_local(st,timebuffer));
   snprintf(end,31,"%s",cf_strtimestamp_local(et,timebuffer));

   if (!strstr(ip->name,"Outcome of version"))
      {
      continue;
      }
   
   sscanf(strstr(ip->name,"Outcome of version")+strlen("Outcome of version"),"%31[^:]",version);
   sscanf(strstr(ip->name,"to be kept")+strlen("to be kept"), "%d%*[^0-9]%d%*[^0-9]%d",&kept,&repaired,&notrepaired);

// replaces  sscanf(ip->name,"%31[^-]->%31[^O]Outcome of version %250[^:]: Promises observed to be kept %d%*[^0-9]%d%*[^0-9]%d",start,end,version,&kept,&repaired,&notrepaired);

   if (i < 12*24)
      {
      av_day_kept = GAverage((double)kept,av_day_kept,0.5);
      av_day_repaired = GAverage((double)repaired,av_day_repaired,0.5);
      }

   if (i < 12*2)
      {
      av_hour_kept = GAverage((double)kept,av_hour_kept,0.5);
      av_hour_repaired = GAverage((double)repaired,av_hour_repaired,0.5);
      }

   av_week_kept = GAverage((double)kept,av_week_kept,0.1);
   av_week_repaired = GAverage((double)repaired,av_week_repaired,0.1);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],start,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_end][cfb],end,NRX[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_version][cfb],version,NRX[cfx_version][cfe]);
      fprintf(fout,"%s %d %s",NRX[cfx_kept][cfb],kept,NRX[cfx_kept][cfe]);
      fprintf(fout,"%s %d %s",NRX[cfx_repaired][cfb],repaired,NRX[cfx_repaired][cfe]);
      fprintf(fout,"%s %d %s",NRX[cfx_notrepaired][cfb],notrepaired,NRX[cfx_notrepaired][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],start,NRH[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_end][cfb],end,NRH[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],version,NRH[cfx_version][cfe]);
      fprintf(fout,"%s %d%% %s",NRH[cfx_kept][cfb],kept,NRH[cfx_kept][cfe]);
      fprintf(fout,"%s %d%% %s",NRH[cfx_repaired][cfb],repaired,NRH[cfx_repaired][cfe]);
      fprintf(fout,"%s %d%% %s",NRH[cfx_notrepaired][cfb],notrepaired,NRH[cfx_notrepaired][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s,%s,%s,%d,%d,%d",start,end,version,kept,repaired,notrepaired);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table>");
   fprintf(fout,"</div>");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);

METER_KEPT[meter_compliance_week] = av_week_kept;
METER_REPAIRED[meter_compliance_week] = av_week_repaired;
METER_KEPT[meter_compliance_day] = av_day_kept;
METER_REPAIRED[meter_compliance_day] = av_day_repaired;
METER_KEPT[meter_compliance_hour] = av_week_kept;
METER_REPAIRED[meter_compliance_hour] = av_hour_repaired;
}

/*****************************************************************************/

void GrandSummary()

{ char name[CF_BUFSIZE];
  FILE *fout;

Nova_SummarizeComms();

snprintf(name,CF_BUFSIZE-1,"%s/reports/comp_key",CFWORKDIR);
MapName(name);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

fprintf(fout,"Week: %.4lf %.4lf\n",METER_KEPT[meter_compliance_week],METER_REPAIRED[meter_compliance_week]);
fprintf(fout,"Day: %.4lf %.4lf\n",METER_KEPT[meter_compliance_day],METER_REPAIRED[meter_compliance_day]);
fprintf(fout,"Hour: %.4lf %.4lf\n",METER_KEPT[meter_compliance_hour],METER_REPAIRED[meter_compliance_hour]);
fprintf(fout,"Perf: %.4lf %.4lf\n",METER_KEPT[meter_perf_day],METER_REPAIRED[meter_perf_day]);
fprintf(fout,"Soft: %.4lf %.4lf\n",METER_KEPT[meter_other_day],METER_REPAIRED[meter_other_day]);
fprintf(fout,"Comms: %.4lf %.4lf\n",METER_KEPT[meter_comms_hour],METER_REPAIRED[meter_comms_hour]);
fprintf(fout,"Anom: %.4lf %.4lf\n",METER_KEPT[meter_anomalies_day],METER_REPAIRED[meter_anomalies_day]);

fclose(fout);
}

/*****************************************************************************/

void SummarizePerPromiseCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fout;
  char name[CF_BUFSIZE];
  double lsea = SECONDS_PER_WEEK * 52; /* expire after a year */
  Event entry;
  Item *ip,*htmlreport = NULL;
  int ksize,vsize;
  CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *stored;

/* Open the db */

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   CfOut(cf_verbose, "", "!! Could not open database \"%s\"", name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"promise_compliance.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"promise_compliance.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"promise_compliance.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"promise_compliance.txt");
   }

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   CloseDB(dbp);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promise compliance history on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Last checked",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Promise handle/file reference",NRH[cfx_event][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_q][cfb],"Status",NRH[cfx_q][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_av][cfb],"Average",NRH[cfx_av][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_dev][cfb],"Margin",NRH[cfx_dev][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

/* Initialize the key/data return pair. */

while(NextDB(dbp,dbcp,&key,&ksize,&stored,&vsize))
   {
   double measure,av,var;
   time_t then,lastseen,now = time(NULL);
   char tbuf[CF_BUFSIZE],eventname[CF_BUFSIZE];

   cf_strcpy(eventname,(char *)key);

   memcpy(&entry,stored,sizeof(entry));

   then    = entry.t;
   measure = entry.Q.q;
   av = entry.Q.expect;
   var = entry.Q.var;
   lastseen = now - then;

   snprintf(tbuf,CF_BUFSIZE-1,"%s",cf_ctime(&then));
   
   if (then > 0 && lastseen > lsea)
      {
      CfOut(cf_verbose,""," -> Promise usage record \"%s\" expired, removing...\n",eventname);
      DeleteDB(dbp,eventname);
      }
   else
      {
      if (xml)
         {
         fprintf(fout,"%s",NRX[cfx_entry][cfb]);
         fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],tbuf,NRX[cfx_date][cfe]);
         fprintf(fout,"%s <a href=\"promises.cf.html#%s\">%s</a> %s",NRX[cfx_event][cfb],eventname,eventname,NRX[cfx_event][cfe]);
         
         if (measure == 1.0)
            {
            fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"compliant",NRX[cfx_q][cfe]);
            }
         else if (measure == 0.5)
            {
            fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"repaired",NRX[cfx_q][cfe]);
            }
         else if (measure == 0.0)
            {
            fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"non-compliant",NRX[cfx_q][cfe]);
            }
         
         fprintf(fout,"%s %.1lf %s",NRX[cfx_av][cfb],av*100.0,NRX[cfx_av][cfe]);
         fprintf(fout,"%s %.1lf %s",NRX[cfx_dev][cfb],sqrt(var)*100.0,NRX[cfx_dev][cfe]);
         
         fprintf(fout,"%s",NRX[cfx_entry][cfe]);
         }
      else if (html)
         {
         char b1[CF_MAXVARSIZE],b2[CF_MAXVARSIZE],b3[CF_MAXVARSIZE],b4[CF_MAXVARSIZE],b5[CF_MAXVARSIZE],b6[CF_MAXVARSIZE];
         char vbuff[CF_BUFSIZE];
         
         snprintf(b1,CF_MAXVARSIZE-1,"%s %s %s %s",NRH[cfx_entry][cfb],NRH[cfx_date][cfb],tbuf,NRH[cfx_date][cfe]);
         snprintf(b2,CF_MAXVARSIZE-1,"%s <a href=\"promises.cf.html#%s\">%s</a> %s",NRH[cfx_event][cfb],eventname,eventname,NRH[cfx_event][cfe]);
         
         if (measure == 1.0)
            {
            snprintf(b3,CF_MAXVARSIZE-1,"%s %s %s",NRH[cfx_q][cfb],"compliant",NRH[cfx_q][cfe]);
            }
         else if (measure == 0.5)
            {
            snprintf(b3,CF_MAXVARSIZE-1,"%s %s %s",NRH[cfx_q][cfb],"repaired",NRH[cfx_q][cfe]);
            }
         else if (measure == 0.0)
            {
            snprintf(b3,CF_MAXVARSIZE-1,"%s %s %s",NRH[cfx_q][cfb],"non-compliant",NRH[cfx_q][cfe]);
            }
         
         snprintf(b4,CF_MAXVARSIZE-1,"%s %.1lf %s",NRH[cfx_av][cfb],av*100.0,NRH[cfx_av][cfe]);
         snprintf(b5,CF_MAXVARSIZE-1,"%s %.1lf %s",NRH[cfx_dev][cfb],sqrt(var)*100.0,NRH[cfx_dev][cfe]);
         snprintf(b6,CF_MAXVARSIZE-1,"%s",NRH[cfx_entry][cfe]);
         snprintf(vbuff,CF_BUFSIZE-1,"%s %s %s %s %s %s",b1,b2,b3,b4,b5,b6);
         
         PrependItem(&htmlreport,vbuff,NULL);
         htmlreport->time = then;
         }
      else
         {
         if (measure == 1.0)
            {
            fprintf(fout,"%s,%s,compliant,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
            }
         else if (measure == 0.5)
            {
            fprintf(fout,"%s,%s,repaired,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
            }
         else if (measure == 0.0)
            {
            fprintf(fout,"%s,%s,non-compliant,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
            }
         
         }
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);

if (html && !embed)
   {
   htmlreport = SortItemListTimes(htmlreport);

   for (ip = htmlreport; ip != NULL; ip=ip->next)
      {
      fprintf(fout,"%s",ip->name);
      }
   
   fprintf(fout,"</table></div>");
   DeleteItemList(htmlreport);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
}

/*****************************************************************************/

void SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  Item *ip,*file = NULL;
  char start[32];

snprintf(name,CF_BUFSIZE,"%s/cfagent.%s.log",CFWORKDIR,VSYSNAME.nodename);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"setuid.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"setuid.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"setuid.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"setuid.txt");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Known setuid programs on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Filename",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

file = SortItemListNames(file);

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(start,0,32);
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],ip->name,NRX[cfx_end][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],ip->name,NRH[cfx_end][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s",ip->name);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>\n");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],datestr[CF_MAXVARSIZE],size[CF_MAXVARSIZE];
  char no[CF_SMALLBUF],change[CF_BUFSIZE],reformat[CF_BUFSIZE],output[2*CF_BUFSIZE],aggregate[CF_BUFSIZE];
  Item *ip,*file = NULL;
  char pm;
  long start;
  time_t now;
  int i = 0,truncate;

  snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,CF_FILECHANGE);
  MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"file_changes.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"file_changes.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"file_changes.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"file_changes.txt");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"File change events recorded on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Change detected",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"File change",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   char timebuffer[26];
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%ld,%1023[^\n]",&start,name);
   now = (time_t)start;
   snprintf(datestr,CF_MAXVARSIZE-1,"%s",cf_strtimestamp_local(now,timebuffer));
   Chop(datestr);
   
   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],datestr,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],name,NRX[cfx_end][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],datestr,NRH[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],name,NRH[cfx_end][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s,%s",datestr,name);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);


/* Detail log */

file = NULL;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_DIFF_LOG);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s",name);
   return;
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   if (strncmp(line,"CHANGE",strlen("CHANGE")) != 0)
      {
      continue;
      }

   name[0] = '\0';
   sscanf(line,"CHANGE %[^\n]",name);

   fgets(line,CF_BUFSIZE-1,fin);
   sscanf(line,"%128[^;];%[^\n]",datestr,size);

   if (strncmp(datestr,"END",strlen("END")) == 0)
      {
      continue;
      }

   memset(aggregate,0,CF_BUFSIZE);
   snprintf(aggregate,CF_BUFSIZE-1,"%s<br>",aggregate);
   output[0] = '\0';

   truncate = false;

   while (!feof(fin))
      {
      line[0] = '\0';
      fgets(line,CF_BUFSIZE-1,fin);

      if (strncmp(line,"END",strlen("END")) == 0)
         {
         break;
         }

      no[0] = '\0';
      change[0] = '\0';
      sscanf(line,"%c,%[^,],%1024[^\n]",&pm,no,change);

      if (!truncate)
         {
         if (xml)
            {
            snprintf(reformat,CF_BUFSIZE-1,"<pm>%c</pm><line>%s</line> <event>%s</event>\n",pm,no,change);

            if (!JoinSuffix(aggregate,reformat))
               {
               truncate = true;
               }
            }
         else if (html)
            {
            snprintf(reformat,CF_BUFSIZE-1,"<span id=\"pm\">%c</span><span id=\"line\">%s</span><span id=\"change\">%s</span><br>",pm,no,change);

            if (!JoinSuffix(aggregate,reformat))
               {
               truncate = true;
               }
            }
         else
            {
            snprintf(reformat,CF_BUFSIZE-1,"   %s\n",line);
            if (!JoinSuffix(aggregate,reformat))
               {
               }
            }
         }
      }

   if (xml)
      {
      snprintf(output,CF_BUFSIZE-1,
               "%s"
               "%s %s %s"
               "%s %s %s"
               "%s %s %s"
               "%s\n",
               NRX[cfx_entry][cfb],
               NRX[cfx_date][cfb],datestr,NRX[cfx_date][cfe],
               NRX[cfx_filename][cfb],name,NRX[cfx_end][cfe],
               NRX[cfx_event][cfb],aggregate,NRX[cfx_event][cfe],
               NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      if (truncate)
         {
         snprintf(output,CF_BUFSIZE-1,
                  "%s"
                  "%s %s %s"
                  "%s %s %s"
                  "%s %s <br>Truncated, full record at %s:%s/cfdiff.log %s"
                  "%s\n",
                  NRH[cfx_entry][cfb],
                  NRH[cfx_date][cfb],datestr,NRH[cfx_date][cfe],
                  NRH[cfx_filename][cfb],name,NRH[cfx_end][cfe],
                  NRH[cfx_event][cfb],aggregate,VFQNAME,CFWORKDIR,NRH[cfx_event][cfe],
                  NRH[cfx_entry][cfe]);
         }
      else
         {
         snprintf(output,CF_BUFSIZE-1,
                  "%s"
                  "%s %s %s"
                  "%s %s %s"
                  "%s %s %s"
                  "%s\n",
                  NRH[cfx_entry][cfb],
                  NRH[cfx_date][cfb],datestr,NRH[cfx_date][cfe],
                  NRH[cfx_filename][cfb],name,NRH[cfx_end][cfe],
                  NRH[cfx_event][cfb],aggregate,NRH[cfx_event][cfe],
                  NRH[cfx_entry][cfe]);
         }
      }
   else
      {
      snprintf(output,CF_BUFSIZE-1,"%s %s %s",datestr,name,aggregate);
      }

   if (strlen(aggregate) > 0)
      {
      PrependItem(&file,output,NULL);
      aggregate[0] = '\0';
      }
   }

fclose(fin);

if (html)
   {
   snprintf(name,CF_BUFSIZE,"file_diffs.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"file_diffs.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"file_diffs.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"file_diffs.txt");
   }

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"File difference events recorded on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time of change event",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Filename",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Delta",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (i = 0,ip = file; ip != NULL; ip = ip->next)
   {
   fprintf(fout,"%s",ip->name);
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void SummarizePromiseRepaired(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  char handle[CF_MAXVARSIZE];
  time_t date;
  long then;
  Item *ip,*file = NULL;
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_REPAIR_LOG);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"promise_repair.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"promise_repair.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"promise_repair.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"promise_repair.txt");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promises repaired on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Handle",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%ld,%512[^,]",&then,handle);
   date = (time_t)then;

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %ld %s",NRX[cfx_date][cfb],date,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],handle,NRX[cfx_event][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],cf_ctime(&date),NRH[cfx_date][cfe]);
      fprintf(fout,"%s <a href=\"promises.cf.html#%s\">%s</a> %s",NRH[cfx_event][cfb],handle,handle,NRH[cfx_event][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s",ip->name);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void SummarizePromiseNotKept(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  char handle[CF_MAXVARSIZE];
  time_t date;
  long then;
  Item *ip,*file = NULL;
  char start[32];
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_NOTKEPT_LOG);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"promise_notkept.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"promise_notkept.xml");
   }
else if (csv)
   {
   snprintf(name,CF_BUFSIZE,"promise_notkept.csv");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"promise_notkept.txt");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   PrependItem(&file,line,NULL);
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promises not kept on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Handle",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(start,0,32);
   memset(name,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%ld,%512[^,]",&then,handle);
   date = (time_t)then;
   
   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %ld %s",NRX[cfx_date][cfb],date,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],handle,NRX[cfx_event][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],cf_ctime(&date),NRH[cfx_date][cfe]);
      fprintf(fout,"%s <a href=\"promises.cf.html#%s\">%s</a> %s",NRH[cfx_event][cfb],handle,handle,NRH[cfx_event][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s",ip->name);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void ReportPatches(struct CfPackageManager *list)

{ FILE *fout;
  struct CfPackageManager *mp = NULL;
  struct CfPackageItem *pi;
  char name[CF_BUFSIZE];
  int count = 0;

snprintf(name,CF_BUFSIZE,"%s/state/%s",CFWORKDIR,NOVA_PATCHES_INSTALLED);
MapName(name);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->patch_list; pi != NULL; pi=pi->next)
      {
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

fclose(fout);

snprintf(name,CF_BUFSIZE,"%s/state/%s",CFWORKDIR,NOVA_PATCHES_AVAIL);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->patch_avail; pi != NULL; pi=pi->next)
      {
      count++;
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

fclose(fout);
}

/*****************************************************************************/

void SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  Item *ip,*file = NULL;
  int i = 0;

snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,SOFTWARE_PACKAGES_CACHE);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,sizeof(name),"software_packages.html");
   }
else if (xml)
   {
   snprintf(name,sizeof(name),"software_packages.xml");
   }
else
   {
   snprintf(name,sizeof(name),"software_packages.csv");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,sizeof(name),"Software versions installed %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Package",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],"Version",NRH[cfx_version][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],"Architecture",NRH[cfx_ref][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Manager",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],name,NRX[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_version][cfb],version,NRX[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_ref][cfb],arch,NRX[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],mgr,NRX[cfx_event][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],name,NRH[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],version,NRH[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],arch,NRH[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],mgr,NRH[cfx_event][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>\n");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  Item *ip,*file = NULL;
  int i = 0, count = 0;

CfOut(cf_verbose,"","Creating available patch report...\n");

snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_PATCHES_AVAIL);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_MAXVARSIZE,"software_patches_avail.html");
   }
else if (xml)
   {
   snprintf(name,CF_MAXVARSIZE,"software_patches_avail.xml");
   }
else
   {
   snprintf(name,CF_MAXVARSIZE,"software_patches_avail.csv");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,sizeof(name),"Software updates available and outstanding for %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Package",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],"Version",NRH[cfx_version][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],"Architecture",NRH[cfx_ref][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Manager",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],name,NRX[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_version][cfb],version,NRX[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_ref][cfb],arch,NRX[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],mgr,NRX[cfx_event][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],name,NRH[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],version,NRH[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],arch,NRH[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],mgr,NRH[cfx_event][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   count++;

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>\n");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
file = NULL;

/* Now show installed patch level */

CfOut(cf_verbose,"","Creating patch status report...\n");

snprintf(name,sizeof(name),"%s/state/%s",CFWORKDIR,NOVA_PATCHES_INSTALLED);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,sizeof(name),"software_patch_status.html");
   }
else if (xml)
   {
   snprintf(name,sizeof(name),"software_patch_status.xml");
   }
else
   {
   snprintf(name,sizeof(name),"software_patch_status.csv");
   }

/* Max 2016 entries - at least a week */

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);

   if (!IsItemIn(file,line))
      {
      PrependItem(&file,line,NULL);
      }
   }

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,sizeof(name),"Software patches installed on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Package",NRH[cfx_filename][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],"Version",NRH[cfx_version][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],"Architecture",NRH[cfx_ref][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Manager",NRH[cfx_event][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   memset(name,0,CF_MAXVARSIZE);
   memset(version,0,CF_MAXVARSIZE);
   memset(arch,0,CF_MAXVARSIZE);
   memset(mgr,0,CF_MAXVARSIZE);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   sscanf(ip->name,"%250[^,],%250[^,],%250[^,],%250[^\n]",name,version,arch,mgr);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],name,NRX[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_version][cfb],version,NRX[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_ref][cfb],arch,NRX[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],mgr,NRX[cfx_event][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],name,NRH[cfx_filename][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],version,NRH[cfx_version][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],arch,NRH[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],mgr,NRH[cfx_event][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else
      {
      fprintf(fout,"%s",ip->name);
      }

   if (++i > 12*24*7)
      {
      break;
      }
   }

if (html && !embed)
   {
   fprintf(fout,"</table></div>\n");
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void SummarizeVariables(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{
  char line[CF_BUFSIZE];
  FILE *fin,*fout;
  char name[CF_MAXVARSIZE];

snprintf(name,sizeof(name),"%s/state/vars.out",CFWORKDIR);
MapName(name);

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"fopen","Cannot open variable notes %s (try again later)",name);
   return;
   }

if (html)
   {
   snprintf(name,sizeof(name),"variables.html");
   }
else
   {
   return;
   }

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,sizeof(name),"Main variables instantiated on %s",VFQNAME);
   fprintf(fout,"<div id=\"reporttext\">\n");
   }

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   fprintf(fout,"%s",line);
   }

fclose(fin);

if (html && !embed)
   {
   fprintf(fout,"</div>\n");
   }

fclose(fout);
}

/*****************************************************************************/

void Nova_SummarizeComms()

/* Go through the database of recent connections and check for
   Long Time No See ...*/

{ CF_DB *dbp;
  CF_DBC *dbcp;
  int ksize,vsize;
  KeyHostSeen entry;
  double kept = 1,not_kept = 0,repaired = 0,var,average;
  char name[CF_BUFSIZE], *key;
  void *value;
  double now = (double)time(NULL),then,resolution = 300;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_LASTDB_FILE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   CloseDB(dbp);
   return;
   }

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   memset(&entry, 0, sizeof(entry));

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));

      then = entry.Q.q;
      average = (double)entry.Q.expect;
      var = (double)entry.Q.var;
      }
   else
      {
      continue;
      }

   if ((now - then > average+2.0*sqrt(var)) && (now - then > resolution*2))
      {
      not_kept++;
      }
   else if ((now - then > average+sqrt(var)) && (now - then > resolution*2))
      {
      repaired++;
      }
   else
      {
      kept++;
      }

   memset(&value,0,sizeof(value));
   memset(&key,0,sizeof(key));
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);

METER_KEPT[meter_comms_hour] = 100.0*kept/(kept+repaired+not_kept);
METER_REPAIRED[meter_comms_hour] = 100.0*repaired/(kept+repaired+not_kept);
}

/*****************************************************************************/

void SummarizeValue(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{
  char name[CF_BUFSIZE];
  CF_DB *dbp;
  CF_DBC *dbcp;
  int ksize,vsize;
  void *value;
  char *key;
  FILE *fout;
  time_t now = time(NULL);
  struct promise_value pt;
  Item *ip,*data = NULL;
  char timebuffer[26];

// Strip out the date resolution so we keep only each day of the year
  
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_VALUE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

if (NewDBCursor(dbp,&dbcp))
   {
   while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
      {
      if (value == NULL)
         {
         continue;
         }

      memcpy(&pt,value,sizeof(pt));
      snprintf(name,CF_BUFSIZE,"<td>%.4lf</td><td>%.4lf</td><td>%.4lf</td>",pt.kept,pt.repaired,pt.notkept);
      AppendItem(&data,key,name);
      }
   }

CloseDB(dbp);

if ((fout = fopen("value_report.html","w")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Unable to write value report");
   return;
   }

snprintf(name,sizeof(name),"Value return from Cfengine on %s",VFQNAME);
fprintf(fout,"<div id=\"reporttext\">");
fprintf(fout,"<p>Last measured on %s",cf_strtimestamp_local(now,timebuffer));
fprintf(fout,"<table class=\"border\">\n");

fprintf(fout,"<tr><th>Day</th><th>Promises Kept</th><th>Repairs</th><th>Not kept</th></tr>\n");

for (ip = data; ip != NULL; ip=ip->next)
   {
   fprintf(fout,"<tr><td>%s</td>%s</tr>\n",ip->name,ip->classes);
   }

fprintf(fout,"</table></div>\n");
fclose(fout);
    
DeleteItemList(data);
}

/*****************************************************************************/

void Nova_SummarizeLicense(char *stylesheet,char *header,char *footer,char *webdriver)

{ CF_DB *dbp;
  CF_DBC *dbcp;
  int ksize,vsize;
  void *value;
  char *key;
  int min = 9999999,max = -1,count,lic1,lic2,i = 0;
  char name[CF_BUFSIZE];
  long ltime;
  time_t now,dt,then;
  double average,granted,sum_t = 0,ex_t = 0,lic_t = 0;
  FILE *fout = stdout;
  char timebuffer[26];
  
CfOut(cf_verbose,""," -> Writing license summary");

snprintf(name,CF_MAXVARSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_LICENSE);
MapName(name);

// Calculate utilization in each dt of the record

then = time(NULL); // Set this to now for first round

if (OpenDB(name,&dbp))
   {
   if (NewDBCursor(dbp,&dbcp))
      {
      while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
         {
         if (value == NULL)
            {
            continue;
            }

         count = lic1 = lic2 = 0;
         ltime = 0L;
         
         sscanf(value,"%d,%d,%d,%ld",&count,&lic1,&lic2,&ltime);

         i++;
         now = (time_t)ltime;
         
         if (count > max)
            {
            max = count;
            }

         if (count < min)
            {
            min = count;
            }

         dt = now - then;

         if (dt > 0)
            {
            ex_t += (double)dt*count;
            lic_t += (double)dt*lic1;
            sum_t += (double)dt;
            }

         if (then > 0)
            {
            then = now;
            }
         else
            {
            then = time(NULL);
            }
         }

      DeleteDBCursor(dbp,dbcp);
      }
   CloseDB(dbp);
   }

now = time(NULL);
snprintf(name,sizeof(name),"Mean observable license usage");
fprintf(fout,"<div id=\"reporttext\">");
fprintf(fout,"<h4>Last measured on %s based on %d samples</h4>",cf_strtimestamp_local(now,timebuffer),i);
fprintf(fout,"<table class=\"border\">\n");

if (sum_t > 0)
   {
   average = ex_t/sum_t;
   granted = lic_t/sum_t;
   fprintf(fout,"<tr><td>Minimum observed level</td><td> &ge; %d</td><tr>\n",min);
   fprintf(fout,"<tr><td>Maximum observed level</td><td> &ge; %d hosts</td><tr>\n",max);
   fprintf(fout,"<tr><td>Mean actual usage</td><td> &ge; %lf</td></tr>\n",average);
   fprintf(fout,"<tr><td>Mean expected usage</td><td> &le; %lf</td><tr>\n",granted);
   fprintf(fout,"<tr><td>Mean utilization</td><td> &le; %lf %%</td><tr>\n",average/granted*100.0);
   }
else
   {
   fprintf(fout,"<tr><td>Minimum observed level</td><td> &ge; %d</td><tr>\n",min);
   fprintf(fout,"<tr><td>Maximum observed level</td><td> &ge; %d hosts</td><tr>\n",max);
   fprintf(fout,"<tr><td>Mean usage</td><td> unknown</td><tr>\n");
   }

fprintf(fout,"</table></div>\n");

}

/*****************************************************************************/

void Nova_ZenossSummary(char *docroot)

{ char name[CF_MAXVARSIZE];
 Item *clist = NULL,*ip;
  FILE *fout;

snprintf(name,sizeof(name),"%s/reports/summary.z",docroot);
MapName(name);

#ifdef HAVE_LIBMONGOC
clist = Nova_RankHosts(NULL,0,cfrank_compliance,200);
#endif

if ((fout = fopen(name,"w")))
   {
   for (ip = clist; ip != NULL; ip=ip->next)
      {
      if (strcmp(ip->name,"localhost") == 0 || strcmp(ip->name,"127.0.0.1") == 0)
         {
         }
      else
         {
         fprintf(fout,"%s,/Server/%s,%d\n",Hostname2IPString(ip->name),Titleize(ip->classes),ip->counter);
         }
      }
   
   fclose(fout);
   }

chmod(name,0644);
}

/*****************************************************************************/

void Nova_NoteVarUsageDB(void)

/* WARNING: Not thread safe (access to VSCOPE) */

{ Scope *ptr;
  char filename[CF_BUFSIZE];
  CF_DB *dbp;
  CF_DBC *dbcp;
  char key[CF_MAXVARSIZE], *keyDb;  // scope.varname
  void *val;
  struct Variable var = {{0}}, *varDb;
  int keyDbSize, valSize;
  time_t varExpireAge = SECONDS_PER_WEEK * 4;  // remove vars from DB after about one month
  time_t now = time(NULL);

if (MINUSF) /* Only do this for the default policy */
   {
   return;
   }
 
snprintf(filename,sizeof(filename),"%s/state/%s",CFWORKDIR,CF_VARIABLES);
MapName(filename);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

/* sync db with current vars */

// NOTE: can extend to support avg and stddev in future
var.e.t = now;  // all are last seen now

for (ptr = VSCOPE; ptr != NULL; ptr=ptr->next)
   {
   HashIterator i = HashIteratorInit(ptr->hashtable);
   CfAssoc *assoc;

   if (strcmp(ptr->scope,"this") == 0)
      {
      continue;
      }

   while ((assoc = HashIteratorNext(&i)))
      {
      snprintf(key,sizeof(key),"%s.%s", ptr->scope, assoc->lval);
      var.dtype = assoc->dtype;
      var.rtype = assoc->rval.rtype;
      var.rval[0] = '\0';

      if (!PrintRval(var.rval, sizeof(var.rval) - 15, assoc->rval))
         {
         CfOut(cf_verbose, "", "!! Variable %s is too large for transmission to reporting hub (larger than %d bytes) -- will be truncated in reports", key, sizeof(var.rval));
         }
      
      WriteDB(dbp,key,&var,VARSTRUCTUSAGE(var));
      }
   }

/* purge old entries from DB */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to purge variable db");
   CloseDB(dbp);
   return;
   }

while(NextDB(dbp,dbcp,&keyDb,&keyDbSize,&val,&valSize))
   {
   if (val != NULL)
      {
      varDb = (struct Variable *)val;
      
      if (varDb->e.t < now - varExpireAge)
         {
         CfDebug("Variable record %s expired\n",keyDb);
         DeleteDB(dbp,keyDb);
         }
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_OpenCompilationReportFiles(const char *fname)
{
#if defined(HAVE_LIBMONGOC)
if ((FREPORT_TXT = fopen(NULLFILE,"w")) == NULL)
   {
   FatalError("Could not write output log to %s",NULLFILE);
   }

if ((FREPORT_HTML = fopen(NULLFILE,"w")) == NULL)
   {
   FatalError("Could not write output log to %s",NULLFILE);
   }
#else
OpenCompilationReportFiles(fname);
#endif
}

/*****************************************************************************/

void Nova_ShowPromises(Bundle *bundles, Body *bodies)
{
#if defined(HAVE_LIBMONGOC)
Nova_StoreUnExpandedPromises(bundles, bodies);
#else
ShowPromisesInReport(bundles, bodies);
#endif
}

/*****************************************************************************/

void Nova_ShowPromise(const char *version, Promise *pp, int indent)
{
#if defined (HAVE_LIBMONGOC)
Nova_StoreExpandedPromise(pp);
if (FKNOW)
   {
   Nova_MapPromiseToTopic(FKNOW,pp,version);
   }
#else
ShowPromiseInReport(version, pp, indent);
#endif
}

/*********************************************************************/

int Nova_ExportReports(char *reportName)
/*
 * Export from dbm files to one text file.
 */
{
 Item *reports = NULL, *ip;
 enum cfd_menu reportType;
 time_t from;
 char filePath[CF_MAXVARSIZE];
 FILE *fout;

 if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The license has expired");
   return false;
   }

 reportType = String2Menu(reportName);

 switch(reportType)
    {
    case cfd_menu_delta:
        from = time(NULL) - 60*10; // delta = last 10 minutes
        break;
    case cfd_menu_full:
        from = time(NULL) - SECONDS_PER_WEEK; // full = last week
        break;
    default:
        CfOut(cf_error, "", "!! Nova_ExportReports: reportType is not delta or full but %d", reportType);
        return false;
    }
 
 snprintf(filePath, sizeof(filePath), "%s/reports/nova_export.nov", CFWORKDIR);
 MapName(filePath);

 if ((fout = fopen(filePath,"w")) == NULL)
    {
    CfOut(cf_error,"fopen","!! Cannot open export file path %s", filePath);
    return false;
    }

 CfOut(cf_inform, "", " -> Saving all Nova reports to %s", filePath);

 fprintf(fout, "%s %s %s %s %s\n", NOVA_EXPORT_HEADER, reportName, PUBKEY_DIGEST, VIPADDRESS, VFQNAME);
 
 Nova_PackAllReports(&reports,from,0,reportType);

 for(ip = reports; ip != NULL; ip = ip->next)
    {
    ReplaceTrailingChar(ip->name, '\n', '\0');
    fprintf(fout, "%s\n", ip->name);
    }

 DeleteItemList(reports);
 fclose(fout);
 
 return true;
}

/*********************************************************************/

#ifdef HAVE_LIBMONGOC

int Nova_ImportHostReports(mongo_connection *dbconnp, const char *filePath)
/*
 * Import from text file to Mongo database.
 * NOTE: Should only be called on Nova hub.
 */
{
 mongo_connection dbconn;
 char header[CF_MAXVARSIZE];
 FILE *fin;
 
if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The license has expired");
   return false;
   }

if(!CFDB_Open(&dbconn))
    {
    return false;
    }
    
if ((fin = fopen(filePath,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","!! Cannot open import file %s", filePath);
   CFDB_Close(&dbconn);
   return false;
   }

 CfReadLine(header, sizeof(header), fin);

 Nova_ImportHostReportsFromStream(&dbconn, header, fin);

 fclose(fin);
 CFDB_Close(&dbconn);
 
 return true;
}

/*********************************************************************/

int Nova_ImportHostReportsFromStream(mongo_connection *dbconn, char *header, FILE *fin)
{
 char keyHash[CF_MAXVARSIZE] = {0}, ipAddr[CF_MAXVARSIZE] = {0}, hostName[CF_MAXVARSIZE] = {0};
 char buf[CF_BUFSIZE];
 char headerText[CF_SMALLBUF], reportType[CF_SMALLBUF];
 Item *reports[CF_CODEBOOK_SIZE] = {0};
 char validate[5];
 time_t delta1, genTime;
 long length;
 int currReport = -1;
 
 // OK to leave hostname blank - reverse lookup later
 if(sscanf(header,"%32s %32s %255s %255s %255s",headerText, reportType, keyHash, ipAddr, hostName) < 4)
    {
    CfOut(cf_error, "", "!! Error parsing first line of report header");
    return false;
    }

 CfReadLine(buf, sizeof(buf), fin);
 if(sscanf(buf,"%4s %ld %ld %ld",validate,&delta1,&genTime,&length) != 4)
    {
    CfOut(cf_error, "", "!! Error parsing second line of report header");
    return false;    
    }
 
 if (strcmp(validate,"CFR:") != 0)
    {
    CfOut(cf_error,""," !! Invalid report format in - second line is %s not CFR:", validate);
    return false;
    }

 cf_strtimestamp_utc(genTime, buf);

 CfOut(cf_inform, "", " -> Importing Nova %s reports from host %s with UTC timestamp %s", reportType, keyHash, buf);

 NewReportBook(reports);

 while (CfReadLine(buf, sizeof(buf), fin))
    {
    CfDebug("%s\n", buf);
    currReport = Nova_StoreIncomingReports(buf,reports,currReport);
    }

 if (reports == NULL)
    {
    return false;
    }

 CFDB_SaveHostID(dbconn,MONGO_DATABASE,cfr_keyhash,keyHash,ipAddr,hostName);
 CFDB_SaveHostID(dbconn,MONGO_ARCHIVE,cfr_keyhash,keyHash,ipAddr,hostName);
     
 UnpackReportBook(dbconn,keyHash,reports);
 DeleteReportBook(reports);
 
 return true;
}

#endif  /*  HAVE_LIBMONGOC */

/*********************************************************************/

void Nova_CommandAPI(char *lsdata,char *name,char *phandle,char *hostkey,char *classregex)
    
{
#ifdef HAVE_LIBMONGOC
  int plen = strlen(phandle);
  int slen = strlen(lsdata);
  int hlen = strlen(hostkey);
  int clen = strlen(classregex);

if (hlen > 0 && slen == 0 && plen == 0 && clen == 0)
   {
   // Show host details
   char name[CF_MAXVARSIZE] = {0},address[CF_MAXVARSIZE] = {0};
   
   Nova2Txt_hostinfo(hostkey,name,address,CF_MAXVARSIZE);
   printf(" -> Hostname: %s\n",name);
   printf(" -> Recent IP Addresses: %s\n",address);
   return;
   }

if (strcmp(lsdata,"promises") == 0)
   {
   Nova2Txt_compliance_promises(hostkey,phandle,NULL,true,classregex);
   return;
   }
else if (strcmp(lsdata,"compliance") == 0)
   {
   Nova2Txt_compliance_report(hostkey,name,-1,-1,-1,-1,">",classregex);
   return;
   }
else if (strcmp(lsdata,"summary") == 0)
   {
   Nova2Txt_summary_report(hostkey,phandle,NULL,true,classregex);
   return;
   }
else if (strcmp(lsdata,"dead-clients") == 0)
   {
   Nova2Txt_deadclient_report(hostkey,NULL,NULL,NULL,-1,true,classregex);
   return;
   }
else if (strcmp(lsdata,"last-seen") == 0)
   {
   Nova2Txt_lastseen_report(hostkey,NULL,NULL,NULL,-1,true,classregex);
   return;
   }
else if (strcmp(lsdata,"software") == 0)
   {
   Nova2Txt_software_report(hostkey,name,"","",true,cfr_software,classregex);
   return;
   }
else if (strcmp(lsdata,"vars") == 0)
   {
   char lval[CF_MAXVARSIZE] = {0};
   char scope[CF_MAXVARSIZE] = {0},*sp;
       
   if (name && (sp = strchr(name,'.')))
      {
      if (*(sp+1) == '*') // If it looks like a regex, don't split on .
         {
         Nova2Txt_vars_report(hostkey,NULL,name,NULL,NULL,true,classregex);
         return;
         }
      else
         {
         sscanf(name,"%[^.].%s",scope,lval);
         Nova2Txt_vars_report(hostkey,scope,lval,NULL,NULL,true,classregex);
         return;
         }
      }

   Nova2Txt_vars_report(hostkey,NULL,NULL,NULL,NULL,true,NULL);
   return;
   }
else if (strcmp(lsdata,"file_changes") == 0)
   {
   Nova2Txt_filechanges_report(hostkey,name,true,-1,">",classregex);
   return;
   }
else if (strcmp(lsdata,"file_diffs") == 0)
   {
   Nova2Txt_filediffs_report(hostkey,name,NULL,true,-1,">",classregex);
   return;
   }
else if (strcmp(lsdata,"setuid") == 0)
   {
   Nova2Txt_setuid_report(hostkey,name,true,classregex);
   return;
   }
else if (strcmp(lsdata,"license") == 0)
   {
   char buffer[CF_BUFSIZE];
   Nova2Txt_getlicense(buffer,CF_BUFSIZE);
   printf("%s\n",buffer);
   return;
   }


printf("Unknown report\n");
#endif
}

/*****************************************************************************/

void Nova_ImportReports(const char *input_file)
{
 #ifdef HAVE_LIBMONGOC
 if (IsDefinedClass("am_policy_hub"))
    {
    mongo_connection dbconn;

    if(!CFDB_Open(&dbconn))
       {
       CfOut(cf_error, "", "!! Could not connect to database -- skipping import");
       GenericDeInitialize();
       exit(0);
       }

    CFDB_EnsureIndices(&dbconn);

    if(Nova_ImportHostReports(&dbconn, input_file))
       {
       GenericDeInitialize();
       exit(0);
       }
    else
       {
       GenericDeInitialize();
       exit(1);
       }
    }
 else
    {
    CfOut(cf_error, "", "Importing reports is only possible on Nova policy hubs");
    }

#else  /* NOT HAVE_LIBMONGOC */
 CfOut(cf_error, "", "Importing reports is only possible on Nova policy hubs");
#endif
}

/*****************************************************************************/

void NoteEfficiency(double e)
{
Attributes a = {{0}};
Promise p = {0};

NovaNamedEvent("Configuration model efficiency",e,a,&p);
CfOut(cf_verbose,""," -> Configuration model efficiency for %s = %.2lf%%",VUQNAME,e);
}
