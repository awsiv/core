#include "public-api.h"

#include "db_query.h"

#define DATABASE_OPEN(connection) \
   if (!CFDB_Open(connection))\
   {\
   zend_throw_exception(cfmod_exception_db, "Unable to connect to database", 0 TSRMLS_CC);\
   RETURN_NULL();\
   }\

#define DATABASE_CLOSE(connection) \
   if (!CFDB_Close(connection))\
   {\
   zend_throw_exception(cfmod_exception_db, "Unable to close to database", 0 TSRMLS_CC);\
   RETURN_NULL();\
   }\

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
static const char *LABEL_PATH = "path";
static const char *LABEL_HANDLE = "handle";

static const char *LABEL_STATE = "state";
static const char *LABEL_STATE_REPAIRED = "repaired";
static const char *LABEL_STATE_NOTKEPT = "notkept";

static const char *LABEL_UNKNOWN = "unknown";


static const char *PromiseLogStateToString(PromiseLogState state)
{
switch (state)
   {
   case CF_PROMISE_LOG_STATE_REPAIRED:
      return LABEL_STATE_REPAIRED;
   case CF_PROMISE_LOG_STATE_NOTKEPT:
      return LABEL_STATE_NOTKEPT;
   default:
      return LABEL_UNKNOWN;
   }
}

static JsonObject *PromiseLogEntryAsJson(HubPromiseLog *log_entry, PromiseLogState state)
{
JsonObject *entry = NULL;

JsonObjectAppendString(&entry, LABEL_HANDLE, log_entry->handle);
JsonObjectAppendString(&entry, LABEL_HOSTKEY, log_entry->hh->keyhash);
JsonObjectAppendString(&entry, LABEL_DESCRIPTION, log_entry->cause);
JsonObjectAppendString(&entry, LABEL_STATE, PromiseLogStateToString(state));
JsonObjectAppendInteger(&entry, LABEL_TIMESTAMP, log_entry->t);

return entry;
}

static JsonArray *PromiseLogAsJson(mongo_connection *conn, PromiseLogState state, const char *handle,
                                   const char *hostkey, const char *context, int from, int to, PageInfo page)
{
HubQuery *result = CFDB_QueryPromiseLog(conn, hostkey, state, handle, true, from, to, true, context);

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseLog *log_entry = (HubPromiseLog *)rp;
   JsonArrayAppendObject(&output, PromiseLogEntryAsJson(log_entry, state));
   }

DeleteHubQuery(result, DeleteHubPromiseLog);

return output;
}

PHP_FUNCTION(cfmod_resource_promise_log_repaired)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL;
long from,
     to;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, "Unable to parse arguments", 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

JsonArray *output = PromiseLogAsJson(&conn, CF_PROMISE_LOG_STATE_REPAIRED, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn)

RETURN_JSON(output);
}

PHP_FUNCTION(cfmod_resource_promise_log_notkept)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL;
long from,
     to;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, "Unable to parse arguments", 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn);

JsonArray *output = PromiseLogAsJson(&conn, CF_PROMISE_LOG_STATE_NOTKEPT, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn);

RETURN_JSON(output);
}


/************************************************************************************/


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
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubQuery *result = CFDB_QuerySoftware(&conn, hostkey,
   cfr_software, name, version, arch, true, context ,true);

DATABASE_CLOSE(&conn)

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
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubQuery *result = CFDB_QueryVariables(&conn, hostkey,
   scope, name, value, type, true, context);

DATABASE_CLOSE(&conn)

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

   JsonArrayAppendObject(&values, value_entry);
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
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubQuery *result = CFDB_QueryBundleSeen(&conn, hostkey, name, true, context, true);

DATABASE_CLOSE(&conn)

JsonArray *bundles = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubBundleSeen *record = (HubBundleSeen *)rp->item;
   JsonObject *bundle_entry = NULL;

   JsonObjectAppendString(&bundle_entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&bundle_entry, LABEL_NAME, record->bundle);
   JsonObjectAppendInteger(&bundle_entry, LABEL_LASTSEEN, SECONDS_PER_HOUR * record->hrsago);
   JsonObjectAppendReal(&bundle_entry, LABEL_AVERAGE, SECONDS_PER_HOUR * record->hrsavg);
   JsonObjectAppendReal(&bundle_entry, LABEL_STDV, SECONDS_PER_HOUR * record->hrsdev);

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
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslll",
      &hostkey, &len,
      &context, &len,
      &from,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, "Unable to parse arguments", 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubQuery *result = CFDB_QueryClasses(&conn, hostkey, NULL, true, (time_t)from, context, true);

DATABASE_CLOSE(&conn)

JsonArray *contexts = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubClass *record = (HubClass *)rp->item;
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


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_report_setuid_programs)
{
char *hostkey = NULL,
     *name = NULL,
     *context = NULL;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslll",
      &hostkey, &len,
      &name, &len,
      &context, &len,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, "Unable to parse arguments", 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubQuery *result = CFDB_QuerySetuid(&conn, hostkey, name, true, context);

DATABASE_CLOSE(&conn)

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSetUid *record = (HubSetUid *)rp->item;
   JsonObject *entry = NULL;

   JsonObjectAppendString(&entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&entry, LABEL_PATH, record->path);

   JsonArrayAppendObject(&output, entry);
   }

DeleteHubQuery(result, DeleteHubSetUid);

RETURN_JSON(output);
}
