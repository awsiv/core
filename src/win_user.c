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
 * and puts a pointer to it in the preallocated sid-variable. If shouldExist is true,
 * nonexisting user account is treated as an error.
 * Returns true on success, false otherwise.  */
int NovaWin_UserNameToSid(char *userName, SID *sid, DWORD sidSz, int shouldExist)
{
  DWORD sidSzCp = sidSz;  // to avoid writing to the size parameter
  char domName[CF_BUFSIZE];  // we are forced to receive the domain name it is found
  DWORD domNameSz = sizeof(domName);
  SID_NAME_USE accType;

  if(!LookupAccountName(NULL, userName, sid, &sidSzCp, domName, &domNameSz, &accType))
    {
      // nonexisting user is not an error if shouldExist is true
      if(shouldExist || (GetLastError() != ERROR_NONE_MAPPED))  
	{
	  CfOut(cf_error,"LookupAccountName","!! Could not look up user name \"%s\"", userName);
	}

      return false;
    }

  if(accType == SidTypeGroup)  // TODO: Better test ?
    {
      if(shouldExist)
	{
	  CfOut(cf_error,"","!! SID type is expected to be user, but is group");
	}

      return false;
    }

  return true;
}

/*******************************************************************/

/* Finds the security identifier corresponding to a group name 
 * (locally or remotely - use "domain_name\user_name" if a specific domain is desired),
 * and puts a pointer to it in the preallocated sid-variable. If shouldExist is true,
 * nonexisting group account is treated as an error.
 * Returns true on success, false otherwise.  */
int NovaWin_GroupNameToSid(char *groupName, SID *sid, DWORD sidSz, int shouldExist)
{
  DWORD sidSzCp = sidSz;  // to avoid writing to the size parameter
  char domName[CF_BUFSIZE];  // we are forced to receive the domain name it is found
  DWORD domNameSz = sizeof(domName);
  SID_NAME_USE accType;

  if(!LookupAccountName(NULL, groupName, sid, &sidSzCp, domName, &domNameSz, &accType))
    {
      // nonexisting group is not an error if shouldExist is true
      if(shouldExist || (GetLastError() != ERROR_NONE_MAPPED))
	{
	  CfOut(cf_error,"LookupAccountName","!! Could not look up group name \"%s\"", groupName);
	}

      return false;
    }

  if(accType == SidTypeUser)  // TODO: Better test ?
    {
      if(shouldExist)
	{
	  CfOut(cf_error,"","!! SID type is expected to be group, but is user");
	}

      return false;
    }

  return true;
}

/*******************************************************************/

int NovaWin_NameToSid(char *name, SID *sid, DWORD sidSz)
{
  DWORD sidSzCp = sidSz;  // to avoid writing to the size parameter
  char domName[CF_BUFSIZE];  // we are forced to receive the domain name it is found
  DWORD domNameSz = sizeof(domName);
  SID_NAME_USE accType;

  if(!LookupAccountName(NULL, name, sid, &sidSzCp, domName, &domNameSz, &accType))
    {
      CfOut(cf_error,"LookupAccountName","!! Could not look up name \"%s\"", name);
      return false;
    }

  return true;
}

/*******************************************************************/

/* Converts a sid to a user/group name, and writes it to 'name'
 * of size 'nameSz' */
int NovaWin_SidToName(SID* sid, char *name, int nameSz)
{
  SID_NAME_USE sidNameUse;
  char domName[CF_BUFSIZE];
  DWORD reqNameSz = (DWORD)nameSz;
  DWORD reqDomSz = (DWORD)sizeof(domName);
  

  if(!LookupAccountSid(NULL, sid, name, &reqNameSz, domName, &reqDomSz, &sidNameUse))
    {
      CfOut(cf_error,"LookupAccountSid","!! Could not find name corresponding to sid");
      return false;      
    }

  return true;
}

/*******************************************************************/

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

/*******************************************************************/

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

/*******************************************************************/

struct UidList *NovaWin_Rlist2SidList(struct Rlist *uidnames, struct Promise *pp)
{
  struct UidList *uidlist, *currEl;
  struct Rlist *rp;
  char sidBuf[CF_MAXSIDSIZE];

  currEl = NULL;
  uidlist = NULL;

  for(rp = uidnames; rp != NULL; rp = rp->next)
    {
      if(NovaWin_NameToSid(rp->item, (SID *)sidBuf, sizeof(sidBuf)))
	{
	  // allocate new element
	  
	  if(uidlist == NULL)  // first element
	    { 
	      uidlist = (struct UidList *)calloc(1, sizeof(struct UidList));
	      
	      if(uidlist == NULL)
		{
		  FatalError("Memory allocation in NovaWin_Rlist2SidList() - 1");
		}

	      currEl = uidlist;
	    }
	  else  // not first element
	    {
	      currEl->next = (struct UidList *)calloc(1, sizeof(struct UidList));
	      
	      if(currEl->next == NULL)
		{
		  FatalError("Memory allocation in NovaWin_Rlist2SidList() - 2");
		}
	      
	      currEl = currEl->next;
	    }
	  
	  memcpy(currEl->sid, sidBuf, CF_MAXSIDSIZE);
	}
    }

  
  if(uidlist == NULL)  // empty uidnames or no user in it found on this system
    {
      uidlist = (struct UidList *)calloc(1, sizeof(struct UidList));

      if(uidlist == NULL)
	{
	  FatalError("Memory allocation in NovaWin_Rlist2SidList() - 3");
	}
    }

  return uidlist;
}

/*******************************************************************/

struct Rval NovaWin_FnCallUserExists(struct FnCall *fp,struct Rlist *finalargs)

{ struct Rlist *rp;
  struct Rval rval;
  char userSid[CF_MAXSIDSIZE];
  char buffer[CF_BUFSIZE];
  char *arg = finalargs->item;
 
buffer[0] = '\0';  
ArgTemplate(fp,CF_FNCALL_TYPES[cfn_userexists].args,finalargs);

/* begin fn specific content */

strcpy(buffer,CF_ANYCLASS);

 if(!NovaWin_UserNameToSid(arg, (SID *)userSid, sizeof(userSid), false))
   {
     strcpy(buffer,"!any");
   }

if ((rval.item = strdup(buffer)) == NULL)
   {
   FatalError("Memory allocation in NovaWin_FnCallUserExists");
   }

/* end fn specific content */

rval.rtype = CF_SCALAR;
return rval;
}

/*********************************************************************/

struct Rval NovaWin_FnCallGroupExists(struct FnCall *fp,struct Rlist *finalargs)

{ 
  struct Rlist *rp;
  struct Rval rval;
  char groupSid[CF_MAXSIDSIZE];
  char buffer[CF_BUFSIZE];
  char *arg = finalargs->item;
 
buffer[0] = '\0';  
ArgTemplate(fp,CF_FNCALL_TYPES[cfn_groupexists].args,finalargs);

/* begin fn specific content */

strcpy(buffer,CF_ANYCLASS);

 if (!NovaWin_GroupNameToSid(arg, (SID *)groupSid, sizeof(groupSid), false))
   {
   strcpy(buffer,"!any");
   }

if ((rval.item = strdup(buffer)) == NULL)
   {
   FatalError("Memory allocation in FnCallUserExists");
   }

/* end fn specific content */

rval.rtype = CF_SCALAR;
return rval;
}


#endif  /* MINGW */
