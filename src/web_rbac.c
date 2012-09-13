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
#include "db_save.h"
#include "files_names.h"
#include "rlist.h"
#include "item_lib.h"

#include <assert.h>

# define MONGO_ROLES_COLLECTION MONGO_BASE ".roles"
# define MONGO_COLLECTION_USERS_INTERNAL MONGO_BASE ".users"
# define MONGO_COLLECTION_USERS_EXTERNAL MONGO_BASE ".users.external"

# define dbkey_user_name "username"
# define dbkey_user_password "password"
# define dbkey_user_salt "salt"
# define dbkey_user_roles "roles"
# define dbkey_user_email "email"

# define dbkey_role_name "name"
# define dbkey_role_description "description"
# define dbkey_role_classrx_include "crxi"
# define dbkey_role_classrx_exclude "crxx"
# define dbkey_role_bundlerx_include "brxi"
# define dbkey_role_bundlerx_exclude "brxx"

# define SALT_SIZE 10

static const char *_EXTERNAL_FILTER_LDAP = "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson)(mail=*))";
static const char *_EXTERNAL_FILTER_AD = "(&(objectCategory=person)(objectClass=user)(cn=*)(sAMAccountName=*))";

static time_t _EXTERNAL_AUTHENTICATE_TIMEOUT_SECONDS = 5;

static HubQuery *CombineAccessOfRoles(const char *userName, HubQuery *hqRoles);
static char *StringAppendRealloc2(char *start, char *append1, char *append2);
static bool _UsernameExistsExternal(const HubSettingsLDAP *ldap_settings, const char *username);
static void DeAssociateUsersFromRole(EnterpriseDB *conn, const char *roleName);
static bool RoleExists(const char *name);
static cfapi_errid _GetUserRecord(EnterpriseDB *conn, bool external, const char *username, HubUser **user_out);
static HubQuery *CFDB_GetRolesByMultipleNames(const Rlist *names);
static HubQuery *CFDB_GetRoles(bson *query);
static HubQuery *CFDB_GetAllRoles(void);
static HubQuery *CFDB_GetRoleByName(const char *name);
static bool _UserIsAdmin(EnterpriseDB *conn, bool ldap_enabled, const char *username);
static HubQuery *CFDB_GetRBACForUser(const char *userName);
cfapi_errid _UpdateUser(EnterpriseDB *conn, bool external, const char *username, const char *password, const char *email, const Sequence *roles);
static cfapi_errid _AuthenticateExternal(const HubSettingsLDAP *ldap_settings, const char *username, const char *password);
static char *_LDAPUri(const HubSettingsLDAP *ldap_settings);

/*****************************************************************************/

static char *SHAHash(const char *string, int len)
{
    unsigned char digest[EVP_MAX_MD_SIZE + 1] = { 0 };

    HashString(string, len, digest, cf_sha256);

    char *buffer = xcalloc(EVP_MAX_MD_SIZE * 4, sizeof(char));

    HashPrintSafe(cf_sha256, digest, buffer);
    return buffer;
}

static char *GenerateSalt()
{
    unsigned char buffer[SALT_SIZE] = { 0 };

    RAND_bytes(buffer, SALT_SIZE * sizeof(char));
    char *base64 = StringEncodeBase64(buffer, SALT_SIZE);

    // base64 encoded version will have length at least SALT_SIZE
    char *salt = StringSubstring(base64, strlen(base64), 0, SALT_SIZE);

    free(base64);
    return salt;
}

static char *HashPassword(const char *clear_password, size_t clear_password_len, const char *db_salt)
{
    char *salt_password = StringConcatenate(2, db_salt, clear_password);
    char *salt_password_hashed = SHAHash(salt_password, SALT_SIZE + clear_password_len);

    free(salt_password);

    return salt_password_hashed;
}

/*****************************************************************************/

static bool VerifyPasswordInternal(const char *clear_password, const char *db_password, const char *db_salt)
{
    size_t clear_password_len = SafeStringLength(clear_password);
    if (clear_password_len == 0 || clear_password_len > 100)
    {
        return false;
    }

    char *salt_password_hashed = HashPassword(clear_password, clear_password_len, db_salt);

    bool authenticated = strcmp(db_password, salt_password_hashed) == 0;

    free(salt_password_hashed);

    return authenticated;
}

#ifdef HAVE_LIBLDAP

static cfapi_errid _LDAPAuthenticate(const char *url,
                                     const char *login_attribute,
                                     const char *base_dn,
                                     const char *user_directories,
                                     bool start_tls,
                                     const char *username,
                                     const char *password)
{

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
        if (CfLDAPAuthenticate(url, bind_dn, password, "sasl", start_tls,
                               _EXTERNAL_AUTHENTICATE_TIMEOUT_SECONDS,  &errmsg))
        {
            result = ERRID_SUCCESS;
            break;
        }
    }
    DeleteRlist(user_directory_values);

    return result;
}

static cfapi_errid _LDAPAuthenticateAD(const char *url,
                                       const char *ad_domain,
                                       bool start_tls,
                                       const char *username,
                                       const char *password)
{
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
    if (CfLDAPAuthenticate(url, bind_dn, password, "sasl", start_tls,
                           _EXTERNAL_AUTHENTICATE_TIMEOUT_SECONDS, &errmsg))
    {
        return ERRID_SUCCESS;
    }
    else
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }
}

static cfapi_errid _AuthenticateExternal(const HubSettingsLDAP *ldap_settings, const char *username, const char *password)
{

    char url[2048] = { 0 };
    if (StringSafeEqual(ldap_settings->encryption, "ssl"))
    {
        snprintf(url, 2048, "ldaps://%s:%d", ldap_settings->host, ldap_settings->port_ssl);
    }
    else
    {
        snprintf(url, 2048, "ldap://%s:%d", ldap_settings->host, ldap_settings->port);
    }

    bool start_tls = false;
    if (StringSafeEqual(ldap_settings->encryption, "tls"))
    {
        start_tls = true;
    }

    switch (ldap_settings->mode)
    {
    case LDAP_MODE_AD:
        return _LDAPAuthenticateAD(url, ldap_settings->ad_domain, start_tls, username, password);

    default:
    case LDAP_MODE_STANDARD:
        return _LDAPAuthenticate(url, ldap_settings->login_attribute, ldap_settings->base_dn, ldap_settings->users_directory, start_tls, username, password);
    }
}
#else
static cfapi_errid _AuthenticateExternal(const HubSettingsLDAP *ldap_settings, const char *username, const char *password)
{
    assert(false && "Tried to authenticate using LDAP on a non-LDAP build");
    return ERRID_RBAC_ACCESS_DENIED;
}

#endif

static bool _UserIsInRole(const HubUser *user, const char *rolename)
{
    assert(user);
    for (const Rlist *rp = user->roles; rp; rp = rp->next)
    {
        if (StringSafeEqual(rolename, ScalarValue(rp)))
        {
            return true;
        }
    }
    return false;
}

static bool _UserIsAdmin(EnterpriseDB *conn, bool ldap_enabled, const char *username)
{
    static const char *_ROLE_ADMIN = "admin";

    HubUser *user = NULL;
    if (_GetUserRecord(conn, false, username, &user) != ERRID_SUCCESS)
    {
        if (ldap_enabled)
        {
            if (_GetUserRecord(conn, false, username, &user) != ERRID_SUCCESS)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return user && _UserIsInRole(user, _ROLE_ADMIN);
}

static cfapi_errid _AuthenticateInternal(EnterpriseDB *conn, const char *username, const char *password)
{
    bson query;
    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, username);
    bson_finish(&query);

    bson field;
    BsonSelectReportFields(&field, 2,
                           dbkey_user_password,
                           dbkey_user_salt);

    bson record = { 0 };
    bson_bool_t found = MongoFindOne( conn, MONGO_COLLECTION_USERS_INTERNAL, &query, &field, &record ) == MONGO_OK;

    bson_destroy(&query);
    bson_destroy(&field);

    if (found)
    {
        const char *db_password = NULL;
        BsonStringGet(&record, dbkey_user_password, &db_password);
        assert(db_password);

        const char *db_salt = NULL;
        BsonStringGet(&record, dbkey_user_salt, &db_salt);
        assert(db_salt);

        if (db_password)
        {
            if (VerifyPasswordInternal(password, db_password, db_salt))
            {
                bson_destroy(&record);
                return ERRID_SUCCESS;
            }
            else
            {
                bson_destroy(&record);
                return ERRID_RBAC_ACCESS_DENIED;
            }
        }

        bson_destroy(&record);
    }

    return ERRID_RBAC_ACCESS_DENIED;
}

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, bool *authenticated_external)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    if (authenticated_external)
    {
        *authenticated_external = false;
    }

    cfapi_errid result = _AuthenticateInternal(conn, username, password);
    if (result != ERRID_SUCCESS)
    {
        HubSettings *settings = NULL;
        cfapi_errid err = CFDB_QuerySettings(conn, &settings);
        if (err != ERRID_SUCCESS)
        {
            CFDB_Close(conn);
            return ERRID_ACCESS_DENIED;
        }

        if (settings->ldap.enabled)
        {
            if (authenticated_external)
            {
                *authenticated_external = true;
            }


            result = _AuthenticateExternal(&settings->ldap, username, password);
        }

        DeleteHubSettings(settings);
    }

    CFDB_Close(conn);

    return result;
}

/*****************************************************************************/

HubQuery *CFDB_HostClassFilterFromUserRBAC(const char *userName)
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

cfapi_errid CFDB_HasHostAccessFromUserRBAC(const char *userName, const char *hostKey)
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

static HubQuery *CFDB_GetRBACForUser(const char *username)
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

    HubSettings *settings = NULL;
    CFDB_QuerySettings(&conn, &settings);

    if (settings->rbac_enabled == false)
    {
        CFDB_Close(&conn);
        DeleteHubSettings(settings);
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_DISABLED);
    }

    HubUser *user = NULL;
    cfapi_errid err = _GetUserRecord(&conn, false, username, &user);
    if (err != ERRID_SUCCESS && settings->ldap.enabled)
    {
        err = _GetUserRecord(&conn, true, username, &user);
    }

    DeleteHubSettings(settings);

    CFDB_Close(&conn);
    if (err != ERRID_SUCCESS)
    {
        DeleteHubUser(user);
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }
    assert(user);

    if (!user->roles)
    {
        DeleteHubUser(user);
        return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }

    HubQuery *hqRoles = CFDB_GetRolesByMultipleNames(user->roles);

    DeleteHubUser(user);

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

    HubQuery *hqUserRBAC = CombineAccessOfRoles(username, hqRoles);

    DeleteHubQuery(hqRoles, DeleteHubRole);

    return hqUserRBAC;
}

/*****************************************************************************/

static HubQuery *CombineAccessOfRoles(const char *userName, HubQuery *hqRoles)
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

cfapi_errid _CreateUser(EnterpriseDB *conn, bool external, const char *username, const char *password, const char *email, const Sequence *roles)
{
    const char *collection = external ? MONGO_COLLECTION_USERS_EXTERNAL : MONGO_COLLECTION_USERS_INTERNAL;

    if (_GetUserRecord(conn, username, collection, NULL) == ERRID_SUCCESS)
    {
        return ERRID_ITEM_EXISTS;
    }

    if (!username)
    {
        return ERRID_ARGUMENT_MISSING;
    }

    if (!external && !password)
    {
        return ERRID_ARGUMENT_MISSING;
    }

    return _UpdateUser(conn, external, username, password, email, roles);
}

cfapi_errid CFDB_CreateUser(const char *creating_username, const char *username, const char *password, const char *email, const Sequence *roles)
{    
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    if (!_UserIsAdmin(conn, settings, creating_username))
    {
        return ERRID_ACCESS_DENIED;
    }

    cfapi_errid result = _CreateUser(conn, false, username, password, email, roles);

    if (!CFDB_Close(conn))
    {
        return ERRID_DBCONNECT;
    }

    return result;
}

cfapi_errid _UpdateUser(EnterpriseDB *conn, bool external, const char *username, const char *password, const char *email,
                        const Sequence *roles)
{
    const char *users_collection = external ? MONGO_COLLECTION_USERS_EXTERNAL : MONGO_COLLECTION_USERS_INTERNAL;

    bson query;
    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, username);
    bson_finish(&query);

    bson set_op;
    bson_init(&set_op);
    bson_append_start_object(&set_op, "$set");
    {
        if (!NULL_OR_EMPTY(username))
        {
            bson_append_string(&set_op, dbkey_user_name, username);
        }

        if (!NULL_OR_EMPTY(password))
        {
            char *salt = GenerateSalt();
            assert(salt);
            char *hashed_password = HashPassword(password, strlen(password), salt);
            assert(hashed_password);

            bson_append_string(&set_op, dbkey_user_password, hashed_password);
            bson_append_string(&set_op, dbkey_user_salt, salt);

            free(salt);
            free(hashed_password);
        }

        if (roles)
        {
            BsonAppendStringArraySequence(&set_op, dbkey_user_roles, roles);
        }

        if (!NULL_OR_EMPTY(email))
        {
            bson_append_string(&set_op, dbkey_user_email, email);
        }
    }
    bson_append_finish_object(&set_op);
    bson_finish(&set_op);

    MongoUpdate(conn, users_collection, &query, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&query);
    bson_destroy(&set_op);

    cfapi_errid errid = ERRID_SUCCESS;
    bool updated = false;
    if (!MongoCheckForError(conn, "CFDB_UpdateUser", NULL, &updated))
    {
        errid = ERRID_DB_OPERATION;
    }

    return errid;
}

cfapi_errid CFDB_UpdateUser(const char *updating_username, const char *username, const char *password, const char *email,
                            const Sequence *roles)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    bool capable = false;
    if (_UserIsAdmin(conn, settings, updating_username))
    {
        capable = true;
    }
    else if (StringSafeEqual(updating_username, username) && !roles)
    {
        capable = true;
    }

    if (!capable)
    {
        DeleteHubSettings(settings);
        CFDB_Close(conn);
        return ERRID_ACCESS_DENIED;
    }

    cfapi_errid result = ERRID_UNKNOWN;
    if (_GetUserRecord(conn, false, username, NULL) == ERRID_SUCCESS)
    {
        result = _UpdateUser(conn, false, username, password, email, roles);
    }
    else if (settings->ldap.enabled)
    {
        if (password)
        {
            result = ERRID_ACCESS_DENIED_EXTERNAL;
        }
        else
        {
            if (_GetUserRecord(conn, false, username, NULL) == ERRID_SUCCESS)
            {
                result = _UpdateUser(conn, true, username, password, email, roles);
            }
            else if (_UsernameExistsExternal(&settings->ldap, username) == ERRID_SUCCESS)
            {
                result = _CreateUser(conn, true, username, password, email, roles);
            }
            else
            {
                result = ERRID_ITEM_NONEXISTING;
            }
        }
    }
    else
    {
        result = ERRID_ITEM_NONEXISTING;
    }

    DeleteHubSettings(settings);
    CFDB_Close(conn);
    return result;
}

cfapi_errid _DeleteUser(EnterpriseDB *conn, bool external, const char *username)
{
    const char *users_collection = external ? MONGO_COLLECTION_USERS_EXTERNAL : MONGO_COLLECTION_USERS_INTERNAL;

    bson query;
    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, username);
    bson_finish(&query);

    MongoRemove(conn, users_collection, &query, NULL);

    bson_destroy(&query);

    cfapi_errid errid = ERRID_SUCCESS;
    if (!MongoCheckForError(conn, "CFDB_DeleteUser", NULL, false))
    {
        errid = ERRID_DB_OPERATION;
    }
    return errid;
}

cfapi_errid CFDB_DeleteUser(const char *deleting_username, const char *username)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    if (!_UserIsAdmin(conn, settings, deleting_username))
    {
        return ERRID_ACCESS_DENIED;
    }

    cfapi_errid result = ERRID_UNKNOWN;
    if (_GetUserRecord(conn, false, username, NULL) == ERRID_SUCCESS)
    {
        result = _DeleteUser(conn, false, username);
    }
    else if (_GetUserRecord(conn, true, username, NULL) == ERRID_SUCCESS)
    {
        result = ERRID_ACCESS_DENIED_EXTERNAL;
    }
    else
    {
        result = ERRID_ITEM_NONEXISTING;
    }

    CFDB_Close(conn);
    return result;
}

static Rlist *_GetExternalUsernamesLdap(const HubSettingsLDAP *ldap_settings)
{
    Rlist *user_directories = SplitStringAsRList(ldap_settings->users_directory, ';');
    char *uri = _LDAPUri(ldap_settings);
    bool start_tls = StringSafeEqual("tls", ldap_settings->encryption);

    Rlist *result = NULL;
    for (const Rlist *rp = user_directories; rp; rp = rp->next)
    {
        const char *user_dir = ScalarValue(rp);
        char *dn = StringConcatenate(3, user_dir, ",", ldap_settings->base_dn);
        char *bind_dn = StringConcatenate(7, ldap_settings->login_attribute, "=", ldap_settings->username, ",", user_dir, ",", ldap_settings->base_dn);

        const char *errstr = NULL;
        Rlist *partial_result = CfLDAP_GetSingleAttributeList(ldap_settings->username, ldap_settings->password, uri, bind_dn, dn, _EXTERNAL_FILTER_LDAP,
                                                              ldap_settings->login_attribute, "subtree", "sasl",
                                                              start_tls, 0, 0, &errstr);

        free(dn);
        free(bind_dn);

        for (const Rlist *rp2 = partial_result; rp2; rp2 = rp2->next)
        {
            PrependRlist(&result, rp2->item, CF_SCALAR);
        }
        DeleteRlist(partial_result);
    }

    // TODO: signal error
    return result;
}

static Rlist *_GetExternalUsernamesAD(const HubSettingsLDAP *ldap_settings)
{
    char *dn = NULL;
    if (!ldap_settings->users_directory)
    {
        dn = SafeStringDuplicate(ldap_settings->base_dn);
    }
    else
    {
        dn = StringConcatenate(3, ldap_settings->users_directory, ",", ldap_settings->base_dn);
    }
    assert(dn);

    char *bind_dn = NULL;
    {
        if (StringMatchFull("(\\w+\\.)+\\w{2,5}", ldap_settings->ad_domain))
        {
            bind_dn = StringConcatenate(3, ldap_settings->username, "@", ldap_settings->ad_domain);
        }
        else
        {
            bind_dn = StringConcatenate(3, ldap_settings->ad_domain, "\\", ldap_settings->username);
        }
    }
    assert(bind_dn);

    char *uri = _LDAPUri(ldap_settings);
    bool start_tls = StringSafeEqual("tls", ldap_settings->encryption);

    const char *errstr = NULL;
    Rlist *result = CfLDAP_GetSingleAttributeList(ldap_settings->username, ldap_settings->password, uri, bind_dn, dn, _EXTERNAL_FILTER_AD, ldap_settings->login_attribute, "subtree", "sasl",
                                         start_tls, 0, 0, &errstr);

    free(dn);
    free(bind_dn);
    free(uri);

    if (!result)
    {
        // TODO: signal error
        return NULL;
    }

    return result;
}

static char *_LDAPUri(const HubSettingsLDAP *ldap_settings)
{    
    if (StringSafeEqual("ssl", ldap_settings->encryption))
    {
        char *port_str = StringFromLong(ldap_settings->port_ssl);
        char *ret = StringConcatenate(4, "ldaps://", ldap_settings->host, ":", port_str);
        free(port_str);
        return ret;
    }
    else
    {
        char *port_str = StringFromLong(ldap_settings->port);
        char *ret = StringConcatenate(4, "ldap://", ldap_settings->host, ":", port_str);
        free(port_str);
        return ret;
    }
}

static Rlist *_ListUsernamesExternal(const HubSettingsLDAP *ldap_settings)
{
    switch (ldap_settings->mode)
    {
    case LDAP_MODE_STANDARD:
        return _GetExternalUsernamesLdap(ldap_settings);
        break;

    case LDAP_MODE_AD:
        return _GetExternalUsernamesAD(ldap_settings);
        break;

    default:
        assert(false && "External authentication mode not supported");
        return NULL;
    }
}

cfapi_errid _ListUserRecords(EnterpriseDB *conn, bool external, const char *username_rx, Rlist **users_out)
{
    const char *users_collection = external ? MONGO_COLLECTION_USERS_EXTERNAL : MONGO_COLLECTION_USERS_INTERNAL;

    bson query;
    bson_init(&query);
    if (!NULL_OR_EMPTY(username_rx))
    {
        bson_append_regex(&query, dbkey_user_name, username_rx, "");
    }
    bson_finish(&query);

    bson field;
    BsonSelectReportFields(&field, 3,
                           dbkey_user_name,
                           dbkey_user_email,
                           dbkey_user_roles);

    mongo_cursor *cursor = MongoFind(conn, users_collection, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    *users_out = NULL;
    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        const char *username = NULL;
        BsonStringGet(&cursor->current, dbkey_user_name, &username);
        assert(username);

        const char *email = NULL;
        BsonStringGet(&cursor->current, dbkey_user_email, &email);

        Rlist *roles = BsonStringArrayAsRlist(&cursor->current, dbkey_user_roles);

        PrependRlistAlien(users_out, NewHubUser(external, username, email, roles));
    }

    return ERRID_SUCCESS;
}

static bool _UsernameExistsExternal(const HubSettingsLDAP *ldap_settings, const char *username)
{
    Rlist *external_usernames = _ListUsernamesExternal(ldap_settings);
    bool found = false;
    for (const Rlist *rp = external_usernames; rp; rp = rp->next)
    {
        const char *external_username = rp->item;
        if (StringSafeEqual(username, external_username))
        {
            found = true;
            break;
        }
    }
    DeleteRlist(external_usernames);

    return found;
}

static cfapi_errid _GetUserRecord(EnterpriseDB *conn, bool external, const char *username, HubUser **user_out)
{
    const char *users_collection = external ? MONGO_COLLECTION_USERS_EXTERNAL : MONGO_COLLECTION_USERS_INTERNAL;

    bson query;
    bson_init(&query);
    bson_append_string(&query, dbkey_user_name, username);
    bson_finish(&query);

    bson field;
    BsonSelectReportFields(&field, 2,
                           dbkey_user_email,
                           dbkey_user_roles);

    bson record = { 0 };
    if( MongoFindOne( conn, users_collection, &query, &field, &record ) != MONGO_OK )
    {
        bson_destroy(&query);
        bson_destroy(&field);
        return ERRID_ITEM_NONEXISTING;
    }

    bson_destroy(&query);
    bson_destroy(&field);

    if (user_out)
    {
        const char *email = NULL;
        BsonStringGet(&record, dbkey_user_email, &email);

        Rlist *roles = BsonStringArrayAsRlist(&record, dbkey_user_roles);

        *user_out = NewHubUser(external, username, email, roles);

        DeleteRlist(roles);
    }
    bson_destroy(&record);
    return ERRID_SUCCESS;
}

cfapi_errid CFDB_GetUser(const char *getting_username, const char *username, HubUser **user_out)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    bool capable = false;
    if (StringSafeEqual(getting_username, username))
    {
        capable = true;
    }
    else if (_UserIsAdmin(conn, settings, getting_username))
    {
        capable = true;
    }

    if (!capable)
    {
        CFDB_Close(conn);
        return ERRID_ACCESS_DENIED;
    }

    HubUser *user = NULL;
    cfapi_errid result = ERRID_UNKNOWN;
    if (_GetUserRecord(conn, false, username, &user) == ERRID_SUCCESS || _GetUserRecord(conn, true, username, &user) == ERRID_SUCCESS)
    {
        if (user_out)
        {
            *user_out = user;
        }
        else
        {
            DeleteHubUser(user);
        }
        result = ERRID_SUCCESS;
    }
    else if (settings->ldap.enabled && _UsernameExistsExternal(&settings->ldap, username))
    {
        if (user_out)
        {
            *user_out = NewHubUser(true, username, NULL, NULL);
        }
        result = ERRID_SUCCESS;
    }
    else
    {
        return ERRID_ITEM_NONEXISTING;
    }

    CFDB_Close(conn);
    return result;
}

HubQuery *_ListUsers(EnterpriseDB *conn, const HubSettings *settings, const char *username_rx)
{
    Rlist *internal_users = NULL;
    {
        cfapi_errid err = _ListUserRecords(conn, false, username_rx, &internal_users);
        if (err != ERRID_SUCCESS)
        {
            return NewHubQueryErrid(NULL, NULL, err);
        }
    }

    Rlist *external_users = NULL;
    if (settings->ldap.enabled)
    {
        {
            cfapi_errid err = _ListUserRecords(conn, true, username_rx, &external_users);
            if (err != ERRID_SUCCESS)
            {
                DeleteRlist(internal_users);
                return NewHubQueryErrid(NULL, NULL, err);
            }
        }

        // make empty records for all the external users we don't have on file
        {
            Rlist *external_usernames = _ListUsernamesExternal(&settings->ldap);
            for (const Rlist *rp = external_usernames; rp; rp = rp->next)
            {
                const char *external_username = ScalarValue(rp);
                bool have_record = false;
                for (const Rlist *rp2 = external_users; rp2; rp2 = rp2->next)
                {
                    const HubUser *external_user = rp2->item;
                    if (StringSafeEqual(external_user->username, external_username))
                    {
                        have_record = true;
                        break;
                    }
                }

                if (!have_record)
                {
                    PrependRlistAlien(&external_users, NewHubUser(true, external_username, NULL, NULL));
                }
            }
            DeleteRlist(external_usernames);
        }
    }

    Rlist *users = internal_users;
    if (!users)
    {
        users = external_users;
    }
    else
    {
        RlistLast(users)->next = external_users;
    }

    return NewHubQueryErrid(NULL, users, ERRID_SUCCESS);
}

HubQuery *CFDB_ListUsers(const char *listing_username, const char *username_rx)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    if (!_UserIsAdmin(conn, settings->ldap.enabled, listing_username))
    {
        CFDB_Close(conn);
        return NewHubQueryErrid(NULL, NULL, ERRID_ACCESS_DENIED);
    }

    HubQuery *result = _ListUsers(conn, settings, username_rx);

    DeleteHubSettings(settings);
    CFDB_Close(conn);
    return result;
}


//*****************************************************************************

cfapi_errid CFDB_CreateRole(const char *creatingUser, const char *roleName, const char *description,
                            const char *includeClassRx, const char *excludeClassRx,
                            const char *includeBundleRx, const char *excludeBundleRx)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    if (!_UserIsAdmin(conn, settings->ldap.enabled, creatingUser))
    {
        CFDB_Close(conn);
        return ERRID_ACCESS_DENIED;
    }

    if (RoleExists(roleName))
    {
        CFDB_Close(conn);
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

    MongoUpdate(conn, MONGO_ROLES_COLLECTION, &query, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&query);
    bson_destroy(&set_op);

    cfapi_errid errid = ERRID_SUCCESS;
    if (!MongoCheckForError(conn, "CFDB_DeleteRole", NULL, false))
    {
        errid = ERRID_DB_OPERATION;
    }

    CFDB_Close(conn);

    return errid;
}

/*****************************************************************************/

cfapi_errid CFDB_DeleteRole(const char *deletingUser, const char *roleName, bool deassociateUsers)
{
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    if (!_UserIsAdmin(conn, settings, deletingUser))
    {
        CFDB_Close(conn);
        return ERRID_ACCESS_DENIED;
    }

    if (!RoleExists(roleName))
    {
        CFDB_Close(conn);
        return ERRID_ITEM_NONEXISTING;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, dbkey_role_name, roleName);
    bson_finish(&query);

    MongoRemove(conn, MONGO_ROLES_COLLECTION, &query, NULL);
    bson_destroy(&query);

    if (deassociateUsers)
    {
        DeAssociateUsersFromRole(conn, roleName);
    }

    cfapi_errid errid = ERRID_SUCCESS;
    if (!MongoCheckForError(conn, "CFDB_DeleteRole", NULL, false))
    {
        errid = ERRID_DB_OPERATION;
    }

    CFDB_Close(conn);

    return errid;
}

/*****************************************************************************/

cfapi_errid CFDB_UpdateRole(const char *updatingUser, const char *roleName, const char *description,
                            const char *includeClassRx, const char *excludeClassRx,
                            const char *includeBundleRx, const char *excludeBundleRx)
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

cfapi_errid CFDB_UserIsAdminWhenRBAC(const char *username)
{
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    cfapi_errid errid;

    if (settings->rbac_enabled)
    {
        errid = _UserIsAdmin(conn, settings->ldap.enabled, username) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;
    }
    else
    {
        errid = ERRID_SUCCESS;
    }

    if (!CFDB_Close(conn))
    {
        errid = ERRID_DBCLOSE;
    }

    DeleteHubSettings(settings);
    return errid;
}

/*****************************************************************************/

static bool RoleExists(const char *name)
{
    HubQuery *hq = CFDB_GetRoleByName(name);
    bool exists = (hq->records == NULL) ? false : true;

    DeleteHubQuery(hq, DeleteHubRole);

    return exists;
}

/*****************************************************************************/

static void DeAssociateUsersFromRole(EnterpriseDB *conn, const char *roleName)
{
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

    MongoUpdate(conn, MONGO_COLLECTION_USERS_INTERNAL, &query, &pull_op, MONGO_UPDATE_MULTI, NULL);
    MongoUpdate(conn, MONGO_COLLECTION_USERS_EXTERNAL, &query, &pull_op, MONGO_UPDATE_MULTI, NULL);
    bson_destroy(&pull_op);
}

/*****************************************************************************/

HubQuery *CFDB_ListRoles(const char *listing_user)
{
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    cfapi_errid errid = _UserIsAdmin(conn, settings->ldap.enabled, listing_user) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;

    CFDB_Close(conn);

    if (errid != ERRID_SUCCESS)
    {
        return NewHubQueryErrid(NULL, NULL, errid);
    }

    DeleteHubSettings(settings);
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

HubQuery *CFDB_GetRoleByNameAuth(const char *username, const char *role_name)
{
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    HubSettings *settings = NULL;
    CFDB_QuerySettings(conn, &settings);

    cfapi_errid errid = _UserIsAdmin(conn, settings->ldap.enabled, username) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;

    CFDB_Close(conn);
    DeleteHubSettings(settings);

    if (errid != ERRID_SUCCESS)
    {
        return NewHubQueryErrid(NULL, NULL, errid);
    }

    return CFDB_GetRoleByName(role_name);
}

/*****************************************************************************/

static HubQuery *CFDB_GetRoleByName(const char *name)
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

HubQuery *CFDB_GetRolesByMultipleNames(const Rlist *names)
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

        for (const Rlist *rp = names; rp; rp = rp->next, i++)
        {
            snprintf(iStr, sizeof(iStr), "%d", i);
            {
                bson_append_start_object(&query, iStr);

                bson_append_string(&query, dbkey_role_name, ScalarValue(rp));
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

    mongo_cursor *cursor = MongoFind(&conn, MONGO_ROLES_COLLECTION, query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    CFDB_Close(&conn);

    while (mongo_cursor_next(cursor) == MONGO_OK)
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

bool HubSettingsValidate(const HubSettings *settings, char error_out[CF_BUFSIZE])
{
    if (settings->ldap.enabled)
    {
        if (_AuthenticateExternal(&settings->ldap, settings->ldap.username, settings->ldap.password) != ERRID_SUCCESS)
        {
            snprintf(error_out, CF_BUFSIZE, "Unable to authenticate with LDAP settings");
            return false;
        }
    }
    return true;
}

