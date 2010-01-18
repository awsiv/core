/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_ps.c                                                            */
/*                                                                           */
/* Created: Tue Sep 15 13:13:10 2009                                         */
/*                                                                           */
/*****************************************************************************/

/* Implementation of process table query functions */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

/* defines */
#define MAXCMDSTR 512  // max length of process commmand line (e.g. "c:\program.exe --param")
#define TIMESTAMP_WAIT 500  // msecs between checking two process timestamps (for % CPU usage)


#ifndef HAVE_PROCESS_MEMORY_COUNTERS_EX

typedef struct _PROCESS_MEMORY_COUNTERS_EX {
   DWORD cb;
   DWORD PageFaultCount;
   SIZE_T PeakWorkingSetSize;
   SIZE_T WorkingSetSize;
   SIZE_T QuotaPeakPagedPoolUsage;
   SIZE_T QuotaPagedPoolUsage;
   SIZE_T QuotaPeakNonPagedPoolUsage;
   SIZE_T QuotaNonPagedPoolUsage;
   SIZE_T PagefileUsage;
   SIZE_T PeakPagefileUsage;
   SIZE_T PrivateUsage;
} PROCESS_MEMORY_COUNTERS_EX;
typedef PROCESS_MEMORY_COUNTERS_EX *PPROCESS_MEMORY_COUNTERS_EX;

#endif  /* NOT HAVE_PROCESS_MEMORY_COUNTERS_EX */


/* static prototypes */
static char *GetProcessInfo(DWORD pid, char *execName, ULARGE_INTEGER lastTimeStamp, DWORDLONG totalPhysMemB);
static int EnableDebugPriv(void);
//static int GetProcessCmdLine(HANDLE procHandle, char *cmdLineStr, int cmdLineSz);
static void GetProcessTime(HANDLE procHandle, char *timeCreateStr, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr);
static void FormatCpuTime(FILETIME *timeKernel, FILETIME *timeUser, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr);
static void FormatCreationTime(FILETIME *timeCreate, char *timeCreateStr);
static void GetMemoryInfo(HANDLE procHandle, char *memSzStr, DWORDLONG totalPhysMemB);
static void GetProcessCpuTime(DWORD pid, ULARGE_INTEGER *timeCpuInt);
static void GetProcessUserName(HANDLE procHandle, int incDomain, char *nameStr, int nameStrSz);

/*****************************************************************************/

int NovaWin_LoadProcessTable(struct Item **procdata)
/* Creates a list with current process table, and saves lists of SYSTEM
 * and non-SYSTEM processes to files.
 */
{
 struct Item *systemprocs = NULL;
 struct Item *otherprocs = NULL;
 char buf[CF_BUFSIZE];

 if(LICENSES == 0)
    {
    return false;
    }

 if(!NovaWin_GetProcessSnapshot(procdata))
    {
    CfOut(cf_error, "", "!! Could not get process snapshot");
    return false;
    }

 snprintf(buf,CF_MAXVARSIZE,"%s%cstate%ccf_procs",CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);
 RawSaveItemList(*procdata,buf);

 // Split to SYSTEM and non-SYSTEM processes

 CopyList(&systemprocs,*procdata);
 CopyList(&otherprocs,*procdata);

 while (DeleteItemNotContaining(&systemprocs,"SYSTEM"))
    {
    }
 // header was removed too, add it again
 PrependItem(&systemprocs, (*procdata)->name, NULL);

 while (DeleteItemContaining(&otherprocs,"SYSTEM"))
    {
    }

 snprintf(buf,CF_MAXVARSIZE,"%s%cstate%ccf_rootprocs",CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);
 RawSaveItemList(systemprocs,buf);
 DeleteItemList(systemprocs);

 snprintf(buf,CF_MAXVARSIZE,"%s%cstate%ccf_otherprocs",CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);
 RawSaveItemList(otherprocs,buf);
 DeleteItemList(otherprocs);

 return true;
}

/*****************************************************************************/

int NovaWin_GetProcessSnapshot(struct Item **procdata)
/*
 * Returns the equivalent of "ps auxw" in a item list - the current process table
 */
{
 HANDLE processSnap;
 PROCESSENTRY32 pe32;
 ULARGE_INTEGER cpuTimeStamps[CF_BUFSIZE];
 MEMORYSTATUSEX memInfo;
 char buf[CF_BUFSIZE];
 char *psLine;
 int i;

 if(LICENSES == 0)
    {
    return false;
    }
 
 // need debug priviliges to open some special processes, like crss.exe, alg.exe, svchost.exe
 if(!EnableDebugPriv())
    {
    CfOut(cf_error,"EnableDebugPriv","!! Current account needs 'Debug programs' privilige to get info on all processes");
    }

 // get total memory (needed for process % mem usage)
 memInfo.dwLength = sizeof(MEMORYSTATUSEX);
 if(!GlobalMemoryStatusEx(&memInfo))
    {
    CfOut(cf_error,"GlobalMemoryStatusEx","!! Could not obtain memory information");
    memInfo.ullTotalPhys = 1;
    }

 // take snapshot of all processes in the system
 processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

 if(processSnap == INVALID_HANDLE_VALUE)
    {
    CfOut(cf_error, "CreateToolhelp32Snapshot", "!! Could not get process snapshot");
    return false;
    }

 pe32.dwSize = sizeof(pe32);

 // retrieve information about the first process
 if(!Process32First(processSnap,&pe32))
    {
    CfOut(cf_error, "Process32First", "!! Could not get first entry in process snapshot");
    CloseHandle(processSnap);
    return false;
    }

 // get cpu timestamps for all processes
 for(i = 0; i < CF_BUFSIZE; i++)
    {
    GetProcessCpuTime(pe32.th32ProcessID, &cpuTimeStamps[i]);

    if(!Process32Next(processSnap, &pe32))
       {
       break;
       }
    }

 if(i == CF_BUFSIZE)
    {
    CfOut(cf_error, "", "!! More than %d processes on this system - a larger buffer is needed", CF_BUFSIZE);
    }

 Sleep(TIMESTAMP_WAIT);

 if(!Process32First(processSnap,&pe32))
    {
    CfOut(cf_error, "Process32First", "!! Could not get first entry in process snapshot");
    CloseHandle(processSnap);
    return false;
    }

 // get rest of info for all processes
 for(i = 0; i < CF_BUFSIZE; i++)
    {
    psLine = GetProcessInfo(pe32.th32ProcessID, pe32.szExeFile, cpuTimeStamps[i], memInfo.ullTotalPhys);

    if(psLine)
       {
       AppendItem(procdata,psLine,"");
       }

    if(!Process32Next(processSnap, &pe32))
       {
       break;
       }
    }

 CloseHandle(processSnap);

 // save process list output to files, include header
 snprintf(buf, sizeof(buf), "%-20s %5s %s %s %8s %8s %-3s %s %s %5s %s",
          "USER", "PID", "%CPU", "%MEM", "VSZ", "RSS", "TTY", "STAT", "START", "TIME", "COMMAND");

 PrependItem(procdata, buf, NULL); 

 return true;
}


/*****************************************************************************/

int NovaWin_GatherProcessUsers(struct Item **userList, int *userListSz, int *numRootProcs, int *numOtherProcs)
{
 
 
 return true;
}


/*****************************************************************************/


static char *GetProcessInfo(DWORD pid, char *execName, ULARGE_INTEGER lastTimeStamp, DWORDLONG totalPhysMemB)
{
 static char psLine[CF_BUFSIZE];
 HANDLE procHandle;
 char cmdLineStr[MAXCMDSTR];
 char timeCreateStr[16], timeCpuStr[16], memSzStr[32];
 ULARGE_INTEGER newTimeStamp;
 __int64 cpuTimeDiff;
 float cpuUsagePercent;
 char userName[CF_BUFSIZE];


 if(pid == 0 || pid == 4)
    {
    Debug("Skipped process info query from special process (pid=%d)", pid);
    return NULL;
    }

 procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | STANDARD_RIGHTS_READ, FALSE, pid);

 if(procHandle == NULL)
    {
    CfOut(cf_error,"OpenProcess","!! Error opening process handle for pid=%lu", pid);
    return NULL;
    }

 GetProcessTime(procHandle, timeCreateStr, &newTimeStamp, timeCpuStr);

 GetProcessUserName(procHandle, false, userName, sizeof(userName));

 GetMemoryInfo(procHandle, memSzStr, totalPhysMemB);

 // process name and parameters - not obtainable for "System" process
 switch(pid)
    {
    case 4:
    case 8:
        strcpy(cmdLineStr, "System");
        break;

    default:
        //GetProcessCmdLine(procHandle, cmdLineStr, sizeof(cmdLineStr));
        snprintf(cmdLineStr, sizeof(cmdLineStr), "%s", execName);  // use only execuable image name for now
    }

 CloseHandle(procHandle);

 // calulcate % CPU usage
 cpuTimeDiff = newTimeStamp.QuadPart - lastTimeStamp.QuadPart;

 // we need a valid earlier timestamp and a nonzero diff
 if((cpuTimeDiff != 0) && (lastTimeStamp.QuadPart != 0))
    {
    int promille = cpuTimeDiff / (TIMESTAMP_WAIT * 10);
    cpuUsagePercent = (float)promille / 10;
    }
 else
    {
    cpuUsagePercent = 0;
    }

 snprintf(psLine, sizeof(psLine), "%-20.20s %5lu %4.1f %s %s %s %s %s %s",
          userName, pid, cpuUsagePercent, memSzStr, "?  ", "?   ", timeCreateStr, timeCpuStr, cmdLineStr);

 return psLine;
}

/*****************************************************************************/

static int EnableDebugPriv(void)
{
 HANDLE hToken = NULL;
 LUID sedebugnameValue;
 TOKEN_PRIVILEGES tkp;

 if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
 return false;
 }

 if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {
 CloseHandle(hToken);
 return false;
 }

 tkp.PrivilegeCount = 1;
 tkp.Privileges[0].Luid = sedebugnameValue;
 tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

 if ((AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL) != ERROR_SUCCESS) && (GetLastError() != ERROR_SUCCESS)) {
 CloseHandle(hToken);
 return false;
 }

 CloseHandle(hToken);

 return true;
}

/*****************************************************************************/

// NOTE: Only works for Vista/Server2008 +
/* static int GetProcessCmdLine(HANDLE procHandle, char *cmdLineStr, int cmdLineSz) */
/* { */
/*   DWORD cmdLineSzWritten = (DWORD)cmdLineSz; */

/*   if(!QueryFullProcessImageName(procHandle, 0, cmdLineStr, &cmdLineSzWritten)) */
/*     { */
/*       CfOut(cf_error, "QueryFullProcessImageName", "Could not get process image name"); */
/*       snprintf(cmdLineStr, cmdLineSz, "???"); */
/*       return false; */
/*     } */

/*   return true; */
/* } */


/*****************************************************************************/

static void GetProcessTime(HANDLE procHandle, char *timeCreateStr, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr)
{
 FILETIME timeCreate, timeExit, timeKernel, timeUser;

 if(!GetProcessTimes(procHandle, &timeCreate, &timeExit, &timeKernel, &timeUser))
    {
    CfOut(cf_error,"GetProcessTimes","!! Could not obtain process times");
    return;
    }

 FormatCreationTime(&timeCreate, timeCreateStr);

 FormatCpuTime(&timeKernel, &timeUser, timeCpuInt, timeCpuStr);
}

/*****************************************************************************/

// creation time shold be hh:mm if started the same date,
// the date as MmmDD if started the same year, and YYYY otherwise
// everything is interpredet as local time
static void FormatCreationTime(FILETIME *timeCreate, char *timeCreateStr)
{
 SYSTEMTIME timeCreateUTC, timeCreateLocal, timeCurrent;

 // convert creation time to local time
 FileTimeToSystemTime(timeCreate, &timeCreateUTC);

 SystemTimeToTzSpecificLocalTime(NULL, &timeCreateUTC, &timeCreateLocal);

 // check creation time against current time
 GetLocalTime(&timeCurrent);

 if(timeCreateLocal.wYear == timeCurrent.wYear)
    {
    if(timeCreateLocal.wMonth == timeCurrent.wMonth &&
       timeCreateLocal.wDay == timeCurrent.wDay) // today, print time as hh:mm
       {
       sprintf(timeCreateStr, "%02d:%02d", timeCreateLocal.wHour, timeCreateLocal.wMinute);
       }
    else  // not today, but same year, print as MmmDD
       {
       sprintf(timeCreateStr, "%.3s%02d", MONTH_TEXT[timeCreateLocal.wMonth - 1], timeCreateLocal.wDay);
       }

    }
 else  // not this year, print as YYYY
    {
    sprintf(timeCreateStr, "%5d", timeCreateLocal.wYear);
    }
}

/*****************************************************************************/

/* adds user and kernel time, puts it in the int argument,
 *  formats this sum as hh:mm and puts it into the string argument */
static void FormatCpuTime(FILETIME *timeKernel, FILETIME *timeUser, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr)
{
 FILETIME timeCpuTotal;
 SYSTEMTIME timeCpuFormat;
 ULARGE_INTEGER tmp1, tmp2;

 tmp1.HighPart = timeKernel->dwHighDateTime;
 tmp1.LowPart = timeKernel->dwLowDateTime;

 tmp2.HighPart = timeUser->dwHighDateTime;
 tmp2.LowPart = timeUser->dwLowDateTime;

 tmp1.QuadPart += tmp2.QuadPart;

 timeCpuInt->QuadPart = tmp1.QuadPart;  // store process time for later use

 timeCpuTotal.dwHighDateTime = tmp1.HighPart;
 timeCpuTotal.dwLowDateTime = tmp1.LowPart;

 FileTimeToSystemTime(&timeCpuTotal, &timeCpuFormat);

 sprintf(timeCpuStr, "%02d:%02d", timeCpuFormat.wHour, timeCpuFormat.wMinute);
}

/*****************************************************************************/

static void GetMemoryInfo(HANDLE procHandle, char *memSzStr, DWORDLONG totalPhysMemB)
{
 PROCESS_MEMORY_COUNTERS_EX memInfo;

 // get physical memory info
 if(!GetProcessMemoryInfo(procHandle, (PROCESS_MEMORY_COUNTERS *)&memInfo, sizeof(memInfo)))
    {
    *memSzStr = '\0';
    CfOut(cf_error,"GetProcessMemoryInfo","!! Could not obtain process memory information");
    return;
    }

 // virtual memory info: the process' "private bytes" (Commit Size in Win2008 Task Manager, VM Size in XP)

 sprintf(memSzStr, "%4.1f %8lu %8lu", (memInfo.WorkingSetSize * 100.0) / totalPhysMemB, memInfo.PrivateUsage / 1024, memInfo.WorkingSetSize / 1024);
}

/*****************************************************************************/

static void GetProcessCpuTime(DWORD pid, ULARGE_INTEGER *timeCpuInt)
{
 HANDLE procHandle;
 FILETIME timeCreate, timeExit, timeKernel, timeUser;
 ULARGE_INTEGER tmp1, tmp2;

 // skip "idle process" and nt kernel process
 if(pid == 0 || pid == 4)
    {
    Debug("Skipped cpu time query from special process (pid=%d)", pid);
    timeCpuInt->QuadPart = 0;
    return;
    }

 procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

 if(procHandle == NULL)
    {
    CfOut(cf_error,"OpenProcess","!! Could not open process handle for pid %lu", pid);
    timeCpuInt->QuadPart = 0;
    return;
    }

 if(!GetProcessTimes(procHandle, &timeCreate, &timeExit, &timeKernel, &timeUser))
    {
    CfOut(cf_error,"GetProcessTimes","!! Could not get process times");
    timeCpuInt->QuadPart = 0;
    return;
    }

 tmp1.HighPart = timeKernel.dwHighDateTime;
 tmp1.LowPart = timeKernel.dwLowDateTime;

 tmp2.HighPart = timeUser.dwHighDateTime;
 tmp2.LowPart = timeUser.dwLowDateTime;

 tmp1.QuadPart += tmp2.QuadPart;

 timeCpuInt->QuadPart = tmp1.QuadPart;
}

/*****************************************************************************/

void GetProcessUserName(HANDLE procHandle, int incDomain, char *nameStr, int nameStrSz)
{
 char userName[CF_BUFSIZE], domainName[CF_BUFSIZE], tokenBuf[CF_BUFSIZE];
 HANDLE token;
 SID_NAME_USE accType;
 DWORD minLen;
 DWORD unLen = sizeof(userName);
 DWORD dnLen = sizeof(domainName);
 TOKEN_USER *userToken = (TOKEN_USER *)tokenBuf;

 memset(nameStr, 0, nameStrSz);
 strcpy(nameStr, "???");  // in case of error

 if(!OpenProcessToken(procHandle, TOKEN_QUERY, &token))
    {
    // NOTE: we often get "access denied" when we're not running as a service
    Debug("Could not get process user information (often happens when we're not a service) - OpenProcessToken() failed with code %lu\n", GetLastError());
    return;
    }

 // get token SID
 if(!GetTokenInformation(token, TokenUser, userToken, sizeof(tokenBuf), &minLen))
    {
    CfOut(cf_error, "GetTokenInformation", "!! Could not get process token information");
    CloseHandle(token);
    return;
    }

 // get SID account and domain name
 if(!LookupAccountSid(0, userToken->User.Sid, userName, &unLen, domainName, &dnLen, &accType))
    {
    CfOut(cf_error, "LookupAccountSid", "!! Could not look up account name");
    CloseHandle(token);
    return;
    }

 // copy names to argument buffer

 if(incDomain && dnLen)
    {
    snprintf(nameStr, nameStrSz, "%s\\\\%s", domainName, userName);
    }
 else
    {
    snprintf(nameStr, nameStrSz, "%s", userName);
    }

 CloseHandle(token);
}



#endif  /* MINGW */
