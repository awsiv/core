/*****************************************************************************/
/*                                                                           */
/* File: services_win.c                                                      */
/*                                                                           */
/* Created: Wed Dec 16 10:27:07 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


#define STATUSWAIT_MIN 1000   // msecs waiting for a service status change (e.g. pending start -> start)
#define STATUSWAIT_MAX 10000
#define STATUSWAIT_MAXSLEEP 10  // maximum number of times to call Sleep() during waiting


void Nova_VerifyServicesWin(struct Attributes a,struct Promise *pp)
{
#ifdef MINGW

 

#else  /* NOT MINGW */
cfPS(cf_error,CF_FAIL,"",pp,a,"!! Windows services are only supported on Windows");
PromiseRef(cf_error,pp);
#endif
}



#ifdef MINGW





#endif  /* MINGW */
