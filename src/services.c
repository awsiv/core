
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
 struct CfLock thislock;
 
 if (LICENSES == 0)
    {
    return;
    }

 thislock = AcquireLock(pp->promiser,VUQNAME,CFSTARTTIME,a,pp);

 if (thislock.lock == NULL)
    {
    return;
    }

 PromiseBanner(pp);


 if(strcmp(a.service.service_type, "windows") == 0)
    {
    switch(VSYSTEMHARDCLASS)
       {
       case mingw:
           NovaWin_VerifyServices(a, pp);
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

 YieldCurrentLock(thislock);
}
