
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

extern int LIGHTRED,YELLOW,WHITE,BLACK,GREEN,BLUE,LIGHTGREY,ORANGE,BACKGR;
extern int BLUES[];

/*****************************************************************************/

#ifdef HAVE_LIBGD

int Nova_ViewHisto(struct CfDataView *cfv,char *keyhash,enum observables obs)
    
{ int i,y,hint;
  double frac;
  FILE *fout;
  struct stat s1,s2;
  char newfile[CF_BUFSIZE];
  struct Item *spectrum;

  /* Initialization */

snprintf(newfile,CF_BUFSIZE,"%s/hub/%s/%s_hist.png",cfv->docroot,keyhash,OBS[obs][0]);
MakeParentDirectory(newfile,true);

cfv->title = OBS[obs][1];
cfv->im = gdImageCreate(cfv->width+2*cfv->margin,cfv->height+2*cfv->margin);
Nova_MakePalette(cfv);

for (y = 0; y < cfv->height+2*cfv->margin; y++)
   {
   gdImageLine(cfv->im,0,y,cfv->width+2*cfv->margin,y,BACKGR);
   }

/* Done initialization */

if (!Nova_ReadHistogram(cfv,keyhash,obs))
   {
   return false;
   }

spectrum = Nova_MapHistogram(cfv,keyhash,obs);
Nova_PlotHistogram(cfv,BLUES,spectrum);
Nova_Title(cfv,BLUE);
Nova_DrawHistoAxes(cfv,BLACK);
    
if ((fout = fopen(newfile, "wb")) == NULL)
   {
   CfOut(cf_error,"fopen","Cannot write %s file\n",newfile);
   return false;
   }

gdImagePng(cfv->im, fout);
fclose(fout);
gdImageDestroy(cfv->im);
DeleteItemList(spectrum);
return true;
}

/**********************************************************************/

int Nova_ReadHistogram(struct CfDataView *cfv,char *hostkey,enum observables obs)

{ double rx,ry;
 int i,have_data = false;
  mongo_connection dbconn;
  double histo[CF_GRAINS];

if (!CFDB_Open(&dbconn, "127.0.0.1",CFDB_PORT))
   {
   CfOut(cf_verbose,"", "!! Could not open connection to report database");
   return false;
   }

CFDB_QueryHistogram(&dbconn,hostkey,obs,histo);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   } 

cfv->max = 0;
cfv->min = 99999;
cfv->error_scale = 0;      

for (i = 0; i < CF_GRAINS; i++)
   {
   ry = histo[i];

   if (ry > 1)
      {
      have_data = true;
      }
   
   if (ry > cfv->max)
      {
      cfv->max = ry;
      }

   if (ry < cfv->min)
      {
      cfv->min = ry;
      }

   cfv->data_E[i] = ry;
   }

if (cfv->max == cfv->min)
   {
   have_data = false;
   }

return have_data;
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
 double range,dq,q,ticksize = 5;;
 double rx,ry,rs,sx = 0,s;
 double scale_x = ((double)cfv->width /(double)CF_GRAINS);
 double scale_y = 10.0;
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

// Make 5 gradations

gdImageSetThickness(cfv->im,1);
dq = cfv->range/5.0;

if (dq < 0.001)
   {
   char qstr[16];

   q = cfv->max;
   x = Nova_ViewPortX(cfv,0);
   y = Nova_ViewPortY(cfv,q,CF_MAGMARGIN);

   gdImageLine(cfv->im, x-2*ticksize, y, cfv->max_x, y,BLACK);
   snprintf(qstr,15,"%.1f",q);
   gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,BLACK);
   }
else
   {
   for (q = cfv->min; q <= cfv->min+cfv->range; q += dq)
      {
      char qstr[16];

      x = Nova_ViewPortX(cfv,0);
      y = Nova_ViewPortY(cfv,q,CF_MAGMARGIN);
      
      gdImageLine(cfv->im, x-2*ticksize, y, cfv->max_x, y,BLACK);
      snprintf(qstr,15,"%.1f",q);
      gdImageString(cfv->im, gdFontGetLarge(),x-6*ticksize,y,qstr,BLACK);
      }
   }

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

struct Item *Nova_MapHistogram(struct CfDataView *cfv,char *keyhash,enum observables obs)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char fname[CF_BUFSIZE],img[CF_BUFSIZE],output[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct Item *maxima = NULL;

  /* First find the variance sigma2 */

CfOut(cf_verbose,""," -> Looking for maxima in %s\n",OBS[obs][0]);

snprintf(img,CF_BUFSIZE,"%s/hub/%s/%s_hist.png",cfv->docroot,keyhash,OBS[obs][0]);

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv->data_E[(int)sx];
   delta = cfv->data_E[(int)sx] - cfv->data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

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

         snprintf(output,CF_BUFSIZE-1,"key-%f",sx);
         AppendItem(&maxima,output,NULL);
         SetItemListCounter(maxima,output,sx-1);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            }
         }
      }
   
   past_gradient = new_gradient;
   }
return maxima;
}

/*******************************************************************/

void Nova_AnalyseHistogram(char *docroot,char *keyhash,enum observables obs,char *buffer,int bufsize)

{ double sx, q, delta, sum = 0, sigma2;
  int new_gradient = 0, past_gradient = 0, max = 0;
  int redshift = 0, blueshift = 0;
  int above_noise = false;
  char work[CF_BUFSIZE];
  double sensitivity_factor = 1.2;
  struct CfDataView cfv;

cfv.height = 300;
cfv.width = 700; //(7*24*2)*2; // halfhour
cfv.margin = 50;
cfv.docroot = docroot;

Nova_ViewHisto(&cfv,keyhash,obs);

snprintf(work,CF_BUFSIZE-1,"<div id=\"histoanalysis\">\n");
Join(buffer,work,bufsize);

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];
   sum += delta*delta;
   }

sigma2 = sum / (double)CF_GRAINS;

snprintf(work,CF_BUFSIZE-1,"<p> Maximum observed %s = %.2lf\n",OBS[obs][0],cfv->max);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"<p> Minimum observed %s = %.2lf\n",OBS[obs][0],cfv->min);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"<p> Approximate mean value over time = %.2lf\n",Q_MEAN);
Join(buffer,work,bufsize);
snprintf(work,CF_BUFSIZE-1,"<p> Approximate standard deviation time = %.2lf\n",Q_SIGMA);
Join(buffer,work,bufsize);

snprintf(work,CF_BUFSIZE-1,"<ol>\n");
Join(buffer,work,bufsize);

for (sx = 1; sx < CF_GRAINS; sx++)
   {
   q = cfv.data_E[(int)sx];
   delta = cfv.data_E[(int)sx] - cfv.data_E[(int)(sx-1)];

   above_noise = (delta*delta > sigma2) * sensitivity_factor;
   
   if (above_noise)
      {
      new_gradient = delta;

      if (new_gradient < 0 && past_gradient >= 0)
         {
         max++;

         snprintf(work,CF_BUFSIZE-1,"<p>Spectral mode %d, with  peak found at %.0lf/%.0lf grains (conversion &asymp; %.2lf &plusmn; %.2lf)\n",max,sx-1,(double)CF_GRAINS,Q_MEAN,Q_SIGMA);
         Join(buffer,work,bufsize);

         if (sx < ((double)CF_GRAINS)/2.0 - 1.0)
            {
            redshift++;
            snprintf(work,CF_BUFSIZE-1,"<ul><li>Red-shifted, typically shows retardation process</li></ul>\n");
            Join(buffer,work,bufsize);
            
            snprintf(work,CF_BUFSIZE-1,"<div id = \"explain\"><table><tr><td><h4>What does this mean?</h4>"
                    "<p>If the distribution is skewed, it has a long ramp, indicating"
                    "a possible resource ceiling, a well-utilized system."
                    "Or there could be outliers of low value, because data are incomplete."
                    "Finally, it can indicate that this resource process is declining."
                    "</td></tr></table></div>\n");
            Join(buffer,work,bufsize);
            }
         else if (sx > ((double)CF_GRAINS)/2.0 + 1.0)
            {
            blueshift++;
            snprintf(work,CF_BUFSIZE-1,"<ul><li>Blue-shifted, typically shows acceleration process</li></ul>\n");
            Join(buffer,work,bufsize);

            snprintf(work,CF_BUFSIZE-1,"<div id = \"explain\"><table><tr><td><h4>What does this mean?</h4>"
                    "<p>If the distribution is skewed, it has a long tail, indicating"
                    "plenty of resources, or an under-used system."
                    "Or there could be outliers of low value, because data are incomplete."
                    "Finally, it can indicate that this resource process is declining."
                    "</td></tr></table></div>\n");
            Join(buffer,work,bufsize);            
            }

         }
      }
   
   past_gradient = new_gradient;
   }

snprintf(work,CF_BUFSIZE-1,"</ol>\n");      
Join(buffer,work,bufsize);

snprintf(work,CF_BUFSIZE-1,"<p>Spectrum seems to be %d-modal (within the margins of uncertainty)</p>\n",max);
Join(buffer,work,bufsize);

snprintf(work,CF_BUFSIZE-1,"</div>\n");
Join(buffer,work,bufsize);
}

#endif
