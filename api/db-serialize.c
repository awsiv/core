#include "db-serialize.h"

#include <assert.h>

JsonElement *HubUserRBACToJson(const HubUserRBAC *user)
{
    assert(user);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "username", user->userName);

    return obj;
}

JsonElement *HubRoleToJson(const HubRole *role)
{
    assert(role);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "name", role->name);

    if (role->description)
    {
        JsonObjectAppendString(obj, "description", role->description);
    }

    return obj;
}
