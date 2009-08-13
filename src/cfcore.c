
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: cfcore.c                                                            */
/*                                                                           */
/* Created: Sun Oct 14 20:38:36 2007                                         */
/*                                                                           */
/*****************************************************************************/

void Nova_MainPage(char *host,struct Item *eliminate)
    
{ FILE *fout;

if ((fout = fopen("mainpage.html","w")) == NULL)
   {
   return;
   }

NovaHtmlHeader(fout,host,STYLESHEET,WEBDRIVER,BANNER);
Nova_ShowAllGraphs(fout,host,eliminate);
NovaHtmlFooter(fout,FOOTER);

fclose(fout);
}

/*****************************************************************************/

void Nova_OtherPages(char *host,struct Item *eliminate)
    
{ FILE *fout;
  char name[CF_BUFSIZE],exist[CF_BUFSIZE];
  char id[CF_BUFSIZE],desc[CF_BUFSIZE];
  struct stat s1,s2;
  int i;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   Nova_LookupAggregateClassName(i,id,desc);

   if (strcmp(id,"spare") == 0)
      {
      continue;
      }

   snprintf(exist,15,"%d",i);

   if (IsItemIn(eliminate,exist))
      {
      continue;
      }

   snprintf(name,CF_BUFSIZE,"%s.html",id);
   snprintf(exist,CF_BUFSIZE,"%s.mag",id);

   if (stat(name,&s1) != -1 && stat(exist,&s2) != -1)
      {
      if (s1.st_mtime > s2.st_mtime)
         {
         return;
         }
      }
   
   if ((fout = fopen(name,"w")) == NULL)
      {
      return;
      }

   NovaHtmlHeader(fout,host,STYLESHEET,WEBDRIVER,BANNER);
   Nova_ShowGraph(fout,i,s2.st_mtime,(enum observables)i);
   NovaHtmlFooter(fout,FOOTER);

   fclose(fout);
   }
}

/*****************************************************************************/

void Nova_ShowAllGraphs(FILE *fp,char *host,struct Item *eliminate)

{ int i,terminated;
  struct stat sb;
  char img[CF_BUFSIZE];
  char url[CF_BUFSIZE];
  char hist[CF_BUFSIZE];
  char mag[CF_BUFSIZE];
  char week[CF_BUFSIZE];
  char datestr[CF_MAXVARSIZE];
  char name[CF_BUFSIZE],description[CF_BUFSIZE];
  
fprintf(fp,"<table border=1>");
 
for (i = 0; i < CF_OBSERVABLES; i++)
    {
    Nova_LookupAggregateClassName(i,name,description);

    if (strcmp(name,"spare") == 0)
       {
       continue;
       }

    snprintf(hist,15,"%d",i);

    snprintf(datestr,CF_MAXVARSIZE,"%s",ctime(&DATESTAMPS[i]));
    Chop(datestr);

    /* Check is data stream has stopped for this service */
    
    if (IsItemIn(eliminate,hist))
       {
       continue;
       }

    /* Check current data stream */
    
    if (i > ob_spare)
       {
       snprintf(description,CF_BUFSIZE-1,"measurement %s",name);
       }

    fprintf(fp,"<tr>");

    snprintf(img,CF_BUFSIZE,"%s",description);
    snprintf(url,CF_BUFSIZE,"%s.html",name);
    snprintf(hist,CF_BUFSIZE,"%s_hist.html",name);
    snprintf(mag,CF_BUFSIZE,"%s_mag.html",name);
    snprintf(week,CF_BUFSIZE,"%s_week.html",name);

    if (stat(url,&sb) == -1)
       {
       continue;
       }    
    
    fprintf(fp,"<th nowrap><font color=\"#888888\">%s</font><br><br><a href=\"%s\">%s</a><br><small>Latest data<br>%s</small></th>\n",host,url,img,datestr);

    terminated = false;
    
    snprintf(img,CF_BUFSIZE,"%s_mag.png",name);

    if (stat(img,&sb) == -1)
       {
       terminated = true;
       }

    snprintf(img,CF_BUFSIZE,"%s_weekly.png",name);
        
    if (stat(img,&sb) == -1)
       {
       terminated = true;
       }

    snprintf(img,CF_BUFSIZE,"%s_hist.png",name);
        
    if (stat(img,&sb) == -1)
       {
       terminated = true;
       }
    
    if (!terminated)
       {
       snprintf(img,CF_BUFSIZE,"%s_mag.png",name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",mag,img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated)
       {
       snprintf(img,CF_BUFSIZE,"%s_weekly.png",name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",week,img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated)
       {
       snprintf(img,CF_BUFSIZE,"%s_hist.png",name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",hist,img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Insufficient data</center></td>\n");
       }
    
    fprintf(fp,"</tr>");
    }

fprintf(fp,"</table>");
}

/*****************************************************************************/

void Nova_ShowGraph(FILE *fout,int i,time_t date,enum observables obs)

{ char name1[CF_BUFSIZE],name2[CF_BUFSIZE],img[CF_BUFSIZE],datestr[CF_BUFSIZE];
  char name[CF_BUFSIZE],description[CF_BUFSIZE];
  double x1,y1,z1,x2,y2,z2;
  FILE *fp1,*fp2;

Nova_LookupAggregateClassName(i,name,description);
  
snprintf(name1,CF_BUFSIZE-1,"%s.mag",name);
snprintf(img,CF_BUFSIZE-1,"%s_mag.png",name);

snprintf(datestr,CF_MAXVARSIZE,"%s",ctime(&date));
Chop(datestr);

fprintf(fout,"<h1>%s</h1>\n",description);
fprintf(fout,"<h2>Last 4 hours</h2>\n");

fprintf(fout,"<h4>Latest info observed %s</h4>\n",datestr);

fprintf(fout,"<div id=\"occurrences\">\n");

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"500\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");


fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   return;
   }

while (!feof(fp1))
   {
   fscanf(fp1,"%lf %lf %lf %lf",&x1,&y1,&z1,&y2);
   
   fprintf(fout,"<tr><td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td></tr>\n",x1,y2,y1,z1);
   }

fprintf(fout,"</table></p>\n");
fclose(fp1);

/* Averages */

fprintf(fout,"<h2>Past and previous weeks</h2>\n");

snprintf(name1,CF_BUFSIZE-1,"%s.E-sigma",name);
snprintf(name2,CF_BUFSIZE-1,"%s.q",name);
snprintf(img,CF_BUFSIZE-1,"%s_weekly.png",name);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"500\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   return;
   }

if ((fp2 = fopen(name2,"r")) == NULL)
   {
   return;
   }

while (!feof(fp1))
   {
   fscanf(fp1,"%lf %lf %lf",&x1,&y1,&z1);
   fscanf(fp2,"%lf %lf %lf",&x2,&y2,&z2);
   
   fprintf(fout,"<tr><td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td></tr>\n",x1,y2,y1,z1);
   }

fprintf(fout,"</table></p>\n");
fprintf(fout,"</div>\n");
fclose(fp1);
fclose(fp2);
}

