/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "acl.h"
#include "dir.h"
#include "dir_priv.h"
#include "files_names.h"
#include "files_operators.h"
#include "cfstream.h"
#include "logging.h"

/* _mkdir(3) */
#include <direct.h>

void CreateEmptyFile(char *name)
{
    HANDLE fileHandle;

    if (unlink(name) == -1)
    {
        CfDebug("Pre-existing object %s could not be removed or was not there\n", name);
    }

    fileHandle = CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        CfOut(cf_error, "CreateFile", "!! Could not create empty file");
        return;
    }

    CloseHandle(fileHandle);
}

/*****************************************************************************/

FILE *NovaWin_FileHandleToStream(HANDLE fHandle, char *mode)
{
    int crtHandle;
    int flags;

    if (strncmp(mode, "r", 2) == 0)
    {
        flags = _O_RDONLY;
    }
    else if (strncmp(mode, "w", 2) == 0)
    {
        flags = _O_APPEND;
    }
    else
    {
        CfOut(cf_error, "NovaWin_FileHandleToStream", "!! Mode is not 'r' or 'w', but '%s'", mode);
        return NULL;
    }

    crtHandle = _open_osfhandle((intptr_t) fHandle, flags);

    if (crtHandle == -1)
    {
        CfOut(cf_error, "_open_osfhandle", "!! Could not convert file handle");
        return NULL;
    }

    return _fdopen(crtHandle, mode);
}

// TODO: Implement chek for executable bit?
int IsExecutable(const char *file)
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
        CfOut(cf_verbose, "", "The file \"%s\" does not exist\n", fileName);
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

    if (fileAttr == INVALID_FILE_ATTRIBUTES)
    {
        CfOut(cf_error, "", "!! No file object exsists in path \"%s\"", fileName);
        return false;
    }

    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

int VerifyOwner(char *file, Promise *pp, Attributes attr, struct stat *sb)
{
    SECURITY_DESCRIPTOR *secDesc;
    char procOwnerSid[CF_BUFSIZE];
    UidList *ulp;
    SID *ownerSid;
    int sidMatch = false;
    DWORD getRes;

    if (!BOOTSTRAP && !Nova_CheckLicenseWin("VerifyOwner"))
    {
        return false;
    }

    if (attr.perms.owners == NULL || !IsValidSid(attr.perms.owners->sid))       // no owner set
    {
        return true;
    }

    getRes =
        GetNamedSecurityInfo(file, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, (PSID *) & ownerSid, NULL, NULL, NULL,
                             (void **)&secDesc);

    if (getRes != ERROR_SUCCESS)
    {
        CfOut(cf_error, "GetNamedSecurityInfo", "!! Could not retreive existing owner of \"%s\"", file);
        return false;
    }

    // check if file owner is as it should be
    for (ulp = attr.perms.owners; ulp != NULL; ulp = ulp->next)
    {
        if (EqualSid(ownerSid, ulp->sid))
        {
            sidMatch = true;
            break;
        }
    }

    LocalFree(secDesc);

    if (sidMatch)
    {
        cfPS(cf_inform, CF_NOP, "", pp, attr, "-> Owner of \"%s\" needs no modification", file);
    }
    else
    {
        CfOut(cf_verbose, "", "Changing owner of file \"%s\" to the first listed", file);

        if (!NovaWin_GetCurrentProcessOwner((SID *) procOwnerSid, sizeof(procOwnerSid)))
        {
            CfOut(cf_error, "", "!! Could not get owner of current process");
            return false;
        }

        if (EqualSid(attr.perms.owners->sid, procOwnerSid))
        {
            switch (attr.transaction.action)
            {
            case cfa_warn:

                cfPS(cf_error, CF_WARN, "", pp, attr, " !! Owner on \"%s\" needs to be changed", file);
                break;

            case cfa_fix:

                if (!DONTDO)
                {
                    if (!NovaWin_SetFileOwnership(file, (SID *) procOwnerSid))
                    {
                        CfOut(cf_error, "", "!! Could not set owner of file \"%s\" to process owner", file);
                        return false;
                    }
                }
                cfPS(cf_inform, CF_CHG, "", pp, attr, "-> Owner on \"%s\" successfully changed", file);
                break;

            default:
                FatalError("cfengine: internal error: illegal file action\n");
            }

        }
        else                    // windows cannot change file owner to anything else than current process owner
        {
            CfOut(cf_error, "",
                  "Windows is unable to implement a change owner policy, but file \"%s\" violates Cfengine's ownership promise (Windows can only change owner to user running Cfengine)",
                  file);
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

    if (!NovaWin_GetCurrentProcessOwner((SID *) procOwnerSid, sizeof(procOwnerSid)))
    {
        CfOut(cf_error, "", "!! Could not get owner of current process");
        return false;
    }

    if (!NovaWin_SetFileOwnership(path, (SID *) procOwnerSid))
    {
        CfOut(cf_error, "", "!! Could not set ownership of \"%s\"", path);
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

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &currProcToken))
    {
        CfOut(cf_error, "OpenProcessToken", "!! Could not get access token of current process");
        return false;
    }

    // enable the SE_TAKE_OWNERSHIP_NAME privilege, to be able to take ownership of any file object
    if (!NovaWin_SetTokenPrivilege(currProcToken, SE_TAKE_OWNERSHIP_NAME, TRUE))
    {
        CfOut(cf_error, "", "!! Could not set 'SE_TAKE_OWNERSHIP_NAME' privilege - run Cfengine as an administrator");
        CloseHandle(currProcToken);
        return false;
    }

    // change owner to the current process's sid
    setRes = SetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, sid, NULL, NULL, NULL);

    if (setRes != ERROR_SUCCESS)
    {
        CfOut(cf_error, "SetNamedSecurityInfo", "Could not set owner of \"%s\"", path);
        CloseHandle(currProcToken);
        return false;
    }

    if (!NovaWin_SetTokenPrivilege(currProcToken, SE_TAKE_OWNERSHIP_NAME, TRUE))
    {
        CfOut(cf_error, "", "!! Could not disable 'SE_TAKE_OWNERSHIP_NAME' privilege");
    }

    CloseHandle(currProcToken);

    return true;
}

/*****************************************************************************/

/* Gets the name of the entity owning file object in 'path', and writes it to 'owner' */
int GetOwnerName(char *path, struct stat *lstatptr, char *owner, int ownerSz)
{
    SECURITY_DESCRIPTOR *secDesc;
    SID *ownerSid;
    DWORD getRes;

    memset(owner, 0, ownerSz);

    getRes =
        GetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, (PSID *) & ownerSid, NULL, NULL, NULL,
                             (void **)&secDesc);

    if (getRes != ERROR_SUCCESS)
    {
        CfOut(cf_error, "GetNamedSecurityInfo", "!! Could not retreive existing owner of \"%s\"", path);
        return false;
    }

    if (!NovaWin_SidToName(ownerSid, owner, ownerSz))
    {
        CfOut(cf_error, "", "!! Could not get owner name of \"%s\"", path);
        LocalFree(secDesc);
        return false;
    }

    LocalFree(secDesc);

    return true;
}

/*****************************************************************************/

/* Returns the amount of free space, in kilobytes (if type type is
 * avail) or percent, on the device where file resides, or CF_INFINITY
 * on error. 
 * NOTE: Total length returned may be dependent on which account we
 * are calling from if disk quotas are being used - may be fixed with
 * CreateFile() + DeviceIoControl(IOCTL_DISK_GET_LENGTH_INFO).*/

off_t GetDiskUsage(char *file, enum cfsizes type)
{
    ULARGE_INTEGER bytesLenCaller, bytesFree, kbLenCaller, kbFree;

    if (!Nova_CheckLicenseWin("NovaWin_GetDiskUsage"))
    {
        return 0;
    }

    if (!GetDiskFreeSpaceEx(file, NULL, &bytesLenCaller, &bytesFree))
    {
        printf("Error getting free disk space");
        CfOut(cf_error, "GetDiskFreeSpaceEx", "Could not get disk space status for \"%s\"", file);
        return CF_INFINITY;
    }

    kbLenCaller.QuadPart = bytesLenCaller.QuadPart / 1000;
    kbFree.QuadPart = bytesFree.QuadPart / 1000;

    if (type == cfabs)
    {
        return (int) kbFree.QuadPart;
    }
    else
    {
        return ((double) kbFree.QuadPart / (double) kbLenCaller.QuadPart) * 100;
    }
}

/*****************************************************************************/

int NovaWin_GetNumHardlinks(char *path, int *numHardLinks)
{
    HANDLE fp;
    BY_HANDLE_FILE_INFORMATION finfo;

    *numHardLinks = 1;

    if (IsDir(path))
    {
        *numHardLinks = 1;
        return true;
    }

    fp = CreateFile(path, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fp == INVALID_HANDLE_VALUE)
    {
        CfOut(cf_verbose, "CreateFile", "!! Could not open file \"%s\"", path);
        CloseHandle(fp);
        return false;
    }

    if (!GetFileInformationByHandle(fp, &finfo))
    {
        CfOut(cf_verbose, "GetFileInformationByHandle", "!! Could not get information on file \"%s\"", path);
        CloseHandle(fp);
        return false;
    }

    *numHardLinks = finfo.nNumberOfLinks;

    CloseHandle(fp);

    return true;
}

/*****************************************************************************/

Dir *OpenDirLocal(const char *dirname)
{
    Dir *ret;
    HANDLE searchHandle;
    WIN32_FIND_DATA data;
    char pattern[MAX_PATH];

    snprintf(pattern, MAX_PATH, "%s\\*", dirname);

    ret = xcalloc(1, sizeof(Dir));

    ret->entrybuf = xcalloc(1, sizeof(struct dirent));

    ret->dirh = searchHandle = FindFirstFile(pattern, &data);

    if (searchHandle == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_NOT_FOUND)
    {
        free(ret);
        return NULL;
    }

/*
 * Hack: we store 1 in dirent->d_ino to mark "this has not been displayed" after
 * FindFirstFile. Better have it encapsulated somewhere in subtype of Dir.
 */
    ret->entrybuf->d_ino = 1;
    ret->entrybuf->d_namlen = strlen(data.cFileName);
    strlcpy(ret->entrybuf->d_name, data.cFileName, sizeof(ret->entrybuf->d_name));

    return ret;
}

/*****************************************************************************/

const struct dirent *ReadDirLocal(Dir *dir)
{
    if ((HANDLE) dir->dirh == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

/* See comment in OpenDirLocal */
    if (dir->entrybuf->d_ino == 0)
    {
        WIN32_FIND_DATA data;

        if (FindNextFile((HANDLE) dir->dirh, &data))
        {
            dir->entrybuf->d_namlen = strlen(data.cFileName);
            strlcpy(dir->entrybuf->d_name, data.cFileName, sizeof(dir->entrybuf->d_name));
            return dir->entrybuf;
        }
        else
        {
            FindClose((HANDLE) dir->dirh);
            dir->dirh = INVALID_HANDLE_VALUE;
            return NULL;
        }
    }
    else
    {
        dir->entrybuf->d_ino = 0;
        return dir->entrybuf;
    }
}

/*****************************************************************************/

void CloseDirLocal(Dir *dir)
{
    if ((HANDLE) dir->dirh != INVALID_HANDLE_VALUE)
    {
        FindClose((HANDLE) dir->dirh);
    }
    free(dir->entrybuf);
    free(dir);
}

/*****************************************************************************/

struct dirent *AllocateDirentForFilename(const char *filename)
{
    struct dirent *entry = xcalloc(1, sizeof(struct dirent));

    strcpy(entry->d_name, filename);
    return entry;
}
