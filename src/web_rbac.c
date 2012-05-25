/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

/*****************************************************************************/
/*                                                                           */
/* File: web_rbac.c                                                          */
/*                                                                           */
/*  Implements Role-Based Access control for the web API.                    */
/*  Roles are defined as                                                     */
/*   class regex to include                                                  */
/*   class regex to exclude (overrides include)                              */
/*   bundle regex to include                                                 */
/*   bundle regex to exclude (overrides include)                             */
/*   (see HubRole)                                                           */
/*                                                                           */
/*  Currently, reports are filtered by class regexes only (on hosts),        */
/*  while promise definitions are filtered on bundle regex only              */
/*                                                                           */
/*****************************************************************************/

#include "web_rbac.h"

#include "db_query.h"
#include "files_names.h"
#include "rlist.h"
#include "item_lib.h"

#include <assert.h>

typedef enum
{
    AUTHENTICATION_MODE_INTERNAL,
    AUTHENTICATION_MODE_LDAP,
    AUTHENTICATION_MODE_AD
} AuthenticationMode;

static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles);
static char *StringAppendRealloc2(char *start, char *append1, char *append2);
static bool RoleExists(char *name);
static void DeAssociateUsersFromRole(EnterpriseDB *conn, char *roleName);
static Item *CFDB_GetRolesForUser(EnterpriseDB *conn, char *userName);
static HubQuery *CFDB_GetRolesByMultipleNames(Item *names);
static HubQuery *CFDB_GetRoles(bson *query);
static const char *GetUsersCollection(EnterpriseDB *conn);
static AuthenticationMode GetAuthenticationMode(EnterpriseDB *conn);
static bool IsRBACOn(EnterpriseDB *conn);
static HubQuery *CFDB_GetAllRoles(void);
static HubQuery *CFDB_GetRoleByName(char *name);
static cfapi_errid UserIsRoleAdmin(EnterpriseDB *conn, char *userName);

/*****************************************************************************/

static char *SHA1Hash(const char *string, int len)
{
    unsigned char digest[EVP_MAX_MD_SIZE + 1];

    HashString(string, len, digest, cf_sha1);

    char *buffer = xcalloc(EVP_MAX_MD_SIZE * 4, sizeof(char));

    HashPrintSafe(cf_sha1, digest, buffer);
    return buffer;
}

/*****************************************************************************/

static bool VerifyPasswordIonAuth(const char *password, size_t password_len, const char *db_password)
{
    static const int SALT_LENGTH = 10;
    static const size_t SHA1_LENGTH = 40;

    char *salt = StringSubstring(db_password, SHA1_LENGTH, 0, SALT_LENGTH);
    char *salt_password = StringConcatenate(salt, SALT_LENGTH, password, password_len);
    char *salt_password_hashed = SHA1Hash(salt_password, SALT_LENGTH + password_len);
    char *salt_password_hashed_shifted = StringSubstring(salt_password_hashed + 4, SHA1_LENGTH, 0, -SALT_LENGTH);

    char *db_hash = StringConcatenate(salt, SALT_LENGTH, salt_password_hashed_shifted, SHA1_LENGTH - SALT_LENGTH + 1);

    free(salt);
    free(salt_password);
    free(salt_password_hashed);
    free(salt_password_hashed_shifted);

    bool authenticated = strcmp(db_password, db_hash) == 0;

    free(db_hash);
    return authenticated;
}

#ifdef HAVE_LIBLDAP

static cfapi_errid LDAPAuthenticatePlain(EnterpriseDB *conn,
                                         const char *ldap_url,
                                         bool start_tls,
                                         const char *username,
                                         const char *password,
                                         size_t password_len)
{
    char login_attribute[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_login_attribute, login_attribute, sizeof(login_attribute), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        strcpy(login_attribute, "uid");
    }

    char base_dn[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_base_dn, base_dn, sizeof(login_attribute), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }

    char user_directories[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_users_directory, user_directories, sizeof(user_directories), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        return ERRID_DATA_UNAVAILABLE;
    }

    cfapi_errid result = ERRID_RBAC_ACCESS_DENIED;
    Rlist *user_directory_values = SplitStringAsRList(user_directories, ';');
    for (const Rlist *rp = user_directory_values; rp; rp = rp->next)
    {
        const char *user_directory = ScalarValue(rp);

        char bind_dn[4096] = { 0 };
        strcat(bind_dn, login_attribute);
        strcat(bind_dn, "=");
        strcat(bind_dn, username);
        strcat(bind_dn, ",");
        strcat(bind_dn, user_directory);
        strcat(bind_dn, ",");
        strcat(bind_dn, base_dn);

        const char *errmsg = NULL;
        if (CfLDAPAuthenticate(ldap_url, bind_dn, password, start_tls, &errmsg))
        {
            result = ERRID_SUCCESS;
            break;
        }
    }
    DeleteRlist(user_directory_values);

    return result;
}

static cfapi_errid LDAPAuthenticateAD(EnterpriseDB *conn,
                                      const char *ldap_url,
                                      bool start_tls,
                                      const char *username,
                                      const char *password,
                                      size_t password_len)
{
    char ad_domain[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_ad_domain, ad_domain, sizeof(ad_domain), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }

    char bind_dn[4096] = { 0 };
    if (StringMatch("/^(\\w+\\.)+\\w{2,5}$/", ad_domain))
    {
        strcat(bind_dn, username);
        strcat(bind_dn, "@");
        strcat(bind_dn, ad_domain);
    }
    else
    {
        strcat(bind_dn, ad_domain);
        strcat(bind_dn, "\\");
        strcat(bind_dn, username);
    }

    const char *errmsg = NULL;
    if (CfLDAPAuthenticate(ldap_url, bind_dn, password, start_tls, &errmsg))
    {
        return ERRID_SUCCESS;
    }
    else
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }
}

static cfapi_errid LDAPAuthenticate(EnterpriseDB *conn, const char *username, const char *password, size_t password_len,
                                    bool active_directory)
{
    char encryption[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_encryption, encryption, sizeof(encryption), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        strcpy(encryption, "plain");
    }

    char ldap_server[1024] = { 0 };
    if (!CFDB_HandleGetValue(dbkey_mpsettings_host, ldap_server, sizeof(ldap_server), conn, MONGO_MPSETTINGS_COLLECTION))
    {
        return ERRID_HOST_NOT_FOUND;
    }

    char ldap_url[2048] = { 0 };
    if (strcmp(encryption, "ssl") == 0)
    {
        strcat(ldap_url, "ldaps://");
    }
    else
    {
        strcat(ldap_url, "ldap://");
    }
    strcat(ldap_url, ldap_server);

    bool start_tls = false;
    if (strcmp(encryption, "start_tls") == 0)
    {
        start_tls = true;
    }

    if (active_directory)
    {
        return LDAPAuthenticateAD(conn, ldap_url, start_tls, username, password, password_len);
    }
    else
    {
        return LDAPAuthenticatePlain(conn, ldap_url, start_tls, username, password, password_len);
    }
}
#else
static cfapi_errid LDAPAuthenticate(EnterpriseDB *conn, const char *username, const char *password, size_t password_len,
                                    bool active_directory)
{
    assert(false && "Tried to authenticate using LDAP on a non-LDAP build");
    return ERRID_RBAC_ACCESS_DENIED;
}

#endif

static cfapi_errid IonAuthenticate(EnterpriseDB *conn, const char *username, const char *password, size_t password_len)
{
    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, username);
    bson_append_int(&query, dbkey_user_active, 1);
    bson_finish(&query);

    bson field;

    bson_init(&field);
    bson_append_int(&field, dbkey_user_password, 1);
    bson_finish(&field);

    bson record;
    bson_bool_t found = mongo_find_one(conn, GetUsersCollection(conn), &query, &field, &record);

    bson_destroy(&query);
    bson_destroy(&field);

    if (found)
    {
        const char *db_password = NULL;
        BsonStringGet(&record, dbkey_user_password, &db_password);

        bson_destroy(&record);

        assert(db_password);

        if (db_password)
        {
            if (VerifyPasswordIonAuth(password, password_len, db_password))
            {
                return ERRID_SUCCESS;
            }
            else
            {
                return ERRID_RBAC_ACCESS_DENIED;
            }
        }
    }

    return ERRID_RBAC_ACCESS_DENIED;

}

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len)
{
    EnterpriseDB conn;
    if (!CFDB_Open(&conn))
    {
        return ERRID_DBCONNECT;
    }

    cfapi_errid result = ERRID_RBAC_ACCESS_DENIED;

    switch (GetAuthenticationMode(&conn))
    {
    case AUTHENTICATION_MODE_LDAP:
        result = LDAPAuthenticate(&conn, username, password, password_len, false);
        break;

    case AUTHENTICATION_MODE_AD:
        result = LDAPAuthenticate(&conn, username, password, password_len, true);
        break;

    case AUTHENTICATION_MODE_INTERNAL:
        result = IonAuthenticate(&conn, username, password, password_len);
        break;
    }

    CFDB_Close(&conn);
    return result;
}

/*****************************************************************************/

HubQuery *CFDB_HostClassFilterFromUserRBAC(char *userName)
{
    Rlist *recordList = NULL;

    HubQuery *hqRBAC = CFDB_GetRBACForUser(userName);
    cfapi_errid errid = hqRBAC->errid;

    if (errid != ERRID_SUCCESS)
    {
        DeleteHubQuery(hqRBAC, DeleteHubUserRBAC);
        PrependRlistAlien(&(recordList), NewHostClassFilter(NULL, NULL));
        return NewHubQueryErrid(NULL, recordList, errid);
    }

    HubUserRBAC *rbac = HubQueryGetFirstRecord(hqRBAC);

    HostClassFilter *hostClassFilter = NewHostClassFilter(rbac->classRxInclude, rbac->classRxExclude);

    PrependRlistAlien(&(recordList), hostClassFilter);

    DeleteHubQuery(hqRBAC, DeleteHubUserRBAC);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

HubQuery *CFDB_PromiseFilterFromUserRBAC(char *userName)
{
    Rlist *recordList = NULL;

    HubQuery *hqRBAC = CFDB_GetRBACForUser(userName);
    cfapi_errid errid = hqRBAC->errid;

    if (errid != ERRID_SUCCESS)
    {
        DeleteHubQuery(hqRBAC, DeleteHubUserRBAC);
        PrependRlistAlien(&(recordList), NewPromiseFilter());
        return NewHubQueryErrid(NULL, recordList, errid);
    }

    HubUserRBAC *rbac = HubQueryGetFirstRecord(hqRBAC);

    PromiseFilter *promiseFilter = NewPromiseFilter();

    PromiseFilterAddBundlesRx(promiseFilter, rbac->bundleRxInclude, rbac->bundleRxExclude);
    PrependRlistAlien(&(recordList), promiseFilter);

    DeleteHubQuery(hqRBAC, DeleteHubUserRBAC);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

cfapi_errid CFDB_HasHostAccessFromUserRBAC(char *userName, char *hostKey)
/**
 * Convenience function in cases where we are checking for user access
 * to a specific host, as opposed to creating a filter to query reports from
 * some of many hosts.
 **/
{
    assert(SafeStringLength(userName) > 0);
    assert(SafeStringLength(hostKey) > 0);

    HubQuery *hqFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    if (hqFilter->errid == ERRID_RBAC_DISABLED)
    {
        DeleteHubQuery(hqFilter, DeleteHostClassFilter);
        return ERRID_SUCCESS;
    }

    if (hqFilter->errid != ERRID_SUCCESS)
    {
        cfapi_errid errid = hqFilter->errid;

        DeleteHubQuery(hqFilter, DeleteHostClassFilter);
        return errid;
    }

    HostClassFilter *filter = HubQueryGetFirstRecord(hqFilter);

    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        DeleteHubQuery(hqFilter, DeleteHostClassFilter);
        return ERRID_DBCONNECT;
    }

    bool hostAccessForUser = CFDB_HasMatchingHost(&conn, hostKey, filter);

    DeleteHubQuery(hqFilter, DeleteHostClassFilter);

    if (!CFDB_Close(&conn))
    {
        return ERRID_DBCLOSE;
    }

    if (hostAccessForUser)
    {
        return ERRID_SUCCESS;
    }
    else
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }
}

/*****************************************************************************/

HubQuery *CFDB_GetRBACForUser(char *userName)
/*
 * Looks up the roles of the given user, and generates
 * the union of the RBAC permissions of these roles.
 */
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    if (!IsRBACOn(&conn))
    {
        CFDB_Close(&conn);
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_DISABLED);
    }

    Item *roleNames = CFDB_GetRolesForUser(&conn, userName);

    CFDB_Close(&conn);

    if (!roleNames)
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }

    HubQuery *hqRoles = CFDB_GetRolesByMultipleNames(roleNames);

    DeleteItemList(roleNames);

    if (hqRoles->errid != ERRID_SUCCESS)
    {
        HubQuery *hq = NewHubQueryErrid(NULL, NULL, hqRoles->errid);

        DeleteHubQuery(hqRoles, DeleteHubRole);
        return hq;
    }

    if (hqRoles->records == NULL)
    {
        // bad: missing role, but user refers to it (missing foreign key)
        HubQuery *hq = NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);

        DeleteHubQuery(hqRoles, DeleteHubRole);
        return hq;
    }

    HubQuery *hqUserRBAC = CombineAccessOfRoles(userName, hqRoles);

    DeleteHubQuery(hqRoles, DeleteHubRole);

    return hqUserRBAC;
}

/*****************************************************************************/

static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles)
{
    char *combinedClassRxInclude = NULL;
    char *combinedClassRxExclude = NULL;
    char *combinedBundleRxInclude = NULL;
    char *combinedBundleRxExclude = NULL;

    if (hqRoles->records == NULL)
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }

    for (Rlist *rp = hqRoles->records; rp != NULL; rp = rp->next)
    {
        HubRole *role = (HubRole *) rp->item;

        // absolute correctness outweighs efficiency here
        combinedClassRxInclude = StringAppendRealloc2(combinedClassRxInclude, role->classRxInclude, "|");
        combinedClassRxExclude = StringAppendRealloc2(combinedClassRxExclude, role->classRxExclude, "|");
        combinedBundleRxInclude = StringAppendRealloc2(combinedBundleRxInclude, role->bundleRxInclude, "|");
        combinedBundleRxExclude = StringAppendRealloc2(combinedBundleRxExclude, role->bundleRxExclude, "|");
    }

    ReplaceTrailingChar(combinedClassRxInclude, '|', '\0');
    ReplaceTrailingChar(combinedClassRxExclude, '|', '\0');
    ReplaceTrailingChar(combinedBundleRxInclude, '|', '\0');
    ReplaceTrailingChar(combinedBundleRxExclude, '|', '\0');

    HubUserRBAC *rbac = NewHubUserRBAC(userName, combinedClassRxInclude, combinedClassRxExclude,
                                       combinedBundleRxInclude, combinedBundleRxExclude);

    free(combinedClassRxInclude);
    free(combinedClassRxExclude);
    free(combinedBundleRxInclude);
    free(combinedBundleRxExclude);

    Rlist *recordList = NULL;

    PrependRlistAlien(&(recordList), rbac);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

static char *StringAppendRealloc2(char *start, char *append1, char *append2)
{
    if (SafeStringLength(append1) == 0 || SafeStringLength(append2) == 0)
    {
        return start;
    }

    int startLen = SafeStringLength(start);

    start = xrealloc(start, startLen + SafeStringLength(append1) + SafeStringLength(append2) + 1);

    if (startLen == 0)
    {
        start[0] = '\0';
    }

    strcat(start, append1);
    strcat(start, append2);

    return start;
}

/*****************************************************************************/

cfapi_errid CFDB_CreateRole(char *creatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return ERRID_DBCONNECT;
    }

    cfapi_errid errid = UserIsRoleAdmin(&conn, creatingUser);

    if (errid != ERRID_SUCCESS)
    {
        CFDB_Close(&conn);
        return errid;
    }

    if (RoleExists(roleName))
    {
        CFDB_Close(&conn);
        return ERRID_ITEM_EXISTS;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_role_name, roleName);
    bson_finish(&query);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        bson_append_string(&set_op, dbkey_role_description, description);

        if (includeClassRx)
        {
            bson_append_string(&set_op, dbkey_role_classrx_include, includeClassRx);
        }
        if (excludeClassRx)
        {
            bson_append_string(&set_op, dbkey_role_classrx_exclude, excludeClassRx);
        }
        if (includeBundleRx)
        {
            bson_append_string(&set_op, dbkey_role_bundlerx_include, includeBundleRx);
        }
        if (excludeBundleRx)
        {
            bson_append_string(&set_op, dbkey_role_bundlerx_exclude, excludeBundleRx);
        }

        bson_append_finish_object(&set_op);
    }
    bson_finish(&set_op);

    mongo_update(&conn, MONGO_ROLES_COLLECTION, &query, &set_op, MONGO_UPDATE_UPSERT);

    bson_destroy(&query);
    bson_destroy(&set_op);

    if (!MongoCheckForError(&conn, "CFDB_DeleteRole", NULL, false))
    {
        errid = ERRID_DB_OPERATION;
    }

    CFDB_Close(&conn);

    return errid;
}

/*****************************************************************************/

cfapi_errid CFDB_DeleteRole(char *deletingUser, char *roleName, bool deassociateUsers)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return ERRID_DBCONNECT;
    }

    cfapi_errid errid = UserIsRoleAdmin(&conn, deletingUser);

    if (errid != ERRID_SUCCESS)
    {
        CFDB_Close(&conn);
        return errid;
    }

    if (!RoleExists(roleName))
    {
        CFDB_Close(&conn);
        return ERRID_ITEM_NONEXISTING;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_role_name, roleName);
    bson_finish(&query);

    mongo_remove(&conn, MONGO_ROLES_COLLECTION, &query);
    bson_destroy(&query);

    if (deassociateUsers)
    {
        DeAssociateUsersFromRole(&conn, roleName);
    }

    if (!MongoCheckForError(&conn, "CFDB_DeleteRole", NULL, false))
    {
        errid = ERRID_DB_OPERATION;
    }

    CFDB_Close(&conn);

    return errid;
}

/*****************************************************************************/

cfapi_errid CFDB_UpdateRole(char *updatingUser, char *roleName, char *description,
                            char *includeClassRx, char *excludeClassRx, char *includeBundleRx, char *excludeBundleRx)
{
    cfapi_errid errid = CFDB_DeleteRole(updatingUser, roleName, false);

    if (errid != ERRID_SUCCESS)
    {
        return errid;
    }

    return CFDB_CreateRole(updatingUser, roleName, description, includeClassRx, excludeClassRx, includeBundleRx,
                           excludeBundleRx);
}

/*****************************************************************************/

cfapi_errid CFDB_UserIsAdminWhenRBAC(char *username)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return ERRID_DBCONNECT;
    }

    cfapi_errid errid;

    if (IsRBACOn(&conn))
    {
        errid = UserIsRoleAdmin(&conn, username);
    }
    else
    {
        errid = ERRID_SUCCESS;
    }

    if (!CFDB_Close(&conn))
    {
        errid = ERRID_DBCLOSE;
    }

    return errid;
}

/*****************************************************************************/

static bool RoleExists(char *name)
{
    HubQuery *hq = CFDB_GetRoleByName(name);
    bool exists = (hq->records == NULL) ? false : true;

    DeleteHubQuery(hq, DeleteHubRole);

    return exists;
}

/*****************************************************************************/

static void DeAssociateUsersFromRole(EnterpriseDB *conn, char *roleName)
{
    const char *usersCollection = GetUsersCollection(conn);

    bson query;

    bson_empty(&query);

    bson pull_op;

    bson_init(&pull_op);
    {
        bson_append_start_object(&pull_op, "$pull");

        bson_append_string(&pull_op, dbkey_user_roles, roleName);
        bson_append_finish_object(&pull_op);
    }
    bson_finish(&pull_op);

    mongo_update(conn, usersCollection, &query, &pull_op, MONGO_UPDATE_MULTI);
    bson_destroy(&pull_op);
}

/*****************************************************************************/

static const char *GetUsersCollection(EnterpriseDB *conn)
{
    switch (GetAuthenticationMode(conn))
    {
    case AUTHENTICATION_MODE_LDAP:
    case AUTHENTICATION_MODE_AD:
        return MONGO_USERS_LDAP_COLLECTION;

    default:
    case AUTHENTICATION_MODE_INTERNAL:
        return MONGO_USERS_INTERNAL_COLLECTION;
    }
}

/*****************************************************************************/

static AuthenticationMode GetAuthenticationMode(EnterpriseDB *conn)
{
    char result[32] = { 0 };

    CFDB_HandleGetValue(dbkey_mpsettings_auth_mode, result, sizeof(result), conn, MONGO_MPSETTINGS_COLLECTION);

    if (StringSafeEqual(result, "ldap"))
    {
        return AUTHENTICATION_MODE_LDAP;
    }
    else if (StringSafeEqual(result, "active_directory"))
    {
        return AUTHENTICATION_MODE_LDAP;
    }
    else
    {
        return AUTHENTICATION_MODE_INTERNAL;
    }
}

/*****************************************************************************/

static bool IsRBACOn(EnterpriseDB *conn)
{
    char result[8] = { 0 };

    CFDB_HandleGetValue(dbkey_mpsettings_rbac, result, sizeof(result), conn, MONGO_MPSETTINGS_COLLECTION);

    if (strcmp(result, "false") == 0)
    {
        return false;
    }

    return true;                // safe default
}

/*****************************************************************************/

static Item *CFDB_GetRolesForUser(EnterpriseDB *conn, char *userName)
{
    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, userName);
    bson_finish(&query);

    bson field;

    bson_init(&field);
    bson_append_int(&field, dbkey_user_roles, 1);
    bson_finish(&field);

    const char *usersCollection = GetUsersCollection(conn);

    mongo_cursor *cursor = mongo_find(conn, usersCollection, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    Item *memberRoles = NULL;

    if (mongo_cursor_next(cursor))
    {
        memberRoles = BsonGetStringArrayAsItemList(&(cursor->current), dbkey_user_roles);
    }

    mongo_cursor_destroy(cursor);

    return memberRoles;
}

/*****************************************************************************/

HubQuery *CFDB_GetAllRolesAuth(char *userName)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    cfapi_errid errid = UserIsRoleAdmin(&conn, userName);

    CFDB_Close(&conn);

    if (errid != ERRID_SUCCESS)
    {
        return NewHubQueryErrid(NULL, NULL, errid);
    }

    return CFDB_GetAllRoles();
}

/*****************************************************************************/

static HubQuery *CFDB_GetAllRoles(void)
{    
    bson query;

    bson_init(&query);
    BsonAppendSortField(&query, dbkey_role_name);
    bson_finish(&query);
    
    HubQuery *hq = CFDB_GetRoles(&query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_GetRoleByNameAuth(char *userName, char *roleName)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    cfapi_errid errid = UserIsRoleAdmin(&conn, userName);

    CFDB_Close(&conn);

    if (errid != ERRID_SUCCESS)
    {
        return NewHubQueryErrid(NULL, NULL, errid);
    }

    return CFDB_GetRoleByName(roleName);
}

/*****************************************************************************/

static HubQuery *CFDB_GetRoleByName(char *name)
{
    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_role_name, name);
    bson_finish(&query);

    HubQuery *hq = CFDB_GetRoles(&query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_GetRolesByMultipleNames(Item *names)
{
    if (!names)
    {
        NewHubQueryErrid(NULL, NULL, ERRID_ARGUMENT_WRONG);
    }

    bson query;

    bson_init(&query);
    {
        bson_append_start_array(&query, "$or");

        int i = 0;
        char iStr[64];

        for (Item *ip = names; ip != NULL; ip = ip->next, i++)
        {
            snprintf(iStr, sizeof(iStr), "%d", i);
            {
                bson_append_start_object(&query, iStr);

                bson_append_string(&query, dbkey_role_name, ip->name);
                bson_append_finish_object(&query);
            }
        }
        bson_append_finish_object(&query);
    }
    bson_finish(&query);

    HubQuery *hq = CFDB_GetRoles(&query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_GetRoles(bson *query)
{
    Rlist *recordList = NULL;
    HubQuery *hq = NewHubQuery(NULL, recordList);

    EnterpriseDB conn;

    bson fields;

    bson_init(&fields);
    bson_append_int(&fields, dbkey_role_name, 1);
    bson_append_int(&fields, dbkey_role_description, 1);
    bson_append_int(&fields, dbkey_role_classrx_include, 1);
    bson_append_int(&fields, dbkey_role_classrx_exclude, 1);
    bson_append_int(&fields, dbkey_role_bundlerx_include, 1);
    bson_append_int(&fields, dbkey_role_bundlerx_exclude, 1);
    bson_finish(&fields);

    if (!CFDB_Open(&conn))
    {
        hq->errid = ERRID_DBCONNECT;
        bson_destroy(&fields);
        return hq;
    }

    mongo_cursor *cursor = mongo_find(&conn, MONGO_ROLES_COLLECTION, query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    CFDB_Close(&conn);

    while (mongo_cursor_next(cursor))
    {
	char name[CF_MAXVARSIZE], desc[CF_MAXVARSIZE], clRxIncl[CF_MAXVARSIZE],
	    clRxExcl[CF_MAXVARSIZE], bRxIncl[CF_MAXVARSIZE], bRxExcl[CF_MAXVARSIZE];
	
	BsonStringWrite(name, sizeof(name), &(cursor->current), dbkey_role_name);
	BsonStringWrite(desc, sizeof(desc), &(cursor->current), dbkey_role_description);
	BsonStringWrite(clRxIncl, sizeof(clRxIncl), &(cursor->current), dbkey_role_classrx_include);
	BsonStringWrite(clRxExcl, sizeof(clRxExcl), &(cursor->current), dbkey_role_classrx_exclude);
	BsonStringWrite(bRxIncl, sizeof(bRxIncl), &(cursor->current), dbkey_role_bundlerx_include);
	BsonStringWrite(bRxExcl, sizeof(bRxExcl), &(cursor->current), dbkey_role_bundlerx_exclude);

        PrependRlistAlien(&(hq->records), NewHubRole(name, desc, clRxIncl, clRxExcl, bRxIncl, bRxExcl));
    }

    mongo_cursor_destroy(cursor);

    return hq;
}

/*****************************************************************************/

static cfapi_errid UserIsRoleAdmin(EnterpriseDB *conn, char *userName)
{
# define ROLE_NAME_ADMIN "admin"

    Item *roleNames = CFDB_GetRolesForUser(conn, userName);

    if (!roleNames)
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }

    cfapi_errid errid = ERRID_SUCCESS;

    if (!IsItemIn(roleNames, ROLE_NAME_ADMIN))
    {
        errid = ERRID_RBAC_ACCESS_DENIED;
    }

    DeleteItemList(roleNames);

    return errid;
}
