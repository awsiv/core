#include "public-api.h"

#include "db_query.h"
#include "web_rbac.h"
#include "common.h"
#include "conversion.h"
#include "scope.h"
#include "install.h"

static const char *API_NAME = "CFEngine Nova";
static const char *API_VERSION = "v1";

static const char *LABEL_DESCRIPTION = "description";
static const char *LABEL_HOSTKEY = "hostkey";
static const char *LABEL_HOSTKEYS = "hostkeys";
static const char *LABEL_SCOPE = "scope";
static const char *LABEL_NAME = "name";
static const char *LABEL_VALUE = "value";
static const char *LABEL_VERSION = "version";
static const char *LABEL_ARCH = "arch";
static const char *LABEL_TIMESTAMP = "timestamp";
static const char *LABEL_LASTSEEN = "lastseen";
static const char *LABEL_AVERAGE = "average";
static const char *LABEL_STDV = "stdv";
static const char *LABEL_CONTEXT = "context";
static const char *LABEL_PATH = "path";
static const char *LABEL_HANDLE = "handle";
static const char *LABEL_COUNT = "count";
static const char *LABEL_IP = "ip";
static const char *LABEL_TYPE = "type";
static const char *LABEL_ADD = "add";
static const char *LABEL_REMOVE = "remove";
static const char *LABEL_DIFF = "diff";
static const char *LABEL_LINE_NUMBER = "line";

static const char *LABEL_STATE = "state";
static const char *LABEL_STATE_REPAIRED = "repaired";
static const char *LABEL_STATE_NOTKEPT = "notkept";
static const char *LABEL_STATE_KEPT = "kept";
static const char *LABEL_STATE_ANY = "any";

static const char *LABEL_UNKNOWN = "unknown";

/************************************************************************************/

PHP_FUNCTION(cfmod_resource)
{
    EnterpriseDB conn;
    bool db_active = CFDB_Open(&conn);

    if (db_active)
    {
        CFDB_Close(&conn);
    }

    JsonElement *info = JsonObjectCreate(4);

    JsonObjectAppendString(info, "apiName", API_NAME);
    JsonObjectAppendString(info, "apiVersion", API_VERSION);
    JsonObjectAppendString(info, "hubVersion", NOVA_VERSION);
    JsonObjectAppendString(info, "database", db_active ? "connected" : "disconnected");

    JsonElement *output = JsonArrayCreate(1);
    JsonArrayAppendObject(output, info);

    RETURN_JSON(PackageResult(output, 1, 1));
}

PHP_FUNCTION(cfmod_resource_host)
{
    char *username = NULL, *hostname = NULL, *ip = NULL;
    long from = 0,
         to = 0;
    char *context = NULL;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllll",
                              &username, &len,
                              &hostname, &len,
                              &ip, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    }

    Rlist *hostkeys = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        hostkeys = CFDB_QueryHostKeys(&conn, hostname, ip, from, to, filter);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }

    int total = RlistLen(hostkeys);
    PageRecords(&hostkeys, &page, free);

    JsonElement *output = JsonArrayCreate(1000);

    for (Rlist *rp = hostkeys; rp != NULL; rp = rp->next)
    {
        JsonArrayAppendString(output, ScalarValue(rp));
    }

    DeleteRlist(hostkeys);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

PHP_FUNCTION(cfmod_resource_host_id)
{
    char *username = NULL, *hostkey = NULL;
    int len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &len,
                              &hostkey, &len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    }

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(username, hostkey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubHost *record = NULL;
    {
        EnterpriseDB conn;

        DATABASE_OPEN(&conn);

        record = CFDB_GetHostByKey(&conn, hostkey);

        DATABASE_CLOSE(&conn);
    }

    JsonElement *output = JsonArrayCreate(1);
    if (record != NULL)
    {
        JsonElement *entry = JsonObjectCreate(3);

        JsonObjectAppendString(entry, LABEL_HOSTKEY, record->keyhash);
        JsonObjectAppendString(entry, LABEL_NAME, record->hostname);
        JsonObjectAppendString(entry, LABEL_IP, record->ipaddr);

        JsonArrayAppendObject(output, entry);
        DeleteHubHost(record);
    }

    RETURN_JSON(PackageResult(output, 1, 1));
}

/************************************************************************************/

static size_t CountLastSeenRecordsByDirection(const Rlist *records, LastSeenDirection direction)
{
    size_t count = 0;
    for (const Rlist *rp = records; rp != NULL; rp = rp->next)
    {
        HubLastSeen *record = (HubLastSeen *) rp->item;

        if (record->direction == direction)
        {
            count++;
        }
    }
    return count;
}

static JsonElement *HostsLastSeen(const Rlist *records, LastSeenDirection direction)
{
    JsonElement *output = JsonArrayCreate(100);

    for (const Rlist *rp = records; rp != NULL; rp = rp->next)
    {
        HubLastSeen *record = (HubLastSeen *) rp->item;

        if (record->direction == direction)
        {
            JsonElement *entry = JsonObjectCreate(4);

            JsonObjectAppendString(entry, LABEL_HOSTKEY, record->rhost->keyhash);
            JsonObjectAppendInteger(entry, LABEL_LASTSEEN, SECONDS_PER_HOUR * record->hrsago);
            JsonObjectAppendInteger(entry, LABEL_AVERAGE, SECONDS_PER_HOUR * record->hrsavg);
            JsonObjectAppendInteger(entry, LABEL_STDV, SECONDS_PER_HOUR * record->hrsdev);

            JsonArrayAppendObject(output, entry);
        }
    }

    return output;
}

PHP_FUNCTION(cfmod_resource_host_id_seen)
{
    char *username = NULL, *hostkey = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssllll",
                              &username, &len,
                              &hostkey, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryLastSeen(&conn, hostkey, NULL, NULL, NULL, 0.0, from, to,
                                    filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        DATABASE_CLOSE(&conn);
    }
    assert(result);

    int total = CountLastSeenRecordsByDirection(result->records, LAST_SEEN_DIRECTION_OUTGOING);
    PageRecords(&result->records, &page, DeleteHubLastSeen);

    JsonElement *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_OUTGOING);

    DeleteHubQuery(result, DeleteHubLastSeen);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

PHP_FUNCTION(cfmod_resource_host_id_seenby)
{
    char *username = NULL, *hostkey = NULL;
    long from = 0,
         to = 0;
    int len = -1;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssllll",
                              &username, &len,
                              &hostkey, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryLastSeen(&conn, hostkey, NULL, NULL, NULL, 0.0, from, to,
                                    filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(result);

    int total = CountLastSeenRecordsByDirection(result->records, LAST_SEEN_DIRECTION_INCOMING);
    PageRecords(&result->records, &page, DeleteHubLastSeen);

    JsonElement *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_INCOMING);

    DeleteHubQuery(result, DeleteHubLastSeen);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static PromiseState PromiseStateFromString(const char *state)
{
    if (state == NULL)
    {
        return PROMISE_STATE_ANY;
    }
    else if (strcmp(LABEL_STATE_REPAIRED, state) == 0)
    {
        return PROMISE_STATE_REPAIRED;
    }
    else if (strcmp(LABEL_STATE_NOTKEPT, state) == 0)
    {
        return PROMISE_STATE_NOTKEPT;
    }
    else if (strcmp(LABEL_STATE_KEPT, state) == 0)
    {
        return PROMISE_STATE_KEPT;
    }
    else
    {
        return PROMISE_STATE_ANY;
    }
}

static const char *PromiseStateToString(PromiseState state)
{
    switch (state)
    {
    case PROMISE_STATE_REPAIRED:
        return LABEL_STATE_REPAIRED;
    case PROMISE_STATE_NOTKEPT:
        return LABEL_STATE_NOTKEPT;
    case PROMISE_STATE_KEPT:
        return LABEL_STATE_KEPT;
    default:
    case PROMISE_STATE_ANY:
        // FIX: probably should never happen, add warning?
        return LABEL_STATE_ANY;
    }
}

PHP_FUNCTION(cfmod_resource_promise_compliance)
{
    char *username = NULL, *handle = NULL, *hostkey = NULL, *context = NULL, *state = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllll",
                              &username, &len,
                              &handle, &len,
                              &hostkey, &len,
                              &context, &len,
                              &state, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryPromiseCompliance(&conn, hostkey, handle, PromiseStateFromString(state),
                                             from, to, filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                             (QUERY_FLAG_IS_REGEX | QUERY_FLAG_SORT_RESULT));

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        DATABASE_CLOSE(&conn);
    }
    assert(result);

    int total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubPromiseCompliance);

    JsonElement *output = JsonArrayCreate(total);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *record = (HubPromiseCompliance *) rp->item;
        JsonElement *entry = JsonObjectCreate(6);

        JsonObjectAppendString(entry, LABEL_HANDLE, record->handle);
        JsonObjectAppendString(entry, LABEL_HOSTKEY, record->hh->keyhash);
        JsonObjectAppendString(entry, LABEL_STATE, PromiseStateToString(record->status));
        JsonObjectAppendReal(entry, LABEL_AVERAGE, record->e);
        JsonObjectAppendReal(entry, LABEL_STDV, record->d);
        JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, record->t);

        JsonArrayAppendObject(output, entry);
    }

    DeleteHubQuery(result, DeleteHubPromiseCompliance);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static const char *PromiseLogStateToString(PromiseLogState state)
{
    switch (state)
    {
    case PROMISE_LOG_STATE_REPAIRED:
        return LABEL_STATE_REPAIRED;
    case PROMISE_LOG_STATE_NOTKEPT:
        return LABEL_STATE_NOTKEPT;
    default:
        return LABEL_UNKNOWN;
    }
}

static JsonElement *PromiseLogAsJson(EnterpriseDB *conn, PromiseLogState state, const char *handle, const char *cause_rx,
                                     const char *hostkey, int from, int to, HostClassFilter *filter, PageInfo *page,
                                     int *total_results_out)
{
    HubQuery *result = CFDB_QueryPromiseLog(conn, hostkey, state, handle, true, cause_rx, from, to, true,
                                            filter, total_results_out, PROMISE_CONTEXT_MODE_ALL, NULL);
    PageRecords(&(result->records), page, DeleteHubPromiseLog);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubPromiseLog *log_entry = (HubPromiseLog *) rp->item;
        JsonElement *entry = JsonObjectCreate(5);

        JsonObjectAppendString(entry, LABEL_HANDLE, log_entry->handle);
        JsonObjectAppendString(entry, LABEL_HOSTKEY, log_entry->hh->keyhash);
        JsonObjectAppendString(entry, LABEL_DESCRIPTION, log_entry->cause);
        JsonObjectAppendString(entry, LABEL_STATE, PromiseLogStateToString(state));
        JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, log_entry->t);

        JsonArrayAppendObject(output, entry);
    }

    DeleteHubQuery(result, DeleteHubPromiseLog);

    return output;
}

PHP_FUNCTION(cfmod_resource_promise_log_repaired)
{
    char *username = NULL, *handle = NULL, *hostkey = NULL, *context = NULL, *cause_rx = NULL;
    long from = 0,
         to = 0;
    PageInfo page;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllll",
                              &username, &len,
                              &handle, &len,
                              &cause_rx, &len,
                              &hostkey, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *output = NULL;
    int total = 0;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, cause_rx, hostkey, from, to,
                                  filter, &page, &total);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(output);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static JsonElement *PromiseLogSummaryAsJson(EnterpriseDB *conn, PromiseLogState state, const char *handle, const char *cause_rx,
                                            const char *hostkey, int from, int to, HostClassFilter *filter, PageInfo *page,
                                            int *total_results_out)
{
    HubQuery *result = CFDB_QueryPromiseLogSummary(conn, hostkey, state, handle, true, cause_rx, from, to, true,
                                                   filter, PROMISE_CONTEXT_MODE_ALL, NULL);

    if (total_results_out)
    {
        *total_results_out = RlistLen(result->records);
    }

    PageRecords(&result->records, page, DeleteHubPromiseSum);

    JsonElement *output = JsonArrayCreate(total_results_out ? *total_results_out : 100);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubPromiseSum *record = (const HubPromiseSum *)rp->item;
        JsonElement *entry = JsonObjectCreate(4);

        JsonObjectAppendString(entry, LABEL_HANDLE, record->handle);
        JsonObjectAppendString(entry, LABEL_DESCRIPTION, record->cause);
        JsonObjectAppendInteger(entry, LABEL_COUNT, record->occurences);
        JsonObjectAppendString(entry, LABEL_STATE, PromiseLogStateToString(state));

        JsonArrayAppendObject(output, entry);
    }

    DeleteHubQuery(result, DeleteHubPromiseSum);

    return output;
}

PHP_FUNCTION(cfmod_resource_promise_log_repaired_summary)
{
    char *username = NULL, *handle = NULL, *hostkey = NULL, *context = NULL, *cause_rx = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllll",
                              &username, &len,
                              &handle, &len,
                              &cause_rx, &len,
                              &hostkey, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *output = NULL;
    int total = 0;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, cause_rx, hostkey, from, to,
                                         filter, &page, &total);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(output);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

PHP_FUNCTION(cfmod_resource_promise_log_notkept)
{
    char *username = NULL, *handle = NULL, *hostkey = NULL, *context = NULL, *cause_rx = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllll",
                              &username, &len,
                              &handle, &len,
                              &cause_rx, &len,
                              &hostkey, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *output = NULL;
    int total = 0;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, cause_rx, hostkey, from, to,
                                  filter, &page, &total);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        DATABASE_CLOSE(&conn);
    }
    assert(output);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

PHP_FUNCTION(cfmod_resource_promise_log_notkept_summary)
{
    char *username = NULL, *handle = NULL, *hostkey = NULL, *context = NULL, *cause_rx = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllll",
                              &username, &len,
                              &handle, &len,
                              &cause_rx, &len,
                              &hostkey, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *output = NULL;
    int total = 0;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, cause_rx, hostkey, from, to,
                                         filter, &page, &total);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(output);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static const char *DeserializeRvalType(const char *datatype)
{
    if (strcmp("sl", datatype) == 0)
    {
        return "slist";
    }
    if (strcmp("rl", datatype) == 0)
    {
        return "rlist";
    }
    if (strcmp("il", datatype) == 0)
    {
        return "ilist";
    }
    else if (strcmp("ml", datatype) == 0)
    {
        return "mlist";
    }

    else if (strcmp("s", datatype) == 0)
    {
        return "string";
    }
    else if (strcmp("r", datatype) == 0)
    {
        return "real";
    }
    else if (strcmp("i", datatype) == 0)
    {
        return "int";
    }
    else if (strcmp("m", datatype) == 0)
    {
        return "menu";
    }

    assert(false && "Cannot deserialize rval datatype");
    return "unknown";
}

static const char *SerializeRvalType(const char *str)
{
    if (strcmp("slist", str) == 0)
    {
        return "sl";
    }
    if (strcmp("rlist", str) == 0)
    {
        return "rl";
    }
    if (strcmp("ilist", str) == 0)
    {
        return "il";
    }
    else if (strcmp("mlist", str) == 0)
    {
        return "ml";
    }

    else if (strcmp("string", str) == 0)
    {
        return "s";
    }
    else if (strcmp("real", str) == 0)
    {
        return "r";
    }
    else if (strcmp("int", str) == 0)
    {
        return "i";
    }
    else if (strcmp("menu", str) == 0)
    {
        return "m";
    }

    assert(false && "Cannot serialize rval datatype");
    return NULL;                // from core conversion.c
}

PHP_FUNCTION(cfmod_resource_variable)
{
    char *username = NULL, *hostkey = NULL, *scope = NULL, *name = NULL, *value = NULL, *type = NULL, *context = NULL;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssssllll",
                              &username, &len,
                              &hostkey, &len,
                              &scope, &len,
                              &name, &len,
                              &value, &len,
                              &type, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        {
            char ns[CF_MAXVARSIZE] = { 0 };
            char bundle[CF_MAXVARSIZE] = { 0 };
            SplitScopeName(scope, ns, bundle);

            result = CFDB_QueryVariables(&conn, hostkey, ns, bundle, name, value,
                                         SerializeRvalType(type), from, to, filter,
                                         PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX);
        }

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(result);

    int total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubVariable);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubVariable *record = (HubVariable *) rp->item;
        const char *datatype = DeserializeRvalType(record->dtype);

        JsonElement *value_entry = JsonObjectCreate(3);

        JsonObjectAppendString(value_entry, LABEL_HOSTKEY, record->hh->keyhash);
        {
            char record_scope[CF_MAXVARSIZE] = { 0 };
            JoinScopeName(record->ns, record->bundle, record_scope);

            JsonObjectAppendString(value_entry, LABEL_SCOPE, record_scope);
        }
        JsonObjectAppendString(value_entry, LABEL_NAME, record->lval);
        JsonObjectAppendString(value_entry, LABEL_TYPE, datatype);

        if (strstr(datatype, "list"))
        {
            JsonObjectAppendArray(value_entry, LABEL_VALUE, RvalToJson(record->rval));
        }
        else                    // FIX: should condition on int/real/menu. possible?
        {
            JsonObjectAppendString(value_entry, LABEL_VALUE, (const char *) record->rval.item);
        }

        JsonArrayAppendObject(output, value_entry);
    }

    DeleteHubQuery(result, DeleteHubVariable);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

PHP_FUNCTION(cfmod_resource_context)
{
    char *username = NULL, *hostkey = NULL, *context = NULL, *name = NULL;
    int len;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllll",
                              &username, &len,
                              &hostkey, &len,
                              &context, &len,
                              &name, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryClasses(&conn, hostkey, name, from, to, filter,
                                   PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(result);

    int total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubClass);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubClass *record = (HubClass *) rp->item;
        JsonElement *entry = JsonObjectCreate(5);

        JsonObjectAppendString(entry, LABEL_HOSTKEY, record->hh->keyhash);
        JsonObjectAppendString(entry, LABEL_CONTEXT, record->class);
        JsonObjectAppendReal(entry, LABEL_AVERAGE, record->prob);
        JsonObjectAppendReal(entry, LABEL_STDV, record->dev);
        JsonObjectAppendInteger(entry, LABEL_LASTSEEN, record->t);

        JsonArrayAppendObject(output, entry);
    }

    DeleteHubQuery(result, DeleteHubClass);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static JsonElement *SoftwareHostsEntryCreate(HubSoftware *software)
{
    JsonElement *entry = JsonObjectCreate(4);

    JsonObjectAppendString(entry, LABEL_NAME, software->name);
    JsonObjectAppendString(entry, LABEL_VERSION, software->version);
    JsonObjectAppendString(entry, LABEL_ARCH, Nova_LongArch(software->arch));

    JsonElement *hostkeys = JsonArrayCreate(100);

    JsonArrayAppendString(hostkeys, software->hh->keyhash);
    JsonObjectAppendArray(entry, LABEL_HOSTKEYS, hostkeys);

    return entry;
}

static JsonElement *SoftwareHostsEntryFind(JsonElement *entries, HubSoftware *software)
{
    for (size_t i = 0; i < JsonElementLength(entries); i++)
    {
        JsonElement *entry = JsonArrayGetAsObject(entries, i);

        if (strcmp(JsonObjectGetAsString(entry, LABEL_NAME), software->name) == 0 &&
            strcmp(JsonObjectGetAsString(entry, LABEL_VERSION), software->version) == 0 &&
            strcmp(JsonObjectGetAsString(entry, LABEL_ARCH), Nova_LongArch(software->arch)) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

PHP_FUNCTION(cfmod_resource_software)
{
    char *username = NULL, *hostkey = NULL, *name = NULL, *version = NULL, *arch = NULL, *context = NULL;
    int len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssll",
                              &username, &len,
                              &hostkey, &len,
                              &name, &len,
                              &version, &len,
                              &arch, &len,
                              &context, &len,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QuerySoftware(&conn, hostkey, cfr_software, name, version,
                                    Nova_ShortArch(arch), filter,
                                    PROMISE_CONTEXT_MODE_ALL, NULL,
                                    QUERY_FLAG_IS_REGEX | QUERY_FLAG_SORT_RESULT);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    }
    assert(result);

    int total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubSoftware);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *record = (HubSoftware *) rp->item;
        JsonElement *entry = SoftwareHostsEntryFind(output, record);

        if (entry)
        {
            JsonElement *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);

            JsonArrayAppendString(hostkeys, record->hh->keyhash);
        }
        else
        {
            entry = SoftwareHostsEntryCreate(record);
            JsonArrayAppendObject(output, entry);
        }
    }

    DeleteHubQuery(result, DeleteHubSoftware);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

static JsonElement *SetUidHostsEntryCreate(HubSetUid *setuid)
{
    JsonElement *entry = JsonObjectCreate(2);

    JsonObjectAppendString(entry, LABEL_PATH, setuid->path);

    JsonElement *hostkeys = JsonArrayCreate(100);

    JsonArrayAppendString(hostkeys, setuid->hh->keyhash);
    JsonObjectAppendArray(entry, LABEL_HOSTKEYS, hostkeys);

    return entry;
}

static JsonElement *SetUidHostsEntryFind(JsonElement *entries, HubSetUid *setuid)
{
    for (size_t i = 0; i < JsonElementLength(entries); i++)
    {
        JsonElement *entry = JsonArrayGetAsObject(entries, i);

        if (strcmp(JsonObjectGetAsString(entry, LABEL_PATH), setuid->path) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

PHP_FUNCTION(cfmod_resource_setuid)
{
    char *username = NULL, *hostkey = NULL, *path = NULL, *context = NULL;
    int len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssll",
                              &username, &len,
                              &hostkey, &len,
                              &path, &len,
                              &context, &len,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QuerySetuid(&conn, hostkey, path, filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX);

        DATABASE_CLOSE(&conn);
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(result);

    int total = RlistLen(result->records);
    PageRecords(&result->records, &page, DeleteHubSetUid);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
    {
        HubSetUid *record = (HubSetUid *) rp->item;
        JsonElement *entry = SetUidHostsEntryFind(output, record);

        if (entry)
        {
            JsonElement *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);

            JsonArrayAppendString(hostkeys, record->hh->keyhash);
        }
        else
        {
            entry = SetUidHostsEntryCreate(record);
            JsonArrayAppendObject(output, entry);
        }
    }

    DeleteHubQuery(result, DeleteHubSetUid);

    RETURN_JSON(PackageResult(output, page.pageNum, total));
}

/************************************************************************************/

bool FileRecordsEqual(const HubFileChanges *change_record, const HubFileDiff *diff_record)
{
    if (!change_record || !diff_record)
    {
        return false;
    }

    if (strcmp(change_record->hh->keyhash, diff_record->hh->keyhash) != 0)
    {
        return false;
    }

    return (strcmp(change_record->path, diff_record->path) == 0) && (change_record->t == diff_record->t);
}

JsonElement *HubFileChangesToJson(const HubFileChanges *change_record)
{
    JsonElement *entry = JsonObjectCreate(4);

    JsonObjectAppendString(entry, LABEL_HOSTKEY, change_record->hh->keyhash);
    JsonObjectAppendString(entry, LABEL_PATH, change_record->path);
    JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, change_record->t);

    if (change_record->diff_record)
    {
        char diff_buffer[CF_BUFSIZE];

        diff_buffer[0] = '\0';
        char diff_type = -1;
        int line = -1;

        sscanf(change_record->diff_record->diff, "%c,%d,%s", &diff_type, &line, diff_buffer);

        JsonElement *diff_entry = JsonObjectCreate(3);

        switch (diff_type)
        {
        case '+':
            JsonObjectAppendString(diff_entry, LABEL_TYPE, LABEL_ADD);
            break;
        case '-':
            JsonObjectAppendString(diff_entry, LABEL_TYPE, LABEL_REMOVE);
            break;
        default:
            JsonObjectAppendString(diff_entry, LABEL_TYPE, LABEL_UNKNOWN);
            break;
        }

        JsonObjectAppendInteger(diff_entry, LABEL_LINE_NUMBER, line);
        JsonObjectAppendString(diff_entry, LABEL_VALUE, diff_buffer);

        JsonObjectAppendObject(entry, LABEL_DIFF, diff_entry);
    }

    return entry;
}

PHP_FUNCTION(cfmod_resource_file)
{
    char *username = NULL, *hostkey = NULL, *path = NULL, *context = NULL;
    int len;
    long from = 0,
         to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllll",
                              &username, &len,
                              &hostkey, &len,
                              &path, &len,
                              &context, &len,
                              &from,
                              &to,
                              &page.pageNum,
                              &page.resultsPerPage) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *change_result = NULL;
    HubQuery *diff_result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddClasses(filter, context, NULL);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        change_result = CFDB_QueryFileChanges(&conn, hostkey, path, (time_t)from, (time_t)to,
                                              filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                              QUERY_FLAG_IS_REGEX | QUERY_FLAG_SORT_RESULT);
        diff_result = CFDB_QueryFileDiff(&conn, hostkey, path, NULL, (time_t)from, (time_t)to,
                                         filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                         QUERY_FLAG_IS_REGEX | QUERY_FLAG_SORT_RESULT);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(change_result);
    assert(diff_result);

    int total = RlistLen(change_result->records);
    PageRecords(&change_result->records, &page, DeleteHubFileChanges);

    for (const Rlist *diffp = diff_result->records; diffp; diffp = diffp->next)
    {
        HubFileDiff *diff_record = (HubFileDiff *)diffp->item;

        for (const Rlist *changep = change_result->records; changep; changep = changep->next)
        {
            HubFileChanges *change_record = (HubFileChanges *)changep->item;

            if (FileRecordsEqual(change_record, diff_record))
            {
                change_record->diff_record = diff_record;
                break;
            }
        }
    }

    JsonElement *data = JsonArrayCreate(1000);
    for (const Rlist *rp = change_result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubFileChangesToJson((const HubFileChanges *)rp->item));
    }

    DeleteHubQuery(change_result, DeleteHubFileChanges);
    DeleteHubQuery(diff_result, DeleteHubFileDiff);

    RETURN_JSON(PackageResult(data, page.pageNum, total));
}
