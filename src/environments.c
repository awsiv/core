
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: environments.c                                                      */
/*                                                                           */
/* Created: Mon May 17 10:15:20 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_VerifyEnvironmentsPromise(struct Promise *pp)

{ struct Attributes a;

a = GetEnvironmentsAttributes(pp);

if (Nova_EnvironmentsSanityChecks(a,pp))
   {
   Nova_VerifyEnvironments(a,pp);
   }
}

/*****************************************************************************/

int Nova_EnvironmentsSanityChecks(struct Attributes a,struct Promise *pp)

{
return true;
}

/*****************************************************************************/

void Nova_VerifyEnvironments(struct Attributes a,struct Promise *pp)

{


}
