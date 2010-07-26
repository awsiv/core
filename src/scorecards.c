
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

void Nova_MainPage(char *host,struct Item *eliminate)
    
{ FILE *fout;

if (LICENSES == 0)
   {
   return;
   }
 
if ((fout = fopen("mainpage.html","w")) == NULL)
   {
   return;
   }

Nova_ShowAllGraphs(fout,host,eliminate);

fclose(fout);
}

/*****************************************************************************/

void Nova_OtherPages(char *host,struct Item *eliminate)
    
{ FILE *fout;
  char name[CF_BUFSIZE],exist[CF_BUFSIZE];
  char id[CF_BUFSIZE],desc[CF_BUFSIZE];
  struct stat s1,s2;
  int i;

if (LICENSES == 0)
   {
   return;
   }
  
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

   if (cfstat(name,&s1) != -1 && cfstat(exist,&s2) != -1)
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

   Nova_ShowGraph(fout,host,i,s2.st_mtime,(enum observables)i);
   fclose(fout);
   }
}

/*****************************************************************************/

void Nova_ShowAllGraphs(FILE *fp,char *keyhash,struct Item *eliminate)

{ int terminated1,terminated2,terminated3;
  struct stat sb;
  char img[CF_BUFSIZE];
  char url[CF_BUFSIZE];
  char hist[CF_BUFSIZE];
  char mag[CF_BUFSIZE];
  char week[CF_BUFSIZE];
  char datestr[CF_MAXVARSIZE];
  char top_name[CF_BUFSIZE],top_description[CF_BUFSIZE];
  enum observables obs;
  
fprintf(fp,"<table>");
 
for (obs = 0; obs < CF_OBSERVABLES; obs++)
    {
    Nova_LookupAggregateClassName(obs,top_name,top_description);

    if (strcmp(top_name,"spare") == 0)
       {
       continue;
       }

    snprintf(hist,15,"%d",obs);

    snprintf(datestr,CF_MAXVARSIZE,"%s",cf_ctime(&DATESTAMPS[obs]));
    Chop(datestr);

    /* Check is data stream has stopped for this service */
    
    if (IsItemIn(eliminate,hist))
       {
       continue;
       }

    /* Check current data stream */
    
    if (obs > ob_spare)
       {
       snprintf(top_description,CF_BUFSIZE-1,"measurement %s",top_name);
       }

    fprintf(fp,"<tr>");

    snprintf(img,CF_BUFSIZE,"%s",top_description);
    snprintf(url,CF_BUFSIZE,"%s.html",top_name);
    snprintf(hist,CF_BUFSIZE,"%s_hist.html",top_name);
    snprintf(mag,CF_BUFSIZE,"%s_mag.html",top_name);
    snprintf(week,CF_BUFSIZE,"%s_week.html",top_name);

    if (cfstat(url,&sb) == -1)
       {
       continue;
       }    

    snprintf(img,CF_BUFSIZE,"%s_weekly.png",top_name);
    
    if (cfstat(img,&sb) == -1)
       {
       terminated2 = true;
       continue;
       }

    snprintf(url,CF_BUFSIZE,"%s/hub/%s/%s/%s.html",DOCROOT,keyhash,obs);
    
    fprintf(fp,"<th nowrap><div id=\"ip\">%s</div><br><br><a href=\"%s\">%s</a><br><br><small>Latest data<br>%s</small></th>\n",keyhash,URLControl("%s",url),top_name,datestr);

    terminated1 = terminated2 = terminated3  = false;
    
    snprintf(img,CF_BUFSIZE,"%s_mag.png",top_name);

    if (cfstat(img,&sb) == -1)
       {
       terminated1 = true;
       }

    snprintf(img,CF_BUFSIZE,"%s_hist.png",top_name);
        
    if (cfstat(img,&sb) == -1)
       {
       terminated3 = true;
       }
    
    if (!terminated1)
       {
       snprintf(mag,CF_BUFSIZE,"%s/hub/%s/%s_mag.html",DOCROOT,keyhash,top_name);
       snprintf(img,CF_BUFSIZE,"%s/hub/%s/%s_mag.png",DOCROOT,keyhash,top_name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=\"300\"></a></td>\n",URLControl("%s",mag),img);
       }
    else
       {
       fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated2)
       {
       snprintf(week,CF_BUFSIZE,"%s/hub/%s/%s_week.html",DOCROOT,keyhash,top_name);
//       snprintf(img,CF_BUFSIZE,"reports/%s/%s_weekly.png",keyhash,top_name);
//       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",URLControl("%s",week),img);
       }
    else
       {
       continue;
       //fprintf(fp,"<td bgcolor=red><center>Data stream terminated</center></td>\n");
       }

    if (!terminated3)
       {
       snprintf(hist,CF_BUFSIZE,"reports/%s/%s_hist.html",keyhash,top_name);
       snprintf(img,CF_BUFSIZE,"reports/%s/%s_hist.png",keyhash,top_name);
       fprintf(fp,"<td><a href=\"%s\"><img src=\"%s\" width=300></a></td>\n",URLControl("%s",hist),img);
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

void Nova_ShowGraph(FILE *fout,char *host,int i,time_t date,enum observables obs)

{ char name1[CF_BUFSIZE],name2[CF_BUFSIZE],img[CF_BUFSIZE],datestr[CF_BUFSIZE];
  char name[CF_BUFSIZE],description[CF_BUFSIZE];
  double x1,y1,z1,x2,y2,z2;
  FILE *fp1,*fp2;

Nova_LookupAggregateClassName(i,name,description);
  
snprintf(name1,CF_BUFSIZE-1,"%s.mag",name);
snprintf(img,CF_BUFSIZE-1,"reports/%s/%s_mag.png",host,name);

snprintf(datestr,CF_MAXVARSIZE,"%s",cf_ctime(&date));
Chop(datestr);

fprintf(fout,"<h4>%s</h4>\n",description);

fprintf(fout,"<div id=\"legend\">\n");

fprintf(fout,"<h4>Last 3 years</h4>\n");

snprintf(name2,CF_MAXVARSIZE-1,"reports/%s/%s_yr.html",host,name);
fprintf(fout,"<p><a href=\"%s\">Long history</a> provides a rough trend over the past 3 years\n",URLControl("%s",name2));

fprintf(fout,"<h2>Last 4 hours</h2>\n");

fprintf(fout,"<h4>Latest info observed %s</h4>\n",datestr);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"590\"></a></p>\n",img,img);
fprintf(fout,"<p><table>\n");


fprintf(fout,"<p><table>\n");

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
snprintf(img,CF_BUFSIZE-1,"reports/%s/%s_weekly.png",host,name);

fprintf(fout,"<p><a href=\"%s\"><img src=\"%s\" width=\"590\"></a></p>\n",img,img);
fprintf(fout,"<p><table>\n");

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
/*******************************************************************/

void Nova_IncludeFile(FILE *fout,char *name)

{ FILE *fin;
  char buf[CF_BUFSIZE];

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Can't open %s for reading\n",name);
   return;
   }

while(!feof(fin))
   {
   buf[0] = '\0';
   fgets(buf,CF_BUFSIZE-1,fin);
   fprintf(fout,"%s",buf);
   }

fclose(fin);
}


/*****************************************************************************/

void Nova_BuildMainMeter(struct CfDataView *cfv,struct Item *list)

{ FILE *fout;
  char filename[CF_BUFSIZE];
  int i,kept[8],repaired[8];
  static char *names[8] = { "zzz", "Week", "Day", "Hour", "Patch", "Lics", "Coms","Anom" };

cfv->height = CF_METER_HEIGHT;
cfv->width = CF_METER_WIDTH;
cfv->margin = CF_METER_MARGIN;
cfv->title = "System state";
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

// Draw the box

gdImageFilledRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,LIGHTGREY);
gdImageSetThickness(cfv->im,2);
gdImageRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,BLACK);

Nova_GetAllLevels(kept,repaired,list,names);

for (i = 1; i < 8; i++)
   {
   Nova_BarMeter(cfv,i,kept[i],repaired[i],names[i]);
   }

snprintf(filename,CF_BUFSIZE,"meters.png");

if ((fout = fopen(filename, "wb")) == NULL)
   {
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",filename);
   }
    
gdImagePng(cfv->im,fout);
fclose(fout);
gdImageDestroy(cfv->im);
}

/*****************************************************************************/

int Nova_BuildMeter(struct CfDataView *cfv,char *hostkey)

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
  
cfv->height = CF_METER_HEIGHT;
cfv->width = CF_METER_WIDTH;
cfv->margin = CF_METER_MARGIN;
cfv->title = "System state";
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

gdImageSetThickness(cfv->im,2);
gdImageFilledRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,LIGHTGREY);
gdImageRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,BLACK);

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
          Nova_BarMeter(cfv,1,kept,repaired,"Week");
          break;
      case cfmeter_hour:
          Nova_BarMeter(cfv,3,kept,repaired,"Hour");
          returnval = kept+repaired;
          break;          
      case cfmeter_day:
          Nova_BarMeter(cfv,2,kept,repaired,"Day");
          break;
      case cfmeter_perf:
          Nova_BarMeter(cfv,4,kept,repaired,"Perf");
          break;
      case cfmeter_comms:
          Nova_BarMeter(cfv,6,kept,repaired,"Coms");
          break;
      case cfmeter_anomaly:
          Nova_BarMeter(cfv,7,kept,repaired,"Anom");
          break;
      case cfmeter_other:
          Nova_BarMeter(cfv,5,kept,repaired,"Lics");
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
    
gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);
return returnval;
}

/*****************************************************************************/

struct Item *Nova_CreateHostPortal(struct Item *list)

{ FILE *fout,*fall;
  char filename[CF_BUFSIZE],col[CF_BUFSIZE];
  char url1[CF_MAXVARSIZE],url2[CF_MAXVARSIZE],url3[CF_MAXVARSIZE];
  struct Item *ip;
  struct stat sb;
  time_t now = time(NULL);
  char *retval,rettype;
  int state,count = 0, licenses = 1,ccount[3];
  int format_width = 8;
  
snprintf(filename,CF_BUFSIZE,"host_portal.html");

if ((fout = fopen(filename, "w")) == NULL)
   {
   return list;
   }

//CfHtmlTitle(fout,"20 Weakest Host Symptoms");
CfHtmlTitle(fout,"20 Least compliant hosts");

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   CfOut(cf_inform,""," -> %d paid licenses have been asserted (this is a promise by you)",licenses);
   }

for (ip = list; ip != NULL; ip=ip->next)
   {
   count++;
   ip->counter = -1;
   
   if (!Nova_CountHostIssues(ip))
      {
      //continue;
      }

   snprintf(filename,CF_BUFSIZE-1,"%s/fluctuations.nov",ip->name);

   if (cfstat(filename,&sb) != -1)
      {
      ip->time = sb.st_mtime;
      
      if (now > sb.st_mtime + 3600 || ip->counter > CF_RED_THRESHOLD)
         {
         ip->counter = CF_RED_THRESHOLD + 1;

         if (ip->counter > CF_RED_THRESHOLD)
            {
            ip->classes = strdup("Compliance issues");
            }
         else
            {
            ip->classes = strdup("Problems for past hour");
            }
         }
      else if (now > sb.st_mtime + 1800 || ip->counter > CF_AMBER_THRESHOLD)
         {
         ip->counter = CF_AMBER_THRESHOLD + 1;
         ip->classes = strdup("Problem for 30 mins");
         }
      else
         {
         ip->classes = strdup("ok");
         }
      }
   else
      {
      ip->time = 0; 
      ip->counter = CF_RED_THRESHOLD + 1;
      ip->classes = strdup("No data found!");
      }
   }

list = SortItemListCounters(list);

if (count > LICENSES)
   {   
   fprintf(fout,"<div id=\"warning\"><h4>You have promised that %d licenses have been paid for, but %d licenses have been granted by Cfengine, expiring %s <br>%d host(s) seem to be enrolled</h4></div>",licenses,LICENSES,EXPIRY,count);
   CfOut(cf_log,""," !! You have exceeded the number of paid licenses. You are in breach of your license agreement.");
   }
else
   {
   if (strlen(LICENSE_COMPANY) > 0)
      {
      fprintf(fout,"<div id=\"ok\"><h4>%s<br>%d hosts are known, sharing %d granted licenses that expire on %s, and you promise that you have paid for %d</h4></div>",LICENSE_COMPANY,count,LICENSES,EXPIRY,licenses);
      }
   else
      {
      fprintf(fout,"<div id=\"ok\"><h4>%d hosts are known, sharing %d granted licenses that expire on %s, and you promise that you have paid for %d</h4></div>",count,LICENSES,EXPIRY,licenses);
      }
   }

fprintf(fout,"\n<div id=\"directory\">\n<table>\n");

for (count = 0,ip = list; ip != NULL; ip=ip->next)
   {
   if (count++ > 20)
      {
      break;
      }
   
   if (ip->counter > CF_RED_THRESHOLD)
      {
      ip->counter = CF_RED_THRESHOLD + 1;
      strcpy(col,"red");
      }
   else if (ip->counter > CF_AMBER_THRESHOLD)
      {
      ip->counter = CF_AMBER_THRESHOLD + 1;
      strcpy(col,"yellow");
      }
   else if (ip->counter > 0)
      {
      strcpy(col,"green");
      }

   snprintf(filename,CF_BUFSIZE-1,"%s/fluctuations.nov",ip->name);

   if (cfstat(filename,&sb) == -1)
      {
      continue;
      }

   snprintf(url1,CF_MAXVARSIZE-1,"reports/%s/mainpage.html",ip->name);
   snprintf(url2,CF_MAXVARSIZE-1,"reports/%s/promise_output_common.html",ip->name);
   snprintf(url3,CF_MAXVARSIZE-1,"reports/%s/classes.html",ip->name);
   
   fprintf(fout,"<tr><td><img src=\"%s.png\"></td><td><div id=\"signal%s\"><center><p>%s<p>%s</center></div></td><td><center><p>Last updated at<p>%s</center></td>\n",
           col,
           col,
           ip->name,
           ip->classes,
           cf_ctime(&(ip->time)));
           
   fprintf(fout,"<td><a href=\"%s\"><img src=\"reports/%s/meters.png\"></a></td>\n",URLControl("%s",url1),ip->name);
   fprintf(fout,"<td><p><span id=\"rbuttons\"><a href=\"%s\">Promises</a>\n",URLControl("%s",url2));
   fprintf(fout,"<p><a href=\"%s\">Classes</a></span></td></tr>\n",URLControl("%s",url3));
   }

fprintf(fout,"</table></div>\n");
fclose(fout);

/* All hosts page */

snprintf(filename,CF_BUFSIZE,"allhosts.html");

if ((fall = fopen(filename, "w")) == NULL)
   {
   fclose(fout);
   return list;
   }

// Split here?

CfHtmlTitle(fall,"Status all managed hosts");

ccount[0] = 0;
ccount[1] = 0;
ccount[2] = 0;

for (ip = list; ip != NULL; ip=ip->next)
   {
   snprintf(filename,CF_BUFSIZE-1,"%s/meters.png",ip->name);

   if (cfstat(filename,&sb) != -1)
      {
      if (now > sb.st_mtime + 3600 || ip->counter > CF_RED_THRESHOLD)
         {
         ip->counter = 0;
         ccount[0]++;
         }
      else if (now > sb.st_mtime + 1800 || ip->counter > CF_AMBER_THRESHOLD)
         {
         ip->counter = 1;
         ccount[1]++;
         }
      else
         {
         ip->counter = 2;
         ccount[2]++;
         }
      }
   else
      {
      strcpy(col,"red");
      ip->counter = 0;
      }
   }

fprintf(fall,"<div id=\"allhosts\">\n<table>\n");

fprintf(fall,"<tr><td colspan=\"6\">Currently aware of %d licensed hosts</td></tr>\n",count);

count = 0;

for (state = 0; state < 3; state++)
   {
   fprintf(fall,"<tr>\n");

   if (state == 0 && ccount[0] == 0)
      {
      fprintf(fall,"<td><span id=\"signalred\"><a href=\"%s\">(none)</a></span></td> \n",URLControl("%s","reports/host_portal.html"));
      ccount[0]++;
      continue;
      }

   if (state == 1 && ccount[1] == 0)
      {
      ccount[1]++;
      fprintf(fall,"<td colspan=\"6\"><span id=\"signalyellow\"><a href=\"%s\">(none)</a></span></td> \n",URLControl("%s","reports/host_portal.html"));
      continue;
      }

   for (ip = list; ip != NULL; ip=ip->next)
      {
      if (ip->counter == state)
         {
         snprintf(url1,CF_MAXVARSIZE-1,"reports/%s/mainpage.html",ip->name);
         
         switch (state)
            {
            case 0:
                fprintf(fall,"<td><span id=\"signalred\"><img src=\"red.png\"></span> <p>%s</p> <a href=\"%s\">Check</a></td> \n",ip->name,URLControl("%s",url1));
                count++;
                break;
            case 1:
                fprintf(fall,"<td><span id=\"signalyellow\"><img src=\"yellow.png\"></span> <p>%s</p> <a href=\"%s\">Check</a></td> \n",ip->name,URLControl("%s",url1));
                count++;
                break;
            case 2:
                fprintf(fall,"<td><span id=\"smallgreen\"><img src=\"green.png\"></span> <p>%s</p> <a href=\"%s\">Check</a></td> \n",ip->name,URLControl("%s",url1));
                count++;
                break;
            }

         if (count == 6)
            {
            fprintf(fall,"</tr><tr>\n");
            }         
         }
      }
   
   fprintf(fall,"</tr>\n");
   }

fprintf(fall,"</table></div>\n");
fclose(fall);
return list;
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

void Nova_GetAllLevels(int *kept,int *repaired,struct Item *list,char **names)

{ FILE *fin;
  char buf[CF_BUFSIZE];
  double a = 0, b=0,count = 0,aa[8],bb[8];
  struct Item *ip;
  char *retval,rettype;
  int i,licenses = 1;

for (i = 0; i < 8; i++)
   {
   aa[i] = 0;
   bb[i] = 0;
   }

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   CfOut(cf_inform,""," -> %d paid licenses have been asserted (this is a promise by you)",licenses);
   }

if (licenses == 0)
   {
   licenses = 1;
   }

for (ip = list; ip != NULL; ip=ip->next)
   {
   snprintf(buf,CF_BUFSIZE-1,"%s/comp_key",ip->name);

   count++;

   if ((fin = fopen(buf,"r")) == NULL)
      {
      continue;
      }

   while(!feof(fin))
      {
      a = 0;
      b = 0;
      buf[0] = '\0';
      fgets(buf,CF_BUFSIZE-1,fin);

      sscanf(buf,"%*s %lf %lf",&a,&b);

      for (i = 0; i < 8; i++)
         {
         if (strncmp(buf,names[i],strlen(names[i])) == 0)
            {
            aa[i] += a;
            bb[i] += b;
            }            
         }
      }
   
   fclose(fin);
   }

for (i = 0; i < 8; i++)
   {
   /* Special exception for counting the licenses */
   if (strcmp(names[i],"Lics") == 0)
      {
      if (count > licenses)
         {
         kept[i] = licenses/count * 100.0;
         repaired[i] = 0;
         }
      else
         {
         kept[i] = count/licenses * 100.0;
         repaired[i] = (licenses-count)/licenses * 100.0;         
         }
      }
   else
      {
      kept[i] = (int)(aa[i]/count+0.5);
      repaired[i] = (int)(bb[i]/count+0.5);
      }
   }
}

/*****************************************************************************/

void Nova_GetLevels(int *kept,int *repaired,char *hostname,char **names)

{ FILE *fin;
  char buf[CF_BUFSIZE];
  double a = 0, b = 0,count = 1.0,aa[8],bb[8];
  struct Item *ip;
  char *retval,rettype;
  int i,licenses = 1;

for (i = 0; i < 8; i++)
   {
   aa[i] = 0.0;
   bb[i] = 0.0;
   }

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   CfOut(cf_inform,""," -> %d paid licenses have been asserted (this is a promise by you)",licenses);
   }

if (licenses == 0)
   {
   licenses = 1;
   }

snprintf(buf,CF_BUFSIZE-1,"%s/comp_key",hostname);

if ((fin = fopen(buf,"r")) == NULL)
   {
   return;
   }

while(!feof(fin))
   {
   a = 0.0;
   b = 0.0;
   buf[0] = '\0';
   fgets(buf,CF_BUFSIZE-1,fin);
   
   sscanf(buf,"%*s %lf %lf",&a,&b);
   
   for (i = 0; i < 8; i++)
      {
      if (strncmp(buf,names[i],strlen(names[i])) == 0)
         {
         aa[i] += a;
         bb[i] += b;
         }            
      }
   }

fclose(fin);

for (i = 0; i < 8; i++)
   {
   /* Special exception for counting the licenses */
   if (strcmp(names[i],"Lics") == 0)
      {
      if (count > licenses)
         {
         kept[i] = licenses/count * 100.0;
         repaired[i] = 0;
         }
      else
         {
         kept[i] = count/licenses * 100.0;
         repaired[i] = (licenses-count)/licenses * 100.0;         
         }
      }
   else
      {
      kept[i] = (int)(aa[i]/count+0.5);
      repaired[i] = (int)(bb[i]/count+0.5);
      }
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_CountHostIssues(struct Item *item)

{ int i,kept[8],repaired[8], issues = 0;
  static char *names[8] = { "zzz", "Week", "Day", "Hour", "Patch", "Lics", "Coms","Anom" };
 
Nova_GetLevels(kept,repaired,item->name,names);

// Compliance and anomalies

issues += (100 - kept[3] - repaired[3]);
issues += (100 - kept[7] - repaired[7]);

// By this reckoning, red > 100, yellow > 50 green < 50

if (issues > 200)
   {
   return false;
   }

if (issues < 0)
   {
   return false;
   }

item->counter = issues;
return true;
}

/*****************************************************************************/

char *Nova_GetHostClass(char *host)

{ FILE *fin,*fout;
  char class[CF_SMALLBUF];
  char *sp,line[CF_BUFSIZE];

class[0] = '\0';

if ((fin = fopen("hardclass.txt","r")) != NULL)
   {
   fscanf(fin,"%s",class);
   fclose(fin);
   
   if (strlen(class) > 0)
      {
      return strdup(class);
      }
   }

// See if it's been cached

if ((fin = fopen("variables.html","r")) == NULL)
   {
   return strdup("any");
   }

while (!feof(fin))
   {
   fgets(line,CF_BUFSIZE,fin);
   
   if (sp = strstr(line,"class</td><td> "))
      {
      sscanf(sp+strlen("class</td><td> "),"%[^<]",class);
      break;
      }
   }

fclose(fin);

if (strlen(class) > 0)
   {
   if ((fout = fopen("hardclass.txt","w")) == 0)
      {
      return strdup("any");
      }

   fprintf(fout,"%s",class);
   fclose(fout);
   }

return strdup("any");
}

