/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_pipe.c                                                          */
/*                                                                           */
/* Created: Fri Sep 24 13:48:44 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

struct CfWinPipe{  // links pipe descriptors to process handles
  FILE *pipe;
  HANDLE procHandle;
};

#define MAX_PIPES 20
struct CfWinPipe PIPES[MAX_PIPES];


/* static prototypes */
static FILE *OpenProcessPipe(char *comm, int useshell, char *startDir, char *type, int background);
static int InitializePipes(HANDLE *childInWrite, HANDLE *childInRead, HANDLE *childOutWrite, HANDLE *childOutRead);
static int SaveDescriptorPair(FILE *pipe, HANDLE procHandle);
static int PopDescriptorPair(FILE *pipe, HANDLE *procHandle);


FILE *NovaWin_cf_popen(char *command,char *type)
{
  return OpenProcessPipe(command, false, NULL, type, false);
}


FILE *NovaWin_cf_popen_sh(char *command,char *type)
{
  return OpenProcessPipe(command, true, NULL, type, false);
}


FILE *NovaWin_cf_popensetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv, int background)
{
  // NT unsupported: uid, gid, chrootv
  if(uid != CF_UNDEFINED)
    {
      CfOut(cf_verbose, "", "NovaWin_cf_popensetuid: uid is ignored on NT");
    }
    
  if(gid != CF_UNDEFINED)
    {
      CfOut(cf_verbose, "", "NovaWin_cf_popensetuid: gid is ignored on NT");
    }
  
  if(chrootv != NULL)
    {
      CfOut(cf_verbose, "", "NovaWin_cf_popensetuid: chrootv is ignored on NT");
    }

  return OpenProcessPipe(command, false, chdirv, type, background); 
}


FILE *NovaWin_cf_popen_shsetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv, int background)
{
  // we can only run in the current user environment (assumed to be root)
  if((uid == -1 || uid == 0) && (gid == -1 || gid == 0) && chrootv == NULL)
    {
      return OpenProcessPipe(command, true, chdirv, type, background);
    }
  else
    {
      CfOut(cf_error,"","!! Starting process with uid=%d, gid=%d, chrootv=%s is unsupported on NT.", uid, gid, chrootv);
      return NULL;
    }
}


int NovaWin_cf_pclose(FILE *pp)
{
  HANDLE procHandle;
  
  if(!PopDescriptorPair(pp, &procHandle))
    {
      CfOut(cf_error,"","!! Could not find process corresponding to pipe");
      return -1;
    }

  if(!CloseHandle(procHandle))
    {
      CfOut(cf_error,"CloseHandle","!! Could not close process handle");
    }

  return _close((long)pp);
}


int NovaWin_cf_pclose_def(FILE *pfp, struct Attributes a, struct Promise *pp)
{
  HANDLE procHandle;
  DWORD exitCode;
  
  if(!PopDescriptorPair(pfp, &procHandle))
    {
      CfOut(cf_error,"","!! Could not find process corresponding to pipe");
      return -1;
    }

  if(!GetExitCodeProcess(procHandle, &exitCode))
    {
      CfOut(cf_error,"GetExitCodeProcess","!! Error getting exit code");
      cfPS(cf_error,CF_FAIL,"",pp,a," !! Could not get exit code of process\n",pp->promiser);
      return -1;
    }

  if(!CloseHandle(procHandle))
    {
      CfOut(cf_error, "CloseHandle", "!! Could not close process handle");
    }

  _close((long)pfp);

  if(exitCode == 0)
    {
      cfPS(cf_verbose,CF_CHG,"",pp,a," -> Finished script - succeeded %s\n",pp->promiser);
    }
  else
    {
      cfPS(cf_inform,CF_INTERPT,"",pp,a," !! Finished script %s -- an error occurred (returned %d)\n",pp->promiser, (int)exitCode);
    }

  return (int)exitCode;
}


/* Starts a new process in startDir (NULL means current), inside a
 * shell environment if specified.  If type is "r", we wait for the
 * process to finish, and return a pipe that has the STDOUT and STDERR
 * of the process. If type is "w", we do not wait for the process to
 * finish, but return a pipe to the process' STDIN.
**/
static FILE *OpenProcessPipe(char *comm, int useshell, char *startDir, char *type, int background)
{
  HANDLE childOutWrite; // child's stdout & stderr writes to this
  HANDLE childOutRead;  // .. and appears here for parent to read
  HANDLE childInWrite;  // parent writes to this  
  HANDLE childInRead;   // .. and it appears here - child's stdin
  HANDLE childProcess;  // process handle of child
  HANDLE retPipe = NULL;
  STARTUPINFO si;
  FILE *streamPipe;
  
  if((*type != 'r' && *type != 'w') || (type[1] != '\0'))
    {
      CfOut(cf_error,"","!! Pipe type is not 'r' or 'w' - shouldn't happen");
      return NULL;
    }

  if(!InitializePipes(&childInWrite, &childInRead, &childOutWrite, &childOutRead))
    {
      CfOut(cf_error,"","!! Could not create pipes");
      return NULL;
    }
   
  // redirect stdin, stdout and stderr into our pipes
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  si.hStdError = childOutWrite;
  si.hStdOutput = childOutWrite;
  si.hStdInput = childInRead;
  si.dwFlags |= STARTF_USESTDHANDLES;

  if(!NovaWin_RunCmd(comm, useshell, true, startDir, &si, &childProcess))
    {
      CfOut(cf_error,"","!! Error running command");
      return NULL;
    }

  // wait for process to exit if we are reading from the pipe
  if(*type == 'r')
    {
      if(background)
	{
	  CloseHandle(childInRead);
	  CloseHandle(childOutWrite);
	  CloseHandle(childInWrite);
	  CloseHandle(childOutRead);
	}
      else
	{
	  if(WaitForSingleObject(childProcess, INFINITE) == WAIT_FAILED)
	    {
	      CfOut(cf_error,"WaitForSingleObject","!! Error waiting for process to finish");
	      return NULL;
	    }
	  
	  CloseHandle(childInRead);
	  CloseHandle(childOutWrite);
	  CloseHandle(childInWrite);
	  retPipe = childOutRead;
	}
    }
  else  // *type == 'w'
    {
      CloseHandle(childOutRead);
      CloseHandle(childInRead);
      CloseHandle(childOutWrite);
      retPipe = childInWrite;
    }

  if(background)
    {
      streamPipe = CreateEmptyStream();
    }
  else
    {
      streamPipe = NovaWin_FileHandleToStream(retPipe, type);
   
      if(streamPipe == NULL)
	{
	  CfOut(cf_error,"","!! Error converting file handle");
	  return NULL;
	}
    }

  if(!SaveDescriptorPair(streamPipe, childProcess))
    {
      CfOut(cf_error,"","!! Error saving descriptor pair");
      return NULL;
    }

  return streamPipe;
}


/* Sets up two pipes; one for input to the child process,
 * and one for output from it (stdout + stderr). 
 * Returns true on success, false otherwise*/
static int InitializePipes(HANDLE *childInWrite, HANDLE *childInRead, HANDLE *childOutWrite, HANDLE *childOutRead)
{
  SECURITY_ATTRIBUTES saAttr;
  //  DWORD pipeMode = PIPE_NOWAIT;  TODO: Remove?

  // make pipe handles inheritable
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
  saAttr.bInheritHandle = TRUE; 
  saAttr.lpSecurityDescriptor = NULL; 

  // pipe for child's stdout and stderr
  if (!CreatePipe(childOutRead, childOutWrite, &saAttr, 0))
    {
      CfOut(cf_error,"CreatePipe","!! Could not create pipe");
      return false;
    }

  // read handle to child's stdout should not be inherited
  if (!SetHandleInformation(*childOutRead, HANDLE_FLAG_INHERIT, 0))
    {
      CfOut(cf_error,"SetHandleInformation","!! Could not set handle information");
      return false;
    }

  // pipe for child's stdin
  if (!CreatePipe(childInRead, childInWrite, &saAttr, 0))
    {
      CfOut(cf_error,"CreatePipe","!! Could not create pipe");
      return false;
    }

  if (!SetHandleInformation(*childInWrite, HANDLE_FLAG_INHERIT, 0))
    {
      CfOut(cf_error,"SetHandleInformation","!! Could not set handle information");
      return false;
    }
   
  return true;
}


/* Saves the link between a pipe and process handle.
 * Returns true on succes, false otherwise. */
static int SaveDescriptorPair(FILE *pipe, HANDLE procHandle)
{
  int i;

  if (!ThreadLock(cft_count))
    {
      return false;
    }

  // find first free slot
  i = 0;
  while(PIPES[i].pipe != NULL)
    {
      i++;
      if(i == MAX_PIPES)
	{
	  ThreadUnlock(cft_count);
	  CfOut(cf_error,"","!! Too many open pipes");
	  return false;
	}
    }

  PIPES[i].pipe = pipe;
  PIPES[i].procHandle = procHandle;

  ThreadUnlock(cft_count);
  return true;
}


/* Looks through saved descriptor pairs for a pipe containing the first argument,
 * and writes the corresponding process handle to the second argument.
 * The space used for the pair is then freed.
 * Returns true on success, false otherwise. */
static int PopDescriptorPair(FILE *pipe, HANDLE *procHandle)
{
  int i;

  if (!ThreadLock(cft_count))
    {
      return false;
    }

  if(PIPES == NULL)
    {
      ThreadUnlock(cft_count);
      CfOut(cf_error,"","!! No pipe descriptors are saved");
      return false;
    }

  i = 0;
  while(PIPES[i].pipe != pipe)
    {
      i++;
      if(i == MAX_PIPES)
	{
	  ThreadUnlock(cft_count);
	  CfOut(cf_error,"","!! Pipe descriptor was not found");
	  return false;
	}
    }

  *procHandle = PIPES[i].procHandle;
  
  // free slot
  PIPES[i].pipe = NULL;
  PIPES[i].procHandle = NULL;

  ThreadUnlock(cft_count);
  return true;
}


#endif  /* MINGW */
