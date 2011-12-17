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

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC

int Nova_GetReportedScalar(char *hostkey,char *scope,char *lval,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE];
 struct HubVariable *hv;
 struct HubQuery *hq;
 struct Rlist *rp;
 mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   return false;
   }

hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,NULL,NULL,false,NULL);

returnval[0] = '\0';

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hv = (struct HubVariable *)rp->item;
   
   if (strlen(hv->dtype) > 1) // list
      {
      char b[CF_BUFSIZE];
      b[0] = '\0';
      PrintRlist(b,CF_BUFSIZE,hv->rval);
      snprintf(returnval,bufsize-1,"%s",b);
      }
   else
      {
      snprintf(returnval,bufsize-1,"%s",(char *)hv->rval);
      }
   }

if (hq->records == NULL)
   {
   snprintf(buffer,sizeof(buffer),"Unknown value");
   }

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

/*****************************************************************************/

int Nova_GetReportedList(char *hostkey,char *scope,char *lval,struct Rlist **list)

/* This function allocates memory which needs to be deleted afterwards */
    
{ char buffer[CF_BUFSIZE];
  struct HubVariable *hv;
  struct HubQuery *hq;
  struct Rlist *rp;
  mongo_connection dbconn;

if (!CFDB_Open(&dbconn))
   {
   return false;
   }

hq = CFDB_QueryVariables(&dbconn,hostkey,scope,lval,NULL,NULL,false,NULL);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hv = (struct HubVariable *)rp->item;
   
   if (strlen(hv->dtype) > 1) // list
      {
      *list = CopyRvalItem((struct Rval) { hv->rval, CF_LIST }).item;
      }
   else
      {
      // Shoud not get here      
      }
   }

if (hq->records == NULL)
   {
   snprintf(buffer,sizeof(buffer),"Unknown value");
   }

DeleteHubQuery(hq,DeleteHubVariable);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to report database");
   }

return true;
}

#endif
