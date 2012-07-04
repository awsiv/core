/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_API_UTILS_H
#define CFENGINE_API_UTILS_H

#include "api-php.h"
#include "json.h"
#include "db_common.h"

#define THROW_GENERIC(code, msg, ...) \
    { \
        char *buffer = NULL; \
        xasprintf(&buffer, msg, ##__VA_ARGS__); \
        zend_throw_exception(cfapi_exception, buffer, code TSRMLS_CC); \
        free(buffer); \
        RETURN_NULL(); \
    }

#define THROW_ARGS_MISSING() \
    { \
        zend_throw_exception(cfapi_exception, "Wrong number or arguments", ERRID_ARGUMENT_MISSING TSRMLS_CC); \
        RETURN_NULL(); \
    }

#define ARGUMENT_CHECK_CONTENTS(cond) \
 if(!(cond)) \
    { \
    zend_throw_exception(cfapi_exception, "One or more required arguments are empty", ERRID_ARGUMENT_WRONG TSRMLS_CC); \
    RETURN_NULL(); \
    }

#define RETURN_JSON(json) \
{ \
   JsonElement *__return_json_json = json; \
   Writer *__return_json_writer = StringWriter(); \
   JsonElementPrint(__return_json_writer, __return_json_json, 0); \
   JsonElementDestroy(__return_json_json); \
   char *__return_json_output = StringWriterClose(__return_json_writer); \
   char *__return_json_output_php = estrdup(__return_json_output); \
   free(__return_json_output); \
   RETURN_STRING(__return_json_output_php, 0); \
}

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t total);

EnterpriseDB *EnterpriseDBAcquire(void);
bool EnterpriseDBRelease(EnterpriseDB *conn);

#endif
