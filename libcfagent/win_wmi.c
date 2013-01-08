/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

/* Functions to query WMI, currently requires disphelper. */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "cfstream.h"
#include "atexit.h"
#include "logging.h"

#include "win_disphelper.h"

#define RUN_QUERY(col,q) (!FAILED(dhGetValue(L"%o", &col, wmiSvc, L".ExecQuery(%S)",  L ## q)))

static void NovaWin_PrintWmiError(char *str);
static int NovaWin_WmiInitialize(void);
static bool PackageListInstalledFromWMI(PackageItem ** pkgList, Attributes a, Promise *pp);

DISPATCH_OBJ(wmiSvc);

int NovaWin_PackageListInstalledFromAPI(PackageItem ** pkgList, Attributes a, Promise *pp)
{
    CfOut(cf_verbose, "", " Using Win32 API to list installed packages");

    if (!NovaWin_WmiInitialize())       // deinitialized before agents exit
    {
        CfOut(cf_error, "", "!! Could not initialize WMI to get installed packages");
        return false;
    }

    return PackageListInstalledFromWMI(pkgList, a, pp);;
}

/*****************************************************************************/
/*                             WMI FUNCTIONS                                 */
/*****************************************************************************/

static bool PackageListInstalledFromWMI(PackageItem ** pkgList, Attributes a, Promise *pp)
/* For Windows Server 2003 R2, Windows XP and earlier.
 * Less accurate since it does not get .msi file names, only
 * the Caption (fiendly name) of the packages and canonifies those.
 * On Windows Server 2003, "Windows Installer Provider" may need
 * to be installed (Add/Remove Windows Components). */
{
    char *caption = NULL;
    char *version = NULL;
    char *sp;
    int count = 0;

    DISPATCH_OBJ(colSoftware);

    CfDebug("NovaWin_WmiGetInstalledPkgsOld()\n");

    if (!RUN_QUERY(colSoftware, "SELECT Caption, Version FROM Win32_Product"))
    {
        NovaWin_PrintWmiError
            ("Could not execute query 'SELECT Caption, Version FROM Win32_Product' in 'NovaWin_WmiGetInstalledPkgsOld'");

        SAFE_RELEASE(colSoftware);
        return false;
    }

    FOR_EACH(softwareItem, colSoftware, NULL)
    {
        dhGetValue(L"%s", &caption, softwareItem, L".Caption");
        dhGetValue(L"%s", &version, softwareItem, L".Version");

        if (caption == NULL || strlen(caption) == 0)
        {
            CfOut(cf_error, "", "!! Empty package caption for installed package");
        }
        else if (version == NULL || strlen(version) == 0)
        {
            CfOut(cf_verbose, "", "!! Empty package version for installed package \"%s\"", caption);
        }
        else
        {

            CfDebug("pkgname=\"%s\", pkgver=\"%s\"\n", caption, version);

            for (sp = caption; *sp != '\0'; sp++)
            {
                if (*sp == ' ')
                {
                    *sp = '-';
                }
                else
                {
                    *sp = tolower(*sp);
                }
            }

            if (!PrependPackageItem(pkgList, caption, version, VSYSNAME.machine, a, pp))
            {
                CfOut(cf_error, "", "!! Could not prepend package name to list");
            }

        }

        if (caption != NULL)
        {
            dhFreeString(caption);
            caption = NULL;
        }

        if (version != NULL)
        {
            dhFreeString(version);
            version = NULL;
        }

        count++;

    }
    NEXT(softwareItem);

    SAFE_RELEASE(colSoftware);

    if (count == 0)
    {
        // can never have empty list since Cfengine is installed
        CfOut(cf_error, "",
              "!! List of installed packages is empty - make sure \"Windows Installer Provider\" is installed and working");
    }

    return true;
}

/****************************************************************************/

static void NovaWin_PrintWmiError(char *str)
{
    char dhErrMsg[CF_BUFSIZE];

    dhFormatExceptionA(NULL, dhErrMsg, sizeof(dhErrMsg) / sizeof(dhErrMsg[0]), TRUE);

    CfOut(cf_error, "", "!! WMI (disphelper) error: \"%s\" (%s)", dhErrMsg, str);
}

/*****************************************************************************/

/* Safe to call even when not initialized */

static void NovaWin_WmiDeInitialize(void)
{
    CfDebug("NovaWin_WmiDeInitialize()\n");

    if (wmiSvc != NULL)
    {
        SAFE_RELEASE(wmiSvc);
        dhUninitialize(TRUE);
    }

    wmiSvc = NULL;
}

static int NovaWin_WmiInitialize(void)
/*
 * Initialize WMI for this thread, safe to call when already initialized.
 */
{
    CfDebug("NovaWin_WmiInitialize()\n");

    if (wmiSvc != NULL)
    {
        CfDebug("WMI already initialized\n");
        return true;
    }

    if (FAILED(dhInitialize(TRUE)))
    {
        CfOut(cf_error, "dhInitialize", "!! Could not initialize disphelper");
        return false;
    }

    // Debug: call with TRUE to enable dialog box error messages
    if (FAILED(dhToggleExceptions(FALSE)))
    {
        CfOut(cf_error, "dhToggleExceptions", "!! Could not control disphelper exception messages");
    }

    if (FAILED(dhGetObject(L"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2", NULL, &wmiSvc)))
    {
        CfOut(cf_error, "dhGetObject", "!! Could not obtain WMI object");
        return false;
    }

    RegisterAtExitFunction(&NovaWin_WmiDeInitialize);

    return true;
}
