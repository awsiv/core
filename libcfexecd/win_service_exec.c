/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

/* Functions to run the cf-execd as a windows service.
 * It can for example be registered in windows as follows:
 * sc create CfengineNovaExec binpath= "c:\Program Files\Cfengine\bin\cf-execd.exe"
 * sc config CfengineNovaExec DisplayName= "Cfengine Nova Executor"
 * sc description CfengineNovaExec "The executor daemon is a scheduler and wrapper for execution of cf-agent. It collects the output of the agent and can email it to a specified address. It can splay the start time of executions across the network and work as a class-based clock for scheduling."
 * sc config CfengineNovaExec start= demand
 * sc start CfengineNovaExec
 */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "cfstream.h"
#include "cf-execd-runner.h"
#include "reporting.h"
#include "string_lib.h"
#include "logging.h"
#include "generic_agent.h"

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE statusHandle;

/* prototypes */
void NovaWin_ServiceMain(int argc, char *argv[]);
void NovaWin_ControlHandler(DWORD request);

/* core/src/atexit.c */
void CallAtExitFunctions(void);

void StartServer(Policy *policy, ExecConfig *config, const ReportContext *report_context);
void KeepPromises(Policy *policy, ExecConfig *config);

void NovaWin_StartExecService(void)
{
    SERVICE_TABLE_ENTRY serviceTable[2];

    serviceTable[0].lpServiceName = WINSERVICE_NAME;
    serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION) NovaWin_ServiceMain;

    serviceTable[1].lpServiceName = NULL;
    serviceTable[1].lpServiceProc = NULL;

    // Start the control dispatcher thread for our service
    StartServiceCtrlDispatcher(serviceTable);
}

/*******************************************************************/

void NovaWin_ServiceMain(int argc, char *argv[])
/* Called by Windows Service Control Manager */
{
    serviceStatus.dwServiceType = SERVICE_WIN32;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    statusHandle = RegisterServiceCtrlHandler(WINSERVICE_NAME, (LPHANDLER_FUNCTION) NovaWin_ControlHandler);

    if (!statusHandle)
    {
        // Registering Control Handler failed
        CfOut(cf_error, "RegisterServiceCtrlHandler", "!! Could not register control handler for cf-execd service");
        return;
    }

    // report running to SCM
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(statusHandle, &serviceStatus);

    GenericAgentConfig config = GenericAgentDefaultConfig(AGENT_TYPE_EXECUTOR);

    ReportContext *report_context = OpenReports("executor");
    Policy *policy = GenericInitialize("executor", config, report_context);

    ExecConfig exec_config = {
        .scheduled_run = true,
        .exec_command = SafeStringDuplicate(""),
        .mail_server = SafeStringDuplicate(""),
        .mail_from_address = SafeStringDuplicate(""),
        .mail_to_address = SafeStringDuplicate(""),
        .mail_max_lines = 30,
        .fq_name = VFQNAME,
        .ip_address = VIPADDRESS,
        .agent_expireafter = 10080,
    };

    KeepPromises(policy, &exec_config);

    CfOut(cf_log, "", "Started service %s", WINSERVICE_NAME);

    // worker function (loop)
    StartServer(policy, &exec_config, report_context);
    ReportContextDestroy(report_context);
}

/*******************************************************************/

void NovaWin_ControlHandler(DWORD request)
/* Callback function on e.g. system shutdown or service stop signal */
{
    switch (request)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        CfOut(cf_log, "", "Terminating service %s: received stop request (%lu)", WINSERVICE_NAME, request);

        /* Windows service does not call atexit(3)-registered functions on
         * shutdown, so call them explicitely */

        CallAtExitFunctions();

        serviceStatus.dwWin32ExitCode = 0;
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(statusHandle, &serviceStatus);

    default:

        // report current status
        SetServiceStatus(statusHandle, &serviceStatus);
        break;
    }

}
