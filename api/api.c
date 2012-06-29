#include "api.h"

#include "reporting-engine.h"
#include "web_rbac.h"
#include "utils.h"

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

static JsonElement *HubUserRBACToJson(const HubUserRBAC *user)
{
    assert(user);

    JsonElement *obj = JsonObjectCreate(5);
    JsonObjectAppendString(obj, "username", user->userName);

    return obj;
}

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
