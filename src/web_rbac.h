/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "bson_lib.h"

#define MONGO_ROLES_COLLECTION "phpcfengine.roles"
#define dbkey_role_members "members"
#define dbkey_role_name "name"

Item *GetAllRoles(void);
Item *GetRolesForUser(char *userName);
HubRBAC *GetRBACForUser(char *userName);
