#include "public-api.h"

#include "db_query.h"

#define RETURN_JSON(json) \
   Writer *writer = StringWriter(); \
   JsonArrayPrint(writer, json, 0); \
   JsonArrayDelete(json);           \
   RETURN_STRING(StringWriterClose(writer), 1);

PHP_FUNCTION(cfmod_resource_host_list)
{
int len;
char *hostname;
char *ip;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
      &hostname, &len,
      &ip, &len) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, "Unable to parse arguments", 0 TSRMLS_CC);
   }

// TODO: more query parameters

mongo_connection conn;
if (!CFDB_Open(&conn))
   {
   zend_throw_exception(cfmod_exception_db, "Unable to connect to database", 0 TSRMLS_CC);
   }

JsonArray *host_keys = CFDB_QueryHostKeys(&conn, hostname, ip);

RETURN_JSON(host_keys);
}

PHP_FUNCTION(cfmod_resource_report_list)
{
JsonArray *reports = NULL;

for (struct ReportInfo *report = BASIC_REPORTS; report->id != NULL; report++)
   {
   JsonArrayAppendString(&reports, report->id);
   }

RETURN_JSON(reports);
}

