
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
    NULL,NULL
   };

/*****************************************************************************/

void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

/* /var/cfengine/promise.log */

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32],end[32];
  char version[CF_MAXVARSIZE];
  int kept,repaired,notrepaired;
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/promise.log",CFWORKDIR);
 
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
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (strlen(ip->name) == 0)
      {
      continue;
      }
   
   sscanf(ip->name,"%31[^-]->%31[^O]Outcome of version %250[^:]: Promises observed to be kept %d%*[^0-9]%d%*[^0-9]%d",start,end,version,&kept,&repaired,&notrepaired);
   
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
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizePerPromiseCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE];
  char start[32],end[32];
  char version[CF_MAXVARSIZE];
  int i = 0;
  DB *dbp;
  DBC *dbcp;
  DBT key,stored;
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

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   dbp->close(dbp,0);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Promise compliance history on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

if ((errno = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0)
   {
   dbp->err(dbp,errno,"DB->cursor");
   dbp->close(dbp,0);
   return;
   }

/* Initialize the key/data return pair. */
 
memset(&key, 0, sizeof(key));
memset(&stored, 0, sizeof(stored));

while (dbcp->c_get(dbcp,&key,&stored,DB_NEXT) == 0)
   {
   double measure,av,var;
   time_t then,lastseen,now = time(NULL);
   char tbuf[CF_BUFSIZE],eventname[CF_BUFSIZE];

   strcpy(eventname,(char *)key.data);

   if (stored.data != NULL)
      {
      memcpy(&entry,stored.data,sizeof(entry));
      
      then    = entry.t;
      measure = entry.Q.q;
      av = entry.Q.expect;
      var = entry.Q.var;
      lastseen = now - then;
            
      snprintf(tbuf,CF_BUFSIZE-1,"%s",ctime(&then));

      tbuf[strlen(tbuf)-9] = '\0';                     /* Chop off second and year */

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
            fprintf(fout,"%s %s %s",NRH[cfx_event][cfb],eventname,NRH[cfx_event][cfe]);

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

dbp->close(dbp,0);

if (html && !embed)
   {
   fprintf(fout,"</table>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
}

/*****************************************************************************/

void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];

snprintf(name,CF_BUFSIZE,"%s/cfagent.%s.log",CFWORKDIR,VSYSNAME.nodename);
 
if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the source log %s",name);
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
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (strlen(ip->name) == 0)
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
   fprintf(fout,"</table>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_BUFSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  char start[32];
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/state/file_hash_event_history",CFWORKDIR);
 
if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the source log %s",name);
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
   snprintf(name,CF_BUFSIZE,"File hash-change events recorded on %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (strlen(ip->name) == 0)
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
   fprintf(fout,"</table>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
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

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->pack_list; pi != NULL; pi=pi->next)
      {
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

fclose(fout);

snprintf(name,CF_BUFSIZE,"%s/state/software_updates.csv",CFWORKDIR);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

for (mp = list; mp != NULL; mp = mp->next)
   {
   for (pi = mp->update_list; pi != NULL; pi=pi->next)
      {
      fprintf(fout,"%s,%s,%s,%s\n",pi->name,pi->version,pi->arch,ReadLastNode(GetArg0(mp->manager)));
      }
   }

fclose(fout);
}


/*****************************************************************************/

void Nova_SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/state/software_packages.csv",CFWORKDIR);
 
if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
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

fclose(fin);

if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the destination file %s",name);
   return;
   }

if (html && !embed)
   {
   snprintf(name,CF_BUFSIZE,"Software versions installed %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (strlen(ip->name) == 0)
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
   fprintf(fout,"</table>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}

/*****************************************************************************/

void Nova_SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web)

{ FILE *fin,*fout;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch[CF_MAXVARSIZE],mgr[CF_MAXVARSIZE],line[CF_BUFSIZE];
  struct Item *ip,*file = NULL;
  int i = 0;

snprintf(name,CF_BUFSIZE-1,"%s/state/software_updates.csv",CFWORKDIR);
 
if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",name);
   return;
   }

if (html)
   {
   snprintf(name,CF_BUFSIZE,"software_updates.html");
   }
else if (xml)
   {
   snprintf(name,CF_BUFSIZE,"software_updates.xml");
   }
else
   {
   snprintf(name,CF_BUFSIZE,"software_updates.csv");
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
   snprintf(name,CF_BUFSIZE,"Software updates available and outstanding for %s",VFQNAME);
   NovaHtmlHeader(fout,name,stylesheet,web,head);
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

   if (strlen(ip->name) == 0)
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
   fprintf(fout,"</table>");
   NovaHtmlFooter(fout,foot);
   }

if (XML)
   {
   fprintf(fout,"</output>\n");
   }

fclose(fout);
DeleteItemList(file);
}
