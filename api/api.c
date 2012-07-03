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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && password_len);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && role);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && name_len);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && name_len);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);

    HubQuery *result = CFDB_ListUsers(username, NULL);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing users");
    }

    JsonElement *data = JsonArrayCreate(500);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubUserRBACToJson((HubUserRBAC *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubUserRBAC);

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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && username_arg_len);

    HubQuery *result = CFDB_ListUsers(username, username_arg);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error looking up user");
    }
    if (RlistLen(result->records) < 1)
    {
        RETURN_NULL();
    }

    JsonElement *user_json = HubUserRBACToJson((HubUserRBAC *)result->records->item);
    DeleteHubQuery(result, DeleteHubUserRBAC);

    RETURN_JSON(PackageResult(user_json, 1, 1));
}

PHP_FUNCTION(cfapi_user_put)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len) == FAILURE)
    {
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && username_arg_len && password_len);

    {
        HubQuery *users = CFDB_ListUsers(username, username_arg);
        if (users->errid != ERRID_SUCCESS)
        {
            THROW_GENERIC(users->errid, "Unable to lookup user");
        }
        if (RlistLen(users->records) >= 1)
        {
            cfapi_errid err = ERRID_UNKNOWN;
            if ((err = CFDB_DeleteUser(username, username_arg)) != ERRID_SUCCESS)
            {
                THROW_GENERIC(err, "Unable to delete existing user");
            }
        }

        DeleteHubQuery(users, DeleteHubUserRBAC);
    }

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateUser(username, username_arg, password)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to create user");
    }

    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_delete)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &username_arg, &username_arg_len) == FAILURE)
    {
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && username_arg_len);

    cfapi_errid err = CFDB_DeleteUser(username, username_arg);
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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);

    JsonElement *settings = JsonObjectCreate(50);

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to open database");
    }

    char buffer[1024] = { 0 };
    if (CFDB_GetSetting(conn, SETTING_RBAC, buffer, sizeof(buffer)))
    {
        JsonObjectAppendBool(settings, settingLabels[SETTING_RBAC], StringSafeEqual(buffer, "true"));
    }

    if (CFDB_GetSetting(conn, SETTING_AUTH_MODE, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_AUTH_MODE], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_ENCRYPTION, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_LDAP_ENCRYPTION], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_LOGIN_ATTRIBUTE, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_LDAP_LOGIN_ATTRIBUTE], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_BASE_DN, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_LDAP_BASE_DN], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_USERS_DIRECTORY, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_LDAP_USERS_DIRECTORY], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_LDAP_HOST, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_LDAP_HOST], buffer);
    }

    if (CFDB_GetSetting(conn, SETTING_AD_DOMAIN, buffer, sizeof(buffer)))
    {
        JsonObjectAppendString(settings, settingLabels[SETTING_AD_DOMAIN], buffer);
    }

    if (!EnterpriseDBRelease(conn))
    {
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, settings);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
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
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && query_len);

    JsonElement *query_parsed = JsonParse(&query);
    if (!query_parsed)
    {
        zend_throw_exception(cfapi_exception_bad_request, LABEL_ERROR_INVALID_JSON, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *data = ReportingEngineQuery(query_parsed);
    JsonElementDestroy(query_parsed);

    assert(result);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}
