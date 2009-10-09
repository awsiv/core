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

/* Implementation of the equivalent of "ps auxw" for windows */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

/* defines */
#define MAXCMDSTR 512  // too big: we read across boundries, giving failure
                       // too small: we truncate the command line string
#define TIMESTAMP_WAIT 1500  // msecs between checking two process timestamps (for % CPU usage)


/* structs TODO: Get from headers ? */
typedef struct _PEB_LDR_DATA
{
   BYTE Reserved1[8];
   PVOID Reserved2[3];
   LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {  // Exists in Winternl.h, which is nonexistant on MinGW. Win64: use different structure.
  BYTE                          Reserved1[2];
  BYTE                          BeingDebugged;
  BYTE                          Reserved2[1];
  PVOID                         Reserved3[2];
  PPEB_LDR_DATA                 Ldr;
  PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
  BYTE                          Reserved4[104];
  PVOID                         Reserved5[52];
  PVOID                         PostProcessInitRoutine;
  BYTE                          Reserved6[128];
  PVOID                         Reserved7[1];
  ULONG                         SessionId;
}PEB, *PPEB;

// define pointer to NtQueryInformationProcess function
typedef NTSTATUS (WINAPI *NTQIP)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);


/* static prototypes */
static char *GetProcessInfo(DWORD pid, ULARGE_INTEGER lastTimeStamp, DWORDLONG totalPhysMemB);
static int EnableDebugPriv(void);
static BOOL GetProcessCmdLine(HANDLE procHandle, char *cmdLineStr, int cmdLineSz);
static void GetProcessTime(HANDLE procHandle, char *timeCreateStr, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr);
static void FormatCpuTime(FILETIME *timeKernel, FILETIME *timeUser, ULARGE_INTEGER *timeCpuInt, char *timeCpuStr);
static void FormatCreationTime(FILETIME *timeCreate, char *timeCreateStr);
static void GetMemoryInfo(HANDLE procHandle, char *memSzStr, DWORDLONG totalPhysMemB);
static void GetProcessCpuTime(DWORD pid, ULARGE_INTEGER *timeCpuInt);
static char *GetProcessUserNameOld(HANDLE procHandle, BOOL incDomain);
static void GetProcessUserName(HANDLE hProcess, int incDomain, char *nameStr, int nameStrSz);
static BOOL SidToUser(PSID pSid, LPTSTR szUser, DWORD dwUserLen, LPTSTR szDomain, DWORD dwDomainLen);


int NovaWin_LoadProcessTable(struct Item **procdata,char *psopts)
{
  DWORD pids[CF_BUFSIZE];
  ULARGE_INTEGER cpuTimeStamps[CF_BUFSIZE];
  MEMORYSTATUSEX memInfo;
  DWORD pidLength, pidCount;
  struct Item *systemprocs = NULL;
  struct Item *otherprocs = NULL;
  char buf[CF_BUFSIZE];
  char *psLine;
  int i;
  
  // need debug priviliges to open some special processes, like crss.exe, alg.exe, svchost.exe
  if(!EnableDebugPriv())
    {
      CfOut(cf_verbose,"EnableDebugPriv","Current account need 'Debug programs' privilige to get info on all processes");
    }


  if (!EnumProcesses(pids, sizeof(pids), &pidLength))
    {
      CfOut(cf_error,"EnumProcesses","!! Could enumerate processes");
      return false;
    }

  if(pidLength == sizeof(pids))
    {
      CfOut(cf_verbose,"","Array may be too small to fit all process IDs");
    }

  // get total memory (needed for process % mem usage)
  memInfo.dwLength = sizeof(MEMORYSTATUSEX);
  if(!GlobalMemoryStatusEx(&memInfo))
    {
      CfOut(cf_error,"GlobalMemoryStatusEx","!! Could not obtain memory information");
      memInfo.ullTotalPhys = 1;
    }

  pidCount = pidLength / sizeof(DWORD);
  
  Debug("Total number of processes found is %lu", pidCount);

  // need two timestamps to get CPU % usage

  for (i = 0; i < pidCount; i++)
    {
      GetProcessCpuTime(pids[i], &cpuTimeStamps[i]);
    }

  Sleep(TIMESTAMP_WAIT);
  
  for (i = 0; i < pidCount; i++)
    {
      psLine = GetProcessInfo(pids[i], cpuTimeStamps[i], memInfo.ullTotalPhys);
      
      if(psLine)
	{
	  AppendItem(procdata,psLine,"");
	}
    }

  // save ps output to files, include header
  sprintf(buf, "%-20s %5s %s %s %8s %8s %-3s %s %5s %s", 
	  "USER", "PID", "%CPU", "%MEM", "VSZ", "RSS", "TTY", "START", "TIME", "COMMAND"); 
  
  PrependItem(procdata, buf, NULL);

  snprintf(buf,CF_MAXVARSIZE,"%s%cstate%ccf_procs",CFWORKDIR, FILE_SEPARATOR, FILE_SEPARATOR);
  RawSaveItemList(*procdata,buf);

  // FIXME: Is "SYSTEM" a good replacement for "root" below ?

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


static char *GetProcessInfo(DWORD pid, ULARGE_INTEGER lastTimeStamp, DWORDLONG totalPhysMemB)
{
  static char psLine[CF_BUFSIZE];
  HANDLE procHandle;
  char cmdLineStr[MAXCMDSTR];
  char timeCreateStr[16], timeCpuStr[16], memSzStr[32];
  ULARGE_INTEGER newTimeStamp;
  __int64 cpuTimeDiff;
  float cpuUsagePercent;
  char userName[CF_BUFSIZE];

  
  if(pid == 0)
    {
      Debug("Skipped information query from idle process (pid=0)");
      return NULL;
    }
  
  procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | STANDARD_RIGHTS_READ, FALSE, pid);
  
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
      GetProcessCmdLine(procHandle, cmdLineStr, sizeof(cmdLineStr));
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
  
  memset(psLine, 0, sizeof(psLine));
  sprintf(psLine, "%-20.20s %5lu %4.1f %s %s %s %s %s", userName, pid, cpuUsagePercent, memSzStr, "?  ", timeCreateStr, timeCpuStr, cmdLineStr);
  return psLine;
}


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


static BOOL GetProcessCmdLine(HANDLE procHandle, char *cmdLineStr, int cmdLineSz)
{
  PROCESS_BASIC_INFORMATION procInfo = {0};
  RTL_USER_PROCESS_PARAMETERS procParams = {0};
  WCHAR cmdLineWstr[cmdLineSz];
  PEB envBlock = {{0}};
  SIZE_T read;

  // load ntdll.dll and get function pointer
  HMODULE ntdll = LoadLibrary("ntdll.dll");

  if(ntdll == NULL)
    {
      CfOut(cf_error,"LoadLibrary","!! Error loading ntdll library");
      return false;
    }

  NTQIP ntqip = (NTQIP) GetProcAddress(ntdll, "NtQueryInformationProcess");

  if(ntqip == NULL)
    {
      CfOut(cf_error,"GetProcAddress","!! Could not get pointer to NtQueryInformationProcess()");
      return false;
    }
  
  // call function to get PROCESS_BASIC_INFORMATION
  ntqip(procHandle, ProcessBasicInformation, (LPVOID) &procInfo, sizeof(procInfo), NULL);

  FreeLibrary(ntdll);

  // read PEB structure
  if(!ReadProcessMemory(procHandle, (LPVOID) procInfo.PebBaseAddress, (LPVOID) &envBlock, sizeof(PEB), NULL))
    {
      CfOut(cf_error,"ReadProcessMemory","!! Could not read process environment block");
      return false;
    }

  // read PROCESS_PARAMETERS
  if(!ReadProcessMemory(procHandle, (LPVOID) envBlock.ProcessParameters, (LPVOID) &procParams, sizeof(RTL_USER_PROCESS_PARAMETERS), NULL))
    {
      CfOut(cf_error,"ReadProcessMemory","!! Could not read process parameters");
      return false;
    }

  // read process commandline + terminating null (2 bytes in unicode)
  wmemset(cmdLineWstr, '\0', cmdLineSz);
  if(!ReadProcessMemory(procHandle, (LPVOID) procParams.CommandLine.Buffer, (LPVOID) cmdLineWstr, sizeof(cmdLineWstr), &read))
    {
      CfOut(cf_error,"ReadProcessMemory","!! Could not read process command line");
      return false;
    }

  // format the command-line as a multibyte string instead of wide string
  wcstombs(cmdLineStr, cmdLineWstr, cmdLineSz);

  return true;
}


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


static void GetMemoryInfo(HANDLE procHandle, char *memSzStr, DWORDLONG totalPhysMemB)
{
  PROCESS_MEMORY_COUNTERS memInfo;
  VM_COUNTERS vmemInfo;

  // get physical memory info
  if(!GetProcessMemoryInfo(procHandle, &memInfo, sizeof(memInfo)))
    {
      *memSzStr = '\0';
      CfOut(cf_error,"GetProcessMemoryInfo","!! Could not obtain process memory information");
      return;
    }

  // get virtual memory info; the process' address space size

  // load ntdll.dll and get function pointer
  HMODULE ntdll = LoadLibrary("ntdll.dll");

  if(ntdll == NULL)
    {
      CfOut(cf_error,"LoadLibrary","!! Could not load ntdll library");
      return;
    }

  NTQIP ntqip = (NTQIP) GetProcAddress(ntdll, "NtQueryInformationProcess");

  if(ntqip == NULL)
    {
      CfOut(cf_error,"GetProcAddress","!! Could not get pointer to NtQueryInformationProcess()");
      return;
    }

  ntqip(procHandle, ProcessVmCounters, (LPVOID) &vmemInfo, sizeof(vmemInfo), NULL);

  FreeLibrary(ntdll);

  sprintf(memSzStr, "%4.1f %8lu %8lu", (memInfo.WorkingSetSize * 100.0) / totalPhysMemB, vmemInfo.VirtualSize / 1024, memInfo.WorkingSetSize / 1024);
}


static void GetProcessCpuTime(DWORD pid, ULARGE_INTEGER *timeCpuInt)
{
  HANDLE procHandle;
  FILETIME timeCreate, timeExit, timeKernel, timeUser;
  ULARGE_INTEGER tmp1, tmp2;
  
  // skip "idle process"
  if(pid == 0)
    {
      Debug("Skipped cpu time query from idle process (pid=0)");
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


// FIXME: Cannot get username of "svchost -k rpcss" and "svchost.exe -k LocalService":
// uses logon SID (we now print string SID instead)
static void GetProcessUserName(HANDLE hProcess, int incDomain, char *nameStr, int nameStrSz)
{
  SECURITY_DESCRIPTOR *pSecDescr = NULL;
  DWORD dwSecInfoSize;
  BOOL bDefaulted;
  PSID psidUser;
  char userName[CF_BUFSIZE];
  char domainName[CF_BUFSIZE];
  char *stringSID;

  memset(nameStr, 0, sizeof(nameStrSz));
  strcpy(nameStr, "???");  // in case of error

  // Get Security Descriptor size
  GetKernelObjectSecurity(hProcess, OWNER_SECURITY_INFORMATION, pSecDescr, 0, &dwSecInfoSize);

  // Call should have failed due to zero-length buffer.
  if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {

      // Security Descriptor allocation
      pSecDescr = (SECURITY_DESCRIPTOR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BYTE) * dwSecInfoSize);

      if (pSecDescr == NULL)
	{
	  CfOut(cf_error,"HeapAlloc","!! Could not allocate memory for security descriptor");
	  return;
	}

      // Get Security Descriptor (real call)
      if (GetKernelObjectSecurity(hProcess, OWNER_SECURITY_INFORMATION, pSecDescr, dwSecInfoSize, &dwSecInfoSize))
	{
	  // Get descriptor owner SID
	  if (GetSecurityDescriptorOwner(pSecDescr, &psidUser, &bDefaulted))
	    {   
	      // Converts SID to Domain + Username
	      if (SidToUser(psidUser, userName, sizeof(userName), domainName, sizeof(domainName)))
		{
		  if(incDomain)
		    {
		      strcpy(nameStr, domainName);
		      strcat(nameStr, "\\");
		    }
		  else
		    {
		      *nameStr = '\0';
		    }

		  strcat(nameStr, userName);
		  
		  HeapFree (GetProcessHeap(), 0, pSecDescr);
		  return;
		}
	      else
		{
		  Debug("Failed to convert SID to user name, using SID instead");

		  // This is usually Session Logon SID, S-1-5-5-#-# http://blog.delphi-jedi.net/2008/03/10/what-is-a-sid/
		  
		  if(ConvertSidToStringSid(psidUser, &stringSID))
		    {
		      strncpy(nameStr, stringSID, nameStrSz);
		      LocalFree(stringSID);
		    }
		}
	    }
	  else
	    {
	      CfOut(cf_error,"GetSecurityDescriptorOwner","!! Could not get owner security descriptor");
	    }
	}
      else
	{
	  CfOut(cf_error,"GetKernelObjectSecurity","!! Could not get kernel security information for process");
	}
    }
  else
    {
      CfOut(cf_error,"GetKernelObjectSecurity","!! Could not get kernel security information for process");
    }


  if (pSecDescr != NULL)
    HeapFree (GetProcessHeap(), 0, pSecDescr);

  return;
}


static BOOL SidToUser(PSID pSid, LPTSTR szUser, DWORD dwUserLen, LPTSTR szDomain, DWORD dwDomainLen)
{
  SID_NAME_USE snuSIDNameUse;
  DWORD dwULen;
  DWORD dwDLen;

  dwULen = dwUserLen;
  dwDLen = dwDomainLen;

  // Get user and domain name based on SID
  if (LookupAccountSid(NULL, pSid, szUser, &dwULen, szDomain, &dwDLen, &snuSIDNameUse))
    {
      // LocalSystem processes are incorrectly reported as owned by BUILTIN\Administrators
      // We modify that behavior to conform to standard taskmanager
      if (lstrcmpi(szDomain, TEXT("builtin")) == 0 && lstrcmpi(szUser, TEXT("administrators")) == 0)
	{
	  strncpy(szUser, TEXT("SYSTEM"), dwUserLen);
	  strncpy(szDomain, TEXT("NT AUTHORITY"), dwDomainLen);
	}
			
      return true;
    }
  return false;
}

#endif  /* MINGW */
