
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

int LIGHTRED,YELLOW,WHITE,BLACK,RED,GREEN,BLUE,LIGHTGREY;
int GREYS[CF_SHADES];
int BLUES[CF_SHADES];

/*****************************************************************************/

void Nova_BuildGraphs(struct CfDataView *cfv)

{ DIR *dirh;
  struct dirent *dirp;
  int i, count = 0;
  char main[CF_BUFSIZE],hostdir[CF_BUFSIZE],index[16];
  FILE *fout;
  struct stat sb;
  struct Item *serverlist = NULL,*ip;
  struct Item *eliminate = NULL;

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
   if (stat(dirp->d_name,&sb) == -1)
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

/* Create host pages first */

for (ip = serverlist; ip != NULL; ip=ip->next)
   {
   if (chdir(ip->name) == -1)
      {
      printf("Could not write to directory %s\n",ip->name);
      continue;
      }
   
   for (i = 0; i < CF_OBSERVABLES; i++)
      {
      if (strcmp(OBS[i][0],"spare") == 0)
         {
         continue;
         }

      if (!Nova_ViewWeek(cfv,OBS[i][0],OBS[i][1],i))
         {
         snprintf(index,15,"%d",i);
         //PrependItem(&eliminate,index,NULL);
         //continue;
         }

      CfOut(cf_verbose,"","Processing host source %s / %s",ip->name,OBS[i][0]);

      Nova_ViewLatest(cfv,OBS[i][0],OBS[i][1],i);
      Nova_ViewHisto(cfv,OBS[i][0],OBS[i][1],i);
      }

   Nova_MainPage(ip->name,eliminate);
   Nova_OtherPages(ip->name,eliminate);
   DeleteItemList(eliminate);
   eliminate = NULL;
   chdir("..");
   }
}

/*****************************************************************************/

void Nova_Title(struct CfDataView *cfv,int col)

{  
gdImageString(cfv->im, gdFontGetGiant(),
              cfv->im->sx / 2 - (strlen(cfv->title) * gdFontGetGiant()->w / 2),
              cfv->margin/2,
              cfv->title,col);
}

/*****************************************************************************/

void Nova_MakePalette(struct CfDataView *cfv)

{ int i,hint,r,g,b;
  int startgrey = 120,startblue = 150, startgreen = 80;

for (i = 0; i < CF_SHADES; i++)
   {
   hint = startgrey + (int)((255.0-(double)startgrey)/(double)CF_SHADES * (double)i);
   GREYS[i] = gdImageColorAllocate(cfv->im,hint,hint,hint);
   }

for (i = 0; i < CF_SHADES; i++)
   {
   r = (int)((255.0)/(double)CF_SHADES * (double)i);
   
   g = startgreen +
       (int)( (255.0-(double)startgreen)/((double)CF_SHADES*1.5) * (double)i*1.5);

   b = startblue + (int)((255.0-(double)startblue)/(double)CF_SHADES * (double)i);
   BLUES[i] = gdImageColorAllocate(cfv->im,r,g,b);
   }

WHITE    = gdImageColorAllocate(cfv->im, 255, 255, 255);
LIGHTGREY= gdImageColorAllocate(cfv->im, 220, 220, 220);
BLACK    = gdImageColorAllocate(cfv->im, 0, 0, 0);
GREEN    = gdImageColorAllocate(cfv->im, 0, 200, 0);
BLUE     = gdImageColorAllocate(cfv->im, 50, 100, 100);
YELLOW   = gdImageColorAllocate(cfv->im, 200, 255, 0);
LIGHTRED = gdImageColorAllocate(cfv->im, 255, 150, 150);
RED      = gdImageColorAllocate(cfv->im, 255, 0, 0);
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
  
sprintf(str,"%s",ctime(&now));

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

