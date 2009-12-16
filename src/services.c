
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


void Nova_VerifyServices(struct Attributes a,struct Promise *pp)

{
 if (LICENSES == 0)
    {
    return;
    }
 
 
 if(strcmp(a.service.service_type, "windows") == 0)
    {
    switch(VSYSTEMHARDCLASS)
       {
       case mingw:
           Nova_VerifyServicesWin(a, pp);
           break;

       default:
           CfOut(cf_inform,"","!! Windows services are not supported on this system");
           break;
       }
    }
 else
    {
    CfOut(cf_inform,"","!! Service management is not yet supported on this system.");
    }


}
