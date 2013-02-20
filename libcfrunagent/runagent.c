/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "db_common.h"
#include "item_lib.h"
#include "conversion.h"
#include "datapack.h"

/***********************************************************************/

bool ExecuteRunagent(AgentConnection *conn, const char *menu_str)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    ReportRequestType menu = ReportRequestTypeFromString(menu_str);

    switch (menu)
    {
    case REPORT_REQUEST_TYPE_DELTA:
        Nova_QueryClientForReports(&dbconn, conn, menu_str, time(0) - SECONDS_PER_MINUTE * 10);
        break;
    case REPORT_REQUEST_TYPE_FULL:
        Nova_QueryClientForReports(&dbconn, conn, menu_str, time(0) - SECONDS_PER_WEEK);
        break;
    default:
        break;
    }

    CFDB_Close(&dbconn);
    return true;
}
