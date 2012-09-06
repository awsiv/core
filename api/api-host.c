#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"
#include "granules.h"


PHP_FUNCTION(cfapi_host_list)
{
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

    HostClassFilter *context_filter = NULL;
    {
        HubQuery *result = CFDB_HostClassFilterFromUserRBAC(username);
        if (result->errid != ERRID_RBAC_DISABLED && result->errid != ERRID_SUCCESS)
        {
            THROW_GENERIC(result->errid, "Access denied");
        }

        context_filter = (HostClassFilter *)HubQueryGetFirstRecord(result);
        HostClassFilterAddIncludeExcludeLists(context_filter, context_include, context_exclude);
    }

    EnterpriseDB *conn = EnterpriseDBAcquire();
    if (!conn)
    {
        THROW_GENERIC(ERRID_DBCONNECT, "Unable to connect to database");
    }

    HubQuery *result = CFDB_QueryHostsByHostClassFilter(conn, context_filter);

    if (!EnterpriseDBRelease(conn))
    {
        DeleteHubQuery(result, NULL);
        THROW_GENERIC(ERRID_DBCLOSE, "Unable to close database");
    }

    const size_t total = RlistLen(result->hosts);
    PageRecords(&result->hosts, &page, DeleteHubHost);

    JsonElement *data = JsonArrayCreate(1);
    for (const Rlist *rp = result->hosts; rp; rp = rp->next)
    {
        const HubHost *host = rp->item;
        JsonArrayAppendObject(data, HubHostToJson(host));
    }

    DeleteHubQuery(result, NULL);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}


PHP_FUNCTION(cfapi_host_get)
{
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

PHP_FUNCTION(cfapi_host_context_list)
{
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

    HubQuery *result = CFDB_QueryClasses(conn, hostkey, NULL, false, 0, time(NULL), NULL, false);

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

    HubVital *results = CFDB_QueryVitalsMeta(conn, hostkey);

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
        cfapi_errid err = CFDB_QueryVital(conn, hostkey, vital_id, last_update, from, to, &vital);
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
