/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_wmi.c                                                           */
/*                                                                           */
/* Created: Wed Feb 10 15:59:33 2010                                         */
/*                                                                           */
/*****************************************************************************/

/* Functions to query WMI, currently requires disphelper. */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

DISPATCH_OBJ(wmiSvc);


int NovaWin_WmiInitialize(void)
/*
 * Initialize WMI for this thread
 */
{
  if(FAILED(dhInitialize(TRUE)))
    {
      CfOut(cf_error, "dhInitialize", "!! Could not initialize disphelper");
      return false;
    }
	
  // Debug: call with TRUE to enable dialog box error messages
  if(FAILED(dhToggleExceptions(FALSE)))
    {  
      CfOut(cf_error, "dhToggleExceptions", "!! Could not control disphelper exception messages");
    }

  if(FAILED(dhGetObject(L"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2", NULL, &wmiSvc)))
    {
      CfOut(cf_error, "dhGetObject", "!! Could not obtain WMI object");
      return false;
    }
	
  return true;
}


int NovaWin_WmiDeInitialize(void)
{
  SAFE_RELEASE(wmiSvc);
  dhUninitialize(TRUE);
	
  return true;
}

#endif  /* MINGW */
