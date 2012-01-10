#include "public-api.h"

#include "db_query.h"

#define RETURN_JSON(json) \
   Writer *writer = StringWriter(); \
   JsonArrayPrint(writer, json, 0); \
   JsonArrayDelete(json);           \
   RETURN_STRING(StringWriterClose(writer), 1);


PHP_FUNCTION(cfmod_resource_report_list)
{
JsonArray *reports = NULL;

for (struct ReportInfo *report = BASIC_REPORTS; report->id != NULL; report++)
   {
   JsonObject *report_entry = NULL;
   JsonObjectAppendString(&report_entry, "id", report->id);
   JsonObjectAppendString(&report_entry, "category", report->category);
   JsonObjectAppendString(&report_entry, "description", report->description);

   JsonArrayAppendObject(&reports, report_entry);
   }

RETURN_JSON(reports);
}

/************************************************************************************/

PHP_FUNCTION(cfmod_resource_report_software_installed)
{
char *hostkey = NULL,
     *name = NULL,
     *version = NULL,
     *arch = NULL,
     *context = NULL;
int len;
struct PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssll",
      &hostkey, &len,
      &name, &len,
      &version, &len,
      &arch, &len,
      &context, &len,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, "Unable to parse arguments", 0 TSRMLS_CC);
   }

mongo_connection conn;
if (!CFDB_Open(&conn))
   {
   zend_throw_exception(cfmod_exception_db, "Unable to connect to database", 0 TSRMLS_CC);
   }

struct HubQuery *result = CFDB_QuerySoftware(&conn, hostkey,
   cfr_software, name, version, arch, true, context ,true);

if (!CFDB_Close(&conn))
   {
   zend_throw_exception(cfmod_exception_db, "Unable to close database", 0 TSRMLS_CC);
   }

JsonArray *software = NULL;
for (struct Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   struct HubSoftware *record = (struct HubSoftware *)rp->item;
   struct JsonObject *software_entry = NULL;

   JsonObjectAppendString(&software_entry, "hostkey", record->hh->keyhash);
   JsonObjectAppendString(&software_entry, "name", record->name);
   JsonObjectAppendString(&software_entry, "version", record->version);
   JsonObjectAppendString(&software_entry, "arch", Nova_LongArch(record->arch));
   JsonObjectAppendInteger(&software_entry, "timestamp", (int)record->t);

   JsonArrayAppendObject(&software, software_entry);
   }

DeleteHubQuery(result, DeleteHubSoftware);

RETURN_JSON(software);
}
