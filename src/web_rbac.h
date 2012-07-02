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

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len);

cfapi_errid CFDB_HasHostAccessFromUserRBAC(char *userName, char *hostKey);
HubQuery *CFDB_HostClassFilterFromUserRBAC(char *userName);
HubQuery *CFDB_PromiseFilterFromUserRBAC(char *userName);
HubQuery *CFDB_GetRBACForUser(char *userName);

cfapi_errid CFDB_CreateUser(const char *creating_user, const char *username, const char *password);
cfapi_errid CFDB_DeleteUser(const char *deleting_user, const char *username);
HubQuery *CFDB_ListUsers(const char *listing_user, const char *usernameRx);

cfapi_errid CFDB_CreateRole(char *creatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx);
cfapi_errid CFDB_DeleteRole(char *deletingUser, char *roleName, bool deassociateUsers);
cfapi_errid CFDB_UpdateRole(char *updatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx);
HubQuery *CFDB_ListRoles(const char *listing_user);

cfapi_errid CFDB_UserIsAdminWhenRBAC(char *username);
HubQuery *CFDB_GetRoleByNameAuth(const char *user_name, const char *role_name);

#endif
