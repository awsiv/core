/*****************************************************************************/
/*                                                                           */
/* File: services_win.c                                                      */
/*                                                                           */
/* Created: Wed Dec 16 10:27:07 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


void NovaWin_VerifyServices(struct Attributes a,struct Promise *pp)
{
#ifdef MINGW

 struct Rlist *dep;
 char *srvName;
 int onlyCheckDeps;
 int result = false;
 
 srvName = pp->promiser;

 // figure out how to handle dependencies
 
 if(strcmp(a.service.service_depend_chain, "all_related") == 0)
    {
    onlyCheckDeps = false;
    }
 else if((strcmp(a.service.service_depend_chain, "stop_child_services") == 0) &&
         ((a.service.service_policy == cfsrv_stop) || (a.service.service_policy == cfsrv_disable)))
    {
    onlyCheckDeps = false;
    }
 else if((strcmp(a.service.service_depend_chain, "start_parent_services") == 0) &&
         (a.service.service_policy == cfsrv_start))
    {
    onlyCheckDeps = false;
    }
 else
    {
    onlyCheckDeps = true;
    }

 
 // dependencies must be running while service is running - i.e. started first, but are never stopped implicitly

 if(a.service.service_policy == cfsrv_start)
    {
    for(dep = a.service.service_depend; dep != NULL; dep = dep->next)
       {
       if(!NovaWin_CheckServiceStatus((char *)dep->item, a.service.service_policy, NULL, onlyCheckDeps, true, a, pp, false))
          {
          cfPS(cf_error,CF_FAIL,"",pp,a," !! Failed checking status of dependency \"%s\" of service \"%s\"", (char *)dep->item, srvName);
          PromiseRef(cf_error,pp);
          return;
          }
       }
    }
        
 if(!NovaWin_CheckServiceStatus(srvName, a.service.service_policy, a.service.service_args, onlyCheckDeps, false, a, pp, true))
    {
    cfPS(cf_error,CF_FAIL,"",pp,a," !! Failed checking status of service \"%s\"", srvName);
    PromiseRef(cf_error,pp);
    return;
    }

 
#else  /* NOT MINGW */
 cfPS(cf_error,CF_FAIL,"",pp,a,"!! Windows services are only supported on Windows");
 PromiseRef(cf_error,pp);
#endif
}

/*****************************************************************************/

#ifdef MINGW

#define STATUSWAIT_MIN 1000   // msecs waiting for a service status change (e.g. pending start -> start)
#define STATUSWAIT_MAX 10000
#define STATUSWAIT_MAXSLEEP 10  // maximum number of times to call Sleep() during waiting


int NovaWin_CheckServiceStatus(char *srvName, enum cf_srv_policy policy, char *argStr,int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs)
/* cfPS() is never called on failure (i.e. when functions return false), and parameter setCfPs indicates wether cfPS is called on success.  */
{
 SC_HANDLE managerHandle;
 SC_HANDLE srvHandle;
 int result = false;
 char **argv = NULL;
 int argc = 0;
 DWORD startTime = SERVICE_DEMAND_START;
 int startTimeRes = 0;

 Debug("NovaWin_CheckServiceStatus(%s, %d, %d, %d)\n", srvName, policy, onlyCheckDeps, isDependency);
 
 managerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
 if(managerHandle == NULL)
    {
    CfOut(cf_error,"OpenSCManager","!! Could not open service manager");
    return false;
    }
 
 srvHandle = OpenService(managerHandle, srvName, SERVICE_ALL_ACCESS);
 
 if(srvHandle == NULL)
    {
    CfOut(cf_error,"OpenService","!! Could not open service \"%s\"", srvName);
    CloseServiceHandle(managerHandle);
    return false;
    }
 
 switch(policy)
    {
    case cfsrv_start:

        // split argument string, if used
        if(argStr)
           {
           NovaWin_AllocSplitServiceArgs(argStr, &argc, &argv);
           }

        result = NovaWin_CheckServiceStart(managerHandle, srvHandle, argc, argv, onlyCheckDeps, isDependency, a, pp, setCfPs);

        if(!result)
           {
           CfOut(cf_error,"","!! Could not start service \"%s\"", srvName);
           }
        
        break;

    case cfsrv_stop:

        result = NovaWin_CheckServiceStop(managerHandle, srvHandle, onlyCheckDeps, isDependency, true, a, pp, setCfPs);

        if(!result)
           {
           CfOut(cf_error,"","!! Could not stop service \"%s\"", srvName);
           }
        
        break;

    case cfsrv_disable:

        result = NovaWin_CheckServiceDisable(managerHandle, srvHandle, onlyCheckDeps, isDependency, a, pp, setCfPs);
        
        if(!result)
           {
           CfOut(cf_error,"","!! Could not disable service \"%s\"", srvName);
           }
        
        break;

    default:

        FatalError("Unknown service type in NovaWin_CheckServiceStatus())\n");
    }


 // check dispatch time

 if(result && (policy == cfsrv_start) && !isDependency)
    {
    if(a.service.service_autostart_policy)
       {
       if(strcmp(a.service.service_autostart_policy, "none") == 0)
          {
          startTime = SERVICE_DEMAND_START;
          }
       else if(strcmp(a.service.service_autostart_policy, "boot_time") == 0)
          {
          startTime = SERVICE_AUTO_START;
          }
       else
          {
          FatalError("Unknown service_start_policy in NovaWin_CheckServiceStatus()");
          }


       if(!NovaWin_SetServiceStartTime(srvHandle, startTime, true, startTime, &startTimeRes))
          {
          CfOut(cf_error,"","!! Could not check autostart status of service");
          result = false;;
          }

       if(result)
          {
          switch(startTimeRes)
             {
             case 0:  // already correct

                 cfPS(cf_inform,CF_NOP,"",pp,a,"-> Autostart policy of service is already correct");
                 break;
              
             case 1:  // not correct
              
                 if(a.transaction.action == cfa_warn)
                    {
                    cfPS(cf_error,CF_WARN,"",pp,a," !! The service autostart policy needs change");
                    break;
                    }
              
                 if(!DONTDO)
                    {
                    if(!NovaWin_SetServiceStartTime(srvHandle, startTime, false, 0, &startTimeRes))
                       {
                       CfOut(cf_error,"","!! Could not change autostart of service to \"%s\"", a.service.service_autostart_policy);
                       result = false;
                       break;
                       }
                    }
              
                 cfPS(cf_inform,CF_CHG,"",pp,a,"-> Successfully updated autostart policy of service");
                 break;
              
             default:
              
                 FatalError("Wrong autostart result in NovaWin_CheckServiceStatus()");
             }
          }
       }
       
    }

 CloseServiceHandle(srvHandle); 
 CloseServiceHandle(managerHandle);

 if(argv)
    {
    free(argv);
    }

 return result;
}

/*****************************************************************************/

int NovaWin_CheckServiceStart(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int argc, char **argv, int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs)
/* Checks that a given service is in started state, or starts it if not. If onlyCheckDeps
 * is true, we don't start the service if there are other not running services depending on it.
 * If onlyCheckDeps is false, all dependent services are started too.
 * Returns true on success, false otherwise. */
{
 SERVICE_STATUS_PROCESS srvStatus;
 DWORD bytesNeeded;
 int allDepsRunning;

 // check current run state
 if(!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (BYTE *)&srvStatus, sizeof(srvStatus), &bytesNeeded))
    {
    CfOut(cf_error,"QueryServiceStatusEx","!! Could not get run state of service");
    return false;
    }

 Debug("Service has currently state %lu, we want it to run (state %d)\n", srvStatus.dwCurrentState, SERVICE_RUNNING);

 switch(srvStatus.dwCurrentState)
    {
    case SERVICE_RUNNING:  // service already running, we don't start it again
        if(setCfPs)
           {
           cfPS(cf_inform,CF_NOP,"",pp,a,"-> Service is already running");
           }
        return true;
        break;

    case SERVICE_CONTINUE_PENDING:
    case SERVICE_START_PENDING:
        
        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_RUNNING))
           {
           CfOut(cf_error,"","!! Service did not start running while waiting (timeout reached)");
           return false;
           }
        else
           {
           Debug("Service started running after waiting from pending continue or start state (state=%lu)\n", srvStatus.dwCurrentState);
           if(setCfPs)
              {
              cfPS(cf_inform,CF_NOP,"",pp,a,"-> Service is already running");
              }
           return true;
           }

        break;

    case SERVICE_PAUSED:  // start the service below
    case SERVICE_STOPPED:
        break;        

    case SERVICE_PAUSE_PENDING:
        
        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_PAUSED))
           {
           CfOut(cf_error,"","!! Service did not pause while waiting (timeout reached)");
           return false;
           }
        break;

    case SERVICE_STOP_PENDING:

        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_STOPPED))
           {
           CfOut(cf_error,"","!! Service did not stop while waiting (timeout reached)");
           return false;
           }
        break;

    default:

        FatalError("Unknown service status in NovaWin_CheckServiceStart()\n");
    }

 if(onlyCheckDeps)
    {
    if(isDependency)
       {
       CfOut(cf_error,"","!! This dependency is not started, and service_method.service_dependence_chain says not to start it");
       return false;
       }
    
    if(!NovaWin_ServiceDepsRunning(managerHandle, srvHandle, &allDepsRunning))
       {
       CfOut(cf_error,"","!! Could not check if service has only running dependencies");
       return false;
       }
    
    if(!allDepsRunning)
       {
       CfOut(cf_error,"","!! Not all dependencies are running, and service_method.service_dependence_chain says not to start them");
       return false;
       }
    }

 if(setCfPs && a.transaction.action == cfa_warn)
    {
    cfPS(cf_error,CF_WARN,"",pp,a," !! The service needs to be started");
    return true;
    }

 if(!DONTDO)
    {
    // enable the service and its dependencies if they're disabled
    if(!NovaWin_SetSrvDepsStartTime(managerHandle, srvHandle, SERVICE_DEMAND_START, true, SERVICE_DISABLED))
       {
       CfOut(cf_error,"","!! Could not enable service and all its dependencies - needed before starting service");
       return false;
       }
 
    // next, we start the service and check that it is running

    if(!StartService(srvHandle, argc, (LPCSTR *)argv))
       {
       CfOut(cf_error,"StartService","!! Could not start service");
       return false;
       }

    if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_RUNNING))
       {
       CfOut(cf_error,"","!! Service is not in running state after it is started");
       return false;
       }
    }

 if(setCfPs)
    {
    cfPS(cf_inform,CF_CHG,"",pp,a,"-> Successfully started service");
    }
 
 return true;
}

/*****************************************************************************/

int NovaWin_CheckServiceStop(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps, int isDependency, int unDisable, struct Attributes a,struct Promise *pp, int setCfPs)
/* Checks that a given service is in stopped state, or stops it if not. If onlyCheckDeps
 * is true, we don't stop the service if there are other running services depending on it.
 * If onlyCheckDeps is false, all dependent services are stopped too.
 * Returns true on success, false otherwise. */
{
 SERVICE_STATUS_PROCESS srvStatus;
 int disableStatus;
 DWORD bytesNeeded;
 
 // check current run state
 if(!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (BYTE *)&srvStatus, sizeof(srvStatus), &bytesNeeded))
    {
    CfOut(cf_error,"QueryServiceStatusEx","!! Could not get service run state");
    return false;
    }

 Debug("Service has currently state %lu, we want it to stop (state %d)\n", srvStatus.dwCurrentState, SERVICE_STOPPED);

 switch(srvStatus.dwCurrentState)
    {
    case SERVICE_STOP_PENDING:

        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_STOPPED))
           {
           CfOut(cf_error,"","!! Service did not stop while waiting (timeout reached)");
           return false;
           }
        else
           {
           Debug("Service went to stop state while waiting in pending stop\n");

           // fallthrough
           }
    
    case SERVICE_STOPPED:  // already stopped, but might need un-disabling it

        if((isDependency && onlyCheckDeps) || !unDisable)
           {
           return true;
           }
        
        if(!NovaWin_SetServiceStartTime(srvHandle, SERVICE_DEMAND_START, true, SERVICE_DISABLED , &disableStatus))
           {
           CfOut(cf_error,"","!! Could not check enable-status of service in stop (but it was stopped)");
           return false;
           }

        if(setCfPs)
           {
           if(disableStatus == 0 || disableStatus == 1)
              {
              cfPS(cf_inform,CF_NOP,"",pp,a,"-> Service is already stopped");
              }
           else
              {
              cfPS(cf_inform,CF_CHG,"",pp,a,"-> Enabled service, but kept it stopped");
              }
           }
        return true;
        
        break;

    case SERVICE_CONTINUE_PENDING:
    case SERVICE_START_PENDING:
        
        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_RUNNING))
           {
           CfOut(cf_error,"","!! Service did not start running while waiting (timeout reached)");
           return false;
           }
        else
           {
           Debug("Service started running after waiting from pending continue or start state (state=%lu), trying to stop\n", srvStatus.dwCurrentState);
           }
        break;

    case SERVICE_PAUSED:  // stop the service below
    case SERVICE_RUNNING:
        break;        

    case SERVICE_PAUSE_PENDING:
        
        if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_PAUSED))
           {
           CfOut(cf_error,"","!! Service did not pause while waiting (timeout reached)");
           return false;
           }
        break;

    default:

        FatalError("Unknown service status in NovaWin_CheckServiceStop()\n");
    }

 if(isDependency && onlyCheckDeps)
    {
    CfOut(cf_error,"","!! This dependency is not stopped, and service_method.service_dependence_chain says not to stop it");
    return false;
    }

 if(setCfPs && a.transaction.action == cfa_warn)
    {
    cfPS(cf_error,CF_WARN,"",pp,a," !! The service needs to be stopped");
    return true;
    }
 
 // we must stop any dependent services before stopping the service itself

 if(!DONTDO)
    {
    if(!NovaWin_StopDependentServices(managerHandle, srvHandle, onlyCheckDeps))
       {
       CfOut(cf_error,"","!! Could not stop all dependent services (check only=%d)", onlyCheckDeps);
       return false;
       }

    // stop the service
    if (!ControlService(srvHandle, SERVICE_CONTROL_STOP, (SERVICE_STATUS *)&srvStatus))
       {
       CfOut(cf_error,"ControlService","!! Could not stop service");
       return false;
       } 

    if(!NovaWin_ServiceStateWait(srvHandle, SERVICE_STOPPED))
       {
       CfOut(cf_error,"","!! Service is not in stopped state after it got stop signal (timeout reached)");
       return false;
       }
    }

 if(setCfPs)
    {
    cfPS(cf_inform,CF_CHG,"",pp,a,"-> Successfully stopped service");
    }
 
 return true;
}

/*****************************************************************************/

int NovaWin_CheckServiceDisable(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs)
/* Checks that a given service is in stopped state and is disabled, or stops and disables it
 * if not. Stopped means not running, and disabled means it can't be started again directly.
 * If onlyCheckDeps is true, we don't do anything if there are other running services
 * depending on it. If onlyCheckDeps is false, all dependent services are stopped, and this
 * service is stopped and disabled. Returns true on success, false otherwise. */
{
 int disableRes;
 
 // first make sure the service is stopped, then disable it
 if(!NovaWin_CheckServiceStop(managerHandle, srvHandle, onlyCheckDeps, isDependency, false, a, pp, false))
    {
    CfOut(cf_error,"","!! Could not disable service because of failure when stopping it");
    return false;
    }

 if(isDependency)  // don't disable dependencies
    {
    return true;
    }

 // check if the service is already disabled first
 if(!NovaWin_SetServiceStartTime(srvHandle, SERVICE_DISABLED, true, SERVICE_DISABLED, &disableRes))
    {
    CfOut(cf_error,"","!! Could not check enable-status of service (but it was stopped)");
    return false;
    }

 switch(disableRes)
    {
    case 0:  // already disabled
        if(setCfPs)
           {
           cfPS(cf_inform,CF_NOP,"",pp,a,"-> Service is already disabled");
           }
        break;
        
    case 1:  // not disabled, not changed (yet)
        
        if(a.transaction.action == cfa_warn)
           {
           if(setCfPs)
              {
              cfPS(cf_error,CF_WARN,"",pp,a," !! The service needs to be disabled");
              }

           break;
           }

        if(!DONTDO)
           {
           if(!NovaWin_SetServiceStartTime(srvHandle, SERVICE_DISABLED, false, 0, &disableRes))
              {
              CfOut(cf_error,"","!! Could not disable service (but it was successfully stopped)");
              return false;
              }
           }

        if(setCfPs)
           {
           cfPS(cf_inform,CF_CHG,"",pp,a,"-> Successfully disabled service");
           }
        
        break;

    default:
        
        FatalError("Wrong disable-result in NovaWin_CheckServiceDisable()");
    }

 return true;
}

/*****************************************************************************/

int NovaWin_ServiceDepsRunning(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int *allDepsRunning)
/* Checks if all dependencies of the given service is in running or paused state, or
 * pending to any of these. allDepsRunning is set to true if so, false otherwise - if the
 * function succeeds. Returns true on success, false otherwise. */
{
 QUERY_SERVICE_CONFIG *srvConfig;
 SERVICE_STATUS_PROCESS depStatus;
 DWORD bytesNeeded;
 SC_HANDLE depHandle;
 char *depName;

 *allDepsRunning = false;

 srvConfig = NovaWin_AllocServiceConfig(srvHandle);

 if(srvConfig == NULL)
    {
    CfOut(cf_error,"","!! Could not get service configuration");
    return false;
    }

 for(depName = srvConfig->lpDependencies; depName != NULL && *depName != '\0'; depName += (strlen(depName) + 1))
    {
    Debug("Checking if dependecy \"%s\" is running or paused\n", depName);
    
    depHandle = OpenService(managerHandle, depName, SERVICE_ALL_ACCESS);
    
    if(depHandle == NULL)
       {
       CfOut(cf_error,"OpenService","!! Could not open handle to service \"%s\" in order to check if dependency is running", depName);
       free(srvConfig);
       return false;
       }

    // check dependency run state
    if(!QueryServiceStatusEx(depHandle, SC_STATUS_PROCESS_INFO, (BYTE *)&depStatus, sizeof(depStatus), &bytesNeeded))
       {
       CfOut(cf_error,"QueryServiceStatusEx","!! Could not check dependency run state");
       CloseServiceHandle(depHandle);
       free(srvConfig);
       return false;
       }

    switch(depStatus.dwCurrentState)
       {
       case SERVICE_CONTINUE_PENDING:
       case SERVICE_PAUSE_PENDING:
       case SERVICE_PAUSED:
       case SERVICE_RUNNING:
       case SERVICE_START_PENDING:
           // ok - running or paused
           break;

       case SERVICE_STOP_PENDING:
       case SERVICE_STOPPED:
           CfOut(cf_inform,"","Service dependency \"%s\" is not running (state %lu)", depName, depStatus.dwCurrentState);
           CloseServiceHandle(depHandle);
           free(srvConfig);
           return true;
           break;

       default:
           FatalError("Unknown service state in NovaWin_ServiceDepsRunning()");
       }
    
    CloseServiceHandle(depHandle);
    }
  
 
 free(srvConfig);

 *allDepsRunning = true;
 
 return true;
}

/*****************************************************************************/

QUERY_SERVICE_CONFIG *NovaWin_AllocServiceConfig(SC_HANDLE srvHandle)
/* Returns the result of QueryServiceConfig allocated by malloc -
 * should therefore be freed when done using it. Returns NULL on error. */
{
 DWORD bytesNeeded, srvConfigSz;
 QUERY_SERVICE_CONFIG *srvConfig; 
 
 // first call is done to get required buffer size
 
 if(QueryServiceConfig(srvHandle, NULL, 0, &bytesNeeded))
    {
    CfOut(cf_error,"QueryServiceConfig","!! Call was supposed to fail due to insufficient buffer");
    return NULL;
    }
 else
    {
    if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
       {
       srvConfigSz = bytesNeeded;
       
       srvConfig = (QUERY_SERVICE_CONFIG *)malloc(srvConfigSz);

       if(srvConfig == NULL)
          {
          FatalError("Memory allocation in NovaWin_AllocServiceConfig()");
          }
       }
    else
       {
       CfOut(cf_error,"QueryServiceConfig","!! Could not get required buffer size");
       return NULL;
       }
    }
 
 if(!QueryServiceConfig(srvHandle, srvConfig, srvConfigSz, &bytesNeeded))
    {
    CfOut(cf_error,"QueryServiceConfig","!! Could not qurey service configuration");
    free(srvConfig);
    return NULL;
    }

 return srvConfig;
}

/*****************************************************************************/

int NovaWin_StopDependentServices(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps)
/* Stops all services that depend on the service given in srvHandle. If onlyCheckDeps is true,
 * no services are stopped, and the function returns false if other services depend on the
 * srvHandle service. Returns true on success, false otherwise. */
{
 ENUM_SERVICE_STATUS *deps = NULL;
 ENUM_SERVICE_STATUS ess;
 SC_HANDLE depSrvHandle;
 SERVICE_STATUS_PROCESS ssp;
 DWORD bytesNeeded;
 DWORD depCount;
 int i;

 // first call is used to check if there are zero dependent services and get required buffer size

 if (EnumDependentServices(srvHandle, SERVICE_ACTIVE, deps, 0, &bytesNeeded, &depCount)) 
    {
    Debug("No other running services depend on this service\n");
    return true;
    } 
 else 
    {
    if(GetLastError() != ERROR_MORE_DATA)
       {
       CfOut(cf_error,"EnumDependentServices","!! Could not enumerate dependent services");
       return false;
       }

    if(onlyCheckDeps)
       {
       CfOut(cf_error,"","!! Other services depend on this service, but policy says not to stop them");
       return false;
       }
    
    deps = (ENUM_SERVICE_STATUS *)calloc(1, bytesNeeded);

    if(deps == NULL)
       {
       FatalError("Memory allocation in NovaWin_StopDependentServices()");
       }

    if(!EnumDependentServices(srvHandle, SERVICE_ACTIVE, deps, bytesNeeded, &bytesNeeded, &depCount))
       {
       CfOut(cf_error,"","!! Could not enumerate dependent services (after memory allocation)");
       free(deps);
       return false;
       }

    Debug("Stopping %lu dependant services...\n", depCount);

    for(i = 0; i < depCount; i++) 
       {
       ess = *(deps + i);

       Debug("Trying to stop dependant service \"%s\"\n", ess.lpServiceName);
       
       depSrvHandle = OpenService(managerHandle, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);

       if (depSrvHandle == NULL)
          {
          CfOut(cf_error,"OpenService","!! Could not open handle to stop dependent service \"%s\"", ess.lpServiceName);
          free(deps);
          return false;
          }

       if(!ControlService(depSrvHandle, SERVICE_CONTROL_STOP, (SERVICE_STATUS *)&ssp))
          {
          CfOut(cf_error,"","!! Could not send stop signal to dependent service \"%s\"", ess.lpServiceName);
          CloseServiceHandle(depSrvHandle);
          free(deps);
          return false;
          }

       // wait for the service to stop
       if(!NovaWin_ServiceStateWait(depSrvHandle, SERVICE_STOPPED))
          {
          CfOut(cf_error,"","!! Timeout while waiting for dependent service to stop");
          CloseServiceHandle(depSrvHandle);
          free(deps);
          return false;
          }

       CloseServiceHandle(depSrvHandle);
       } 
    }

 free(deps);

 return true;
}

/*****************************************************************************/

int NovaWin_SetServiceStartTime(SC_HANDLE srvHandle, DWORD setState, int onlyFrom, DWORD fromState, int *changeRes)
/* Sets the startup time of a service to setState. If onlyFrom is true, the state is set
 * only if the current state is fromState. If changeRes is not NULL, the change result is set here -
 * 0 means that setState was already the state, 1 means no change due to onlyFrom restriction, 2 means changed.
 * Returns true on success, false otherwise. */
{
 QUERY_SERVICE_CONFIG *srvConfig;

 srvConfig = NovaWin_AllocServiceConfig(srvHandle);

 if(srvConfig == NULL)
    {
    CfOut(cf_error,"","!! Could not get service configuration");
    return false;
    }

 // check if we must change the start time
 
 if(srvConfig->dwStartType != setState)
    {
    if(!onlyFrom || (srvConfig->dwStartType == fromState))
       {
       if(!ChangeServiceConfig(srvHandle, SERVICE_NO_CHANGE, setState, SERVICE_NO_CHANGE,
                               NULL, NULL, NULL, NULL, NULL, NULL, NULL))
          {
          CfOut(cf_error,"ChangeServiceConfig","!! Could not set service enable-status");
          free(srvConfig);
          return false;
          }
       else
          {
          Debug("Successfully changed startup time of service from %lu to %lu\n", srvConfig->dwStartType, setState);
          
          if(changeRes)
             {
             *changeRes = 2;
             }
          }
       }
    else
       {
       Debug("No change in service startup time (onlyFrom=%d, fromState=%lu, currState=%lu)\n", onlyFrom, fromState, srvConfig->dwStartType);

       if(changeRes)
          {
          *changeRes = 1;
          }
       }
    }
 else
    {
    Debug("Service startup time is already correct (is %lu)\n", srvConfig->dwStartType);

    if(changeRes)
       {
       *changeRes = 0;
       }
    }

 free(srvConfig);
 
 return true;
}

/*****************************************************************************/

int NovaWin_SetSrvDepsStartTime(SC_HANDLE managerHandle, SC_HANDLE srvHandle, DWORD setState, int onlyFrom, DWORD fromState)
{
 QUERY_SERVICE_CONFIG *srvConfig;
 SC_HANDLE depHandle;
 char *depName;

 srvConfig = NovaWin_AllocServiceConfig(srvHandle);

 if(srvConfig == NULL)
    {
    CfOut(cf_error,"","!! Could not get service configuration");
    return false;
    }

 for(depName = srvConfig->lpDependencies; depName != NULL && *depName != '\0'; depName += (strlen(depName) + 1))
    {
    Debug("Dependency: \"%s\"\n", depName);
    
    depHandle = OpenService(managerHandle, depName, SERVICE_ALL_ACCESS);
    
    if(depHandle == NULL)
       {
       CfOut(cf_error,"","!! Could not open handle to service \"%s\" in order to enable service dependency", depName);
       free(srvConfig);
       return false;
       }

    Debug("Un-disabling dependencies of service \"%s\", if needed\n", depName);

    if(!NovaWin_SetSrvDepsStartTime(managerHandle, depHandle, setState, onlyFrom, fromState))
        {
        CfOut(cf_error,"","!! Failed while enabling (possibly dependencies of) dependency service \"%s\"", depName);
        free(srvConfig);
        CloseServiceHandle(depHandle);
        return false;
        }

    CloseServiceHandle(depHandle);
    }

 // un-disable this service, if needed
 if(!NovaWin_SetServiceStartTime(srvHandle, SERVICE_DEMAND_START, true, SERVICE_DISABLED, NULL))
    {
    CfOut(cf_error,"","!! Could not enable service dependency - needed before starting it");
    free(srvConfig);
    return false;
    }
 
 free(srvConfig);
 
 return true;
}

/*****************************************************************************/

int NovaWin_ServiceStateWait(SC_HANDLE srvHandle, DWORD state)
/* Waits until the state or timeout is reached, returns true on desired state,
   false on timeout or if other state is reached */
{
 SERVICE_STATUS_PROCESS srvStatus;
 DWORD sleepTime;
 DWORD bytesNeeded;
 int i;

 for(i = 0; i < STATUSWAIT_MAXSLEEP; i++)
    {
    if(!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (BYTE *)&srvStatus, sizeof(srvStatus), &bytesNeeded))
       {
       CfOut(cf_error,"QueryServiceStatusEx","!! Could not get service run status");
       return false;
       }
    
    switch(srvStatus.dwCurrentState)
       {
       case SERVICE_CONTINUE_PENDING:  // wait until we're out of "pending" states
       case SERVICE_PAUSE_PENDING:
       case SERVICE_START_PENDING:
       case SERVICE_STOP_PENDING:

           break;

       default:  // running, paused or stopped

           if(srvStatus.dwCurrentState == state)
              {
              return true;
              }
           else
              {
              return false;
              }
       }

    // compute a reasonable sleep time
    sleepTime = srvStatus.dwWaitHint / 10;

    if(sleepTime < STATUSWAIT_MIN)
       {
       sleepTime = STATUSWAIT_MIN;
       }
    else if (sleepTime > STATUSWAIT_MAX)
       {
       sleepTime = STATUSWAIT_MAX;
       }
    
    Sleep(sleepTime);
    }

 CfOut(cf_error,"","!! Error waiting for state %lu: timeout reached (waited %lu msecs)", state, sleepTime * STATUSWAIT_MAXSLEEP);

 return false;
}

/*****************************************************************************/

void NovaWin_AllocSplitServiceArgs(char *argStr, int *argcp, char ***argvp)
{
 static char arg[CF_MAXSHELLARGS][CF_BUFSIZE];
 char **argv = NULL;
 int argc = 0;
 int i;

 argc = ArgSplitCommand(argStr,arg);
 argv = (char **) malloc((argc+1)*sizeof(char *));

 if(argv == NULL)
    {
    FatalError("Memory allocation in NovaWin_AllocSplitServiceArgs()");
    }
 
 for (i = 0; i < argc; i++)
    {
    argv[i] = arg[i];
    }
   
 argv[i] = (char *) NULL;

 *argvp = argv;
 *argcp = argc;
}

#endif  /* MINGW */
