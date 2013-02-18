/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "cfstream.h"
#include "item_lib.h"
#include "conversion.h"
#include "verify_processes.h"
#include "logging.h"

// TODO: Implement ? - we only support one signal: SIGKILL (9)
/* Returns true if SIGKILL was one of the signals, false otherwise */

int DoAllSignals(Item *siglist, Attributes a, Promise *pp)
{
    if (!BOOTSTRAP && !Nova_CheckLicenseWin("DoAllSignals"))
    {
        return 0;
    }

    int kill = false;
    int signal;
    Item *ip;
    Rlist *rp;
    pid_t pid;

    if (siglist == NULL)
    {
        return 0;
    }

    if (a.signals == NULL)
    {
        CfOut(OUTPUT_LEVEL_INFORM, "", " -> No signals to send for %s\n", pp->promiser);
        return 0;
    }

    for (ip = siglist; ip != NULL; ip = ip->next)
    {
        pid = ip->counter;

        for (rp = a.signals; rp != NULL; rp = rp->next)
        {
            signal = Signal2Int(rp->item);

            if (signal != SIGKILL)
            {
                CfOut(OUTPUT_LEVEL_VERBOSE, "", "The only supported signal on windows is 'kill'");
                continue;
            }

            kill = true;

            if (!DONTDO)
            {
                if (!GracefulTerminate(pid))
                {
                    cfPS(OUTPUT_LEVEL_VERBOSE, CF_FAIL, "", pp, a, " !! Couldn't terminate process with pid %jd\n", (intmax_t)pid);
                    continue;
                }
                else
                {
                    cfPS(OUTPUT_LEVEL_INFORM, CF_CHG, "", pp, a, " -> Terminated process with pid %jd\n", (intmax_t)pid);
                    break;
                }
            }
            else
            {
                CfOut(OUTPUT_LEVEL_ERROR, "", " -> Need to terminate process with pid %jd", (intmax_t)pid);
            }
        }
    }

    return kill;
}

/*****************************************************************************/

/* Terminates the process identified by pid.
 * TODO: Try to send quit-message to process before terminating it ? */
int GracefulTerminate(pid_t pid)
{
    int res;
    HANDLE procHandle;

    // get process handle from pid
    procHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if (procHandle == NULL)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "OpenProcess", "!! Could not get process handle");
        return false;
    }

    res = TerminateProcess(procHandle, -1);

    if (!CloseHandle(procHandle))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "CloseHandle", "!! Could not close process handle");
    }

    return (res != 0);
}

/*****************************************************************************/

void ProcessSignalTerminate(pid_t pid)
{
    GracefulTerminate(pid);
}

/*****************************************************************************/

int NovaWin_IsProcessRunning(pid_t pid)
{
    DWORD ret;
    HANDLE procHandle;

    procHandle = OpenProcess(SYNCHRONIZE, FALSE, pid);

    if (procHandle == NULL)
    {
        return false;
    }

    ret = WaitForSingleObject(procHandle, 0);
    CloseHandle(procHandle);

    return (ret == WAIT_TIMEOUT);
}

/*****************************************************************************/

/* returns true if execution of 'cmd' returns zero
 * (waits for completion)  */
int ShellCommandReturnsZero(const char *comm, int useshell)
{
    HANDLE procHandle;
    DWORD exitcode;

    if (!NovaWin_RunCmd(comm, useshell, false, NULL, NULL, &procHandle))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RunCmd", "!! Command \"%s\" failed", comm);
        exit(1);
    }

    if (WaitForSingleObject(procHandle, INFINITE) == WAIT_FAILED)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "WaitForSingleObject", "!! Error waiting for process to finish");
        exit(1);
    }

    if (!GetExitCodeProcess(procHandle, &exitcode))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "GetExitCodeProcess", "!! Error getting exit code");
        exit(1);
    }

    CloseHandle(procHandle);

    return (exitcode == 0);
}

/*****************************************************************************/

/* Runs the command comm, in a shell if useshell is true. The new process inherit handles if
 * inheritHandles is true. If si is not NULL, these parameters are used to create the process.
 * The process handle of the new process is put in procHandle, this needs to be closed with
 * CloseHandle(). The first parameter in comm is the executable, and it must be quoted if 
 * it contains spaces (e.g. ""C:\Program Files\Cfengine\bin\cf-promises.exe" -f file.cf").
 * Returns true on success, false otherwise. */
int NovaWin_RunCmd(const char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle)
{
    STARTUPINFO emptySi;
    PROCESS_INFORMATION pi;
    char buf[CF_BUFSIZE];
    char cmdPath[CF_BUFSIZE];
    char *binary;
    char *binaryParams;

    if (useshell)
    {
        if (sizeof("/c \"") + strlen(comm) + 1 >= sizeof(buf))
        {
            CfOut(OUTPUT_LEVEL_ERROR, "", "!! Buffer to small to hold command-string");
            return false;
        }

        strcpy(buf, "/c \"");
        strcat(buf, comm);
        strcat(buf, "\"");

        if (!NovaWin_GetSysDir(cmdPath, sizeof(cmdPath) - sizeof("\\cmd.exe")))
        {
            CfOut(OUTPUT_LEVEL_ERROR, "", "!! Could not get system directory and thus not cmd.exe directory");
            return false;
        }

        strcat(cmdPath, "\\cmd.exe");

        binary = cmdPath;
        binaryParams = xstrdup(buf);
    }
    else
    {
        binary = NULL;
        binaryParams = xstrdup(comm);
    }

    // use empty startupinfo struct if it is unspecified
    if (si == NULL)
    {
        memset(&emptySi, 0, sizeof(emptySi));
        emptySi.cb = sizeof(emptySi);
        si = &emptySi;
    }

    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(binary, binaryParams, NULL, NULL, inheritHandles, 0, NULL, startDir, si, &pi))
    {
        free(binaryParams);
        CfOut(OUTPUT_LEVEL_ERROR, "CreateProcess", "!! Failed to start process");
        return false;
    }

    free(binaryParams);
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

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &currProcToken))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "OpenProcessToken", "!! Could not get access token of current process");
        return false;
    }

    // get required buffer size first
    if (!GetTokenInformation(currProcToken, TokenUser, userToken, 0, &reqBufSz))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            CfOut(OUTPUT_LEVEL_ERROR, "GetTokenInformation", "!! Could not get owner information on current process");
            CloseHandle(currProcToken);
            return false;
        }
    }
    else                        // the call should fail according to doc
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! Could not get required buffer size");
        CloseHandle(currProcToken);
        return false;
    }

    // allocate enough buffer bytes and do real call
    userToken = xcalloc(1, reqBufSz);

    if (!GetTokenInformation(currProcToken, TokenUser, userToken, reqBufSz, &reqBufSz))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "GetTokenInformation", "!! Could not get owner information on current process");
        free(userToken);
        CloseHandle(currProcToken);
        return false;
    }

    ownerSidSz = GetLengthSid(userToken->User.Sid);

    if (ownerSidSz > sidSz)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! Sid buffer too small");
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
int GetCurrentUserName(char *userName, int userNameLen)
{
    DWORD userNameMax = (DWORD) userNameLen;

    if (!GetUserName(userName, &userNameMax))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "GetUserName", "!! Could not get user name of current process, using \"UNKNOWN\"");

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

    if (!LookupPrivilegeValue(NULL, privilegeName, &luid))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "LookupPrivilegeValue", "!! Could not get priviligege");
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
        CfOut(OUTPUT_LEVEL_ERROR, "AdjustTokenPrivileges", "!! Could not set privilege");
        return false;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "AdjustTokenPrivileges", "!! The token does not have the desired privilege");
        return false;
    }

    return true;
}
