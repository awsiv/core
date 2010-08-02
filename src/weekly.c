/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: timeseries.c                                                        */
/*                                                                           */
/*****************************************************************************/

extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,BACKGR,ORANGE;
extern char *UNITS[];


/*****************************************************************************/

double Num(double x)
{
if (isnan(x) || isinf(x))
   {
   return 0;
   }

if (x > CF_BIGNUMBER)
   {
   return 0;
   }

if (x < 0)
   {
   return 0;
   }

return x;
}

/*****************************************************************************/

#ifdef HAVE_LIBGD

void Nova_ViewWeek(struct CfDataView *cfv,char *keyhash,enum observables obs)
    
{ int i,y,hint;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];

  /* Initialization */

snprintf(newfile,CF_BUFSIZE,"%s/hub/%s/%s_week.png",cfv->docroot,keyhash,OBS[obs][0]);
MakeParentDirectory(newfile,true);

cfv->title = OBS[obs][1];
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);

Nova_MakePalette(cfv);

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,BACKGR);
   }

/* Done initialization */

Nova_ReadTimeSeries(cfv,keyhash,obs);
Nova_PlotQFile(cfv,LIGHTRED,GREEN,YELLOW);
Nova_Title(cfv,BLUE);
Nova_DrawQAxes(cfv,BLACK);

if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_verbose,"fopen"," -> Making %s\n",newfile);
   return;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",newfile);
   }
    
gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);
}

/**********************************************************************/

void Nova_ReadTimeSeries(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double rx,ry,rs,rq;
  FILE *fp;
  char name[CF_BUFSIZE];
  int i,have_data = false;
  mongo_connection dbconn;
  double q[CF_TIMESERIESDATA],e[CF_TIMESERIESDATA],d[CF_TIMESERIESDATA];

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

CFDB_QueryWeekView(&dbconn,keyhash,obs,q,e,d);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

cfv->over = 0;
cfv->under = 0;
cfv->over_dev1 = 0;
cfv->under_dev1 = 0;
cfv->over_dev2 = 0;
cfv->under_dev2 = 0;
cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

Q_MEAN = 0;
Q_SIGMA = 0;

for (i = 0; i < CF_TIMESERIESDATA; i++)
   {
   ry = Num(e[i]);
   rs = Num(d[i]);
   rq = Num(q[i]);

   if (ry + rq > 0)
      {
      have_data = true;
      }
   }

if (have_data)
   {
   for (i = 0; i < CF_TIMESERIESDATA; i++)
      {
      ry = Num(e[i]);
      rs = Num(d[i]);
      rq = Num(q[i]);
      
      if (ry > cfv->max)
         {
         cfv->max = ry;
         }
      
      cfv->error_scale = (cfv->error_scale+rs)/2;
      
      if (ry < cfv->min)
         {
         cfv->min = ry;
         }
      
      cfv->data_E[i] = ry;
      cfv->bars[i] = rs;
      cfv->data_q[i] = rq;
      
      if (rq > cfv->max)
         {
         cfv->max = rq;
         }
      
      cfv->error_scale = (cfv->error_scale+rs)/2;
      
      if (rq < cfv->min)
         {
         cfv->min = rq;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i])
         {
         cfv->over++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i])
         {
         cfv->under++;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i]+cfv->bars[i])
         {
         cfv->over_dev1++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i]-cfv->bars[i])
         {
         cfv->under_dev1++;
         }
      
      if (cfv->data_q[i] > cfv->data_E[i]+2*cfv->bars[i])
         {
         cfv->over_dev2++;
         }
      
      if (cfv->data_q[i] < cfv->data_E[i]-2*cfv->bars[i])
         {
         cfv->under_dev2++;
         }
      
      if (cfv->data_E[i] != 0)
         {
         Q_MEAN = GAverage(Q_MEAN,cfv->data_E[i],0.5);
         }
      
      if (cfv->bars[i])
         {
         Q_SIGMA = GAverage(Q_SIGMA,cfv->bars[i],0.5);
         }
      }
   
   if (cfv->max > CF_MAX_LIMIT)
      {
      cfv->max = CF_MAX_LIMIT;
      }
   
   cfv->origin_x = cfv->margin;
   cfv->origin_y = cfv->height+cfv->margin;
   
   cfv->max_x = cfv->margin+cfv->width;
   cfv->max_y = cfv->margin;
   
   if (cfv->error_scale > cfv->max - cfv->min)
      {
      cfv->error_scale = cfv->max - cfv->min;
      }
   
   if (cfv->max == cfv->min)
      {
      if (cfv->error_scale > 0)
         {
         cfv->max += cfv->error_scale/2.0;
         cfv->min -= cfv->error_scale/2.0;
         }
      else
         {
         cfv->max += 1;
         cfv->min -= 1;
         }
      }
   
   cfv->range = (cfv->max - cfv->min + cfv->error_scale);
   
   if (cfv->error_scale > (cfv->max - cfv->min)*1.5)
      {
      cfv->range = (cfv->max - cfv->min);
      }
   else
      {
      cfv->range = (cfv->max - cfv->min + cfv->error_scale);   
      }
   
   cfv->scale_x = (double)cfv->width / (double)CF_TIMESERIESDATA;
   cfv->scale_y = ((double) cfv->height) / cfv->range;
   }
else
   {
   cfv->range = 2;
   cfv->max += 1;
   cfv->min -= 1;         
   cfv->max += 1;
   cfv->min -= 1;
   }
}

/**********************************************************************/

void Nova_DrawQAxes(struct CfDataView *cfv,int col)

{ int origin_x = cfv->margin;
  int origin_y = cfv->height+cfv->margin;
  int max_x = cfv->margin+cfv->width;
  int max_y = cfv->margin;
  int day,x,y;
  double q,dq;
  int ticksize = cfv->height/50;
  static char *days[7] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
  
gdImageLine(cfv->im, origin_x, origin_y, max_x, origin_y, col);
gdImageLine(cfv->im, origin_x, origin_y, origin_x, max_y, col);

for (day = 0; day < 7; day++)
   {
   x = origin_x + day * cfv->width/7;

   gdImageLine(cfv->im, x, origin_y-ticksize, x, origin_y+ticksize, col);   
   gdImageString(cfv->im, gdFontGetLarge(),x,origin_y+2*ticksize,days[day],col);
   }


// Make 5 gradations

dq = cfv->range/5.0;

if (dq < 0.001)
   {
   char qstr[16];

   q = cfv->max;
   x = Nova_ViewPortX(cfv,0);
   y = Nova_ViewPortY(cfv,q,CF_MAGMARGIN);

   gdImageLine(cfv->im, x-2*ticksize, y, x+2*ticksize, y, col);

   snprintf(qstr,15,"%.1f",q);
   gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,col);
   }
else
   {
   for (q = cfv->min; q <= cfv->min+cfv->range; q += dq)
      {
      char qstr[16];

      x = Nova_ViewPortX(cfv,0);
      y = Nova_ViewPortY(cfv,q,CF_MAGMARGIN);
      
      gdImageLine(cfv->im, x-2*ticksize, y, x+2*ticksize, y, col);
      snprintf(qstr,15,"%.1f",q);
      gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,col);
      }
   }
}

/*******************************************************************/

void Nova_PlotQFile(struct CfDataView *cfv,int col1,int col2,int col3)

{ int origin_x = cfv->margin;
  int origin_y = cfv->height+cfv->margin;
  int max_x = cfv->margin+cfv->width;
  int max_y = cfv->margin;
  int i,x,y,lx = 0,ly = 0;
  double rx,ry,rs,sx = 0,s;
  double low,high,av;
  time_t now = time(NULL);

// First plot average

for (sx = 0; sx < CF_TIMESERIESDATA; sx++)
   {
   x = Nova_ViewPortX(cfv,sx);
   y = Nova_ViewPortY(cfv,cfv->data_E[(int)sx],cfv->error_scale);

   s = cfv->bars[(int)sx] * cfv->scale_y;

   if (lx == 0 && ly == 0)
      {
      gdImageSetPixel(cfv->im,x,y,col1);
      }
   else
      {
      low = (y-s < max_y)? max_y : y-s;
      high = (y+s > origin_y)? origin_y : y+s;
      av = (y < max_y)? max_y : y;
      
      gdImageSetThickness(cfv->im,1);
      gdImageLine(cfv->im,lx,ly,x,y,col2);

      // Error bars

      gdImageSetThickness(cfv->im,1);

      if (av > origin_y)
         {
         gdImageLine(cfv->im,x,low,x,av,col1);      
         }
      else
         {
         gdImageLine(cfv->im,x,low,x,av,col1);      
         gdImageLine(cfv->im,x,av,x,high,col2);
         }
      }
   
   lx = x;
   ly = y;
   }

// Now overlay current

lx = ly = 0;

for (sx = 0; sx < CF_TIMESERIESDATA; sx++)
   {
   x = Nova_ViewPortX(cfv,sx);
   y = Nova_ViewPortY(cfv,cfv->data_q[(int)sx],cfv->error_scale);

   if (lx == 0 && ly == 0)
      {
      gdImageSetPixel(cfv->im,x,y,col1);
      }
   else
      {
      gdImageSetThickness(cfv->im,3);
      gdImageLine(cfv->im,lx,ly,x,y,ORANGE);
      }
   
   lx = x;
   ly = y;
   }

// Now plot now spot

sx = Nova_GetNowPosition(now);

x = Nova_ViewPortX(cfv,sx);
y = Nova_ViewPortY(cfv,cfv->data_q[(int)sx],cfv->error_scale);

gdImageSetThickness(cfv->im,3);
gdImageArc(cfv->im,x,y,20,20,0,360,RED);
}

/***********************************************************/

void Nova_AnalyseWeek(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ char fname[CF_BUFSIZE],img[CF_BUFSIZE];
  FILE *fp = stdout;
  double x;

  /* First find the variance sigma2 */


fprintf(fp,"<h3>Weekly trends on %s</h3>\n",OBS[obs][0]);

Nova_GraphLegend(fp);

snprintf(img,CF_BUFSIZE,"%s/hub/%s/%s_weekly.png",DOCROOT,keyhash,OBS[obs][0]);

fprintf(fp,"<div id=\"graph\">\n");
fprintf(fp,"<a href=\"%s\"><img src=\"%s\" width=\"590\"></a>\n",img,img);
fprintf(fp,"</div>\n");

fprintf(fp,"<div id=\"weekanalysis\">\n");

fprintf(fp,"<table>\n");
fprintf(fp,"<tr><td>Maximum value </td><td>%lf</td><td>%s</td></tr>\n",cfv->max,UNITS[obs]);
fprintf(fp,"<tr><td>Minimum value </td><td>%lf</td><td>%s</td></tr>\n",cfv->min,UNITS[obs]);

Q_MAX = cfv->max;
Q_MIN = cfv->min;

x = 100*(double)cfv->over/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage over average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);
x = 100*(double)cfv->under/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage under average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);

x = 100*(double)cfv->over_dev1/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage 1 deviation over average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);
x = 100*(double)cfv->under_dev1/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage 1 deviation under average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);

x = 100*(double)cfv->over_dev2/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage 2 deviations over average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);
x = 100*(double)cfv->under_dev2/(double)CF_TIMESERIESDATA;
fprintf(fp,"<tr><td>Percentage 2 deviations under average/normal</td><td>%lf</td><td>\%</td></tr>\n",x);

fprintf(fp,"</table>\n");
fprintf(fp,"</div></div>\n");
}

#endif
