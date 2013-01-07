#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "file_cache.h"
#include "crypto.h"
#include "reporting_engine.h"
#include "files_hashes.h"
#include "string_lib.h"

ZEND_EXTERN_MODULE_GLOBALS(cfapi);

static const time_t CACHE_EXPIRATION = (5 * 60);

static char *CacheKey(const char *username, const char *query)
{
    char digest[EVP_MAX_MD_SIZE + 1] = { 0 };

    HashString(username, strlen(username), digest, cf_md5);
    HashString(query, strlen(query), digest, cf_md5);

    return HashPrint(cf_md5, digest);
}

static int RowSort(const JsonElement *row_a, const JsonElement *row_b, void *_column_index)
{
    int *column_index = _column_index;
    const JsonElement *a = JsonAt(row_a, *column_index);
    const JsonElement *b = JsonAt(row_b, *column_index);

    JsonPrimitiveType type = JsonGetPrimitiveType(a);
    if (JsonGetPrimitiveType(b) != type)
    {
        // prioritize columns with non-null values
        return type == JSON_PRIMITIVE_TYPE_NULL ? 1 : -1;
    }

    assert(JsonPrimitiveType(a) == JsonPrimitiveType(b));

    switch (type)
    {
    case JSON_PRIMITIVE_TYPE_STRING:
        return StringSafeCompare(JsonPrimitiveGetAsString(a), JsonPrimitiveGetAsString(b));
    case JSON_PRIMITIVE_TYPE_INTEGER:
        return JsonPrimitiveGetAsInteger(a) - JsonPrimitiveGetAsInteger(b);
    case JSON_PRIMITIVE_TYPE_REAL:
        return JsonPrimitiveGetAsReal(a) - JsonPrimitiveGetAsReal(b);
    case JSON_PRIMITIVE_TYPE_BOOL:
        {
            bool a_val = JsonPrimitiveGetAsBool(a);
            bool b_val = JsonPrimitiveGetAsBool(b);
            if (a_val == b_val)
            {
                return 0;
            }
            else if (a_val)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
    default:
    case JSON_PRIMITIVE_TYPE_NULL:
        return 0;
    }
}

static int ColumnIndex(const JsonElement *column_array, const char *column)
{
    JsonIterator iter = JsonIteratorInit(column_array);
    const JsonElement *value = NULL;
    int index = 0;
    while ((value = JsonIteratorNextValue(&iter)) != NULL)
    {
        if (StringSafeEqual(column, JsonPrimitiveGetAsString(value)))
        {
            return index;
        }
        index++;
    }
    return -1;
}

PHP_FUNCTION(cfapi_query_post)
{
    syslog(LOG_DEBUG, "Requesting POST /api/query");

    const char *username = NULL; int username_len = 0;
    const char *query = NULL; int query_len = 0;
    const char *sort_column = 0; int sort_column_len = 0;
    bool sort_descending;
    long skip = 0, limit = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbll",
                              &username, &username_len,
                              &query, &query_len,
                              &sort_column, &sort_column_len,
                              &sort_descending,
                              &skip,
                              &limit) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");

    FileCacheRemoveExpired(CFAPI_G(query_cache), time(NULL) - CACHE_EXPIRATION);
    char *cache_key = CacheKey(username, query);

    JsonElement *table = NULL;
    {
        FileCacheEntry cache_entry = FileCacheGet(CFAPI_G(query_cache), cache_key);
        if (cache_entry.value)
        {
            const char *data = cache_entry.value;
            table = JsonParse(&data);
            assert(table);
            JsonObjectAppendBool(table, "cached", true);
        }
        else
        {
            table = EnterpriseExecuteSQLSync(username, query);
            assert(table);
            Writer *writer = StringWriter();
            JsonElementPrint(writer, table, 0);
            FileCachePut(CFAPI_G(query_cache), cache_key, StringWriterData(writer), StringWriterLength(writer) + 1);
            WriterClose(writer);
            JsonObjectAppendBool(table, "cached", false);
        }
        FileCacheEntryDestroy(&cache_entry);
    }

    JsonElement *rows = JsonObjectGet(table, "rows");

    if (sort_column_len > 0)
    {
        int column_index = ColumnIndex(JsonObjectGet(table, "header"), sort_column);
        if (column_index >= 0)
        {
            syslog(LOG_DEBUG, "Sorting query result on column %s", sort_column);
            JsonSort(rows, RowSort, &column_index);
        }
        else
        {
            syslog(LOG_DEBUG, "Invalid sorting column selected (%s) for query %s", sort_column, query);
        }
        JsonObjectAppendString(table, "sortColumn", sort_column);
    }

    if (sort_descending)
    {
        syslog(LOG_DEBUG, "Reversing order of rows for query: %s", query);
        JsonContainerReverse(rows);
    }
    JsonObjectAppendBool(table, "sortDescending", sort_descending);

    if (skip > 0)
    {
        skip = MIN(skip, JsonElementLength(rows));
        syslog(LOG_DEBUG, "Skipping %lu results for query %s", skip, query);
        JsonArrayRemoveRange(rows, 0, skip - 1);
        JsonObjectAppendInteger(table, "skip", skip);
    }
    if (limit >= 0)
    {
        if (limit < JsonElementLength(rows))
        {
            syslog(LOG_DEBUG, "Limiting %lu results for query %s", limit, query);
            JsonArrayRemoveRange(rows, limit, JsonElementLength(rows) - 1);
        }
        JsonObjectAppendInteger(table, "limit", limit);
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, table);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_query_async_post)
{
    syslog(LOG_DEBUG, "Requesting POST /api/query/async");

    const char *username = NULL; int username_len = 0;
    const char *query = NULL; int query_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &query, &query_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(query_len, "query");

    JsonElement *payload = EnterpriseExecuteSQLAsync(username, query);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, payload);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_query_async_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/query/async/:token");

    const char *username = NULL; int username_len = 0;
    const char *token = NULL; int token_len = 0;
    char *static_files_uri = NULL; int static_files_uri_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &token, &token_len,
                              &static_files_uri, &static_files_uri_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(token_len, "token");
    ARGUMENT_CHECK_CONTENTS(static_files_uri_len, "static_files_uri");

    syslog(LOG_DEBUG, "Looking up status for async query %s", token);

    JsonElement *payload = AsyncQueryStatus(token, REPORT_FORMAT_CSV, NULLStringToEmpty(static_files_uri));
    assert(payload);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, payload);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_query_async_delete)
{
    syslog(LOG_DEBUG, "Requesting DELETE /api/query/async/:token");

    const char *username = NULL; int username_len = 0;
    const char *token = NULL; int token_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &token, &token_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(token_len, "token");

    JsonElement *payload = AsyncQueryAbort(token);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, payload);

    RETURN_JSON(PackageResult(data, 1, 1));
}
