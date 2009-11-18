
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
    "<entry>\n","\n</entry>\n",
    "<event>\n","\n</event>\n",
    "<hostname>\n","\n</hostname>\n",
    "<pm>\n","\n</pm>\n",
    "<ip>\n","\n</ip>\n",
    "<date>\n","\n</date>\n",
    "<q>\n","\n</q>\n",
    "<expect>\n","\n</expect>\n",
    "<sigma>\n","\n</sigma>\n",
    "<version>\n","\n</version>\n",
    "<ref>\n","\n</ref>\n",
    "<filename>\n","\n</filename>\n",
    "<index>\n","\n</index>\n",
    "<min>\n","\n</min>\n",
    "<max>\n","\n</max>\n",
    "<end>\n","\n</end>\n",
    "<kept>\n","\n</kept>\n",
    "<repaired>\n","\n</repaired>\n",
    "<notrepaired>\n","\n</notrepaired>\n",
    NULL,NULL
   };

char *NRH[][2] =
   {
    "<tr>","</tr>\n\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td bgcolor=#add8e6>","</td>\n",
    "<td bgcolor=#e0ffff>","</td>\n",
    "<td bgcolor=#f0f8ff>","</td>\n",
    "<td bgcolor=#fafafa>","</td>\n",
    "<td bgcolor=#ededed>","</td>\n",
    "<td bgcolor=#e0e0e0>","</td>\n",
    "<td bgcolor=#add8e6>","</td>\n",
    "<td bgcolor=#e0ffff>","</td>\n",
    "<td bgcolor=#fafafa><small>","</small></td>\n",
    "<td bgcolor=#fafafa><small>","</small></td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    "<td>","</td>\n",
    NULL,NULL
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

/*****************************************************************************/

void Nova_CSV2XML(struct Rlist *list)

{ struct Rlist *rp,*rline,*rl;
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
             fprintf(fout,"   <%s>%s</%s>\n",NUMBER_TXT[i],rl->item,NUMBER_TXT[i]);
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

void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

/* /var/cfengine/promise.log */

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32],end[32],*sp;
  char version[CF_MAXVARSIZE];
  int kept,repaired,notrepaired;
  int i = 0,today = false;
  double av_day_kept = 0, av_day_repaired = 0;
  double av_week_kept = 0, av_week_repaired = 0;
  double av_hour_kept = 0, av_hour_repaired = 0;


snprintf(name,CF_BUFSIZE-1,"%s/promise.log",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Policy compliance summary on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Start check",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_end][cfb],"End check",NRH[cfx_end][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_version][cfb],"Policy version",NRH[cfx_version][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_kept][cfb],"% scheduled promises kept",NRH[cfx_kept][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_repaired][cfb],"% scheduled promises repaired",NRH[cfx_repaired][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_notrepaired][cfb],"% scheduled promises ignored",NRH[cfx_notrepaired][cfe]);
   fprintf(fout,"%s",NRH[cfx_entry][cfe]);
   }
else if (XML)
   {
   fprintf(fout,"<?xml version=\"1.0\"?>\n<output>\n");
   }

for (ip = file; ip != NULL; ip = ip->next)
   {
   kept = repaired = notrepaired = 0;
   memset(start,0,32);
   memset(end,0,32);
   memset(version,0,255);

   if (cf_strlen(ip->name) == 0)
      {
      continue;
      }

   // Complex parsing/extraction

   sscanf(ip->name,"%31[^-]",start);
   sscanf(strstr(ip->name,"Outcome of version")+strlen("Outcome of version"),"%31[^:]",version);
   sscanf(strstr(ip->name,"to be kept")+strlen("to be kept"), "%d%*[^0-9]%d%*[^0-9]%d",&kept,&repaired,&notrepaired);
   sscanf(strstr(ip->name,"->")+2,"%31[^-]",end);
   if (sp = strstr(end,": Out"))
      {
      *sp = '\0';
      }

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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);

METER_KEPT[meter_compliance_week] = av_week_kept;
METER_REPAIRED[meter_compliance_week] = av_week_repaired;
METER_KEPT[meter_compliance_day] = av_day_kept;
METER_REPAIRED[meter_compliance_day] = av_day_repaired;
METER_KEPT[meter_compliance_hour] = av_week_kept;
METER_REPAIRED[meter_compliance_hour] = av_hour_repaired;
}

/*****************************************************************************/

void Nova_GrandSummary()

{ char name[CF_BUFSIZE];
  FILE *fout;

SummarizeComms();

snprintf(name,CF_BUFSIZE-1,"%s/reports/comp_key",CFWORKDIR);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

fprintf(fout,"Week: %.4lf %.4lf\n",METER_KEPT[meter_compliance_week],METER_REPAIRED[meter_compliance_week]);
fprintf(fout,"Day: %.4lf %.4lf\n",METER_KEPT[meter_compliance_day],METER_REPAIRED[meter_compliance_day]);
fprintf(fout,"Hour: %.4lf %.4lf\n",METER_KEPT[meter_compliance_hour],METER_REPAIRED[meter_compliance_hour]);
fprintf(fout,"Patch: %.4lf %.4lf\n",METER_KEPT[meter_patch_day],METER_REPAIRED[meter_patch_day]);
fprintf(fout,"Soft: %.4lf %.4lf\n",METER_KEPT[meter_soft_day],METER_REPAIRED[meter_soft_day]);
fprintf(fout,"Comms: %.4lf %.4lf\n",METER_KEPT[meter_comms_hour],METER_REPAIRED[meter_comms_hour]);
fprintf(fout,"Anom: %.4lf %.4lf\n",METER_KEPT[meter_anomalies_day],METER_REPAIRED[meter_anomalies_day]);

fclose(fout);
}

/*****************************************************************************/

void Nova_SummarizePerPromiseCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE];
  int i = 0,ksize,vsize;
  CF_DB *dbp;
  CF_DBC *dbcp;
  char *key;
  void *stored;
  struct Event entry,e,newe;
  double lsea = CF_WEEK * 52; /* expire after a year */

/* Open the db */

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
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

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   dbp->close(dbp,0);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promise compliance history on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (stored != NULL)
      {
      memcpy(&entry,stored,sizeof(entry));

      then    = entry.t;
      measure = entry.Q.q;
      av = entry.Q.expect;
      var = entry.Q.var;
      lastseen = now - then;

      snprintf(tbuf,CF_BUFSIZE-1,"%s",ctime(&then));

      tbuf[cf_strlen(tbuf)-9] = '\0';                     /* Chop off second and year */

      if (lastseen > lsea)
         {
         Debug("Promise usage record %s expired\n",eventname);
         DeleteDB(dbp,eventname);
         }
      else
         {
         if (xml)
            {
            fprintf(fout,"%s",NRX[cfx_entry][cfb]);
            fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],tbuf,NRX[cfx_date][cfe]);
            fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],eventname,NRX[cfx_event][cfe]);

            if (measure = 1.0)
               {
               fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"compliant",NRX[cfx_q][cfe]);
               }
            else if (measure = 0.5)
               {
               fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"repaired",NRX[cfx_q][cfe]);
               }
            else if (measure = 0.0)
               {
               fprintf(fout,"%s %s %s",NRX[cfx_q][cfb],"non-compliant",NRX[cfx_q][cfe]);
               }

            fprintf(fout,"%s %.1lf %s",NRX[cfx_av][cfb],av*100.0,NRX[cfx_av][cfe]);
            fprintf(fout,"%s %.1lf %s",NRX[cfx_dev][cfb],sqrt(var)*100.0,NRX[cfx_dev][cfe]);

            fprintf(fout,"%s",NRX[cfx_entry][cfe]);
            }
         else if (html)
            {
            fprintf(fout,"%s",NRH[cfx_entry][cfb]);
            fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],tbuf,NRH[cfx_date][cfe]);
            fprintf(fout,"%s <a href=\"promise_output_common.html#%s\">%s</a> %s",NRH[cfx_event][cfb],eventname,eventname,NRH[cfx_event][cfe]);

            if (measure = 1.0)
               {
               fprintf(fout,"%s %s %s",NRH[cfx_q][cfb],"compliant",NRH[cfx_q][cfe]);
               }
            else if (measure = 0.5)
               {
               fprintf(fout,"%s %s %s",NRH[cfx_q][cfb],"repaired",NRH[cfx_q][cfe]);
               }
            else if (measure = 0.0)
               {
               fprintf(fout,"%s %s %s",NRH[cfx_q][cfb],"non-compliant",NRH[cfx_q][cfe]);
               }

            fprintf(fout,"%s %.1lf %s",NRH[cfx_av][cfb],av*100.0,NRH[cfx_av][cfe]);
            fprintf(fout,"%s %.1lf %s",NRH[cfx_dev][cfb],sqrt(var)*100.0,NRH[cfx_dev][cfe]);

            fprintf(fout,"%s",NRH[cfx_entry][cfe]);
            }
         else
            {
            if (measure = 1.0)
               {
               fprintf(fout,"%s,%s,compliant,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
               }
            else if (measure = 0.5)
               {
               fprintf(fout,"%s,%s,repaired,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
               }
            else if (measure = 0.0)
               {
               fprintf(fout,"%s,%,non-compliant,%.1lf,%.1lf",tbuf,eventname,av*100.0,sqrt(var)*100.0);
               }

            }
         }
      }
   }

DeleteDBCursor(dbp,dbcp);
CloseDB(dbp);

if (html && !embed)
   {
   fprintf(fout,"</table></div>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
}

/*****************************************************************************/

void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];

snprintf(name,CF_BUFSIZE,"%s/cfagent.%s.log",CFWORKDIR,VSYSNAME.nodename);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Known setuid programs on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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
   fprintf(fout,"</table></div>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE],datestr[CF_MAXVARSIZE],size[CF_MAXVARSIZE];
  char no[CF_SMALLBUF],change[CF_BUFSIZE],reformat[CF_BUFSIZE],output[2*CF_BUFSIZE],aggregate[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char pm,start[32];
  int i = 0,truncate;

snprintf(name,CF_BUFSIZE-1,"%s/state/file_hash_event_history",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"File hash-change events recorded on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time of change event",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Filename",NRH[cfx_filename][cfe]);
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

   sscanf(ip->name,"%31[^,],%250[^\n]",start,name);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],start,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],name,NRX[cfx_end][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],start,NRH[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],name,NRH[cfx_end][cfe]);
      fprintf(fout,"%s",NRH[cfx_entry][cfe]);
      }
   else if (csv)
      {
      fprintf(fout,"%s,%s",start,name);
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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);


/* Detail log */

file = NULL;

snprintf(name,CF_BUFSIZE-1,"%s/cfdiff.log",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

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

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"File difference events recorded on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizePromiseRepaired(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  char date[CF_MAXVARSIZE],handle[CF_MAXVARSIZE],bundle[CF_MAXVARSIZE],ref[CF_MAXVARSIZE],filename[CF_MAXVARSIZE],lineno[CF_MAXVARSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int i = 0;

  snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_REPAIR_LOG);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promises repaired on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_bundle][cfb],"Bundle",NRH[cfx_bundle][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Handle",NRH[cfx_event][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],"Comment",NRH[cfx_ref][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Filename",NRH[cfx_end][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_index][cfb],"Line no.",NRH[cfx_index][cfe]);
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

   date[0] = '\0';

   sscanf(ip->name,"%31[^,],%31[^,],%31[^,],%512[^,],%512[^,],%8s",date,bundle,handle,ref,filename,lineno);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],date,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_bundle][cfb],bundle,NRX[cfx_bundle][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],handle,NRX[cfx_event][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_ref][cfb],ref,NRX[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],filename,NRX[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_index][cfb],lineno,NRX[cfx_index][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],date,NRH[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_bundle][cfb],bundle,NRH[cfx_bundle][cfe]);
      fprintf(fout,"%s <a href=\"promise_output_common.html#%s\">%s %s",NRH[cfx_event][cfb],handle,handle,NRH[cfx_event][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],ref,NRH[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],filename,NRH[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_index][cfb],lineno,NRH[cfx_index][cfe]);
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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizePromiseNotKept(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  char date[CF_MAXVARSIZE],handle[CF_MAXVARSIZE],bundle[CF_MAXVARSIZE],ref[CF_MAXVARSIZE],filename[CF_MAXVARSIZE],lineno[CF_MAXVARSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int i = 0;

  snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_NOTKEPT_LOG);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s",name);
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promises not kept on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
   fprintf(fout,"<div id=\"reporttext\">\n");
   fprintf(fout,"<table class=border cellpadding=5>\n");
   fprintf(fout,"%s",NRH[cfx_entry][cfb]);
   fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],"Time",NRH[cfx_date][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_bundle][cfb],"Bundle",NRH[cfx_bundle][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],"Handle",NRH[cfx_event][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],"Comment",NRH[cfx_ref][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],"Filename",NRH[cfx_end][cfe]);
   fprintf(fout,"%s %s %s",NRH[cfx_index][cfb],"Line no.",NRH[cfx_index][cfe]);
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

   date[0] = '\0';

   sscanf(ip->name,"%31[^,],%31[^,],%31[^,],%512[^,],%512[^,],%8s",date,bundle,handle,ref,filename,lineno);

   if (xml)
      {
      fprintf(fout,"%s",NRX[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRX[cfx_date][cfb],date,NRX[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_bundle][cfb],bundle,NRX[cfx_bundle][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_event][cfb],handle,NRX[cfx_event][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_ref][cfb],ref,NRX[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_filename][cfb],filename,NRX[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRX[cfx_index][cfb],lineno,NRX[cfx_index][cfe]);
      fprintf(fout,"%s",NRX[cfx_entry][cfe]);
      }
   else if (html)
      {
      fprintf(fout,"%s",NRH[cfx_entry][cfb]);
      fprintf(fout,"%s %s %s",NRH[cfx_date][cfb],date,NRH[cfx_date][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_bundle][cfb],bundle,NRH[cfx_bundle][cfe]);
      fprintf(fout,"%s <a href=\"promise_output_common.html#%s\">%s</a> %s",NRH[cfx_event][cfb],handle,handle,NRH[cfx_event][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_ref][cfb],ref,NRH[cfx_ref][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_filename][cfb],filename,NRH[cfx_end][cfe]);
      fprintf(fout,"%s %s %s",NRH[cfx_index][cfb],lineno,NRH[cfx_index][cfe]);
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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_ReportSoftware(struct CfPackageManager *list)

{ FILE *fout;
  struct CfPackageManager *mp = NULL;
  struct CfPackageItem *pi;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int i = 0;

snprintf(name,CF_BUFSIZE,"%s/state/software_packages.csv",CFWORKDIR);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->pack_list; pi != NULL; pi=pi->next)
      {
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

cf_fclose(fout);
}

/*****************************************************************************/

void Nova_ReportPatches(struct CfPackageManager *list)

{ FILE *fout;
  struct CfPackageManager *mp = NULL;
  struct CfPackageItem *pi;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int i = 0,count = 0;

snprintf(name,CF_BUFSIZE,"%s/state/software_patch_status.csv",CFWORKDIR);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->patch_list; pi != NULL; pi=pi->next)
      {
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

cf_fclose(fout);

snprintf(name,CF_BUFSIZE,"%s/state/software_patches_avail.csv",CFWORKDIR);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
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

cf_fclose(fout);
}

/*****************************************************************************/

void Nova_SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/state/software_packages.csv",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"software_packages.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"software_packages.xml");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"software_packages.csv");
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Software versions installed %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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
   fprintf(fout,"</table></div>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);

METER_REPAIRED[meter_soft_day] = 0;
METER_KEPT[meter_soft_day] = 0;
}

/*****************************************************************************/

void Nova_SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0, count = 0;

CfOut(cf_verbose,"","Creating available patch report...\n");

snprintf(name,CF_BUFSIZE-1,"%s/state/software_patches_avail.csv",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"software_patches_avail.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"software_patches_avail.xml");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"software_patches_avail.csv");
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Software updates available and outstanding for %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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
   fprintf(fout,"</table></div>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);
file = NULL;

/* Now show installed patch level */

CfOut(cf_verbose,"","Creating patch status report...\n");

snprintf(name,CF_BUFSIZE-1,"%s/state/software_patch_status.csv",CFWORKDIR);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"software_patch_status.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"software_patch_status.xml");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"software_patch_status.csv");
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

cf_fclose(fin);

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Software patched installed on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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
   fprintf(fout,"</table></div>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

cf_fclose(fout);
DeleteItemList(file);

if (count > 1)
   {
   METER_KEPT[meter_patch_day] = 0;
   METER_REPAIRED[meter_patch_day] = 0;
   }
else
   {
   METER_KEPT[meter_patch_day] = 100.0;
   METER_REPAIRED[meter_patch_day] = 0;
   }

}

/*****************************************************************************/

void Nova_NoteVarUsage()

{ char name[CF_BUFSIZE];

if (MINUSF) /* Only do this for the default policy */
   {
   return;
   }

snprintf(name,CF_BUFSIZE-1,"%s/state/vars.out",CFWORKDIR);
MapName(name);

if ((FREPORT_HTML = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot write to %s",name);
   return;
   }

if ((FREPORT_TXT = fopen(NULLFILE,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot write to NULL-file");
   fclose(FREPORT_HTML);
   return;
   }

ShowScopedVariables();

fclose(FREPORT_HTML);
fclose(FREPORT_TXT);
}


/*****************************************************************************/

void Nova_SummarizeVariables(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];

snprintf(name,sizeof(name),"%s/state/vars.out",CFWORKDIR);
MapName(name);

if ((fin = cf_fopen(name,"r")) == NULL)
   {
   CfOut(cf_inform,"cf_fopen","Cannot variable notes %s (try again later)",name);
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

if ((fout = cf_fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"cf_fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,sizeof(name),"Main variables instantiated on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
   fprintf(fout,"<div id=\"reporttext\">\n");
   }

while (!feof(fin))
   {
   line[0] = '\0';
   fgets(line,CF_BUFSIZE-1,fin);
   fprintf(fout,"%s",line);
   }

cf_fclose(fin);

if (html && !embed)
   {
   fprintf(fout,"</div>\n");
   NovaHtmlFooter(fout,foot);
   }

cf_fclose(fout);
}

/*****************************************************************************/

void SummarizeComms()

/* Go through the database of recent connections and check for
   Long Time No See ...*/

{ CF_DB *dbp;
  CF_DBC *dbcp;
  int ret,ksize,vsize;
  struct QPoint entry;
  double kept = 1,not_kept = 0,repaired = 0,var,average;
  char name[CF_BUFSIZE], *key;
  void *value;
  time_t now = time(NULL),then;

snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_LASTDB_FILE);

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan class db");
   return;
   }

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   memset(&entry, 0, sizeof(entry));

   if (value != NULL)
      {
      memcpy(&entry,value,sizeof(entry));
      then = (time_t)entry.q;
      average = (double)entry.expect;
      var = (double)entry.var;
      }
   else
      {
      continue;
      }

   if (now - then > (int)(average+2.0*sqrt(var)+0.5))
      {
      not_kept++;
      }
   else if (now - then > (int)(average))
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
