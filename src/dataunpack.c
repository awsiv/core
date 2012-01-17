
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

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

void Nova_UnPackPerformance(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  time_t t;
  char eventname[CF_MAXVARSIZE];
  double measure = 0,average = 0,dev = 0;

CfOut(cf_verbose,""," -> Performance data ...................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SavePerformance(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   eventname[0] = '\0';
   sscanf(ip->name,"%ld,%lf,%lf,%lf,%255[^\n]\n",&t,&measure,&average,&dev,eventname);
   CfDebug("Performance of \"%s\" is %.4lf (av %.4lf +/- %.4lf) measured at %s\n",eventname,measure,average,dev,cf_ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackClasses(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char name[CF_MAXVARSIZE];
  time_t t;
  double q = 0, dev = 0;

CfOut(cf_verbose,""," -> Class data .................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveClasses(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   sscanf(ip->name,"%[^,],%ld,%lf,%lf\n",name,&t,&q,&dev);
   CfDebug("Class: \"%s\" seen with probability %.4lf +- %.4lf last seen at %s\n",name,q,dev,cf_ctime(&t));
   }
}

/*****************************************************************************/

void Nova_UnPackSetuid(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> setuid data ......................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveSetUid(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("Set-uid program: %s",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackFileChanges(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char name[CF_MAXVARSIZE];
  long date;
  time_t then;

CfOut(cf_verbose,""," -> File change data....................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveFileChanges(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   sscanf(ip->name,"%ld,%255[^\n]",&date,name);
   then = (time_t)date;
   CfDebug("File-change event: in \"%s\" at %ld\n",name,then);
   }
}

/*****************************************************************************/

void Nova_UnPackDiffs(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char name[CF_MAXVARSIZE],change[CF_BUFSIZE];
  long t;
  char *sp;
  
CfOut(cf_verbose,""," -> File diff data...................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveFileDiffs(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract records
   change[0] = '\0';
   sscanf(ip->name,"%ld|%255[^|]|%2047[^\n]",&t,name,change);

   for (sp = change; *sp != '\0'; sp++)
      {
      if (*sp == CF_N_CODE)
         {
         *sp = '\n';
         }
      }
   
   CfDebug("Change-diff: in file %s at %ld \nbegin\n%s\nend\n",name,t,change);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorWeek(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  int observable,slot;
  double q,e,dev;
  char t[CF_TIME_SIZE];

CfOut(cf_verbose,""," -> Monitor weekly data.....................");
CfOut(cf_inform, "", "!! Deprecated monitor weekly format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED //HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData(dbconn, id, mon_rep_week, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      memset(t,0,CF_TIME_SIZE);
      sscanf(ip->name+3,"%31[^,],%d",t,&slot);
      continue;
      }

   // Extract records
   
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf\n",&observable,&q,&e,&dev);
   CfDebug("Week-obs %d in slot %d: %.2lf,%.2lf,%.2lf\n",observable,slot,q,e,dev);
   }

}

/*****************************************************************************/

void Nova_UnPackMonitorMag(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  int observable,slot;
  double q,e,dev;

CfOut(cf_verbose,""," -> Monitor magnified data.....................");
CfOut(cf_inform, "", "!! Deprecated monitor magnified format - response from Nova 2.0.4 or earlier -- skipped");


#ifdef NEVERDEFINED //HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData(dbconn,id,mon_rep_mag,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      sscanf(ip->name+3,"%d",&slot);
      continue;
      }

   // Extract records
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf",&observable,&q,&e,&dev);
   CfDebug("Mag-obs %d: %.2lf,%.2lf,%.2lf measured for slot %d\n",observable,q,e,dev,slot);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorYear(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
 int observable,slot = 0;
  double q,e,dev;

CfOut(cf_verbose,""," -> Monitor year data.....................");
CfOut(cf_inform, "", "!! Deprecated monitor year format - response from Nova 2.0.4 or earlier -- skipped");


#ifdef NEVERDEFINED //HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData(dbconn, id, mon_rep_yr, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   // Extract time stamp
   if (strncmp(ip->name,"T: ", 3) == 0)
      {
      sscanf(ip->name+3,"%d",&slot); // 3*12
      continue;
      }

   // Extract records
   q = e = dev = 0;
   sscanf(ip->name,"%d %lf %lf %lf\n",&observable,&q,&e,&dev);
   CfDebug("Year-obs %d: %.2lf,%.2lf,%.2lf measured at slot %d\n",observable,q,e,dev,slot);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorHist(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> Monitor histogram data.....................");
CfOut(cf_inform, "", "!! Deprecated monitor histogram format - response from Nova 2.0.4 or earlier -- skipped");

#ifdef NEVERDEFINED // HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorHistograms(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("OLDHist-obs %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorHg(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> Monitor histogram data.....................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorHistograms(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("Hist-obs %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorMg(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> Monitor magnified data.....................");


#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData2(dbconn,id,mon_rep_mag,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("Mag-obs %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorWk(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> Monitor weekly data.....................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData2(dbconn, id, mon_rep_week, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("Week-obs %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackMonitorYr(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;

CfOut(cf_verbose,""," -> Monitor year data.....................");


#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMonitorData2(dbconn, id, mon_rep_yr, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("Year-obs %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackCompliance(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  time_t then;
  double av,dev;
  char type,eventname[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> Promise Compliance data..............");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SavePromiseCompliance(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%255[^,],%c,%lf,%lf\n",&then,eventname,&type,&av,&dev);
   
   switch (type)
      {
      case 'c':
          CfDebug("Promise \"%s\" was compliant, av %.2lf +/- %.2lf at %s\n",eventname,av,dev,cf_ctime(&then));
          break;
      case 'r':
          CfDebug("Promise \"%s\" was repaired, av %.2lf +/- %.2lf at %s\n",eventname,av,dev,cf_ctime(&then));
          break;
      case 'n':
          CfDebug("Promise \"%s\" was non-compliant, av %.2lf +/- %.2lf at %s\n",eventname,av,dev,cf_ctime(&then));
          break;
      default:
          CfDebug("Unknown state '%c' (eventname=%s,%.2lf +/- %.2lf at %s)\n",type,eventname,av,dev,cf_ctime(&then));
      }
   }
}

/*****************************************************************************/

void Nova_UnPackSoftware(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char name[CF_MAXVARSIZE],version[CF_MAXVARSIZE],arch;

CfOut(cf_verbose,""," -> Installed software data...............");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveSoftware(dbconn,sw_rep_installed,id,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   CfDebug("Installed software: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPackAvailPatches(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char arch, name[CF_MAXVARSIZE],version[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> Available patch data...................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveSoftware(dbconn,sw_rep_patch_avail,id,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   CfDebug("Patch available: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPackPatchStatus(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char arch, name[CF_MAXVARSIZE],version[CF_MAXVARSIZE];
  
CfOut(cf_verbose,""," -> Patches installed data.......................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveSoftware(dbconn,sw_rep_patch_installed,id,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%250[^,],%250[^,],%c",name,version,&arch);

   // architcure coding, see Nova_ShortArch
   
   CfDebug("Patch applied: %s version (%s on %c)\n",name,version,arch);
   }
}

/*****************************************************************************/

void Nova_UnPack_promise_output_common(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  
CfOut(cf_verbose,""," -> Expanded private promise data.............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   CfDebug("POLICY: %s",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackValueReport(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  double kept,notkept,repaired;
  char date[CF_SMALLBUF];

CfOut(cf_verbose,""," -> Value data..............................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveValueReport(dbconn,id,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%[^,],%lf,%lf,%lf\n",date,&kept,&repaired,&notkept);

   CfDebug("Business value: (%.0lf,%.0lf,%.0lf) from %s\n",kept,repaired,notkept,date);
   }
}

/*****************************************************************************/

void Nova_UnPackVariables(mongo_connection *dbconn, char *id, Item *data)

/* Should be deprecated some time - was replaced after Nova 2.0.4 */

{ Item *ip;
  char type[CF_SMALLBUF],name[CF_MAXVARSIZE],value[CF_BUFSIZE],scope[CF_MAXVARSIZE];

CfOut(cf_verbose,""," -> Variable data...........................");
CfOut(cf_inform, "", "!! Deprecated variable data format - response from Nova 2.0.4 or earlier (some features unavailible)");


#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveVariables(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"S: ", 3) == 0)
      {
      scope[0] = '\0';
      sscanf(ip->name+3,"%254[^\n]",scope);
      CfDebug("SCOPE: %s\n",scope);
      continue;
      }

   sscanf(ip->name,"%4[^,], %255[^,], %2040[^\n]",type,name,value);
   
   CfDebug("line is \"%s\"\n", ip->name);
   CfDebug("var: (%s) \"%s\"=\"%s\"\n",type,name,value);
   }

}

/*****************************************************************************/

void Nova_UnPackVariables2(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char type[CF_SMALLBUF],name[CF_MAXVARSIZE],value[CF_BUFSIZE],scope[CF_MAXVARSIZE];
  time_t t;

CfOut(cf_verbose,""," -> Variable data with date stamp...........");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
     CFDB_SaveVariables2(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   if (strncmp(ip->name,"S:", 2) == 0)
      {
      scope[0] = '\0';
      sscanf(ip->name+2,"%254[^\n]",scope);
      CfDebug("SCOPE: %s\n",scope);
      continue;
      }

   sscanf(ip->name,"%4[^,],%ld,%255[^,],%2040[^\n]",type,&t,name,value);
   
   CfDebug("var: (%s) at %s \"%s\"=\"%s\"\n",type,cf_ctime(&t),name,value);
   }

}

/*****************************************************************************/

void Nova_UnPackLastSeen(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
 char inout, asserted[CF_MAXVARSIZE],dns[CF_MAXVARSIZE],hash[CF_MAXVARSIZE];
  double ago,average,dev;
  long fthen;
 
CfOut(cf_verbose,""," -> Last-seen data..........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveLastSeen(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c %128s %25s %15s %ld %lf %lf %lf\n",
          &inout,
          hash,
          asserted,
          dns,
          &fthen,
          &ago,
          &average,
          &dev);

   // map our own address
   if(IsInterfaceAddress(asserted))
     {
     snprintf(asserted,sizeof(asserted),"%s",VFQNAME);
     }

   
   CfDebug("Saw: %c%s (alias %s/%s) seen %.2lf hrs ago, av %.2lf +/- %.2lf at %s",inout,hash,asserted,dns,ago,average,dev,cf_ctime(&fthen));
   }

}

/*****************************************************************************/

void Nova_UnPackTotalCompliance(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char version[CF_SMALLBUF];
  int kept,repaired,notrepaired;
  long date;
  time_t then;
  
CfOut(cf_verbose,""," -> Total Compliance data......................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveTotalCompliance(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^,],%d,%d,%d\n",&date,version,&kept,&repaired,&notrepaired);
   then = (time_t)date;
   
   CfDebug("Tcompliance: (%d,%d,%d) for version %s at %ld\n",kept,repaired,notrepaired,version,then);
   }

}

/*****************************************************************************/

void Nova_UnPackRepairLog(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char handle[CF_MAXVARSIZE];
  long then;
  time_t tthen;
  
CfOut(cf_verbose,""," -> Repair log data........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SavePromiseLog(dbconn, id, PROMISE_LOG_STATE_REPAIRED, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^\n]",&then,handle);
   tthen = (time_t)then;

   CfDebug("Repair: of promise \"%s\" at %lu\n",handle,tthen);
   }
}

/*****************************************************************************/

void Nova_UnPackNotKeptLog(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char handle[CF_MAXVARSIZE];
  time_t tthen;
  long then;
  
CfOut(cf_verbose,""," -> Not kept data...........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SavePromiseLog(dbconn, id, PROMISE_LOG_STATE_NOTKEPT, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%ld,%127[^\n]",&then,handle);
   tthen = (time_t)then;
   CfDebug("Failure: of promise \"%s\" at %lu\n",handle,tthen);
   }

}

/*****************************************************************************/

void Nova_UnPackMeter(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char type;
  double kept,repaired;
  
CfOut(cf_verbose,""," -> Meter data...........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveMeter(dbconn, id, data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c: %lf %lf",&type,&kept,&repaired);

   switch (type)
      {
      case 'W':
          CfDebug("Meter week compliance: %lf, %lf\n",kept,repaired);
          break;
      case 'D':
          CfDebug("Meter daily compliance: %lf, %lf\n",kept,repaired);
          break;
      case 'H':
          CfDebug("Meter hourly compliance: %lf, %lf\n",kept,repaired);
          break;
      case 'P':
          CfDebug("Meter performance: %lf, %lf\n",kept,repaired);
          break;
      case 'S':
          CfDebug("Meter licenses: %lf, %lf\n",kept,repaired);
          break;
      case 'C':
          CfDebug("Meter comms: %lf, %lf\n",kept,repaired);
          break;          
      case 'A':
          CfDebug("Meter anomalies: %lf, %lf\n",kept,repaired);
          break;
      }
   }
}

/*****************************************************************************/

void Nova_UnPackSoftwareDates(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char type;

CfOut(cf_verbose,""," -> Software dates data...........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveSoftwareDates(dbconn, id, data);
   }
#endif

time_t t;

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%c:%ld",&type,&t);

   switch (type)
      {
      case 'S':
          CfDebug("Software installed date: %ld",t);
          break;
      }
   }
}

/*****************************************************************************/

void Nova_UnPackBundles(mongo_connection *dbconn, char *id, Item *data)

{ Item *ip;
  char bundle[CF_SMALLBUF];
  double ago,average,dev;
  long fthen;
  
CfOut(cf_verbose,""," -> Bundle data...........................");

#ifdef HAVE_LIBMONGOC
if (dbconn)
   {
   CFDB_SaveBundles(dbconn,id,data);
   }
#endif

for (ip = data; ip != NULL; ip=ip->next)
   {
   sscanf(ip->name,"%25s %ld %lf %lf %lf\n",
          bundle,
          &fthen,
          &ago,
          &average,
          &dev);

   CfDebug("Bundle: %s done %.2lf hrs ago, av %.2lf +/- %.2lf at %s",bundle,ago,average,dev,cf_ctime(&fthen));
   }
}

/*****************************************************************************/

char *Nova_LongArch(char *arch)
    
{
if (arch == NULL)
   {
   return "*";
   }
 
if (strcmp(arch,"3") == 0)
   {
   return "i386";
   }

if (strcmp(arch,"4") == 0)
   {
   return "i486";
   }

if (strcmp(arch,"5") == 0)
   {
   return "i586";
   }

if (strcmp(arch,"6") == 0)
   {
   return "i686";
   }

if (strcmp(arch,"") == 0)
   {
   return "noarch";
   }

if (strcmp(arch,"\n") == 0)
   {
   return "*";
   }

if (strcmp(arch,"x") == 0)
   {
   return "x86_64";
   }

if (strcmp(arch,"t") == 0)
   {
   return "s390";
   }

if (strcmp(arch,"s") == 0)
   {
   return "s390x";
   }

if (strcmp(arch,"d") == 0)
   {
   return "default";
   }

return arch;
}
