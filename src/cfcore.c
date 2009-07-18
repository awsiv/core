
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


/*****************************************************************************/

void Nova_MainPage(char *host,struct Item *eliminate)
    
{ FILE *fout;

mkdir("html",0755);
    
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
  int i;
  struct stat s1,s2;

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (strcmp(OBS[i][0],"spare") == 0)
      {
      continue;
      }

   snprintf(exist,15,"%d",i);

   if (IsItemIn(eliminate,exist))
      {
      continue;
      }

   snprintf(name,CF_BUFSIZE,"%s.html",OBS[i][0]);
   snprintf(exist,CF_BUFSIZE,"%s.mag",OBS[i][0]);

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

{ int i;
  char img[CF_BUFSIZE];
  char url[CF_BUFSIZE];
  char hist[CF_BUFSIZE];
  char mag[CF_BUFSIZE];
  char week[CF_BUFSIZE];
  char datestr[CF_MAXVARSIZE];

fprintf(fp,"<table border=1>");
 
for (i = 0; i < CF_OBSERVABLES; i++)
    {
    if (strcmp(OBS[i][0],"spare") == 0)
       {
       break;
       }
    
    snprintf(hist,15,"%d",i);
    
    if (IsItemIn(eliminate,hist))
       {
       continue;
       }

    snprintf(datestr,CF_MAXVARSIZE,"%s",ctime(&DATESTAMPS[i]));
    Chop(datestr);

    fprintf(fp,"<tr>");

    snprintf(img,CF_BUFSIZE,"%s",OBS[i][1]);
    snprintf(url,CF_BUFSIZE,"%s.html",OBS[i][0]);
    snprintf(hist,CF_BUFSIZE,"%s_hist.html",OBS[i][0]);
    snprintf(mag,CF_BUFSIZE,"%s_mag.html",OBS[i][0]);
    snprintf(week,CF_BUFSIZE,"%s_week.html",OBS[i][0]);
    
    fprintf(fp,"<th nowrap><font color=\"#888888\">%s</font><br><br><a href=\"%s\">%s</a><br><small>Latest data<br>%s</small></th>\n",host,url,img,datestr);

    snprintf(img,CF_BUFSIZE,"%s_mag.png",OBS[i][0]);
    fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",mag,img);

    snprintf(img,CF_BUFSIZE,"%s_weekly.png",OBS[i][0]);
    fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",week,img);

    snprintf(img,CF_BUFSIZE,"%s_hist.png",OBS[i][0]);
    fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",hist,img);

    fprintf(fp,"</tr>");
    }

fprintf(fp,"</table>");
}

/*****************************************************************************/

void Nova_ShowGraph(FILE *fout,int i,time_t date,enum observables obs)

{ char name1[CF_BUFSIZE],name2[CF_BUFSIZE],img[CF_BUFSIZE],datestr[CF_BUFSIZE];
  double x1,y1,z1,x2,y2,z2;
  FILE *fp1,*fp2;

snprintf(name1,CF_BUFSIZE-1,"%s.mag",OBS[i][0]);
snprintf(img,CF_BUFSIZE-1,"%s_mag.png",OBS[i][0]);

snprintf(datestr,CF_MAXVARSIZE,"%s",ctime(&date));
Chop(datestr);

fprintf(fout,"<h1>%s</h1>\n",OBS[i][1]);
fprintf(fout,"<h2>Last 4 hours</h2>\n");

fprintf(fout,"<h4>Latest info observed %s</h4>\n",datestr);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");


fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   printf("ops show1 %s name1\n",name1);
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

snprintf(name1,CF_BUFSIZE-1,"%s.E-sigma",OBS[i][0]);
snprintf(name2,CF_BUFSIZE-1,"%s.q",OBS[i][0]);
snprintf(img,CF_BUFSIZE-1,"%s_weekly.png",OBS[i][0]);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\"></a></p>\n",img,img);
fprintf(fout,"<p><table border=1>\n");

fprintf(fout,"<tr><th bgcolor=#eeeeee>Time</th>\n<th bgcolor=#eeeeee>q</th>\n<th bgcolor=#eeeeee>E(q)</th>\n<th bgcolor=#eeeeee>delta q</tdh></tr>\n");
   
if ((fp1 = fopen(name1,"r")) == NULL)
   {
   printf("ops show1a %s name1\n",name1);
   return;
   }

if ((fp2 = fopen(name2,"r")) == NULL)
   {
   printf("ops show2 %s name2\n",name2);
   return;
   }

while (!feof(fp1))
   {
   fscanf(fp1,"%lf %lf %lf",&x1,&y1,&z1);
   fscanf(fp2,"%lf %lf %lf",&x2,&y2,&z2);
   
   fprintf(fout,"<tr><td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td>\n<td bgcolor=#eeeeee>%f</td></tr>\n",x1,y2,y1,z1);
   }

fprintf(fout,"</table></p>\n");
fclose(fp1);
fclose(fp2);
}

