
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

void Nova_CheckInstalledLibraries(void)
{
#ifndef HAVE_LIBLDAP
printf("\t->LIBLDAP not found!!\n");
#endif

#ifndef HAVE_FIPS_MODE_SET
printf("\t->FIPS OpenSSL canister not found!!\n");
#endif

#ifndef HAVE_LIBVIRT
printf("\t->LIBVIRT not found!!\n");
#endif
}
