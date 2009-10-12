
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

int Nova_ViewLatest(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *host)
    
{ int i,y,hint;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];
  char oldfile[CF_BUFSIZE];

  /* Initialization */

snprintf(newfile,CF_BUFSIZE,"%s_mag.png",filename);
snprintf(oldfile,CF_BUFSIZE,"%s.mag",filename);

if ((stat(oldfile,&s1) != -1) && (stat(newfile,&s2) != -1))
   {
   if (s2.st_mtime > s1.st_mtime)
      {
      /* no changes */
      DATESTAMPS[obs] = s2.st_mtime;
      return true;
      }
   }
 
cfv->title = title;
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   hint = (int)((double)(cfv->height+2*cfv->margin-y)/(cfv->height+2*cfv->margin) * CF_SHADES);
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,GREYS[hint]);
   }

/* Done initialization */

CfOut(cf_verbose,""," -> Looking for %s",oldfile);

if (!Nova_ReadMagTimeSeries(cfv,oldfile))
   {
   CfOut(cf_verbose,"","Aborting %s\n",oldfile);
   return false;
   }

Nova_DrawMagQAxes(cfv,BLACK);
Nova_PlotMagQFile(cfv,LIGHTRED,GREEN,YELLOW);
Nova_Title(cfv,BLUE);

if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_verbose,"fopen","Cannot write %s file\n",newfile);
   return true;
   }
else
   {
   CfOut(cf_verbose,""," -> Making %s\n",newfile);
   }

gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);

Nova_AnalyseMag(cfv,filename,obs,host);

stat(newfile,&s2);
DATESTAMPS[obs] = s2.st_mtime;
return true;
}

/**********************************************************************/
/* Magdata                                                            */
/**********************************************************************/

int Nova_ReadMagTimeSeries(struct CfDataView *cfv, char *name)

{ double range,rx,ry,rq,rs,sx = 0;
  FILE *fp;
  char buffer[CF_BUFSIZE];

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      
  
if ((fp = fopen(name,"r")) == NULL)
   {
   CfOut(cf_verbose,"","Cannot read mag data %s\n",name);
   return false;
   }

for (sx = 0; sx < CF_MAGDATA; sx++)
   {
   rx = rs = ry = 0;

   memset(buffer,0,CF_BUFSIZE);
      
   if (!fgets(buffer,CF_BUFSIZE,fp))
      { 
      return false;
      }

   sscanf(buffer,"%lf %lf %lf %lf",&rx,&ry,&rs,&rq);

   if (rq > cfv->max)
      {
      cfv->max = rq;
      }

   cfv->error_scale = (cfv->error_scale+rs)/2;

   if (rq < cfv->min)
      {
      cfv->min = rq;
      }

   cfv->data_E[(int)sx] = ry;
   cfv->data_q[(int)sx] = rq;
   cfv->bars[(int)sx] = rs;
   }

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

cfv->scale_x = (double)cfv->width / (double)CF_MAGDATA;
cfv->scale_y = ((double) cfv->height) / cfv->range;

fclose(fp);
return true;
}

/**********************************************************************/

void Nova_DrawMagQAxes(struct CfDataView *cfv,int col)

{ int hour,x,y;
  double q,dq;
  time_t now;
  int ticksize = cfv->height/50;
  static char *hours[4] = { "t-4 hrs","t-3 hrs","t-2 hrs","t-1 hrs"};

gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->max_x, cfv->origin_y, col);
gdImageLine(cfv->im, cfv->origin_x, cfv->origin_y, cfv->origin_x, cfv->max_y, col);

for (hour = 0; hour < 4; hour++)
   {
   x = cfv->origin_x + hour * cfv->width/4;
   gdImageLine(cfv->im, x, cfv->origin_y-ticksize, x, cfv->origin_y+ticksize, col);
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

/*******************************************************************/

void Nova_PlotMagQFile(struct CfDataView *cfv,int col1,int col2,int col3)

{ int i,x,y,lx = 0,ly = 0,now,under,over,av;
  double range,sx;
  double low,high,a,s;
 
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
      low = a-s;
      high = (a+s > cfv->origin_y)? cfv->origin_y : a+s;

      av = Nova_ViewPortY(cfv,a,CF_MAGMARGIN);
      over = Nova_ViewPortY(cfv,low,CF_MAGMARGIN);
      under = Nova_ViewPortY(cfv,high,CF_MAGMARGIN);
      
      if (y < (a-s) || ly < (a-s)) /* Mark above 1 dev, note upside down coords +/-  */
         {
         gdImageSetThickness(cfv->im,4);
         gdImageLine(cfv->im,lx,ly,x,y,RED);
         }
      else
         {
         gdImageSetThickness(cfv->im,3);
         gdImageLine(cfv->im,lx,ly,x,y,col3);
         }

      /* Error bars */
      gdImageSetThickness(cfv->im,3);
      gdImageLine(cfv->im,x,under,x,av,col1);
      gdImageLine(cfv->im,x,av,x,over,col2);
      }
   
   lx = x;
   ly = y;
   }
}

/***********************************************************/

void Nova_AnalyseMag(struct CfDataView *cfv,char *name,enum observables obs,char *host)

{ char fname[CF_BUFSIZE],img[CF_BUFSIZE];
  FILE *fp;

  /* First find the variance sigma2 */

snprintf(fname,CF_BUFSIZE,"%s_mag.html",name);
  
if ((fp = fopen(fname,"w")) == 0)
   {
   CfOut(cf_verbose,"","Can't open magnification analysis file %s\n",fname);
   return;   
   }

NovaHtmlHeader(fp,host,STYLESHEET,WEBDRIVER,BANNER);

fprintf(fp,"<h1>Zoom: %s Last four hours</h1>\n",name);

Nova_GraphMagLegend(fp);

snprintf(img,CF_BUFSIZE,"%s_mag.png",name);

fprintf(fp,"<div id=\"graph\">\n");
fprintf(fp,"<a href=\"%s\"><img src=\"%s\" width=\"590\"></a>\n",img,img);
fprintf(fp,"</div>\n");

fprintf(fp,"<div id=\"maganalysis\">\n");

if (cfv->max - cfv->min < cfv->error_scale * 2)
   {
   fprintf(fp,"<p><font color=red>The variations you are seeing are not significant.</p>");
   }

fprintf(fp,"<table>\n");
fprintf(fp,"<tr><td>Maximum value </td><td>%lf</td><td>%s</td></tr>\n",cfv->max,UNITS[obs]);
fprintf(fp,"<tr><td>Minimum value </td><td>%lf</td><td>%s</td></tr>\n",cfv->min,UNITS[obs]);
fprintf(fp,"<tr><td>Average variability </td><td>+/- %lf</td><td>%s</td></tr>\n",cfv->error_scale,UNITS[obs]);
fprintf(fp,"</table>\n");

fprintf(fp,"</div></div>\n");

NovaHtmlFooter(fp,FOOTER);
fclose(fp);
}

#endif
