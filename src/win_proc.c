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

// TODO: Implement ?
int NovaWin_DoAllSignals(struct Item *siglist,struct Attributes a,struct Promise *pp)
{
CfOut(cf_verbose,"","Signals are not yet supported on NT.\n");
return 0;
}


/* Terminates the process identified by pid.
 * TODO: Try to send quit-message to process before terminating it ? */
int NovaWin_GracefulTerminate(pid_t pid)
{
  nw_exper("GracefulTerminate", "needs testing");

  int res;
  HANDLE procHandle;

  // get process handle from pid
  procHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

  if(procHandle == NULL)
    {
      CfOut(cf_error,"OpenProcess","Could not get process handle");
      return false;
    }

  res = TerminateProcess(procHandle, -1);

  if(!CloseHandle(procHandle))
    {
      CfOut(cf_error,"CloseHandle","Could not close process handle");
    }

  return (res != 0);
}


/* returns true if execution of 'cmd' returns zero
 * (waits for completion)  */
int NovaWin_ShellCommandReturnsZero(char *comm, int useshell)
{
  HANDLE procHandle;
  DWORD exitcode;
  
  mgw_exper("ShellCommandReturnsZero", "needs testing");
  
  if(!NovaWin_RunCmd(comm, useshell, false, NULL, NULL, &procHandle))
    {
      CfOut(cf_error,"RunCmd","Command %s failed",comm);
      exit(1);
    }
  
  if(WaitForSingleObject(procHandle, INFINITE) == WAIT_FAILED)
    {
      CfOut(cf_error,"WaitForSingleObject","Error waiting for process to finish");
      exit(1);
    }
  
  if(!GetExitCodeProcess(procHandle, &exitcode))
    {
      CfOut(cf_error,"GetExitCodeProcess","Error getting exit code");
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
	  CfOut(cf_error,"","Buffer to small to hold command-string");
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
      CfOut(cf_error,"CreateProcess","Failed to start process");
      return false;
    }

  CloseHandle(pi.hThread);
  *procHandle = pi.hProcess;

  return true;
}

#endif /* MINGW */
