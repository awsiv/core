/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_api.c                                                           */
/*                                                                           */
/* Created: Fri Sep 11 13:13:29 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW
/* ------ BEGIN TEMP DEFINES ------ */

typedef unsigned long uid_t;
typedef unsigned long gid_t;
typedef unsigned short mode_t;
typedef int	pid_t;

/* ------ END TEMP DEFINES ------ */

// always returns 0 (assumes process is run by root/Administrator)
uid_t getuid(void)
{
  return 0;
}

/*****************************************************************************/

// always returns 0 (assumes process is run by group root)
gid_t getgid(void)
{
  return 0;
}

/*****************************************************************************/

// there are no symbolic links on NT, so stat and lstat are equivalent
int lstat(const char *file_name, struct stat *buf)
{
  return cfstat(file_name, buf);
}

/*****************************************************************************/

unsigned int sleep(unsigned int seconds)
{
  // TODO: Note: Use SleepEx() if it should be alertable when waiting for IO
  // return values differ when signaled
  Sleep((seconds)*1000);

  // on windows, the sleep seconds have always elapsed when we return (i.e. no signals are received)
  return 0;
}

/*****************************************************************************/

/* Change owner of file pointed to by path. If owner and group id are 0,
 * the owner is set to the "BUILTIN\Administrators" group. Otherwise, the function fails.
 * Returns 0 on success, and -1 on failure. */
int chown(const char *path, uid_t owner, gid_t group)
{
  if(owner == 0 && group == 0)
    {
      // change owner to the owner of the current process
      
      if(!NovaWin_TakeFileOwnership((char *)path))
	{
	  CfOut(cf_error,"","!! Could not change owner of \"%s\" to owner of current process", path);
	  return -1;
	}
      
      Debug("Owner of \"%s\" is set to the owner of the current process", path);

      return 0;
    }
  else
    {
      CfOut(cf_error,"","!! Owner and group are not both 0: such numerical user or group ids makes no sense on NT");
      return -1;
    }
}

/*****************************************************************************/

/* TODO: Implement ? */
int NovaWin_chmod(const char *path, mode_t mode)
{
  if(NovaWin_FileExists(path))
    {
      return 0;
    }
  else
    {
      CfOut(cf_error, "NovaWin_chmod", "File \"%s\" not found");
      return -1;
    }
}

/*****************************************************************************/

/* TODO: Implement ? Used only one place in Cf3. */
int fchmod(int fildes, mode_t mode)
{
  return 0;
}

/*****************************************************************************/

long int random(void)
{
  return rand();
}

/*****************************************************************************/

void srandom(unsigned int seed)
{
  srand(seed);
}

/*****************************************************************************/

void setlinebuf(FILE *stream)
{
  setvbuf(stream, (char *)NULL, _IOLBF, 0);
}

/*****************************************************************************/

int NovaWin_stat(const char *path, struct stat *statBuf)
/* Implementation of stat() which gives better times and correct nlinks */
{
  WIN32_FILE_ATTRIBUTE_DATA attr;
  int numHardLinks;
  int statRes;
  
  statRes = stat(path, statBuf);

  if(statRes != 0)
    {
      return statRes;
    }

  if(!NovaWin_GetNumHardlinks((char *)path, &numHardLinks))
    {
      CfOut(cf_error, "", "Could not get number of hard links");
      return -1;
    }
  
  // correct times 
  if(!GetFileAttributesEx(path, GetFileExInfoStandard, &attr))
    {
      CfOut(cf_error, "GetFileAttributesEx", "Could not get file attributes");      
      return -1;
    }
  
  statBuf->st_nlink = (short)numHardLinks;
  statBuf->st_atime = NovaWin_FileTimeToTimet(&(attr.ftLastAccessTime));
  statBuf->st_mtime = NovaWin_FileTimeToTimet(&(attr.ftLastWriteTime));
  statBuf->st_ctime = NovaWin_FileTimeToTimet(&(attr.ftCreationTime)); // set to creation time  
  
  return 0;
  
}

/*****************************************************************************/

/* Start up Winsock */
void NovaWin_OpenNetwork()
{ 
struct WSAData wsaData;
int nCode;

nCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

if (nCode != 0)
  {
  CfOut(cf_error,"","!! Winsock could not be initialized: WSAStartup() returned error code %d.\n", nCode);
  }
else
  {
  CfOut(cf_verbose,"","Windows sockets successfully initialized.\n"); 
  }
}

/*****************************************************************************/

void NovaWin_CloseNetwork()
{
  if(!WSACleanup())
    {
      CfOut(cf_error, "WSACleanup", "!! Could not close network");
    }
  else
    {
      CfOut(cf_verbose,"","Windows sockets successfully deinitialized.\n"); 
    }
}

/*****************************************************************************/

/* TODO: move the functions below to other files ? */

char *NovaWin_GetErrorStr(void)
{
  static char errbuf[CF_BUFSIZE];
  int len;

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errbuf, CF_BUFSIZE, NULL);

  // remove CRLF from end
  len = strlen(errbuf);
  errbuf[len-2] = errbuf[len-1] = '\0';
  
  return errbuf;
}

/*****************************************************************************/

time_t NovaWin_FileTimeToTimet(FILETIME *ft)
{
  LARGE_INTEGER li;
  
  li.LowPart = ft->dwLowDateTime;
  li.HighPart = ft->dwHighDateTime;

  li.QuadPart = ( li.QuadPart - 0x19DB1DED53E8000 ) / 10000000;

  return (time_t)(li.QuadPart);
}


#endif  /* MINGW */
