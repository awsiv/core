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

/* static prototypes */
static int NovaWin_WmiGetInstalledPkgsNew(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp);
static int NovaWin_WmiGetInstalledPkgsOld(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp);
static int Nova_PrependPackageItem(struct CfPackageItem **list,char *name,char *version,char *arch,struct Attributes a,struct Promise *pp);

DISPATCH_OBJ(wmiSvc);


int NovaWin_GetInstalledPkgs(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp)
{
  int res;
  
  if(!NovaWin_WmiInitialize())  // deinitialized before agents exit
    {
      CfOut(cf_error, "", "!! Could not initialize WMI to get installed packages");
      return false;
    }
  
  
if (WINVER_MAJOR < 6) // 2003/XP or earlier versions
   {
     res = NovaWin_WmiGetInstalledPkgsOld(pkgList, a, pp);
   }
else
   {
     res = NovaWin_WmiGetInstalledPkgsNew(pkgList, a, pp);
   }

 return res;
}


/*****************************************************************************/
/*                             WMI FUNCTIONS                                 */
/*****************************************************************************/

static int NovaWin_WmiGetInstalledPkgsNew(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp)

{ char *pkgName = NULL;
  char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE];
  char *nameRegex, *versionRegex;

  DISPATCH_OBJ(colSoftware);

 Debug("NovaWin_WmiGetInstalledPkgsNew()\n");


 // default to user-defined name and version regex
 if(a.packages.package_name_regex != NULL)
   {
   nameRegex = a.packages.package_name_regex;
   }
 else
   {
   nameRegex = "([^-]+).*";
   }


 if(a.packages.package_version_regex != NULL)
   {
   versionRegex = a.packages.package_version_regex;
   }
 else
   {
   versionRegex = "[^-]+-([^-]+).*";
   }


if (!RUN_QUERY(colSoftware, "SELECT PackageName FROM Win32_Product"))
   {
   NovaWin_PrintWmiError("Could not execute query 'SELECT PackageName FROM Win32_Product' in 'NovaWin_WmiGetInstalledPkgsNew'");
   
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
	Debug("pkgname=\"%s\"", pkgName);
	
	snprintf(name, sizeof(name), "%s", ExtractFirstReference(nameRegex, pkgName));
        snprintf(version, sizeof(version), "%s", ExtractFirstReference(versionRegex, pkgName));
          
        Debug("regex_pkgname=\"%s\", regex_pkgver=\"%s\"\n", name, version);
      
      if (!Nova_PrependPackageItem(pkgList, name, version, VSYSNAME.machine, a, pp))
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

static int NovaWin_WmiGetInstalledPkgsOld(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp)
/* For Windows Server 2003 R2, Windows XP and earlier.
 * Less accurate since it does not get .msi file names, only
 * the Caption (fiendly name) of the packages and canonifies those.
 * On Windows Server 2003, "Windows Installer Provider" may need
 * to be installed (Add/Remove Windows Components). */
{ char *caption = NULL;
  char *version = NULL;

  DISPATCH_OBJ(colSoftware);

 Debug("NovaWin_WmiGetInstalledPkgsOld()\n");

  if(!RUN_QUERY(colSoftware, "SELECT Caption, Version FROM Win32_Product"))
    {
      NovaWin_PrintWmiError("Could not execute query 'SELECT Caption, Version FROM Win32_Product' in 'NovaWin_WmiGetInstalledPkgsOld'");
      
      SAFE_RELEASE(colSoftware);
      return false;
    }

  FOR_EACH(softwareItem, colSoftware, NULL)
    {
      dhGetValue(L"%s", &caption, softwareItem, L".Caption");
      dhGetValue(L"%s", &version, softwareItem, L".Version");
	    

      if(caption == NULL || strlen(caption) == 0)
	{
	  CfOut(cf_error, "", "!! Empty package caption for installed package");
	}
      else if(version == NULL || strlen(version) == 0)
	{
	  CfOut(cf_verbose, "", "!! Empty package version for installed package \"%s\"", caption);
	}
      else
	{

	  Debug("pkgname=\"%s\", pkgver=\"%s\"\n", caption, version);

          snprintf(caption, sizeof(caption), "%s", CanonifyName(caption));
          

	  if(!Nova_PrependPackageItem(pkgList, caption, version, VSYSNAME.machine, a, pp))
	    {
	      CfOut(cf_error, "", "!! Could not prepend package name to list");
	    }
	  
	}

      if(caption != NULL)
        {
	  dhFreeString(caption);
	  caption = NULL;
        }
      
      if(version != NULL)
        {
	  dhFreeString(version);
	  version = NULL;
        }


    } NEXT(softwareItem);


SAFE_RELEASE(colSoftware);

return true;
}

/****************************************************************************/

void NovaWin_PrintWmiError(char *str)
{
  char dhErrMsg[CF_BUFSIZE];

  dhFormatExceptionA(NULL, dhErrMsg, sizeof(dhErrMsg)/sizeof(dhErrMsg[0]), TRUE);

  CfOut(cf_error, "", "!! WMI (disphelper) error: \"%s\" (%s)", dhErrMsg, str);
}


/*****************************************************************************/

int NovaWin_WmiInitialize(void)
/*
 * Initialize WMI for this thread, safe to call when already initialized.
 */
{
  Debug("NovaWin_WmiInitialize()\n");

  if(wmiSvc != NULL)
    {
      Debug("WMI already initialized\n");
      return true;
    }

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
/* Safe to call even when not initialized */
{
  Debug("NovaWin_WmiDeInitialize()\n");

  if(wmiSvc != NULL)
    {
      SAFE_RELEASE(wmiSvc);
      dhUninitialize(TRUE);
    }

  wmiSvc = NULL;

  return true;
}

#endif  /* MINGW */

/*****************************************************************************/

/* Copy from Cf3 verify_packages.c due to linker issues */

static int Nova_PrependPackageItem(struct CfPackageItem **list,char *name,char *version,char *arch,struct Attributes a,struct Promise *pp)

{ struct CfPackageItem *pi;

if (strlen(name) == 0 || strlen(version) == 0 || strlen(arch) == 0)
   {
   return false;
   }

if ((pi = (struct CfPackageItem *)malloc(sizeof(struct CfPackageItem))) == NULL)
   {
   CfOut(cf_error,"malloc","Can't allocate new package\n");
   return false;
   }

if (list)
   {
   pi->next = *list;
   }
else
   {
   pi->next = NULL;
   }

pi->name = strdup(name);
pi->version = strdup(version);
pi->arch = strdup(arch);
*list = pi;

/* Finally we need these for later schedule exec, once this iteration context has gone */

pi->pp = DeRefCopyPromise("this",pp);
return true;
}
