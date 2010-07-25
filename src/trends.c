
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: magnify.c                                                           */
/*                                                                           */
/*****************************************************************************/

extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY;
extern int GREYS[CF_SHADES];
extern int BLUES[CF_SHADES];
extern time_t DATESTAMPS[CF_OBSERVABLES];
extern char *UNITS[];

#ifdef HAVE_LIBGD

/*****************************************************************************/

void Nova_ViewLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs)
    
{ int i,y,hint,yr,num = 0,ago,lc;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];
  double max=0,min=9999,range;

snprintf(newfile,CF_BUFSIZE,"%s/%s/%s_yr.png",DOCROOT,keyhash,OBS[obs][0]);
   
cfv->title = OBS[obs][1];
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);
   
for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   hint = (int)((double)(cfv->height+2*cfv->margin-y)/(cfv->height+2*cfv->margin) * CF_SHADES);
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,GREYS[hint]);
   }
   
/* Done initialization */

Nova_ReadLongHistory(cfv,keyhash,obs);
Nova_DrawLongHAxes(cfv,BLACK);
Nova_PlotLongHFile(cfv,LIGHTRED,GREEN,YELLOW);
Nova_Title(cfv,BLUE);
   
if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_verbose,"fopen","Cannot write %s file\n",newfile);
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
/* Yrdata                                                             */
/**********************************************************************/

void Nova_ReadLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double range,rx,ry,rq,rs;
  double max,min;
  int count = 0,i;
  mongo_connection dbconn;
  double q[CF_LHISTORYDATA],e[CF_LHISTORYDATA],d[CF_LHISTORYDATA];

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

CFDB_QueryYearView(&dbconn,keyhash,obs,q,e,d);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

max = 0;
min = 99999;
cfv->error_scale = 0;      
  
for (i = 0; i < CF_LHISTORYDATA; i++)
   {
   ry = e[i];
   rq = q[i];
   rs = d[i];

   if (ry > max)
      {
      max = ry;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (ry < min)
      {
      min = ry;
      }

   cfv->data_E[i] = ry;
   cfv->data_q[i] = rq;
   cfv->bars[i] = rs;
   }

cfv->origin_x = cfv->margin;
cfv->origin_y = cfv->height+cfv->margin;

cfv->max_x = cfv->margin+cfv->width;
cfv->max_y = cfv->margin;

if (max == min)
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
else
   {
   if (min > max)
      {
      cfv->max = 1;
      cfv->min = 0;
      }
   
   if (max > cfv->max)
      {
      cfv->max = max;
      }
   
   if (min < cfv->min)
      {
      cfv->min = min;
      }   
   }
}

/**********************************************************************/

void Nova_DrawLongHAxes(struct CfDataView *cfv,int col)

{ int month,x,y,mnow;
  double q,dq;
  time_t now;
  int ticksize = cfv->height/50;
  static char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

mnow = Month2Int(VMONTH);
  
gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->max_x, cfv->origin_y, col);
gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->origin_x, cfv->max_y, col);

for (month = 0; month < 12; month++)
   {
   x = cfv->origin_x + month * cfv->width/12;
   gdImageLine(cfv->im, x, cfv->origin_y-ticksize, x, cfv->origin_y+ticksize, col);
   gdImageString(cfv->im, gdFontGetLarge(),x,cfv->origin_y+2*ticksize,months[(month+mnow) % 12],col);
   }

// Make 10 gradations

dq = cfv->range/10.0;

if (dq < 0.001)
   {
   char qstr[16];

   q = cfv->max;
   x = Nova_ViewPortX(cfv,0);
   y = Nova_ViewPortY(cfv,q,CF_LHISTORYMARGIN);

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
      y = Nova_ViewPortY(cfv,q,CF_LHISTORYMARGIN);
      
      gdImageLine(cfv->im, x-2*ticksize, y, cfv->max_x, y, col);
      snprintf(qstr,15,"%.1f",q);
      gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,col);
      }
   }
}

/*******************************************************************/

void Nova_PlotLongHFile(struct CfDataView *cfv,int col1,int col2,int col3)

{ int i,x,y,lx = 0,ly = 0,now,under,over,av;
  double range,sx;
  double low,high,a,s;
 
for (sx = 0; sx < CF_LHISTORYDATA; sx++)
   {
   x = Nova_ViewPortX(cfv,sx);
   y = Nova_ViewPortY(cfv,cfv->data_E[(int)sx],CF_LHISTORYMARGIN);

   if (lx == 0 && ly == 0)
      {
      gdImageSetPixel(cfv->im,x,y,col1);
      }
   else
      {
      gdImageSetThickness(cfv->im,2);
      gdImageLine(cfv->im,lx,ly,x,y,col3);
      }
   
   lx = x;
   ly = y;
   }
}

/***********************************************************/

void Nova_AnalyseLongHistory(struct CfDataView *cfv,char *name,enum observables obs,char *host)

{ char fname[CF_BUFSIZE],img[CF_BUFSIZE];
  struct stat sb;
  FILE *fp;
  int yr,this_lifecycle,this;

  /* First find the variance sigma2 */

snprintf(fname,CF_BUFSIZE,"%s_yr.html",name);
  
if ((fp = fopen(fname,"w")) == 0)
   {
   CfOut(cf_verbose,"","Can't open magnification analysis file %s\n",fname);
   return;   
   }

fprintf(fp,"<h3>Lifecycle history: %s (beta)</h3>\n",name);

Nova_GraphLegend(fp);

fprintf(fp,"<div id=\"graph\">\n");

for (yr = 2; yr >= 0; yr--)
   {
   snprintf(img,CF_BUFSIZE,"%s_%d.yr.png",name,yr);

   if (cfstat(img,&sb) != -1)
      {
      fprintf(fp,"<a href=\"reports/%s/%s\"><img src=\"reports/%s/%s\" width=\"200\"></a>\n",host,img,host,img);
      }
   }

fprintf(fp,"</div>\n");

fprintf(fp,"<div id=\"longhistory\">\n");

fprintf(fp,"<table>\n");
fprintf(fp,"<tr><td>Maximum value </td><td>%.2lf</td><td>%s</td></tr>\n",cfv->max,UNITS[obs]);
fprintf(fp,"<tr><td>Minimum value </td><td>%.2lf</td><td>%s</td></tr>\n",cfv->min,UNITS[obs]);
fprintf(fp,"<tr><td>Average variability </td><td>+/- %lf</td><td>%s</td></tr>\n",cfv->error_scale,UNITS[obs]);
fprintf(fp,"</table>\n");

fprintf(fp,"</div></div>\n");
fclose(fp);
}

#endif
