/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_mon.c                                                           */
/*                                                                           */
/* Created: Mon Sep 28 13:36:09 2009                                         */
/*                                                                           */
/*****************************************************************************/

/* Functions related to windows performance monitoring (cf-monitord
   for windows). */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

// last measured CPU ticks
ULARGE_INTEGER ticksWorkLast = {0};
ULARGE_INTEGER ticksTotalLast = {0};


void NovaWin_GatherCPUData(double *CF_THIS)
{
  ULARGE_INTEGER ticksWork;
  ULARGE_INTEGER ticksTotal;
  double util;

  if(!NovaWin_GetCpuTicks(&ticksWork, &ticksTotal))
    {
      CfOut(cf_verbose, "", "Skipping this CPU measure due to lack of data");
      return;
    }

  // need two samples to find utilisation
  if(ticksTotalLast.QuadPart != 0)
    {
      // wrap around
      if(ticksTotalLast.QuadPart > ticksTotal.QuadPart)
	{
	  util = 50.0;
	}
      else
	{
	  util = (double)(((ticksWork.QuadPart - ticksWorkLast.QuadPart)*100)/(ticksTotal.QuadPart - ticksTotalLast.QuadPart));
	}

      CF_THIS[ob_cpuall] = util;
    }

  ticksWorkLast.QuadPart = ticksWork.QuadPart;
  ticksTotalLast.QuadPart = ticksTotal.QuadPart;
}

/*********************************************************************/

int NovaWin_GetCpuTicks(ULARGE_INTEGER *ticksWork, ULARGE_INTEGER *ticksTotal)
/*
 * Gets the number of CPU ticks used in non-idle threads and the
 * total. This are total for all the cores and CPUs of the system.
 * Returns true on success, false otherwise.
 */
{
  ULARGE_INTEGER ker, usr, idl;
  FILETIME timeIdle;
  FILETIME timeKernel;
  FILETIME timeUser;

  if(!GetSystemTimes( &timeIdle, &timeKernel, &timeUser))
    {
      CfOut(cf_error, "GetSystemTimes", "!! Could not get CPU ticks");
      return false;
    }

  // kernel + user is total time, idle is unused fraction

  ker.HighPart = timeKernel.dwHighDateTime;
  ker.LowPart = timeKernel.dwLowDateTime;

  usr.HighPart = timeUser.dwHighDateTime;
  usr.LowPart = timeUser.dwLowDateTime;
  
  idl.HighPart = timeKernel.dwHighDateTime;
  idl.LowPart = timeKernel.dwLowDateTime;
  
  ticksTotal->QuadPart = ker.QuadPart + usr.QuadPart;
  ticksWork->QuadPart =  ticksTotal->QuadPart - idl.QuadPart;

  return true;
}


#endif  /* MINGW */

