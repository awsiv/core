/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_proc.c                                                          */
/*                                                                           */
/* Created: Tue Sep 15 13:27:55 2009                                         */
/*                                                                           */
/*****************************************************************************/


#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

// TODO: Implement ? - we only support one signal: SIGKILL (9)
/* Returns true if SIGKILL was one of the signals, false otherwise */
int NovaWin_DoAllSignals(struct Item *siglist,struct Attributes a,struct Promise *pp)
{
  int kill = false;
  int signal;
  struct Item *ip;
  struct Rlist *rp;
  pid_t pid;

if (siglist == NULL)
   {
   return 0;
   }

if (a.signals == NULL)
   {
   CfOut(cf_inform,""," -> No signals to send for %s\n",pp->promiser);
   return 0;
   }

for (ip = siglist; ip != NULL; ip=ip->next)
   {
   pid = ip->counter;
   
   for (rp = a.signals; rp != NULL; rp=rp->next)
      {
      signal = Signal2Int(rp->item);

      if(signal != SIGKILL)
	{
	  CfOut(cf_verbose, "", "The only supported signal on windows is 'kill'");
	  continue;
	}
      
      kill = true;
      
      if (!DONTDO)
         {         
	   if(!NovaWin_GracefulTerminate(pid))
            {
            cfPS(cf_verbose,CF_FAIL,"",pp,a," !! Couldn't terminate process with pid %d\n", pid);
            continue;
            }
         else
            {
            cfPS(cf_inform,CF_CHG,"",pp,a," -> Terminated process with pid %d\n", pid);
            break;
            }
         }
      else
         {
	   CfOut(cf_error,""," -> Need to terminate process with pid %d", pid);
         }
      }
   }

return kill;
}


/* Terminates the process identified by pid.
 * TODO: Try to send quit-message to process before terminating it ? */
int NovaWin_GracefulTerminate(pid_t pid)
{
  int res;
  HANDLE procHandle;

  // get process handle from pid
  procHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

  if(procHandle == NULL)
    {
      CfOut(cf_error,"OpenProcess","!! Could not get process handle");
      return false;
    }

  res = TerminateProcess(procHandle, -1);

  if(!CloseHandle(procHandle))
    {
      CfOut(cf_error,"CloseHandle","!! Could not close process handle");
    }

  return (res != 0);
}


/* returns true if execution of 'cmd' returns zero
 * (waits for completion)  */
int NovaWin_ShellCommandReturnsZero(char *comm, int useshell)
{
  HANDLE procHandle;
  DWORD exitcode;
  
  if(!NovaWin_RunCmd(comm, useshell, false, NULL, NULL, &procHandle))
    {
      CfOut(cf_error,"RunCmd","!! Command \"%s\" failed",comm);
      exit(1);
    }
  
  if(WaitForSingleObject(procHandle, INFINITE) == WAIT_FAILED)
    {
      CfOut(cf_error,"WaitForSingleObject","!! Error waiting for process to finish");
      exit(1);
    }
  
  if(!GetExitCodeProcess(procHandle, &exitcode))
    {
      CfOut(cf_error,"GetExitCodeProcess","!! Error getting exit code");
      exit(1);
    }
  
  CloseHandle(procHandle);
  
  return (exitcode == 0);
}


/* Runs the command comm, in a shell if useshell is true. The new process inherit handles if
 * inheritHandles is true. If si is not NULL, these parameters are used to create the process.
 * The process handle of the new process is put in procHandle, this needs to be closed with
 * CloseHandle(). The first parameter in comm is the executable, and it must be quoted if 
 * it contains spaces (e.g. ""C:\Program Files\Cfengine\bin\cf-promises.exe" -f file.cf").
 * Returns true on success, false otherwise. */
int NovaWin_RunCmd(char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle)
{
  STARTUPINFO emptySi;
  PROCESS_INFORMATION pi;
  char buf[CF_BUFSIZE];
  char *binary;
  char *binaryParams;


  if(useshell)
    {
      if(sizeof("/c \"") + strlen(comm) + 1 >= sizeof(buf))
	{
	  CfOut(cf_error,"","!! Buffer to small to hold command-string");
	  return false;
	}
      
      strcpy(buf, "/c \"");
      strcat(buf, comm);
      strcat(buf, "\"");

      binary = CMD_PATH;
      binaryParams = buf;
    }
  else
    {
      binary = NULL;
      binaryParams = comm;
    }

  
  // use empty startupinfo struct if it is unspecified
  if(si == NULL)
    {
      memset(&emptySi, 0, sizeof(emptySi));
      emptySi.cb = sizeof(emptySi);
      si = &emptySi;
    }

  memset(&pi, 0, sizeof(pi));
  
  if(!CreateProcess(binary, binaryParams, NULL, NULL, inheritHandles, 0, NULL, startDir, si, &pi))
    {
      CfOut(cf_error,"CreateProcess","!! Failed to start process");
      return false;
    }

  CloseHandle(pi.hThread);
  *procHandle = pi.hProcess;

  return true;
}

/*****************************************************************************/

/* Gets the owner of the current process and places it into sid, 
 * which has size sidSz. */
int NovaWin_GetCurrentProcessOwner(SID *sid, int sidSz)
{
  HANDLE currProcToken;
  TOKEN_USER *userToken = NULL;
  DWORD reqBufSz;
  int ownerSidSz;

  if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &currProcToken))
    {
      CfOut(cf_error,"OpenProcessToken","!! Could not get access token of current process");
      return false;
    }

  // get required buffer size first
  if(!GetTokenInformation(currProcToken, TokenUser, userToken, 0, &reqBufSz))
    {
      if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
	  CfOut(cf_error,"GetTokenInformation","!! Could not get owner information on current process");
	  CloseHandle(currProcToken);
	  return false;
	}
    }
  else  // the call should fail according to doc
    {
      CfOut(cf_error,"","!! Could not get required buffer size");
      CloseHandle(currProcToken);
      return false;
    }


  // allocate enough buffer bytes and do real call
  if(!(userToken = calloc(1, reqBufSz)))
    {
      CloseHandle(currProcToken);	  
      FatalError("Memory allocation in NovaWin_GetCurrentProcessOwner()");
      return false;
    }

  if(!GetTokenInformation(currProcToken, TokenUser, userToken, reqBufSz, &reqBufSz))
    {
      CfOut(cf_error,"GetTokenInformation","!! Could not get owner information on current process");
      free(userToken);
      CloseHandle(currProcToken);
      return false;
    }

  ownerSidSz = GetLengthSid(userToken->User.Sid);
  
  if(ownerSidSz > sidSz)
    {
      CfOut(cf_error,"","!! Sid buffer too small");
      free(userToken);
      CloseHandle(currProcToken);
      return false;
    }
  
  memcpy(sid, userToken->User.Sid, ownerSidSz);

  free(userToken);
  CloseHandle(currProcToken);

  return true;
}

/*****************************************************************************/

/* Get user name of owner of this process */
int NovaWin_GetCurrentUserName(char *userName, int userNameLen)
{
  DWORD userNameMax = (DWORD)userNameLen;
  
  if(!GetUserName(userName, &userNameMax))
    {
      CfOut(cf_error,"GetUserName","!! Could not get user name of current process, using \"UNKNOWN\"");

      strncpy(userName, "UNKNOWN", userNameLen);
      userName[userNameLen - 1] = '\0';
      return false;
    }
  
  return true;
}

/*****************************************************************************/

int NovaWin_SetTokenPrivilege(HANDLE token, char *privilegeName, int enablePriv) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue(NULL, privilegeName,	&luid ))
	{
		CfOut(cf_error,"LookupPrivilegeValue","!! Could not get priviligege");
		return false; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (enablePriv)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;


	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{ 
		  CfOut(cf_error,"AdjustTokenPrivileges","!! Could not set privilege");
		  return false; 
	} 

	if(GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		  CfOut(cf_error,"AdjustTokenPrivileges","!! The token does not have the desired privilege");
		  return false;
	} 

	return true;
}


#endif /* MINGW */
