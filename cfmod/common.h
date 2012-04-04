/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_COMMON_H
#define CFENGINE_CFMOD_COMMON_H

#include "php_cfmod.h"

#define LABEL_ERROR_DATABASE_OPEN "Unable to connect to database"
#define LABEL_ERROR_DATABASE_CLOSE "Unable to close to database"
#define LABEL_ERROR_ARGS "Incorrect argument count or types"
#define LABEL_ERROR_ARGS_EMPTY "Missing argument contents"
#define LABEL_ERROR_NOTIMPLEMENTED "Not implemented"
#define LABEL_ERROR_RBAC_NOT_ADMIN "Only members of the admin role has access to this information"

// FIX: symptom of something
#define CF_WEBBUFFER 7*1024*1024

#include "db_query.h"

#define DATABASE_OPEN(connection) \
   if (!CFDB_Open(connection))\
   {\
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_DATABASE_OPEN, 0 TSRMLS_CC);\
   RETURN_NULL();\
   }\

#define DATABASE_CLOSE(connection) \
   if (!CFDB_Close(connection))\
   {\
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_DATABASE_CLOSE, 0 TSRMLS_CC);\
   RETURN_NULL();\
   }\

#define ARGUMENT_CHECK_CONTENTS(cond) \
 if(!(cond))                          \
    {                                 \
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS_EMPTY, 0 TSRMLS_CC); \
    RETURN_NULL();                    \
    }

#define ERRID_RBAC_CHECK(hq, DeleteFunction)           \
 if(hq->errid != ERRID_RBAC_DISABLED && hq->errid != ERRID_SUCCESS)       \
    {                                             \
    cfapi_errid errid = hq->errid;                \
    DeleteHubQuery(hq, DeleteFunction);           \
    zend_throw_exception(cfmod_exception_rbac, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);  \
    RETURN_NULL();                                \
    }                                             \


#include "json.h"

#define RETURN_JSON(json) \
   Writer *writer = StringWriter(); \
   JsonElementPrint(writer, json, 0); \
   JsonElementDestroy(json); \
   char *JSON_buf = StringWriterClose(writer); \
   char *JSON_ebuf = estrdup(JSON_buf); \
   free(JSON_buf); \
   RETURN_STRING(JSON_ebuf, 0);

void HostClassFilterAddIncludeExcludeLists(HostClassFilter *filter, zval * includes, zval * excludes);

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t count);
JsonElement *JsonObjectWrapper(JsonElement *data, int total_result_count);

#endif
