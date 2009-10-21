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


void NovaWin_CreateEmptyFile(char *name)
{
  HANDLE fileHandle;

  if (unlink(name) == -1)
    {
      Debug("Pre-existing object %s could not be removed or was not there\n",name);
    }

  fileHandle = CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

  if(fileHandle == INVALID_HANDLE_VALUE)
    {
      CfOut(cf_error,"CreateFile","!! Could not create empty file");
      return;
    }

  CloseHandle(fileHandle);
}

/*****************************************************************************/

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
      CfOut(cf_error,"NovaWin_FileHandleToStream","!! Mode is not 'r' or 'w', but '%s'", mode);
      return NULL;
    }
  
  crtHandle = _open_osfhandle((long)fHandle, flags);
  
  if(crtHandle == -1)
    {
      CfOut(cf_error,"_open_osfhandle","!! Could not convert file handle");
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
      CfOut(cf_error,"NovaWin_chmod","!! The mode-string '%o' on '%s' is not supported in NT", mode, path);
      return -1;
    }
}


/* Return true if file 'fileName' exists */
int NovaWin_FileExists(const char *fileName)
{
    DWORD fileAttr;

    fileAttr = GetFileAttributes(fileName);
    
    if (fileAttr == INVALID_FILE_ATTRIBUTES)
      {
	CfOut(cf_verbose,"","The file \"%s\" does not exist\n", fileName);
        return false;
      }

    return true;
}

/*****************************************************************************/

/* Return true if 'fileName' is a directory */
int NovaWin_IsDir(char *fileName)
{
  DWORD fileAttr;
  
  fileAttr = GetFileAttributes(fileName);
  
  if(fileAttr == INVALID_FILE_ATTRIBUTES)
    {
      CfOut(cf_error,"","!! No file object exsists in path \"%s\"", fileName);
      return false;
    }
  
  if(fileAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
      return true;
    }
  
  return false;
}

/*****************************************************************************/

int NovaWin_VerifyOwner(char *file,struct Promise *pp,struct Attributes attr)
{
  SECURITY_DESCRIPTOR *secDesc;
  char procOwnerSid[CF_BUFSIZE];
  struct UidList *ulp;
  SID *ownerSid;
  int sidMatch = false;
  DWORD getRes;

  getRes = GetNamedSecurityInfo(file, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, (PSID*)&ownerSid, NULL, NULL, NULL, &secDesc);

  if(getRes != ERROR_SUCCESS)
    {
      CfOut(cf_error,"GetNamedSecurityInfo","!! Could not retreive existing owner of \"%s\"", file);
      return false;
    }
  
  // check if file owner is as it should be
  for(ulp = attr.perms.owners; ulp != NULL; ulp = ulp->next)
    {
      if(EqualSid(ownerSid, ulp->sid))
	{
	  sidMatch = true;
	  break;
	}
    }

  LocalFree(secDesc);

  if(sidMatch)
    {
      cfPS(cf_inform,CF_NOP,"",pp,attr,"-> Owner of \"%s\" needs no modification", file);
    }
  else
    {
      CfOut(cf_verbose,"","Changing owner of file \"%s\" to the first listed", file);

      if(!NovaWin_GetCurrentProcessOwner((SID *)procOwnerSid, sizeof(procOwnerSid)))
	{
	  CfOut(cf_error,"","!! Could not get owner of current process");
	  return false;
	}

      if(EqualSid(attr.perms.owners->sid, procOwnerSid))
	{
	  switch (attr.transaction.action)
	    {
	    case cfa_warn:
          
	      cfPS(cf_error,CF_WARN,"",pp,attr," !! Owner on \"%s\" needs to be changed", file);
	      break;
          
	    case cfa_fix:
          
	      if (!DONTDO)
		{
		  if(!NovaWin_SetFileOwnership(file, (SID *)procOwnerSid))
		    {
		      CfOut(cf_error,"","!! Could not set owner of file \"%s\" to process owner", file);
		      return false;
		    }
		}
	      cfPS(cf_inform,CF_CHG,"",pp,attr,"-> Owner on \"%s\" successfully changed", file);
	      break;
          
	    default:
	      FatalError("cfengine: internal error: illegal file action\n");
	    }

	}
      else  // windows cannot change file owner to anything else than current process owner
	{
	  CfOut(cf_error,"","Windows is unable to implement a change owner policy, but file \"%s\" violates Cfengine's ownership promise (Windows can only change owner to user running Cfengine)", file);
	  return false;
	}
    }

  return true;
}

/*****************************************************************************/

/* Set user running the process as owner of file object pointed to by path. */
int NovaWin_TakeFileOwnership(char *path)
{
  char procOwnerSid[CF_BUFSIZE];

  if(!NovaWin_GetCurrentProcessOwner((SID *)procOwnerSid, sizeof(procOwnerSid)))
    {
      CfOut(cf_error,"","!! Could not get owner of current process");
      return false;
    }

  if(!NovaWin_SetFileOwnership(path, (SID *)procOwnerSid))
    {
      CfOut(cf_error,"","!! Could not set ownership of \"%s\"", path);
      return false;
    }
  
  return true;
}

/*****************************************************************************/

/* Tries to set the owner of file object pointed to by path to sid */
int NovaWin_SetFileOwnership(char *path, SID *sid)
{
  HANDLE currProcToken;
  DWORD setRes;
    
  if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &currProcToken))
    {
      CfOut(cf_error,"OpenProcessToken","!! Could not get access token of current process");
      return false;
    }


  // enable the SE_TAKE_OWNERSHIP_NAME privilege, to be able to take ownership of any file object
  if (!NovaWin_SetTokenPrivilege(currProcToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) 
    {
      CfOut(cf_error,"","!! Could not set 'SE_TAKE_OWNERSHIP_NAME' privilege - run Cfengine as an administrator");
      CloseHandle(currProcToken);
      return false;
    }
  
  
  // change owner to the current process's sid
  setRes = SetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, sid, NULL, NULL, NULL);
  
  if(setRes != ERROR_SUCCESS)
    {
      CfOut(cf_error, "SetNamedSecurityInfo", "Could not set owner of \"%s\"", path);
      CloseHandle(currProcToken);
      return false;
    }

  if (!NovaWin_SetTokenPrivilege(currProcToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) 
    {
      CfOut(cf_error,"","!! Could not disable 'SE_TAKE_OWNERSHIP_NAME' privilege");
    }
  
  CloseHandle(currProcToken);

  return true;
}

/*****************************************************************************/

void NovaWin_VerifyFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp)

{
Debug("NovaWin_VerifyFileAttributes()\n");

if (VerifyOwner(file,pp,attr,dstat))
   {
   /* nop */
   }

if(NovaWin_FileExists(file) && !NovaWin_IsDir(file))
   {
   VerifyFileIntegrity(file,attr,pp);
   }

if (attr.havechange)
   {
   VerifyFileChanges(file,dstat,attr,pp);
   }

if (attr.acl.acl_entries)
   { 
   VerifyACL(file,attr,pp); 
   }

if (attr.touch)
   {
   if (utime(file,NULL) == -1)
      {
      cfPS(cf_inform,CF_DENIED,"utime",pp,attr," !! Touching file %s failed",file);
      }
   else
      {
      cfPS(cf_inform,CF_CHG,"",pp,attr," -> Touching file %s",file);
      }
   }

Debug("NovaWin_VerifyFileAttributes(Done)\n"); 
}

/*****************************************************************************/

void NovaWin_VerifyCopiedFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp)

{
  SECURITY_DESCRIPTOR *secDesc = NULL;
  char savedOwner[CF_MAXSIDSIZE];
  SID *ownerSid;
  DWORD getRes;
  int noOwnerChange = false;

  Debug("NovaWin_VerifyCopiedFileAttributes(%s)\n",file); 

  if(!IsValidSid((attr.perms.owners)->uid))  // invalid sid indicates no owner change
    {
      noOwnerChange = true;
    }

  if(noOwnerChange)  // set current file owner as first element to avoid owner change
    {
      memcpy(savedOwner, (attr.perms.owners)->sid, CF_MAXSIDSIZE);
    
      getRes = GetNamedSecurityInfo(file, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, (PSID*)&ownerSid, NULL, NULL, NULL, &secDesc);

      if(getRes != ERROR_SUCCESS)
	{
	  CfOut(cf_error,"GetNamedSecurityInfo","!! Could not retreive existing owner of \"%s\"", file);
	  return;
	}

      if(!CopySid(CF_MAXSIDSIZE, (attr.perms.owners)->sid, ownerSid))
	{
	  CfOut(cf_error,"CopySid","!! Could not copy sid");
	  LocalFree(secDesc);
	  return;
	}

      LocalFree(secDesc);
    }

  VerifyFileAttributes(file,dstat,attr,pp);

  // reset the owner if we modified it
  if(noOwnerChange)
    {
      memset((attr.perms.owners)->sid, 0, CF_MAXSIDSIZE);
    }
}


#endif  /* MINGW */
