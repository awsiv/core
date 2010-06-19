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

CfOut(cf_verbose,""," -> Performance data ...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("PERF: %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackClasses(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Class data .................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("CLASS: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackSetuid(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> setuid data ......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackFileChanges(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> File change data....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackDiffs(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> File diff data...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackMonitorWeek(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor weekly data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackMonitorMag(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor magnified data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackMonitorHist(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor histogram data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("HISTO: %s\n",ip->name);
   }
}

/*****************************************************************************/

void Nova_UnPackMonitorYear(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Monitor year data.....................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackCompliance(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Promise Compliance data..............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackSoftware(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Installed software data...............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackAvailPatches(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Available patch data...................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackPatchStatus(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Patch status data.......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPack_promise_output_common(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Expanded private promise data.............");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackValueReport(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Value data..............................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackVariables(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Variable data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackLastSeen(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Last-seen data..........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackTotalCompliance(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Total Compliance data......................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackRepairLog(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Repair log data........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}

/*****************************************************************************/

void Nova_UnPackNotKeptLog(struct Item *data)

{ struct Item *ip;

CfOut(cf_verbose,""," -> Not kept data...........................");

for (ip = data; ip != NULL; ip=ip->next)
   {
   printf("GOT: %s\n",ip->name);
   }

}


