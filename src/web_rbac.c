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
/*   (see struct HubRBAC)                                                    */
/*                                                                           */
/*  Currently, reports are filtered by class regexes only (on hosts),        */
/*  while promise definitions are filtered on bundle regex only              */
/*                                                                           */
/*****************************************************************************/

#include "web_rbac.h"

static Item *GetRolesFromDB(bson *query);


HubRBAC_t *GetRBACForUser(char *userName)
/*
 * Looks up the roles of the given user, and generates
 * the union of the RBAC permissions of these roles.
 */
{
 
return NULL;

}


Item *GetAllRoles(void)
{
 bson query;
 bson_empty(&query);
 
 return GetRolesFromDB(&query);
}


Item *GetRolesForUser(char *userName)
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


void *SetRolesForUser(char *userName, Item *roles)
{
 bson_buffer bb;

 bson_buffer_init(&bb);
// BsonAppendStringArray(&bb, dbkey_role_roles, roles);
 return NULL;
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
