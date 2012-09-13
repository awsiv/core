#include "db-serialize.h"

#include <assert.h>

JsonElement *HubScheduledReportToJson(const HubScheduledReport *scheduled_report)
{
    assert(scheduled_report);

    JsonElement *obj = JsonObjectCreate(8);
    JsonObjectAppendString(obj, "id", scheduled_report->query_id);
    JsonObjectAppendString(obj, "username", scheduled_report->username);
    JsonObjectAppendString(obj, "to", scheduled_report->email);
    JsonObjectAppendString(obj, "query", scheduled_report->query);
    JsonObjectAppendString(obj, "schedule", scheduled_report->schedule);
    JsonObjectAppendBool(obj, "enabled", scheduled_report->enabled);
    JsonObjectAppendInteger(obj, "outputType", scheduled_report->output_type);
    JsonObjectAppendInteger(obj, "lastRun", scheduled_report->last_run);

    return obj;
}

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

    if (vital->last_update > 0)
    {
        JsonObjectAppendInteger(obj, "timestamp", vital->last_update);
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

typedef enum
{
    _SETTING_UNKNOWN = 0,

    _SETTING_RBAC_ENABLED,
    _SETTING_LDAP_ENABLED,
    _SETTING_LDAP_AD_DOMAIN,
    _SETTING_LDAP_BASE_DN,
    _SETTING_LDAP_ENCRYPTION,
    _SETTING_LDAP_HOST,
    _SETTING_LDAP_MODE,
    _SETTING_LDAP_LOGIN_ATTRIBUTE,
    _SETTING_LDAP_USERNAME,
    _SETTING_LDAP_PASSWORD,
    _SETTING_LDAP_USERS_DIR,
    _SETTING_LDAP_PORT,
    _SETTING_LDAP_PORT_SSL,
    _SETTING_BLUEHOST_HORIZON,

    _SETTING_MAX
} _Setting;

const char *_setting_strings[_SETTING_MAX] =
{
    [_SETTING_UNKNOWN] = "unknown",

    [_SETTING_RBAC_ENABLED] = "rbacEnabled",
    [_SETTING_LDAP_ENABLED] = "ldapEnabled",
    [_SETTING_LDAP_AD_DOMAIN] = "ldapActiveDirectoryDomain",
    [_SETTING_LDAP_BASE_DN] = "ldapBaseDN",
    [_SETTING_LDAP_ENCRYPTION] = "ldapEncryption",
    [_SETTING_LDAP_HOST] = "ldapHost",
    [_SETTING_LDAP_MODE] = "ldapMode",
    [_SETTING_LDAP_LOGIN_ATTRIBUTE] = "ldapLoginAttribute",
    [_SETTING_LDAP_USERNAME] = "ldapUsername",
    [_SETTING_LDAP_PASSWORD] = "ldapPassword",
    [_SETTING_LDAP_USERS_DIR] = "ldapUsersDirectory",
    [_SETTING_LDAP_PORT] = "ldapPort",
    [_SETTING_LDAP_PORT_SSL] = "ldapPortSSL",
    [_SETTING_BLUEHOST_HORIZON] = "blueHostHorizon"
};

const JsonPrimitiveType _setting_types[_SETTING_MAX] =
{
    [_SETTING_UNKNOWN] = JSON_PRIMITIVE_TYPE_NULL,

    [_SETTING_RBAC_ENABLED] = JSON_PRIMITIVE_TYPE_BOOL,
    [_SETTING_LDAP_ENABLED] = JSON_PRIMITIVE_TYPE_BOOL,
    [_SETTING_LDAP_AD_DOMAIN] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_BASE_DN] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_ENCRYPTION] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_HOST] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_MODE] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_LOGIN_ATTRIBUTE] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_USERNAME] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_PASSWORD] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_USERS_DIR] = JSON_PRIMITIVE_TYPE_STRING,
    [_SETTING_LDAP_PORT] = JSON_PRIMITIVE_TYPE_INTEGER,
    [_SETTING_LDAP_PORT_SSL] = JSON_PRIMITIVE_TYPE_INTEGER,
    [_SETTING_BLUEHOST_HORIZON] = JSON_PRIMITIVE_TYPE_INTEGER
};

const char * _setting_ranges[_SETTING_MAX] =
{
    [_SETTING_UNKNOWN] = NULL,

    [_SETTING_RBAC_ENABLED] = NULL,
    [_SETTING_LDAP_ENABLED] = NULL,
    [_SETTING_LDAP_AD_DOMAIN] = NULL,
    [_SETTING_LDAP_BASE_DN] = NULL,
    [_SETTING_LDAP_ENCRYPTION] = "(plain|ssl|tls)",
    [_SETTING_LDAP_HOST] = NULL,
    [_SETTING_LDAP_MODE] = "(standard|activeDirectory)",
    [_SETTING_LDAP_LOGIN_ATTRIBUTE] = NULL,
    [_SETTING_LDAP_USERNAME] = NULL,
    [_SETTING_LDAP_PASSWORD] = NULL,
    [_SETTING_LDAP_USERS_DIR] = NULL,
    [_SETTING_LDAP_PORT] = NULL,
    [_SETTING_LDAP_PORT_SSL] = NULL,

    [_SETTING_BLUEHOST_HORIZON] = NULL
};


_Setting _SettingFromString(const char *setting_str)
{
    for (size_t i = 0; i < _SETTING_MAX; i++)
    {
        if (StringSafeEqual(_setting_strings[i], setting_str))
        {
            return (_Setting)i;
        }
    }
    return _SETTING_UNKNOWN;
}

JsonElement *HubSettingsToJson(const HubSettings *settings)
{
    assert(settings);

    JsonElement *obj = JsonObjectCreate(20);

    if (settings->rbac_enabled != TRINARY_UNDEFINED)
    {
        JsonObjectAppendBool(obj, _setting_strings[_SETTING_RBAC_ENABLED], settings->rbac_enabled == TRINARY_TRUE);
    }

    if (settings->ldap.enabled != TRINARY_UNDEFINED)
    {
        JsonObjectAppendBool(obj, _setting_strings[_SETTING_LDAP_ENABLED], settings->ldap.enabled == TRINARY_TRUE);
    }

    if (settings->ldap.ad_domain)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_AD_DOMAIN], settings->ldap.ad_domain);
    }

    if (settings->ldap.base_dn)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_BASE_DN], settings->ldap.base_dn);
    }

    if (settings->ldap.encryption)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_ENCRYPTION], settings->ldap.encryption);
    }

    if (settings->ldap.host)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_HOST], settings->ldap.host);
    }

    if (settings->ldap.login_attribute)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_LOGIN_ATTRIBUTE], settings->ldap.login_attribute);
    }

    if (settings->ldap.mode != LDAP_MODE_UNDEFINED)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_MODE], HubSettingsLDAPModeToString(settings->ldap.mode));
    }

    if (settings->ldap.password)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_PASSWORD], settings->ldap.password);
    }

    if (settings->ldap.port > 0)
    {
        JsonObjectAppendInteger(obj, _setting_strings[_SETTING_LDAP_PORT], settings->ldap.port);
    }

    if (settings->ldap.port_ssl > 0)
    {
        JsonObjectAppendInteger(obj, _setting_strings[_SETTING_LDAP_PORT_SSL], settings->ldap.port_ssl);
    }

    if (settings->ldap.username)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_USERNAME], settings->ldap.username);
    }

    if (settings->ldap.users_directory)
    {
        JsonObjectAppendString(obj, _setting_strings[_SETTING_LDAP_USERS_DIR], settings->ldap.users_directory);
    }

    if (settings->bluehost_horizon > 0)
    {
        JsonObjectAppendInteger(obj, _setting_strings[_SETTING_BLUEHOST_HORIZON], settings->bluehost_horizon);
    }

    return obj;
}

const char *_JsonPrimitiveTypeToString(JsonPrimitiveType type)
{
    switch (type)
    {
    case JSON_PRIMITIVE_TYPE_STRING:
        return "string";
    case JSON_PRIMITIVE_TYPE_INTEGER:
        return "integer";
    case JSON_PRIMITIVE_TYPE_REAL:
        return "real";
    case JSON_PRIMITIVE_TYPE_BOOL:
        return "boolean";
    default:
        return NULL;
    }
}

HubSettings *HubSettingsFromJson(const JsonElement *json, char error_out[CF_BUFSIZE])
{
    assert(json);
    assert(error_out);

    HubSettings *settings = NewHubSettings();

    JsonIterator it = JsonIteratorInit(json);
    const char *setting_key = NULL;
    while ((setting_key = JsonIteratorNextKey(&it)) != NULL)
    {
        _Setting setting = _SettingFromString(setting_key);

        if (setting == _SETTING_UNKNOWN)
        {
            DeleteHubSettings(settings);
            snprintf(error_out, CF_BUFSIZE, "Unsupported setting: %s", setting_key);
            return NULL;
        }

        const JsonElement *value = JsonIteratorCurrentValue(&it);
        if (JsonGetElementType(value) != JSON_ELEMENT_TYPE_PRIMITIVE)
        {
            DeleteHubSettings(settings);
            snprintf(error_out, CF_BUFSIZE, "Setting must be JSON primitive: %s", setting_key);
            return NULL;
        }

        if (JsonGetPrimitiveType(value) != _setting_types[setting])
        {
            DeleteHubSettings(settings);
            snprintf(error_out, CF_BUFSIZE, "Setting %s must have type %s", setting_key, _JsonPrimitiveTypeToString(_setting_types[setting]));
            return NULL;
        }

        if (JsonGetPrimitiveType(value) == JSON_PRIMITIVE_TYPE_STRING)
        {
            const char *range = _setting_ranges[setting];
            if (range)
            {
                if (!StringMatchFull(range, JsonPrimitiveGetAsString(value)))
                {
                    DeleteHubSettings(settings);
                    snprintf(error_out, CF_BUFSIZE, "Setting %s must match: %s", setting_key, range);
                    return NULL;
                }
            }
        }

        switch (setting)
        {
        case _SETTING_RBAC_ENABLED:
            settings->rbac_enabled = JsonPrimitiveGetAsBool(value);
            break;
        case _SETTING_LDAP_ENABLED:
            settings->ldap.enabled = JsonPrimitiveGetAsBool(value);
            break;
        case _SETTING_LDAP_AD_DOMAIN:
            settings->ldap.ad_domain = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_BASE_DN:
            settings->ldap.base_dn = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_ENCRYPTION:
            settings->ldap.encryption = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_HOST:
            settings->ldap.host = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_MODE:
            settings->ldap.mode = HubSettingsLDAPModeFromString(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_LOGIN_ATTRIBUTE:
            settings->ldap.login_attribute = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_USERNAME:
            settings->ldap.username = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_PASSWORD:
            settings->ldap.password = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_USERS_DIR:
            settings->ldap.users_directory = SafeStringDuplicate(JsonPrimitiveGetAsString(value));
            break;
        case _SETTING_LDAP_PORT:
            settings->ldap.port = JsonPrimitiveGetAsInteger(value);
            break;
        case _SETTING_LDAP_PORT_SSL:
            settings->ldap.port_ssl = JsonPrimitiveGetAsInteger(value);
            break;
        case _SETTING_BLUEHOST_HORIZON:
            settings->bluehost_horizon = JsonPrimitiveGetAsInteger(value);
            break;

        default:
            assert(false && "invalid setting");
            DeleteHubSettings(settings);
            snprintf(error_out, CF_BUFSIZE, "Invalid setting, internal error");
            return NULL;
        }
    }

    return settings;
}

JsonElement *HubPromiseToJson(const HubPromise *promise)
{
    assert(promise);

    JsonElement *obj = JsonObjectCreate(20);

    if (promise->handle)
    {
        JsonObjectAppendString(obj, "id", promise->handle);
    }

    if (promise->promiseType)
    {
        JsonObjectAppendString(obj, "type", promise->promiseType);
    }

    if (promise->promiser)
    {
        JsonObjectAppendString(obj, "promiser", promise->promiser);
    }

    if (promise->promisees)
    {
        JsonElement *promisees_arr = JsonArrayCreate(20);
        for (const Rlist *rp = promise->promisees; rp; rp = rp->next)
        {
            const char *promisee = ScalarValue(rp);
            JsonArrayAppendString(promisees_arr, promisee);
        }
        JsonObjectAppendObject(obj, "promisees", promisees_arr);
    }

    if (promise->bundleName)
    {
        JsonObjectAppendString(obj, "bundle", promise->bundleName);
    }

    if (promise->comment)
    {
        JsonObjectAppendString(obj, "comment", promise->comment);
    }

    return obj;
}
