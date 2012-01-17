/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_WEB_RBAC_H
#define CFENGINE_WEB_RBAC_H

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"
#include "bson_lib.h"

#ifdef HAVE_LIBMONGOC

#define MONGO_ROLES_COLLECTION "phpcfengine.roles"
#define MONGO_USERS_INTERNAL_COLLECTION "phpcfengine.users"
#define MONGO_USERS_LDAP_COLLECTION "phpcfengine.ldap_users"
#define dbkey_user_name "username"
#define dbkey_user_roles "roles"
#define dbkey_role_name "name"
#define dbkey_role_description "description"
#define dbkey_role_classrx_include "crxi"
#define dbkey_role_classrx_exclude "crxx"
#define dbkey_role_bundlerx_include "brxi"

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len);
HubQuery *CFDB_GetRBACForUser(char *userName);
cfapi_errid CFDB_CreateRole(char *creatingUser, char *roleName, char *description, char *includeClassRx, char *excludeClassRx, char *includeBundleRx);
cfapi_errid CFDB_DeleteRole(char *deletingUser, char *roleName);
HubQuery *CFDB_GetAllRoles(void);
HubQuery *CFDB_GetRoleByName(char *name);

#endif

#endif
