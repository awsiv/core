#include "api.h"

#include "reporting-engine.h"
#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"

static const char *API_NAME = "CFEngine Enterprise API";
static const char *API_VERSION = "v1";

//******************************************************************************

PHP_FUNCTION(cfapi)
{
    JsonElement *info = JsonObjectCreate(4);

    JsonObjectAppendString(info, "apiName", API_NAME);
    JsonObjectAppendString(info, "apiVersion", API_VERSION);
    JsonObjectAppendString(info, "hubVersion", NOVA_VERSION);
    JsonObjectAppendString(info, "databaseHostname", DBHostname());
    JsonObjectAppendInteger(info, "databasePort", DBPort());

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

    cfapi_errid err = CFDB_UserAuthenticate(username, password, password_len);
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

    {
        HubQuery *roles = CFDB_GetRoleByNameAuth(username, name);
        if (roles->errid != ERRID_SUCCESS)
        {
            THROW_GENERIC(roles->errid, "Unable to lookup role");
        }
        if (RlistLen(roles->records) >= 1)
        {
            cfapi_errid err = ERRID_UNKNOWN;
            if ((err = CFDB_DeleteRole(username, name, true)) != ERRID_SUCCESS)
            {
                THROW_GENERIC(err, "Unable to delete existing role");
            }
        }

        DeleteHubQuery(roles, DeleteHubRole);
    }

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateRole(username, name, description,
                               include_context_rx, exclude_context_rx,
                               include_bundle_rx, exclude_bundle_rx)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to create role");
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
    const char *username = NULL; int username_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &username_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    cfapi_errid errid = CFDB_UserIsAdminWhenRBAC(username);
    if (errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(errid, "Access denied");
    }

    HubQuery *result = CFDB_ListUsers(NULL);
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
    const char *username_arg = NULL; int username_arg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &username_arg, &username_arg_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    cfapi_errid errid = CFDB_UserIsAdminWhenRBAC(username);
    if (errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(errid, "Access denied");
    }

    HubQuery *result = CFDB_ListUsers(username_arg);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error looking up user");
    }
    if (RlistLen(result->records) < 1)
    {
        RETURN_NULL();
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubUserToJson((HubUser *)result->records->item));

    DeleteHubQuery(result, DeleteHubUser);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_user_put)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    bool active;
    const char *email = NULL; int email_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbs",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &active,
                              &email, &email_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(password_len, "password");

    cfapi_errid errid = CFDB_UserIsAdminWhenRBAC(username);
    if (errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(errid, "Access denied");
    }

    {
        HubQuery *users = CFDB_ListUsers(username_arg);
        if (users->errid != ERRID_SUCCESS)
        {
            THROW_GENERIC(users->errid, "Unable to lookup user");
        }
        if (RlistLen(users->records) >= 1)
        {
            cfapi_errid err = ERRID_UNKNOWN;
            if ((err = CFDB_DeleteUser(username_arg)) != ERRID_SUCCESS)
            {
                THROW_GENERIC(err, "Unable to delete existing user");
            }
        }

        DeleteHubQuery(users, DeleteHubUser);
    }

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateUser(username_arg, password, active, email)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to create user");
    }

    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_post)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    bool active;
    const char *email = NULL; int email_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbs",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &active,
                              &email, &email_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    cfapi_errid errid = CFDB_UserIsAdminWhenRBAC(username);
    if (errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(errid, "Access denied");
    }

    cfapi_errid err = CFDB_UpdateUser(username_arg, password, active, email);
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

    cfapi_errid err = CFDB_UserIsAdminWhenRBAC(username);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Access denied");
    }

    err = CFDB_DeleteUser(username_arg);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to delete user");
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

    if (CFDB_GetSetting(conn, SETTING_AUTH_MODE, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_AUTH_MODE), buffer);
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

    if (CFDB_GetSetting(conn, SETTING_AD_DOMAIN, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, HubSettingToString(SETTING_AD_DOMAIN), buffer);
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
        const char *setting = NULL;
        while ((setting = JsonIteratorNextKey(&iter)))
        {
            if (HubSettingFromString(setting) == SETTING_UNKNOWN)
            {
                THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Invalid setting: %s", setting);
            }

            const JsonElement *value = JsonIteratorCurrentValue(&iter);

            if (JsonGetElementType(value) != JSON_ELEMENT_TYPE_PRIMITIVE ||
                    JsonGetPrimitiveType(value) != JSON_PRIMITIVE_TYPE_STRING)
            {
                THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Setting value for %s must be a string", setting);
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
        const char *setting = NULL;
        while ((setting = JsonIteratorNextKey(&iter)))
        {
            const char *value = JsonPrimitiveGetAsString(JsonIteratorCurrentValue(&iter));
            if (!CFDB_UpdateSetting(conn, HubSettingFromString(setting), value))
            {
                THROW_GENERIC(ERRID_DB_OPERATION, "Unable to write setting: %s", setting);
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

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &query, &query_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");

    JsonElement *query_parsed = JsonParse(&query);
    if (!query_parsed)
    {
        THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Could not parse JSON argument");
    }

    JsonElement *data = ReportingEngineQuery(query_parsed);
    JsonElementDestroy(query_parsed);

    assert(result);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

/******************************************************************************/
/* TODO: user management (RBAC) needed here */
PHP_FUNCTION(cfapi_exec_sql)
{
    char *sql;
    int sql_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &sql, &sql_len) == FAILURE)
    {
       THROW_ARGS_MISSING();
    }

    JsonElement *data = EnterpriseExecuteSQL(sql);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

/******************************************************************************/
