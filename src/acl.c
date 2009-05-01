
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: acl.c                                                               */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_VerifyACL(char *file,struct Attributes a, struct Promise *pp)

{
if (!Nova_CheckACLSyntax(a.acl,pp))
   {
   cfPS(cf_error,CF_INTERPT,"",pp,a," !! Syntax error in access control list for %s",file);
   PromiseRef(cf_error,pp);
   return;
   }

switch(a.acl.acl_type)
   {
   case cfacl_posix:
       switch (VSYSTEMHARDCLASS)
          {
          case linuxx:
              Nova_CheckPosixLinuxACL(file,a.acl);
              break;

          default:
              CfOut(cf_inform,"","Posix ACLs not available on this system");
              break;
           }
       break;

   case cfacl_ntfs:
       CfOut(cf_inform,"","NTFS ACLs not yet implemented");
       break;

   default:
       CfOut(cf_error,"","Unknown ACL type - software error");
       break;
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_CheckACLSyntax(struct CfACL acl, struct Promise *pp)

{ int valid = true;
  int deny_support;
  char *valid_ops = NULL;
  char *valid_nperms = NULL;
  struct Rlist *rp;    

// set unset fields to defautls
Nova_SetACLDefaults(&acl);

// find valid values for op

switch(acl.acl_method)
   {
   case cfacl_overwrite:
       valid_ops = CF_VALID_OPS_METHOD_OVERWRITE;
       break;
       
   case cfacl_append:
       valid_ops = CF_VALID_OPS_METHOD_APPEND;
       break;
       
   default:
       // never executed: should be set to a default value by now
       break;
   }

switch(acl.acl_type)
   {
   case cfacl_generic:  // generic ACL type: cannot include native or deny-type permissions
       valid_nperms = "";
       deny_support = false;       
       break;

   case cfacl_posix:
       valid_nperms = CF_VALID_NPERMS_POSIX;
       deny_support = false;  // posix does not support deny-type permissions
       break;
       
   case cfacl_ntfs:
       //valid_nperms = CF_VALID_NPERMS_NTFS;
       // deny_support = true;
       break;

   default:
     // never executed: should be set to a default value by now
     break;
   }

// check that acl_directory_inherit is set to a valid value

if (!Nova_CheckDirectoryInherit(&acl, pp))
   {
   return false;
   }

for (rp = acl.acl_entries; rp != NULL; rp=rp->next)
   {
   valid = Nova_CheckACESyntax(rp->item,valid_ops,valid_nperms,deny_support,pp);
   
   if (!valid)  // wrong syntax in this ace
      {
      CfOut(cf_error,"","The ACE \"%s\" contains errors",rp->item);
      PromiseRef(cf_error,pp);
      break;
      }
   }

for (rp = acl.acl_inherit_entries; rp != NULL; rp=rp->next)
   {
   valid = Nova_CheckACESyntax(rp->item,valid_ops,valid_nperms,deny_support,pp);
   
   if (!valid)  // wrong syntax in this ace
      {
      CfOut(cf_error,"","The ACE \"%s\" contains errors",rp->item);
      PromiseRef(cf_error,pp);
      break;
      }
   }

return valid;
}

/*****************************************************************************/

/**
 * Set unset fields with documented defaults, to these defaults.
 **/

void Nova_SetACLDefaults(struct CfACL *acl)

{
// default: acl_method => overwrite

if (acl->acl_method == cfacl_nomethod)
   {
   acl->acl_method = cfacl_overwrite;
   }
 
// default: acl_type => generic

if (acl->acl_type == cfacl_notype)
   {
   acl->acl_type = cfacl_generic;
   }
 

// default: acl_directory_inherit => parent

if(acl->acl_directory_inherit == cfacl_noinherit)
   {
   acl->acl_directory_inherit = cfacl_parent;
   }
}

/*****************************************************************************/

int Nova_CheckDirectoryInherit(struct CfACL *acl, struct Promise *pp)

/*
  Checks that acl_directory_inherit is set to a valid value for this acl type.
  Returns true if so, or false otherwise.
*/

{int i;
 enum cf_acl_type acltypes_specify[] = { cfacl_posix };

// acl_directory_inherit => specify is not supported by all ACL APIs
 
if (acl->acl_directory_inherit == cfacl_specify)
   {
   for(i = 0; i < sizeof(acltypes_specify) / sizeof(enum cf_acl_type); i++)
      {
      if (acl->acl_type == acltypes_specify[i])
         {
         return true;
         }
      }
   
   CfOut(cf_error,"","This ACL type does not support acl_directory_inherit => specify.");
   PromiseRef(cf_error,pp);
   return false;
   }
else  // the other values than "specify" are supported by all acl types
   {
   return true;
   }
}

/*****************************************************************************/

int Nova_CheckACESyntax(char *ace,char *valid_ops,char *valid_nperms,int deny_support,struct Promise *pp)

{ char *str;
  int chkid;
  int valid_mode;
  int valid_permt;

str = ace;
chkid = false;

// first element must be "user", "group" or "all"

if (strncmp(str, "user:", 5) == 0)
   {
   str += 5;
   chkid = true;
   }
else if(strncmp(str, "group:", 6) == 0)
   {
   str += 6;
   chkid = true;
   }  
else if(strncmp(str, "all:", 4) == 0)
   {
   str += 4;
   chkid = false;
   }
else
   {
   CfOut(cf_error,"","ace does not start with user:/group:/all");
   PromiseRef(cf_error,pp);
   return false;
   }

if (chkid)  // look for following "id:" TODO: Accept only alphanumeric?
   {
   if(*str == ':')
      {
      CfOut(cf_error,"","id cannot be empty or contain ':'");
      return false;
      }
   
   // skip id-string (no check: allow any id-string)

   while (true)
      {
      str++;
      if (*str == ':')
         {
         str++;
         break;
         }
      else if(*str == '\0')
         {
         // TODO: Error: Log: nothing following id string
         return -1;
         }
      }
   }

// check the mode-string (also skips to next field)

valid_mode = Nova_CheckModeSyntax(&str, valid_ops, valid_nperms,pp);

if (valid_mode)
   {
   CfOut(cf_error,"","Malformed mode-string");
   PromiseRef(cf_error,pp);
   return false;
   }

if (*str == '\0')  // mode was the last field
   {
   return true;
   }

str++;

// last field; must be a perm_type field
valid_permt = Nova_CheckPermTypeSyntax(str,deny_support,pp);

if (valid_permt != 0)
   {
   CfOut(cf_error,"","Malformed perm_type syntax");
   return false;
   }

return true;
}

/*****************************************************************************/

int Nova_CheckModeSyntax(char **mode_p, char *valid_ops, char *valid_nperms,struct Promise *pp)

/*
  Checks the syntax of a ':' or NULL terminated mode string.
  Moves the string pointer to the character following the mode
  (i.e. ':' or '\0')
*/

{ char *mode;
  int valid;
  
valid = false;
mode = *mode_p;

// mode is allowed to be empty

if (*mode == '\0' || *mode == ':')
   {
   return true;
   }

while(true)
   {
   if (IsIn(*mode,valid_ops) == 0)
      {
      mode++;
      }
   
   // scan past any generic perms
   while (IsIn(*mode,CF_VALID_GPERMS) == 0)
      {
      mode++;
      }
   
   if (*mode == CF_NATIVE_PERMS_SEP_START)
      {
      mode++;
      
      while (IsIn(*mode, valid_nperms) == 0)
         {
         mode++;
         }
      
      if (*mode == CF_NATIVE_PERMS_SEP_END)
         {
         mode++;
         }
      else
         {
         CfOut(cf_error,"","Invalid native permission '*mode', or missing native end separator");
         PromiseRef(cf_error,pp);
         valid = false;
         break;
         }
      }
   
   if (*mode == '\0' || *mode == ':')  // end of mode-string
      {
      valid = true;
      break;
      }
   else if(*mode == ',')  // one more iteration
      {
      mode++;
      }
   else
      {
      CfOut(cf_error,"","Mode string contains invalid characters");
      PromiseRef(cf_error,pp);
      valid = false;
      break;
      }  
   }

*mode_p = mode;  // move pointer to past mode-field

return valid;
}

/*****************************************************************************/

int Nova_CheckPermTypeSyntax(char *permt, int deny_support,struct Promise *pp)

/*
  Checks if the given string corresponds to the perm_type syntax.
  Only "allow" or "deny", followed by NULL-termination are valid.
  In addition, "deny" is only valid for ACL types supporting it.
 */

{ int valid;

valid = false;

if (strcmp(permt,"allow") == 0)
   {
   valid = true;
   }
else if(strcmp(permt,"deny") == 0)
   {
   if (deny_support)
      {
      valid = true;
      }
   else
      {
      CfOut(cf_error,"","Deny permission not supported by this ACL type");
      PromiseRef(cf_error,pp);
      }
   }

return valid;
}

