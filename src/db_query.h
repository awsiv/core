#ifndef CFENGINE_NOVA_DB_QUERY_H
#define CFENGINE_NOVA_DB_QUERY_H

#include "cf.nova.h"
#include "json.h"

#ifdef HAVE_LIBMONGOC

void CFDB_HandleGetValue(char *lval, char *rval, int size, mongo_connection *conn, char *db_name);
HubQuery *CFDB_QueryHostsInClassContext(mongo_connection *conn,char *expression,time_t horizon,int sort);
HubQuery *CFDB_QueryHosts(mongo_connection *conn, char *db, char *dbkey,bson *query);
HubQuery *CFDB_QueryHostsByAddress(mongo_connection *conn, char *hostNameRegex, char *ipRegex, char *classRegex);
HubQuery *CFDB_QueryValueReport(mongo_connection *conn,char *keyHash,char *lday,char *lmonth,char *lyear, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryValueGraph(mongo_connection *conn,char *keyHash,char *lday,char *lmonth,char *lyear, int sort, char *classRegex);
HubQuery *CFDB_QueryPromiseLog(mongo_connection *conn, const char *keyHash, PromiseLogState state,const char *lhandle, int regex, time_t from, time_t to, int sort,HostClassFilter *hostClassFilter);
HubQuery *CFDB_QuerySoftware(mongo_connection *conn,char *keyHash,char *type,char *lname,char *lver,char *larch,int regex, HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryClassSum(mongo_connection *conn, char **classes);
HubQuery *CFDB_QueryTotalCompliance(mongo_connection *conn,char *keyHash,char *lversion,time_t lt,int lkept,int lnotkept,int lrepaired,int cmp, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryVariables(mongo_connection *conn,char *keyHash,char *lscope,char *llval,char *lrval,char *ltype,int regex, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryPromiseCompliance(mongo_connection *conn, char *keyHash, char *lhandle, PromiseState lstatus, int regex, time_t minTime, int sort, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryLastSeen(mongo_connection *conn,char *keyHash,char *lhash,char *lhost,char *laddr,time_t lago,int regex,int sort,HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryMeter(mongo_connection *conn,bson *query,char *db);
HubQuery *CFDB_QueryPerformance(mongo_connection *conn,char *keyHash,char *lname,int regex,int sort,HostClassFilter *hostClassFilter);
HubQuery *CFDB_QuerySetuid(mongo_connection *conn,char *keyHash,char *lname,int regex, HostClassFilter *hostClassFilter);
HubQuery *CFDB_QueryBundleSeen(mongo_connection *conn, char *keyHash, char *lname,int regex, HostClassFilter *hostClassFilter, int sort);
HubQuery *CFDB_QueryFileChanges(mongo_connection *conn,char *keyHash,char *lname,int regex,time_t lt,int cmp, int sort, HostClassFilter *hostClassFilter, int lookInArchive);
HubQuery *CFDB_QueryFileDiff(mongo_connection *conn,char *keyHash,char *lname,char *ldiff,int regex,time_t lt,int cmp, int sort, HostClassFilter *hostClassFilter, int lookInArchive);

// class finder
Rlist *CFDB_QueryDateTimeClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
Rlist *CFDB_QuerySoftClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
Rlist *CFDB_QueryIpClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
Rlist *CFDB_QueryAllClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
Rlist *CFDB_QueryHostClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);

//int CFDB_QueryMagView(mongo_connection *conn,char *keyhash,enum observables obs,time_t start_time,double *qa,double *ea,double *da);
Item *CFDB_QueryVitalIds(mongo_connection *conn, char *keyHash);
HubVital *CFDB_QueryVitalsMeta(mongo_connection *conn, char *keyHash);
int CFDB_QueryMagView2(mongo_connection *conn,char *keyhash,char *monId,time_t start_time,double *qa,double *ea,double *da);
int CFDB_QueryMonView(mongo_connection *conn, char *keyhash,char *monId, enum monitord_rep rep_type,double *qa,double *ea,double *da);
int CFDB_QueryWeekView(mongo_connection *conn,char *keyhash,enum observables obs,double *qa,double *ea,double *da);
bool CFDB_QueryHistogram(mongo_connection *conn,char *keyhash,char *monId,double *histo);
int CFDB_QueryLastUpdate(mongo_connection *conn,char *db, char *dbkey,char *keyhash,time_t *date);

HubPromise *CFDB_QueryPromise(mongo_connection *conn, char *handle, char *file, int lineNo);
int CFDB_QueryPromiseAttr(mongo_connection *conn, char *handle, char *attrKey, char *attrVal, int attrValSz);
Item *CFDB_QueryExpandedPromiseAttr(mongo_connection *conn, char *handle, char *attrKey);
HubQuery *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName, int regex, bool filter);
HubQuery *CFDB_QueryHandlesForBundlesWithComments(mongo_connection *conn, char *bType, char *bName);
HubQuery *CFDB_QueryPolicyFinderData(mongo_connection *conn, char *handle,char *promiser,char *bName, int escRegex);
Item *CFDB_QueryBundles(mongo_connection *conn,char *bTypeRegex,char *bNameRegex);
Rlist *CFDB_QueryBundleClasses(mongo_connection *conn, char *bType, char *bName);
Item *CFDB_QueryBundlesUsing(mongo_connection *conn, char *bNameReferenced);
int CFDB_QueryBundleCount(mongo_connection *conn);
int CFDB_QueryBundleType(mongo_connection *conn,char *bName,char *buffer,int bufsize);
int CFDB_QueryPromiseCount(mongo_connection *conn);
HubBody *CFDB_QueryBody(mongo_connection *conn, char *type, char *name);
Item *CFDB_QueryAllBodies(mongo_connection *conn,char *bTypeRegex,char *bNameRegex);
Item *CFDB_QueryCdpAcls(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpCommands(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpPromiser(mongo_connection *conn, char *sep, char *bundleName, char *promiseType);
int CFDB_QueryLastFileChange(mongo_connection *conn, char *keyHash, char *reportType, char *fileName, char *outBuf, int outBufSz);
Item *CFDB_QueryCdpRegistry(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpServices(mongo_connection *conn, char *sep);
Item *CFDB_QueryCdpCompliance(mongo_connection *conn, char *handle);

void CFDB_ListEverything(mongo_connection *conn);
void CFDB_ScanHubHost(bson_iterator *it,char *keyhash,char *ipaddr,char *hostnames);
int QueryInsertHostInfo(mongo_connection *conn,Rlist *host_list);
void PrintCFDBKey(bson_iterator *it, int depth);
int CFDB_IteratorNext(bson_iterator *it, bson_type valType);
int Nova_MagViewOffset(int start_slot,int dbslot,int wrap);
int CFDB_CountHosts(mongo_connection *conn, HostClassFilter *hostClassFilter);
int CFDB_CountHostsGeneric(mongo_connection *conn, bson *query);
int CFDB_QueryHostName(mongo_connection *conn, char *ipAddr, char *hostName, int hostNameSz);
bool MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra, bool *checkUpdate);

//replica set
Item * CFDB_GetLastseenCache(void);
int CFDB_QueryIsMaster(void);
int CFDB_QueryMasterIP(char *buffer,int bufsize);
int CFDB_QueryReplStatus(mongo_connection *conn, char *buffer,int bufsize);
Item *CFDB_GetDeletedHosts(void);

// host
Rlist *CFDB_QueryHostKeys(mongo_connection *conn, const char *hostname, const char *ip);
HubHost *CFDB_GetHostByKey(mongo_connection *conn, const char *hostkey);

#endif

#endif
