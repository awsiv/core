/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"
#include "bson_lib.h"

#define MONGO_ROLES_COLLECTION "phpcfengine.roles"
#define MONGO_USERS_INTERNAL_COLLECTION "phpcfengine.users"
#define MONGO_USERS_LDAP_COLLECTION "phpcfengine.ldap_users"
#define dbkey_user_roles "roles"
#define dbkey_role_name "name"
#define dbkey_role_members "members"
#define dbkey_role_description "description"
#define dbkey_role_classrx_include "crxi"
#define dbkey_role_classrx_exclude "crxx"
#define dbkey_role_bundlerx_include "brxi"

cfapi_errid CFDB_CreateRole(char *name, char *description, char *includeClassRx, char *excludeClassRx, char *includeBundleRx);
cfapi_errid CFDB_DeleteRole(char *name);
Item *CFDB_GetAllRoles(void);
Item *CFDB_GetRolesForUser(char *userName);
cfapi_errid CFDB_SetRolesForUser(char *userName, Item *roles);
HubUserRBAC *CFDB_GetRBACForUser(char *userName);
