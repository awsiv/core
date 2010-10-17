
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

void Nova_Initialize()

{
#ifdef HAVE_FIPS_MODE_SET
if (FIPS_MODE && !FIPS_mode_set(1))
   {
   FatalError("Unable to initialize validated FIPS mode");
   }
#else
if (FIPS_MODE)
   {
   FatalError("This software version has not been built with FIPS mode support");
   }
#endif
}
