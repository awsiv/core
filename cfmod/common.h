/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_COMMON_H
#define CFENGINE_CFMOD_COMMON_H

#define LABEL_ERROR_DATABASE_OPEN "Unable to connect to database"
#define LABEL_ERROR_DATABASE_CLOSE "Unable to close to database"
#define LABEL_ERROR_ARGS "Incorrect argument count or types"
#define LABEL_ERROR_NOTIMPLEMENTED "Not implemented"

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

#endif
