/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "item_lib.h"
#include "conversion.h"
#include "atexit.h"
#include "cfstream.h"
#include "transaction.h"
#include "logging.h"

static int CheckRegistryLogKey(void);

static HANDLE logHandle = NULL;

extern int FACILITY;

static pthread_once_t log_shutdown_once = PTHREAD_ONCE_INIT;

static char *Item2String(Item *ip)
{
    Item *currItem;
    int stringSz = 0;
    char *buf;

    // compute required buffer size
    for (currItem = ip; currItem != NULL; currItem = currItem->next)
    {
        stringSz += strlen(currItem->name);
        stringSz++;             // newline space
    }

    // we automatically get \0-termination because we are not appending a \n after the last item

    buf = xcalloc(1, stringSz);

    // do the copy
    for (currItem = ip; currItem != NULL; currItem = currItem->next)
    {
        strcat(buf, currItem->name);

        if (currItem->next != NULL)     // no newline after last item
        {
            strcat(buf, "\n");
        }
    }

    return buf;
}


/* We use Event Logging on widows. */
void MakeLog(Item *mess, enum cfreport level)
{
    WORD eventType;
    DWORD eventId;
    char *strMsg;
    const char *insertStrings[4] = { 0 };

    if (logHandle == NULL)      // OpenLog not called or failed
    {
        return;
    }

    if (!ThreadLock(cft_output))
    {
        return;
    }

    switch (level)
    {
    case OUTPUT_LEVEL_INFORM:
    case OUTPUT_LEVEL_LOG:
        eventType = EVENTLOG_INFORMATION_TYPE;
        eventId = EVMSG_DEFAULT_INFO;
        break;

    case OUTPUT_LEVEL_REPORTING:
        eventType = EVENTLOG_INFORMATION_TYPE;
        eventId = EVMSG_REPORT;
        break;

    case OUTPUT_LEVEL_VERBOSE:
    case OUTPUT_LEVEL_CMDOUT:
        eventType = EVENTLOG_INFORMATION_TYPE;
        eventId = EVMSG_DEFAULT_VERBOSE;
        break;

    case OUTPUT_LEVEL_ERROR:
        eventType = EVENTLOG_ERROR_TYPE;
        eventId = EVMSG_DEFAULT_ERROR;
        break;

    default:
        ThreadUnlock(cft_output);
        return;
        break;
    }

    // don't include prefix if it is the default
    if (strcmp(VPREFIX, GetConsolePrefix()) == 0)
    {
        insertStrings[0] = "";
    }
    else
    {
        insertStrings[0] = VPREFIX;
    }

    strMsg = Item2String(mess);
    insertStrings[1] = strMsg;
    insertStrings[2] = CF_AGENTTYPES[THIS_AGENT_TYPE];

    ReportEvent(logHandle, eventType, 0, eventId, NULL, 3, 0, (LPCSTR *) insertStrings, NULL);

    free(strMsg);

    ThreadUnlock(cft_output);
}

/*****************************************************************************/

static void RegisterCloseLog(void)
{
    RegisterAtExitFunction(&CloseLog);
}

void OpenLog(int facility)
{
    pthread_once(&log_shutdown_once, &RegisterCloseLog);

    if (!CheckRegistryLogKey())
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! Could not check for logging key in registry");
        return;
    }

    logHandle = RegisterEventSource(NULL, EVENTSOURCE_NAME);

    if (logHandle == NULL)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegisterEventSource", "!! Could not open log");
    }

    FACILITY = facility;
}

/*****************************************************************************/

static int CheckRegistryLogKey(void)
{
    long openRes;
    HKEY keyHandle;
    DWORD existStatus;
    DWORD categoryCount, eventCount;
    char logDllPath[CF_MAXVARSIZE];

    openRes =
        RegCreateKeyEx(HKEY_LOCAL_MACHINE, EVENTSOURCE_HKLM_REGPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                       NULL, &keyHandle, &existStatus);

    if (openRes != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegCreateKeyEx", "!! Could not open registry log key");
        return false;
    }

    switch (existStatus)
    {
    case REG_CREATED_NEW_KEY:
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "Created new registry key for logging");
        break;

    case REG_OPENED_EXISTING_KEY:
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "A registry key for logging already existed");
        break;

    default:
        break;
    }

    // we assume log-dll in WORKDIR\bin
    snprintf(logDllPath, sizeof(logDllPath), "%s/bin/cf.events.dll", CFWORKDIR);
    MapName(logDllPath);

    if (!NovaWin_FileExists(logDllPath))
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! Event-logging dll could not be found in \"%s\"", logDllPath);
    }

    categoryCount = 0;
    eventCount = EVENT_COUNT;

    // create five registry entries used for logging purposes
    if (RegSetValueEx(keyHandle, "CategoryCount", 0, REG_DWORD, (BYTE *) & categoryCount, sizeof(DWORD))
        != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegSetValueEx", "!! Could not set registry log value (CategoryCount)");
        RegCloseKey(keyHandle);
        return false;
    }

    if (RegSetValueEx(keyHandle, "CategoryMessageFile", 0, REG_SZ, logDllPath, strlen(logDllPath) + 1) != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegSetValueEx", "!! Could not set registry log value (CategoryMessageFile)");
        RegCloseKey(keyHandle);
        return false;
    }

    if (RegSetValueEx(keyHandle, "EventMessageFile", 0, REG_SZ, logDllPath, strlen(logDllPath) + 1) != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegSetValueEx", "!! Could not set registry log value (EventMessageFile)");
        RegCloseKey(keyHandle);
        return false;
    }

    if (RegSetValueEx(keyHandle, "ParameterMessageFile", 0, REG_SZ, logDllPath, strlen(logDllPath) + 1)
        != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegSetValueEx", "!! Could not set registry log value (ParameterMessageFile)");
        RegCloseKey(keyHandle);
        return false;
    }

    if (RegSetValueEx(keyHandle, "TypesSupported", 0, REG_DWORD, (BYTE *) & eventCount, sizeof(DWORD)) != ERROR_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "RegSetValueEx", "!! Could not set registry log value (TypesSupported)");
        RegCloseKey(keyHandle);
        return false;
    }

    RegCloseKey(keyHandle);

    return true;
}

/*****************************************************************************/

void CloseLog(void)
{
    if (logHandle != NULL)
    {
        if (!DeregisterEventSource(logHandle))
        {
            CfOut(OUTPUT_LEVEL_ERROR, "DeregisterEventSource", "Could not close windows log\n");
        }
        else
        {
            CfDebug("Windows log successfully closed.\n");
        }

        logHandle = NULL;
    }
}

/*****************************************************************************/

/* Logs promise result. CfOut() should not be called here due to risk of 
 * infinite recursion. */
void LogPromiseResult(char *promiser, char peeType, void *promisee, char status, enum cfreport log_level, Item *mess)
{
    char *strMsg, *strPromisee;
    const char *insertStrings[6] = { 0 };
    char peeBuf[CF_BUFSIZE];
    WORD eventType;
    DWORD eventId;
    Rlist *rp;
    int peebCopied = 0;

    if (!BOOTSTRAP && !Nova_CheckLicenseWin("LogPromiseResult"))
    {
        return;
    }

    if (logHandle == NULL)      // OpenLog not called or failed
    {
        return;
    }

    switch (status)
    {
    case CF_NOP:               // promise kept
    case CF_UNKNOWN:
        eventType = EVENTLOG_INFORMATION_TYPE;
        eventId = EVMSG_PROMISE_KEPT;

        if (log_level != OUTPUT_LEVEL_VERBOSE)
        {
            return;
        }
        break;

    case CF_CHG:               // promise repaired
    case CF_REGULAR:
        eventType = EVENTLOG_INFORMATION_TYPE;
        eventId = EVMSG_PROMISE_REPAIRED;

        if (log_level != OUTPUT_LEVEL_VERBOSE)
        {
            return;
        }
        break;

    case CF_WARN:              // promise not kept, but not repaired due to policy (e.g. dry-run)
        eventType = EVENTLOG_ERROR_TYPE;
        eventId = EVMSG_PROMISE_NOT_REPAIRED_POLICY;
        break;

    case CF_FAIL:              // promise not repaired
    case CF_TIMEX:
    case CF_DENIED:
    case CF_INTERPT:
        eventType = EVENTLOG_ERROR_TYPE;
        eventId = EVMSG_PROMISE_NOT_REPAIRED;
        break;

    default:                   // unknown status
        return;
    }

    if (!ThreadLock(cft_output))
    {
        return;
    }

    // make the promisee into a string
    switch (peeType)
    {
    case RVAL_TYPE_SCALAR:
        strPromisee = (char *) promisee;
        break;

    case RVAL_TYPE_LIST:

        memset(peeBuf, 0, sizeof(peeBuf));

        for (rp = (Rlist *) promisee; rp != NULL; rp = rp->next)
        {
            if (strlen(rp->item) + peebCopied + 2 >= sizeof(peeBuf))
            {
                break;
            }

            strcat(peeBuf, rp->item);
            peebCopied += strlen(rp->item);

            if (rp->next != NULL)
            {
                strcat(peeBuf, ", ");
                peebCopied++;
            }
        }

        strPromisee = peeBuf;
        break;

    default:
        strPromisee = "unspecified promisee";
        break;
    }

    strMsg = Item2String(mess);

    // don't include prefix if it is the default
    if (strcmp(VPREFIX, GetConsolePrefix()) == 0)
    {
        insertStrings[0] = "";
    }
    else
    {
        insertStrings[0] = VPREFIX;
    }

    insertStrings[1] = promiser;
    insertStrings[2] = strPromisee;
    insertStrings[3] = strMsg;
    insertStrings[4] = CF_AGENTTYPES[THIS_AGENT_TYPE];

    ReportEvent(logHandle, eventType, 0, eventId, NULL, 5, 0, (LPCSTR *) insertStrings, NULL);

    free(strMsg);

    ThreadUnlock(cft_output);
}