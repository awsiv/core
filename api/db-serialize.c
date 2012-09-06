#include "db-serialize.h"

#include <assert.h>

JsonElement *HubUserToJson(const HubUser *user)
{
    assert(user);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "username", user->username);

    JsonObjectAppendBool(obj, "external", user->external);

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

    if (!NULL_OR_EMPTY(role->classRxInclude))
    {
        JsonObjectAppendString(obj, "includeContext", role->classRxInclude);
    }

    if (!NULL_OR_EMPTY(role->classRxInclude))
    {
        JsonObjectAppendString(obj, "excludeContext", role->classRxExclude);
    }

    if (!NULL_OR_EMPTY(role->classRxInclude))
    {
        JsonObjectAppendString(obj, "includeBundles", role->bundleRxInclude);
    }

    if (!NULL_OR_EMPTY(role->classRxInclude))
    {
        JsonObjectAppendString(obj, "excludeBundles", role->bundleRxExclude);
    }

    return obj;
}

JsonElement *HubHostToJson(const HubHost *host)
{
    assert(host);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "id", host->keyhash);

    if (host->hostname)
    {
        JsonObjectAppendString(obj, "hostname", host->hostname);
    }

    if (host->ipaddr)
    {
        JsonObjectAppendString(obj, "ip", host->ipaddr);
    }

    return obj;
}

JsonElement *HubClassToJson(const HubClass *context)
{
    assert(context);

    JsonElement *obj = JsonObjectCreate(5);

    JsonObjectAppendString(obj, "id", context->class);
    JsonObjectAppendReal(obj, "mean", context->prob);
    JsonObjectAppendReal(obj, "stdv", context->dev);
    JsonObjectAppendInteger(obj, "timestamp", context->t);

    return obj;
}

JsonElement *HubVitalPointToJson(const HubVitalPoint *point)
{
    JsonElement *p = JsonArrayCreate(2);

    JsonArrayAppendInteger(p, point->t);
    JsonArrayAppendReal(p, point->value);

    return p;
}

JsonElement *HubVitalToJson(const HubVital *vital)
{
    assert(vital);

    JsonElement *obj = JsonObjectCreate(5);

    JsonObjectAppendString(obj, "id", vital->id);

    if (vital->description)
    {
        JsonObjectAppendString(obj, "description", vital->description);
    }
    if (vital->description)
    {
        JsonObjectAppendString(obj, "units", vital->units);
    }

    if (vital->q)
    {
        JsonElement *values = JsonArrayCreate(CF_MAX_SLOTS);
        for (size_t i = 0; i < vital->q->length; i++)
        {
            const HubVitalPoint *point = vital->q->data[i];
            JsonArrayAppendArray(values, HubVitalPointToJson(point));
        }

        JsonObjectAppendArray(obj, "values", values);
    }

    return obj;
}
