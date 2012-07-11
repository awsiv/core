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

typedef enum
{
    SETTING_UNKNOWN = 0,

    SETTING_RBAC,
    SETTING_AUTH_MODE,
    SETTING_LDAP_ENCRYPTION,
    SETTING_LDAP_LOGIN_ATTRIBUTE,
    SETTING_LDAP_BASE_DN,
    SETTING_LDAP_USERS_DIRECTORY,
    SETTING_LDAP_HOST,
    SETTING_AD_DOMAIN,
    SETTING_BLUEHOST_HORIZON,

    SETTING_MAX
} HubSetting;

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len);

cfapi_errid CFDB_HasHostAccessFromUserRBAC(char *userName, char *hostKey);
HubQuery *CFDB_HostClassFilterFromUserRBAC(char *userName);
HubQuery *CFDB_PromiseFilterFromUserRBAC(char *userName);

cfapi_errid CFDB_CreateUser(const char *username, const char *password, const char *email, const Rlist *roles);
cfapi_errid CFDB_UpdateUser(const char *username, const char *password, const char *email, const Rlist *roles);
cfapi_errid CFDB_DeleteUser(const char *username);
HubQuery *CFDB_ListUsers(const char *usernameRx);

cfapi_errid CFDB_CreateRole(const char *creatingUser, const char *roleName, const char *description,
                            const char *includeClassRx, const char *excludeClassRx, const char *includeBundleRx, const char *excludeBundleRx);
cfapi_errid CFDB_DeleteRole(const char *deletingUser, const char *roleName, bool deassociateUsers);
cfapi_errid CFDB_UpdateRole(char *updatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx);
HubQuery *CFDB_ListRoles(const char *listing_user);

cfapi_errid CFDB_UserIsAdminWhenRBAC(const char *username);
HubQuery *CFDB_GetRoleByNameAuth(const char *user_name, const char *role_name);

const char *HubSettingToString(HubSetting setting);
HubSetting HubSettingFromString(const char *setting);

bool CFDB_GetSetting(EnterpriseDB *conn, HubSetting setting, char *value_out, size_t size);
bool CFDB_UpdateSetting(EnterpriseDB *conn, HubSetting setting, const char *value);

#endif
