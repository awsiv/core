
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: histogram.c                                                         */
/*                                                                           */
/*****************************************************************************/

extern int LIGHTRED,YELLOW,WHITE,BLACK,GREEN,BLUE,LIGHTGREY;
extern int GREYS[CF_SHADES];
extern int BLUES[CF_SHADES];

/*****************************************************************************/

#ifdef HAVE_LIBGD

void Nova_ViewHisto(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *host)
    
{ int i,y,hint;
  double frac;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];
  char oldfile[CF_BUFSIZE];
  struct Item *spectrum;

  /* Initialization */

snprintf(newfile,CF_BUFSIZE,"%s_hist.png",filename);
snprintf(oldfile,CF_BUFSIZE,"%s.distr",filename);

if ((cfstat(oldfile,&s1) != -1) && (cfstat(newfile,&s2) != -1))
   {
   if (s2.st_mtime > s1.st_mtime)
      {
      /* no changes */
      return;
      }
   }

cfv->title = title;
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   //hint = (int)((double)(cfv->height+2*cfv->margin-y)/(cfv->height+2*cfv->margin) * CF_SHADES);
   //gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,GREYS[hint]);
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,LIGHTGREY);
   }

/* Done initialization */

CfOut(cf_verbose,""," -> Looking for histogram %s",oldfile);

if (!Nova_ReadHistogram(cfv,oldfile))
   {
   return;
   }

spectrum = Nova_AnalyseHistogram(cfv,filename,obs,host);
Nova_PlotHistogram(cfv,BLUES,spectrum);
Nova_Title(cfv,BLUE);
Nova_DrawHistoAxes(cfv,BLACK);
    
if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot write %s file\n",newfile);
   return;
   }

gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);
DeleteItemList(spectrum);
}

/**********************************************************************/

int Nova_ReadHistogram(struct CfDataView *cfv, char *name)

{ double rx,ry,rs,sx = 0;
  FILE *fp;
  char buffer[CF_BUFSIZE];

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

if ((fp = fopen(name,"r")) == NULL)
   {
   CfOut(cf_verbose,"","Can't open histogram %s\n",name);
   return false;
   }

for (sx = 0; sx < CF_GRAINS; sx++)
   {
   rx = ry = 0;

   memset(buffer,0,CF_BUFSIZE);
   
   if (!fgets(buffer,CF_BUFSIZE,fp))
      {
      return false;
      }
   
   sscanf(buffer,"%lf %lf",&rx,&ry);

   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }

   cfv->data_E[(int)sx] = ry;
   }

fclose(fp);
return true;
}

/**********************************************************************/

void Nova_DrawHistoAxes(struct CfDataView *cfv,int col)

{ int origin_x = cfv->margin;
  int origin_y = cfv->height+cfv->margin;
  int max_x = cfv->margin+cfv->width;
  int max_y = cfv->margin;
  int sigma;
  int x,y;
  int ticksize = cfv->height/50;
  static char *grains[CF_GRAINS/4] = {"-2 sig","-sig","AV","+sig","+2 sig"};
  
gdImageLine(cfv->im, origin_x, origin_y, max_x, origin_y, col);
gdImageLine(cfv->im, origin_x, origin_y, origin_x, max_y, col);

/* from -2 to +2 sigma  */

for (sigma = 0; sigma < 5; sigma++)
   {
   x = origin_x + (CF_GRAINS/4) * sigma * (int)((double)cfv->width/(double)CF_GRAINS+0.5);

   gdImageLine(cfv->im, x, origin_y-ticksize, x, origin_y+ticksize, col);
   
   gdImageString(cfv->im, gdFontGetLarge(),x,origin_y+2*ticksize,grains[sigma],col);
   }
}

/*******************************************************************/

void Nova_PlotHistogram(struct CfDataView *cfv,int *blues,struct Item *spectrum)

{
 int origin_x = cfv->margin;
 int origin_y = cfv->height+cfv->margin;
 int max_x = cfv->margin+cfv->width;
 int max_y = cfv->margin;
 int i,x,y,dev;
 double range;
 double rx,ry,rs,sx = 0,s;
 double scale_x = ((double)cfv->width /(double)CF_GRAINS);
 double scale_y;
 double low,high;
 double xfill;
 int col = 0;
 int lightred = gdImageColorAllocate(cfv->im, 255, 150, 150);
 struct Item *ip;

if (cfv->max == 0)
   {
   return;
   }

range = (cfv->max - cfv->min + cfv->error_scale);
scale_y = (double) cfv->height / range;

gdImageSetThickness(cfv->im,1);

// First plot average

for (sx = 0; sx < CF_GRAINS; sx++)
   {
   x = origin_x + (sx * scale_x);
   y = origin_y  - (int)((cfv->data_E[(int)sx]-cfv->min + cfv->error_scale) * scale_y);
   s = cfv->bars[(int)sx] * scale_y;
   
   low = y-s;
   high = (y+s > origin_y)? origin_y : y+s;
   
   //dev = 50 - abs(25 + ((double)50/(double)CF_GRAINS)*(sx-CF_GRAINS));

   dev = sx * (double)CF_SHADES/(double)CF_GRAINS;
   xfill = x;
   
   for (col = 0; col < CF_SHADES; col++)
      {
      xfill += (double)cfv->width/(double)CF_GRAINS/((double)CF_SHADES);
      gdImageLine(cfv->im,xfill,origin_y,xfill,y,blues[col]);
      }

   //gdImageFilledRectangle(cfv->im,x,origin_y,x+1,y,lightred);
   }

for (ip = spectrum; ip != NULL; ip=ip->next)
   {
   sx = ip->counter;
   x = origin_x + (sx * scale_x);
   y = origin_y  - (int)((cfv->data_E[(int)sx]-cfv->min + cfv->error_scale) * scale_y);
   s = cfv->bars[(int)sx] * scale_y;
   
   low = y-s;
   high = (y+s > origin_y)? origin_y : y+s;
   
   dev = sx * (double)CF_SHADES/(double)CF_GRAINS;
   xfill = x;
   
   for (col = 0; col < CF_SHADES; col++)
      {
      xfill += (double)cfv->width/(double)CF_GRAINS/((double)CF_SHADES);
      gdImageLine(cfv->im,xfill,origin_y,xfill,y,GREEN);
      }
   }

gdImageSetThickness(cfv->im,4);
gdImageLine(cfv->im,origin_x+32*scale_x,origin_y,origin_x+cfv->width/2,cfv->max_y,lightred);
}

/*******************************************************************/

struct Item *Nova_AnalyseHistogram(struct CfDataView *cfv,char *name,enum observables obs,char *host)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char fname[CF_BUFSIZE],img[CF_BUFSIZE],output[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct Item *maxima = NULL;
  FILE *fp;

  /* First find the variance sigma2 */

snprintf(fname,CF_BUFSIZE,"%s_hist.html",name);
  
if ((fp = fopen(fname,"w")) == 0)
   {
   CfOut(cf_verbose,"","Can't open histogram analysis file %s\n",fname);
   return NULL;   
   }

NovaHtmlHeader(fp,host,STYLESHEET,WEBDRIVER,BANNER);
fprintf(fp,"<h1>Spectral analysis of %s</h1>",name);

fprintf(fp,"<div id=\"legend\">\n");

CfOut(cf_verbose,""," -> Looking for maxima in %s\n",name);

snprintf(img,CF_BUFSIZE,"%s_hist.png",name);

fprintf(fp,"<div id=\"graph\">\n");
fprintf(fp,"<a href=\"%s\"><img src=\"%s\" width=\"590\"></a>\n",img,img);
fprintf(fp,"</div>\n");

fprintf(fp,"<div id=\"histoanalysis\">\n");

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv->data_E[(int)sx];
   delta = cfv->data_E[(int)sx] - cfv->data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

fprintf(fp,"<p> Maximum observed %s = %.2lf\n",name,Q_MAX);
fprintf(fp,"<p> Minimum observed %s = %.2lf\n",name,Q_MIN);
fprintf(fp,"<p> Approximate mean value over time = %.2lf\n",Q_MEAN);
fprintf(fp,"<p> Approximate standard deviation time = %.2lf\n",Q_SIGMA);

fprintf(fp,"<ol>\n");

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv->data_E[(int)sx];
   delta = cfv->data_E[(int)sx] - cfv->data_E[(int)(sx-1)];

   above_noise = (delta*delta > sigma2) * sensitivity_factor;
   
   if (above_noise)
      {
      new_gradient = delta;

      if (new_gradient < 0 && past_gradient >= 0)
         {
         max++;

         fprintf(fp,"<p>Spectral mode %d, with  peak found at %.0lf/%.0lf grains (conversion &asymp; %.2lf &plusmn; %.2lf)\n",max,sx-1,(double)CF_GRAINS,Q_MEAN,Q_SIGMA);

         snprintf(output,CF_BUFSIZE-1,"key-%f",sx);
         AppendItem(&maxima,output,NULL);
         SetItemListCounter(maxima,output,sx-1);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            fprintf(fp,"<ul><li>Red-shifted, i.e. shows retardation process</li></ul>\n");
            fprintf(fp,"<div id = \"explain\"><table><tr><td><h4>What does this mean?</h4>"
                    "<p>If the distribution is skewed, it has a long ramp, indicating"
                    "a possible resource ceiling, a well-utilized system."
                    "Or there could be outliers of low value, because data are incomplete."
                    "Finally, it can indicate that this resource process is declining."
                    "</td></tr></table></div>\n");
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            fprintf(fp,"<ul><li>Blue-shifted, i.e. shows acceleration process</li></ul>\n");
            fprintf(fp,"<div id = \"explain\"><table><tr><td><h4>What does this mean?</h4>"
                    "<p>If the distribution is skewed, it has a long tail, indicating"
                    "plenty of resources, or an under-used system."
                    "Or there could be outliers of low value, because data are incomplete."
                    "Finally, it can indicate that this resource process is declining."
                    "</td></tr></table></div>\n");
            }

         }
      }
   
   past_gradient = new_gradient;
   }

fprintf(fp,"</ol>\n");      

fprintf(fp,"<p>Spectrum seems to be %d-modal (within the margins of uncertainty)</p>\n",max);


fprintf(fp,"</div></div>\n");

NovaHtmlFooter(fp,FOOTER);
fclose(fp);
return maxima;
}

#endif
