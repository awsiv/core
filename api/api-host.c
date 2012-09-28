#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "db_save.h"
#include "db_maintain.h"
#include "granules.h"
#include "log.h"


PHP_FUNCTION(cfapi_host_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/host");

    const char *username = NULL; int username_len = 0;
    zval *context_include = NULL, *context_exclude = NULL;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "saall",
                              &username, &username_len,
                              &context_include,
                              &context_exclude,
                              &page.pageNum, &page.resultsPerPage) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    HubQuery *result = NULL;
    {
        HubQuery *filter_result = CFDB_HostClassFilterFromUserRBAC(username);
        if (filter_result->errid != ERRID_RBAC_DISABLED && filter_result->errid != ERRID_SUCCESS)
        {
            THROW_GENERIC(result->errid, "Access denied");
        }

        HostClassFilter *context_filter = (HostClassFilter *)HubQueryGetFirstRecord(filter_result);
        HostClassFilterAddIncludeExcludeLists(context_filter, context_include, context_exclude);

        EnterpriseDB *conn = EnterpriseDBAcquire();
        if (!conn)
        {
            THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
        }

        result = CFDB_QueryHostsByHostClassFilter(conn, context_filter);

        if (!EnterpriseDBRelease(conn))
        {
            DeleteHubQuery(result, NULL);
            THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
        }

        DeleteHubQuery(filter_result, DeleteHostClassFilter);
    }
    assert(result);

    const size_t total = RlistLen(result->hosts);
    PageRecords(&result->hosts, &page, DeleteHubHost);

    JsonElement *data = JsonArrayCreate(1);
    for (const Rlist *rp = result->hosts; rp; rp = rp->next)
    {
        const HubHost *host = rp->item;
        JsonArrayAppendObject(data, HubHostToJson(host));
    }

    DeleteHubQuery(result, DeleteHubHost);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}


PHP_FUNCTION(cfapi_host_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/host/:id");

    const char *username = NULL; int username_len = 0;
    const char *hostkey = NULL; int hostkey_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &hostkey, &hostkey_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(hostkey_len, "hostkey");
    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = CFDB_QueryHostByHostKey(conn, hostkey);

    if (!EnterpriseDBRelease(conn))
    {
        DeleteHubQuery(result, NULL);
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    JsonElement *data = JsonArrayCreate(1);
    if (result->hosts && result->hosts->item)
    {
        const HubHost *host = result->hosts->item;
        JsonArrayAppendObject(data, HubHostToJson(host));
    }
    else
    {
        THROW_GENERIC(ERRID_ITEM_NONEXISTING, "Could not find host");
    }
    DeleteHubQuery(result, NULL);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_host_delete)
{
    syslog(LOG_DEBUG, "Requesting DELETE /api/host");

    const char *username = NULL; int username_len = 0;
    const char *hostkey = NULL; int hostkey_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &hostkey, &hostkey_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(hostkey_len, "hostkey");

    {
        cfapi_errid err = CFDB_HasHostAccessFromUserRBAC(username, hostkey);
        if (err != ERRID_SUCCESS)
        {
            THROW_GENERIC(err, "Access denied");
        }
    }
    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    CFDB_MarkAsDeleted(conn, hostkey);
    CFDB_PurgeHost(conn, hostkey);

    EnterpriseDBRelease(conn);

    RETURN_BOOL(true);
}

PHP_FUNCTION(cfapi_host_context_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/host/:id/context");

    const char *username = NULL; int username_len = 0;
    const char *hostkey = NULL; int hostkey_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &hostkey, &hostkey_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(hostkey_len, "hostkey");
    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = CFDB_QueryClasses(conn, hostkey, NULL, false, 0, time(NULL),
                                         NULL, false, PROMISE_CONTEXT_MODE_ALL);

    if (!EnterpriseDBRelease(conn))
    {
        DeleteHubQuery(result, DeleteHubClass);
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    JsonElement *data = JsonArrayCreate(1000);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubClass *context = rp->item;
        JsonArrayAppendObject(data, HubClassToJson(context));
    }

    DeleteHubQuery(result, DeleteHubClass);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_host_vital_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/host/:id/vital");

    const char *username = NULL; int username_len = 0;
    const char *hostkey = NULL; int hostkey_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &hostkey, &hostkey_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(hostkey_len, "hostkey");

    cfapi_errid err = CFDB_HasHostAccessFromUserRBAC(username, hostkey);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Access denied to host");
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    time_t last_host_update = 0;
    CFDB_QueryLastHostUpdate(conn, hostkey, &last_host_update);

    LogPerformanceTimer timer = LogPerformanceStart();
    HubVital *results = CFDB_QueryVitalsMeta(conn, hostkey, last_host_update);
    LogPerformanceStop(&timer, "Listing vitals for host: %s", hostkey);

    EnterpriseDBRelease(conn);

    JsonElement *data = JsonArrayCreate(500);
    for (const HubVital *vital = results; vital; vital = vital->next)
    {
        JsonArrayAppendObject(data, HubVitalToJson(vital));
    }

    DeleteHubVital(results);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_host_vital_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/host/:host/vital/:vital");

    const char *username = NULL; int username_len = 0;
    const char *hostkey = NULL; int hostkey_len = 0;
    const char *vital_id = NULL; int vital_id_len = 0;
    long from = 0;
    long to = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssll",
                              &username, &username_len,
                              &hostkey, &hostkey_len,
                              &vital_id, &vital_id_len,
                              &from,
                              &to) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(hostkey_len, "hostkey");
    ARGUMENT_CHECK_CONTENTS(vital_id_len, "vital_id");

    {
        cfapi_errid err = CFDB_HasHostAccessFromUserRBAC(username, hostkey);
        if (err != ERRID_SUCCESS)
        {
            THROW_GENERIC(err, "Access denied to host");
        }
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    time_t last_update = 0;
    CFDB_QueryLastHostUpdate(conn, hostkey, &last_update);

    HubVital *vital = NULL;
    {
        LogPerformanceTimer timer = LogPerformanceStart();
        cfapi_errid err = CFDB_QueryVital(conn, hostkey, vital_id, last_update, from, to, &vital);
        LogPerformanceStop(&timer, "Getting vital %s for host: %s", vital_id, hostkey);

        EnterpriseDBRelease(conn);
        if (err != ERRID_SUCCESS)
        {
            THROW_GENERIC(err, "Could not lookup vital");
        }
    }
    assert(vital);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubVitalToJson(vital));

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}
