/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_user.c                                                          */
/*                                                                           */
/* Created: Mon Sep 28 13:07:28 2009                                         */
/*                                                                           */
/*****************************************************************************/

/* Functions related to windows user and group management. */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW


/* Finds the security identifier corresponding to a user name 
 * (locally or remotely - use "domain_name\user_name" if a specific domain is desired),
 * and puts a pointer to it in the preallocated sid-variable.
 * Returns true on success, false otherwise.  */
int NovaWin_UserNameToSid(char *userName, SID *sid, DWORD sidSz)
{
  DWORD sidSzCp = sidSz;  // to avoid writing to the size parameter
  char domName[CF_BUFSIZE];  // we are forced to receive the domain name it is found
  DWORD domNameSz = sizeof(domName);
  SID_NAME_USE accType;

  if(!LookupAccountName(NULL, userName, sid, &sidSzCp, domName, &domNameSz, &accType))
    {
      CfOut(cf_error,"LookupAccountName","Could not look up user name");
      return false;
    }

  if(accType == SidTypeGroup)  // TODO: Better test ?
    {
      CfOut(cf_error,"","SID type is expected to be user, but is group");
      return false;
    }

  return true;
}


/* Finds the security identifier corresponding to a group name 
 * (locally or remotely - use "domain_name\user_name" if a specific domain is desired),
 * and puts a pointer to it in the preallocated sid-variable.
 * Returns true on success, false otherwise.  */
int NovaWin_GroupNameToSid(char *groupName, SID *sid, DWORD sidSz)
{
  DWORD sidSzCp = sidSz;  // to avoid writing to the size parameter
  char domName[CF_BUFSIZE];  // we are forced to receive the domain name it is found
  DWORD domNameSz = sizeof(domName);
  SID_NAME_USE accType;

  if(!LookupAccountName(NULL, groupName, sid, &sidSzCp, domName, &domNameSz, &accType))
    {
      CfOut(cf_error,"LookupAccountName","Could not look up group name");
      return false;
    }

  if(accType == SidTypeUser)  // TODO: Better test ?
    {
      CfOut(cf_error,"","SID type is expected to be group, but is user");
      return false;
    }

  return true;
}


/* Converts a SID to a string representation. The string must be preallocated. */
int NovaWin_SidToString(SID *sid, char *stringSid, int stringSz)
{
  char *stringAlloc;
  int stringAllocSz;

  if(!ConvertSidToStringSid(sid, &stringAlloc))
    {
      CfOut(cf_error,"ConvertSidToStringSid","Could not convert SID to string");
      return false;
    }

  stringAllocSz = strlen(stringAlloc);

  if(stringAllocSz >= stringSz)
    {
      CfOut(cf_error,"","String buffer is too small");
      LocalFree(stringAlloc);
      return false;
    }
      
  strcpy(stringSid, stringAlloc);
  LocalFree(stringAlloc);
  
  return true;
}


/* Converts a SID to a string representation. The string must be preallocated. */
int NovaWin_StringToSid(char *stringSid, SID *sid, int sidSz)
{
  SID *sidAlloc;
  size_t sidLen;

  if(!ConvertStringSidToSid(stringSid, (PSID*)&sidAlloc))
    { 
      CfOut(cf_error,"ConvertStringSidToSid","Could not obtain SID \"%s\"", stringSid);
      return false;
    }

  sidLen = (size_t)GetLengthSid(sidAlloc);
  
  if(sidLen > sidSz)
    {
      CfOut(cf_error,"","SID buffer is too small");
      LocalFree(sidAlloc);
      return false;
    }

  memcpy(sid, sidAlloc, sidLen);
  
  LocalFree(sidAlloc);

  return true;
}

#endif  /* MINGW */
