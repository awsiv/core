#include "public-api.h"

#include "db_query.h"
#include "common.h"

static const char *LABEL_DESCRIPTION = "description";
static const char *LABEL_HOSTKEY = "hostkey";
static const char *LABEL_HOSTKEYS = "hostkeys";
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
static const char *LABEL_COUNT = "count";
static const char *LABEL_IP = "ip";

static const char *LABEL_STATE = "state";
static const char *LABEL_STATE_REPAIRED = "repaired";
static const char *LABEL_STATE_NOTKEPT = "notkept";
static const char *LABEL_STATE_KEPT = "kept";
static const char *LABEL_STATE_ANY = "any";

static const char *LABEL_UNKNOWN = "unknown";


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_host)
{
char *hostname = NULL,
     *ip = NULL;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",
      &hostname, &len,
      &ip, &len,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

Rlist *hostkeys = CFDB_QueryHostKeys(&conn, hostname, ip);

DATABASE_CLOSE(&conn);

JsonArray *output = NULL;
for (Rlist *rp = hostkeys; rp != NULL; rp = rp->next)
   {
   JsonArrayAppendString(&output, ScalarValue(rp));
   }

DeleteRlist(hostkeys);

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_host_id)
{
char *hostkey;
int len;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostkey, &len) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubHost *record = CFDB_GetHostByKey(&conn, hostkey);

DATABASE_CLOSE(&conn);

if (record != NULL)
   {
   JsonObject *entry = NULL;
   JsonObjectAppendString(&entry, LABEL_HOSTKEY, record->keyhash);
   JsonObjectAppendString(&entry, LABEL_NAME, record->hostname);
   JsonObjectAppendString(&entry, LABEL_IP, record->ipaddr);

   RETURN_JSON_OBJECT(entry);
   }

RETURN_NULL();
}


/************************************************************************************/


static JsonArray *HostsLastSeen(Rlist *records, LastSeenDirection direction)
{
JsonArray *output = NULL;

for (Rlist *rp = records; rp != NULL; rp = rp->next)
   {
   HubLastSeen *record = (HubLastSeen *)rp->item;

   if (record->direction == direction)
      {
         JsonObject *entry = NULL;

         JsonObjectAppendString(&entry, LABEL_HOSTKEY, record->rhost->keyhash);
         JsonObjectAppendInteger(&entry, LABEL_LASTSEEN, SECONDS_PER_HOUR * record->hrsago);
         JsonObjectAppendInteger(&entry, LABEL_AVERAGE, SECONDS_PER_HOUR * record->hrsavg);
         JsonObjectAppendInteger(&entry, LABEL_STDV, SECONDS_PER_HOUR * record->hrsdev);

         JsonArrayAppendObject(&output, entry);
      }
   }

return output;
}

PHP_FUNCTION(cfmod_resource_host_id_seen)
{
char *hostkey = NULL,
     *remote_hostname = NULL,
     *remote_ip = NULL,
     *context = NULL;
long from = 0;
int len = -1;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslll",
      &hostkey, &len,
      &remote_hostname, &len,
      &remote_ip, &len,
      &context, &len,
      &from,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryLastSeen(&conn, hostkey, NULL, remote_hostname, remote_ip,
                                      (time_t)from, true, false, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn);

JsonArray *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_OUTGOING);

DeleteHubQuery(result, DeleteHubLastSeen);

RETURN_JSON_ARRAY(output);
}


PHP_FUNCTION(cfmod_resource_host_id_seen_by)
{
char *hostkey = NULL,
     *remote_hostname = NULL,
     *remote_ip = NULL,
     *context = NULL;
long from = 0;
int len = -1;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslll",
      &hostkey, &len,
      &remote_hostname, &len,
      &remote_ip, &len,
      &context, &len,
      &from,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryLastSeen(&conn, hostkey, NULL, remote_hostname, remote_ip,
                                      from, true, false, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn);

JsonArray *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_INCOMING);

DeleteHubQuery(result, DeleteHubLastSeen);

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


static PromiseState PromiseStateFromString(const char *state)
{
if (state == NULL)
   {
   return PROMISE_STATE_ANY;
   }
else if (strcmp(LABEL_STATE_REPAIRED, state) == 0)
   {
   return PROMISE_STATE_REPAIRED;
   }
else if (strcmp(LABEL_STATE_NOTKEPT, state) == 0)
   {
   return PROMISE_STATE_NOTKEPT;
   }
else if (strcmp(LABEL_STATE_KEPT, state) == 0)
   {
   return PROMISE_STATE_KEPT;
   }
else
   {
   return PROMISE_STATE_ANY;
   }
}

static const char *PromiseStateToString(PromiseState state)
{
switch (state)
   {
   case PROMISE_STATE_REPAIRED:
      return LABEL_STATE_REPAIRED;
   case PROMISE_STATE_NOTKEPT:
      return LABEL_STATE_NOTKEPT;
   case PROMISE_STATE_KEPT:
      return LABEL_STATE_KEPT;
   default:
   case PROMISE_STATE_ANY:
      // FIX: probably should never happen, add warning?
      return LABEL_STATE_ANY;
   }
}

PHP_FUNCTION(cfmod_resource_promise_compliance)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL,
     *state = NULL;
long from;
int len;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &state, &len,
      &from,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryPromiseCompliance(&conn, hostkey, handle, PromiseStateFromString(state),
                                               true , 0, true, filter);
DeleteHostClassFilter(filter);

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseCompliance *record = (HubPromiseCompliance *)rp->item;
   JsonObject *entry = NULL;

   JsonObjectAppendString(&entry, LABEL_HANDLE, record->handle);
   JsonObjectAppendString(&entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(&entry, LABEL_STATE, PromiseStateToString(record->status));
   JsonObjectAppendReal(&entry, LABEL_AVERAGE, record->e);
   JsonObjectAppendReal(&entry, LABEL_STATE, record->d);
   JsonObjectAppendInteger(&entry, LABEL_TIMESTAMP, record->t);

   JsonArrayAppendObject(&output, entry);
   }

DATABASE_CLOSE(&conn)

RETURN_JSON_ARRAY(output)
}


/************************************************************************************/


static const char *PromiseLogStateToString(PromiseLogState state)
{
switch (state)
   {
   case PROMISE_LOG_STATE_REPAIRED:
      return LABEL_STATE_REPAIRED;
   case PROMISE_LOG_STATE_NOTKEPT:
      return LABEL_STATE_NOTKEPT;
   default:
      return LABEL_UNKNOWN;
   }
}

static JsonArray *PromiseLogAsJson(mongo_connection *conn, PromiseLogState state, const char *handle,
                                   const char *hostkey, const char *context, int from, int to, PageInfo page)
{

 HostClassFilter *filter = NewHostClassFilter(context, NULL);
 HubQuery *result = CFDB_QueryPromiseLog(conn, hostkey, state, handle, true, from, to, true, filter);
 DeleteHostClassFilter(filter);

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseLog *log_entry = (HubPromiseLog *)rp->item;
   JsonObject *entry = NULL;

   JsonObjectAppendString(&entry, LABEL_HANDLE, log_entry->handle);
   JsonObjectAppendString(&entry, LABEL_HOSTKEY, log_entry->hh->keyhash);
   JsonObjectAppendString(&entry, LABEL_DESCRIPTION, log_entry->cause);
   JsonObjectAppendString(&entry, LABEL_STATE, PromiseLogStateToString(state));
   JsonObjectAppendInteger(&entry, LABEL_TIMESTAMP, log_entry->t);
   JsonArrayAppendObject(&output, entry);
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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

JsonArray *output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn)

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


static JsonArray *PromiseLogSummaryAsJson(mongo_connection *conn, PromiseLogState state, const char *handle,
                                          const char *hostkey, const char *context, int from, int to, PageInfo page)
{
 HostClassFilter *filter = NewHostClassFilter(context, NULL);
 HubQuery *result = CFDB_QueryPromiseLog(conn, hostkey, state, handle, true, from, to, true, filter);
 DeleteHostClassFilter(filter);

// FIX: wrong on several levels
Item *summary = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseLog *log_entry = (HubPromiseLog *)rp->item;
   Item *ip = IdempPrependItem(&summary, log_entry->handle, log_entry->cause);
   ip->counter++;
   }

DeleteHubQuery(result, DeleteHubPromiseLog);

JsonArray *output = NULL;
for (Item *ip = summary; ip != NULL; ip = ip->next)
   {
   JsonObject *entry = NULL;

   JsonObjectAppendString(&entry, LABEL_HANDLE, ip->name);
   JsonObjectAppendString(&entry, LABEL_DESCRIPTION, ip->classes);
   JsonObjectAppendInteger(&entry, LABEL_COUNT, ip->counter);
   JsonObjectAppendString(&entry, LABEL_STATE, PromiseLogStateToString(state));

   JsonArrayAppendObject(&output, entry);
   }

DeleteItemList(summary);

return output;
}


PHP_FUNCTION(cfmod_resource_promise_log_repaired_summary)
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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

JsonArray *output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn)

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn);

JsonArray *output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn);

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_promise_log_notkept_summary)
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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn);

JsonArray *output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn);

RETURN_JSON_ARRAY(output);
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


PHP_FUNCTION(cfmod_resource_variable)
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
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryVariables(&conn, hostkey,
   scope, name, value, type, true, filter);
DeleteHostClassFilter(filter);

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
      JsonObjectAppendString(&value_entry, LABEL_VALUE, LABEL_ERROR_NOTIMPLEMENTED);
      }
   else
      {
      JsonObjectAppendString(&value_entry, LABEL_VALUE, (const char *)record->rval);
      }

   JsonArrayAppendObject(&values, value_entry);
   }

DeleteHubQuery(result, DeleteHubVariable);

RETURN_JSON_ARRAY(values);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_context)
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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryClasses(&conn, hostkey, NULL, true, (time_t)from, filter, true);
DeleteHostClassFilter(filter);

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

RETURN_JSON_ARRAY(contexts);
}


/************************************************************************************/

static JsonObject *SoftwareHostsEntryCreate(HubSoftware *software)
{
JsonObject *entry = NULL;

JsonObjectAppendString(&entry, LABEL_NAME, software->name);
JsonObjectAppendString(&entry, LABEL_VERSION, software->version);
JsonObjectAppendString(&entry, LABEL_ARCH, software->arch);

JsonArray *hostkeys = NULL;
JsonArrayAppendString(&hostkeys, software->hh->keyhash);
JsonObjectAppendArray(&entry, LABEL_HOSTKEYS, hostkeys);

return entry;
}

static JsonObject *SoftwareHostsEntryFind(JsonArray *entries, HubSoftware *software)
{
for (Rlist *rp = entries; rp != NULL; rp = rp->next)
   {
   JsonObject *entry = (JsonObject *)rp->item;
   if (strcmp(JsonObjectGetAsString(entry, LABEL_NAME), software->name) == 0 &&
       strcmp(JsonObjectGetAsString(entry, LABEL_VERSION), software->version) == 0 &&
       strcmp(JsonObjectGetAsString(entry, LABEL_ARCH), software->arch) == 0)
      {
      return entry;
      }
   }

return NULL;
}

PHP_FUNCTION(cfmod_resource_software)
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
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);

HubQuery *result = CFDB_QuerySoftware(&conn, hostkey,
   cfr_software, name, version, arch, true, filter,true);

DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn)

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSoftware *record = (HubSoftware *)rp->item;
   JsonObject *entry = SoftwareHostsEntryFind(output, record);
   if (entry)
      {
      JsonArray *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);
      JsonArrayAppendString(&hostkeys, record->hh->keyhash);
      }
   else
      {
      entry = SoftwareHostsEntryCreate(record);
      JsonArrayAppendObject(&output, entry);
      }
   }

DeleteHubQuery(result, DeleteHubSoftware);

RETURN_JSON_ARRAY(output);
}


/************************************************************************************/


static JsonObject *SetUidHostsEntryCreate(HubSetUid *setuid)
{
JsonObject *entry = NULL;
JsonObjectAppendString(&entry, LABEL_PATH, setuid->path);

JsonArray *hostkeys = NULL;
JsonArrayAppendString(&hostkeys, setuid->hh->keyhash);
JsonObjectAppendArray(&entry, LABEL_HOSTKEYS, hostkeys);

return entry;
}

static JsonObject *SetUidHostsEntryFind(JsonArray *entries, HubSetUid *setuid)
{
for (Rlist *rp = entries; rp != NULL; rp = rp->next)
   {
   JsonObject *entry = (JsonObject *)rp->item;
   if (strcmp(JsonObjectGetAsString(entry, LABEL_PATH), setuid->path) == 0)
      {
      return entry;
      }
   }

return NULL;
}


PHP_FUNCTION(cfmod_resource_setuid)
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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QuerySetuid(&conn, hostkey, name, true, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn)

JsonArray *output = NULL;
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSetUid *record = (HubSetUid *)rp->item;
   JsonObject *entry = SetUidHostsEntryFind(output, record);
   if (entry)
      {
      JsonArray *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);
      JsonArrayAppendString(&hostkeys, record->hh->keyhash);
      }
   else
      {
      entry = SetUidHostsEntryCreate(record);
      JsonArrayAppendObject(&output, entry);
      }
   }

DeleteHubQuery(result, DeleteHubSetUid);

RETURN_JSON_ARRAY(output);
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
   zend_throw_exception(cfmod_exception_db, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryBundleSeen(&conn, hostkey, name, true, filter, true);
DeleteHostClassFilter(filter);

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

RETURN_JSON_ARRAY(bundles);
}
