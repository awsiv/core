/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"

#include "cfstream.h"

bool CFDB_HostsWithClass(Rlist **return_list, char *class_name, char *return_format)
{
    CfOut(cf_error, "", "!! Listing hosts with a class is only available locally on Nova hubs (no binary support)");
    return false;
}

bool Nova_ExecuteRunagent(AgentConnection *conn, const char *menu_str)
{
    return true;
}
