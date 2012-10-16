#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "db_save.h"
#include "conversion.h"
#include "sort.h"

static bool _KeepInternal(void *_user, void *_data)
{
    HubUser *user = _user;
    return !user->external;
}

static bool _KeepExternal(void *_user, void *_data)
{
    HubUser *user = _user;
    return user->external;
}

PHP_FUNCTION(cfapi_user_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/user");

    const char *username = NULL, *username_arg_rx = NULL;
    zval *external_zval = NULL;
    int username_len = 0, username_arg_rx_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sszll",
                              &username, &username_len,
                              &username_arg_rx, &username_arg_rx_len,
                              &external_zval,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    HubQuery *result = CFDB_ListUsers(username,
                                      username_arg_rx_len > 0 ? username_arg_rx : NULL);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing users");
    }

    {
        Trinary external = PHPZvalToTrinary(external_zval);
        switch (external)
        {
        case TRINARY_TRUE:
            RlistFilter(&result->records, _KeepExternal, NULL, DeleteHubUser);
            break;

        case TRINARY_FALSE:
            RlistFilter(&result->records, _KeepInternal, NULL, DeleteHubUser);
            break;

        default:
            break;
        }
    }

    result->records = SortRlist(result->records, HubUserCompare);

    const size_t total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubUser);

    JsonElement *data = JsonArrayCreate(500);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubUserToJson((HubUser *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubUser);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}

PHP_FUNCTION(cfapi_user_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/user/:id");

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

    HubUser *user = NULL;
    cfapi_errid err = CFDB_GetUser(username, username_arg, &user);
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
    syslog(LOG_DEBUG, "Requesting PUT /api/user/:id");

    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    const char *name = NULL; int name_len = 0;
    const char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssz",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &name, &name_len,
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

    Sequence *roles = PHPStringArrayToSequence(roles_arg, true);

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateUser(username, username_arg, password, name, email, roles)) != ERRID_SUCCESS)
    {
        SequenceDestroy(roles);
        THROW_GENERIC(err, "Unable to create user");
    }

    SequenceDestroy(roles);
    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_post)
{
    syslog(LOG_DEBUG, "Requesting POST /api/user/:id");

    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    char *password = NULL; int password_len = 0;
    char *name = NULL; int name_len = 0;
    char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssz",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &password, &password_len,
                              &name, &name_len,
                              &email, &email_len,
                              &roles_arg) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    Sequence *roles = PHPStringArrayToSequence(roles_arg, true);
    cfapi_errid err = CFDB_UpdateUser(username, username_arg,
                                      password_len > 0 ? password : NULL,
                                      name_len > 0 ? name : NULL,
                                      email_len > 0 ? email : NULL,
                                      roles);
    SequenceDestroy(roles);

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
    syslog(LOG_DEBUG, "Requesting DELETE /api/user/:id");

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

PHP_FUNCTION(cfapi_user_subscription_query_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/user/:user/subscription/query");

    const char *username = NULL, *username_arg;
    int username_len = 0, username_arg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &username_arg, &username_arg_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = CFDB_QueryScheduledReport(conn, username_arg, NULL, NULL,
                                                 NULL, NULL);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing users");
    }

    EnterpriseDBRelease(conn);
    JsonElement *data = JsonArrayCreate(50);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubScheduledReportToJson((HubScheduledReport *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubScheduledReport);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_user_subscription_query_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/user/:user/subscription/query/:id");

    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *sub_id = NULL; int sub_id_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &sub_id, &sub_id_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(sub_id_len, "sub_id");

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = CFDB_QueryScheduledReport(conn, username_arg, NULL, sub_id,
                                                 NULL, NULL);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing users");
    }

    EnterpriseDBRelease(conn);
    JsonElement *data = JsonArrayCreate(50);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubScheduledReportToJson((HubScheduledReport *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubScheduledReport);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_user_subscription_query_put)
{
    syslog(LOG_DEBUG, "Requesting PUT /api/user/:user/subscription/query/:id");

    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *sub_id = NULL; int sub_id_len = 0;

    const char *to = NULL; int to_len = 0;
    zend_bool enabled = false;
    const char *query = NULL; int query_len = 0;
    const char *schedule = NULL; int schedule_len = 0;
    const char *title = NULL; int title_len = 0;
    const char *description = NULL; int description_len = 0;
    zval *output_type = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbssssz",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &sub_id, &sub_id_len,
                              &to, &to_len,
                              &enabled,
                              &query, &query_len,
                              &schedule, &schedule_len,
                              &title, &title_len,
                              &description, &description_len,
                              &output_type) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(sub_id_len, "sub_id");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");
    ARGUMENT_CHECK_CONTENTS(schedule, "schedule");

    int report_output_type = 0;
    {
        if (!PHPArrayIsStringArray(output_type))
        {
            THROW_GENERIC(ERRID_ARGUMENT_WRONG, "outputType is not a string array");
        }

        Sequence *output_type_seq = PHPStringArrayToSequence(output_type, true);
        for (size_t i = 0; i < output_type_seq->length; i++)
        {
            const char *value = output_type_seq->data[i];
            if (StringSafeEqual("csv", value))
            {
                report_output_type |= REPORT_FORMAT_CSV;
            }
            else if (StringSafeEqual("pdf", value))
            {
                report_output_type |= REPORT_FORMAT_PDF;
            }
            else
            {
                SequenceDestroy(output_type_seq);
                THROW_GENERIC(ERRID_ARGUMENT_WRONG, "Invalid output type");
            }
        }
        SequenceDestroy(output_type_seq);

        if (report_output_type == 0)
        {
            THROW_GENERIC(ERRID_ARGUMENT_WRONG, "No output type selected");
        }
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    CFDB_SaveScheduledReport(conn,
                             username_arg,
                             to_len > 0 ? to : NULL,
                             sub_id,
                             query,
                             schedule,
                             enabled,
                             report_output_type,
                             title_len > 0 ? title : NULL,
                             description_len > 0 ? description : NULL);

    EnterpriseDBRelease(conn);

    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_subscription_query_delete)
{
    syslog(LOG_DEBUG, "Requesting DELETE /api/user/:user/subscription/query/:id");

    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *sub_id = NULL; int sub_id_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &sub_id, &sub_id_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(sub_id_len, "sub_id");

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    CFDB_RemoveScheduledReport(conn, username_arg, sub_id);
    EnterpriseDBRelease(conn);

    RETURN_BOOL(true);
}
