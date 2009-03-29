
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

#ifdef NT

#include <windows.h>

/*****************************************************************************/

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

enum reg_data_type
   {
   cf_reg_sz,
   cf_reg_notype
   };

/*****************************************************************************/

void Nova_VerifyRegistryPromise(struct Attributes a,struct Promise *pp);
int Nova_OpenRegistryKey(char *key, HKEY *key_h, int create);
int Nova_PrintAllValues(HKEY key_h);
int Nova_VerifyRegistryValueAssocs(HKEY key_h,struct Attributes a,struct Promise *pp);
int Nova_GetRegistryValue(HKEY key_h, char *value, void *data_p, unsigned long data_sz);
enum reg_data_type Str2RegDtype(char *datatype);
void Nova_RecursiveQueryKey(DB *dbp,HKEY *key_h,char *name,struct Attributes a,struct Promise *pp, int level);
int Nova_RegistryKeyIntegrity(DB *dbp,char *key,struct Attributes a,struct Promise *pp);
void Nova_RegistryValueIntegrity(DB *dbp,char *key,char *value,char *data,int size,int type,struct Attributes a,struct Promise *pp);
int Nova_ReadCmpPseudoRegistry(DB *dbp,char *dbkey,void *ptr,int size,int *cmp);
int Nova_WritePseudoRegistry(DB *dbp,char *dbkey,void *existing_value,int size);
enum reg_data_type Str2RegDtype(char *datatype);
HKEY Str2HKey(char *root_key);
void Nova_RecursiveRestoreKey(DB *dbp,char *keyname,struct Attributes a,struct Promise *pp);

/*****************************************************************************/

void Nova_VerifyRegistryPromise(struct Attributes a,struct Promise *pp)
 
{ HKEY key_h;  // a registry key handle
  char name[CF_MAXVARSIZE];
  int rr,rw;
  DB *dbp;

*(pp->donep) = true;

  struct CfLock thislock;
  char lockname[CF_BUFSIZE];

snprintf(lockname,CF_BUFSIZE-1,"db-%s",pp->promiser);
 
thislock = AcquireLock(lockname,VUQNAME,CFSTARTTIME,a,pp);

if (thislock.lock == NULL)
   {
   return;
   }

if (a.database.operation && strcmp(a.database.operation,"drop") == 0)
   {
   YieldCurrentLock(thislock);
   return;
   }

else if (a.database.operation && strcmp(a.database.operation,"create") == 0)
   { 
   if (Nova_OpenRegistryKey(pp->promiser,&key_h,false))
      {
      cfPS(cf_verbose,CF_NOP,"",pp,a," -> Registry key exists, promise kept");
      }
   else if (!Nova_OpenRegistryKey(pp->promiser,&key_h,true))
      {
      cfPS(cf_error,CF_FAIL,"",pp,a," !! Registry key could not be created!");
      YieldCurrentLock(thislock);
      return;
      }
   }

if (Nova_OpenRegistryKey(pp->promiser,&key_h,false))
   {
   CfOut(cf_verbose,""," -> Registry key \"%s\" opened...\n",pp->promiser);   
   
   snprintf(name,CF_MAXVARSIZE-1,"%s/mswin.db",CFWORKDIR);
   
   if (!OpenDB(name,&dbp))
      {
      YieldCurrentLock(thislock);
      return;
      }
   
   if (a.database.operation && strcmp(a.database.operation,"restore") == 0)
      {
      Nova_RecursiveRestoreKey(dbp,pp->promiser,a,pp) ;
      YieldCurrentLock(thislock);
      return;
      }
   
   if (a.database.rows)
      {
      Nova_VerifyRegistryValueAssocs(key_h,a,pp);
      }      
   else if (a.database.operation && (strcmp(a.database.operation,"verify") == 0)
            || (strcmp(a.database.operation,"cache") == 0))
      {
      CfOut(cf_verbose,"","Recursive cache of registry from here...\n");
      Nova_RecursiveQueryKey(dbp,&key_h,pp->promiser,a,pp,0) ;
      }
   }
else
   {
   cfPS(cf_error,CF_FAIL,"Open",pp,a," !! Registry key \"%s\"failed to open\n",pp->promiser);
   }

dbp->close(dbp,0);
RegCloseKey(key_h);
YieldCurrentLock(thislock);
}

/*****************************************************************************/

int Nova_OpenRegistryKey(char *key,HKEY *key_h,int create)
    
{ int ret;
  char root_key[CF_MAXVARSIZE],sub_key[CF_MAXVARSIZE];
  char *sp;
  HKEY ms_key;

  /* First remove the root key */
  
strncpy(root_key,key,CF_MAXVARSIZE-1);
sp = strchr(root_key,'\\');
strncpy(sub_key,sp+1,CF_MAXVARSIZE-1);
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

int Nova_PrintAllValues(HKEY key_h)

{ int i, ret;
  char value[CF_MAXVARSIZE] = "VALUE\0";
  char data[CF_BUFSIZE] = "DATA\0";
  unsigned long value_sz, data_sz;

i = 0;
ret = ERROR_SUCCESS;
value_sz = CF_MAXVARSIZE;
data_sz = CF_BUFSIZE;

while (ret == ERROR_SUCCESS)
   {
   memset(value, '\0', CF_MAXVARSIZE);
   memset(data, '\0', CF_BUFSIZE);
   
   // TODO: Note: Also possible to read the type of the data
   ret = RegEnumValue(key_h, i, value, &value_sz, NULL, NULL, data, &data_sz);
   printf("(%s, %s)\n", value, data);
   
   value_sz = CF_MAXVARSIZE;
   data_sz = CF_BUFSIZE;
   i++;
   }

  // the reason for stopping should be an exhausted list of values

if (ret != ERROR_NO_MORE_ITEMS)
   {
   printf("Error: Could not enumerate values.\n");
   printf("Code: %d.\n", ret);
   return -1;
   }

return 0;
}

/*****************************************************************************/

int Nova_VerifyRegistryValueAssocs(HKEY key_h,struct Attributes a,struct Promise *pp)

{ int ret = ERROR_SUCCESS;
  struct Rlist *rp,*rpr,*assign;
  char *name,*value,*datatype;
  unsigned long reg_data_sz = CF_BUFSIZE;
  void *reg_data_p = calloc(1,CF_BUFSIZE);
  enum reg_data_type reg_dtype;
 
for (rp = a.database.rows; rp != NULL; rp=rp->next)
   {
   assign = SplitStringAsRList((char *)(rp->item),',');
   name = assign->item;
   datatype = assign->next->item;
   value = assign->next->next->item;
   reg_dtype = Str2RegDtype(datatype);
   
   if (Nova_GetRegistryValue(key_h,name,reg_data_p,reg_data_sz))
      {
      if (memcmp(reg_data_p,value,strlen(value)) == 0)
         {
         cfPS(cf_inform,CF_NOP,"",pp,a," -> verified value (%s,%s) correct for %s",name,value,pp->promiser);
         continue;         
         }
      else
         {
         cfPS(cf_inform,CF_FAIL,"",pp,a," -> value (%s,%s) incorrect for %s",name,value,pp->promiser);
         }
      }
   
   switch (reg_dtype)
      {
      case cf_reg_sz:
          
          if (!DONTDO && a.transaction.action != cfa_warn)
             {
             cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry value as (%s,%s)",name,value);
             ret = RegSetValueEx(key_h,name,0,REG_SZ,value,strlen(value)+1);
             }
          else
             {
             cfPS(cf_error,CF_NOP,"",pp,a," !! Registry value incorrect, but only a warning was promised.");
             cfPS(cf_inform,CF_NOP,"",pp,a," -> (%s,%s) incorrect for %s",name,value,pp->promiser);
             }
          break;
          
      default:
          cfPS(cf_error,CF_INTERPT,"",pp,a,"Unknown data type for registry value for \"%s\" - ignored",name);
          ret = 0;
      }

   DeleteRlist(assign);
   
   if (ret != ERROR_SUCCESS)
      {
      free(reg_data_p);
      return -1;
      }
   }

free(reg_data_p);
return 0;
}

/*****************************************************************************/

int Nova_GetRegistryValue(HKEY key_h, char *value, void *data_p, unsigned long data_sz)

{ int ret;

ret = RegQueryValueEx(key_h, value, NULL, NULL, data_p, &data_sz);

if (ret == ERROR_SUCCESS)
   {
   return true;
   }
else
   {
   CfOut(cf_verbose,"","Could not read existing registry data value for '%s'.\n", value);
   return false;
   } 
}

/*****************************************************************************/

enum reg_data_type Str2RegDtype(char *datatype)

{ int i;
  static char *types[] = { "REG_SZ", NULL };

for (i = 0; types[i] != NULL; i++)
   {
   if (strcmp(types[i],datatype) == 0)
      {
      return (enum reg_data_type)i;
      }
   }

return cf_reg_notype;
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
  
strncpy(root_key,key,CF_MAXVARSIZE-1);
sp = strchr(root_key,'\\');
*sp = '\0';

for (i = 0; valid[i] != NULL; i++)
   {
   if (strcmp(root_key,valid[i]) == 0)
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
if (strcmp(root_key,"HKEY_CLASSES_ROOT") == 0)
   {
   return HKEY_CLASSES_ROOT;
   }

if (strcmp(root_key,"HKEY_CURRENT_CONFIG") == 0)
   {
   return HKEY_CURRENT_CONFIG;
   }

if (strcmp(root_key,"HKEY_CURRENT_USER") == 0)
   {
   return HKEY_CURRENT_USER;
   }

if (strcmp(root_key,"HKEY_LOCAL_MACHINE") == 0)
   {
   return HKEY_LOCAL_MACHINE;
   }

if (strcmp(root_key,"HKEY_USERS") == 0)
   {
   return HKEY_USERS;
   }

return HKEY_LOCAL_MACHINE;
}

/*****************************************************************************/

void Nova_RecursiveQueryKey(DB *dbp,HKEY *key_h,char *keyname,struct Attributes a,struct Promise *pp, int level) 

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

void Nova_RecursiveRestoreKey(DB *dbp,char *keyname,struct Attributes a,struct Promise *pp) 

{ DBT key,value;
  DBC *dbcp;
  int ret,is_a_key = false,is_a_value = false;

/* Acquire a cursor for the database. */

if ((ret = dbp->cursor(dbp,NULL,&dbcp,0)) != 0)
   {
   cfPS(cf_error,CF_FAIL,"",pp,a,"Error reading from checksum database");
   dbp->err(dbp,ret,"DB->cursor");
   return;
   }

 /* Walk through the database and print out the key/data pairs. */

memset(&key,0,sizeof(key));
memset(&value,0,sizeof(value));

while (dbcp->c_get(dbcp,&key,&value,DB_NEXT) == 0)
   {
   char *name = (char *)key.data;
   is_a_key = false;
   is_a_value = false;

   if (IsInListOfRegex(a.database.exclude,name))
      {
      continue;
      }

   if (strncmp(name,keyname,strlen(keyname)) != 0)
      {
      continue;
      }

   if (strchr(name,':'))
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

      if (!Nova_OpenRegistryKey(name,&skey_h,false))
         {
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry key %s",name);
            if (!Nova_OpenRegistryKey(name,&skey_h,true))
               {
               cfPS(cf_error,CF_FAIL,"",pp,a," !! Registry key could not be created.\n");
               }
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry key \"%s\" missing, but only a warning was promised.",name);
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
      
      sscanf(name,"%255[^:]:%255[^:]:%u",dbkey,dbvalue,&dbtype);
      
      Debug("CACHED: %s\nVAL: %s\n",dbkey,dbvalue);
      
      HKEY skey_h;
      
      if (!Nova_OpenRegistryKey(dbkey,&skey_h,true))
         {
         printf("Couldn't open the key\n");
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
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry value as (%s,%s)",dbvalue,value.data);
            ret = RegSetValueEx(skey_h,dbvalue,0,dbtype,value.data,value.size);
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry value incorrect, but only a warning was promised.");
            cfPS(cf_inform,CF_NOP,"",pp,a," -> (%s,%s) incorrect for %s",dbvalue,value.data,pp->promiser);
            }
         
         continue;
         }  
      
      if (memcmp(reg_data,value.data,value.size) == 0)
         {
         cfPS(cf_inform,CF_NOP,"",pp,a," -> verified value (%s,%s) correct",dbvalue,reg_data);
         continue;         
         }
      else
         {
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            cfPS(cf_error,CF_CHG,"",pp,a," !! Repairing registry value as (%s,%s)",dbvalue,value.data);
            ret = RegSetValueEx(skey_h,dbvalue,0,dbtype,value.data,value.size);
            }
         else
            {
            cfPS(cf_error,CF_NOP,"",pp,a," !! Registry value incorrect, but only a warning was promised.");
            cfPS(cf_inform,CF_NOP,"",pp,a," -> (%s,%s) incorrect for %s",dbvalue,value.data,pp->promiser);
            }
         }
      
      RegCloseKey(skey_h);
      }
   
   memset(&key,0,sizeof(key));
   memset(&value,0,sizeof(value));
   }

dbcp->c_close(dbcp);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_RegistryKeyIntegrity(DB *dbp,char *key,struct Attributes a,struct Promise *pp)

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

   if (a.database.operation && (strcmp(a.database.operation,"cache") == 0))
      {
      Nova_WritePseudoRegistry(dbp,key,"",1);
      }
   return false;
   }
}

/*****************************************************************************/

void Nova_RegistryValueIntegrity(DB *dbp,char *key,char *value,char *data,int size,int type,struct Attributes a,struct Promise *pp)

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
      if (a.database.operation && (strcmp(a.database.operation,"cache") == 0))
         {
         Nova_WritePseudoRegistry(dbp,dbkey,data,size);
         }
      
      cfPS(cf_error,CF_CHG,"",pp,a," !! Registry value \"%s\" found in key \"%s\" changed",value,key);
      }
   }
else
   {
   if (a.database.operation && (strcmp(a.database.operation,"cache") == 0))
      {
      cfPS(cf_error,CF_CHG,"",pp,a," !! New registry value \"%s\" found in \"key\" %s",value,key);
      Nova_WritePseudoRegistry(dbp,dbkey,data,size);
      }
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_ReadCmpPseudoRegistry(DB *dbp,char *dbkey,void *ptr,int size,int *cmp)

{ DBT *key,value;
  
key = NewDBKey(dbkey);
memset(&value,0,sizeof(DBT));

if ((errno = dbp->get(dbp,NULL,key,&value,0)) == 0)
   {
   if (ptr == NULL)
      {
      if (cmp)
         {
         *cmp = true;
         }
      DeleteDBKey(key);
      return true;
      }

   if (value.size != size)
      {
      if (cmp)
         {
         *cmp = false;
         }
      }
   else if (value.data)
      {
      if (memcmp(ptr,value.data,size) == 0)
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
      DeleteDBKey(key);
      return false;
      }
   
   DeleteDBKey(key);
   return true;
   }
else
   {
   if (cmp)
      {
      *cmp = true;
      }
   Debug("Database read failed: %s",db_strerror(errno));
   DeleteDBKey(key);
   return false;
   }
}

/*****************************************************************************/

int Nova_WritePseudoRegistry(DB *dbp,char *dbkey,void *existing_value,int size)

{ DBT *key,*value;
 
key = NewDBKey(dbkey); 
value = NewDBValue(existing_value,size);

if ((errno = dbp->put(dbp,NULL,key,value,0)) != 0)
   {
   Debug("Database write failed: %s",db_strerror(errno));
   DeleteDBKey(key);
   DeleteDBValue(value);
   return false;
   }
else
   {
   DeleteDBKey(key);
   DeleteDBValue(value);
   return true;
   }
}

#endif



