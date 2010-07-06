/*****************************************************************************/
/*                                                                           */
/* File: dataunpack.c                                                        */
/*                                                                           */
/* Created: Wed Jun 16 14:26:50 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_UnPackPerformance(struct Item *data)

{ struct Item *ip;
  time_t t;
  char eventname[CF_MAXVARSIZE];
  double measure = 0,average = 0,dev = 0;

CfOut(cf_verbose,""," -> Performance data ...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   eventname[0] = '\0';
   sscanf(ip->name,"%ld,%lf,%lf,%lf,%255s\n",&t,&measure,&average,&dev,eventname);
   printf("Performance of \"%s\" is %.4lf (av %.4lf +/- %.4lf) measured at %s",eventname,measure,average,dev,cf_ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackClasses(struct Item *data)

{ struct Item *ip;
  char name[CF_MAXVARSIZE];
  time_t t;
  double q = 0, dev = 0;

CfOut(cf_verbose,""," -> Class data .................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   sscanf(ip->name,"%[^,],%ld,%7.4lf,%7.4lf\n",name,&t,&q,&dev);
   printf("Class: \"%s\" seen with probability %.4lf +- %.4lf last seen at %s",name,q,dev,cf_ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackSetuid(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> setuid data ......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   printf("Set-uid program: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackFileChanges(struct Item *data)

{ struct Item *ip;
  char name[CF_MAXVARSIZE],date[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> File change data....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   sscanf(ip->name,"%255[^,],%255[^\n]",name,date);
   printf("File-change event: in %s at %s\n",name,date);
   }
}

/*****************************************************************************/

void Nova_UnPackDiffs(struct Item *data)

{ struct Item *ip;
  char name[CF_MAXVARSIZE],t[CF_MAXVARSIZE],change[CF_BUFSIZE];

CfOut(cf_verbose,""," -> File diff data...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   sscanf(ip->name,"%[^|]|%[^|],%[^\0]",name,&t,&change);
   printf("Change-diff: in file %s at %s begin %s \nend\n",name,t,change);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorWeek(struct Item *data)

{ struct Item *ip;
  int observable;
  double q,e,dev;
  char t[CF_TIME_SIZE];

CfOut(cf_verbose,""," -> Monitor weekly data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      memset(t,0,CF_TIME_SIZE);
      sscanf(ip->name+3,"%31[^\n]",t);
      continue;
      }

   // Extract records
   
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf\n",&observable,&q,&e,&dev);
   printf("Week-obs %d @ %s: %.2lf,%.2lf,%.2lf\n",observable,t,q,e,dev);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorMag(struct Item *data)

{ struct Item *ip;
  time_t t = 0;
  int observable;
  double q,e,dev;

CfOut(cf_verbose,""," -> Monitor magnified data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      sscanf(ip->name+3,"%ld",&t);
      continue;
      }

   // Extract records
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf\n",&observable,&q,&e,&dev);
   printf("Mag-obs %d: %.2lf,%.2lf,%.2lf measured at %s",observable,q,e,dev,cf_ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorHist(struct Item *data)

{ struct Item *ip;
  int weekly[CF_OBSERVABLES][CF_GRAINS];
  int i = 0,j,k;
  char *sp;
 
CfOut(cf_verbose,""," -> Monitor histogram data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sp = ip->name;
   
   sscanf(ip->name,"%d",&i);
   
   while (*(++sp) != ':')
      {
      }

   sp++;
   
   printf(" - Observable %d: ",i);
       
   for (k = 0; k < CF_GRAINS; k++)
      {
      sscanf(sp,"%d",&(weekly[i][k]));
      
      while (*(++sp) != ':')
         {
         }

      if (weekly[i][k] < 0)
         {
         weekly[i][k] = 1;
         }

      sp++;
      printf("%d,",weekly[i][k]);
      }
   
   printf("\n");
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorYear(struct Item *data)

{ struct Item *ip;
  char timekey[CF_SMALLBUF];
  int observable;
  double q,e,dev;

CfOut(cf_verbose,""," -> Monitor year data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      sscanf(ip->name+3,"%s",timekey);
      continue;
      }

   // Extract records
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf\n",&observable,&q,&e,&dev);
   printf("Year-obs %d: %.2lf,%.2lf,%.2lf measured at %s\n",observable,q,e,dev,timekey);
   }
}

/*****************************************************************************/

void Nova_UnPackCompliance(struct Item *data)

{ struct Item *ip;
 time_t then;
 double av,dev;
 char type,eventname[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> Promise Compliance data..............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%255s,%c,%lf,%lf\n",&then,eventname,&type,&av,&dev);

   switch (type)
      {
      case 'c':
          printf("Promise \"%s\" was compliant, av %.2lf +/- %.2lf at %s",eventname,av,dev,cf_ctime(&then));
          break;
      case 'r':
          printf("Promise \"%s\" was repaired, av %.2lf +/- %.2lf at %s",eventname,av,dev,cf_ctime(&then));
          break;
      case 'n':
          printf("Promise \"%s\" was non-compliant, av %.2lf +/- %.2lf at %s",eventname,av,dev,cf_ctime(&then));
          break;
      }
   }
}

/*****************************************************************************/

void Nova_UnPackSoftware(struct Item *data)

{ struct Item *ip;
 char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch;

CfOut(cf_verbose,""," -> Installed software data...............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   printf("Installed software: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPackAvailPatches(struct Item *data)

{ struct Item *ip;
  char arch, name[CF_MAXVARSIZE],version[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> Available patch data...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   printf("Patch available: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPackPatchStatus(struct Item *data)

{ struct Item *ip;
  char arch, name[CF_MAXVARSIZE],version[CF_MAXVARSIZE];
  
CfOut(cf_verbose,""," -> Patch status data.......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   printf("Patch applied: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPack_promise_output_common(struct Item *data)

{ struct Item *ip;
  
CfOut(cf_verbose,""," -> Expanded private promise data.............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("POLICY: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackValueReport(struct Item *data)

{ struct Item *ip;
  char then[CF_SMALLBUF];
  double kept,notkept,repaired;

CfOut(cf_verbose,""," -> Value data..............................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%[^,],%lf,%lf,%lf\n",then,&kept,&repaired,&notkept);

   printf("Business value: (%.0lf,%.0lf,%.0lf) from %s\n",kept,repaired,notkept,then);
   }
}

/*****************************************************************************/

void Nova_UnPackVariables(struct Item *data)

{ struct Item *ip;
  char type[CF_SMALLBUF],name[CF_MAXVARSIZE],value[CF_BUFSIZE];

CfOut(cf_verbose,""," -> Variable data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%4[^,],%255[^,],%2040[^\n]",type,name,value);
   
   printf("var: (%s) %s=%s\n",type,name,value);
   }

}

/*****************************************************************************/

void Nova_UnPackLastSeen(struct Item *data)

{ struct Item *ip;
  char inout, asserted[CF_MAXVARSIZE],dns[CF_MAXVARSIZE];
  double ago,average,dev;
  long fthen;
  time_t then;
 
CfOut(cf_verbose,""," -> Last-seen data..........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c %25s %15s %ld %lf %lf %lf\n",
          &inout,
          asserted,
          dns,
          &fthen,
          &ago,
          &average,
          &dev);

   then = (time_t)fthen;
   
   printf("Saw: %c%s seen %.2lf hrs ago, av %.2lf +/- %.2lf at %s",inout,asserted,ago,average,dev,cf_ctime(&fthen));
   }

}

/*****************************************************************************/

void Nova_UnPackTotalCompliance(struct Item *data)

{ struct Item *ip;
  char then[CF_SMALLBUF],version[CF_SMALLBUF];
  int kept,repaired,notrepaired;
 
CfOut(cf_verbose,""," -> Total Compliance data......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%63[^,],%127[^,],%d,%d,%d\n",then,version,&kept,&repaired,&notrepaired);

   printf("Tcompliance: (%d,%d,%d) for version %s at %s\n",kept,repaired,notrepaired,version,then);
   }

}

/*****************************************************************************/

void Nova_UnPackRepairLog(struct Item *data)

{ struct Item *ip;
  char handle[CF_MAXVARSIZE];
  long then;
  time_t tthen;
  
CfOut(cf_verbose,""," -> Repair log data........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^\n]",&then,handle);
   tthen = (time_t)then;

   printf("Repair: of promise \"%s\" at %s",handle,cf_ctime(&tthen));
   }
}

/*****************************************************************************/

void Nova_UnPackNotKeptLog(struct Item *data)

{ struct Item *ip;
  char handle[CF_MAXVARSIZE];
  time_t tthen;
  long then;
  
CfOut(cf_verbose,""," -> Not kept data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^\n]",&then,handle);
   tthen = (time_t)then;
   printf("Failure: of promise \"%s\" at %s",handle,cf_ctime(&tthen));
   }

}

/*****************************************************************************/

void Nova_UnPackMeter(struct Item *data)

{ struct Item *ip;
  char handle[CF_SMALLBUF];
  time_t then;
  
CfOut(cf_verbose,""," -> Meter data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("Meter: %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackBundles(struct Item *data)

{ struct Item *ip;
  char bundle[CF_SMALLBUF];
  double ago,average,dev;
  long fthen;
  time_t then;
  
CfOut(cf_verbose,""," -> Bundle data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%25s %ld %lf %lf %lf\n",
          bundle,
          &fthen,
          &ago,
          &average,
          &dev);

   then = (time_t)fthen;
   
   printf("Bundle: %s done %.2lf hrs ago, av %.2lf +/- %.2lf at %s",bundle,ago,average,dev,cf_ctime(&fthen));
   }

}




