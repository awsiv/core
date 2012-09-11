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
HubQuery *CFDB_QueryValueReport(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                                HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryValueGraph(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                               char *classRegex);
HubQuery *CFDB_QueryPromiseLog(EnterpriseDB *conn, const char *keyHash, PromiseLogState state, const char *lhandle,
                               bool regex, const char *lcause, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter, int *total_results_out);
int CFDB_QueryPromiseLogFromOldColl(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list);
int CFDB_QueryPromiseLogFromMain(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list);
HubQuery *CFDB_QueryPromiseLogSummary(EnterpriseDB *conn, const char *hostkey, PromiseLogState state, const char *handle,
                                      bool regex, const char *cause, time_t from, time_t to, bool sort, HostClassFilter *host_class_filter);

HubQuery *CFDB_QueryColour(EnterpriseDB *conn, const HostRankMethod method, HostClassFilter *host_class_filter);
HubQuery *CFDB_QuerySoftware(EnterpriseDB *conn, char *keyHash, char *type, char *lname, char *lver, const char *larch,
                             bool regex, HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClasses(EnterpriseDB *conn, const char *keyHash, const char *lclass, bool regex, time_t from, time_t to,
                            HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClassSum(EnterpriseDB *conn, char **classes);
HubQuery *CFDB_QueryTotalCompliance(EnterpriseDB *conn, const char *keyHash, char *lversion, time_t from, time_t to, int lkept,
                                    int lnotkept, int lrepaired, int sort, HostClassFilter *hostClassFilter);
Sequence *CFDB_QueryHostComplianceShifts(EnterpriseDB *conn, HostClassFilter *host_class_filter);
HubQuery *CFDB_QueryVariables(EnterpriseDB *conn, char *keyHash, char *lscope, char *llval, char *lrval,
                              const char *ltype, bool regex, time_t from, time_t to, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryWeightedPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter);
HubQuery *CFDB_QueryLastSeen(EnterpriseDB *conn, char *keyHash, char *lhash, char *lhost, char *laddr, time_t lago,
                             bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryMeter(EnterpriseDB *conn, bson *query, char *db);
HubQuery *CFDB_QueryPerformance(EnterpriseDB *conn, char *keyHash, char *lname, bool regex, int sort,
                                HostClassFilter *hostClassFilter);
HubQuery *CFDB_QuerySetuid(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                           HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryBundleSeen(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryWeightedBundleSeen(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, int sort);
HubQuery *CFDB_QueryFileChanges(EnterpriseDB *conn, char *keyHash, char *lname, bool regex, time_t from, time_t to,
                                int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryFileDiff(EnterpriseDB *conn, char *keyHash, char *lname, char *ldiff, bool regex,
                             time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryCachedTotalCompliance(EnterpriseDB *conn, char *policy, time_t minGenTime);
HubQuery *CFDB_QueryClassesDistinctSorted(EnterpriseDB *conn, const char *class_rx, HostClassFilter *hostClassFilter, PageInfo *page);
HubQuery *CFDB_QueryPromiseBundles(EnterpriseDB *conn, PromiseFilter *filter);
HubQuery *CFDB_QueryPromisesUnexpanded(EnterpriseDB *conn, PromiseFilter *filter);
HubQuery *CFDB_QueryPromisesExpanded(EnterpriseDB *conn, PromiseFilter *filter);

Item *CFDB_QueryVitalIds(EnterpriseDB *conn, char *keyHash);
HubVital *CFDB_QueryVitalsMeta(EnterpriseDB *conn, const char *keyHash);
cfapi_errid CFDB_QueryVital(EnterpriseDB *conn, const char *hostkey, const char *vital_id, time_t last_update, time_t from, time_t to, HubVital **vital_out);
int CFDB_QueryMagView2(EnterpriseDB *conn, const char *keyhash, const char *monId, time_t start_time, double *qa, double *ea, double *da, double *ga);

int CFDB_QueryMonView(EnterpriseDB *conn, char *keyhash, char *monId, enum monitord_rep rep_type, double *qa, double *ea, double *da, double *ga);

bool CFDB_QueryHistogram(EnterpriseDB *conn, char *keyhash, char *monId, double *histo);
cfapi_errid CFDB_QueryLastHostUpdate(EnterpriseDB *conn, const char *hostkey, time_t *last_update);
bool CFDB_QueryLastUpdate(EnterpriseDB *conn, const char *db, const char *dbkey, const char *keyhash, time_t *date, int *size);

HubPromise *CFDB_QueryPromise(EnterpriseDB *conn, char *handle, char *file, int lineNo);
int CFDB_QueryPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey, char *attrVal, int attrValSz);
Item *CFDB_QueryExpandedPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey);
HubQuery *CFDB_QueryPromiseHandles(EnterpriseDB *conn, char *promiser, char *promiserType, char *bType, char *bName,
                                   bool regex, bool filter);
HubQuery *CFDB_QueryHandlesForBundlesWithComments(EnterpriseDB *conn, char *bType, char *bName);
HubQuery *CFDB_QueryPolicyFinderData(EnterpriseDB *conn, char *handle, char *promiser, char *bName, int escRegex);
Item *CFDB_QueryBundles(EnterpriseDB *conn, char *bTypeRegex, char *bNameRegex);
Rlist *CFDB_QueryBundleClasses(EnterpriseDB *conn, PromiseFilter *filter);
Item *CFDB_QueryBundlesUsing(EnterpriseDB *conn, PromiseFilter *promiseFilter, char *bNameReferenced);
HubBody *CFDB_QueryBody(EnterpriseDB *conn, char *type, char *name);
Item *CFDB_QueryAllBodies(EnterpriseDB *conn, char *bTypeRegex, char *bNameRegex);
int CFDB_QueryLastFileChange(EnterpriseDB *conn, char *keyHash, char *reportType, char *fileName, char *outBuf,
                             int outBufSz);
Rlist *CFDB_QueryHostKeys(EnterpriseDB *conn, const char *hostname, const char *ip, time_t from, time_t to, HostClassFilter *hostClassFilter);
Item *CFDB_GetDeletedHosts(void);
Item *CFDB_GetLastseenCache(void);
Rlist *CFDB_QueryNotes(EnterpriseDB *conn, char *keyhash, char *nid, Item *data);
Rlist *CFDB_QueryNoteId(EnterpriseDB *conn, bson *query);
Item *CFDB_QueryDistinctStr(EnterpriseDB *conn, char *database, char *collection, char *dKey, char *qKey,
                            char *qVal);
Item *CFDB_QueryDistinct(EnterpriseDB *conn, char *database, char *collection, char *dKey, bson *queryBson);
Item *CFDB_GetAllHostKeys(EnterpriseDB *conn);

HubQuery *CFDB_QueryScheduledReport( EnterpriseDB *conn, const char *user, const char *email,
                                  const char *scheduled_query_id, const char *scheduled_query, const char *schedule);

cfapi_errid CFDB_QuerySettings(EnterpriseDB *conn, HubSettings **settings_out);

//*****************************************************************************
// Singleton Queries
//*****************************************************************************
int CFDB_QueryBundleCount(EnterpriseDB *conn);
int CFDB_QueryPromiseCount(EnterpriseDB *conn);
int CFDB_CountHosts(EnterpriseDB *conn, HostClassFilter *host_class_filter, HostColourFilter *host_colour_filter);
bool CFDB_HasMatchingHost(EnterpriseDB *conn, const char *hostKey, const HostClassFilter *hostClassFilter);
int CFDB_CountHostsGeneric(EnterpriseDB *conn, bson *query);
int CFDB_QueryHostName(EnterpriseDB *conn, char *ipAddr, char *hostName, int hostNameSz);
HubHost *CFDB_GetHostByKey(EnterpriseDB *conn, const char *hostkey);
bool CFDB_GetHostColour(char *lkeyhash, const HostRankMethod method, HostColour *result);
int CFDB_CountSkippedOldAgents(EnterpriseDB *conn, char *keyhash, HostClassFilter *host_class_filter);
const char *CFDB_QueryVariableValueStr(EnterpriseDB *conn, char *keyHash,
                                       const char *ltype, char *lscope, char *lval);

//*****************************************************************************
// Utility Queries
//*****************************************************************************
long CFDB_GetLastAgentExecution(EnterpriseDB *conn, const char *hostkey);
long CFDB_GetDeltaAgentExecution(EnterpriseDB *conn, const char *hostkey);
bool CFDB_GetBlueHostThreshold(unsigned long *threshold);
bool CFDB_HandleGetValue(const char *lval, char *rval, int size, const char *default_rval, EnterpriseDB *conn, const char *db_name);
int CFDB_QueryIsMaster(void);
int CFDB_QueryMasterIP(char *buffer, int bufsize);
int CFDB_QueryReplStatus(EnterpriseDB *conn, char *buffer, int bufsize);
cfapi_errid CFDB_QueryLicense(EnterpriseDB *conn, JsonElement **license_out);

//*****************************************************************************
// General Utilities / Deprecation Candidates
//*****************************************************************************
void CFDB_ScanHubHost(bson_iterator *it, char *keyhash, char *ipaddr, char *hostnames);
Item *CFDB_GetHostByColour(EnterpriseDB *conn, HostClassFilter *host_class_filter, HostColourFilter *host_colour_filter);
bool CFDB_HostsWithClass(Rlist **return_list, char *class_name, char *return_format);

#endif
