#ifndef CFENGINE_NOVA_DB_QUERY_H
#define CFENGINE_NOVA_DB_QUERY_H

#include "cf.nova.h"
#include "scorecards.h"
#include "db_common.h"
#include "sequence.h"

//*****************************************************************************
// Set Queries
//*****************************************************************************

HubQuery *CFDB_QueryHosts(mongo_connection *conn, char *db, bson *query);
HubQuery *CFDB_QueryHostsByAddress(mongo_connection *conn, char *hostNameRegex, char *ipRegex,
                                   HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryHostsByHostClassFilter(mongo_connection *conn, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryHostByHostKey(mongo_connection *conn, char *hostKey);
HubQuery *CFDB_QueryValueReport(mongo_connection *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                                HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryValueGraph(mongo_connection *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                               char *classRegex);
HubQuery *CFDB_QueryPromiseLog(mongo_connection *conn, const char *keyHash, PromiseLogState state, const char *lhandle,
                               bool regex, const char *lcause, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter, int *total_results_out);
int CFDB_QueryPromiseLogFromOldColl(mongo_connection *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list);
int CFDB_QueryPromiseLogFromMain(mongo_connection *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list);
HubQuery *CFDB_QueryPromiseLogSummary(mongo_connection *conn, const char *hostkey, PromiseLogState state, const char *handle,
                                      bool regex, const char *cause, time_t from, time_t to, bool sort, HostClassFilter *host_class_filter);

HubQuery *CFDB_QueryColour(mongo_connection *conn, const HostRankMethod method, HostClassFilter *host_class_filter);
HubQuery *CFDB_QuerySoftware(mongo_connection *conn, char *keyHash, char *type, char *lname, char *lver, char *larch,
                             bool regex, HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClasses(mongo_connection *conn, char *keyHash, char *lclass, bool regex, time_t from, time_t to,
                            HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClassSum(mongo_connection *conn, char **classes);
HubQuery *CFDB_QueryTotalCompliance(mongo_connection *conn, const char *keyHash, char *lversion, time_t from, time_t to, int lkept,
                                    int lnotkept, int lrepaired, int sort, HostClassFilter *hostClassFilter);
Sequence *CFDB_QueryHostComplianceShifts(mongo_connection *conn, HostClassFilter *host_class_filter);
HubQuery *CFDB_QueryVariables(mongo_connection *conn, char *keyHash, char *lscope, char *llval, char *lrval,
                              const char *ltype, bool regex, time_t from, time_t to, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryPromiseCompliance(mongo_connection *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryWeightedPromiseCompliance(mongo_connection *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter);
HubQuery *CFDB_QueryLastSeen(mongo_connection *conn, char *keyHash, char *lhash, char *lhost, char *laddr, time_t lago,
                             bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryMeter(mongo_connection *conn, bson *query, char *db);
HubQuery *CFDB_QueryPerformance(mongo_connection *conn, char *keyHash, char *lname, bool regex, int sort,
                                HostClassFilter *hostClassFilter);
HubQuery *CFDB_QuerySetuid(mongo_connection *conn, char *keyHash, char *lname, bool regex,
                           HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryBundleSeen(mongo_connection *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryWeightedBundleSeen(mongo_connection *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, int sort);
HubQuery *CFDB_QueryFileChanges(mongo_connection *conn, char *keyHash, char *lname, bool regex, time_t from, time_t to,
                                int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryFileDiff(mongo_connection *conn, char *keyHash, char *lname, char *ldiff, bool regex,
                             time_t from, time_t to, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryCachedTotalCompliance(mongo_connection *conn, char *policy, time_t minGenTime);
HubQuery *CFDB_QueryClassesDistinctSorted(mongo_connection *conn, const char *class_rx, HostClassFilter *hostClassFilter, PageInfo *page);
HubQuery *CFDB_QueryPromiseBundles(mongo_connection *conn, PromiseFilter *filter);
HubQuery *CFDB_QueryPromises(mongo_connection *conn, PromiseFilter *filter);

Item *CFDB_QueryVitalIds(mongo_connection *conn, char *keyHash);
HubVital *CFDB_QueryVitalsMeta(mongo_connection *conn, char *keyHash);
int CFDB_QueryMagView2(mongo_connection *conn, char *keyhash, char *monId, time_t start_time, double *qa, double *ea,
                       double *da);
int CFDB_QueryMonView(mongo_connection *conn, char *keyhash, char *monId, enum monitord_rep rep_type, double *qa,
                      double *ea, double *da);
int CFDB_QueryWeekView(mongo_connection *conn, char *keyhash, enum observables obs, double *qa, double *ea, double *da);
bool CFDB_QueryHistogram(mongo_connection *conn, char *keyhash, char *monId, double *histo);
bool CFDB_QueryLastUpdate(mongo_connection *conn, char *db, char *dbkey, char *keyhash, time_t *date, int *size);

HubPromise *CFDB_QueryPromise(mongo_connection *conn, char *handle, char *file, int lineNo);
int CFDB_QueryPromiseAttr(mongo_connection *conn, char *handle, char *attrKey, char *attrVal, int attrValSz);
Item *CFDB_QueryExpandedPromiseAttr(mongo_connection *conn, char *handle, char *attrKey);
HubQuery *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName,
                                   bool regex, bool filter);
HubQuery *CFDB_QueryHandlesForBundlesWithComments(mongo_connection *conn, char *bType, char *bName);
HubQuery *CFDB_QueryPolicyFinderData(mongo_connection *conn, char *handle, char *promiser, char *bName, int escRegex);
Item *CFDB_QueryBundles(mongo_connection *conn, char *bTypeRegex, char *bNameRegex);
Rlist *CFDB_QueryBundleClasses(mongo_connection *conn, PromiseFilter *filter);
Item *CFDB_QueryBundlesUsing(mongo_connection *conn, PromiseFilter *promiseFilter, char *bNameReferenced);
HubBody *CFDB_QueryBody(mongo_connection *conn, char *type, char *name);
Item *CFDB_QueryAllBodies(mongo_connection *conn, char *bTypeRegex, char *bNameRegex);
Item *CFDB_QueryCdpAcls(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpCommands(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpPromiser(mongo_connection *conn, char *sep, char *bundleName, char *promiseType);
int CFDB_QueryLastFileChange(mongo_connection *conn, char *keyHash, char *reportType, char *fileName, char *outBuf,
                             int outBufSz);
Item *CFDB_QueryCdpRegistry(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpServices(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpCompliance(mongo_connection *conn, char *handle);
Rlist *CFDB_QueryHostKeys(mongo_connection *conn, const char *hostname, const char *ip, time_t from, time_t to, HostClassFilter *hostClassFilter);
Item *CFDB_GetDeletedHosts(void);
Item *CFDB_GetLastseenCache(void);
Rlist *CFDB_QueryNotes(mongo_connection *conn, char *keyhash, char *nid, Item *data);
Rlist *CFDB_QueryNoteId(mongo_connection *conn, bson *query);
Item *CFDB_QueryDistinctStr(mongo_connection *conn, char *database, char *collection, char *dKey, char *qKey,
                            char *qVal);
Item *CFDB_QueryDistinct(mongo_connection *conn, char *database, char *collection, char *dKey, bson *queryBson);

//*****************************************************************************
// Singleton Queries
//*****************************************************************************
int CFDB_QueryBundleCount(mongo_connection *conn);
int CFDB_QueryPromiseCount(mongo_connection *conn);
int CFDB_CountHosts(mongo_connection *conn, HostClassFilter *host_class_filter, HostColourFilter *host_colour_filter);
bool CFDB_HasMatchingHost(mongo_connection *conn, char *hostKey, HostClassFilter *hostClassFilter);
int CFDB_CountHostsGeneric(mongo_connection *conn, bson *query);
int CFDB_QueryHostName(mongo_connection *conn, char *ipAddr, char *hostName, int hostNameSz);
HubHost *CFDB_GetHostByKey(mongo_connection *conn, const char *hostkey);
bool CFDB_GetHostColour(char *lkeyhash, const HostRankMethod method, HostColour *result);


//*****************************************************************************
// Utility Queries
//*****************************************************************************
long CFDB_GetLastAgentExecution(mongo_connection *conn, const char *hostkey);
long CFDB_GetDeltaAgentExecution(mongo_connection *conn, const char *hostkey);
int CFDB_GetBlueHostThreshold(unsigned long *threshold);
bool CFDB_HandleGetValue(char *lval, char *rval, int size, mongo_connection *conn, char *db_name);
int CFDB_QueryIsMaster(void);
int CFDB_QueryMasterIP(char *buffer, int bufsize);
int CFDB_QueryReplStatus(mongo_connection *conn, char *buffer, int bufsize);

//*****************************************************************************
// General Utilities / Deprecation Candidates
//*****************************************************************************
void CFDB_ScanHubHost(bson_iterator *it, char *keyhash, char *ipaddr, char *hostnames);
Item *CFDB_GetHostByColour(mongo_connection *conn, HostClassFilter *host_class_filter, HostColourFilter *host_colour_filter);

#endif
