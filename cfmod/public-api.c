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
   JsonArrayAppendString(&reports, report->id);
   }

RETURN_JSON(reports);
}

