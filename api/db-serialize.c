#include "db-serialize.h"

#include <assert.h>

JsonElement *HubUserToJson(const HubUser *user)
{
    assert(user);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "username", user->username);
    JsonObjectAppendBool(obj, "active", user->active);

    if (user->email)
    {
        JsonObjectAppendString(obj, "email", user->email);
    }

    if (user->roles)
    {
        JsonElement *roles = JsonArrayCreate(50);
        for (const Rlist *rp = user->roles; rp; rp = rp->next)
        {
            JsonArrayAppendString(roles, ScalarValue(rp));
        }
        JsonObjectAppendArray(obj, "roles", roles);
    }

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
