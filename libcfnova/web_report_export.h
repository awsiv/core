#include "conf.h"
#include "cf.nova.h"
#include "json.h"
#include "db_common.h"

#define DATABASE_OPEN_WR(connection) \
   if (!CFDB_Open(connection))\
   {\
    JsonElement *error = JsonObjectCreate(1); \
    JsonObjectAppendString(error, "error", "Database connection failure"); \
    return error;\
   }\

#define DATABASE_CLOSE_WR(connection) \
   if (!CFDB_Close(connection))\
   {\
    JsonElement *error = JsonObjectCreate(1); \
    JsonObjectAppendString(error, "error", "Database connection couldn't be closed successfully"); \
    return error;\
   }\

#define RETURN_WITH_ERROR_JSON_WR( is_true, error_str) \
   if( is_true ) \
   {\
    JsonElement *error = JsonObjectCreate(1); \
    JsonObjectAppendString(error, "error", error_str); \
    return error;\
   }\

#define RETURN_WITH_SUCCESS_JSON_WR( is_true, error_str) \
   if( is_true ) \
   {\
    JsonElement *error = JsonObjectCreate(1); \
    JsonObjectAppendString(error, "success", error_str); \
    return error;\
   }\

JsonElement *WebExportVariablesReport(const char *hostkey, const char *scope, const char *lval,
                                  const char *rval, const char *type, bool regex,
                                  const HostClassFilter *filter,
                                  PromiseContextMode promise_context, WebReportFileInfo *wr_info);

JsonElement *WebExportPromiseComplianceReport(char *hostkey, char *handle, char *status, bool regex,
                                          HostClassFilter *host_class_filter, HostColourFilter *host_color_filter,
                                          bool last_run_only, PromiseContextMode promise_context,
                                          WebReportFileInfo *wr_info);

JsonElement *WebExportSetuidReport(char *hostkey, char *file, bool regex,
                                   HostClassFilter *filter,
                                   PromiseContextMode promise_context,
                                   WebReportFileInfo *wr_info);

JsonElement *WebExportValueReport(char *hostkey, char *day, char *month, char *year,
                                  HostClassFilter *filter,
                                  PromiseContextMode promise_context,
                                  WebReportFileInfo *wr_info);

JsonElement *WebExportPromiseLogReport(char *hostkey, char *handle, char *causeRx,
                                       PromiseLogState state, time_t from, time_t to, HostClassFilter *filter,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info);

JsonElement *WebExportPromiseLogSummaryReport(char *hostkey, char *handle, char *causeRx, PromiseLogState state,
                                              time_t from, time_t to, HostClassFilter *filter,
                                              PromiseContextMode promise_context, WebReportFileInfo *wr_info);

JsonElement *WebExportHostOnlyReport( Rlist *records_p, WebReportFileInfo *wr_info );