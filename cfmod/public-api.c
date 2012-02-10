#include "public-api.h"

#include "db_query.h"
#include "common.h"

static const char *API_NAME = "CFEngine Nova";
static const char *API_VERSION = "v1";

static const char *LABEL_DESCRIPTION = "description";
static const char *LABEL_HOSTKEY = "hostkey";
static const char *LABEL_HOSTKEYS = "hostkeys";
static const char *LABEL_SCOPE = "scope";
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
static const char *LABEL_TYPE = "type";

static const char *LABEL_STATE = "state";
static const char *LABEL_STATE_REPAIRED = "repaired";
static const char *LABEL_STATE_NOTKEPT = "notkept";
static const char *LABEL_STATE_KEPT = "kept";
static const char *LABEL_STATE_ANY = "any";

static const char *LABEL_UNKNOWN = "unknown";


/************************************************************************************/


PHP_FUNCTION(cfmod_resource)
{
mongo_connection conn;
bool db_active = CFDB_Open(&conn);
if (db_active)
   {
   CFDB_Close(&conn);
   }

JsonElement *info = JsonObjectCreate(4);
JsonObjectAppendString(info, "api-name", API_NAME);
JsonObjectAppendString(info, "api-version", API_VERSION);
JsonObjectAppendString(info, "hub-version", NOVA_VERSION);
JsonObjectAppendString(info, "database", db_active ? "connected" : "disconnected");

RETURN_JSON(info);
}

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
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

Rlist *hostkeys = CFDB_QueryHostKeys(&conn, hostname, ip);

DATABASE_CLOSE(&conn);

JsonElement *output = JsonArrayCreate(1000);
for (Rlist *rp = hostkeys; rp != NULL; rp = rp->next)
   {
   JsonArrayAppendString(output, ScalarValue(rp));
   }

DeleteRlist(hostkeys);

RETURN_JSON(output);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_host_id)
{
char *hostkey;
int len;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostkey, &len) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HubHost *record = CFDB_GetHostByKey(&conn, hostkey);

DATABASE_CLOSE(&conn);

if (record != NULL)
   {
   JsonElement *entry = JsonObjectCreate(3);
   JsonObjectAppendString(entry, LABEL_HOSTKEY, record->keyhash);
   JsonObjectAppendString(entry, LABEL_NAME, record->hostname);
   JsonObjectAppendString(entry, LABEL_IP, record->ipaddr);

   RETURN_JSON(entry);
   }

RETURN_NULL();
}


/************************************************************************************/

static time_t Horizon(long from)
{
time_t now = time(NULL);
from = MIN(now, from);
return now - from;
}

static JsonElement *HostsLastSeen(Rlist *records, LastSeenDirection direction)
{
JsonElement *output = JsonArrayCreate(100);

for (Rlist *rp = records; rp != NULL; rp = rp->next)
   {
   HubLastSeen *record = (HubLastSeen *)rp->item;

   if (record->direction == direction)
      {
         JsonElement *entry = JsonObjectCreate(4);

         JsonObjectAppendString(entry, LABEL_HOSTKEY, record->rhost->keyhash);
         JsonObjectAppendInteger(entry, LABEL_LASTSEEN, SECONDS_PER_HOUR * record->hrsago);
         JsonObjectAppendInteger(entry, LABEL_AVERAGE, SECONDS_PER_HOUR * record->hrsavg);
         JsonObjectAppendInteger(entry, LABEL_STDV, SECONDS_PER_HOUR * record->hrsdev);

         JsonArrayAppendObject(output, entry);
      }
   }

return output;
}

PHP_FUNCTION(cfmod_resource_host_id_seen)
{
char *hostkey = NULL,
     *sortColumnName = NULL;
long from = 0;
int len = -1;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slsbll",
      &hostkey, &len,
      &from,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(NULL, NULL);
HubQuery *result = CFDB_QueryLastSeen(&conn, hostkey, NULL, NULL, NULL,
                                      from, false, false, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn);

JsonElement *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_OUTGOING);

DeleteHubQuery(result, DeleteHubLastSeen);

RETURN_JSON(output);
}


PHP_FUNCTION(cfmod_resource_host_id_seen_by)
{
char *hostkey = NULL,
     *sortColumnName = NULL;
long from = 0;
int len = -1;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slsbll",
      &hostkey, &len,
      &from,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(NULL, NULL);
HubQuery *result = CFDB_QueryLastSeen(&conn, hostkey, NULL, NULL, NULL,
                                      from, false, false, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn);

JsonElement *output = HostsLastSeen(result->records, LAST_SEEN_DIRECTION_INCOMING);

DeleteHubQuery(result, DeleteHubLastSeen);

RETURN_JSON(output);
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
     *state = NULL,
     *sortColumnName = NULL;
long from;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslsbll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &state, &len,
      &from,
      &sortColumnName, &len,
      &sortDescending,
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

JsonElement *output = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseCompliance *record = (HubPromiseCompliance *)rp->item;
   JsonElement *entry = JsonObjectCreate(6);

   JsonObjectAppendString(entry, LABEL_HANDLE, record->handle);
   JsonObjectAppendString(entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(entry, LABEL_STATE, PromiseStateToString(record->status));
   JsonObjectAppendReal(entry, LABEL_AVERAGE, record->e);
   JsonObjectAppendReal(entry, LABEL_STDV, record->d);
   JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, record->t);

   JsonArrayAppendObject(output, entry);
   }

DATABASE_CLOSE(&conn)

RETURN_JSON(output)
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

static JsonElement *PromiseLogAsJson(mongo_connection *conn, PromiseLogState state, const char *handle,
                                   const char *hostkey, const char *context, int from, int to, PageInfo page)
{

 HostClassFilter *filter = NewHostClassFilter(context, NULL);
 HubQuery *result = CFDB_QueryPromiseLog(conn, hostkey, state, handle, true, from, to, true, filter);
 DeleteHostClassFilter(filter);

JsonElement *output = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubPromiseLog *log_entry = (HubPromiseLog *)rp->item;
   JsonElement *entry = JsonObjectCreate(5);

   JsonObjectAppendString(entry, LABEL_HANDLE, log_entry->handle);
   JsonObjectAppendString(entry, LABEL_HOSTKEY, log_entry->hh->keyhash);
   JsonObjectAppendString(entry, LABEL_DESCRIPTION, log_entry->cause);
   JsonObjectAppendString(entry, LABEL_STATE, PromiseLogStateToString(state));
   JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, log_entry->t);

   JsonArrayAppendObject(output, entry);
   }

DeleteHubQuery(result, DeleteHubPromiseLog);

return output;
}

PHP_FUNCTION(cfmod_resource_promise_log_repaired)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL,
     *sortColumnName = NULL;
long from,
     to;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllsbll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

JsonElement *output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn)

RETURN_JSON(output);
}


/************************************************************************************/


static JsonElement *PromiseLogSummaryAsJson(mongo_connection *conn, PromiseLogState state, const char *handle,
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

JsonElement *output = JsonArrayCreate(100);
for (Item *ip = summary; ip != NULL; ip = ip->next)
   {
   JsonElement *entry = JsonObjectCreate(4);

   JsonObjectAppendString(entry, LABEL_HANDLE, ip->name);
   JsonObjectAppendString(entry, LABEL_DESCRIPTION, ip->classes);
   JsonObjectAppendInteger(entry, LABEL_COUNT, ip->counter);
   JsonObjectAppendString(entry, LABEL_STATE, PromiseLogStateToString(state));

   JsonArrayAppendObject(output, entry);
   }

DeleteItemList(summary);

return output;
}


PHP_FUNCTION(cfmod_resource_promise_log_repaired_summary)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL,
     *sortColumnName = NULL;
long from,
     to;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllsbll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

JsonElement *output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_REPAIRED, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn)

RETURN_JSON(output);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_promise_log_notkept)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL,
     *sortColumnName = NULL;
long from,
     to;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllsbll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn);

JsonElement *output = PromiseLogAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn);

RETURN_JSON(output);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_promise_log_notkept_summary)
{
char *handle = NULL,
     *hostkey = NULL,
     *context = NULL,
     *sortColumnName = NULL;
long from,
     to;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllsbll",
      &handle, &len,
      &hostkey, &len,
      &context, &len,
      &from,
      &to,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn);

JsonElement *output = PromiseLogSummaryAsJson(&conn, PROMISE_LOG_STATE_NOTKEPT, handle, hostkey, context, from, to, page);

DATABASE_CLOSE(&conn);

RETURN_JSON(output);
}


/************************************************************************************/


static const char *DataTypeToString(const char *datatype)
{
if (strlen(datatype) == 2)
   {
   return "list";
   }

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
     *context = NULL,
     *sortColumnName = NULL;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssssbll",
      &hostkey, &len,
      &scope, &len,
      &name, &len,
      &value, &len,
      &type, &len,
      &context, &len,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryVariables(&conn, hostkey,
   scope, name, value, type, true, filter);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn)

JsonElement *values = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubVariable *record = (HubVariable *)rp->item;
   const char *type = DataTypeToString(record->dtype);

   JsonElement *value_entry = JsonObjectCreate(3);
   JsonObjectAppendString(value_entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(value_entry, LABEL_SCOPE, record->scope);
   JsonObjectAppendString(value_entry, LABEL_NAME, record->lval);
   JsonObjectAppendString(value_entry, LABEL_TYPE, type);

   if (strcmp(type, "list") == 0 )
      {
      JsonObjectAppendArray(value_entry, LABEL_VALUE, RvalToJson(record->rval));
      }
   else // FIX: should condition on int/real/menu. possible?
      {
      JsonObjectAppendString(value_entry, LABEL_VALUE, (const char *)record->rval.item);
      }

   JsonArrayAppendObject(values, value_entry);
   }

DeleteHubQuery(result, DeleteHubVariable);

RETURN_JSON(values);
}


/************************************************************************************/


PHP_FUNCTION(cfmod_resource_context)
{
char *hostkey = NULL,
     *context = NULL,
     *sortColumnName = NULL;
int len;
long from = 0;
bool sortDescending = false;
PageInfo page = { 0 };


if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslsbll",
      &hostkey, &len,
      &context, &len,
      &from,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);
HubQuery *result = CFDB_QueryClasses(&conn, hostkey, NULL, false, Horizon(from), filter, false);
DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn)

JsonElement *contexts = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubClass *record = (HubClass *)rp->item;
   JsonElement *entry = JsonObjectCreate(5);

   JsonObjectAppendString(entry, LABEL_HOSTKEY, record->hh->keyhash);
   JsonObjectAppendString(entry, LABEL_CONTEXT, record->class);
   JsonObjectAppendReal(entry, LABEL_AVERAGE, record->prob);
   JsonObjectAppendReal(entry, LABEL_STDV, record->dev);
   JsonObjectAppendInteger(entry, LABEL_LASTSEEN, record->t);

   JsonArrayAppendObject(contexts, entry);
   }

DeleteHubQuery(result, DeleteHubClass);

RETURN_JSON(contexts);
}


/************************************************************************************/

static JsonElement *SoftwareHostsEntryCreate(HubSoftware *software)
{
JsonElement *entry = JsonObjectCreate(4);

JsonObjectAppendString(entry, LABEL_NAME, software->name);
JsonObjectAppendString(entry, LABEL_VERSION, software->version);
JsonObjectAppendString(entry, LABEL_ARCH, Nova_LongArch(software->arch));

JsonElement *hostkeys = JsonArrayCreate(100);
JsonArrayAppendString(hostkeys, software->hh->keyhash);
JsonObjectAppendArray(entry, LABEL_HOSTKEYS, hostkeys);

return entry;
}

static JsonElement *SoftwareHostsEntryFind(JsonElement *entries, HubSoftware *software)
{
for (size_t i = 0; i < JsonElementLength(entries); i++)
   {
   JsonElement *entry = JsonArrayGetAsObject(entries, i);
   if (strcmp(JsonObjectGetAsString(entry, LABEL_NAME), software->name) == 0 &&
       strcmp(JsonObjectGetAsString(entry, LABEL_VERSION), software->version) == 0 &&
       strcmp(JsonObjectGetAsString(entry, LABEL_ARCH), Nova_LongArch(software->arch)) == 0)
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
     *context = NULL,
     *sortColumnName = NULL;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssbll",
      &hostkey, &len,
      &name, &len,
      &version, &len,
      &arch, &len,
      &context, &len,
      &sortColumnName, &len,
      &sortDescending,
      &(page.resultsPerPage),
      &(page.pageNum)) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

mongo_connection conn;
DATABASE_OPEN(&conn)

HostClassFilter *filter = NewHostClassFilter(context, NULL);

HubQuery *result = CFDB_QuerySoftware(&conn, hostkey,
   cfr_software, name, version, Nova_ShortArch(arch), true, filter,true);

DeleteHostClassFilter(filter);

DATABASE_CLOSE(&conn)

JsonElement *output = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSoftware *record = (HubSoftware *)rp->item;
   JsonElement *entry = SoftwareHostsEntryFind(output, record);
   if (entry)
      {
      JsonElement *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);
      JsonArrayAppendString(hostkeys, record->hh->keyhash);
      }
   else
      {
      entry = SoftwareHostsEntryCreate(record);
      JsonArrayAppendObject(output, entry);
      }
   }

DeleteHubQuery(result, DeleteHubSoftware);

RETURN_JSON(output);
}


/************************************************************************************/


static JsonElement *SetUidHostsEntryCreate(HubSetUid *setuid)
{
JsonElement *entry = JsonObjectCreate(2);
JsonObjectAppendString(entry, LABEL_PATH, setuid->path);

JsonElement *hostkeys = JsonArrayCreate(100);
JsonArrayAppendString(hostkeys, setuid->hh->keyhash);
JsonObjectAppendArray(entry, LABEL_HOSTKEYS, hostkeys);

return entry;
}

static JsonElement *SetUidHostsEntryFind(JsonElement *entries, HubSetUid *setuid)
{
for (size_t i = 0; i < JsonElementLength(entries); i++)
   {
   JsonElement *entry = JsonArrayGetAsObject(entries, i);
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
     *context = NULL,
     *sortColumnName = NULL;
int len;
bool sortDescending = false;
PageInfo page = { 0 };

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbll",
      &hostkey, &len,
      &name, &len,
      &context, &len,
      &sortColumnName, &len,
      &sortDescending,
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

JsonElement *output = JsonArrayCreate(100);
for (Rlist *rp = result->records; rp != NULL; rp = rp->next)
   {
   HubSetUid *record = (HubSetUid *)rp->item;
   JsonElement *entry = SetUidHostsEntryFind(output, record);
   if (entry)
      {
      JsonElement *hostkeys = JsonObjectGetAsArray(entry, LABEL_HOSTKEYS);
      JsonArrayAppendString(hostkeys, record->hh->keyhash);
      }
   else
      {
      entry = SetUidHostsEntryCreate(record);
      JsonArrayAppendObject(output, entry);
      }
   }

DeleteHubQuery(result, DeleteHubSetUid);

RETURN_JSON(output);
}
