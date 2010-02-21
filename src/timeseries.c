
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

extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY;
extern int GREYS[CF_SHADES];
extern int BLUES[CF_SHADES];
extern char *UNITS[];

/*****************************************************************************/

#ifdef HAVE_LIBGD

int Nova_ViewWeek(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *host)
    
{ int i,y,hint;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];
  char oldfile[CF_BUFSIZE];
  char fileroot[CF_BUFSIZE];

  /* Initialization */

snprintf(newfile,CF_BUFSIZE,"%s_weekly.png",filename);
snprintf(oldfile,CF_BUFSIZE,"%s.q",filename);
snprintf(fileroot,CF_BUFSIZE,"%s",filename);

if ((cfstat(oldfile,&s1) == -1))
   {
   CfOut(cf_verbose,""," -> No q series data for %s - abort",filename);
   return false;
   }

if ((cfstat(newfile,&s2) == -1))
   {
   CfOut(cf_verbose,""," -> No existing graph for %s",filename);
   }
else
   {
   if (s2.st_mtime > s1.st_mtime)
      {
      /* no changes */
      CfOut(cf_verbose,""," -> No graphs up to date for %s",filename);
      return true;
      }
   }

CfOut(cf_verbose,""," -> There seem to be new data in %s\n",fileroot);

cfv->title = title;
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   hint = (int)((double)(cfv->height+2*cfv->margin-y)/(cfv->height+2*cfv->margin) * CF_SHADES);
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,GREYS[hint]);
   }

/* Done initialization */

if (!Nova_ReadTimeSeries(cfv,fileroot))
   {
   CfOut(cf_verbose,""," -> Found no actual data in %s\n",fileroot);
   return false;
   }

Nova_PlotQFile(cfv,LIGHTRED,GREEN,YELLOW);
Nova_Title(cfv,BLUE);
Nova_DrawQAxes(cfv,BLACK);

if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_verbose,"fopen"," -> Making %s\n",newfile);
   return false;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",newfile);
   }
    
gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);

Nova_AnalyseWeek(cfv,filename,obs,host);
return true;
}

/**********************************************************************/

int Nova_ReadTimeSeries(struct CfDataView *cfv, char *filename)

{ double rx,ry,rs,sx = 0;
  FILE *fp;
  char name[CF_BUFSIZE],buffer[CF_BUFSIZE];
  int have_data = false;

strcpy(name,filename);
strcat(name,".E-sigma");

cfv->over = 0;
cfv->under = 0;
cfv->over_dev1 = 0;
cfv->under_dev1 = 0;
cfv->over_dev2 = 0;
cfv->under_dev2 = 0;
cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

if ((fp = fopen(name,"r")) == NULL)
   {
   CfOut(cf_verbose,"","Can't open time series %s\n",name);
   return false;
   }

CfOut(cf_verbose,""," -> Studying time-series %s\n",name);

for (sx = 0; sx < CF_TIMESERIESDATA; sx++)
   {
   rx = ry = rs = 0;

   memset(buffer,0,CF_BUFSIZE);
      
   if (!fgets(buffer,CF_BUFSIZE,fp))
      {
      return false;
      }
   
   sscanf(buffer,"%lf %lf %lf",&rx,&ry,&rs);

   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }

   cfv->data_E[(int)sx] = ry;
   cfv->bars[(int)sx] = rs;

   if (ry > 0)
      {
      have_data = true;
      }
   }

fclose(fp);

strcpy(name,filename);
strcat(name,".q");
  
if ((fp = fopen(name,"r")) == NULL)
   {
   CfOut(cf_verbose,"fopen"," -> Can't open %s\n",name);
   return false;
   }

CfOut(cf_verbose,""," -> Studying time-series %s\n",name);

Q_MEAN = 0;
Q_SIGMA = 0;

for (sx = 0; sx < CF_TIMESERIESDATA; sx++)
   {
   memset(buffer,0,CF_BUFSIZE);
   
   if (!fgets(buffer,CF_BUFSIZE,fp))
      {
      return false;
      }

   sscanf(buffer,"%lf %lf %lf",&rx,&ry,&rs);

   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }

   cfv->data_q[(int)sx] = ry;

   if (cfv->data_q[(int)sx] > cfv->data_E[(int)sx])
      {
      cfv->over++;
      }

   if (cfv->data_q[(int)sx] < cfv->data_E[(int)sx])
      {
      cfv->under++;
      }

   if (cfv->data_q[(int)sx] > cfv->data_E[(int)sx]+cfv->bars[(int)sx])
      {
      cfv->over_dev1++;
      }

   if (cfv->data_q[(int)sx] < cfv->data_E[(int)sx]-cfv->bars[(int)sx])
      {
      cfv->under_dev1++;
      }

   if (cfv->data_q[(int)sx] > cfv->data_E[(int)sx]+2*cfv->bars[(int)sx])
      {
      cfv->over_dev2++;
      }

   if (cfv->data_q[(int)sx] < cfv->data_E[(int)sx]-2*cfv->bars[(int)sx])
      {
      cfv->under_dev2++;
      }

   if (ry > 0)
      {
      have_data = true;
      }

   if (cfv->data_E[(int)sx] != 0)
      {
      Q_MEAN = GAverage(Q_MEAN,cfv->data_E[(int)sx],0.5);
      }
   
   if (cfv->bars[(int)sx])
      {
      Q_SIGMA = GAverage(Q_SIGMA,cfv->bars[(int)sx],0.5);
      }
   }

fclose(fp);

cfv->origin_x = cfv->margin;
cfv->origin_y = cfv->height+cfv->margin;

cfv->max_x = cfv->margin+cfv->width;
cfv->max_y = cfv->margin;

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

if (have_data)
   {
   CfOut(cf_verbose,""," -> Read data for %s\n",name);
   }

return have_data;
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

   gdImageLine(cfv->im, x-2*ticksize, y, cfv->max_x, y, col);

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
  double low,high;
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
      low = y-s;
      high = (y+s > origin_y)? origin_y : y+s;

      gdImageSetThickness(cfv->im,3);
      gdImageLine(cfv->im,lx,ly,x,y,col2);
      
      gdImageSetThickness(cfv->im,1);
      gdImageLine(cfv->im,x,low,x,high,col1);
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
      gdImageSetThickness(cfv->im,1);
      gdImageLine(cfv->im,lx,ly,x,y,col3);
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

void Nova_AnalyseWeek(struct CfDataView *cfv,char *name,enum observables obs,char *host)

{ char fname[CF_BUFSIZE],img[CF_BUFSIZE];
  FILE *fp;
  double x;

  /* First find the variance sigma2 */

snprintf(fname,CF_BUFSIZE,"%s_week.html",name);
  
if ((fp = fopen(fname,"w")) == 0)
   {
   CfOut(cf_verbose,"","Can't open week analysis file %s\n",fname);
   return;   
   }

NovaHtmlHeader(fp,host,STYLESHEET,WEBDRIVER,BANNER);
fprintf(fp,"<h1>Weekly trends on %s</h1>\n",name);

Nova_GraphLegend(fp);

snprintf(img,CF_BUFSIZE,"%s_weekly.png",name);

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

NovaHtmlFooter(fp,FOOTER);
fclose(fp); 
}

#endif
