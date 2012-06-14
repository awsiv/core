#include "api.h"

#include "reporting-engine.h"
#include "utils.h"

static const char *API_NAME = "CFEngine Enterprise API";
static const char *API_VERSION = "v2";

PHP_FUNCTION(cfapi)
{
    JsonElement *info = JsonObjectCreate(4);

    JsonObjectAppendString(info, "apiName", API_NAME);
    JsonObjectAppendString(info, "apiVersion", API_VERSION);

    JsonElement *output = JsonArrayCreate(1);
    JsonArrayAppendObject(output, info);

    RETURN_JSON(output);
}

PHP_FUNCTION(cfapi_query)
{
    const char *username = NULL; int username_len = 0;
    const char *query = NULL; int query_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &query, &query_len) == FAILURE)
    {
        zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && query_len);

    JsonElement *query_parsed = JsonParse(&query);
    if (!query_parsed)
    {
        zend_throw_exception(cfapi_exception_bad_request, LABEL_ERROR_INVALID_JSON, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *result = ReportingEngineQuery(query_parsed);
    JsonElementDestroy(query_parsed);

    assert(result);

    RETURN_JSON(result);
}
