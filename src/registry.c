
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: registry.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

/*****************************************************************************/

#define MAX_KEY_LENGTH 255
#define REGDTYPE_UNKNOWN 1239999  // must not match any windows REG_* defines
#define MAX_VALUE_SIZE 16384

/*****************************************************************************/

void Nova_VerifyRegistryPromise(struct Attributes a,struct Promise *pp);
int Nova_OpenRegistryKey(char *key, HKEY *key_h, int create);
int Nova_PrintAllValues(HKEY key_h);
int Nova_VerifyRegistryValueAssocs(HKEY key_h,struct Attributes a,struct Promise *pp);
int Nova_GetRegistryValue(HKEY key_h, char *name, void *data_p, unsigned long *data_sz);
DWORD Str2RegDtype(char *datatypeStr);
void Nova_RecursiveQueryKey(CF_DB *dbp,HKEY *key_h,char *name,struct Attributes a,struct Promise *pp, int level);
int Nova_RegistryKeyIntegrity(CF_DB *dbp,char *key,struct Attributes a,struct Promise *pp);
void Nova_RegistryValueIntegrity(CF_DB *dbp,char *key,char *value,char *data,int size,int type,struct Attributes a,struct Promise *pp);
int Nova_ReadCmpPseudoRegistry(CF_DB *dbp,char *dbkey,void *ptr,int size,int *cmp);
HKEY Str2HKey(char *root_key);
void Nova_RecursiveRestoreKey(CF_DB *dbp,char *keyname,struct Attributes a,struct Promise *pp);
int Nova_CopyRegistryValue(char *key,char *value,char *buffer);
void Nova_DeleteRegistryKey(struct Attributes a,struct Promise *pp);
static bool Nova_CompareRegistryValue(HKEY key_h, DWORD dataType, char *name, char *valueStr, bool *outCmp);


/*****************************************************************************/

void Nova_VerifyRegistryPromise(struct Attributes a,struct Promise *pp)
 
{ HKEY key_h;  // a registry key handle
  char name[CF_MAXVARSIZE];
  int rr,rw,create = false;
  CF_DB*dbp;
  struct CfLock thislock;
  char lockname[CF_BUFSIZE];

if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The license has expired");
   return;
   }

snprintf(lockname,CF_BUFSIZE-1,"db-%s",pp->promiser);
 
thislock = AcquireLock(lockname,VUQNAME,CFSTARTTIME,a,pp,false);

if (thislock.lock == NULL)
   {
   return;
   }

if (a.database.operation && (cf_strcmp(a.database.operation,"drop") == 0 || cf_strcmp(a.database.operation,"delete") == 0))
   {
   Nova_DeleteRegistryKey(a,pp);
   YieldCurrentLock(thislock);
   return;
   }

else if (a.database.operation && cf_strcmp(a.database.operation,"create") == 0)
   { 
   if (Nova_OpenRegistryKey(pp->promiser,&key_h,false))
      {
      CfOut(cf_verbose,""," -> Registry key exists");
      }
   else if (!Nova_OpenRegistryKey(pp->promiser,&key_h,true))
      {
      cfPS(cf_error,CF_FAIL,"",pp,a," !! Registry key could not be created!");
      YieldCurrentLock(thislock);
      return;
      }
   }

if (a.database.operation && cf_strcmp(a.database.operation,"restore") == 0)
   {
   create = true;
   }

if (Nova_OpenRegistryKey(pp->promiser,&key_h,create))
   {
   CfOut(cf_verbose,""," -> Registry key \"%s\" opened...\n",pp->promiser);   
   
   snprintf(name,CF_MAXVARSIZE-1,"%s/%s",CFWORKDIR,NOVA_REGISTRY);
   MapName(name);
   
   if (!OpenDB(name,&dbp))
      {
      YieldCurrentLock(thislock);
      return;
      }
   
   if (a.database.operation && cf_strcmp(a.database.operation,"restore") == 0)
      {
      Nova_RecursiveRestoreKey(dbp,pp->promiser,a,pp) ;
      YieldCurrentLock(thislock);
      CloseDB(dbp);
      return;
      }
   
   if (a.database.rows)
      {
      Nova_VerifyRegistryValueAssocs(key_h,a,pp);
      }      
   else if (a.database.operation && (cf_strcmp(a.database.operation,"verify") == 0)
            || (cf_strcmp(a.database.operation,"cache") == 0))
      {
      CfOut(cf_verbose,"","Recursive cache of registry from here...\n");
      Nova_RecursiveQueryKey(dbp,&key_h,pp->promiser,a,pp,0) ;
      }

   CloseDB(dbp);
   }
else
   {
   cfPS(cf_error,CF_FAIL,"",pp,a," !! Registry key \"%s\" failed to open\n",pp->promiser);
   }

RegCloseKey(key_h);
YieldCurrentLock(thislock);
}

/*****************************************************************************/

int Nova_CopyRegistryValue(char *key,char *value,char *buffer)

{
 char reg_data_p[CF_BUFSIZE] = {0};
 unsigned long reg_data_sz = sizeof(reg_data_p);
 int len;
 HKEY key_h;

buffer[0] = '\0';

if (Nova_OpenRegistryKey(key,&key_h,false))
   {
   if (Nova_GetRegistryValue(key_h,value,reg_data_p,&reg_data_sz))
      {
      if (reg_data_sz > CF_BUFSIZE-1)
         {
         CfOut(cf_error,"","Registry value too large to be sensibly maniupulated");
         return false;
         }
      else
         {
         memcpy(buffer,reg_data_p,reg_data_sz);
         }
      
      return true;      
      }
   }

return false;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_RecursiveQueryKey(CF_DB*dbp,HKEY *key_h,char *keyname,struct Attributes a,struct Promise *pp, int level) 

/*  key_h,                   // key handle 
    classname,               // buffer for class name 
    &cchClassName,           // size of class string 
    NULL,                    // reserved 
    &subkeys,                // number of subkeys 
    &cbMaxSubKey,            // longest subkey size 
    &cchMaxClass,            // longest class string 
    &no_vals,                // number of values for this key 
    &cchMaxValue,            // longest value name 
    &cbMaxValueData,         // longest value data 
    &cbSecurityDescriptor,   // security descriptor 
    &ftLastWriteTime         // last write time 
*/

{ TCHAR    sub_key_name[MAX_KEY_LENGTH];  // buffer for subkey name
  DWORD    key_len;                        // size of name string 
  TCHAR    classname[MAX_PATH] = TEXT(""); // buffer for class name 
  DWORD    cchClassName = MAX_PATH;       // size of class string 
  DWORD    subkeys=0;                     // number of subkeys 
  DWORD    cbMaxSubKey;                   // longest subkey size 
  DWORD    cchMaxClass;                   // longest class string 
  DWORD    val_num;                       // number of values for key 
  DWORD    cchMaxValue;                   // longest value name 
  DWORD    cbMaxValueData;                // longest value data 
  DWORD    cbSecurityDescriptor;          // size of security descriptor 
  FILETIME ftLastWriteTime;               // last write time 
  TCHAR    value[CF_MAXVARSIZE]; 
  DWORD    valsize = CF_MAXVARSIZE; 
  TCHAR    data[CF_MAXVARSIZE]; 
  DWORD    datasize = CF_MAXVARSIZE; 
  DWORD    type;
  int i,ret,changes = 0;
  char root_key[CF_MAXVARSIZE],sub_key[CF_MAXVARSIZE];
  char *sp;
  HKEY sub_key_h;
  char subkeyname[CF_BUFSIZE];

/* Stat the key/dir */

if (RegQueryInfoKey(*key_h,classname,&cchClassName,NULL,&subkeys,&cbMaxSubKey,&cchMaxClass,&val_num,&cchMaxValue,&cbMaxValueData,&cbSecurityDescriptor,&ftLastWriteTime) != ERROR_SUCCESS)
   {
   cfPS(cf_error,CF_FAIL,"",pp,a,"Couldn't stat data in registry %s",pp->promiser);
   return;
   }

/* First check the values in this registry key */

if (val_num) 
   {
   CfOut(cf_verbose,""," -> %d values found in key", val_num);
   
   for (i = 0; i < val_num; i++) 
      {
      value[0] = '\0'; 
      data[0] = '\0'; 
      datasize = CF_MAXVARSIZE;
      valsize = CF_MAXVARSIZE; 
      
      if ((ret = RegEnumValue(*key_h,i,value,&valsize,NULL,&type,data,&datasize)) == ERROR_SUCCESS)
         {
         }
      
      if (strlen(value) > 0)
         {
         Nova_RegistryValueIntegrity(dbp,keyname,value,data,datasize,(int)type,a,pp);   
         }
      }
   }

/* Now look for sub-keys/dirs */

if (subkeys)
   {
   for (i = 0; i < subkeys; i++) 
      { 
      key_len = CF_MAXVARSIZE;
      
      if ((ret = RegEnumKeyEx(*key_h,i,sub_key_name,&key_len,NULL,NULL,NULL,NULL)) != ERROR_SUCCESS) 
         {
         cfPS(cf_error,CF_FAIL,"reg",pp,a," !! Could not enumerate the registry key in %s - error %d",pp->promiser,ret);
         continue;
         }
      
      if ((ret = RegOpenKeyEx(*key_h,sub_key_name,0,KEY_READ,&sub_key_h)) != ERROR_SUCCESS)
         {
         cfPS(cf_inform,CF_FAIL,"",pp,a," !! Could not open the key called \"%s\" - error %d",sub_key_name,ret);
         continue;
         }      
      
      snprintf(subkeyname,CF_BUFSIZE-1,"%s\\%s",keyname,sub_key_name);
      CfOut(cf_verbose,""," -> Descending into registry subkey \"%s\"",subkeyname);

      if (!Nova_RegistryKeyIntegrity(dbp,subkeyname,a,pp))
         {
         changes++;
         }

      Nova_RecursiveQueryKey(dbp,&sub_key_h,subkeyname,a,pp,level+1); 
      RegCloseKey(sub_key_h);
      }
   }

if (changes)
   {
   CfOut(cf_verbose,"","Promised verification of the registry recorded %d changes",changes);
   }
}

/*****************************************************************************/

void Nova_DeleteRegistryKey(struct Attributes a,struct Promise *pp)
    
{ int ret;
  HKEY key_h;
  char root_key[CF_MAXVARSIZE],sub_key[CF_MAXVARSIZE];
  char *sp;
  HKEY ms_key;
  struct Rlist *rp;

cf_strncpy(root_key,pp->promiser,CF_MAXVARSIZE-1);
sp = strchr(root_key,'\\');
cf_strncpy(sub_key,sp+1,CF_MAXVARSIZE-1);
*sp = '\0';

ms_key = Str2HKey(root_key);

if (a.database.columns)
   {
   ret = RegOpenKeyEx(ms_key,sub_key,0,KEY_ALL_ACCESS,&key_h);

   switch (ret)
      {
      case ERROR_SUCCESS:
          break;
      default:
          CfOut(cf_error,""," !! Unable to open key %s",pp->promiser);
          return;
      }

   CfOut(cf_verbose,""," -> Registry key \"%s\" opened...\n",pp->promiser);

   for (rp = a.database.columns; rp != NULL; rp = rp->next)
      {
      ret = RegDeleteValue(key_h,rp->item);
      
      switch (ret)
         {
         case ERROR_SUCCESS:
             cfPS(cf_inform,CF_CHG,"",pp,a," -> Registry value \"%s\" in %s deleted...\n",rp->item,pp->promiser);
             break;
         case ERROR_FILE_NOT_FOUND:
	     cfPS(cf_inform,CF_NOP,"",pp,a," -> Registry value \"%s\" in \"%s\" already gone", rp->item, pp->promiser);
             break;
         default:
             CfOut(cf_error,""," !! Unable to delete key value with name \"%s\" - code %d",rp->item,ret);
             break;
         }
      }
   
   return;
   }
else
   {
   ret = RegDeleteKey(ms_key,sub_key);

   switch (ret)
      {
      case ERROR_SUCCESS:
          cfPS(cf_inform,CF_CHG,"",pp,a,"-> Registry key \"%s\" deleted\n",pp->promiser);
          return;
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
	cfPS(cf_inform,CF_NOP,"",pp,a," -> Registry key \"%s\" already gone", pp->promiser);
	  return;
      default:
          CfOut(cf_error,""," !! Unable to delete key \"%s\" - perhaps it has subkeys",pp->promiser);
          return;
      }
   }

}

/*****************************************************************************/

int Nova_VerifyRegistryValueAssocs(HKEY key_h,struct Attributes a,struct Promise *pp)

{ int ret = ERROR_SUCCESS;
  struct Rlist *rp,*rpr,*assign;
  unsigned long reg_data_sz = CF_BUFSIZE;
  DWORD reg_dtype;
  int regCmpSize;
 
for (rp = a.database.rows; rp != NULL; rp=rp->next)
   {
   assign = SplitStringAsRList((char *)(rp->item),',');
   char *name = assign->item;
   char *datatypeStr = assign->next->item;
   char *valueStr = assign->next->next->item;
   reg_dtype = Str2RegDtype(datatypeStr);

   bool match = false;
   
   if(!Nova_CompareRegistryValue(key_h, reg_dtype, name, valueStr, &match))
      {
      // Error
      continue;
      }

   if(match)
      {
      cfPS(cf_inform,CF_NOP,"",pp,a," -> Verified value (%s,%s) correct for %s",name,valueStr,pp->promiser);
      continue;         
      }
   
   DWORD dValue;
   DWORD dataSize;
   
   if (!DONTDO && a.transaction.action != cfa_warn)
      {
      switch (reg_dtype)
         {
         case REG_SZ:
         case REG_EXPAND_SZ:
             dataSize = strlen(valueStr)+1;
             
             cfPS(cf_inform,CF_CHG,"",pp,a," -> Repairing registry %s value as (%s,%s)",datatypeStr,name,valueStr);
             ret = RegSetValueEx(key_h, name, 0, reg_dtype, valueStr, dataSize);
             break;
             
         case REG_DWORD:
             dValue = atoi(valueStr);
             dataSize = sizeof(dValue);
             
             cfPS(cf_inform,CF_CHG,"",pp,a," -> Repairing registry %s value as (%s,%s)",datatypeStr,name,valueStr);
             ret = RegSetValueEx(key_h, name, 0, REG_DWORD, (char *)&dValue, dataSize);
             break;
             
         default:
             cfPS(cf_error,CF_INTERPT,"",pp,a,"Unknown data type for registry value for \"%s\" - ignored",name);
             ret = ERROR_SUCCESS;
         }
      }
   else
      {
      cfPS(cf_error,CF_WARN,"",pp,a," !! Registry value incorrect, but only a warning was promised");
      CfOut(cf_inform,""," -> (%s,%s) incorrect for %s",name,valueStr,pp->promiser);
      ret = ERROR_SUCCESS;
      }

   DeleteRlist(assign);
   
   if (ret != ERROR_SUCCESS)
      {
      return -1;
      }
   }

return 0;
}

/*****************************************************************************/

int Nova_GetRegistryValue(HKEY key_h, char *name, void *data_p, unsigned long *data_sz)

{ int ret;

ret = RegQueryValueEx(key_h,name,NULL,NULL,data_p,data_sz);

if (ret == ERROR_SUCCESS)
   {
   return true;
   }
else
   {
   CfOut(cf_verbose,"","Could not read existing registry data for '%s'.\n", name);
   return false;
   } 
}

/*****************************************************************************/

int Nova_GetRegistryValueAsString(char *key, char *name, char *buf, int bufSz)

{
 unsigned long reg_data_sz = CF_BUFSIZE;
 int len;
 HKEY key_h;
 
 if (!Nova_OpenRegistryKey(key,&key_h,false))
    {
    CfOut(cf_verbose,"RegQueryValueEx","Could not read existing registry data for '%s'.\n", name);
    return false;
    }

 DWORD dType;
 char work[MAX_VALUE_SIZE];
 DWORD workSz = sizeof(work);
 
 if(RegQueryValueEx(key_h,name,NULL,&dType,work,&workSz) != ERROR_SUCCESS)
    {
    CfOut(cf_verbose,"RegQueryValueEx","Could not read existing registry data for '%s'.\n", name);
    return false;
    }

 DWORD *dwordValuep;
 
 switch(dType)
    {
    case REG_SZ:
    case REG_EXPAND_SZ:
        strlcpy(buf,work,bufSz);
        break;
        
    case REG_DWORD:
        dwordValuep = (DWORD *)work;
        snprintf(buf, bufSz, "%d", *dwordValuep);
        break;
        
    default:
        CfOut(cf_error, "", "!! Nova_GetRegistryValueAsString: Unknown value type %d for %s", dType, name);
        return false;
    }

 return true;
}

/*****************************************************************************/

DWORD Str2RegDtype(char *datatypeStr)

{
 if(strcmp(datatypeStr, "REG_SZ") == 0)
    {
    return REG_SZ;
    }
 else if(strcmp(datatypeStr, "REG_DWORD") == 0)
    {
    return REG_DWORD;
    }
 else if(strcmp(datatypeStr, "REG_EXPAND_SZ") == 0)
    {
    return REG_EXPAND_SZ;
    }

 CfOut(cf_error, "", "!! Str2RegDtype: Unknown data type %s", datatypeStr);
 return REGDTYPE_UNKNOWN;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_ValidateRegistryPromiser(char *key,struct Attributes a,struct Promise *pp)

{ static char *valid[] = { "HKEY_CLASSES_ROOT","HKEY_CURRENT_CONFIG",
                           "HKEY_CURRENT_USER","HKEY_LOCAL_MACHINE", "HKEY_USERS", NULL };
  char root_key[CF_MAXVARSIZE];
  char *sp;
  int i;

  /* First remove the root key */
  
cf_strncpy(root_key,key,CF_MAXVARSIZE-1);
sp = strchr(root_key,'\\');
*sp = '\0';

for (i = 0; valid[i] != NULL; i++)
   {
   if (cf_strcmp(root_key,valid[i]) == 0)
      {
      return true;
      }
   }

CfOut(cf_error,"","Non-editable registry prefix \"%s\"",root_key);
PromiseRef(cf_error,pp);
return false;
}

/*****************************************************************************/

HKEY Str2HKey(char *root_key)

{
if (cf_strcmp(root_key,"HKEY_CLASSES_ROOT") == 0)
   {
   return HKEY_CLASSES_ROOT;
   }

if (cf_strcmp(root_key,"HKEY_CURRENT_CONFIG") == 0)
   {
   return HKEY_CURRENT_CONFIG;
   }

if (cf_strcmp(root_key,"HKEY_CURRENT_USER") == 0)
   {
   return HKEY_CURRENT_USER;
   }

if (cf_strcmp(root_key,"HKEY_LOCAL_MACHINE") == 0)
   {
   return HKEY_LOCAL_MACHINE;
   }

if (cf_strcmp(root_key,"HKEY_USERS") == 0)
   {
   return HKEY_USERS;
   }

return HKEY_LOCAL_MACHINE;
}

/*****************************************************************************/

void Nova_RecursiveRestoreKey(CF_DB *dbp,char *keyname,struct Attributes a,struct Promise *pp) 

{ CF_DBC *dbcp;
  int ret,is_a_key = false,is_a_value = false;
  char *key;
  void *value;
  int ksize,vsize;

/* Acquire a cursor for the database. */

if (!NewDBCursor(dbp,&dbcp))
   {
   CfOut(cf_inform,""," !! Unable to scan registry cache db");
   return;
   }

 /* Walk through the database and print out the key/data pairs. */

while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
   {
   is_a_key = false;
   is_a_value = false;

   if (IsInListOfRegex(a.database.exclude,key))
      {
      continue;
      }

   if (cf_strncmp(key,keyname,strlen(keyname)) != 0)
      {
      continue;
      }

   if (cf_strchr(key,':'))
      {
      is_a_value = true;
      }
   else
      {
      is_a_key = true;
      }

   /* Need to create the container */

   if (is_a_key)
      {
      HKEY skey_h;

      if (!Nova_OpenRegistryKey(key,&skey_h,false))
         {
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry key %s",key);

            if (!Nova_OpenRegistryKey(key,&skey_h,true))
               {
               cfPS(cf_error,CF_FAIL,"",pp,a," !! Registry key could not be created.\n");
               }
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry key \"%s\" missing, but only a warning was promised.",key);
            }
         }
      else
         {
         RegCloseKey(skey_h);
         }

      continue;
      }

   /* Need to refill the container */

   if (is_a_value)
      { 
      char dbkey[CF_MAXVARSIZE],regkey[CF_MAXVARSIZE];
      char dbvalue[CF_MAXVARSIZE],reg_data[CF_BUFSIZE];
      int  dbtype = REG_SZ,regtype = REG_SZ;
      unsigned long data_size = CF_BUFSIZE;
      
      sscanf(key,"%255[^:]:%255[^:]:%u",dbkey,dbvalue,&dbtype);
      
      Debug("CACHED: %s\nVAL: %s\n",dbkey,dbvalue);
      
      HKEY skey_h;
      
      if (!Nova_OpenRegistryKey(dbkey,&skey_h,true))
         {
         CfOut(cf_error,"","Couldn't open the key %s\n",dbkey);
         continue;
         }
      
      ret = RegQueryValueEx(skey_h,dbvalue,NULL,NULL,reg_data,&data_size);
      
      if (ret == ERROR_SUCCESS)
         {
         Debug("READ value %s\n",reg_data);
         }
      else
         {
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry value as (%s,%s)",dbvalue,value);
            ret = RegSetValueEx(skey_h,dbvalue,0,dbtype,value,vsize);
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry value incorrect, but only a warning was promised.");
            cfPS(cf_inform,CF_NOP,"",pp,a," -> (%s,%s) incorrect for %s",dbvalue,value,pp->promiser);
            }
         
         continue;
         }  
      
      if (memcmp(reg_data,value,vsize) == 0)
         {
         cfPS(cf_inform,CF_NOP,"",pp,a," -> verified value (%s,%s) correct",dbvalue,reg_data);
         continue;         
         }
      else
         {
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry value as (%s,%s)",dbvalue,value);
            ret = RegSetValueEx(skey_h,dbvalue,0,dbtype,value,vsize);
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry value incorrect, but only a warning was promised.");
            cfPS(cf_inform,CF_NOP,"",pp,a," -> (%s,%s) incorrect for %s",dbvalue,value,pp->promiser);
            }
         }
      
      RegCloseKey(skey_h);
      }
   }

DeleteDBCursor(dbp,dbcp);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_OpenRegistryKey(char *key,HKEY *key_h,int create)
    
{ int ret;
  char root_key[CF_MAXVARSIZE],sub_key[CF_MAXVARSIZE];
  char *sp;
  HKEY ms_key;

  /* First remove the root key */
  
cf_strncpy(root_key,key,CF_MAXVARSIZE-1);
sp = strchr(root_key,'\\');
cf_strncpy(sub_key,sp+1,CF_MAXVARSIZE-1);
*sp = '\0';

ms_key = Str2HKey(root_key);

if (create)  // create key if nonexistant
   {
   ret = RegCreateKeyEx(ms_key,sub_key,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,key_h,NULL);
   }
else
   {
   ret = RegOpenKeyEx(ms_key,sub_key,0,KEY_ALL_ACCESS,key_h);
   }

switch (ret)
   {
   case ERROR_SUCCESS:
       return true;
   default:
       if (create)
          {
          CfOut(cf_error,"","Could not create/open registry key '%s' in '%s' - system error %d\n",sub_key,root_key,ret);
          }
       return false;
   }
}

/*****************************************************************************/

int Nova_RegistryKeyIntegrity(CF_DB *dbp,char *key,struct Attributes a,struct Promise *pp)

{ char dbkey[CF_BUFSIZE];
  int size = 0, dummy;

if (Nova_ReadCmpPseudoRegistry(dbp,dbkey,NULL,0,NULL))
   {
   CfOut(cf_error,""," -> Registry key \"%s\" exists",key);
   return true;
   }
else
   {
   CfOut(cf_inform,""," !! New registry key \"%s\" ",key);

   if (a.database.operation && (cf_strcmp(a.database.operation,"cache") == 0))
      {
      WriteDB(dbp,key,"",1);
      }
   return false;
   }
}

/*****************************************************************************/

void Nova_RegistryValueIntegrity(CF_DB *dbp,char *key,char *value,char *data,int size,int type,struct Attributes a,struct Promise *pp)

{ char dbkey[CF_BUFSIZE];
  int cmp_ok;

if (IsInListOfRegex(a.database.exclude,key))
   {
   CfOut(cf_verbose,""," -> Registry key \"%s\" excluded as promised",key);
   return;
   }

if (IsInListOfRegex(a.database.exclude,value))
   {
   CfOut(cf_verbose,""," -> Registry value \"%s\" excluded as promised",value);
   return;
   }

snprintf(dbkey,CF_BUFSIZE-1,"%s:%s:%u",key,value,type);

if (Nova_ReadCmpPseudoRegistry(dbp,dbkey,data,size,&cmp_ok))
   {
   if (cmp_ok)
      {
      CfOut(cf_verbose,""," -> Registry value \"%s\" unchanged",value);
      }
   else
      {
      if (a.database.operation && (cf_strcmp(a.database.operation,"cache") == 0))
         {
         WriteDB(dbp,dbkey,data,size);
         }
      
      cfPS(cf_error,CF_CHG,"",pp,a," !! Registry value \"%s\" found in key \"%s\" changed",value,key);
      }
   }
else
   {
   if (a.database.operation && (cf_strcmp(a.database.operation,"cache") == 0))
      {
      cfPS(cf_error,CF_CHG,"",pp,a," !! New registry value \"%s\" found in key \"%s\"",value,key);
      WriteDB(dbp,dbkey,data,size);
      }
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_ReadCmpPseudoRegistry(CF_DB *dbp,char *dbkey,void *ptr,int size,int *cmp)

{ void *value;
  int vsize;
 
if (RevealDB(dbp,dbkey,&value,&vsize))
   {
   if (ptr == NULL)
      {
      if (cmp)
         {
         *cmp = true;
         }

      return true;
      }

   if (vsize != size)
      {
      if (cmp)
         {
         *cmp = false;
         }
      }
   else if (value)
      {
      if (memcmp(ptr,value,size) == 0)
         {
         if (cmp)
            {
            *cmp = true;
            }
         }
      else
         {
         if (cmp)
            {
            *cmp = false;
            }
         }
      }
   else
      {
      if (cmp)
         {
         *cmp = true;
         }

      return false;
      }
   
   return true;
   }
else
   {
   if (cmp)
      {
      *cmp = true;
      }

   return false;
   }
}

/*****************************************************************************/

static bool Nova_CompareRegistryValue(HKEY key_h, DWORD dataType, char *name, char *valueStr, bool *outMatch)
/**
 * Stores compared result in outMatch.
 * Returns true on successful check, false otherwise.
 **/
{
 DWORD currDtype;
 char currValue[MAX_VALUE_SIZE];
 DWORD valueSz = sizeof(currValue);

 *outMatch = false;

 DWORD dwRet = RegQueryValueEx(key_h, name, NULL, &currDtype, currValue, &valueSz);

 switch(dwRet)
    {
    case ERROR_SUCCESS:
        break;
    case ERROR_FILE_NOT_FOUND:
        return true;
    default:
        CfOut(cf_error, "RegQueryValueEx", "!! Nova_CompareRegistryValue: Could not read existing value (got %d)", dwRet);
        return false;
    }
 
 if(currDtype != dataType)
    {
    return true;
    }

 DWORD *dwordCurrValuep;
 
 switch(dataType)
    {
    case REG_SZ:
    case REG_EXPAND_SZ:
        
        if(strcmp(currValue, valueStr) != 0)
           {
           return true;
           }
        
        break;
        
    case REG_DWORD:

        if(valueSz != sizeof(DWORD))
           {
           CfOut(cf_error, "", "!! Nova_CompareRegistryValue: Existing DWORD-value did not have DWORD size");
           return false;
           }
        
        dwordCurrValuep = (DWORD *)currValue;
        
        if((*dwordCurrValuep != atoi(valueStr)))
           {
           return true;
           }
        
        break;
        
    default:
        CfOut(cf_error, "", "!! Nova_CompareRegistryValue: Unknown registry value type %d", dataType);
        return false;
    }

 
 *outMatch = true;  // match
 return true;
}

#endif



 
