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
/*   (see HubUserRBAC)                                                       */
/*                                                                           */
/*  Currently, reports are filtered by class regexes only (on hosts),        */
/*  while promise definitions are filtered on bundle regex only              */
/*                                                                           */
/*****************************************************************************/

#include "web_rbac.h"

#ifdef HAVE_LIBMONGOC

static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles);
static char *StringAppendRealloc2(char *start, char *append1, char *append2);
const char *GetUsersCollection(mongo_connection *conn);
static bool RoleExists(char *name);
static void DeAssociateUsersFromRole(mongo_connection *conn, char *roleName);
static bool IsLDAPOn(mongo_connection *conn);
static Item *CFDB_GetRolesForUser(char *userName);
static HubQuery *CFDB_GetRolesByMultipleNames(Item *names);
static HubQuery *CFDB_GetRoles(bson *query);


HubQuery *CFDB_GetRBACForUser(char *userName)
/*
 * Looks up the roles of the given user, and generates
 * the union of the RBAC permissions of these roles.
 */
{
 
 Item *roleNames = CFDB_GetRolesForUser(userName);

 if(!roleNames)
    {
    DeleteItemList(roleNames);
    return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }
 
 HubQuery *hqRoles = CFDB_GetRolesByMultipleNames(roleNames);
 DeleteItemList(roleNames);

 if(hqRoles->errid != ERRID_SUCCESS)
    {
    HubQuery *hq = NewHubQueryErrid(NULL, NULL, hqRoles->errid);
    DeleteHubQuery(hqRoles, DeleteHubRole);
    return hq;
    }

 if(hqRoles->records == NULL)
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


static HubQuery *CombineAccessOfRoles(char *userName, HubQuery *hqRoles)
{
 char *combinedClassRxInclude = NULL;
 char *combinedClassRxExclude = NULL;
 char *combinedBundleRxInclude = NULL;

 if(hqRoles->records == NULL)
    {
    return NewHubQueryErrid(NULL, NULL, ERRID_RBAC_ACCESS_DENIED);
    }
 
 for (Rlist *rp = hqRoles->records; rp != NULL; rp = rp->next)
    {
    HubRole *role = (HubRole *)rp->item;

    // absolute correctness outweighs efficiency here
    combinedClassRxInclude = StringAppendRealloc2(combinedClassRxInclude, role->classRxInclude, "|");
    combinedClassRxExclude = StringAppendRealloc2(combinedClassRxExclude, role->classRxExclude, "|");
    combinedBundleRxInclude = StringAppendRealloc2(combinedBundleRxInclude, role->bundleRxInclude, "|");
    }

 ReplaceTrailingChar(combinedClassRxInclude, '|', '\0');
 ReplaceTrailingChar(combinedClassRxExclude, '|', '\0');
 ReplaceTrailingChar(combinedBundleRxInclude, '|', '\0');
 
 HubUserRBAC *rbac = NewHubUserRBAC(userName, combinedClassRxInclude, combinedClassRxExclude, combinedBundleRxInclude);

 free(combinedClassRxInclude);
 free(combinedClassRxExclude);
 free(combinedBundleRxInclude);

 Rlist *recordList = NULL;
 PrependRlistAlien(&(recordList), rbac);
 
 return NewHubQuery(NULL, recordList);
}


static char *StringAppendRealloc2(char *start, char *append1, char *append2)
{
 if(SafeStringLength(append1) == 0)
    {
    return start;
    }

 int startLen = SafeStringLength(start);
 
 start = xrealloc(start, startLen + strlen(append1) + strlen(append2) + 1);

 if(startLen == 0)
    {
    start[0] = '\0';
    }
 
 strcat(start, append1);
 strcat(start, append2);

 return start;
}


cfapi_errid CFDB_CreateRole(char *name, char *description, char *includeClassRx, char *excludeClassRx, char *includeBundleRx)
{
 
 if(RoleExists(name))
    {
    return ERRID_ITEM_EXISTS;
    }

 bson_buffer bb;
 
 bson query;
 bson_buffer_init(&bb);
 bson_append_string(&bb, dbkey_role_name, name);
 bson_from_buffer(&query, &bb);

 bson update;
 bson_buffer_init(&bb);
 bson_buffer *set = bson_append_start_object(&bb, "$set");
 bson_append_string(set, dbkey_role_description, description);
 bson_append_string(set, dbkey_role_classrx_include, includeClassRx);
 bson_append_string(set, dbkey_role_classrx_exclude, excludeClassRx); 
 bson_append_string(set, dbkey_role_bundlerx_include, includeBundleRx);
 bson_append_finish_object(set);
 bson_from_buffer(&update, &bb);

 mongo_connection conn;
 
 if(!CFDB_Open(&conn))
    {
    bson_destroy(&query);
    bson_destroy(&update);
    return ERRID_DBCONNECT;
    }

 mongo_update(&conn, MONGO_ROLES_COLLECTION, &query, &update, MONGO_UPDATE_UPSERT);
 
 bson_destroy(&query);
 bson_destroy(&update);

 CFDB_Close(&conn);
 
 return ERRID_SUCCESS;
}


cfapi_errid CFDB_DeleteRole(char *name)
{
 if(!RoleExists(name))
    {
    return ERRID_ITEM_NONEXISTING;
    }
 
 bson_buffer bb;
 bson query;

 bson_buffer_init(&bb);
 bson_append_string(&bb, dbkey_role_name, name);
 bson_from_buffer(&query, &bb);

 mongo_connection conn;
 
 if(!CFDB_Open(&conn))
    {
    bson_destroy(&query);
    return ERRID_DBCONNECT;
    }
 
 mongo_remove(&conn, MONGO_ROLES_COLLECTION, &query);
 bson_destroy(&query);

 DeAssociateUsersFromRole(&conn, name);

 CFDB_Close(&conn);

 return ERRID_SUCCESS;
}


static bool RoleExists(char *name)
{
 HubQuery *hq = CFDB_GetRoleByName(name);
 bool exists = (hq->records == NULL) ? false : true;
 DeleteHubQuery(hq, DeleteHubRole);

 return exists;
}


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


const char *GetUsersCollection(mongo_connection *conn)
{
 if(IsLDAPOn(conn))
    {
    return MONGO_USERS_LDAP_COLLECTION;
    }
 else
    {
    return MONGO_USERS_INTERNAL_COLLECTION;
    }
}


static bool IsLDAPOn(mongo_connection *conn)
{
 // FIXME - check this 
 return false;
}

static Item *CFDB_GetRolesForUser(char *userName)
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

 mongo_connection conn;

 if(!CFDB_Open(&conn))
    {
    bson_destroy(&query);
    bson_destroy(&field);
    return NULL;
    }
 
 const char *usersCollection = GetUsersCollection(&conn);

 mongo_cursor *cursor = mongo_find(&conn, usersCollection, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);
 bson_destroy(&query);
 bson_destroy(&field);

 CFDB_Close(&conn);

 Item *memberRoles = NULL;

 if(mongo_cursor_next(cursor))
    {
    memberRoles = BsonGetStringArrayAsItemList(&(cursor->current), dbkey_user_roles);
    }
 
 mongo_cursor_destroy(cursor);

 return memberRoles;
 }


HubQuery *CFDB_GetAllRoles(void)
{
 bson query;
 bson_empty(&query);
 
 return CFDB_GetRoles(&query);
}


HubQuery *CFDB_GetRoleByName(char *name)
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


HubQuery *CFDB_GetRolesByMultipleNames(Item *names)
{
 if(!names)
    {
    NewHubQueryErrid(NULL, NULL, ERRID_ARGUMENT_WRONG);
    }
 
 bson_buffer bb;

 bson query;
 bson_buffer_init(&bb);
 
 bson_buffer *or = bson_append_start_array(&bb, "$or");

 int i = 0;
 char iStr[64];
 
 for(Item *ip = names; ip != NULL; ip = ip->next, i++)
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
 bson_from_buffer(&field, &bb);
 
 if(!CFDB_Open(&conn))
    {
    hq->errid = ERRID_DBCONNECT;
    return hq;
    }
 
 mongo_cursor *cursor = mongo_find(&conn, MONGO_ROLES_COLLECTION, query, &field, 0, 0, CF_MONGO_SLAVE_OK);
 bson_destroy(&field);

 CFDB_Close(&conn);

 while (mongo_cursor_next(cursor))
    {
    char *name = SafeStringDuplicate(BsonGetString(&(cursor->current), dbkey_role_name));
    char *desc = SafeStringDuplicate(BsonGetString(&(cursor->current), dbkey_role_description));
    char *clRxIncl = SafeStringDuplicate(BsonGetString(&(cursor->current), dbkey_role_classrx_include));
    char *clRxExcl = SafeStringDuplicate(BsonGetString(&(cursor->current), dbkey_role_classrx_exclude));
    char *bRxIncl = SafeStringDuplicate(BsonGetString(&(cursor->current), dbkey_role_bundlerx_include));
    
    PrependRlistAlien(&(hq->records), NewHubRole(name, desc, clRxIncl, clRxExcl, bRxIncl));

    free(name);
    free(desc);
    free(clRxIncl);
    free(clRxExcl);
    free(bRxIncl);
    }

 mongo_cursor_destroy(cursor);

 return hq;
}

#endif
