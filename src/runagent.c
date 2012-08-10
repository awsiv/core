/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "db_common.h"
#include "item_lib.h"
#include "conversion.h"

/***********************************************************************/

bool Nova_ExecuteRunagent(AgentConnection *conn, const char *menu_str)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    enum cfd_menu menu = String2Menu(menu_str);

    switch (menu)
    {
    case cfd_menu_delta:
        Nova_QueryClientForReports(&dbconn, conn, menu_str, time(0) - SECONDS_PER_MINUTE * 10);
        break;
    case cfd_menu_full:
        Nova_QueryClientForReports(&dbconn, conn, menu_str, time(0) - SECONDS_PER_WEEK);
        break;
    default:
        break;
    }

    CFDB_Close(&dbconn);
    return true;
}
