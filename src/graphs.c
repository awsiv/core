
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: graphs.c                                                            */
/*                                                                           */
/*****************************************************************************/

int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,ORANGE,SKY;

int GREYS[CF_SHADES];
int BLUES[CF_SHADES];
int YELLOWS[CF_SHADES];
int PINKS[CF_SHADES];
int BROWNS[CF_SHADES];

#define CF_METER_HEIGHT 80
#define CF_METER_WIDTH  500
#define CF_METER_MARGIN 5

/*****************************************************************************/

#ifdef HAVE_LIBGD

void Nova_BuildGraphs(struct CfDataView *cfv)

{ DIR *dirh;
  struct dirent *dirp;
  int i, count = 0, compliance;
  char name[CF_BUFSIZE],description[CF_BUFSIZE],index[16];
  struct Item *serverlist = NULL,*ip;
  struct Item *eliminate = NULL;
  struct CfDataView cfv_small;
  struct Item *unique = NULL;
  struct stat sb;
  FILE *fout;
    
cfv->height = 300;
cfv->width = 700; //(7*24*2)*2; // halfhour
cfv->margin = 50;

/* Get the list of clients */

CfOut(cf_verbose,""," -> Entering the core in %s\n",AGGREGATION);
chdir(AGGREGATION);

if ((dirh = opendir(AGGREGATION)) == NULL)
   {
   CfOut(cf_error,"opendir","Can't open directory %s\n",AGGREGATION);
   return;
   }

for (dirp = readdir(dirh); dirp != NULL; dirp = readdir(dirh))
   {
   if (cfstat(dirp->d_name,&sb) == -1)
      {
      CfOut(cf_error,"stat"," !! %s is unreadable",dirp->d_name);
      continue;
      }

   if (strcmp(dirp->d_name,".") == 0 || strcmp(dirp->d_name,"..") == 0)
      {
      continue;
      }
   
   if (!S_ISDIR(sb.st_mode))
      {
      continue;
      }

   AppendItem(&serverlist,dirp->d_name,NULL);
   }

closedir(dirh);

/* Create portal */

Banner("Create Host Portal");
serverlist = Nova_CreateHostPortal(serverlist);
Nova_BuildMainMeter(&cfv_small,serverlist);

/* Create host pages */

for (ip = serverlist; ip != NULL; ip=ip->next)
   {
   char hostn[CF_MAXVARSIZE];
   
   if (chdir(ip->name) == -1)
      {
      printf("Could not write to directory %s\n",ip->name);
      continue;
      }
   
   snprintf(hostn,CF_MAXVARSIZE-1," -> Rendering reports for %s",ip->name);
   Banner(hostn);

   Nova_UnPackNerveBundle();

   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      Nova_LookupAggregateClassName(i,name,description);

      if (strcmp(name,"spare") == 0)
         {
         continue;
         }

      if (i > ob_spare)
         {
         snprintf(description,CF_BUFSIZE-1,"measurement %s",name);
         }
      
      CfOut(cf_verbose,""," -> Detected %s/%s\n",name,description);

      if (!Nova_ViewLatest(cfv,name,description,i,ip->name))
         {
         snprintf(index,15,"%d",i);
         PrependItem(&eliminate,index,NULL);
         CfOut(cf_verbose,""," !! Eliminating %s/%s due to lack of data\n",name,description);
         }

      CfOut(cf_verbose,""," -> Processing host source %s / %s",ip->name,name);

      Nova_ViewWeek(cfv,name,description,i,ip->name);
      Nova_ViewHisto(cfv,name,description,i,ip->name);
      Nova_ViewLongHistory(cfv,name,description,i,ip->name);
      CfOut(cf_verbose,""," -> Done with %s / %s",ip->name,name);
      }

   compliance = Nova_BuildMeters(&cfv_small,ip->name);
   ip->counter = compliance;
   ip->classes = Nova_GetHostClass(ip->name);
   
   Nova_MainPage(ip->name,eliminate);
   Nova_OtherPages(ip->name,eliminate);
   DeleteItemList(eliminate);
   eliminate = NULL;
   chdir("..");
   }

// Create a simple list of status for 3rd party export/integration

snprintf(name,CF_BUFSIZE,"summary.z");

if (fout = fopen(name,"w"))
   {
   for (ip = serverlist; ip != NULL; ip=ip->next)
      {
      if (!IsItemIn(unique,Hostname2IPString(ip->name)))
         {
         PrependItem(&unique,Hostname2IPString(ip->name),NULL);
         
         if (strcmp(ip->name,"localhost") == 0 || strcmp(ip->name,"127.0.0.1") == 0)
            {
            }
         else
            {
            fprintf(fout,"%s,/Server/%s,%d\n",Hostname2IPString(ip->name),Titleize(ip->classes),ip->counter);
            }
         }
         
      }

   fclose(fout);
   }

DeleteItemList(unique);
DeleteItemList(serverlist);
}

/*****************************************************************************/

void Nova_Title(struct CfDataView *cfv,int col)

{ char datestr[CF_MAXVARSIZE];
  char title[CF_MAXVARSIZE];
  time_t now = time(NULL);

strcpy(datestr,cf_ctime(&now));  
Chop(datestr);

snprintf(title,CF_MAXVARSIZE-1,"%s @ %s",cfv->title,datestr);
    
gdImageString(cfv->im,
              gdFontGetGiant(),
              cfv->im->sx / 2 - (strlen(title) * gdFontGetGiant()->w / 2),
              cfv->margin/2,
              title,col);
}

/*****************************************************************************/

void Nova_MakePalette(struct CfDataView *cfv)

{ int i,hint,r,g,b;
  int startgrey = 220,startblue = 150, startgreen = 40;

BLACK = gdImageColorAllocate(cfv->im, 48, 48, 42);
WHITE    = gdImageColorAllocate(cfv->im, 255, 255, 255);
BLUE     = gdImageColorAllocate(cfv->im, 50, 100, 100);
LIGHTRED = gdImageColorAllocate(cfv->im, 189, 58, 43);

RED      = gdImageColorAllocate(cfv->im, 208, 45, 72);
YELLOW   = gdImageColorAllocate(cfv->im, 242, 238, 134);
GREEN    = gdImageColorAllocate(cfv->im, 115, 177, 103);

LIGHTGREY= gdImageColorAllocate(cfv->im, 75, 75, 66); // Background
}

/*****************************************************************************/

void Nova_MakeCosmosPalette(struct CfDataView *cfv)

{ int i,hint,r,g,b;
  int startgrey = 160,startblue = 150, startgreen = 80;
  int rs,gs,bs,re,ge,be,dr,dg,db;
  
BLACK = gdImageColorAllocate(cfv->im, 0, 0, 0);

for (i = 0; i < CF_SHADES; i++)
   {
   r = (int)((255.0)/(double)CF_SHADES * (double)i);
   
   g = startgreen +
       (int)((255.0-(double)startgreen)/((double)CF_SHADES*1.5) * (double)i*1.5);

   b = startblue + (int)((255.0-(double)startblue)/(double)CF_SHADES * (double)i);
   BLUES[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

// Browns From 205,170,125, 165,130,85

rs = 245;
re = 210;
gs = 245;
ge = 210;
bs = 245;
be = 210;

dr = (int)((double)(re-rs)/(double)CF_SHADES);
dg = (int)((double)(ge-gs)/(double)CF_SHADES);
db = (int)((double)(be-bs)/(double)CF_SHADES);

for (i = 0; i < CF_SHADES; i++)
   {
   r = rs + i * dr;
   g = gs + i * dg;
   b = bs + i * db;
   BROWNS[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

// Yell 255,255,142 to 255,255,255

rs = 255;
re = 255;
gs = 215;
ge = 255;
bs = 102;
be = 200;

dr = (int)((double)(re-rs)/(double)CF_SHADES);
dg = (int)((double)(ge-gs)/(double)CF_SHADES);
db = (int)((double)(be-bs)/(double)CF_SHADES);

for (i = 0; i < CF_SHADES; i++)
   {
   r = rs + i * dr;
   g = gs + i * dg;
   b = bs + i * db;
   YELLOWS[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

// pinks 238,213,183, 198,173,143

rs = 248;
re = 193;
gs = 229;
ge = 168;
bs = 203;
be = 138;

dr = (int)((double)(re-rs)/(double)CF_SHADES);
dg = (int)((double)(ge-gs)/(double)CF_SHADES);
db = (int)((double)(be-bs)/(double)CF_SHADES);

for (i = 0; i < CF_SHADES; i++)
   {
   r = rs + i * dr;
   g = gs + i * dg;
   b = bs + i * db;
   PINKS[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

// "Greys" 205,170,125

rs = 238;
re = 220;
gs = 213;
ge = 220;
bs = 183;
be = 220;

dr = (int)((double)(re-rs)/(double)CF_SHADES);
dg = (int)((double)(ge-gs)/(double)CF_SHADES);
db = (int)((double)(be-bs)/(double)CF_SHADES);

for (i = 0; i < CF_SHADES; i++)
   {
   r = rs + i * dr;
   g = gs + i * dg;
   b = bs + i * db;
   GREYS[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }


WHITE    = gdImageColorAllocate(cfv->im, 255, 255, 255);
LIGHTGREY= gdImageColorAllocate(cfv->im, 220, 220, 220);
GREEN    = gdImageColorAllocate(cfv->im, 0, 200, 0);
BLUE     = gdImageColorAllocate(cfv->im, 50, 100, 100);
YELLOW   = gdImageColorAllocate(cfv->im, 200, 255, 0);
LIGHTRED = gdImageColorAllocate(cfv->im, 255, 150, 150);
RED      = gdImageColorAllocate(cfv->im, 200, 0, 0);
ORANGE   = gdImageColorAllocate(cfv->im, 223,149,0);
SKY      = gdImageColorAllocate(cfv->im, 239,234,204);
}

/*****************************************************************************/

void Nova_GraphMagLegend(FILE *fout)
{
fprintf(fout,"<div id=\"legend\">"
        "<table>"
        "<tr>"
        "<td><span id=\"below\">Average deviation below mean</span></td>"
        "<td><span id=\"above\">Average deviation above mean</span></td>"
        "<td><span id=\"last\">Last measured</span></td>"
        "</tr>"
        "</table>");
}

/*****************************************************************************/

void Nova_GraphLegend(FILE *fout)
{
fprintf(fout,"<div id=\"legend\">"
        "<table>"
        "<tr>"
        "<td><span id=\"below\">Local mean</span></td>"
        "<td><span id=\"above\">Average deviation about mean</span></td>"
        "<td><span id=\"last\">Last measured value</span></td>"
        "</tr>"
        "</table>");
}

/*****************************************************************************/

double Nova_GetNowPosition(time_t now)

{ int this_week,i,days=0,hrs=0,mins=0;
  double position;
  char str[CF_BUFSIZE],buf1[CF_BUFSIZE],buf4[CF_BUFSIZE],buf[CF_BUFSIZE];
  static char *wdays[7] =
      {
      "Mon",
      "Tue",
      "Wed",
      "Thu",
      "Fri",
      "Sat",
      "Sun"
      };
  
sprintf(str,"%s",cf_ctime(&now));

sscanf(str,"%s %*s %*s %s %*s",buf1,buf4,buf);

/* Day */

for (i = 0; i < 7; i++)
   {
   if (strcmp(buf1,wdays[i]) == 0)
      {
      days = i;
      break;
      }
   }
 
/* Hours */
sscanf(buf4,"%[^:]",buf);
hrs = atoi(buf);

/* Minutes */
sscanf(buf4,"%*[^:]:%[^:]",buf);
mins = atoi(buf);

this_week = days * 3600 * 24 + hrs * 3600 + (mins-2)* 60;
return  ((double)this_week/(double)CF_WEEK * (double)CF_TIMESERIESDATA);
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

int Nova_BuildMeters(struct CfDataView *cfv,char *hostname)

{ FILE *fout;
  char filename[CF_BUFSIZE];
  int kept,repaired,returnval = 0;
  struct stat sb;
  struct utimbuf t;
  
cfv->height = CF_METER_HEIGHT;
cfv->width = CF_METER_WIDTH;
cfv->margin = CF_METER_MARGIN;
cfv->title = "System state";
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

gdImageSetThickness(cfv->im,2);
gdImageFilledRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,LIGHTGREY);
gdImageRectangle(cfv->im,0,0,cfv->width+2*cfv->margin,cfv->height+2*cfv->margin,BLACK);

// Bar 1

Nova_GetLevel("Week",&kept,&repaired);
Nova_BarMeter(cfv,1,kept,repaired,"Week");
Nova_GetLevel("Day",&kept,&repaired);
Nova_BarMeter(cfv,2,kept,repaired,"Day");
Nova_GetLevel("Hour",&kept,&repaired);
Nova_BarMeter(cfv,3,kept,repaired,"Hour");

returnval = kept+repaired;

Nova_GetLevel("Patch",&kept,&repaired);
Nova_BarMeter(cfv,4,kept,repaired,"Ptch");
Nova_GetLevel("Lics",&kept,&repaired);
Nova_BarMeter(cfv,5,kept,repaired,"Lics");
Nova_GetLevel("Comms",&kept,&repaired);
Nova_BarMeter(cfv,6,kept,repaired,"Coms");
Nova_GetLevel("Anom",&kept,&repaired);
Nova_BarMeter(cfv,7,kept,repaired,"Anom");

snprintf(filename,CF_BUFSIZE,"meters.png");

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

#ifdef HAVE_UTIME_H

if (cfstat("comp_key",&sb) != -1)
   {   
   t.actime = sb.st_ctime;
   t.modtime = sb.st_mtime;
   utime("meters.png",&t);
   }

#endif

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
  
snprintf(filename,CF_BUFSIZE,"host_portal.html");

if ((fout = fopen(filename, "w")) == NULL)
   {
   return list;
   }

NovaHtmlHeader(fout,"20 Weakest Host Symptoms",STYLESHEET,WEBDRIVER,BANNER);

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   CfOut(cf_inform,""," -> %d paid licenses have been asserted (this is a promise by you)",licenses);
   }

for (ip = list; ip != NULL; ip=ip->next)
   {
   count++;
   ip->counter = 0;
   Nova_CountHostIssues(ip);

   snprintf(filename,CF_BUFSIZE-1,"%s/fluctuations.nov",ip->name);

   if (cfstat(filename,&sb) != -1)
      {
      if (now > sb.st_mtime + 3600 || ip->counter > CF_RED_THRESHOLD)
         {
         ip->counter = CF_RED_THRESHOLD + 1;
         }
      else if (now > sb.st_mtime + 1800 || ip->counter > CF_AMBER_THRESHOLD)
         {
         ip->counter = CF_AMBER_THRESHOLD + 1;
         }
      }
   else
      {
      ip->counter = CF_RED_THRESHOLD + 1;
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

fprintf(fout,"<div id=\"directory\"><table>\n");

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
   else
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
   
   fprintf(fout,"<tr><td>%s<br><center><div id=\"signal%s\">\n"
           "<table><tr><td width=\"80\">&nbsp;</div></center></td></tr></table></td>\n"
           "<td width=\"200\"><center>Last updated at<br>%s</center></td>\n",
           ip->name,
           col,
           cf_ctime(&(sb.st_mtime)));
           
   fprintf(fout,"<td><a href=\"%s\"><img src=\"reports/%s/meters.png\"></a></td>\n",URLControl("%s",url1),ip->name);
   fprintf(fout,"<td><span id=\"rbuttons\"><a href=\"%s\">Promises</a>\n",URLControl("%s",url2));
   fprintf(fout,"<br><a href=\"%s\">Classes</a></span></td></tr>\n",URLControl("%s",url3));
   }

fprintf(fout,"</table></div>\n");
NovaHtmlFooter(fout,FOOTER);

fclose(fout);

/* All hosts page */

snprintf(filename,CF_BUFSIZE,"allhosts.html");

if ((fall = fopen(filename, "w")) == NULL)
   {
   fclose(fout);
   return list;
   }

// Split here?

NovaHtmlHeader(fall,"Status all managed hosts",STYLESHEET,WEBDRIVER,BANNER);

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
         strcpy(col,"red");
         }
      else if (now > sb.st_mtime + 1800 || ip->counter > CF_AMBER_THRESHOLD)
         {
         ip->counter = 1;
         ccount[1]++;
         strcpy(col,"yellow");
         }
      else
         {
         ip->counter = 2;
         ccount[2]++;
         strcpy(col,"green");
         }
      }
   else
      {
      ip->counter = 0;
      }
   }

fprintf(fall,"<div id=\"allhosts\">\n<table>\n");

fprintf(fall,"<tr><td>Currently aware of %d licensed hosts</td></tr>\n",count);

for (state = 0; state < 3; state++)
   {
   fprintf(fall,"<tr><td>\n");

   if (state == 0 && ccount[0] == 0)
      {
      fprintf(fall,"<span id=\"signalred\"><a href=\"%s\">(none)</a></span> ",URLControl("%s","reports/host_portal.html"));
      ccount[0]++;
      continue;
      }

   if (state == 1 && ccount[1] == 0)
      {
      ccount[1]++;
      fprintf(fall,"<span id=\"signalyellow\"><a href=\"%s\">(none)</a></span> ",URLControl("%s","reports/host_portal.html"));
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
                fprintf(fall,"<span id=\"signalred\"><a href=\"%s\">%s</a></span> ",URLControl("%s",url1),ip->name);
                break;
            case 1:
                fprintf(fall,"<span id=\"signalyellow\"><a href=\"%s\">%s</a></span> ",URLControl("%s",url1),ip->name);
                break;
            case 2:
                fprintf(fall,"<span id=\"signalgreen\"><a href=\"%s\">%s</a></span> ",URLControl("%s",url1),ip->name);
                break;
            }
         }
      }
   
   fprintf(fall,"</td></tr>\n");
   }

fprintf(fall,"</table></div>\n");
NovaHtmlFooter(fall,FOOTER);
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

void Nova_GetLevel(char *id,int *kept,int *repaired)

{ FILE *fin;
  char buf[CF_BUFSIZE];
  double a = 0, b= 0;
  
*kept = 0;
*repaired = 0;

if (strcmp(id,"Lics") == 0)
   {
   *kept = 100;
   *repaired = 0;
   }

if ((fin = fopen("comp_key","r")) == NULL)
   {
   return;
   }

while(!feof(fin))
   {
   fgets(buf,CF_BUFSIZE-1,fin);

   if (strncmp(buf,id,strlen(id)) == 0)
      {
      sscanf(buf,"%*s %lf %lf",&a,&b);
      *kept = (int)(a+0.5);
      *repaired = (int)(b+0.5);
      }
   }

fclose(fin);
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

snprintf(buf,CF_BUFSIZE-1,"%s/comp_key",hostname);

if ((fin = fopen(buf,"r")) == NULL)
   {
   return;
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

void Nova_CountHostIssues(struct Item *item)

{ int i,kept[8],repaired[8], issues = 0;
  static char *names[8] = { "zzz", "Week", "Day", "Hour", "Patch", "Lics", "Coms","Anom" };
 
Nova_GetLevels(kept,repaired,item->name,names);

issues += (100 - kept[3] - repaired[3]);
issues += (100 - kept[6] - repaired[6]);
issues += (100 - kept[7] - repaired[7]);

// By this reckoning, red > 100, yellow > 50 green < 50

if (issues < 0)
   {
   issues = 0;
   }

item->counter = issues;
item->time = 100 - kept[2] + repaired[2];
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

/*****************************************************************************/

void Nova_Font(struct CfDataView *cfv,double x,double y,char *s,int colour)

{ char *err,ps[CF_MAXVARSIZE];
  int x1,y1,x2,y2,margin = 1,padding=2,tab=3;
  static char *font1 = "DejaVuSans";
  char *font = font1;
  int brect[8];
  double size = 8.0;

snprintf(ps,CF_MAXVARSIZE,"%s",s);


/* brect
   0	lower left corner, X position
   1	lower left corner, Y position
   2	lower right corner, X position
   3	lower right corner, Y position
   4	upper right corner, X position
   5	upper right corner, Y position
   6	upper left corner, X position
   7	upper left corner, Y position
*/

err = gdImageStringFT(NULL,&brect[0],colour,font1,size,0.,0,0,ps);

if (err)
   {
   printf("Rendering failure %s\n",err);
   }

// Plus y is now downward

gdImageStringFT(cfv->im,&brect[0],colour,font,size,0.0,x,y,ps);
}


#endif
