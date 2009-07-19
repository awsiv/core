
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


/*****************************************************************************/
/*                                                                           */
/* File: aggregation.c                                                       */
/*                                                                           */
/* Created: Sun Jul 19 09:57:14 2009                                         */
/*                                                                           */
/*****************************************************************************/

void Nova_Aggregate()

{ struct CfDataView cfv;
  struct stat sb;

if (strlen(AGGREGATION) == 0)
   {
   return;
   }

if (stat(AGGREGATION,&sb) == -1)
   {
   return;
   }

if (!S_ISDIR(sb.st_mode))
   {
   return;
   }

Nova_BuildGraphs(&cfv); 
}

/*****************************************************************************/

void Nova_MagProbe()

{ char filename[CF_BUFSIZE],buffer[CF_BUFSIZE];
  struct stat sb;

snprintf(filename,CF_BUFSIZE-1,"%s/reports",CFWORKDIR);
  
if (stat(filename,&sb) != -1)
   {
   if (S_ISDIR(sb.st_mode))
      {
      Nova_PackNervBundle();
      }
   }

if (strlen(AGGREGATION) == 0)
   {
   return;
   }

if (stat(AGGREGATION,&sb) != -1)
   {
   if (S_ISDIR(sb.st_mode))
      {
      Nova_UnPackNervBundle();
      }
   }
}

/*****************************************************************************/

void Nova_PackNerveBundle()
    
{ FILE *fin,*fout;
  int i;
  char filename[CF_BUFSIZE],buffer[CF_BUFSIZE],name[CF_BUFSIZE];
  static char *slow[] =
   {
   "performance.html",
   "classes.html",
   "hashes.html",
   "audit.html",
   "monitor_summary.html",
   "promises.cf.html",
   "setuid.html",
   "file_changes.html",
   "compliance.html",
   "software_packages.html",
   "software_patches_avail.html",
   "software_patch_status.html",
   NULL
   // OBS.E-sigma,OBS.distr,OBS.yr
   };

  static char *fast[] =
   {
   "variables.html",
   "lastseen.html",
   "promise_compliance.html",
   NULL
   //"../ts_key",
   // OBS.mag   
   };

/* First the slowly changing (adiabatic variables) */
  
snprintf(filename,CF_BUFSIZE-1,"%s/mean_field.nov",CFWORKDIR);

if ((fout = fopen(filename,"w")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open nerve bundle");
   return;
   }

for (i = 0; slow[i] != NULL; i++)
   {
   snprintf(filename,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,slow[i]);

   if ((fin = fopen(filename,"w")) == NULL)
      {
      CfOut(cf_verbose,"fopen"," !! Cannot open nerve bundle");
      continue;
      }

   fprintf(fout,"%%CFENGINE: %s\n",slow[i]);
   
   while (!feof(fin))
      {
      fgets(buffer,CF_BUFSIZE-1,fin);

      if (feof(fin))
         {
         break;
         }
      
      fwrite(buffer,strlen(buffer),1,fout);
      }
   
   fclose(fin);   
   }

/* Now semantic key */

snprintf(filename,CF_BUFSIZE-1,"%s/state/ts_key",CFWORKDIR);

if ((fin = fopen(filename,"w")) != NULL)
   {
   CfOut(cf_verbose,"fopen"," !! Cannot open nerve bundle");

   fprintf(fout,"%%CFENGINE: ts_key\n");
   
   while (!feof(fin))
      {
      fgets(buffer,CF_BUFSIZE-1,fin);
      
      if (feof(fin))
         {
         break;
         }
      
      fwrite(buffer,strlen(buffer),1,fout);
      }
   
   fclose(fin);   
   }

/* Now the monitor data */

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   LookUpClassName(i,name);

   if (strcmp(name,"spare") == 0)
      {
      continue;
      }

   snprintf(filename,CF_BUFSIZE-1,"%s.E-sigma",name);
   Nova_WriteSignalData(fout,filename);
   snprintf(filename,CF_BUFSIZE-1,"%s.q",name);
   Nova_WriteSignalData(fout,filename);
   snprintf(filename,CF_BUFSIZE-1,"%s.hist",name);
   Nova_WriteSignalData(fout,filename);
   snprintf(filename,CF_BUFSIZE-1,"%s_0.yr",name);
   Nova_WriteSignalData(fout,filename);
   snprintf(filename,CF_BUFSIZE-1,"%s_1.yr",name);
   Nova_WriteSignalData(fout,filename);
   snprintf(filename,CF_BUFSIZE-1,"%s_2.yr",name);
   Nova_WriteSignalData(fout,filename);
   }

fclose(fout);

/* First the fluctuating (variables) */

snprintf(filename,CF_BUFSIZE-1,"%s/fluctuations.nov",CFWORKDIR);

if ((fout = fopen(filename,"w")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open nerve bundle");
   return;
   }

for (i = 0; fast[i] != NULL; i++)
   {
   snprintf(filename,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,fast[i]);

   if ((fin = fopen(filename,"w")) == NULL)
      {
      CfOut(cf_verbose,"fopen"," !! Cannot open nerve bundle");
      continue;
      }

   fprintf(fout,"%%CFENGINE: %s\n",fast[i]);
   
   while (!feof(fin))
      {
      fgets(buffer,CF_BUFSIZE-1,fin);

      if (feof(fin))
         {
         break;
         }
      
      fwrite(buffer,strlen(buffer),1,fout);
      }
   
   fclose(fin);   
   }

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   LookUpClassName(i,name);

   if (strcmp(name,"spare") == 0)
      {
      continue;
      }

   snprintf(filename,CF_BUFSIZE-1,"%s.mag",name);
   Nova_WriteSignalData(fout,filename);
   }

fclose(fout);
}

/*****************************************************************************/

void Nova_UnPackNerveBundle(char *hostname)
    
{ FILE *fin = NULL,*fout = NULL;
  int i;
  struct stat sb1,sb2;
  char filename[CF_BUFSIZE],buffer[CF_BUFSIZE];

snprintf(filename,CF_BUFSIZE-1,"%s/%s/fluctuations.nov",AGGREGATION,hostname);

if (stat(filename,&sb1) == -1)
   {
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"%s/%s/rootprocs.mag",AGGREGATION,hostname);

if (stat(filename,&sb2) == -1)
   {
   }
else
   {
   if (sb2.st_mtime > sb1.st_mtime)
      {
      CfOut(cf_verbose,""," -> No mag updates for %s",hostname);
      return;
      }
   }

snprintf(filename,CF_BUFSIZE-1,"%s/%s/fluctuations.nov",AGGREGATION,hostname);
  
if ((fin = fopen(filename,"r")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open nerve bundle %s",filename);
   return;
   }

while (!feof(fin))
   {
   fgets(buffer,CF_BUFSIZE-1,fin);
      
   if (feof(fin))
      {
      break;
      }

   filename[0] = '\0';
   sscanf(buffer,"%%CFENGINE: %s\n",filename);

   if (strlen(filename) > 0)
      {
      if (fout)
         {
         fclose(fout);
         }

      snprintf(buffer,CF_BUFSIZE-1,"%s/%s",AGGREGATION,filename);
      
      if ((fout = fopen(buffer,"w")) == NULL)
         {
         CfOut(cf_error,"fopen"," !! Cannot open mag fibre %s",buffer);
         return;
         }      
      }

   fwrite(buffer,strlen(buffer),1,fout);
   }

if (fout)
   {
   fclose(fout);
   }

fclose(fin);   

/* Now the adiabatic variation, if updated */

snprintf(filename,CF_BUFSIZE-1,"%s/%s/mean_field.nov",AGGREGATION,hostname);

if (stat(filename,&sb1) == -1)
   {
   return;
   }

snprintf(filename,CF_BUFSIZE-1,"%s/%s/rootprocs.q",AGGREGATION,hostname);

if (stat(filename,&sb2) == -1)
   {
   }
else
   {
   if (sb2.st_mtime > sb1.st_mtime)
      {
      CfOut(cf_verbose,""," -> No basal updates for %s",hostname);
      return;
      }
   }

snprintf(filename,CF_BUFSIZE-1,"%s/%s/mean_field.nov",AGGREGATION,hostname);
  
if ((fin = fopen(filename,"r")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open mean bundle %s",filename);
   return;
   }

while (!feof(fin))
   {
   fgets(buffer,CF_BUFSIZE-1,fin);
      
   if (feof(fin))
      {
      break;
      }

   filename[0] = '\0';
   sscanf(buffer,"%%CFENGINE: %s\n",filename);

   if (strlen(filename) > 0)
      {
      if (fout)
         {
         fclose(fout);
         }

      snprintf(buffer,CF_BUFSIZE-1,"%s/%s",AGGREGATION,filename);
      
      if ((fout = fopen(buffer,"w")) == NULL)
         {
         CfOut(cf_error,"fopen"," !! Cannot open nerve fibre %s",buffer);
         return;
         }      
      }

   fwrite(buffer,strlen(buffer),1,fout);
   }

if (fout)
   {
   fclose(fout);
   }

fclose(fin);   
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_WriteSignalData(FILE *fout,char *name)

{ double x,y;
  double count = 0, non_zero = 0;
  char buffer[CF_BUFSIZE],filename[CF_BUFSIZE];
  FILE *fin;
  
snprintf(filename,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,name);
  
if ((fin = fopen(filename,"r")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open nerve bundle %s",filename);
   return;
   }

while(!feof(fin))
   {
   fgets(buffer,CF_BUFSIZE-1,fin);
   count++;
   sscanf(buffer,"%lf %lf",&x,&y);

   if (y > 0)
      {
      non_zero++;
      }
   }

fclose(fin);

/* don't bother updating anything that is less than 10% developed */

if (non_zero/count < 0.1)
   {
   return;
   }

fprintf(fout,"%%CFENGINE: %s\n",filename);
   
if ((fin = fopen(filename,"r")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Cannot open nerve bundle %s",filename);
   return;
   }

while(!feof(fin))
   {
   fgets(buffer,CF_BUFSIZE-1,fin);

   if (feof(fin))
      {
      break;
      }

   fwrite(buffer,strlen(buffer),1,fout);
   }

fclose(fin);
}

