/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_file.c                                                          */
/*                                                                           */
/* Created: Fri Sep 11 13:13:29 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

// FIXME: Should we unlink first? See UNIX version..
void NovaWin_CreateEmptyFile(char *name)
{ int tempfd;

if((tempfd = fopen(name, "w")) == -1)
   {
   CfOut(cf_error,"fopen","Couldn't open a file %s\n",name);  
   }

close(tempfd);
}


FILE *NovaWin_FileHandleToStream(HANDLE fHandle, char *mode)
{
  int crtHandle;
  int flags;

  if(strncmp(mode, "r", 2) == 0)
    {
      flags = _O_RDONLY;
    }
  else if(strncmp(mode, "w", 2) == 0)
    {
      flags = _O_APPEND;
    }
  else
    {
      CfOut(cf_error,"NovaWin_FileHandleToStream","Mode is not 'r' or 'w', but '%s'", mode);
      return NULL;
    }
  
  crtHandle = _open_osfhandle((long)fHandle, flags);
  
  if(crtHandle == -1)
    {
      CfOut(cf_error,"_open_osfhandle","Could not convert file handle");
      return NULL;
    }

  return _fdopen(crtHandle, mode);
}


// FIXME: Implement
int NovaWin_IsExecutable(char *file)
{
nw_exper("NovaWin_IsExecutable", "saying every file is executable");

return NovaWin_FileExists(file);
}


int NovaWin_mkdir(const char *path, mode_t mode)
{
  nw_exper("NovaWin_mkdir", "mode is ignored");
    
  // no mode on windows, use windows mkdir - return value seems compatible

  return _mkdir(path);

  // TODO: run chmod(path,mode) here ?
}


/* Unix rename() overwrites newpath if it exists, windows rename() does not.
 * We fix this by removing the file first if it exists */
int NovaWin_rename(const char *oldpath, const char *newpath)
{
  nw_exper("NovaWin_rename", "using unlink() before rename()");

  unlink(newpath);

  return rename(oldpath, newpath);
}


/* returns 0 on success, -1 on error */
int NovaWin_chmod(const char *path, mode_t mode)
{
  nw_unimpl("chmod");
  return 0;  // FIXME: Hack for now.

  //nw_exper("NovaWin_chmod", "only supporting a subset of mode-strings for NT");
  

  unsigned char owner = (mode >> 6) & 07;
  mode_t other = mode & 077;

  // only support a couple of mode-strings
  if((owner == 00 || owner == 04 || owner == 05 || owner == 06 || owner == 07) &&
     (other == 000 || other == 044 || other == 055 || other == 066 || other == 077) &&
     (!(mode & 07000)))
    {
      // TODO: FIXME: Implement using ACLs
      return -1;
    }
  else
    {
      CfOut(cf_error,"NovaWin_chmod","The mode-string '%o' on '%s' is not supported in NT", mode, path);
      return -1;
    }
}


/* Return true if file 'fileName' exists */
int NovaWin_FileExists(const char *fileName)
{
    DWORD fileAttr;

    fileAttr = GetFileAttributes(fileName);
    if (0xFFFFFFFF == fileAttr)
      {
	CfOut(cf_verbose,"","The file '%s' does not exist\n",fileName);
        return false;
      }

    return true;
}


#endif  /* MINGW */
