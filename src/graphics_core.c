
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
int GREENS[CF_SHADES];
int BLUES[CF_SHADES];
int YELLOWS[CF_SHADES];
int PINKS[CF_SHADES];
int BROWNS[CF_SHADES];

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
WHITE = gdImageColorAllocate(cfv->im, 255, 255, 255);
BLUE  = gdImageColorAllocate(cfv->im, 50, 100, 100);
LIGHTRED = gdImageColorAllocate(cfv->im, 189, 58, 43);

for (i = 0; i < CF_SHADES; i++)
   {
   r = (int)((100.0)/(double)CF_SHADES * (double)i);
   g = startgreen + (int)( (100.0-(double)startgreen)/((double)CF_SHADES*1.5) * (double)i*1.5);
   b = startblue + (int)((255.0-(double)startblue)/(double)CF_SHADES * (double)i);
   BLUES[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

// Room for a about 16 pixels on the compliance graph width / span (scorecards.c)

for (i = 0; i < 16; i++)
   {
   r = 40+(int)((115-40)/(double)16 * (double)i);
   g = 100 + (int)( (148-100)/((double)16) * (double)i);
   b = 60 + (int)((103-(double)60)/(double)16 * (double)i);
   GREENS[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

RED      = gdImageColorAllocate(cfv->im, 208, 45, 72);
YELLOW   = gdImageColorAllocate(cfv->im, 242, 238, 134);
GREEN    = gdImageColorAllocate(cfv->im, 115, 177, 103);

ORANGE   = gdImageColorAllocate(cfv->im, 223,149,0);
LIGHTGREY= gdImageColorAllocate(cfv->im, 75, 75, 66);
BACKGR   = gdImageColorAllocate(cfv->im,225, 225, 227); // Background
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
  char font1[CF_MAXVARSIZE] = {0};
  char *font2 = "Vera";
  char *font3 = "DejaVuSans.ttf";
  char *font;
  int brect[8];
  double size = 8.0;


snprintf(font1,sizeof(font1),"%s/fonts/DejaVuSans.ttf", CFWORKDIR);
MapName(font1);
  
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

font = font1;
err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

if (err)
   {
   font = font2;
   err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);

   if (err)
      {
      font = font3;
      err = gdImageStringFT(NULL,&brect[0],0,font,size,0.,0,0,ps);
      
      if (err)
	{
	CfOut(cf_error, "", "!! Font rendering failure: %s", err);
	}
      }
   }

// Plus y is now downward

gdImageStringFT(cfv->im,&brect[0],colour,font,size,0.0,x,y,ps);
}

