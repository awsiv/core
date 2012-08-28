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
# define MONGO_SETTINGS_COLLECTION MONGO_BASE ".settings"

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

static const char *settingLabels[SETTING_MAX] =
{
    [SETTING_UNKNOWN] = "unknown",

    [SETTING_RBAC] = "rbac",
    [SETTING_LDAP_ENABLED] = "ldapEnabled",
    [SETTING_LDAP_MODE] = "ldapMode",
    [SETTING_LDAP_USERNAME] = "ldapUsername",
    [SETTING_LDAP_PASSWORD] = "ldapPassword",
    [SETTING_LDAP_ENCRYPTION] = "ldapEncryption",
    [SETTING_LDAP_LOGIN_ATTRIBUTE] = "ldapLoginAttribute",
    [SETTING_LDAP_BASE_DN] = "ldapBaseDN",
    [SETTING_LDAP_USERS_DIRECTORY] = "ldapUsersDirectory",
    [SETTING_LDAP_HOST] = "ldapHost",
    [SETTING_LDAP_PORT] = "ldapPort",
    [SETTING_LDAP_PORT_SSL] = "ldapPortSSL",
    [SETTING_LDAP_AD_DOMAIN] = "ldapActiveDirectoryDomain",
    [SETTING_BLUEHOST_HORIZON] = "blueHostHorizon",
};

static const JsonPrimitiveType setting_types[SETTING_MAX] =
{
    [SETTING_UNKNOWN] = JSON_PRIMITIVE_TYPE_NULL,

    [SETTING_RBAC] = JSON_PRIMITIVE_TYPE_BOOL,
    [SETTING_LDAP_ENABLED] = JSON_PRIMITIVE_TYPE_BOOL,
    [SETTING_LDAP_MODE] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_USERNAME] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_PASSWORD] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_ENCRYPTION] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_LOGIN_ATTRIBUTE] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_BASE_DN] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_USERS_DIRECTORY] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_HOST] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_LDAP_PORT] = JSON_PRIMITIVE_TYPE_INTEGER,
    [SETTING_LDAP_PORT_SSL] = JSON_PRIMITIVE_TYPE_INTEGER,
    [SETTING_LDAP_AD_DOMAIN] = JSON_PRIMITIVE_TYPE_STRING,
    [SETTING_BLUEHOST_HORIZON] = JSON_PRIMITIVE_TYPE_INTEGER
};

static const char *_EXTERNAL_FILTER_LDAP = "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson)(mail=*))";
static const char *_EXTERNAL_FILTER_AD = "(&(objectCategory=person)(objectClass=user)(cn=*)(sAMAccountName=*))";

static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles);
static char *StringAppendRealloc2(char *start, char *append1, char *append2);
static bool _UsernameExistsExternal(EnterpriseDB *conn, const char *username);
static void DeAssociateUsersFromRole(EnterpriseDB *conn, const char *roleName);
static bool RoleExists(const char *name);
static cfapi_errid _GetUserRecord(EnterpriseDB *conn, bool external, const char *username, HubUser **user_out);
static HubQuery *CFDB_GetRolesByMultipleNames(const Rlist *names);
static HubQuery *CFDB_GetRoles(bson *query);
static bool _IsLDAPEnabled(EnterpriseDB *conn);
static LDAPMode _LDAPMode(EnterpriseDB *conn);
static bool IsRBACOn(EnterpriseDB *conn);
static HubQuery *CFDB_GetAllRoles(void);
static HubQuery *CFDB_GetRoleByName(const char *name);
static bool _UserIsAdmin(EnterpriseDB *conn, const char *username);
static HubQuery *CFDB_GetRBACForUser(char *userName);
cfapi_errid _UpdateUser(EnterpriseDB *conn, bool external, const char *username, const char *password, const char *email, const Rlist *roles);
static cfapi_errid _AuthenticateExternal(EnterpriseDB *conn, const char *username, const char *password);

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

static cfapi_errid _LDAPAuthenticate(EnterpriseDB *conn,
                                     const char *ldap_url,
                                     bool start_tls,
                                     const char *username,
                                     const char *password)
{
    char login_attribute[1024] = { 0 };
    CFDB_GetSetting(conn, SETTING_LDAP_LOGIN_ATTRIBUTE, login_attribute, sizeof(login_attribute));

    char base_dn[1024] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_BASE_DN, base_dn, sizeof(base_dn)))
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }

    char user_directories[1024] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_USERS_DIRECTORY, user_directories, sizeof(user_directories)))
    {
        return ERRID_RBAC_ACCESS_DENIED;
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
        if (CfLDAPAuthenticate(ldap_url, bind_dn, password, "sasl", start_tls, &errmsg))
        {
            result = ERRID_SUCCESS;
            break;
        }
    }
    DeleteRlist(user_directory_values);

    return result;
}

static cfapi_errid _LDAPAuthenticateAD(EnterpriseDB *conn,
                                       const char *ldap_url,
                                       bool start_tls,
                                       const char *username,
                                       const char *password)
{
    char ad_domain[1024] = { 0 };
    if (!CFDB_HandleGetValue(settingLabels[SETTING_LDAP_AD_DOMAIN], ad_domain, sizeof(ad_domain), NULL, conn, MONGO_SETTINGS_COLLECTION))
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
    if (CfLDAPAuthenticate(ldap_url, bind_dn, password, "sasl", start_tls, &errmsg))
    {
        return ERRID_SUCCESS;
    }
    else
    {
        return ERRID_RBAC_ACCESS_DENIED;
    }
}

static cfapi_errid _AuthenticateExternal(EnterpriseDB *conn, const char *username, const char *password)
{
    char encryption[1024] = { 0 };
    CFDB_GetSetting(conn, SETTING_LDAP_ENCRYPTION, encryption, sizeof(encryption));

    char ldap_host[1024] = { 0 };
    CFDB_GetSetting(conn, SETTING_LDAP_HOST, ldap_host, sizeof(ldap_host));
    if (NULL_OR_EMPTY(ldap_host))
    {
        return ERRID_HOST_NOT_FOUND;
    }

    char ldap_url[2048] = { 0 };
    if (StringSafeEqual(encryption, "ssl"))
    {
        char ldap_port_ssl[1024] = { 0 };
        CFDB_GetSetting(conn, SETTING_LDAP_PORT_SSL, ldap_port_ssl, sizeof(ldap_port_ssl));

        snprintf(ldap_url, 2048, "ldaps://%s:%s", ldap_host, ldap_port_ssl);
    }
    else
    {
        char ldap_port[1024] = { 0 };
        CFDB_GetSetting(conn, SETTING_LDAP_PORT, ldap_port, sizeof(ldap_port));

        snprintf(ldap_url, 2048, "ldap://%s:%s", ldap_host, ldap_port);
    }

    bool start_tls = false;
    if (StringSafeEqual(encryption, "tls"))
    {
        start_tls = true;
    }

    switch (_LDAPMode(conn))
    {
    case LDAP_MODE_AD:
        return _LDAPAuthenticateAD(conn, ldap_url, start_tls, username, password);

    default:
    case LDAP_MODE_STANDARD:
        return _LDAPAuthenticate(conn, ldap_url, start_tls, username, password);
    }
}
#else
static cfapi_errid _AuthenticateExternal(EnterpriseDB *conn, const char *username, const char *password)
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

static bool _UserIsAdmin(EnterpriseDB *conn, const char *username)
{
    static const char *_ROLE_ADMIN = "admin";

    HubUser *user = NULL;
    if (_GetUserRecord(conn, false, username, &user) != ERRID_SUCCESS)
    {
        if (_IsLDAPEnabled(conn))
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

    bson record;
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
    if (result != ERRID_SUCCESS && _IsLDAPEnabled(conn))
    {
        if (authenticated_external)
        {
            *authenticated_external = true;
        }

        result = _AuthenticateExternal(conn, username, password);
    }

    CFDB_Close(conn);

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

static HubQuery *CFDB_GetRBACForUser(char *username)
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

    HubUser *user = NULL;
    cfapi_errid err = _GetUserRecord(&conn, false, username, &user);
    if (err != ERRID_SUCCESS && _IsLDAPEnabled(&conn))
    {
        err = _GetUserRecord(&conn, true, username, &user);
    }

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

cfapi_errid _CreateUser(EnterpriseDB *conn, bool external, const char *username, const char *password, const char *email, const Rlist *roles)
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

cfapi_errid CFDB_CreateUser(const char *creating_username, const char *username, const char *password, const char *email, const Rlist *roles)
{    
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    if (!_UserIsAdmin(conn, creating_username))
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
                        const Rlist *roles)
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
            BsonAppendStringArrayRlist(&set_op, dbkey_user_roles, roles);
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
                            const Rlist *roles)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return ERRID_DBCONNECT;
    }

    bool capable = false;
    if (_UserIsAdmin(conn, updating_username))
    {
        capable = true;
    }
    else if (StringSafeEqual(updating_username, username) && !roles)
    {
        capable = true;
    }

    if (!capable)
    {
        CFDB_Close(conn);
        return ERRID_ACCESS_DENIED;
    }

    cfapi_errid result = ERRID_UNKNOWN;
    if (_GetUserRecord(conn, false, username, NULL) == ERRID_SUCCESS)
    {
        result = _UpdateUser(conn, false, username, password, email, roles);
    }
    else if (_IsLDAPEnabled(conn))
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
            else if (_UsernameExistsExternal(conn, username) == ERRID_SUCCESS)
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

    if (!_UserIsAdmin(conn, deleting_username))
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

static Rlist *_GetExternalUsernamesLdap(EnterpriseDB *conn, const char *username, const char *password, const char *uri, const char *base_dn,
                                        const char *login_attribute, bool start_tls, const Rlist *user_directories)
{
    Rlist *result = NULL;
    for (const Rlist *rp = user_directories; rp; rp = rp->next)
    {
        const char *user_dir = ScalarValue(rp);
        char *dn = StringConcatenate(3, user_dir, ",", base_dn);
        char *bind_dn = StringConcatenate(7, login_attribute, "=", username, ",", user_dir, ",", base_dn);

        const char *errstr = NULL;
        Rlist *partial_result = CfLDAP_GetSingleAttributeList(username, password, uri, bind_dn, dn, _EXTERNAL_FILTER_LDAP, login_attribute, "subtree", "sasl",
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

static Rlist *_GetExternalUsernamesAD(EnterpriseDB *conn, const char *username, const char *password, const char *uri, const char *base_dn,
                                      const char *login_attribute, bool start_tls, const char *user_directory)
{
    char *dn = NULL;
    if (!user_directory)
    {
        dn = SafeStringDuplicate(base_dn);
    }
    else
    {
        dn = StringConcatenate(3, user_directory, ",", base_dn);
    }
    assert(dn);

    char *bind_dn = NULL;
    {
        char ad_domain[1024] = { 0 };
        if (CFDB_GetSetting(conn, SETTING_LDAP_AD_DOMAIN, ad_domain, sizeof(ad_domain)))
        {
            assert(false && "Must have AD-domain set to use AD");
            return NULL;
        }

        if (StringMatchFull("(\\w+\\.)+\\w{2,5}", ad_domain))
        {
            bind_dn = StringConcatenate(3, username, "@", ad_domain);
        }
        else
        {
            bind_dn = StringConcatenate(3, ad_domain, "\\", username);
        }
    }
    assert(bind_dn);

    const char *errstr = NULL;
    Rlist *result = CfLDAP_GetSingleAttributeList(username, password, uri, bind_dn, dn, _EXTERNAL_FILTER_AD, login_attribute, "subtree", "sasl",
                                         start_tls, 0, 0, &errstr);

    free(dn);
    free(bind_dn);

    if (!result)
    {
        // TODO: signal error
        return NULL;
    }

    return result;
}

static Rlist *_ListUsernamesExternal(EnterpriseDB *conn)
{
    char username[CF_SMALLBUF] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_USERNAME, username, sizeof(username)))
    {
        assert(false && "Must have LDAP username setting");
        return NULL;
    }

    char password[CF_SMALLBUF] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_PASSWORD, password, sizeof(password)))
    {
        assert(false && "Must have LDAP password setting");
        return NULL;
    }

    char login_attribute[CF_SMALLBUF] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_LOGIN_ATTRIBUTE, login_attribute, sizeof(login_attribute)))
    {
        assert(false && "Should have login attribute by default");
        return NULL;
    }

    char base_dn[1024] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_BASE_DN, base_dn, sizeof(base_dn)))
    {
        assert(false && "Need base domain name to use ldap");
        return NULL;
    }

    char encryption[1024] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_ENCRYPTION, encryption, sizeof(encryption)))
    {
        assert(false && "Encryption should have a default setting");
        return NULL;
    }

    char host[1024] = { 0 };
    if (!CFDB_GetSetting(conn, SETTING_LDAP_HOST, host, sizeof(host)))
    {
        assert(false && "Need host setting to use ldap");
        return NULL;
    }

    Rlist *user_directories = NULL;
    {
        char user_dirs[1024] = { 0 };
        if (!CFDB_GetSetting(conn, SETTING_LDAP_USERS_DIRECTORY, user_dirs, sizeof(user_dirs)))
        {
            assert(false && "Need host setting to use ldap");
            return NULL;
        }
        user_directories = SplitStringAsRList(user_dirs, ';');
    }


    char *uri = NULL;
    if (StringSafeEqual("ssl", encryption))
    {
        char port_ssl[1024] = { 0 };
        if (!CFDB_GetSetting(conn, SETTING_LDAP_PORT_SSL, port_ssl, sizeof(port_ssl)))
        {
            assert(false && "LDAP port for SSL should have a default");
            return NULL;
        }

        uri = StringConcatenate(4, "ldaps://", host, ":", port_ssl);
    }
    else
    {
        char port[1024] = { 0 };
        if (!CFDB_GetSetting(conn, SETTING_LDAP_PORT, port, sizeof(port)))
        {
            assert(false && "LDAP port should have a default");
            return NULL;
        }

        uri = StringConcatenate(4, "ldap://", host, ":", port);
    }

    bool start_tls = StringSafeEqual("tls", encryption);

    Rlist *external_usernames = NULL;
    switch (_LDAPMode(conn))
    {
    case LDAP_MODE_STANDARD:
        external_usernames = _GetExternalUsernamesLdap(conn, username, password, uri, base_dn, login_attribute, start_tls, user_directories);
        break;

    case LDAP_MODE_AD:
        external_usernames = _GetExternalUsernamesAD(conn, username, password, uri, base_dn, login_attribute, start_tls, ScalarValue(user_directories));
        break;

    default:
        assert(false && "External authentication mode not supported");
        break;
    }

    free(uri);
    DeleteRlist(user_directories);

    return external_usernames;
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

static bool _UsernameExistsExternal(EnterpriseDB *conn, const char *username)
{
    Rlist *external_usernames = _ListUsernamesExternal(conn);
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

    bson record;
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

    bool capable = false;
    if (StringSafeEqual(getting_username, username))
    {
        capable = true;
    }
    else if (_UserIsAdmin(conn, getting_username))
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
    else if (_IsLDAPEnabled(conn) && _UsernameExistsExternal(conn, username))
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

HubQuery *_ListUsers(EnterpriseDB *conn, const char *username_rx)
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
    if (_IsLDAPEnabled(conn))
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
            Rlist *external_usernames = _ListUsernamesExternal(conn);
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

    if (!_UserIsAdmin(conn, listing_username))
    {
        CFDB_Close(conn);
        return NewHubQueryErrid(NULL, NULL, ERRID_ACCESS_DENIED);
    }

    HubQuery *result = _ListUsers(conn, username_rx);

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

    if (!_UserIsAdmin(conn, creatingUser))
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

    if (!_UserIsAdmin(conn, deletingUser))
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

    cfapi_errid errid;

    if (IsRBACOn(conn))
    {
        errid = _UserIsAdmin(conn, username) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;
    }
    else
    {
        errid = ERRID_SUCCESS;
    }

    if (!CFDB_Close(conn))
    {
        errid = ERRID_DBCLOSE;
    }

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

static bool _IsLDAPEnabled(EnterpriseDB *conn)
{
    char value[32] = { 0 };
    CFDB_GetSetting(conn, SETTING_LDAP_ENABLED, value, sizeof(value));

    return StringSafeEqual("true", value);
}

static LDAPMode _LDAPMode(EnterpriseDB *conn)
{
    char value[32] = { 0 };
    CFDB_GetSetting(conn, SETTING_LDAP_MODE, value, sizeof(value));

    if (StringSafeEqual("activeDirectory", value))
    {
        return LDAP_MODE_AD;
    }
    else
    {
        return LDAP_MODE_STANDARD;
    }
}

/*****************************************************************************/

static bool IsRBACOn(EnterpriseDB *conn)
{
    char result[8] = { 0 };

    CFDB_HandleGetValue(settingLabels[SETTING_RBAC], result, sizeof(result), "true", conn, MONGO_SETTINGS_COLLECTION);

    if (strcmp(result, "false") == 0)
    {
        return false;
    }

    return true;                // safe default
}

/*****************************************************************************/

HubQuery *CFDB_ListRoles(const char *listing_user)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    cfapi_errid errid = _UserIsAdmin(&conn, listing_user) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;

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

HubQuery *CFDB_GetRoleByNameAuth(const char *username, const char *role_name)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return NewHubQueryErrid(NULL, NULL, ERRID_DBCONNECT);
    }

    cfapi_errid errid = _UserIsAdmin(&conn, username) ? ERRID_SUCCESS : ERRID_ACCESS_DENIED;

    CFDB_Close(&conn);

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

//*****************************************************************************

const char *HubSettingToString(HubSetting setting)
{
    return settingLabels[setting];
}

HubSetting HubSettingFromString(const char *setting_key)
{
    for (size_t i = 0; i < SETTING_MAX; i++)
    {
        if (StringSafeEqual(settingLabels[i], setting_key))
        {
            return (HubSetting)i;
        }
    }

    return SETTING_UNKNOWN;
}

JsonPrimitiveType HubSettingGetType(HubSetting setting)
{
    return setting_types[setting];
}

const char *HubSettingStringRange(HubSetting setting)
{
    assert(HubSettingGetType(setting) == JSON_PRIMITIVE_TYPE_STRING);

    switch (setting)
    {
    case SETTING_LDAP_MODE:
        return "^(standard|activeDirectory)$";
    case SETTING_LDAP_ENCRYPTION:
        return "^(none|ssl|tls)$";

    default:
        return ".*";
    }
}

bool CFDB_GetSetting(EnterpriseDB *conn, HubSetting setting, char *value_out, size_t size)
{
    switch (setting)
    {
    case SETTING_RBAC:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_RBAC), value_out, size, "true", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_ENABLED:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_ENABLED), value_out, size, "false", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_MODE:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_MODE), value_out, size, "standard", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_USERNAME:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_USERNAME), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_PASSWORD:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_PASSWORD), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_ENCRYPTION:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_ENCRYPTION), value_out, size, "none", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_LOGIN_ATTRIBUTE:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_LOGIN_ATTRIBUTE), value_out, size, "uid", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_BASE_DN:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_BASE_DN), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_USERS_DIRECTORY:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_USERS_DIRECTORY), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_HOST:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_HOST), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_PORT:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_PORT), value_out, size, "389", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_PORT_SSL:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_PORT_SSL), value_out, size, "636", conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_LDAP_AD_DOMAIN:
        return CFDB_HandleGetValue(HubSettingToString(SETTING_LDAP_AD_DOMAIN), value_out, size, NULL, conn, MONGO_SETTINGS_COLLECTION);

    case SETTING_BLUEHOST_HORIZON:
    {
        char *default_str = StringFromLong(CF_BLUEHOST_THRESHOLD_DEFAULT);
        bool found = CFDB_HandleGetValue(HubSettingToString(SETTING_BLUEHOST_HORIZON), value_out, size, default_str, conn, MONGO_SETTINGS_COLLECTION);
        free(default_str);
        return found;
    }

    default:
        assert(false && "Attempted to get unknown setting");
        return false;
    }
}

bool CFDB_UpdateSetting(EnterpriseDB *conn, HubSetting setting, const char *value)
{
    assert(setting > 0 && setting < SETTING_MAX);
    if (setting <= 0 || setting >= SETTING_MAX)
    {
        return false;
    }

    const char *setting_key = HubSettingToString(setting);

    return CFDB_PutValue(conn, setting_key, value, MONGO_SETTINGS_COLLECTION);
}

