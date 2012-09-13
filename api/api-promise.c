#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"


PHP_FUNCTION(cfapi_promise_list)
{
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

    HubQuery *result = CFDB_QueryPromisesUnexpanded(conn, filter);
    EnterpriseDBRelease(conn);

    JsonElement *data = JsonArrayCreate(5000);
    const size_t total = RlistLen(result->records);
    PageRecords(&(result->records), &page, DeleteHubPromise);

    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubPromise *promise = rp->item;
        JsonArrayAppendObject(data, HubPromiseToJson(promise));
    }

    DeleteHubQuery(result, DeleteHubPromise);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}
