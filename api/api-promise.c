#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "log.h"


PHP_FUNCTION(cfapi_promise_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/promise");

    const char *username = NULL; int username_len = 0;
    const char *type = NULL; int type_len = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &username, &username_len,
                              &type, &type_len,
                              &page.pageNum, &page.resultsPerPage) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    PromiseFilter *filter = NULL;
    {
        HubQuery *result = CFDB_PromiseFilterFromUserRBAC(username);
        if (result->errid != ERRID_RBAC_DISABLED && result->errid != ERRID_SUCCESS)
        {
            DeleteHubQuery(result, DeletePromiseFilter);
            THROW_GENERIC(ERRID_ACCESS_DENIED, "Access denied");
        }
        filter = HubQueryGetFirstRecord(result);
        DeleteHubQuery(result, NULL);
    }
    assert(filter);

    if (type_len > 0)
    {
        PromiseFilterAddPromiseType(filter, type);
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        DeletePromiseFilter(filter);
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = NULL;
    size_t total = 0;
    {
        LogPerformanceTimer timer = LogPerformanceStart();
        result = CFDB_QueryPromisesUnexpanded(conn, filter, PROMISE_CONTEXT_MODE_ALL);
        total = RlistLen(result->records);
        LogPerformanceStop(&timer, "Queries unexpanded promises from database, got %d results", total);
    }
    assert(result);

    DeletePromiseFilter(filter);
    EnterpriseDBRelease(conn);

    JsonElement *data = JsonArrayCreate(5000);
    PageRecords(&(result->records), &page, DeleteHubPromise);

    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubPromise *promise = rp->item;
        JsonArrayAppendObject(data, HubPromiseToJson(promise));
    }

    DeleteHubQuery(result, DeleteHubPromise);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}


PHP_FUNCTION(cfapi_promise_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/promise/:id");

    const char *username = NULL; int username_len = 0;
    const char *handle = NULL; int handle_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &handle, &handle_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(handle_len, "username");

    PromiseFilter *filter = NULL;
    {
        HubQuery *result = CFDB_PromiseFilterFromUserRBAC(username);
        if (result->errid != ERRID_RBAC_DISABLED && result->errid != ERRID_SUCCESS)
        {
            DeleteHubQuery(result, DeletePromiseFilter);
            THROW_GENERIC(ERRID_ACCESS_DENIED, "Access denied");
        }
        filter = HubQueryGetFirstRecord(result);
        DeleteHubQuery(result, NULL);

        PromiseFilterAddPromiseBody(filter, handle, NULL);
    }
    assert(filter);

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        DeletePromiseFilter(filter);
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }


    HubQuery *result = NULL;
    {
        LogPerformanceTimer timer = LogPerformanceStart();
        result = CFDB_QueryPromisesUnexpanded(conn, filter, PROMISE_CONTEXT_MODE_ALL);
        LogPerformanceStop(&timer, "Queried unexpanded promise %s from database", handle);
    }
    DeletePromiseFilter(filter);
    EnterpriseDBRelease(conn);

    const size_t total = RlistLen(result->records);
    assert(total == 1 || total == 0);
    if (total > 0)
    {
        JsonElement *data = JsonArrayCreate(1);
        assert(total == 1 || total == 0);

        for (const Rlist *rp = result->records; rp; rp = rp->next)
        {
            const HubPromise *promise = rp->item;
            JsonArrayAppendObject(data, HubPromiseToJson(promise));
        }

        DeleteHubQuery(result, DeleteHubPromise);

        RETURN_JSON(PackageResult(data, 1, total));
    }
    else
    {
        DeleteHubQuery(result, DeleteHubPromise);
        THROW_GENERIC(ERRID_ITEM_NONEXISTING, "Not found");
    }
}
