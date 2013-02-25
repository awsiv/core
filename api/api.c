#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"

static const char *API_NAME = "CFEngine Enterprise API";
static const char *API_VERSION = "v1";

//******************************************************************************

PHP_FUNCTION(cfapi)
{
    syslog(LOG_DEBUG, "Requesting GET /api");

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

    bool authenticated_external = false;
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
    JsonObjectAppendString(info, "enterpriseVersion", NOVA_VERSION);
    JsonObjectAppendString(info, "coreVersion", VERSION);
    JsonObjectAppendString(info, "databaseHostname", DBHostname());
    JsonObjectAppendInteger(info, "databasePort", DBPort());
    JsonObjectAppendBool(info, "databasePrimary", CFDB_QueryIsMaster());
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
    syslog(LOG_DEBUG, "Requesting Authentication (cfapi_auth)");

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