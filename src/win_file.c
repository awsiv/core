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


// TODO: Implement chek for executable bit?
int NovaWin_IsExecutable(char *file)
{
  return NovaWin_FileExists(file);
}


int NovaWin_mkdir(const char *path, mode_t mode)
{
  // no mode on windows, use windows mkdir - return value seems compatible

  return _mkdir(path);

  // TODO: run chmod(path,mode) here ?
}


/* Unix rename() overwrites newpath if it exists, windows rename() does not.
 * We fix this by removing the file first if it exists */
int NovaWin_rename(const char *oldpath, const char *newpath)
{
  unlink(newpath);

  return rename(oldpath, newpath);
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

  if(attr.perms.owners == NULL || !IsValidSid(attr.perms.owners->sid))  // no owner set
    {
      return true;
    }

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

/* Gets the name of the entity owning file object in 'path', and writes it to 'owner' */
int NovaWin_GetOwnerName(char *path, char *owner, int ownerSz)
{
  SECURITY_DESCRIPTOR *secDesc;
  SID *ownerSid;
  DWORD getRes;

  memset(owner, 0, ownerSz);

  getRes = GetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, (PSID*)&ownerSid, NULL, NULL, NULL, &secDesc);

  if(getRes != ERROR_SUCCESS)
    {
      CfOut(cf_error,"GetNamedSecurityInfo","!! Could not retreive existing owner of \"%s\"", path);
      return false;
    }
  
  if(!NovaWin_SidToName(ownerSid, owner, ownerSz))
    {
      CfOut(cf_error,"","!! Could not get owner name of \"%s\"", path);
      LocalFree(secDesc);
      return false;
    }

  LocalFree(secDesc);

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
  // TODO: Correct assumption?: if attr.owner.sid is invalid, it will not be changed - no need to backup like on Unix
  NovaWin_VerifyFileAttributes(file,dstat,attr,pp);
}

/*****************************************************************************/

/* Returns the amount of free space, in kilobytes (if type type is
 * avail) or percent, on the device where file resides, or CF_INFINITY
 * on error. 
 * NOTE: Total length returned may be dependent on which account we
 * are calling from if disk quotas are being used - may be fixed with
 * CreateFile() + DeviceIoControl(IOCTL_DISK_GET_LENGTH_INFO).
 * TODO: Increase returned datatype range for supporting > 2 TB (-> 64 bit)? */
int NovaWin_GetDiskUsage(char *file,enum cfsizes type)
{
  ULARGE_INTEGER bytesLenCaller, bytesFree, kbLenCaller, kbFree;
  
  
  if(!GetDiskFreeSpaceEx(file, NULL, &bytesLenCaller, &bytesFree))
    {
      printf("Error getting free disk space");
      CfOut(cf_error, "GetDiskFreeSpaceEx", "Could not get disk space status for \"%s\"", file);
      return CF_INFINITY;
    }
  
  kbLenCaller.QuadPart = bytesLenCaller.QuadPart / 1000;
  kbFree.QuadPart = bytesFree.QuadPart / 1000;
  
if (type == cfabs)
   {
     return (int)kbFree.QuadPart;
   }
else
   {
     return ((double)kbFree.QuadPart / (double)kbLenCaller.QuadPart) * 100;
   }
}

/*****************************************************************************/

int NovaWin_GetNumHardlinks(char *path, int *numHardLinks)
{
  HANDLE fp;
  BY_HANDLE_FILE_INFORMATION finfo;

  if(IsDir(path))
    {
      *numHardLinks = 1;
      return true;
    }

  fp = CreateFile(path, 0, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    
  if(fp == INVALID_HANDLE_VALUE)
    {
      CfOut(cf_error, "CreateFile", "!! Could not open file \"%s\"", path);
      CloseHandle(fp);
      return false;
    }
  
  if(!GetFileInformationByHandle(fp, &finfo))
    {
      CfOut(cf_error, "GetFileInformationByHandle", "!! Could not get information on file \"%s\"", path);
      CloseHandle(fp);
      return false;
    }

  *numHardLinks = finfo.nNumberOfLinks;

  CloseHandle(fp);

  return true;
}



#endif  /* MINGW */
