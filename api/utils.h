/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_API_UTILS_H
#define CFENGINE_API_UTILS_H

#include "api-php.h"
#include "json.h"

#define LABEL_ERROR_ARGS "Incorrect argument count or types"
#define LABEL_ERROR_ARGS_EMPTY "Missing argument contents"
#define LABEL_ERROR_INVALID_JSON "Invalid JSON payload"

#define ARGUMENT_CHECK_CONTENTS(cond) \
 if(!(cond))                          \
    {                                 \
    zend_throw_exception(cfapi_exception_args, LABEL_ERROR_ARGS_EMPTY, 0 TSRMLS_CC); \
    RETURN_NULL();                    \
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

#endif
