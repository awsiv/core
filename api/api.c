#include "api.h"

#include "reporting-engine.h"
#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"

static const char *API_NAME = "CFEngine Enterprise API";
static const char *API_VERSION = "v1";

//******************************************************************************

PHP_FUNCTION(cfapi)
{
    const char *username = NULL; int username_len = 0;
    const char *password = NULL; int password_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &password, &password_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");

    bool authenticated_external;
    if (CFDB_UserAuthenticate(username, password, &authenticated_external) != ERRID_SUCCESS)
    {
        THROW_GENERIC(ERRID_RBAC_ACCESS_DENIED, "Forbidden");
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    JsonElement *info = JsonObjectCreate(10);

    JsonObjectAppendString(info, "apiName", API_NAME);
    JsonObjectAppendString(info, "apiVersion", API_VERSION);
    JsonObjectAppendString(info, "hubVersion", NOVA_VERSION);
    JsonObjectAppendString(info, "databaseHostname", DBHostname());
    JsonObjectAppendInteger(info, "databasePort", DBPort());
    JsonObjectAppendString(info, "authenticated", authenticated_external ? "external" : "internal");

    if (conn)
    {
        JsonElement *license = NULL;
        cfapi_errid err = CFDB_QueryLicense(conn, &license);
        if (err == ERRID_SUCCESS)
        {
            JsonObjectAppendObject(info, "license", license);
        }

        EnterpriseDBRelease(conn);
    }

    JsonElement *output = JsonArrayCreate(1);
    JsonArrayAppendObject(output, info);

    RETURN_JSON(PackageResult(output, 1, 1));
}

//******************************************************************************

PHP_FUNCTION(cfapi_auth)
{
    const char *username = NULL; int username_len = 0;
    const char *password = NULL; int password_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &password, &password_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");

    cfapi_errid err = CFDB_UserAuthenticate(username, password, NULL);
    switch (err)
    {
    case ERRID_SUCCESS:
        RETURN_BOOL(true);

    default:
        RETURN_BOOL(false);
    }
}

//******************************************************************************

PHP_FUNCTION(cfapi_role_list)
{
    const char *username = NULL; int username_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &username_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    HubQuery *result = CFDB_ListRoles(username);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing roles");
    }

    JsonElement *data = JsonArrayCreate(500);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubRoleToJson((HubRole *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubRole);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_role_get)
{
    const char *username = NULL; int username_len = 0;
    const char *role = NULL; int role_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &role, &role_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(role_len, "role");

    HubQuery *result = CFDB_GetRoleByNameAuth(username, role);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error looking up role");
    }
    if (RlistLen(result->records) < 1)
    {
        RETURN_NULL();
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubRoleToJson((HubRole *)result->records->item));

    DeleteHubQuery(result, DeleteHubRole);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_role_put)
{
    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;
    const char *description = NULL; int description_len = 0;
    const char *include_context_rx = NULL; int include_context_rx_len = 0;
    const char *exclude_context_rx = NULL; int exclude_context_rx_len = 0;
    const char *include_bundle_rx = NULL; int include_bundle_rx_len = 0;
    const char *exclude_bundle_rx = NULL; int exclude_bundle_rx_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &username, &username_len,
                              &name, &name_len,
                              &description, &description_len,
                              &include_context_rx, &include_context_rx_len,
                              &exclude_context_rx, &exclude_context_rx_len,
                              &include_bundle_rx, &include_bundle_rx_len,
                              &exclude_bundle_rx, &exclude_bundle_rx_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    if (!CFDB_UserIsAdminWhenRBAC(username))
    {
        THROW_GENERIC(ERRID_RBAC_ACCESS_DENIED, "Must be admin to add roles");
    }

    cfapi_errid err = CFDB_DeleteRole(username, name, true);
    if (err != ERRID_SUCCESS && err != ERRID_ITEM_NONEXISTING)
    {
        THROW_GENERIC(err, "Error trying to delete existing role");
    }

    if ((err = CFDB_CreateRole(username, name, description,
                               include_context_rx, exclude_context_rx,
                               include_bundle_rx, exclude_bundle_rx)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to create role");
    }

    RETURN_BOOL(true);
}


PHP_FUNCTION(cfapi_role_post)
{
    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;
    const char *description = NULL; int description_len = 0;
    const char *include_context_rx = NULL; int include_context_rx_len = 0;
    const char *exclude_context_rx = NULL; int exclude_context_rx_len = 0;
    const char *include_bundle_rx = NULL; int include_bundle_rx_len = 0;
    const char *exclude_bundle_rx = NULL; int exclude_bundle_rx_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &username, &username_len,
                              &name, &name_len,
                              &description, &description_len,
                              &include_context_rx, &include_context_rx_len,
                              &exclude_context_rx, &exclude_context_rx_len,
                              &include_bundle_rx, &include_bundle_rx_len,
                              &exclude_bundle_rx, &exclude_bundle_rx_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    if (!CFDB_UserIsAdminWhenRBAC(username))
    {
        THROW_GENERIC(ERRID_RBAC_ACCESS_DENIED, "Must be admin to edit roles");
    }

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_UpdateRole(username, name, description,
                              include_context_rx, exclude_context_rx,
                              include_bundle_rx, exclude_bundle_rx)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to update role");
    }

    RETURN_BOOL(true);
}


PHP_FUNCTION(cfapi_role_delete)
{
    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &name, &name_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    cfapi_errid err = CFDB_DeleteRole(username, name, true);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to delete role");
    }

    RETURN_BOOL(true);
}

//******************************************************************************

PHP_FUNCTION(cfapi_user_list)
{
    const char *username = NULL, *password = NULL;
    int username_len = 0, password_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &password, &password_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");

    HubQuery *result = CFDB_ListUsers(username, NULL);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing users");
    }

    JsonElement *data = JsonArrayCreate(500);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubUserToJson((HubUser *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubUser);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_user_get)
{
    const char *username = NULL; int username_len = 0;
    const char *password = NULL; int password_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &password, &password_len,
                              &username_arg, &username_arg_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    HubUser *user = NULL;
    cfapi_errid err = CFDB_GetUser(username, password, username_arg, &user);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Error looking up user");
    }
    assert(user);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubUserToJson(user));

    DeleteHubUser(user);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_user_put)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    const char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssa",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &email, &email_len,
                              &roles_arg) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");

    {
        cfapi_errid delete_result = CFDB_DeleteUser(username, username_arg);
        switch (delete_result)
        {
        case ERRID_SUCCESS:
            break;

        case ERRID_ITEM_NONEXISTING:
            break;

        default:
            THROW_GENERIC(delete_result, "Unable to delete existing user");
            break;
        }
    }

    Rlist *roles = PHPStringArrayToRlist(roles_arg, true);

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateUser(username, username_arg, password, email, roles)) != ERRID_SUCCESS)
    {
        DeleteRlist(roles);
        THROW_GENERIC(err, "Unable to create user");
    }

    DeleteRlist(roles);
    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_post)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    const char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssa",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &email, &email_len,
                              &roles_arg) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    Rlist *roles = PHPStringArrayToRlist(roles_arg, true);
    cfapi_errid err = CFDB_UpdateUser(username, username_arg, password, email, roles);
    DeleteRlist(roles);

    switch (err)
    {
    case ERRID_SUCCESS:
        RETURN_BOOL(true);

    case ERRID_ITEM_NONEXISTING:
        THROW_GENERIC(err, "User must be created first in order to update");

    default:
        THROW_GENERIC(err, "Error updating user");
    }
}

PHP_FUNCTION(cfapi_user_delete)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &username_arg, &username_arg_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    cfapi_errid result = CFDB_DeleteUser(username, username_arg);
    if (result != ERRID_SUCCESS)
    {
        THROW_GENERIC(result, "Unable to delete user");
    }

    RETURN_BOOL(true);
}

//******************************************************************************

PHP_FUNCTION(cfapi_settings_get)
{
    const char *username = NULL; int username_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &username_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    JsonElement *settings = JsonObjectCreate(50);

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to open database");
    }

    char buffer[1024] = { 0 };
    if (CFDB_GetSetting(conn, SETTING_RBAC, buffer, sizeof(buffer)))
    {
        JsonObjectAppendBool(settings, HubSettingToString(SETTING_RBAC), StringSafeEqual(buffer, "true"));
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_ENABLED, buffer, sizeof(buffer)))
    {
        JsonObjectAppendBool(settings, HubSettingToString(SETTING_LDAP_ENABLED), StringSafeEqual(buffer, "true"));
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_MODE, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_MODE), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_USERNAME, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_USERNAME), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_PASSWORD, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_PASSWORD), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_ENCRYPTION, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_ENCRYPTION), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_LOGIN_ATTRIBUTE, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_LOGIN_ATTRIBUTE), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_BASE_DN, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_BASE_DN), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_USERS_DIRECTORY, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_USERS_DIRECTORY), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_HOST, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_HOST), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_PORT, buffer, sizeof(buffer)))
    {
        JsonObjectAppendInteger(settings, HubSettingToString(SETTING_LDAP_PORT), StringToLong(buffer));
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_PORT_SSL, buffer, sizeof(buffer)))
    {
        JsonObjectAppendInteger(settings, HubSettingToString(SETTING_LDAP_PORT_SSL), StringToLong(buffer));
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_AD_DOMAIN, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_LDAP_AD_DOMAIN), buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_BLUEHOST_HORIZON, buffer, sizeof(buffer)))
    {
        JsonObjectAppendInteger(settings, HubSettingToString(SETTING_BLUEHOST_HORIZON), StringToLong(buffer));
    }

    if (!EnterpriseDBRelease(conn))
    {
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, settings);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}


PHP_FUNCTION(cfapi_settings_post)
{
    const char *username = NULL; int username_len = 0;
    const char *post_data = NULL; int post_data_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &post_data, &post_data_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(post_data_len, "post_data");

    JsonElement *new_settings = JsonParse(&post_data);
    if (!new_settings)
    {
        THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Unable to parse JSON payload");
    }
    if ((JsonGetElementType(new_settings) != JSON_ELEMENT_TYPE_CONTAINER) ||
            (JsonGetContrainerType(new_settings) != JSON_CONTAINER_TYPE_OBJECT))
    {
        THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Update payload must be a JSON object");
    }

    // first validate that all settings are valid
    {
        JsonIterator iter = JsonIteratorInit(new_settings);
        const char *setting_key = NULL;
        while ((setting_key = JsonIteratorNextKey(&iter)))
        {
            const HubSetting setting = HubSettingFromString(setting_key);
            if (setting == SETTING_UNKNOWN)
            {
                THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Invalid setting: %s", setting_key);
            }

            const JsonElement *value = JsonIteratorCurrentValue(&iter);
            if (JsonGetElementType(value) != JSON_ELEMENT_TYPE_PRIMITIVE ||
                    JsonGetPrimitiveType(value) != HubSettingGetType(setting))
            {
                THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Setting type for %s is %s", setting_key,
                              JsonPrimitiveTypeToString(HubSettingGetType(setting)));
            }

            if (JsonGetPrimitiveType(value) == JSON_PRIMITIVE_TYPE_STRING)
            {
                const char *range_rx = HubSettingStringRange(setting);
                assert(range_rx);
                if (!StringMatchFull(range_rx, JsonPrimitiveGetAsString(value)))
                {
                    THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Value for setting %s must match regular-expression %s", setting_key, range_rx);
                }
            }
        }
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    // update settings
    {
        JsonIterator iter = JsonIteratorInit(new_settings);
        const char *setting_key = NULL;
        while ((setting_key = JsonIteratorNextKey(&iter)))
        {
            const HubSetting setting = HubSettingFromString(setting_key);
            const JsonElement *value = JsonIteratorCurrentValue(&iter);

            const char *value_str = NULL;
            switch (JsonIteratorCurrentPrimitiveType(&iter))
            {
            case JSON_PRIMITIVE_TYPE_STRING:
                value_str = JsonPrimitiveGetAsString(value);
                break;
            case JSON_PRIMITIVE_TYPE_INTEGER:
                value_str = StringFromLong(JsonPrimitiveGetAsInteger(value));
                break;
            case JSON_PRIMITIVE_TYPE_BOOL:
                value_str = JsonPrimitiveGetAsBool(value) ? "true" : "false";
                break;
            default:
                assert(false && "Never reach");
            }
            assert(value_str);

            if (!CFDB_UpdateSetting(conn, setting, value_str))
            {
                THROW_GENERIC(ERRID_DB_OPERATION, "Unable to write setting: %s", setting_key);
            }
        }
    }

    if (!EnterpriseDBRelease(conn))
    {
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    RETURN_BOOL(true);
}


//******************************************************************************

PHP_FUNCTION(cfapi_query_post)
{
    const char *username = NULL; int username_len = 0;
    const char *query = NULL; int query_len = 0;
    zval *context_includes = NULL, *context_excludes = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaa",
                              &username, &username_len,
                              &query, &query_len,
                              &context_includes,
                              &context_excludes) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");

    Rlist *include_list = PHPStringArrayToRlist(context_includes, true);
    Rlist *exclude_list = PHPStringArrayToRlist(context_excludes, true);

    JsonElement *data = EnterpriseExecuteSQL(username, query, include_list,
                                             exclude_list);

    DeleteRlist(include_list);
    DeleteRlist(exclude_list);

    assert(result);

    if (data == NULL)
    {
        data = JsonArrayCreate(0);
    }

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}
