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
    CfOut(cf_verbose, "", "!! Could not get number of hard links for \"%s\", assuming 1", path);
    numHardLinks = 1;
    }
  
 // correct times 
 if(!GetFileAttributesEx(path, GetFileExInfoStandard, &attr))
    {
    CfOut(cf_error, "GetFileAttributesEx", "!! Could not get file attributes");
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

 if (nCode == 0)
    {
    CfOut(cf_verbose,"","Windows sockets successfully initialized.\n"); 
    }
 else
    {
    CfOut(cf_error,"","!! Winsock could not be initialized: WSAStartup() returned error code %d.\n", nCode);
    }
}

/*****************************************************************************/

void NovaWin_CloseNetwork()
{
 int res;

 res = WSACleanup();

 if(res == 0)
    {
    Debug("Windows sockets successfully deinitialized.\n");
    }
 else
    {
    CfOut(cf_error, "WSACleanup", "!! Could not close network (res=%d,sock-errcode=%d)", res, WSAGetLastError());
    }
}

/*****************************************************************************/

/* TODO: move the functions below to other files ? */

char *NovaWin_GetErrorStr(void)
{
 static char errbuf[CF_BUFSIZE];
 int len;

 memset(errbuf, 0, sizeof(errbuf));

 FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
               MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), errbuf, CF_BUFSIZE, NULL);

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

/*****************************************************************************/

int NovaWin_uname(struct utsname *buf)
/* Sample result:
 * --------------
 * sysname: WINDOWS_NT-5.1.2600 Workstation
 * nodename: ecs-laptop
 * release: Service Pack 3.0
 * version: Windows XP
 * machine: i686
 */
{
 OSVERSIONINFOEX osInfo;
 SYSTEM_INFO sysInfo;
 int intelArchNum;
 char hostName[MAXHOSTNAMELEN] = {0};
 char ip[MAXIP4CHARLEN];
 char *osType;

 GetSystemInfo(&sysInfo);

 // os version info
 memset(&osInfo, 0, sizeof(osInfo));

 osInfo.dwOSVersionInfoSize = sizeof(osInfo);
  
 if(!GetVersionEx((OSVERSIONINFO *)&osInfo))
    {
    CfOut(cf_error, "GetVersionEx", "!! Could not get os version info");
    return -1;
    }

 switch(osInfo.wProductType)
    {
    case VER_NT_WORKSTATION:
        osType = "workstation";
        break;

    case VER_NT_DOMAIN_CONTROLLER:
        osType = "domain controller";
        break;

    case VER_NT_SERVER:
        osType = "server";
        break;

    default:
        osType = "unknown-ostype";
        break;
    }

 snprintf(buf->sysname, _SYS_NMLN, "WINDOWS_NT-%lu.%lu.%lu %s", osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber, osType);
 
 WINVER_MAJOR = osInfo.dwMajorVersion;
 WINVER_MINOR = osInfo.dwMinorVersion;
 WINVER_BUILD = osInfo.dwBuildNumber;


 // release - set to service pack number

 snprintf(buf->release, _SYS_NMLN, "Service Pack %d.%d", osInfo.wServicePackMajor, osInfo.wServicePackMinor);

 // version - set to human friendly string representing OS version
 // TODO: Update on new OS releases
 switch(osInfo.dwMajorVersion)  // the joy of 10 products for every release...
    {                            // see http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
    case 6:

        switch(osInfo.dwMinorVersion)
           {
           case 1:

               if(osInfo.wProductType == VER_NT_WORKSTATION)
                  {
                  snprintf(buf->version, _SYS_NMLN, "Windows 7");
                  }
               else
                  {
                  snprintf(buf->version, _SYS_NMLN, "Windows Server 2008 R2");
                  }

               break;

           case 0:

               if(osInfo.wProductType == VER_NT_WORKSTATION)
                  {
                  snprintf(buf->version, _SYS_NMLN, "Windows Vista");
                  }
               else
                  {
                  snprintf(buf->version, _SYS_NMLN, "Windows Server 2008");
                  }
	  
               break;

           default:
               snprintf(buf->version, _SYS_NMLN, "unknown-osrelease");
               break;
           }

        break;

    case 5:

        switch(osInfo.dwMinorVersion)
           {
           case 2:

               if(osInfo.wProductType == VER_NT_WORKSTATION)
                  {
                  if(sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                     {
                     snprintf(buf->version, _SYS_NMLN, "Windows XP Professional x64 Edition");
                     }
                  else
                     {
                     snprintf(buf->version, _SYS_NMLN, "unknown-osrelease");
                     }
                  }
               else  // server
                  {
                  if(GetSystemMetrics(SM_SERVERR2) != 0)
                     {
                     snprintf(buf->version, _SYS_NMLN, "Windows Server 2003 R2");
                     }
                  else if(osInfo.wSuiteMask == VER_SUITE_WH_SERVER)
                     {
                     snprintf(buf->version, _SYS_NMLN, "Windows Home Server");
                     }
                  else
                     {
                     snprintf(buf->version, _SYS_NMLN, "Windows Server 2003");
                     }
                  }

               break;

           case 1:

               snprintf(buf->version, _SYS_NMLN, "Windows XP");

               break;

           case 0:
	  
               snprintf(buf->version, _SYS_NMLN, "Windows 2000");
	  
               break;

           default:
               snprintf(buf->version, _SYS_NMLN, "unknown-osrelease");
               break;
           }

        break;

    default:
        snprintf(buf->version, _SYS_NMLN, "unknown-osrelease");
        break;
    }
  

 // architecture

 switch(sysInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:

        snprintf(buf->machine, _SYS_NMLN, "x86_64");

        break;

    case PROCESSOR_ARCHITECTURE_IA64:

        snprintf(buf->machine, _SYS_NMLN, "ia64");

        break;

    case PROCESSOR_ARCHITECTURE_INTEL:

        if(sysInfo.wProcessorLevel <= 3)
           {
           intelArchNum = 3;
           }
        else if(sysInfo.wProcessorLevel >= 6)
           {
           intelArchNum = 6;
           }
        else
           {
           intelArchNum = sysInfo.wProcessorLevel;  // 4 or 5 (-86)
           }

        snprintf(buf->machine, _SYS_NMLN, "i%d86", intelArchNum);

        break;

    case PROCESSOR_ARCHITECTURE_MIPS:  // mobile platforms following
      
        snprintf(buf->machine, _SYS_NMLN, "mips r%d000", sysInfo.wProcessorLevel);
      
        break;

    case PROCESSOR_ARCHITECTURE_SHX:

        snprintf(buf->machine, _SYS_NMLN, "sh%u", sysInfo.wProcessorLevel);

        break;

    case PROCESSOR_ARCHITECTURE_ARM:

        snprintf(buf->machine, _SYS_NMLN, "arm%u", sysInfo.wProcessorLevel);

        break;

    default:

        snprintf(buf->machine, _SYS_NMLN, "unknown");

        break;
    }

 // hostname
 if(!GetMyHostInfo(hostName, ip))
    {
    CfOut(cf_error, "", "!! Could not get this host's hostname");
    snprintf(buf->nodename, _SYS_NMLN, "%s", "UNKNOWN_HOSTNAME");
    }
  else
    {
    snprintf(buf->nodename, _SYS_NMLN, "%s", hostName);
    }
  
 return 0;
}


#endif  /* MINGW */
