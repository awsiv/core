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

#include "cf3.defs.h"
#include "cf3.extern.h"

/*****************************************************************************/

static const char *CFCON_VIEWS[] = { "Comp",  // NOTE: must match cfl_view enum
                        "VirtBundleComp",
                        "AggrRepaired",
                        "AggrNotKept",
                        "AggrRepairedReason",
                        "AggrNotKeptReason",
                        "AggrFileChange",
                        "AggrSoftware",
                        "AggrClasses",
                        "Software",
                        "RepairLog",
                        "NotKeptLog",
                        "HubStatus",
                        "HubMeter",
                        "ValueGraph",
                        "HubDetails",             
                        NULL };

/*****************************************************************************/

enum cfl_view Str2View(const char *s)
{
int i;

for (i = 0; CFCON_VIEWS[i] != NULL; i++)
   {
   if (strcmp(s,CFCON_VIEWS[i]) == 0)
      {
      return i;
      }
   }

return cfl_view_error;
}

/*********************************************************************/

const char *View2Str(enum cfl_view view)
{
if(view < 0 || view >= cfl_view_error)
   {
   return "VIEWERROR";
   }

return CFCON_VIEWS[view];
}
