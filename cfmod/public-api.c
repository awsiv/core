#include "public-api.h"

#include "db_query.h"

#define RETURN_JSON(json) \
   Writer *writer = StringWriter(); \
   JsonArrayPrint(writer, json, 0); \
   JsonArrayDelete(json);           \
   RETURN_STRING(StringWriterClose(writer), 1);

static const char *LABEL_ID = "id";
static const char *LABEL_CATEGORY = "category";
static const char *LABEL_DESCRIPTION = "description";
static const char *LABEL_HOSTKEY = "hostkey";
static const char *LABEL_NAME = "name";
static const char *LABEL_VALUE = "value";
static const char *LABEL_VERSION = "version";
static const char *LABEL_ARCH = "arch";
static const char *LABEL_TIMESTAMP = "timestamp";
static const char *LABEL_LASTSEEN = "last-seen";
static const char *LABEL_AVERAGE = "average";
static const char *LABEL_STDV = "stdv";
static const char *LABEL_CONTEXT = "context";

static void database_open(mongo_connection *connection)
{
if (!CFDB_Open(connection))
   {
   zend_throw_exception(cfmod_exception_db, "Unable to connect to database", 0 TSRMLS_CC);
   }
}

static void database_close(mongo_connection *connection)
{
if (!CFDB_Close(connection))
   {
   zend_throw_exception(cfmod_exception_db, "Unable to close database", 0 TSRMLS_CC);
   }
}


PHP_FUNCTION(cfmod_resource_report_list)
{
JsonArray *reports = NULL;

for (ReportInfo *report = BASIC_REPORTS; report->id != NULL; report++)
   {
   JsonObject *report_entry = NULL;
   JsonObjectAppendString(&report_entry, LABEL_ID, report->id);
   JsonObjectAppendString(&report_entry, LABEL_CATEGORY, report->category);
   JsonObjectAppendString(&report_entry, LABEL_DESCRIPTION, report->description);

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
PageInfo page = { 0 };

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
database_open(&conn);

HubQuery *result = CFDB_QuerySoftware(&conn, hostkey,
   cfr_software, name, version, arch, true, context ,true);

database_close(&conn);

JsonArray *software = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSoftware *record = (HubSoftware *)rp->item;
   JsonObject *software_entry = NULL;

   JsonObjectAppendString(&software_entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&software_entry, LABEL_NAME, record->name);
   JsonObjectAppendString(&software_entry, LABEL_VERSION, record->version);
   JsonObjectAppendString(&software_entry, LABEL_ARCH, Nova_LongArch(record->arch));
   //JsonObjectAppendInteger(&software_entry, LABEL_TIMESTAMP, (int)record->t);

   JsonArrayAppendObject(&software, software_entry);
   }

DeleteHubQuery(result, DeleteHubSoftware);

RETURN_JSON(software);
}


/************************************************************************************/


static const char *DataTypeToString(const char *datatype)
{
switch (*datatype)
   {
   case 's':
     return "string";
   case 'i':
     return "int";
   case 'r':
     return "real";
   case 'm':
     return "menu";

   default:
      if (strlen(datatype) == 2)
      {
      return "list";
      }
      return "unknown";
   }
}

PHP_FUNCTION(cfmod_resource_report_values)
{
char *hostkey = NULL,
     *scope = NULL,
     *name = NULL,
     *value = NULL,
     *type = NULL,
     *context = NULL;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssll",
      &hostkey, &len,
      &scope, &len,
      &name, &len,
      &value, &len,
      &type, &len,
      &context, &len,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, "Unable to parse arguments", 0 TSRMLS_CC);
   }

mongo_connection conn;
database_open(&conn);

HubQuery *result = CFDB_QueryVariables(&conn, hostkey,
   scope, name, value, type, true, context);

database_close(&conn);

JsonArray *values = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubVariable *record = (HubVariable *)rp->item;
   const char *type = DataTypeToString(record->dtype);

   JsonObject *value_entry = NULL;
   JsonObjectAppendString(&value_entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&value_entry, LABEL_NAME, type);

   if (strcmp(type, "list"))
      {
      JsonObjectAppendString(&value_entry, LABEL_VALUE, "not implemented");
      }
   else
      {
      JsonObjectAppendString(&value_entry, LABEL_VALUE, (const char *)record->rval);
      }

   JsonArrayAppendObject(&value_entry, value_entry);
   }

DeleteHubQuery(result, DeleteHubVariable);

RETURN_JSON(values);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_report_bundle_profile)
{
char *hostkey = NULL,
     *name = NULL,
     *context = NULL;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssll",
      &hostkey, &len,
      &name, &len,
      &context, &len,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, "Unable to parse arguments", 0 TSRMLS_CC);
   }

mongo_connection conn;
database_open(&conn);

HubQuery *result = CFDB_QueryBundleSeen(&conn, hostkey, name, true, context, true);

database_close(&conn);

JsonArray *bundles = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubBundleSeen *record = (HubBundleSeen *)rp->item;
   JsonObject *bundle_entry = NULL;

   JsonObjectAppendString(&bundle_entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&bundle_entry, LABEL_NAME, record->bundle);
   //JsonObjectAppendInteger(&bundle_entry, LABEL_LASTSEEN, SECONDS_PER_HOUR * record->hrsago);
   //JsonObjectAppendReal(&bundle_entry, LABEL_AVERAGE, SECONDS_PER_HOUR * record->hrsavg);
   //JsonObjectAppendReal(&bundle_entry, LABEL_STDV, SECONDS_PER_HOUR * record->hrsdev);

   JsonArrayAppendObject(&bundles, bundle_entry);
   }

DeleteHubQuery(result, DeleteHubBundleSeen);

RETURN_JSON(bundles);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_report_contexts)
{
char *hostkey = NULL,
     *context = NULL;
int len, from;
struct PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslll",
      &hostkey, &len,
      &context, &len,
      &from,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, "Unable to parse arguments", 0 TSRMLS_CC);
   }

mongo_connection conn;
database_open(&conn);

struct HubQuery *result = CFDB_QueryClasses(&conn, hostkey, NULL, true, (time_t)from, context, true);

database_close(&conn);

JsonArray *contexts = NULL;
for (struct Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   struct HubClass *record = (struct HubClass *)rp->item;
   JsonObject *entry = NULL;

   JsonObjectAppendString(&entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&entry, LABEL_CONTEXT, record->class);
   JsonObjectAppendReal(&entry, LABEL_AVERAGE, record->prob);
   JsonObjectAppendReal(&entry, LABEL_STDV, record->dev);
   JsonObjectAppendInteger(&entry, LABEL_LASTSEEN, record->t);

   JsonArrayAppendObject(&contexts, entry);
   }

DeleteHubQuery(result, DeleteHubClass);

RETURN_JSON(contexts);
}
