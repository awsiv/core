/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_sysinfo.c                                                       */
/*                                                                           */
/* Created: Fri Sep 11 13:13:29 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

int NovaWin_GetWinDir(char *winDir, int winDirSz)
// NOTE: winDirSz should be MAX_PATH
// Example winDir: "C:\WINDOWS"
{
  UINT res = GetWindowsDirectory(winDir, winDirSz);

  if(res == 0 || res > winDirSz)
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

  if(res == 0 || res > sysDirSz)
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

  if(res == 0 || res > varContentsSz)
    {
      CfOut(cf_error, "GetEnvironmentVariable", "!! Could not get environment variable \"%s\"", varName);
      return false;
    }

  return true;
}


#endif  /* MINGW */
