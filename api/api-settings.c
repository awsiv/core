#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "db_save.h"

PHP_FUNCTION(cfapi_settings_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/settings");

    const char *username = NULL; int username_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &username_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to open database");
    }

    HubSettings *settings = NULL;
    cfapi_errid err = CFDB_QuerySettings(conn, &settings);
    EnterpriseDBRelease(conn);
    if (err != ERRID_SUCCESS)
    {
        syslog(LOG_ERR, "Retrieving settings from database FAILED, using built-in defaults");
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubSettingsToJson(settings));

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_settings_post)
{
    syslog(LOG_DEBUG, "Requesting POST /api/settings");

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

    HubSettings *new_settings = NULL;
    {
        JsonElement *json_settings = JsonParse(&post_data);
        if (!json_settings)
        {
            THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Unable to parse JSON payload");
        }

        char err_msg[CF_BUFSIZE] = { 0 };
        new_settings = HubSettingsFromJson(json_settings, err_msg);
        JsonElementDestroy(json_settings);
        if (!new_settings)
        {
            THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Invalid settings: %s", err_msg);
        }
    }
    assert(new_settings);

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        DeleteHubSettings(new_settings);
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to open database");
    }

    HubSettings *old_settings = NULL;
    {
        cfapi_errid err = CFDB_QuerySettings(conn, &old_settings);
        if (err != ERRID_SUCCESS)
        {
            syslog(LOG_ERR, "Retrieving settings from database FAILED, using built-in defaults");
        }
    }
    assert(old_settings);

    HubSettings *updated_settings = NULL;
    {
        updated_settings = HubSettingsUpdate(old_settings, new_settings);
        DeleteHubSettings(new_settings);
        DeleteHubSettings(old_settings);

        char error_msg[CF_BUFSIZE] = { 0 };
        if (!HubSettingsValidate(updated_settings, error_msg))
        {
            syslog(LOG_NOTICE, "Settings update FAILED validation");
            DeleteHubSettings(updated_settings);
            EnterpriseDBRelease(conn);
            THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Error validating settings: %s", error_msg);
        }
    }
    assert(updated_settings);

    {
        cfapi_errid err = CFDB_SaveSettings(conn, updated_settings);
        if (err != ERRID_SUCCESS)
        {
            EnterpriseDBRelease(conn);
            DeleteHubSettings(updated_settings);
            THROW_GENERIC(err, "Unable to save settings");
        }
        syslog(LOG_DEBUG, "Settings saved");
    }

    DeleteHubSettings(updated_settings);
    EnterpriseDBRelease(conn);

    RETURN_BOOL(true);
}
