/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_COMMON_H
#define CFENGINE_CFMOD_COMMON_H

#define LABEL_ERROR_DATABASE_OPEN "Unable to connect to database"
#define LABEL_ERROR_DATABASE_CLOSE "Unable to close to database"
#define LABEL_ERROR_ARGS "Incorrect argument count or types"
#define LABEL_ERROR_ARGS_EMPTY "Missing argument contents"
#define LABEL_ERROR_NOTIMPLEMENTED "Not implemented"

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

#define RETURN_JSON_ARRAY(json) \
   Writer *writer = StringWriter(); \
   JsonArrayPrint(writer, json, 0); \
   JsonArrayDelete(json);           \
   RETURN_STRING(StringWriterClose(writer), 1);

#define RETURN_JSON_OBJECT(json) \
   Writer *writer = StringWriter(); \
   JsonObjectPrint(writer, json, 0); \
   JsonObjectDelete(json);           \
   RETURN_STRING(StringWriterClose(writer), 1);

#endif
