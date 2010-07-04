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
   sscanf(ip->name,"%ld,%lf,%lf,%255s\n",&t,&measure,&average,&dev,eventname);

   printf("Performance of \"%s\" is %.4lf (av %.4lf +/- %.4lf) measured at %s",eventname,measure,average,dev,ctime(&t));
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
   printf("Class: \"%s\" seen with probability %.4lf +- %.4lf last seen at %s",name,q,dev,ctime(&t));
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
   printf("File-change event: in %s at %s",name,date);
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
   printf("Mag-obs %d: %.2lf,%.2lf,%.2lf measured at %s",observable,q,e,dev,ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorHist(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor histogram data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("HISTO: %s\n",ip->name);

// FIX ME HERE.....   sscanf(ip->name,"%d:",i);

/*   sp += 3; etc
   
   for (k = 0; k < CF_GRAINS; k++)
      {      
      snprintf(val,CF_SMALLBUF,"%d ",weekly[i][k]);
      strcat(buffer,val);
      }
*/ 

   }
}

/*****************************************************************************/

void Nova_UnPackMonitorYear(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor year data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("YEAR: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackCompliance(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Promise Compliance data..............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("COMPLIANE: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackSoftware(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Installed software data...............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("SOFT: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackAvailPatches(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Available patch data...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("AVAIL_PATCH: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackPatchStatus(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Patch status data.......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("PATCHSTATUS: %s",ip->name);
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

CfOut(cf_verbose,""," -> Value data..............................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("VALUE: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackVariables(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Variable data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("VARS: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackLastSeen(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Last-seen data..........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("LASTSEEN: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackTotalCompliance(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Total Compliance data......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("TOTCOMPLIANCE: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackRepairLog(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Repair log data........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("REPAIR: %s",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackNotKeptLog(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Not kept data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("NOTKEPT: %s",ip->name);
   }

}


