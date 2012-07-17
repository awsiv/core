/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

int NovaWin_GetWinDir(char *winDir, int winDirSz)
// NOTE: winDirSz should be MAX_PATH
// Example winDir: "C:\WINDOWS"
{
    UINT res = GetWindowsDirectory(winDir, winDirSz);

    if (res == 0 || res > winDirSz)
    {
        CfOut(cf_error, "GetWindowsDirectory", "Could not get windows directory");
        return false;
    }

    return true;
}

/*****************************************************************************/

int NovaWin_GetSysDir(char *sysDir, int sysDirSz)
// Example sysDir: "C:\WINDOWS\system32"
{
    UINT res = GetSystemDirectory(sysDir, sysDirSz);

    if (res == 0 || res > sysDirSz)
    {
        CfOut(cf_error, "GetSystemDirectory", "!! Could not get system directory");
        return false;
    }

    return true;
}

/*****************************************************************************/

int NovaWin_GetProgDir(char *progDir, int progDirSz)
// Example progDir: "C:\Program Files"
{
    return NovaWin_GetEnv("PROGRAMFILES", progDir, progDirSz);
}

/*****************************************************************************/

int NovaWin_GetEnv(char *varName, char *varContents, int varContentsSz)
// NOTE: varContentSz needs to maximum be 32767
{
    UINT res = GetEnvironmentVariable(varName, varContents, varContentsSz);

    if (res == 0 || res > varContentsSz)
    {
        CfOut(cf_error, "GetEnvironmentVariable", "!! Could not get environment variable \"%s\"", varName);
        return false;
    }

    return true;
}

/*****************************************************************************/

const char *GetDefaultWorkDir(void)
{
    static char workdir[CF_BUFSIZE];

    if (!*workdir)
    {
        if (NovaWin_GetProgDir(workdir, CF_BUFSIZE - sizeof("Cfengine")))
        {
            strcat(workdir, "\\Cfengine");
        }
        else
        {
            CfOut(cf_error, "",
                  "!! Could not get CFWORKDIR from Windows environment variable, falling b'ack to compile time dir (%s)",
                  WORKDIR);
            strcpy(workdir, WORKDIR);
        }
    }

    return workdir;
}
