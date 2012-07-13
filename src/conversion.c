/*
   Copyright (C) Cfengine AS

   This file is part of Cfengine 3 - written and maintained by Cfengine AS.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

  To the extent this program is licensed as part of the Enterprise
  versions of Cfengine, the applicable Commerical Open Source License
  (COSL) may apply to this file if you as a licensee so wish it. See
  included file COSL.txt.
*/

#include "conversion.h"

#include "env_context.h"
#include "db_common.h"
#include "db_query.h"

#include <assert.h>

static Rlist *HubHostListToRlist(Rlist *hub_host_list, char *return_format);


int Nova_GetReportedScalar(char *hostkey, char *scope, char *lval, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, NULL, NULL, false, 0, time(NULL), NULL);

    returnval[0] = '\0';

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hv = (HubVariable *) rp->item;

        if (strlen(hv->dtype) > 1)      // list
        {
            char b[CF_BUFSIZE];

            b[0] = '\0';
            PrintRlist(b, CF_BUFSIZE, hv->rval.item);
            snprintf(returnval, bufsize - 1, "%s", b);
        }
        else
        {
            snprintf(returnval, bufsize - 1, "%s", (char *) hv->rval.item);
        }
    }

    if (hq->records == NULL)
    {
        snprintf(buffer, sizeof(buffer), "Unknown value");
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova_GetReportedList(char *hostkey, char *scope, char *lval, Rlist **list)
/* This function allocates memory which needs to be deleted afterwards */
{
    char buffer[CF_BUFSIZE];
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, NULL, NULL, false, 0, time(NULL), NULL);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hv = (HubVariable *) rp->item;

        if (strlen(hv->dtype) > 1)      // list
        {
            *list = CopyRvalItem((Rval) {hv->rval.item, CF_LIST}).item;
        }
        else
        {
            // Shoud not get here      
        }
    }

    if (hq->records == NULL)
    {
        snprintf(buffer, sizeof(buffer), "Unknown value");
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

bool CFDB_HostsWithClass(Rlist **return_list, char *class_name, char *return_format)
{
    if(!IsDefinedClass("am_policy_hub"))
    {
        CfOut(cf_error, "", "!! Listing hosts with a class is only available locally on Nova hubs (not running as a hub)");
        return false;
    }
    
    EnterpriseDB conn;
    
    if(!CFDB_Open(&conn))
    {
        return false;
    }
    
    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);
    HostClassFilterAddClasses(filter, class_name, NULL);
    
    HubQuery *hq = CFDB_QueryHostsByHostClassFilter(&conn, filter);

    DeleteHostClassFilter(filter);
    CFDB_Close(&conn);

    *return_list = HubHostListToRlist(hq->hosts, return_format);

    DeleteHubQuery(hq, NULL);
    
    return true;
}

static Rlist *HubHostListToRlist(Rlist *hub_host_list, char *return_format)
{
    bool return_ip_address;

    if(strcmp(return_format, "address") == 0)
    {
        return_ip_address = true;
    }
    else if(strcmp(return_format, "name") == 0)
    {
        return_ip_address = false;
    }
    else
    {
        FatalError("HubHostListToRlist: Unknown return format %s", return_format);
    }
    
    Rlist *return_list = NULL;
    
    for(Rlist *rp = hub_host_list; rp != NULL; rp = rp->next)
    {
        HubHost *hh = (HubHost *)rp->item;

        if(return_ip_address)
        {
            PrependRScalar(&return_list, hh->ipaddr, CF_SCALAR);
        }
        else
        {
            PrependRScalar(&return_list, hh->hostname, CF_SCALAR);
        }
    }
    
    return return_list;
}

const char *Nova_ShortArch(const char *arch)
{
    if (strcmp(arch, "i386") == 0)
    {
        return "3";
    }

    if (strcmp(arch, "i486") == 0)
    {
        return "4";
    }

    if (strcmp(arch, "i586") == 0)
    {
        return "5";
    }

    if (strcmp(arch, "i686") == 0)
    {
        return "6";
    }

    if (strcmp(arch, "noarch") == 0)
    {
        return "";
    }

    if (strcmp(arch, "x86_64") == 0)
    {
        return "x";
    }

    if (strcmp(arch, "s390") == 0)
    {
        return "t";
    }

    if (strcmp(arch, "s390x") == 0)
    {
        return "s";
    }

    if (strcmp(arch, "default") == 0)
    {
        return "d";
    }

    return arch;
}

const char *Nova_LongArch(const char *arch)
{
    if (arch == NULL)
    {
        return "*";
    }

    if (strcmp(arch, "3") == 0)
    {
        return "i386";
    }

    if (strcmp(arch, "4") == 0)
    {
        return "i486";
    }

    if (strcmp(arch, "5") == 0)
    {
        return "i586";
    }

    if (strcmp(arch, "6") == 0)
    {
        return "i686";
    }

    if (strcmp(arch, "") == 0)
    {
        return "noarch";
    }

    if (strcmp(arch, "\n") == 0)
    {
        return "*";
    }

    if (strcmp(arch, "x") == 0)
    {
        return "x86_64";
    }

    if (strcmp(arch, "t") == 0)
    {
        return "s390";
    }

    if (strcmp(arch, "s") == 0)
    {
        return "s390x";
    }

    if (strcmp(arch, "d") == 0)
    {
        return "default";
    }

    return arch;
}

int Nova_CoarseLaterThan(const char *bigger, const char *smaller)
{
    char month_small[CF_SMALLBUF];
    char month_big[CF_SMALLBUF];
    int m_small, day_small, year_small, m_big, year_big, day_big;

    sscanf(smaller, "%d %s %d", &day_small, month_small, &year_small);
    sscanf(bigger, "%d %s %d", &day_big, month_big, &year_big);

    if (year_big < year_small)
    {
        return false;
    }

    m_small = Month2Int(month_small);
    m_big = Month2Int(month_big);

    if (m_big < m_small)
    {
        return false;
    }

    if (day_big < day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    return true;
}

int Nova_LaterThan(const char *bigger, const char *smaller)
{
    char month_small[CF_SMALLBUF];
    char month_big[CF_SMALLBUF];
    int m_small, day_small, year_small, m_big, year_big, day_big;
    int min_small, min_big, hour_small, hour_big;

// Format: Fri Mar 27 15:45:52 2009

    month_small[0] = '\0';
    month_big[0] = '\0';

    sscanf(smaller, "%*s %s %d %d:%d:%*d %d", month_small, &day_small, &hour_small, &min_small, &year_small);
    sscanf(bigger, "%*s %s %d %d:%d:%*d %d", month_big, &day_big, &hour_big, &min_big, &year_big);

    if (year_big < year_small)
    {
        return false;
    }

    m_small = Month2Int(month_small);
    m_big = Month2Int(month_big);

    if (m_big < m_small)
    {
        return false;
    }

    if (day_big < day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    if (hour_big < hour_small && day_big == day_small && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    if (min_big < min_small && hour_big == hour_small && day_big == day_small
        && m_big == m_small && year_big == year_small)
    {
        return false;
    }

    return true;
}
