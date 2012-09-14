#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "db_save.h"
#include "conversion.h"

PHP_FUNCTION(cfapi_user_list)
{
    const char *username = NULL, *password = NULL;
    int username_len = 0, password_len = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &username, &username_len,
                              &password, &password_len,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
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
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *password = NULL; int password_len = 0;
    const char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssz",
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

    Sequence *roles = PHPStringArrayToSequence(roles_arg, true);

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_CreateUser(username, username_arg, password, email, roles)) != ERRID_SUCCESS)
    {
        SequenceDestroy(roles);
        THROW_GENERIC(err, "Unable to create user");
    }

    SequenceDestroy(roles);
    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_post)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    char *password = NULL; int password_len = 0;
    char *email = NULL; int email_len = 0;
    zval *roles_arg = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssz",
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

    Sequence *roles = PHPStringArrayToSequence(roles_arg, true);
    cfapi_errid err = CFDB_UpdateUser(username, username_arg,
                                      password_len > 0 ? password : NULL,
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
    DeleteHubQuery(result, DeleteScheduledReport);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_user_subscription_query_get)
{
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
    DeleteHubQuery(result, DeleteScheduledReport);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_user_subscription_query_put)
{
    const char *username = NULL; int username_len = 0;
    const char *username_arg = NULL; int username_arg_len = 0;
    const char *sub_id = NULL; int sub_id_len = 0;

    const char *to = NULL; int to_len = 0;
    zend_bool enabled = false;
    const char *query = NULL; int query_len = 0;
    const char *schedule = NULL; int schedule_len = 0;
    //const char *report_output_type = NULL; int report_output_type_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbss",
                              &username, &username_len,
                              &username_arg, &username_arg_len,
                              &sub_id, &sub_id_len,
                              &to, &to_len,
                              &enabled,
                              &query, &query_len,
                              &schedule, &schedule_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(username_arg_len, "username_arg");
    ARGUMENT_CHECK_CONTENTS(sub_id_len, "sub_id");
    ARGUMENT_CHECK_CONTENTS(to_len, "to");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");
    ARGUMENT_CHECK_CONTENTS(schedule, "schedule");

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    CFDB_SaveScheduledReport(conn, username_arg, to, sub_id, query, schedule,
                             enabled, 1);

    EnterpriseDBRelease(conn);

    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_user_subscription_query_delete)
{
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
