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

#include "win_disphelper.h"

#define RUN_QUERY(col,q) (!FAILED(dhGetValue(L"%o", &col, wmiSvc, L".ExecQuery(%S)",  L ## q)))

DISPATCH_OBJ(wmiSvc);



int NovaWin_GetInstalledPkgs(struct CfPackageItem *pkgList, struct Attributes a, struct Promise *pp)
{
  if(!NovaWin_CheckWmiInitialized("NovaWin_GetInstalledPkgs"))
    {
      return false;
    }


  if(WINVER_MAJOR < 6) // 2003/XP or earlier versions
    {
      return NovaWin_WmiGetInstalledPkgsOld(pkgList, a, pp);
    }
  else
    {
      return NovaWin_WmiGetInstalledPkgsNew(pkgList, a, pp);
    }

}


/*****************************************************************************/
/*                             WMI FUNCTIONS                                 */
/*****************************************************************************/

int NovaWin_WmiGetInstalledPkgsNew(struct CfPackageItem *pkgList, struct Attributes a, struct Promise *pp)
{
  char *pkgName = NULL;

  DISPATCH_OBJ(colSoftware);

  if(!RUN_QUERY(colSoftware, "SELECT PackageName FROM Win32_Product"))
    {
      CfOut(cf_error, "dhGetValue", "!! Could not execute query \"%s\"", "SELECT PackageName FROM Win32_Product");
      
      SAFE_RELEASE(colSoftware);
      return false;
    }


  FOR_EACH(softwareItem, colSoftware, NULL)
    {
      dhGetValue(L"%s", &pkgName, softwareItem, L".PackageName");
	    

      if(pkgName == NULL)
	{
	  CfOut(cf_error, "", "!! Empty package name for installed package");
	}
      else
	{
	  if(!PrependPackageItem(&pkgList, pkgName, "9.9.9", VSYSNAME.machine, a, pp))
	    {
	      CfOut(cf_error, "", "!! Could not prepend package name to list");
	    }
	  
	  dhFreeString(pkgName);
	  pkgName = NULL;
	}
      

    } NEXT(softwareItem);
  

  SAFE_RELEASE(colSoftware);
  
  return true;
}

/*****************************************************************************/

int NovaWin_WmiGetInstalledPkgsOld(struct CfPackageItem *pkgList, struct Attributes a, struct Promise *pp)
/* For Windows Server 2003 R2, Windows XP and earlier.
 * Less accurate since it does not get .msi file names, only
 * the Caption (fiendly name) of the packages and canonifies those.
 * On Windows Server 2003, "Windows Installer Provider" may need
 * to be installed (Add/Remove Windows Components). */
{
  return false;
}

/*****************************************************************************/

int NovaWin_CheckWmiInitialized(char *caller)
{
  if(wmiSvc == NULL)
    {
      CfOut(cf_error, "", "!! WMI used unitialized in \"%s\"", caller);
      return false;
    }
  
  return true;
}

/*****************************************************************************/

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

/*****************************************************************************/

int NovaWin_WmiDeInitialize(void)
{
  SAFE_RELEASE(wmiSvc);
  dhUninitialize(TRUE);
	
  return true;
}

#endif  /* MINGW */
