
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: processes.c                                                         */
/*                                                                           */
/* Created: Fri Jun 19 10:22:38 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_ZONE_H
# include <zone.h>
#endif

char *Nova_GetProcessOptions()

{
#ifdef HAVE_GETZONEID
 zoneid_t zid;
 char zone[ZONENAME_MAX];

if (LICENSES == 0)
   {
   return VPSOPTS[VSYSTEMHARDCLASS];
   }
 
zid = getzoneid();
getzonenamebyid(zid,zone,ZONENAME_MAX);

if (cf_strcmp(zone,"global") == 0)
   {
   return "-f -z global";
   }

#else
return VPSOPTS[VSYSTEMHARDCLASS];
#endif
}
