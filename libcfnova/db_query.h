#ifndef CFENGINE_NOVA_DB_QUERY_H
#define CFENGINE_NOVA_DB_QUERY_H

#include "cf.nova.h"
#include "scorecards.h"
#include "db_common.h"
#include "sequence.h"
#include "json.h"

//*****************************************************************************
// Set Queries
//*****************************************************************************

HubQuery *CFDB_QueryHosts(EnterpriseDB *conn, char *db, bson *query);
HubQuery *CFDB_QueryHostsByAddress(EnterpriseDB *conn, char *hostNameRegex, char *ipRegex,
                                   HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryHostsByHostClassFilter(EnterpriseDB *conn, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryHostByHostKey(EnterpriseDB *conn, const char *hostKey);
HubQuery *CFDB_QueryValueReport(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear,
                                HostClassFilter *hostClassFilter, PromiseContextMode promise_context,
                                WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryValueGraph(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                               char *classRegex);

HubQuery *CFDB_QueryPromiseLog(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause_rx, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, int *total_results_out,
                               PromiseContextMode promise_context, WebReportFileInfo *wr_info);
int CFDB_QueryPromiseLogFromOldColl(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                                    const char *lhandle, bool regex, const char *lcause_rx, time_t from, time_t to, int sort,
                                    HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list, WebReportFileInfo *wr_info);
int CFDB_QueryPromiseLogFromMain(EnterpriseDB *conn, const char *hostkey, PromiseLogState state,
                                 const char *lhandle, bool regex, const char *lcause_rx, time_t from,
                                 time_t to, int sort, HostClassFilter *host_class_filter, Rlist **host_list,
                                 Rlist **record_list, PromiseContextMode promise_context, WebReportFileInfo *wr_info);
HubQuery *CFDB_QueryPromiseLogSummary(EnterpriseDB *conn, const char *hostkey,
                                      PromiseLogState state, const char *handle,
                                      bool regex, const char *cause, time_t from,
                                      time_t to, bool sort, HostClassFilter *host_class_filter,
                                      PromiseContextMode promise_context, WebReportFileInfo *wr_info);
HubQuery *CFDB_QueryColour(EnterpriseDB *conn, const HostRankMethod method, HostClassFilter *host_class_filter, PromiseContextMode promise_context);
HubQuery *CFDB_QuerySoftware(EnterpriseDB *conn, char *keyHash, char *type, char *lname,
                             char *lver, const char *larch, HostClassFilter *hostClassFilter,
                             PromiseContextMode promise_context, WebReportFileInfo *wr_info,
                             int db_options);
HubQuery *CFDB_QueryClasses(EnterpriseDB *conn, const char *keyHash,
                            const char *lclass, time_t from, time_t to, HostClassFilter *hostClassFilter,
                            PromiseContextMode promise_context, WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryClassSum(EnterpriseDB *conn, char **classes);
HubQuery *CFDB_QueryTotalCompliance(EnterpriseDB *conn, const char *keyHash, char *lversion,
                                    time_t from, time_t to, int lkept, int lnotkept, int lrepaired,
                                    HostClassFilter *hostClassFilter, PromiseContextMode promise_context_mode,
                                    WebReportFileInfo *wr_info, int db_options);
Seq *CFDB_QueryHostComplianceShifts(EnterpriseDB *conn, HostClassFilter *host_class_filter,
                                         PromiseContextMode promise_context_mode);
HubQuery *CFDB_QueryVariables(EnterpriseDB *conn, const char *keyHash, const char *ns,
                              const char *bundle, const char *llval, const char *lrval,
                              const char *ltype, time_t from, time_t to, const HostClassFilter *hostClassFilter,
                              PromiseContextMode promise_context, WebReportFileInfo *wr_info,
                              int db_options);
HubQuery *CFDB_QueryPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle,
                                      PromiseState lstatus, time_t from, time_t to,
                                      HostClassFilter *hostClassFilter, PromiseContextMode promise_context,
                                      WebReportFileInfo *wr_info,
                                      int db_options);
HubQuery *CFDB_QueryWeightedPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle,
                                              PromiseState lstatus, time_t from, time_t to,
                                              HostClassFilter *hostClassFilter,
                                              HostColourFilter *hostColourFilter,
                                              PromiseContextMode promise_context,
                                              WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryLastSeen(EnterpriseDB *conn, char *keyHash, char *lhash, char *lhost,
                             char *laddr, double lago, time_t from, time_t to,
                             HostClassFilter *hostClassFilter, PromiseContextMode promise_context,
                             WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryMeter(EnterpriseDB *conn, bson *query, char *db);

HubQuery *CFDB_QueryPerformance(EnterpriseDB *conn, char *keyHash, char *lname,
                                HostClassFilter *hostClassFilter, PromiseContextMode promise_context,
                                WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QuerySetuid(EnterpriseDB *conn, char *keyHash, char *lname,
                           HostClassFilter *hostClassFilter,
                           PromiseContextMode promise_context,
                           WebReportFileInfo *wr_info,
                           int db_options);
HubQuery *CFDB_QueryBundleSeen(EnterpriseDB *conn, char *keyHash, char *ns,
                               char *lname, HostClassFilter *hostClassFilter,
                               PromiseContextMode promise_context, WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryWeightedBundleSeen(EnterpriseDB *conn, char *keyHash, char *ns,
                                       char *lname, HostClassFilter *hostClassFilter,
                                       HostColourFilter *hostColourFilter,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info, int db_options);
HubQuery *CFDB_QueryFileChanges(EnterpriseDB *conn, char *keyHash, char *lname,
                                time_t from, time_t to, HostClassFilter *hostClassFilter,
                                PromiseContextMode promise_context,
                                WebReportFileInfo *wr_info,
                                int db_options);
HubQuery *CFDB_QueryFileDiff(EnterpriseDB *conn, char *keyHash, char *lname,
                             char *ldiff, time_t from, time_t to, HostClassFilter *hostClassFilter,
                             PromiseContextMode promise_context, WebReportFileInfo *wr_info,
                             int db_options);
HubQuery *CFDB_QueryCachedTotalCompliance(EnterpriseDB *conn, char *policy, time_t minGenTime);
HubQuery *CFDB_QueryClassesDistinctSorted(EnterpriseDB *conn, const char *class_rx,
                                          HostClassFilter *hostClassFilter, PageInfo *page);
HubQuery *CFDB_QueryPromiseBundles(EnterpriseDB *conn, PromiseFilter *filter,
                                   PromiseContextMode promise_context);
HubQuery *CFDB_QueryPromisesUnexpanded(EnterpriseDB *conn, PromiseFilter *filter,
                                       PromiseContextMode promise_context);
HubQuery *CFDB_QueryPromisesExpanded(EnterpriseDB *conn, PromiseFilter *filter);

Item *CFDB_QueryVitalIds(EnterpriseDB *conn, char *keyHash);
HubQuery *CFDB_QueryVitalsMeta(EnterpriseDB *conn, const char *keyHash);
HubQuery *CFDB_QueryVital(EnterpriseDB *conn, const char *hostkey, const char *vital_id,
                            time_t from, time_t to);
int CFDB_QueryMagView2(EnterpriseDB *conn, const char *keyhash, const char *monId,
                       time_t start_time, double *qa, double *ea, double *da,
                       double *ga);

int CFDB_QueryMonView(EnterpriseDB *conn, char *keyhash, char *monId,
                      enum monitord_rep rep_type, double *qa, double *ea, double *da,
                      double *ga);

bool CFDB_QueryHistogram(EnterpriseDB *conn, char *keyhash, char *monId, double *histo);
cfapi_errid CFDB_QueryLastHostUpdate(EnterpriseDB *conn, const char *hostkey,
                                     time_t *last_update);
bool CFDB_QueryLastUpdate(EnterpriseDB *conn, const char *db, const char *dbkey,
                          const char *keyhash, time_t *date, int *size);

HubPromise *CFDB_QueryPromise(EnterpriseDB *conn, char *handle, char *file, int lineNo);
int CFDB_QueryPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey,
                          char *attrVal, int attrValSz);
Item *CFDB_QueryExpandedPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey);
HubQuery *CFDB_QueryPromiseHandles(EnterpriseDB *conn, char *promiser, char *promiserType,
                                   char *bType, char *bName, bool regex, bool filter);
HubQuery *CFDB_QueryHandlesForBundlesWithComments(EnterpriseDB *conn, char *bType,
                                                  char *bName);
HubQuery *CFDB_QueryPolicyFinderData(EnterpriseDB *conn, char *handle, char *promiser,
                                     char *bName, int escRegex);
Item *CFDB_QueryBundles(EnterpriseDB *conn, char *bTypeRegex, char *bNameRegex);
Rlist *CFDB_QueryBundleClasses(EnterpriseDB *conn, const PromiseFilter *filter);
Item *CFDB_QueryBundlesUsing(EnterpriseDB *conn, PromiseFilter *promiseFilter,
                             char *bNameReferenced);
HubBody *CFDB_QueryBody(EnterpriseDB *conn, char *type, char *name);
Item *CFDB_QueryAllBodies(EnterpriseDB *conn, char *bTypeRegex, char *bNameRegex);
int CFDB_QueryLastFileChange(EnterpriseDB *conn, char *keyHash, char *reportType,
                             char *fileName, char *outBuf, int outBufSz);
Rlist *CFDB_QueryHostKeys(EnterpriseDB *conn, const char *hostname, const char *ip,
                          time_t from, time_t to, HostClassFilter *hostClassFilter);
Item *CFDB_GetDeletedHosts(void);
bool CFDB_QueryDeleteHostPending(void);
Item *CFDB_GetLastseenCache(void);
Rlist *CFDB_QueryNotes(EnterpriseDB *conn, char *keyhash, char *nid, Item *data);
Rlist *CFDB_QueryNoteId(EnterpriseDB *conn, bson *query);
Item *CFDB_QueryDistinctStr(EnterpriseDB *conn, char *database, char *collection,
                            char *dKey, char *qKey, char *qVal);
Item *CFDB_QueryDistinct(EnterpriseDB *conn, char *database, char *collection,
                         char *dKey, bson *queryBson);
Item *CFDB_GetAllHostKeys(EnterpriseDB *conn);

HubQuery *CFDB_QueryScheduledReport(EnterpriseDB *conn, const char *user, const char *email,
                                    const char *scheduled_query_id, const char *scheduled_query,
                                    const char *schedule);

cfapi_errid CFDB_QuerySettings(EnterpriseDB *conn, HubSettings **settings_out);

//*****************************************************************************
// Singleton Queries
//*****************************************************************************
int CFDB_QueryBundleCount(EnterpriseDB *conn);
int CFDB_CountHosts(EnterpriseDB *conn, HostClassFilter *host_class_filter,
                    HostColourFilter *host_colour_filter, PromiseContextMode promise_context);
bool CFDB_HasMatchingHost(EnterpriseDB *conn, const char *hostKey,
                          const HostClassFilter *hostClassFilter);
int CFDB_CountHostsGeneric(EnterpriseDB *conn, bson *query);
int CFDB_QueryHostName(EnterpriseDB *conn, char *ipAddr, char *hostName, int hostNameSz);
HubHost *CFDB_GetHostByKey(EnterpriseDB *conn, const char *hostkey);
bool CFDB_GetHostColour(char *lkeyhash, const HostRankMethod method, HostColour *result,
                        PromiseContextMode promise_context);
const char *CFDB_QueryVariableValueStr(EnterpriseDB *conn, char *keyHash, const char *ltype,
                                       char *lscope, char *lval);
/**
 * CFDB_QueryValueFromHostKeyStr:
 * Read top level value for corresponding db key and hostkey
 * Returns false if key is not found
 */
bool CFDB_QueryValueFromHostKeyStr(EnterpriseDB *conn, const char *keyhash,
                                    const char *lval, char *buffer, int bufsize);

//*****************************************************************************
// Utility Queries
//*****************************************************************************
long CFDB_GetLastAgentExecution(EnterpriseDB *conn, const char *hostkey);
long CFDB_GetDeltaAgentExecution(EnterpriseDB *conn, const char *hostkey);
void CFDB_GetBlueHostThreshold(unsigned long *threshold);
bool CFDB_GetValue( EnterpriseDB *conn, char *lval, char *rval, int size, char *db_name );
bool CFDB_HandleGetValue(const char *lval, char *rval, int size, const char *default_rval,
                         EnterpriseDB *conn, const char *db_name);
bool CFDB_QueryIsMaster(void);
bool CFDB_QueryMasterIP(char *buffer, int bufsize);
int CFDB_QueryReplStatus(EnterpriseDB *conn, char *buffer, int bufsize);
cfapi_errid CFDB_QueryLicense(EnterpriseDB *conn, JsonElement **license_out);

//*****************************************************************************
// General Utilities / Deprecation Candidates
//*****************************************************************************
void CFDB_ScanHubHost(bson_iterator *it, char *keyhash, char *ipaddr, char *hostnames);
Item *CFDB_GetHostByColour(EnterpriseDB *conn, HostClassFilter *host_class_filter,
                           HostColourFilter *host_colour_filter);
bool CFDB_HostsWithClass(Rlist **return_list, char *class_name, char *return_format);

#endif