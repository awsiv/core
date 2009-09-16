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

#endif /* MINGW */
