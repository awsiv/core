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

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, bool *authenticated_external);

cfapi_errid CFDB_HasHostAccessFromUserRBAC(const char *userName, const char *hostKey);
HubQuery *CFDB_HostClassFilterFromUserRBAC(const char *userName);
HubQuery *CFDB_PromiseFilterFromUserRBAC(const char *userName);

cfapi_errid CFDB_CreateUser(const char *creating_username, const char *username, const char *password, const char *email, const Sequence *roles);
cfapi_errid CFDB_UpdateUser(const char *updating_username, const char *username, const char *password, const char *email, const Sequence *roles);
cfapi_errid CFDB_DeleteUser(const char *deleting_username, const char *username);
cfapi_errid CFDB_GetUser(const char *getting_username, const char *username, HubUser **user_out);
HubQuery *CFDB_ListUsers(const char *listing_username, const char *username_rx);


cfapi_errid CFDB_CreateRole(const char *creatingUser, const char *roleName, const char *description,
                            const char *includeClassRx, const char *excludeClassRx, const char *includeBundleRx, const char *excludeBundleRx);
cfapi_errid CFDB_DeleteRole(const char *deletingUser, const char *roleName, bool deassociateUsers);
cfapi_errid CFDB_UpdateRole(const char *updatingUser, const char *roleName, const char *description,
                            const char *includeClassRx, const char *excludeClassRx, const char *includeBundleRx, const char *excludeBundleRx);
HubQuery *CFDB_ListRoles(const char *listing_user);

cfapi_errid CFDB_UserIsAdminWhenRBAC(const char *username);
HubQuery *CFDB_GetRoleByNameAuth(const char *user_name, const char *role_name);

bool HubSettingsValidate(const HubSettings *settings, char error_out[CF_BUFSIZE]);
bool RBACFilterSuccess(cfapi_errid errid);
#endif
