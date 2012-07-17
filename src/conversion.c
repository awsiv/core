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

#include <assert.h>

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

bool BundleQualifiedNameSplit(const char *qualified_bundle_name, char namespace_out[CF_MAXVARSIZE], char bundle_name_out[CF_MAXVARSIZE])
{
    Rlist *parts = SplitStringAsRList(qualified_bundle_name, '.');
    if (parts)
    {
        const char *first = ScalarValue(parts);

        if (parts->next)
        {
            const char *second = ScalarValue(parts->next);
            strncpy(namespace_out, first, CF_MAXVARSIZE);
            strncpy(bundle_name_out, second, CF_MAXVARSIZE);
        }
        else
        {
            strncpy(namespace_out, "\0", 1);
            strncpy(bundle_name_out, first, CF_MAXVARSIZE);
        }

        DeleteRlist(parts);
        return true;
    }

    return false;
}
