
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: graphics_core.c                                                     */
/*                                                                           */
/* Created: Mon Jul 26 09:57:32 2010                                         */
/*                                                                           */
/*****************************************************************************/


int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,BACKGR,ORANGE,SKY;

int GREYS[CF_SHADES];
int BLUES[CF_SHADES];
int YELLOWS[CF_SHADES];
int PINKS[CF_SHADES];
int BROWNS[CF_SHADES];

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

/*
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


Banner("Create Host Portal");
serverlist = Nova_CreateHostPortal(serverlist);
Nova_BuildMainMeter(&cfv_small,serverlist);

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

*/
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

for (i = 0; i < CF_SHADES; i++)
   {
   r = (int)((100.0)/(double)CF_SHADES * (double)i);

   g = startgreen +
       (int)( (100.0-(double)startgreen)/((double)CF_SHADES*1.5) * (double)i*1.5);

   b = startblue + (int)((255.0-(double)startblue)/(double)CF_SHADES * (double)i);
   BLUES[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }


RED      = gdImageColorAllocate(cfv->im, 208, 45, 72);
YELLOW   = gdImageColorAllocate(cfv->im, 242, 238, 134);
GREEN    = gdImageColorAllocate(cfv->im, 115, 177, 103);

ORANGE   = gdImageColorAllocate(cfv->im, 223,149,0);
LIGHTGREY= gdImageColorAllocate(cfv->im, 75, 75, 66);
//BACKGR   = gdImageColorAllocate(cfv->im,239, 234, 204); // Background
BACKGR   = gdImageColorAllocate(cfv->im,255, 255, 255); // Background
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
SKY      = gdImageColorAllocate(cfv->im, 255,255,255);
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
