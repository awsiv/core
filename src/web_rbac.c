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

static Item *GetRolesFromDB(bson *query);
static bool RoleExists(char *name);
static void DeAssociateUsersFromRole(mongo_connection *conn, char *roleName);
static const char *GetUsersCollection(mongo_connection *conn);
static bool IsLDAPOn(mongo_connection *conn);


HubUserRBAC *CFDB_GetRBACForUser(char *userName)
/*
 * Looks up the roles of the given user, and generates
 * the union of the RBAC permissions of these roles.
 */
{
 
 return NULL;
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
 Item *roles = CFDB_GetAllRoles();
 bool exists = IsItemIn(roles, name);
 DeleteItemList(roles);

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


static const char *GetUsersCollection(mongo_connection *conn)
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


Item *CFDB_GetAllRoles(void)
{
 bson query;
 bson_empty(&query);
 
 return GetRolesFromDB(&query);
}


Item *CFDB_GetRolesForUser(char *userName)
{
 bson_buffer bb;
 bson query;

 bson_buffer_init(&bb);
 bson_append_string(&bb, dbkey_role_members, userName);
 bson_from_buffer(&query, &bb);

 Item *roles = GetRolesFromDB(&query);
 bson_destroy(&query);

 return roles;
}


cfapi_errid CFDB_SetRolesForUser(char *userName, Item *roles)
{
 bson_buffer bb;
 bson query;

 bson_buffer_init(&bb);
 
 bson_append_start_object(&bb, dbkey_role_name);
 BsonAppendStringArray(&bb, "$in", roles);
 bson_append_finish_object(&bb);
 bson_from_buffer(&query, &bb);

 bson update;
 bson_buffer_init(&bb);
 BsonAppendAddToSetString(&bb, dbkey_role_members, userName);
 bson_from_buffer(&update, &bb);

 mongo_connection conn;
 
 if(!CFDB_Open(&conn))
    {
    return ERRID_DBCONNECT;
    }

 mongo_update(&conn, MONGO_ROLES_COLLECTION, &query, &update, MONGO_UPDATE_MULTI);

 bson_destroy(&query);
 bson_destroy(&update);

 CFDB_Close(&conn);

 return ERRID_SUCCESS;
}


static Item *GetRolesFromDB(bson *query)

{
 mongo_connection conn;
 bson_buffer bb;
 bson field;
 
 bson_buffer_init(&bb);
 bson_append_int(&bb, dbkey_role_name, 1);
 bson_from_buffer(&field, &bb);
 
 if(!CFDB_Open(&conn))
    {
    return NULL;
    }
 
 mongo_cursor *cursor = mongo_find(&conn, MONGO_ROLES_COLLECTION, query, &field, 0, 0, CF_MONGO_SLAVE_OK);
 bson_destroy(&field);

 CFDB_Close(&conn);

 Item *roles = NULL;
 
 while (mongo_cursor_next(cursor))
    {
    const char *roleName = BsonGetString(&(cursor->current), dbkey_role_name);

    if(roleName)
       {
       PrependItem(&roles, roleName, NULL);
       }
    }

 mongo_cursor_destroy(cursor);

 return roles;
}
