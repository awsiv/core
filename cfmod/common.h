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

void HostClassFilterAddIncludeExcludeLists(HostClassFilter *filter, zval * includes, zval * excludes);

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t total);
JsonElement *JsonObjectWrapper(JsonElement *data, int total_result_count);

#include "web-report-export.h"

WebReportFileInfo *PHPArrayWebReportFileInfoGet( zval *php_array );
bool PHPArrayBoolGet(zval *php_array, char *key, bool *out);
bool PHPArrayStringGet(zval *php_array, char *key, char *buffer, int bufsize);
bool PHPArrayIsEmpty(zval *php_array);

#define PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info) \
if( report_file_info_array && !PHPArrayIsEmpty(report_file_info_array) ) \
{ \
    report_file_info = PHPArrayWebReportFileInfoGet(report_file_info_array); \
    if( !report_file_info ) \
    { \
        zend_throw_exception(cfmod_exception_args, "Incorrect/Incomplete arguments for CSV/PDF esporting", 0 TSRMLS_CC); \
        RETURN_NULL(); \
    } \
} \

#endif
