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

# define MONGO_ROLES_COLLECTION MONGO_MPBASE ".roles"
# define MONGO_USERS_INTERNAL_COLLECTION MONGO_MPBASE ".users"
# define MONGO_USERS_LDAP_COLLECTION MONGO_MPBASE ".ldap_users"
# define MONGO_MPSETTINGS_COLLECTION MONGO_MPBASE ".appsettings"

# define dbkey_mpsettings_rbac "rbac"
# define dbkey_mpsettings_auth_mode "mode"
# define dbkey_mpsettings_encryption "encryption"
# define dbkey_mpsettings_login_attribute "login_attribute"
# define dbkey_mpsettings_base_dn "base_dn"
# define dbkey_mpsettings_users_directory "users_directory"
# define dbkey_mpsettings_host "host"

# define dbkey_user_name "username"
# define dbkey_user_password "password"
# define dbkey_user_active "active"
# define dbkey_user_roles "roles"

# define dbkey_role_name "name"
# define dbkey_role_description "description"
# define dbkey_role_classrx_include "crxi"
# define dbkey_role_classrx_exclude "crxx"
# define dbkey_role_bundlerx_include "brxi"
# define dbkey_role_bundlerx_exclude "brxx"

cfapi_errid CFDB_HasHostAccessFromUserRBAC(char *userName, char *hostKey);
HubQuery *CFDB_HostClassFilterFromUserRBAC(char *userName);
HubQuery *CFDB_PromiseFilterFromUserRBAC(char *userName);
cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len);
HubQuery *CFDB_GetRBACForUser(char *userName);
cfapi_errid CFDB_CreateRole(char *creatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx);
cfapi_errid CFDB_DeleteRole(char *deletingUser, char *roleName, bool deassociateUsers);
cfapi_errid CFDB_UpdateRole(char *updatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx);
cfapi_errid CFDB_UserIsAdminWhenRBAC(char *username);
HubQuery *CFDB_GetAllRolesAuth(char *userName);
HubQuery *CFDB_GetRoleByNameAuth(char *userName, char *roleName);

#endif
