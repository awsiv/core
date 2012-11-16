/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

/* Functions related to windows user and group management. */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "cfstream.h"

/* Finds the security identifier corresponding to a user name
 * (locally or remotely - use "domain_name\user_name" if a specific domain is desired),
 * and puts a pointer to it in the preallocated sid-variable. If should_exist is true,
 * nonexisting user account is treated as an error.
 * Returns true on success, false otherwise.  */
int NovaWin_UserNameToSid(char *user_name, SID *sid, DWORD sid_sz, int should_exist)
{
    DWORD sid_sz_cp = sid_sz;      // to avoid writing to the size parameter
    char dom_name[CF_BUFSIZE];   // we are forced to receive the domain name it is found
    DWORD dom_name_sz = sizeof(dom_name);
    SID_NAME_USE acc_type;

    if (!LookupAccountName(NULL, user_name, sid, &sid_sz_cp, dom_name, &dom_name_sz, &acc_type))
    {
        // nonexisting user is not an error if should_exist is true
        if (should_exist || (GetLastError() != ERROR_NONE_MAPPED))
        {
            CfOut(cf_error, "LookupAccountName", "!! Could not look up user name \"%s\"", user_name);
        }

        return false;
    }

    if (acc_type == SidTypeGroup)        // TODO: Better test ?
    {
        if (should_exist)
        {
            CfOut(cf_error, "", "!! SID type is expected to be user, but is group");
        }

        return false;
    }

    return true;
}

/*******************************************************************/

/* Finds the security identifier corresponding to a group name
 * (locally or remotely - use "domain_name\user_name" if a specific domain is desired),
 * and puts a pointer to it in the preallocated sid-variable. If should_exist is true,
 * nonexisting group account is treated as an error.
 * Returns true on success, false otherwise.  */
int NovaWin_GroupNameToSid(char *group_name, SID *sid, DWORD sid_sz, int should_exist)
{
    DWORD sid_sz_cp = sid_sz;      // to avoid writing to the size parameter
    char dom_name[CF_BUFSIZE];   // we are forced to receive the domain name it is found
    DWORD dom_name_sz = sizeof(dom_name);
    SID_NAME_USE acc_type;

    if (!LookupAccountName(NULL, group_name, sid, &sid_sz_cp, dom_name, &dom_name_sz, &acc_type))
    {
        // nonexisting group is not an error if should_exist is true
        if (should_exist || (GetLastError() != ERROR_NONE_MAPPED))
        {
            CfOut(cf_error, "LookupAccountName", "!! Could not look up group name \"%s\"", group_name);
        }

        return false;
    }

    if (acc_type == SidTypeUser) // TODO: Better test ?
    {
        if (should_exist)
        {
            CfOut(cf_error, "", "!! SID type is expected to be group, but is user");
        }

        return false;
    }

    return true;
}

/*******************************************************************/

int NovaWin_NameToSid(char *name, SID *sid, DWORD sid_sz)
{
    DWORD sid_sz_cp = sid_sz;      // to avoid writing to the size parameter
    char dom_name[CF_BUFSIZE];   // we are forced to receive the domain name it is found
    DWORD dom_name_sz = sizeof(dom_name);
    SID_NAME_USE acc_type;

    if (!LookupAccountName(NULL, name, sid, &sid_sz_cp, dom_name, &dom_name_sz, &acc_type))
    {
        CfOut(cf_error, "LookupAccountName", "!! Could not look up name \"%s\"", name);
        return false;
    }

    return true;
}

/*******************************************************************/

/* Converts a sid to a user/group name, and writes it to 'name'
 * of size 'name_sz' */
int NovaWin_SidToName(SID *sid, char *name, int name_sz)
{
    SID_NAME_USE sid_name_use;
    char dom_name[CF_BUFSIZE];
    DWORD req_name_sz = (DWORD) name_sz;
    DWORD req_dom_sz = (DWORD) sizeof(dom_name);

    if (!LookupAccountSid(NULL, sid, name, &req_name_sz, dom_name, &req_dom_sz, &sid_name_use))
    {
        CfOut(cf_error, "LookupAccountSid", "!! Could not find name corresponding to sid");
        return false;
    }

    return true;
}

/*******************************************************************/

/* Converts a SID to a string representation. The string must be preallocated. */
int NovaWin_SidToString(SID *sid, char *string_sid, int stringSz)
{
    char *string_alloc;
    int string_alloc_sz;

    if (!ConvertSidToStringSid(sid, &string_alloc))
    {
        CfOut(cf_error, "ConvertSidToStringSid", "Could not convert SID to string");
        return false;
    }

    string_alloc_sz = strlen(string_alloc);

    if (string_alloc_sz >= stringSz)
    {
        CfOut(cf_error, "", "String buffer is too small");
        LocalFree(string_alloc);
        return false;
    }

    strcpy(string_sid, string_alloc);
    LocalFree(string_alloc);

    return true;
}

/*******************************************************************/

/* Converts a SID to a string representation. The string must be preallocated. */
int NovaWin_StringToSid(char *string_sid, SID *sid, int sid_sz)
{
    SID *sid_alloc;
    size_t sid_len;

    if (!ConvertStringSidToSid(string_sid, (PSID *) & sid_alloc))
    {
        CfOut(cf_error, "ConvertStringSidToSid", "Could not obtain SID \"%s\"", string_sid);
        return false;
    }

    sid_len = (size_t) GetLengthSid(sid_alloc);

    if (sid_len > sid_sz)
    {
        CfOut(cf_error, "", "SID buffer is too small");
        LocalFree(sid_alloc);
        return false;
    }

    memcpy(sid, sid_alloc, sid_len);

    LocalFree(sid_alloc);

    return true;
}

/*******************************************************************/

UidList *NovaWin_Rlist2SidList(Rlist *uidnames)
{
    UidList *uidlist = NULL, *curr_el = NULL;
    Rlist *rp;
    char sid_buf[CF_MAXSIDSIZE];


    for (rp = uidnames; rp != NULL; rp = rp->next)
    {
        if (NovaWin_NameToSid(rp->item, (SID *) sid_buf, sizeof(sid_buf)))
        {
            // allocate new element

            if (uidlist == NULL)        // first element
            {
                uidlist = xcalloc(1, sizeof(UidList));

                curr_el = uidlist;
            }
            else                // not first element
            {
                curr_el->next = xcalloc(1, sizeof(UidList));

                curr_el = curr_el->next;
            }

            memcpy(curr_el->sid, sid_buf, CF_MAXSIDSIZE);
        }
    }

    if (uidlist == NULL)        // empty uidnames or no user in it found on this system
    {
        uidlist = xcalloc(1, sizeof(UidList));
    }

    return uidlist;
}

/*******************************************************************/

FnCallResult FnCallUserExists(FnCall *fp, Rlist *finalargs)
{
    char user_sid[CF_MAXSIDSIZE];
    char buffer[CF_BUFSIZE];
    char *arg = finalargs->item;

    if (NovaWin_UserNameToSid(arg, (SID *) user_sid, sizeof(user_sid), false))
    {
        strcpy(buffer, CF_ANYCLASS);
    }
    else
    {
        strcpy(buffer, "!any");
    }

    return (FnCallResult) { FNCALL_SUCCESS, { xstrdup(buffer), CF_SCALAR } };
}

/*********************************************************************/

FnCallResult FnCallGroupExists(FnCall *fp, Rlist *finalargs)
{
    char group_sid[CF_MAXSIDSIZE];
    char buffer[CF_BUFSIZE];
    char *arg = finalargs->item;

    if (NovaWin_GroupNameToSid(arg, (SID *) group_sid, sizeof(group_sid), false))
    {
        strcpy(buffer, CF_ANYCLASS);
    }
    else
    {
        strcpy(buffer, "!any");
    }

    return (FnCallResult) { FNCALL_SUCCESS, { xstrdup(buffer), CF_SCALAR } };
}
