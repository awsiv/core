
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

extern int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY,BACKGR,ORANGE;
extern time_t DATESTAMPS[CF_OBSERVABLES];
extern char *UNITS[];

#ifdef HAVE_LIBGD

/*****************************************************************************/

void Nova_ViewMag(struct CfDataView *cfv,char *keyhash,enum observables obs)
    
{ int i,y,hint;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];

snprintf(newfile,CF_BUFSIZE,"%s/hub/%s/%s_mag.png",cfv->docroot,keyhash,OBS[obs][0]);
MakeParentDirectory(newfile,true);

cfv->title = OBS[obs][1];
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

/* background colour */

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,BACKGR);
   }

/* Done initialization */

Nova_ReadMagTimeSeries(cfv,keyhash,obs);
Nova_PlotMagQFile(cfv,LIGHTRED,GREEN,ORANGE);
Nova_Title(cfv,BLUE);

// Assume we are in the keyhash directory

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
/* Magdata                                                            */
/**********************************************************************/

void Nova_ReadMagTimeSeries(struct CfDataView *cfv,char *hostkey,enum observables obs)

{ double range,rx,ry,rq,rs;
  double ly = 1,lq = 1,ls = 1;
  double q[CF_MAGDATA],e[CF_MAGDATA],d[CF_MAGDATA];
  int i;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"","!! Could not open connection to report database");
   }

CFDB_QueryMagView(&dbconn,hostkey,obs,time(NULL) - 4*3600,q,e,d);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"","!! Could not close connection to report database");
   } 

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

for (i = 0; i < CF_MAGDATA; i++)
   {
   cfv->data_E[i] = 0;
   cfv->data_q[i] = 0;
   cfv->bars[i] = 0;
   }

for (i = 0; i < CF_MAGDATA; i++)
   {
   ry = Num(e[i]);
   rq = Num(q[i]);
   rs = Num(d[i]);
   
   if (ry / ly > 1000)
      {
      ry = ly;
      }

   if (rs / ls > 1000)
      {
      rs = ls;
      }

   if (rq / lq > 100000)
      {
      rq = lq;
      }

   if (rq > cfv->max)
      {
      cfv->max = rq;
      }

   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (rq < cfv->min)
      {
      cfv->min = rq;
      }

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }
   
   ly = cfv->data_E[i] = ry;
   lq = cfv->data_q[i] = rq;
   ls = cfv->bars[i] = rs;
   }

cfv->max++;
cfv->min--;

if (cfv->max > CF_MAX_LIMIT)
   {
   cfv->max = CF_MAX_LIMIT;
   }

if (cfv->error_scale > cfv->max - cfv->min)
   {
   cfv->error_scale = cfv->max - cfv->min;
   }

cfv->origin_x = cfv->margin;
cfv->origin_y = cfv->height+cfv->margin;

cfv->max_x = cfv->margin+cfv->width;
cfv->max_y = cfv->margin;

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

cfv->range = (cfv->max - cfv->min + cfv->error_scale);

if (cfv->error_scale > (cfv->max - cfv->min)*1.5)
   {
   cfv->range = (cfv->max - cfv->min);
   }
else
   {
   cfv->range = (cfv->max - cfv->min + cfv->error_scale);   
   }

cfv->scale_x = (double)cfv->width / (double)CF_MAGDATA;
cfv->scale_y = ((double) cfv->height) / cfv->range;
}

/*******************************************************************/

void Nova_PlotMagQFile(struct CfDataView *cfv,int col1,int col2,int col3)

{ int i,x,y,lx = 0,ly = 0,now,under,over,av = 0;
  double range,sx;
  double low,high,a,s;

lx = 0;
ly = 0;

for (sx = 0; sx < CF_MAGDATA; sx++)
   {
   x = Nova_ViewPortX(cfv,sx);
   y = Nova_ViewPortY(cfv,cfv->data_q[(int)sx],CF_MAGMARGIN);
   a = cfv->data_E[(int)sx];
   s = cfv->bars[(int)sx];

   if (lx == 0 && ly == 0)
      {
      gdImageSetPixel(cfv->im,x,y,col1);
      }
   else
      {
      int width = cfv->max_x / CF_MAGDATA / 3;

      low = a-s;
      high = a+s;

      av = Nova_ViewPortY(cfv,a,CF_MAGMARGIN);
      over = Nova_ViewPortY(cfv,high,CF_MAGMARGIN);
      under = Nova_ViewPortY(cfv,low,CF_MAGMARGIN);

      /* Error bars - note upside down y */

      if (under > cfv->origin_y)
         {
         under = cfv->origin_y;
         }

      if (over < cfv->max_y)
         {
         over = cfv->max_y;   
         }

      // Average line
      
      gdImageSetThickness(cfv->im,1);
      gdImageLine(cfv->im,lx,ly,x,av,col2);

      // Error bars
      
      gdImageSetThickness(cfv->im,width);
      gdImageLine(cfv->im,x,over,x,av,col1);
      gdImageLine(cfv->im,x,av,x,under,col2);
      }
   
   lx = x;
   ly = av;
   }

Nova_DrawMagQAxes(cfv,BLACK);

lx = 0;
ly = 0;

for (sx = 0; sx < CF_MAGDATA; sx++)
   {
   x = Nova_ViewPortX(cfv,sx);
   y = Nova_ViewPortY(cfv,cfv->data_q[(int)sx],CF_MAGMARGIN);
   a = cfv->data_E[(int)sx];
   s = cfv->bars[(int)sx];

   if (lx == 0 && ly == 0)
      {
      gdImageSetPixel(cfv->im,x,y,col1);
      }
   else
      {
      if (cfv->data_q[(int)sx] > cfv->data_E[(int)sx] + cfv->bars[(int)sx])
         {
         gdImageSetThickness(cfv->im,4);
         gdImageLine(cfv->im,lx,ly,x,y,RED);
         }
      else
         {
         gdImageSetThickness(cfv->im,4);
         gdImageLine(cfv->im,lx,ly,x,y,col3);
         }
      }
   
   lx = x;
   ly = y;
   }
}

/***********************************************************/

void Nova_AnalyseMag(char *hostkey,enum observables obs,char *buffer,int bufsize)

{ char work[CF_BUFSIZE];
  struct CfDataView cfv;

Nova_ReadMagTimeSeries(&cfv,hostkey,obs);

snprintf(work,CF_BUFSIZE,"<div id=\"maganalysis\">\n");
Join(buffer,work,bufsize);
   
if (cfv.max - cfv.min < cfv.error_scale * 2)
   {
   snprintf(work,CF_BUFSIZE,"<p><font color=red>The variations you are seeing are not significant.</p>");
   Join(buffer,work,bufsize);
   }

snprintf(work,CF_BUFSIZE,"<table>\n");
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Maximum value </td><td>%lf</td><td>%s</td></tr>\n",cfv.max,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Minimum value </td><td>%lf</td><td>%s</td></tr>\n",cfv.min,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"<tr><td>Average variability </td><td>+/- %lf</td><td>%s</td></tr>\n",cfv.error_scale,UNITS[obs]);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"</table>\n");
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE,"</div>\n");
Join(buffer,work,bufsize);
}

/**********************************************************************/

void Nova_DrawMagQAxes(struct CfDataView *cfv,int col)

{ int hour,x,y;
  double q,dq;
  time_t now;
  int ticksize = cfv->height/50;
  static char *hours[4] = { "t-4 hrs","t-3 hrs","t-2 hrs","t-1 hrs"};

gdImageSetThickness(cfv->im,1);
        
gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->max_x, cfv->origin_y, col);
gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->origin_x, cfv->max_y, col);

gdImageLine(cfv->im, cfv->max_x, cfv->max_y, cfv->origin_x, cfv->max_y, col);
gdImageLine(cfv->im, cfv->max_x, cfv->origin_y, cfv->max_x, cfv->max_y, col);

for (hour = 0; hour < 4; hour++)
   {
   x = cfv->origin_x + hour * cfv->width/4;

   gdImageLine(cfv->im, x, cfv->origin_y-ticksize, x, cfv->max_y, col);
   gdImageString(cfv->im, gdFontGetLarge(),x,cfv->origin_y+2*ticksize,hours[hour],col);
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
      
      gdImageLine(cfv->im, x-2*ticksize, y, cfv->max_x, y, col);
      snprintf(qstr,15,"%.1f",q);
      gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,col);
      }
   }
}


#endif
