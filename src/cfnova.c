
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: cfnova.c                                                            */
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
