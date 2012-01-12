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

static HubQuery *CFDB_GetRoles(bson *query);
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
 HubQuery *hq = CFDB_GetRoleByName(name);

 // TODO: handle db connect error?
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


Item *CFDB_GetRolesForUser(char *userName)
{
 bson_buffer bb;
 bson query;

 bson_buffer_init(&bb);
 bson_append_string(&bb, dbkey_user_name, userName);
 bson_from_buffer(&query, &bb);

 // FIXME: users table
// Item *roles = CFDB_GetRole(&query);
 bson_destroy(&query);

 return NULL;
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
    char name[CF_MAXVARSIZE],  desc[CF_MAXVARSIZE],
        clRxIncl[CF_MAXVARSIZE], clRxExcl[CF_MAXVARSIZE], bRxIncl[CF_MAXVARSIZE];

    snprintf(name, sizeof(name), "%s", BsonGetString(&(cursor->current), dbkey_role_name));
    snprintf(desc, sizeof(desc), "%s", BsonGetString(&(cursor->current), dbkey_role_description));
    snprintf(clRxIncl, sizeof(clRxIncl), "%s", BsonGetString(&(cursor->current), dbkey_role_classrx_include));
    snprintf(clRxExcl, sizeof(clRxExcl), "%s", BsonGetString(&(cursor->current), dbkey_role_classrx_exclude));
    snprintf(bRxIncl, sizeof(bRxIncl), "%s", BsonGetString(&(cursor->current), dbkey_role_bundlerx_include));
    PrependRlistAlien(&(hq->records), NewHubRole(name, desc, clRxIncl, clRxExcl, bRxIncl));
    }

 mongo_cursor_destroy(cursor);

 return hq;
}
