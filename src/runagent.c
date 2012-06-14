/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "db_common.h"
#include "item_lib.h"

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

    case cfd_menu_relay:
# ifdef HAVE_CONSTELLATION
    {
        Item *queries = Constellation_CreateAllQueries();

        Constellation_QueryRelay(&dbconn, conn, queries);
        DeleteItemList(queries);
    }
# endif
        break;

    default:
        break;
    }

    CFDB_Close(&dbconn);
    return true;
}
