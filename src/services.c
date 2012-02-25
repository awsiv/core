
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: services.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/* !__MINGW32__ */
void VerifyWindowsService(Attributes a, Promise *pp)
{
    cfPS(cf_error, CF_FAIL, "", pp, a, "!! Windows services are only supported on Windows");
    PromiseRef(cf_error, pp);
}
