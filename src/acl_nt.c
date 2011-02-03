/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: acl_nt.c                                                            */
/*                                                                           */
/* Created: Fri Oct 09 11:50:33 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "../../core/src/cf3.defs.h"
#include "../../core/src/cf3.extern.h"
#include "cf.nova.h"

int Nova_CheckNtACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp)
{
#ifdef MINGW
  Nova_SetACLDefaults(file_path, &acl);

  if(!Nova_CheckNtACEs(file_path, acl.acl_entries, INHERIT_ACCESS_ONLY, acl.acl_method, a, pp))
    {
      cfPS(cf_error,CF_FAIL,"",pp,a," !! Failed checking access ACL on \"%s\"", file_path);
      PromiseRef(cf_error,pp);
      return false;
    }

  if(IsDir(file_path))
    {
      if(!Nova_CheckNtInheritACEs(file_path, acl.acl_inherit_entries, acl.acl_method, acl.acl_directory_inherit, a, pp))
        {
          cfPS(cf_error,CF_FAIL,"",pp,a," !! Failed checking inheritance ACL on \"%s\"", file_path);
          PromiseRef(cf_error,pp);
          return false;
        }
    }
#else  /* NOT MINGW */
cfPS(cf_error,CF_FAIL,"",pp,a,"!! NT ACLs are only supported on Windows");
PromiseRef(cf_error,pp);
#endif

  return true;
}


#ifdef MINGW


int Nova_CheckNtInheritACEs(char *file_path, struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, struct Attributes a, struct Promise *pp)

{ int result;

  switch(directory_inherit)
    {
   case cfacl_nochange:  // no change always succeeds

       result = true;
       break;

    case cfacl_specify:  // default ALC is specified in promise

      result = Nova_CheckNtACEs(file_path, aces, INHERIT_DEFAULT_ONLY, method, a, pp);
      break;

    case cfacl_parent: // default ACL should be the same as access ACL

      result = Nova_CheckNtDefaultEqualsAccessACL(file_path, a, pp);
      break;

    case cfacl_clear:  // default ALC should be empty

      result = Nova_CheckNtDefaultClearACL(file_path, a, pp);
      break;

    default:  // unknown inheritance policy
      CfOut(cf_error,"","!! Unknown inheritance policy - shouldn't happen");
      result = false;
      break;
    }

  return result;
}


int Nova_CheckNtACEs(char *file_path, struct Rlist *aces, inherit_t inherit, enum cf_acl_method method, struct Attributes a, struct Promise *pp)
{
  SECURITY_DESCRIPTOR *existingSecDesc;
  ACL *existingAcl;
  EXPLICIT_ACCESS *eas;
  struct Rlist *rp;
  int newAceCount = 0;
  int aclsEqual;
  int eaCount;
  int retvAcl;
  char *aclTypeStr;

  for(rp = aces; rp != NULL; rp=rp->next)
    {
      newAceCount++;
    }

  aclTypeStr = (inherit == INHERIT_ACCESS_ONLY) ? "Access" : "Default";

  retvAcl = GetNamedSecurityInfo(file_path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &existingAcl, NULL, &existingSecDesc);

  if(retvAcl != ERROR_SUCCESS)
    {
      CfOut(cf_error,"GetNamedSecurityInfo","!! Could not retreive existing ACL");
      return false;
    }

  if(existingAcl == NULL)
    {
      eaCount = 0;
    }
  else
    {
      eaCount = existingAcl->AceCount;
    }

  // allocate buffer large enough for existing and new aces
  eas = calloc(eaCount + newAceCount, sizeof(EXPLICIT_ACCESS));

  if(eas == NULL)
    {
      LocalFree(existingSecDesc);
      FatalError("Memory allocation in Nova_CheckNtACEs()");
      return false;
    }


  // convert existing aces to another format (EXPLICIT_ACCESS)
  if(!Nova_AclToExplicitAccess(eas, eaCount, existingAcl))
    {
      CfOut(cf_error,"","!! Could not convert ACL to EXPLICIT_ACCESS structures");
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

 // if we are overwriting, remove all aces of the inheritance type we are overwriting
  if(method == cfacl_overwrite)
    {
      Nova_RemoveEasByInheritance(eas, &eaCount, inherit);
    }

  if(!Nova_ParseAcl(file_path, aces, eas, &eaCount, inherit))
    {
      CfOut(cf_error,"","!! Could not parse ACL");
      Nova_FreeSids(eas, eaCount);
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

  // remove aces with empty permissions
  Nova_RemoveEmptyEas(eas, &eaCount);

  // check if the ACL has been modified (compare existing and new)
  if(!Nova_ACLEquals(&aclsEqual, eas, eaCount, existingAcl))
    {
      CfOut(cf_error,"","!! Failed while comparing new and existing ACL");
      Nova_FreeSids(eas, eaCount);
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

  if(aclsEqual)
    {
      cfPS(cf_inform,CF_NOP,"",pp,a,"-> %s ACL on \"%s\" needs no modification.", aclTypeStr, file_path);
    }
  else  // ACL needs to be changed
    {

      switch (a.transaction.action)
	{
	case cfa_warn:
          
	  cfPS(cf_error,CF_WARN,"",pp,a," !! %s ACL on \"%s\" needs to be changed", aclTypeStr, file_path);
          break;
          
	case cfa_fix:
          
          if (!DONTDO)
	    {
	      if(!Nova_SetEas(file_path, eas, eaCount))
		{
		  CfOut(cf_error,"","!! Could not set the ACL");
		  Nova_FreeSids(eas, eaCount);
		  free(eas);
		  LocalFree(existingSecDesc);
		  return false;
		}
	    }
	  cfPS(cf_inform,CF_CHG,"",pp,a,"-> %s ACL on \"%s\" successfully changed", aclTypeStr, file_path);
          break;
          
	default:
          FatalError("cfengine: internal error: illegal file action\n");
	}

    }

  Nova_FreeSids(eas, eaCount);
  free(eas);
  LocalFree(existingSecDesc);

  return true;
}


/* Removes any default aces (aces that apply to child objects) */
int Nova_CheckNtDefaultClearACL(char *file_path, struct Attributes a, struct Promise *pp)
{
  SECURITY_DESCRIPTOR *existingSecDesc;
  ACL *existingAcl;
  EXPLICIT_ACCESS *eas;
  int aclsEqual;
  int eaCount;
  int retvAcl;

  retvAcl = GetNamedSecurityInfo(file_path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &existingAcl, NULL, &existingSecDesc);

  if(retvAcl != ERROR_SUCCESS)
    {
      CfOut(cf_error,"","!! Could not retreive existing ACL");
      return false;
    }

  if(existingAcl == NULL)  // empty acl means no default aces, as desired
    {
      LocalFree(existingSecDesc);
      return true;
    }
  else
    {
      eaCount = existingAcl->AceCount;
    }

  // allocate buffer large enough for existing aces
  eas = calloc(eaCount, sizeof(EXPLICIT_ACCESS));

  if(eas == NULL)
    {
      LocalFree(existingSecDesc);
      FatalError("Memory allocation in Nova_CheckNtDefaultClearACL()");
      return false;
    }


  // convert existing aces to another format (EXPLICIT_ACCESS)
  if(!Nova_AclToExplicitAccess(eas, eaCount, existingAcl))
    {
      CfOut(cf_error,"","!! Could not convert ACL to EXPLICIT_ACCESS structures");
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

  // remove all default aces
  Nova_RemoveEasByInheritance(eas, &eaCount, INHERIT_DEFAULT_ONLY);

  // check if the ACL has been modified (compare existing and new)
  if(!Nova_ACLEquals(&aclsEqual, eas, eaCount, existingAcl))
    {
      CfOut(cf_error,"","!! Failed while comparing new and existing ACL");
      Nova_FreeSids(eas, eaCount);
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

  if(aclsEqual)
    {
      cfPS(cf_inform,CF_NOP,"",pp,a,"-> Default ACL on \"%s\" needs no modification (was already empty).", file_path);
    }
  else
    {

      switch (a.transaction.action)
	{
	case cfa_warn:
          
	  cfPS(cf_error,CF_WARN,"",pp,a," !! Default ACL on \"%s\" needs to be cleared.", file_path);
          break;
          
	case cfa_fix:
          
          if (!DONTDO)
	    {
	      if(!Nova_SetEas(file_path, eas, eaCount))
		{
		  CfOut(cf_error,"","!! Could not set the ACL");
		  Nova_FreeSids(eas, eaCount);
		  free(eas);
		  LocalFree(existingSecDesc);
		  return false;
		}
	    }

	  cfPS(cf_inform,CF_CHG,"",pp,a,"-> Default ACL on \"%s\" successfully cleared.", file_path);
          break;
          
	default:
          FatalError("cfengine: internal error: illegal file action\n");
	}

    }

  Nova_FreeSids(eas, eaCount);
  free(eas);
  LocalFree(existingSecDesc);

  return true;
}


/* Makes sure default ACL is the same as access ACL.
 * On NT, this means that all aces with INHERIT_ONLY_ACE
 * set are removed. */
int Nova_CheckNtDefaultEqualsAccessACL(char *file_path, struct Attributes a, struct Promise *pp)
{
  SECURITY_DESCRIPTOR *existingSecDesc;
  ACL *existingAcl;
  EXPLICIT_ACCESS *eas;
  int eaCount;
  int retvAcl;
  int i;
  int modified = false;

  retvAcl = GetNamedSecurityInfo(file_path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &existingAcl, NULL, &existingSecDesc);

  if(retvAcl != ERROR_SUCCESS)
    {
      CfOut(cf_error,"GetNamedSecurityInfo","!! Could not retreive existing ACL");
      return false;
    }

  if(existingAcl == NULL)  // empty acl means no access and default aces - so they equal
    {
      LocalFree(existingSecDesc);
      return true;
    }
  else
    {
      eaCount = existingAcl->AceCount;
    }

  // allocate buffer large enough for existing aces
  eas = calloc(eaCount, sizeof(EXPLICIT_ACCESS));

  if(eas == NULL)
    {
      LocalFree(existingSecDesc);
      FatalError("Memory allocation in Nova_CheckNtDefaultEqualsAccessACL()");
      return false;
    }


  // convert existing aces to another format (EXPLICIT_ACCESS)
  if(!Nova_AclToExplicitAccess(eas, eaCount, existingAcl))
    {
      CfOut(cf_error,"","!! Could not convert ACL to EXPLICIT_ACCESS structures");
      free(eas);
      LocalFree(existingSecDesc);
      return false;
    }

  // remove any aces with INHERIT_ONLY_ACE set
  for(i = 0; i < eaCount; i++)
    {
      // make sure the ace is inherited
      if(!(eas[i].grfInheritance & (CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE)))
        {
          eas[i].grfInheritance |= (CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE);
          modified = true;
        }

      if(eas[i].grfInheritance & INHERIT_ONLY_ACE)  // default-only ACE: remove it
        {
          // free this entry's SID and move last entry to its place
          Nova_FreeSids(&eas[i], 1);

          memset(&eas[i], 0, sizeof(EXPLICIT_ACCESS));
          memmove(&eas[i], &eas[eaCount - 1], sizeof(EXPLICIT_ACCESS));

          eaCount -= 1;
          i--;  // new entry here, so "recheck" it

          modified = true;
        }
    }

  if(!modified)
    {
      cfPS(cf_inform,CF_NOP,"",pp,a,"-> Default ACL on \"%s\" needs no modification (was already the same as access ACL).", file_path);
    }
  else
    {

      switch (a.transaction.action)
	{
	case cfa_warn:
          
	  cfPS(cf_error,CF_WARN,"",pp,a," !! Default ACL on \"%s\" needs to be copied from access ACL.", file_path);
          break;
          
	case cfa_fix:
          
          if (!DONTDO)
	    {
	      if(!Nova_SetEas(file_path, eas, eaCount))
		{
		  CfOut(cf_error,"","!! Could not set the ACL");
		  Nova_FreeSids(eas, eaCount);
		  free(eas);
		  LocalFree(existingSecDesc);
		  return false;
		}
	    }
	  cfPS(cf_inform,CF_CHG,"",pp,a,"-> Default ACL on \"%s\" successfully copied from access ACL.", file_path);          
          break;
          
	default:
          FatalError("cfengine: internal error: illegal file action\n");
	}

    }


  Nova_FreeSids(eas, eaCount);
  free(eas);
  LocalFree(existingSecDesc);

  return true;
}


/* Find any structures with inheritance type inherit, and remove them or make
 * them apply only to the other inheritance type (access or default) */
void Nova_RemoveEasByInheritance(EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit)
{
  int i;

  if(inherit == INHERIT_ACCESS_ONLY)  // remove all access eas
    {

      for(i = 0; i < *eaCount; i++)
        {
          if((eas[i].grfInheritance & INHERIT_ONLY_ACE) == 0)  // INHERIT_ONLY_ACE unset: ace applies to access
            {
              if((eas[i].grfInheritance & ~INHERITED_ACE) == 0)  // ace applies ONLY to access, remove it
                {
                  // free this entry's SID and move last entry to its place
                  Nova_FreeSids(&eas[i], 1);

                  memset(&eas[i], 0, sizeof(EXPLICIT_ACCESS));
                  memmove(&eas[i], &eas[*eaCount - 1], sizeof(EXPLICIT_ACCESS));

                  *eaCount -= 1;
                  i--;  // new entry here, so "recheck" it
                }
              else  // ace applies to access and default, make it default-only
                {
                  eas[i].grfInheritance |= INHERIT_ONLY_ACE;
                }

            }
        }
    }

  else  // remove all default eas
    {
      for(i = 0; i < *eaCount; i++)
        {

          if((eas[i].grfInheritance & (OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE)) != 0)  // ace is inherited
            {
              if((eas[i].grfInheritance & INHERIT_ONLY_ACE) != 0)  // ace applies only to default, remove it
                {
                  // free this entry's SID and move last entry to its place
                  Nova_FreeSids(&eas[i], 1);

                  memset(&eas[i], 0, sizeof(EXPLICIT_ACCESS));
                  memmove(&eas[i], &eas[*eaCount - 1], sizeof(EXPLICIT_ACCESS));

                  *eaCount -= 1;
                  i--;  // new entry here, so "recheck" it
                }
              else  // ace applies to default and access, make it not apply to default
                {
                  eas[i].grfInheritance = eas[i].grfInheritance & ~(OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE);
                }
            }
        }
    }
}


/* Find any ea structures with unset permission, and remove them
 * by moving the last entry to their place and decreasing the count. */
void Nova_RemoveEmptyEas(EXPLICIT_ACCESS *eas, int *eaCount)
{
  int i;

  for(i = 0; i < *eaCount; i++)
    {
      // remove entries with same or less bits set than minimum
      if(!(eas[i].grfAccessPermissions & (~CF_MINIMUM_PERMS_NT)))
        {
          // free this entry's SID and move last entry to its place
          Nova_FreeSids(&eas[i], 1);

          memset(&eas[i], 0, sizeof(EXPLICIT_ACCESS));
          memmove(&eas[i], &eas[*eaCount - 1], sizeof(EXPLICIT_ACCESS));

          *eaCount -= 1;
          i--;  // new entry here, so "recheck" it
        }
    }
}


/* Compares the eaCount number of structures in firstEas against the entries found
 * in  acl. If there is a one-to-one correspondance (permutations allowed),
 * aclEqual is set to true, or false otherwise. Returns true on success,
 * or false on error. */
int Nova_ACLEquals(int *aclsEqual, EXPLICIT_ACCESS *firstEas, int eaCount, ACL *acl)
{
  EXPLICIT_ACCESS secondEas[eaCount];
  int i, j;

  // check if the structures contain equally many aces
  if(acl == NULL)
    {
      if(eaCount == 0)
        {
          *aclsEqual = true;
          return true;
        }
      else
        {
          *aclsEqual = false;
          return true;
        }
    }

  if(eaCount != acl->AceCount)
    {
      *aclsEqual = false;
      return true;
    }

  // convert the alc into a EA-struct

  if(!Nova_AclToExplicitAccess(secondEas, eaCount, acl))
    {
      CfOut(cf_error,"","!! Could not convert ACL to EXPLICIT_ACCESS structures");
      return false;
    }

  // need nested loop because ordering of aces may differ
  for(i = 0; i < eaCount; i++)
    {
      for(j = 0;; j++)
        {
          if(j == eaCount)  // ace not found
            {
              Nova_FreeSids(secondEas, eaCount);
              *aclsEqual = false;
              return true;
            }

          // check if all fields match
          if(firstEas[i].grfAccessPermissions == secondEas[j].grfAccessPermissions &&
             firstEas[i].grfAccessMode == secondEas[j].grfAccessMode &&
             firstEas[i].grfInheritance == secondEas[j].grfInheritance &&
             EqualSid(firstEas[i].Trustee.ptstrName,secondEas[j].Trustee.ptstrName))
            {
              break;  // go to next i
            }
        }

    }


  Nova_FreeSids(secondEas, eaCount);

  *aclsEqual = true;
  return true;
}


int Nova_AclToExplicitAccess(EXPLICIT_ACCESS *eas, int eaCount, ACL *acl)
{
  ACCESS_ALLOWED_ACE *currAce;
  SID *sidAlloc;
  int i;

  for(i = 0; i < eaCount; i++)
    {
      if(!GetAce(acl, i, (void **)&currAce))
        {
          CfOut(cf_error,"GetAce","!! Could not get ACE");
          Nova_FreeSids(eas, i);
          return false;
        }

      eas[i].grfAccessPermissions = currAce->Mask;

      switch(currAce->Header.AceType)
        {
        case ACCESS_ALLOWED_ACE_TYPE:
          eas[i].grfAccessMode = GRANT_ACCESS;
          break;

        case ACCESS_DENIED_ACE_TYPE:
          eas[i].grfAccessMode = DENY_ACCESS;
          break;

        default:
          CfOut(cf_error,"","!! Unknown ace type: %d", currAce->Header.AceType);
          return false;
        }

      // NOTE: the ea struct becomes invalid if it has the INHERITED_ACE bit
      eas[i].grfInheritance = (currAce->Header.AceFlags & ~INHERITED_ACE);

      eas[i].Trustee.TrusteeForm = TRUSTEE_IS_SID;

      // copy SID
      sidAlloc = (SID *)calloc(1, GetLengthSid((SID *)&(currAce->SidStart)));

      if(sidAlloc == NULL)
      {
        Nova_FreeSids(eas, i);
        FatalError("Memory allocation in Nova_AclToExplicitAccess()");
        return false;
      }

      memcpy(sidAlloc, &(currAce->SidStart), GetLengthSid((SID *)&(currAce->SidStart)));

      eas[i].Trustee.ptstrName = (void *)sidAlloc;  // freed in Nova_FreeSids()
    }

  return true;
}


/* assmes large enough buffer is allocated in eas. eaCount elements are filled from existing ACL
 * when called, and contains the number of elemements when returning successfully */
int Nova_ParseAcl(char *file_path, struct Rlist *aces, EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit)
{
  EXPLICIT_ACCESS *currEa, *nextFreeEa, *firstUsedEa, *splitOldEa;
  int numNewEas = 0;
  char sidBuf[CF_BUFSIZE];
  SID *sid = (SID*)sidBuf;
  struct Rlist *rp;
  SID *sidAlloc;
  char *currAce;
  ACCESS_MASK newPerms = CF_MINIMUM_PERMS_NT;

  if((inherit != INHERIT_ACCESS_ONLY) && (inherit != INHERIT_DEFAULT_ONLY))
    {
      CfOut(cf_error,"","!! Inheritance policy is not access or default - shouldn't happen");
      return false;
    }

  firstUsedEa = &eas[*eaCount];
  nextFreeEa = firstUsedEa;

  for(rp = aces; rp != NULL; rp=rp->next)
    {
      currAce = (char *)rp->item;

      splitOldEa = NULL;

      // parse "entity-type:id" into SID
      if(!Nova_EntityToSid(&currAce, sid, sizeof(sidBuf)))
        {
          CfOut(cf_error,"","!! Could not obtain the security identifier corresponding to the entity in \"%s\"", (char *)rp->item);
          Nova_FreeSids(firstUsedEa, numNewEas);
          return false;
        }

      // check if this sid and perm-type is present in any existing aces
      currEa = Nova_FindAceNt(eas, nextFreeEa, sid, Nova_ParsePermTypeNt((char *)rp->item), inherit);  // TODO: Pointer from integer ?
      if(currEa == NULL)
        {
          // check if an ace which applies both to default and access exists, and split it into two if so
          currEa = Nova_FindAceNt(eas, nextFreeEa, sid, Nova_ParsePermTypeNt((char *)rp->item), INHERIT_ACCESS_AND_DEFAULT);  // TODO: Pointer from integer ?
          if(currEa != NULL)
            {
              // we leave the existing ace unmodified for now, but may dedicate it fully to one
              // inheritance type later, if the new ace we create differ from the existing
              splitOldEa = currEa;

              newPerms = currEa->grfAccessPermissions;  // make our new ACE start with same permission bits
              currEa = NULL;  // allocate a new ACE (below)
            }
        }

      if(currEa == NULL)  // allocate new ACE
        {
          currEa = nextFreeEa;

          memset(currEa, 0, sizeof(EXPLICIT_ACCESS));

          // newPerms is CF_MINIMUM_PERMS_NT or permissions from any corresponding splitted ACE (from above)
          currEa->grfAccessPermissions = newPerms;

          currEa->Trustee.TrusteeForm = TRUSTEE_IS_SID;
          sidAlloc = (SID *)calloc(1, GetLengthSid(sid));

          if(sidAlloc == NULL)
            {
              Nova_FreeSids(firstUsedEa, numNewEas);
              FatalError("Memory allocation in Nova_ParseAcl()");
              return false;
            }

          memcpy(sidAlloc, sid, GetLengthSid(sid));
          currEa->Trustee.ptstrName = (void *)sidAlloc;  // freed in Nova_FreeSids()

          if(inherit == INHERIT_ACCESS_ONLY)
            {
              currEa->grfInheritance = 0;  // no inheritance
            }
          else  // inherit == INHERIT_DEFAULT_ONLY
            {
              currEa->grfInheritance = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
            }

          newPerms = CF_MINIMUM_PERMS_NT;
          nextFreeEa++;
          numNewEas++;
        }

      // move past ':', to the mode-string
      currAce++;

      // apply mode-string on permission bits
      if(!Nova_ParseModeNt(&currAce, &(currEa->grfAccessPermissions)))
        {
          CfOut(cf_error,"","!! Could not parse mode-string in ace \"%s\"", (char *)rp->item);
          Nova_FreeSids(firstUsedEa, numNewEas);
          return false;
        }

      // last field is the optional perm_type ("allow" or "deny")
      switch(*currAce)
        {
        case '\0':  // no perm_type specified, use default (allow)
          currEa->grfAccessMode = GRANT_ACCESS;
          break;

        case ':':
          currEa->grfAccessMode = Nova_ParsePermTypeNt((char *)rp->item);
          break;

        default:
          CfOut(cf_error,"","!! Element following mode-string in ace \"%s\" is not ':' or NULL-terminator", (char *)rp->item);
          Nova_FreeSids(firstUsedEa, numNewEas);
          return false;
        }

      // If we have split an ace and their permission bits differ, we must dedicate the existing ace to one
      // inheritance type, while the new ace is dedicated to the other.
      // If they do not differ, we just remove the new one, and let the old represent them both.
      if(splitOldEa != NULL)
        {
          if(currEa->grfAccessPermissions == splitOldEa->grfAccessPermissions)
            {
              // remove new ace, and set existing to apply to both access and default
              Nova_FreeSids(currEa, 1);
              nextFreeEa--;
              numNewEas--;

              splitOldEa->grfInheritance &= (~INHERIT_ONLY_ACE);
              splitOldEa->grfInheritance |= (OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE);
            }
          else
            {
              if(inherit == INHERIT_ACCESS_ONLY)
                {
                  splitOldEa->grfInheritance |= INHERIT_ONLY_ACE;  // make existing ACE default only
                }
              else
                {
                  splitOldEa->grfInheritance = 0;  // make existing ACE access only
                }

            }
        }
    }

  // update the number of used ea structs for the caller
  *eaCount = *eaCount + numNewEas;

  return true;
}


/* Sets aces on the given file.
 * Returns true on succes, false otherwise. */
int Nova_SetEas(char *file_path, EXPLICIT_ACCESS *eas, int eaCount)
{
  ACL *newAcl;
  DWORD setRes, writeRes;

  setRes = SetEntriesInAcl(eaCount, eas, NULL, &newAcl);
  if(setRes != ERROR_SUCCESS)
    {
      CfOut(cf_error,"SetEntriesInAcl","!! Could not merge ACEs into an ACL (errcode=%lu)", setRes);
      return false;
    }


  // NOTE: PROTECTED_DACL_SECURITY_INFORMATION = no automatic inheritance of ACEs from parent, exclude to inherit
  writeRes = SetNamedSecurityInfo(file_path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, newAcl, NULL);

  if(writeRes != ERROR_SUCCESS)
    {
      CfOut(cf_error,"SetNamedSecurityInfo","!! Could not set new ACL");
      return false;
    }

  LocalFree(newAcl);

  return true;
}


/* Searches through the array eas of ACEs before endEa for a Trustee
 * matching findSid and access mode findPermt. findInherit decides if we want to match
 * only ACCESS (explicit) aces, only DEFAULT (inherited) aces or both.
 * Returns a pointer to that ACE if fount, NULL otherwise
*/
EXPLICIT_ACCESS *Nova_FindAceNt(EXPLICIT_ACCESS *eas, EXPLICIT_ACCESS *endEa, SID *findSid, ACCESS_MODE findPermt, inherit_t findInherit)
{
  EXPLICIT_ACCESS *currEa = eas;
  DWORD inheritMask;
  int isSet;

  switch(findInherit)
    {
    case INHERIT_ACCESS_ONLY:
      // none of INHERIT_ONLY_ACE, OBJECT_INHERIT_ACE, or CONTAINER_INHERIT_ACE must be set
      inheritMask = INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
      isSet = false;
      break;

    case INHERIT_DEFAULT_ONLY:
      inheritMask = INHERIT_ONLY_ACE;
      isSet = true;
      break;

    case INHERIT_ACCESS_AND_DEFAULT:
      // fallthrough
    default:  // match all bits
      inheritMask = 0;
      isSet = false;
      break;
    }

  while(currEa != endEa)
    {
      if(EqualSid((SID *)currEa->Trustee.ptstrName, findSid) &&
         (currEa->grfAccessMode == findPermt))
        {
          if((isSet && (currEa->grfInheritance & inheritMask)) ||
             (!isSet && !(currEa->grfInheritance & inheritMask)))
            {
              return currEa;
            }
        }
      currEa++;
    }
  return NULL;  // not found
}


/* Frees all memory allocated in the "ptstrName" element of
 * up to "num" structures  */
void Nova_FreeSids(EXPLICIT_ACCESS *eas, int num)
{
  int i;

  for(i = 0; i < num; i++)
    {
      free(eas[i].Trustee.ptstrName);
    }
}


/* Converts the first two elements of the Cfengine ace "entity-type:name" into
 * a security identifier. Moves the char pointer to the next element.
 * Returns true on success, false otherwise                                    */
int Nova_EntityToSid(char **acePtr, SID *sid, DWORD sidSz)
{
  char nameBuf[CF_BUFSIZE];
  int i = 0;

  memset(nameBuf, 0, sizeof(nameBuf));

  // find entity-type
  if(strncmp(*acePtr, "user:", 5) == 0)
    {
      *acePtr += 5;

      while(**acePtr != ':' && **acePtr != '\0')
        {
          nameBuf[i++] = **acePtr;
          *acePtr += 1;
        }

      if(!NovaWin_UserNameToSid(nameBuf, sid, sidSz, true))
        {
          return false;
        }

    }
  else if(strncmp(*acePtr, "group:", 6) == 0)
    {
      *acePtr += 6;

      while(**acePtr != ':' && **acePtr != '\0')
        {
          nameBuf[i++] = **acePtr;
          *acePtr += 1;
        }

      if(!NovaWin_GroupNameToSid(nameBuf, sid, sidSz, true))
        {
          return false;
        }

    }
  else if(strncmp(*acePtr, "all:", 4) == 0)
    {
      *acePtr += 3;

      // "S-1-1-0" is (always) the "Everyone" group
      if(!NovaWin_StringToSid("S-1-1-0", sid, sidSz))
        {
          CfOut(cf_error,"","!! Could not find the security identifier of group \"Everyone\" (assumed to be \"S-1-1-0\") - shouldn't happen");
          return false;
        }

    }
  else
    {
      CfOut(cf_error,"","!! Ace does not start with user:/group:/all:/mask:");
      return false;
    }


  return true;
}


/* Takes a Cfengine-syntax mode-string and applies them on the
 * NT formatted perms. The mode is applied on the perms.
 * Moves the mode pointer past the mode-string.
 * Returns true on success, false otherwise.  */
int Nova_ParseModeNt(char **modePtr, ACCESS_MASK *perms)
{
  int moreEntries;
  ACCESS_MASK currPerms;
  enum {add, del} op;
  char *mode = *modePtr;

  op = add;

  if (*mode == '\0' || *mode == ':')
    {
      *perms = CF_MINIMUM_PERMS_NT;  // empty mode-string: clear all perms
      return true;
    }

  moreEntries = true;

  while (moreEntries)
    {

      // parse operation (+,-,= or empty)
      switch(*mode)
        {
        case '+':
          op = add;
          mode++;
          break;

        case '-':
          op = del;
          mode++;
          break;

        case '=':
          mode++;
          // fallthrough

        default:
          // if mode does not start with + or -, we clear existing perms
          op = add;
          *perms = 0;
        }

      // parse generic perms
      while(IsIn(*mode,CF_VALID_GPERMS))
        {
          switch(*mode)
            {
            case 'r':
              currPerms = CF_GENERIC_READ_NT;
              break;

            case 'w':
              currPerms = CF_GENERIC_WRITE_NT;
              break;

            case 'x':
              currPerms = CF_GENERIC_EXECUTE_NT;
              break;

            default:
              CfOut(cf_error,"","!! No NT support for generic permission '%c'",*mode);
              return false;
            }

          if(op == add)
            {
              *perms |= currPerms;
            }
          else
            {
              *perms &= ~currPerms;
            }

          mode++;
        }

      // parse any native perms
      if (*mode == CF_NATIVE_PERMS_SEP_START)
        {
          mode++;

          while(IsIn(*mode,CF_VALID_NPERMS_NTFS))
            {
              switch(*mode)
                {
                case 'd':
                  currPerms = DELETE;
                  break;

                case 'r':
                  currPerms = FILE_READ_DATA;
                  break;

                case 't':
                  currPerms = FILE_READ_ATTRIBUTES;
                  break;

                case 'x':
                  currPerms = FILE_EXECUTE;
                  break;

                case 'T':
                  currPerms = FILE_READ_EA;
                  break;

                case 'w':
                  currPerms = FILE_WRITE_DATA;
                  break;

                case 'a':
                  currPerms = FILE_APPEND_DATA;
                  break;

                case 'b':
                  currPerms = FILE_WRITE_ATTRIBUTES;
                  break;

                case 'B':
                  currPerms = FILE_WRITE_EA;
                  break;

                case 'p':
                  currPerms = READ_CONTROL;
                  break;

                case 'c':
                  currPerms = WRITE_DAC;
                  break;

                case 'o':
                  currPerms = WRITE_OWNER;
                  break;

                case 'D':
                  currPerms = FILE_DELETE_CHILD;
                  break;

                default:
                  CfOut(cf_error,"","!! No NT support for native permission flag '%c'",*mode);
                  return false;
                }

              if(op == add)
                {
                  *perms |= currPerms;
                }
              else
                {
                  *perms &= ~currPerms;
                }

              mode++;
            }

          // scan past native perms end seperator
          mode++;
        }
      if (*mode == ',')
        {
          moreEntries = true;
          mode++;
        }
      else
        {
          moreEntries = false;
        }
    }

  *perms |= CF_MINIMUM_PERMS_NT;

  // move argument pointer past mode-string
  *modePtr = mode;

  return true;
}

/* Checks if the last element in the ace is ":deny",
 * and returns the NT deny perm type if so. The default,
 * allow, is returned otherwise.
*/
ACCESS_MODE Nova_ParsePermTypeNt(char *ace)
{
  char *prevCol = ace;

  while(*ace != '\0')
    {
      if(*ace == ':')
        {
          prevCol = ace;
        }
      ace++;
    }

  if(strncmp(prevCol+1, "deny", 4) == 0)
    {
      return DENY_ACCESS;
    }
  else
    {
      return GRANT_ACCESS;
    }
}

#endif  /* MINGW */
