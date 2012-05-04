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

#include <assert.h>


static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles);
static char *StringAppendRealloc2(char *start, char *append1, char *append2);
static bool RoleExists(char *name);
static void DeAssociateUsersFromRole(mongo_connection *conn, char *roleName);
static Item *CFDB_GetRolesForUser(mongo_connection *conn, char *userName);
static HubQuery *CFDB_GetRolesByMultipleNames(Item *names);
static HubQuery *CFDB_GetRoles(bson *query);
static const char *GetUsersCollection(mongo_connection *conn);
static bool IsLDAPOn(mongo_connection *conn);
static bool IsRBACOn(mongo_connection *conn);
static HubQuery *CFDB_GetAllRoles(void);
static HubQuery *CFDB_GetRoleByName(char *name);
static cfapi_errid UserIsRoleAdmin(mongo_connection *conn, char *userName);

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

/*****************************************************************************/

cfapi_errid CFDB_UserAuthenticate(const char *username, const char *password, size_t password_len)
{
// query
    bson_buffer buffer;

    bson_buffer_init(&buffer);
    bson_append_string(&buffer, dbkey_user_name, username);
    bson_append_int(&buffer, dbkey_user_active, 1);

    bson query;

    bson_from_buffer(&query, &buffer);

// projection
    bson_buffer_init(&buffer);
    bson_append_int(&buffer, dbkey_user_password, 1);

    bson field;

    bson_from_buffer(&field, &buffer);

    mongo_connection conn;

    if (!CFDB_Open(&conn))
    {
        bson_destroy(&query);
        bson_destroy(&field);
        return ERRID_DBCONNECT;
    }

    bson record;
    bson_bool_t found = mongo_find_one(&conn, GetUsersCollection(&conn), &query, &field, &record);

    bson_destroy(&query);
    bson_destroy(&field);

    if (found)
    {
        const char *db_password = NULL;
        BsonStringGet(&record, dbkey_user_password, &db_password);
        assert(db_password);

        if (db_password)
        {
            if (!IsLDAPOn(&conn))
            {
                CFDB_Close(&conn);

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
    }

    CFDB_Close(&conn);

    return ERRID_RBAC_ACCESS_DENIED;
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

    mongo_connection conn;

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
    mongo_connection conn;

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
    mongo_connection conn;

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

    bson_buffer bb;

    bson query;

    bson_buffer_init(&bb);
    bson_append_string(&bb, dbkey_role_name, roleName);
    bson_from_buffer(&query, &bb);

    bson update;

    bson_buffer_init(&bb);
    bson_buffer *set = bson_append_start_object(&bb, "$set");

    bson_append_string(set, dbkey_role_description, description);

    if (includeClassRx)
    {
        bson_append_string(set, dbkey_role_classrx_include, includeClassRx);
    }
    if (excludeClassRx)
    {
        bson_append_string(set, dbkey_role_classrx_exclude, excludeClassRx);
    }
    if (includeBundleRx)
    {
        bson_append_string(set, dbkey_role_bundlerx_include, includeBundleRx);
    }
    if (excludeBundleRx)
    {
        bson_append_string(set, dbkey_role_bundlerx_exclude, excludeBundleRx);
    }

    bson_append_finish_object(set);
    bson_from_buffer(&update, &bb);

    mongo_update(&conn, MONGO_ROLES_COLLECTION, &query, &update, MONGO_UPDATE_UPSERT);

    bson_destroy(&query);
    bson_destroy(&update);

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
    mongo_connection conn;

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

    bson_buffer bb;
    bson query;

    bson_buffer_init(&bb);
    bson_append_string(&bb, dbkey_role_name, roleName);
    bson_from_buffer(&query, &bb);

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
    mongo_connection conn;

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

static void DeAssociateUsersFromRole(mongo_connection *conn, char *roleName)
{
    const char *usersCollection = GetUsersCollection(conn);

    bson query;

    bson_empty(&query);

    bson_buffer bb;
    bson update;

    bson_buffer_init(&bb);
    bson_buffer *pull = bson_append_start_object(&bb, "$pull");

    bson_append_string(pull, dbkey_user_roles, roleName);
    bson_append_finish_object(pull);
    bson_from_buffer(&update, &bb);

    mongo_update(conn, usersCollection, &query, &update, MONGO_UPDATE_MULTI);
    bson_destroy(&update);
}

/*****************************************************************************/

static const char *GetUsersCollection(mongo_connection *conn)
{
    if (IsLDAPOn(conn))
    {
        return MONGO_USERS_LDAP_COLLECTION;
    }
    else
    {
        return MONGO_USERS_INTERNAL_COLLECTION;
    }
}

/*****************************************************************************/

static bool IsLDAPOn(mongo_connection *conn)
{
    char result[32] = { 0 };

    CFDB_HandleGetValue(dbkey_mpsettings_auth_mode, result, sizeof(result), conn, MONGO_MPSETTINGS_COLLECTION);

    if (strcmp(result, "ldap") == 0 || strcmp(result, "active_directory") == 0)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

static bool IsRBACOn(mongo_connection *conn)
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

static Item *CFDB_GetRolesForUser(mongo_connection *conn, char *userName)
{
    bson_buffer bb;

    bson query;

    bson_buffer_init(&bb);
    bson_append_string(&bb, dbkey_user_name, userName);
    bson_from_buffer(&query, &bb);

    bson field;

    bson_buffer_init(&bb);
    bson_append_int(&bb, dbkey_user_roles, 1);
    bson_from_buffer(&field, &bb);

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
    mongo_connection conn;

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
    bson_buffer bb;
    
    bson query;    
    bson_buffer_init(&bb);
    BsonAppendSortField(&bb, dbkey_role_name);
    bson_from_buffer(&query, &bb);
    
    HubQuery *hq = CFDB_GetRoles(&query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_GetRoleByNameAuth(char *userName, char *roleName)
{
    mongo_connection conn;

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
    bson_buffer bb;

    bson query;

    bson_buffer_init(&bb);
    bson_append_string(&bb, dbkey_role_name, name);
    bson_from_buffer(&query, &bb);

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

    bson_buffer bb;

    bson query;

    bson_buffer_init(&bb);

    bson_buffer *or = bson_append_start_array(&bb, "$or");

    int i = 0;
    char iStr[64];

    for (Item *ip = names; ip != NULL; ip = ip->next, i++)
    {
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_buffer *entry = bson_append_start_object(or, iStr);

        bson_append_string(entry, dbkey_role_name, ip->name);
        bson_append_finish_object(entry);
    }

    bson_append_finish_object(or);

    bson_from_buffer(&query, &bb);

    HubQuery *hq = CFDB_GetRoles(&query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_GetRoles(bson *query)
{
    Rlist *recordList = NULL;
    HubQuery *hq = NewHubQuery(NULL, recordList);

    mongo_connection conn;
    bson_buffer bb;
    bson field;

    bson_buffer_init(&bb);
    bson_append_int(&bb, dbkey_role_name, 1);
    bson_append_int(&bb, dbkey_role_description, 1);
    bson_append_int(&bb, dbkey_role_classrx_include, 1);
    bson_append_int(&bb, dbkey_role_classrx_exclude, 1);
    bson_append_int(&bb, dbkey_role_bundlerx_include, 1);
    bson_append_int(&bb, dbkey_role_bundlerx_exclude, 1);
    bson_from_buffer(&field, &bb);

    if (!CFDB_Open(&conn))
    {
        hq->errid = ERRID_DBCONNECT;
        bson_destroy(&field);
        return hq;
    }

    mongo_cursor *cursor = mongo_find(&conn, MONGO_ROLES_COLLECTION, query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&field);

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

static cfapi_errid UserIsRoleAdmin(mongo_connection *conn, char *userName)
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
