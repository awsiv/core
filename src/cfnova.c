
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "../../core/src/cf3.defs.h"
#include "../../core/src/cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: cfnova.c                                                            */
/*                                                                           */
/*****************************************************************************/

void Nova_Initialize()

{
#ifdef HAVE_FIPS_MODE_SET
if (FIPS_MODE && !FIPS_mode_set(1))
   {
   FatalError("Unable to initialize validated FIPS mode");
   }
else
   {
   CfOut(cf_verbose,""," ** Cfengine Nova Operating if FIPS 140-2 validated mode");
   }
#else
if (FIPS_MODE)
   {
   FatalError("This software version has not been built with FIPS mode support");
   }
#endif
}

/*****************************************************************************/

int Nova_HaveFIPS(void)
{
#ifdef HAVE_FIPS_MODE_SET
return true;
#else
return false;
#endif
}


