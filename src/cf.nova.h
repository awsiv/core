/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.h                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef CFENGINE_NOVA_H
#define CFENGINE_NOVA_H

#ifndef NOVA
#define NOVA 1

#ifdef HAVE_CONFIG_H
#  include "conf.h"
#endif

# ifdef HAVE_LIBLDAP
#  include <ldap.h>
# endif

#ifdef HAVE_ACL_H
# include <acl.h>
#endif

#ifdef HAVE_SYS_ACL_H
# include <sys/acl.h>
#endif

#ifdef HAVE_ACL_LIBACL_H
# include <acl/libacl.h>
#endif


#ifdef HAVE_LIBVIRT
# include <libvirt/libvirt.h>
# include <libvirt/virterror.h>
#endif

#ifdef HAVE_LIBMONGOC
// WARNING: If changing collection names: grep through source
# define MONGO_HAVE_STDINT
# define MONGO_BASE "cfreport"
# define MONGO_KMBASE "cfknow"
# define MONGO_DATABASE MONGO_BASE ".hosts"
# define MONGO_DATABASE_MON MONGO_BASE ".monitoring"
# define MONGO_PROMISES_EXP MONGO_BASE ".promises_exp"
# define MONGO_PROMISES_UNEXP_COLLECTION "promises_unexp"
# define MONGO_PROMISES_UNEXP MONGO_BASE "." MONGO_PROMISES_UNEXP_COLLECTION
# define MONGO_KM_TOPICS MONGO_KMBASE ".""KT"
# define MONGO_KM_OCCURRENCES MONGO_KMBASE ".""KO"
# define MONGO_KM_INFERENCES MONGO_KMBASE ".""KI"
# define MONGO_BODIES   MONGO_BASE ".bodies"
# define MONGO_SCRATCH MONGO_BASE ".scratch"
# define MONGO_CACHE MONGO_BASE ".cache"
# define MONGO_LOGS_REPAIRED MONGO_BASE ".logs_rep"
# define MONGO_LOGS_NOTKEPT MONGO_BASE ".logs_nk"
# define MONGO_NOTEBOOK MONGO_BASE ".notebook"
# include <mongo.h>
#else
# define mongo_connection char
#endif


#undef PACKAGE

#define CF_SHADES 25
#define CF_TIMESERIESDATA 168 /* (24*7) */
#define CF_MAGDATA 48         /* (4*12) */
#define CF_MAX_SLOTS 2016     /* (24*7*12) */
#define CF_LHISTORYDATA 1464
#define CF_YEAR_SLOTS 3*52
#define CF_MAGMARGIN 0
#define CF_LHISTORYMARGIN 0
#define CF_TRIBE_SIZE 25
#define CF_MAX_ERROR_LIMIT 50
#define CF_MAX_LIMIT 999999
#define CF_N_CODE '\07'
#define CF_THIS_HH (void *)-1
#define CF_KMAP "knowledge.php"
#define CFDB_PORT 27017
#define MONITOR_CLASS_PREFIX "mXC_"
#define CF_CHANGE_HORIZON 10
#define NOVA_EXPORT_HEADER "NOVA_EXPORT"
#define CF_CODEBOOK_SIZE 24
#define NOVA_MAXDIFFSIZE (80 * 1024 * 1024)

/*****************************************************************************/

enum cf_rank_method
   {
   cfrank_compliance,
   cfrank_anomaly,
   cfrank_performance,
   cfrank_lastseen,
   cfrank_default
   };

/*****************************************************************************/

struct CfDataView
   {
   double max;
   double min;
   int over;
   int under;
   int over_dev1;
   int over_dev2;
   int under_dev1;
   int under_dev2;
   double data_E[CF_LHISTORYDATA];
   double data_q[CF_LHISTORYDATA];
   double bars[CF_LHISTORYDATA];
   double error_scale;
   double range;
   int margins;
   char *title;
   char *docroot;
   };

struct CfGraphNode
   {
   int real_id;
   int tribe_id;
   char *shortname;
   char *fullname;
   int distance_from_centre;
   };

#define CF_MIN_RADIUS    15.0  // Reduce this at your peril, could hang gd
#define CF_RADIUS_SCALE  10.0

/*****************************************************************************/

struct CfFileLine
   {
   char  *text;
   int    counter;
   struct CfFileLine *next;
   struct CfFileLine *prev;
   };

/*****************************************************************************/

struct PageInfo
{
  int resultsPerPage;
  int pageNum;
  int totalResultCount;
};

/*****************************************************************************/

#ifdef MINGW

#define EVENTSOURCE_NAME "Cfengine Nova"  // appears in e.g. Event Viewer
#define EVENTSOURCE_HKLM_REGPATH "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\System\\" EVENTSOURCE_NAME
#define EVENT_COUNT 9  // mapped to "TypesSupported" registry log entry
#include "cf.events.h"  // defines events for logging on windows

#define WINSERVICE_NAME "CfengineNovaExec"

#endif

/*****************************************************************************/

/* Valid ACL syntax values, from documentation */

// Valid generic permissions
#define CF_VALID_GPERMS "rwx"

// Valid native permission characters
#define CF_VALID_NPERMS_NTFS "drtxTwabBpcoD"
#define CF_VALID_NPERMS_POSIX "rwx"

// Valid operations (first char of mode)
#define CF_VALID_OPS_METHOD_OVERWRITE "=+-"
#define CF_VALID_OPS_METHOD_APPEND "=+-"

// Native perms separators in mode
#define CF_NATIVE_PERMS_SEP_START '('
#define CF_NATIVE_PERMS_SEP_END ')'

#ifdef MINGW
#define CF_GENERIC_READ_NT (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL)
#define CF_GENERIC_WRITE_NT (FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
#define CF_GENERIC_EXECUTE_NT (FILE_EXECUTE)
#define CF_MINIMUM_PERMS_NT (SYNCHRONIZE)  // access is always denied if synchronize is not set
#endif /* MINGW */

/************************************************************************************/
/* Prototypes                                                                       */
/************************************************************************************/

#ifndef HAVE_SERV_H
#include "../../core/src/cf3.server.h"
#endif
   
/* acl.c */

void Nova_VerifyACL(char *file,struct Attributes a, struct Promise *pp);
void  Nova_SetACLDefaults(char *path, struct CfACL *acl);
int Nova_CheckACLSyntax(char *file,struct CfACL acl,struct Promise *pp);
int Nova_CheckACESyntax(char *ace, char *valid_nperms, char *valid_ops, int deny_support, int mask_support,struct Promise *pp);
int Nova_CheckModeSyntax(char **mode_p, char *valid_nperms, char *valid_ops,struct Promise *pp);
int Nova_CheckPermTypeSyntax(char *permt, int deny_support,struct Promise *pp);
int Nova_CheckDirectoryInherit(char *path, struct CfACL *acl, struct Promise *pp);

/* acl_linux.c */

int Nova_CheckPosixLinuxACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp);
#ifdef HAVE_LIBACL
int Nova_CheckPosixLinuxAccessACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckPosixLinuxInheritACEs(struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckPosixLinuxACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path, acl_type_t acl_type, struct Attributes a, struct Promise *pp);
int Nova_CheckDefaultEqualsAccessACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckDefaultClearACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_ParseEntityPosixLinux(char **str, acl_entry_t ace, int *is_mask);
int Nova_ParseModePosixLinux(char *mode, acl_permset_t old_perms);
acl_entry_t Nova_FindACE(acl_t acl, acl_entry_t ace_find);
int Nova_ACLEquals(acl_t first, acl_t second);
int Nova_ACECount(acl_t acl);
int Nova_PermsetEquals(acl_permset_t first, acl_permset_t second);
#endif

/* acl_nt.c */

int Nova_CheckNtACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp);
#ifdef MINGW
int Nova_CheckNtACEs(char *file_path, struct Rlist *aces, inherit_t inherit, enum cf_acl_method method, struct Attributes a, struct Promise *pp);
int Nova_CheckNtInheritACEs(char *file_path, struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, struct Attributes a, struct Promise *pp);
int Nova_CheckNtDefaultClearACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckNtDefaultEqualsAccessACL(char *file_path, struct Attributes a, struct Promise *pp);
void Nova_RemoveEasByInheritance(EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
void Nova_RemoveEmptyEas(EXPLICIT_ACCESS *eas, int *eaCount);
int Nova_ACLEquals(int *aclsEqual, EXPLICIT_ACCESS *firstEas, int eaCount, ACL *acl);
int Nova_AclToExplicitAccess(EXPLICIT_ACCESS *eas, int eaCount, ACL *acl);
int Nova_ParseAcl(char *file_path, struct Rlist *aces, EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
int Nova_SetEas(char *file_path, EXPLICIT_ACCESS *eas, int eaCount);
EXPLICIT_ACCESS *Nova_FindAceNt(EXPLICIT_ACCESS *eas, EXPLICIT_ACCESS *endEa, SID *findSid, ACCESS_MODE findPermt, inherit_t findInherit);
void Nova_FreeSids(EXPLICIT_ACCESS *eas, int num);
int Nova_EntityToSid(char **acePtr, SID *sid, DWORD sidSz);
int Nova_ParseModeNt(char **modePtr, ACCESS_MASK *perms);
ACCESS_MODE Nova_ParsePermTypeNt(char *ace);
#endif  /* MINGW */

/* cfnova.c */

void Nova_Initialize(void);
void Nova_CheckInstalledLibraries(void);
void Nova_SaveDocumentRoot(void);

/* client_code.c */

int Nova_QueryForKnowledgeMap(struct cfagent_connection *conn,char *menu,time_t since);
int Nova_StoreIncomingReports(char *reply,struct Item **reports,int current_report);
void NewReportBook(struct Item **reports);
void DeleteReportBook(struct Item **reports);
void UnpackReportBook(char *id,char *ipaddr,struct Item **reports);
void Nova_RecordNetwork(time_t now, double datarate,struct cfagent_connection *conn);

/* copernicus.c */

void Nova_PrimeGraph(struct Rlist **semantic);
void Nova_DrawTribe_PNG(char *filename,int *tribe_id,struct CfGraphNode *tribe_node, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size, double *tribe_evc,int topic,char *buffer,int bufsize);
void Nova_DrawTribe(int *tribe_id,struct CfGraphNode *tribe_node, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size, double *tribe_evc,int topic,char *buffer,int bufsize);
int Nova_GetMaxEvcNode(double *evc,int tribe_size);
int Nova_GetAdjacent(int i,double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int size, struct CfGraphNode *tribe, struct CfGraphNode *neighbours);
int Nova_SplayAdjacent(int i,double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size,struct CfGraphNode *tribe,int *trail,struct CfGraphNode *neighbours);
void Nova_CopyNeighbours2(struct CfGraphNode *from,int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1);
void Nova_CopyNeighbours3(struct CfGraphNode *from,int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1,int index2);
int Nova_GetEvcTops(double **adj,int size, double *evc, int *tops);
void Nova_CentreScale(struct CfDataView *cfv,double min_x,double max_x,double min_y,double max_y);
int Nova_X(struct CfDataView cfv,double x);
int Nova_Y(struct CfDataView cfv,double y);
int Nova_TribeUnion(int *array1,struct CfGraphNode *array2, int size1, int size2);
void Nova_ClearTrail(int *array);
void Nova_AnchorTrail(int *array,int node);
int Nova_InTrail(int *trail,int node);
void Nova_Line(struct CfDataView cfv,double x1,double y1,double x2,double y2,int colour);
void Nova_Disc(struct CfDataView cfv,double x1,double y1,double radius,int colour);
void Nova_Print(struct CfDataView cfv,double x,double y,char *s,int colour);
void Nova_BigPrint(struct CfDataView cfv,double x,double y,char *s,int colour);
double Nova_SignPerturbation(int i);
void Nova_ColdBall(struct CfDataView cfv,double x,double y,double radius,int *shade);
void Nova_HotBall(struct CfDataView cfv,double x,double y,double radius,int *shade);
void Nova_AlignmentCorrection(double *x,double *y,double cx,double cy);
void Nova_MapHorizon(struct CfDataView cfv,double x,double y,double *min_x,double *min_y,double *max_x,double *max_y);
double Nova_Orbit(struct CfDataView cfv,double radius,double min_x,double max_x,double min_y,double max_y);
void Nova_MapBall(struct CfDataView cfv,struct CfGraphNode n,char *buffer,int bufsize);
void Nova_Annealing(struct CfGraphNode neighbours1[CF_TRIBE_SIZE],struct CfGraphNode neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE],struct CfGraphNode neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int size1,int size2[CF_TRIBE_SIZE],int size3[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size);
int Overlap(double x1,double y1,double x2,double y2);
int Nova_InRange(struct CfDataView cfv,int x,int y);
void Nova_BoundaryCheck(struct CfDataView *cfv,int *x1,int *y1,int *x2, int *y2);

/* database.c */

int Nova_CheckDatabaseSanity(struct Attributes a, struct Promise *pp);

/* db_query.c */

#ifdef HAVE_LIBMONGOC

int CFDB_GetValue(char *lval,char *rval,int size);
void CFDB_HandleGetValue(char *lval, char *rval, int size, mongo_connection *conn);
int Nova2PHP_countclasses(char *hostkey,char *name,int regex,char *returnval,int bufsize);
struct HubQuery *CFDB_QueryHostsInClassContext(mongo_connection *conn,char *expression,time_t horizon,int sort);
struct HubQuery *CFDB_QueryHosts(mongo_connection *conn,bson *query);
struct HubQuery *CFDB_QueryHostsByAddress(mongo_connection *conn, char *hostNameRegex, char *ipRegex, char *classRegex);
struct HubQuery *CFDB_QueryValueReport(mongo_connection *conn,char *keyHash,char *lday,char *lmonth,char *lyear, int sort, char *classRegex);
struct HubQuery *CFDB_QueryValueGraph(mongo_connection *conn,char *keyHash,char *lday,char *lmonth,char *lyear, int sort, char *classRegex);
struct HubQuery *CFDB_QueryPromiseLog(mongo_connection *conn,char *keyHash,enum promiselog_rep type,char *lhandle, int regex, time_t from, time_t to,int sort,char *classRegex);
struct HubQuery *CFDB_QuerySoftware(mongo_connection *conn,char *keyHash,char *type,char *lname,char *lver,char *larch,int regex, char *classRegex, int sort);
struct HubQuery *CFDB_QueryClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
struct HubQuery *CFDB_QueryClassSum(mongo_connection *conn, char **classes);
struct HubQuery *CFDB_QueryTotalCompliance(mongo_connection *conn,char *keyHash,char *lversion,time_t lt,int lkept,int lnotkept,int lrepaired,int cmp, int sort, char *classRegex);
struct HubQuery *CFDB_QueryVariables(mongo_connection *conn,char *keyHash,char *lscope,char *llval,char *lrval,char *ltype,int reg, char *classRegex);
struct HubQuery *CFDB_QueryPromiseCompliance(mongo_connection *conn,char *keyHash,char *lhandle,char lstatus,int regex, int sort, char *classRegex);
struct HubQuery *CFDB_QueryLastSeen(mongo_connection *conn,char *keyHash,char *lhash,char *lhost,char *laddr,time_t lago,int regex,int sort,char *classRegex);
struct HubQuery *CFDB_QueryMeter(mongo_connection *conn,char *hostkey);
struct HubQuery *CFDB_QueryPerformance(mongo_connection *conn,char *keyHash,char *lname,int regex,int sort,char *classRegex);
struct HubQuery *CFDB_QuerySetuid(mongo_connection *conn,char *keyHash,char *lname,int regex, char *classRegex);
struct HubQuery *CFDB_QueryBundleSeen(mongo_connection *conn, char *keyHash, char *lname,int regex, char *classRegex, int sort);
struct HubQuery *CFDB_QueryFileChanges(mongo_connection *conn,char *keyHash,char *lname,int regex,time_t lt,int cmp, int sort, char *classRegex);
struct HubQuery *CFDB_QueryFileDiff(mongo_connection *conn,char *keyHash,char *lname,char *ldiff,int regex,time_t lt,int cmp, int sort, char *classRegex);

// class finder
struct Rlist *CFDB_QueryDateTimeClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
struct Rlist *CFDB_QuerySoftClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
struct Rlist *CFDB_QueryIpClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
struct Rlist *CFDB_QueryAllClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);
struct Rlist *CFDB_QueryHostClasses(mongo_connection *conn,char *keyHash,char *lclass,int regex,time_t horizon, char *classRegex, int sort);

int CFDB_QueryMagView(mongo_connection *conn,char *keyhash,enum observables obs,time_t start_time,double *qa,double *ea,double *da);
int CFDB_QueryWeekView(mongo_connection *conn,char *keyhash,enum observables obs,double *qa,double *ea,double *da);
int CFDB_QueryYearView(mongo_connection *conn,char *keyhash,enum observables obs,double *qa,double *ea,double *da);
int CFDB_QueryHistogram(mongo_connection *conn,char *keyhash,enum observables obs,double *histo);
int CFDB_QueryLastUpdate(mongo_connection *conn,char *keyhash,time_t *date);

struct HubPromise *CFDB_QueryPromise(mongo_connection *conn, char *handle, char *file, int lineNo);
int CFDB_QueryPromiseAttr(mongo_connection *conn, char *handle, char *attrKey, char *attrVal, int attrValSz);
struct Item *CFDB_QueryExpandedPromiseAttr(mongo_connection *conn, char *handle, char *attrKey);
struct HubQuery *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName, int regex, bool filter);
struct HubQuery *CFDB_QueryPolicyFinderData(mongo_connection *conn, char *handle,char *promiser,char *bName, int escRegex);
struct Item *CFDB_QueryBundles(mongo_connection *conn,char *bTypeRegex,char *bNameRegex);
struct Rlist *CFDB_QueryBundleClasses(mongo_connection *conn, char *bType, char *bName);
struct Item *CFDB_QueryBundleArgs(mongo_connection *conn, char *bType, char *bName);
struct Item *CFDB_QueryBundlesUsing(mongo_connection *conn, char *bNameReferenced);
int CFDB_QueryBundleCount(mongo_connection *conn);
int CFDB_QueryPromiseCount(mongo_connection *conn);
struct HubBody *CFDB_QueryBody(mongo_connection *conn, char *type, char *name);
struct Item *CFDB_QueryAllBodies(mongo_connection *conn,char *bTypeRegex,char *bNameRegex);
struct Item *CFDB_QueryCdpAcls(mongo_connection *conn, char *sep);
struct Item *CFDB_QueryCdpCommands(mongo_connection *conn, char *sep);
struct Item *CFDB_QueryCdpPromiser(mongo_connection *conn, char *sep, char *bundleName, char *promiseType);
int CFDB_QueryLastFileChange(mongo_connection *conn, char *keyHash, char *reportType, char *fileName, char *outBuf, int outBufSz);
struct Item *CFDB_QueryCdpRegistry(mongo_connection *conn, char *sep);
struct Item *CFDB_QueryCdpServices(mongo_connection *conn, char *sep);
struct Item *CFDB_QueryCdpCompliance(mongo_connection *conn, char *handle);

void CFDB_ListEverything(mongo_connection *conn);
void CFDB_ScanHubHost(bson_iterator *it,char *keyhash,char *ipaddr,char *hostnames);
int QueryHostsWithClass(mongo_connection *conn, bson_buffer *bb, char *classRegex);
int QueryInsertHostInfo(mongo_connection *conn,struct Rlist *host_list);
void PrintCFDBKey(bson_iterator *it, int depth);
int CFDB_IteratorNext(bson_iterator *it, bson_type valType);
int Nova_MagViewOffset(int start_slot,int dbslot,int wrap);
int CFDB_QueryHostCount(mongo_connection *conn);
int CFDB_QueryHostName(mongo_connection *conn, char *ipAddr, char *hostName, int hostNameSz);
void MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra);

#endif

/* db_save.c */

#ifdef HAVE_LIBMONGOC

int CFDB_Open(mongo_connection *conn, char *host, int port);
int CFDB_Close(mongo_connection *conn);
void CFDB_Initialize(void);

int CFDB_PutValue(char *lval,char *rval);

void CFDB_SaveSoftware(mongo_connection *conn,enum software_rep sw, char *kH, struct Item *data);
void CFDB_SaveMonitorData(mongo_connection *conn, char *kH, enum monitord_rep rep_type, struct Item *data);
void CFDB_SaveMonitorHistograms(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveClasses(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveVariables(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveVariables2(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveTotalCompliance(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SavePromiseLog(mongo_connection *conn, char *kH, enum promiselog_rep rep_type, struct Item *data);
void CFDB_SaveLastSeen(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveMeter(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SavePerformance(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveSetUid(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SavePromiseCompliance(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveFileChanges(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveFileDiffs(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveBundles(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveValueReport(mongo_connection *conn, char *kH, struct Item *data);
void CFDB_SaveHostID(mongo_connection *conn, char *database, char *keyhash,char *ipaddr);
void Nova_CheckGlobalKnowledgeClass(char *name,char *key);
void BsonToString(char *retBuf, int retBufSz, char *data);
void CFDB_SaveLastUpdate(mongo_connection *conn, char *keyhash);

struct HubQuery *CFDB_QueryCachedTotalCompliance(mongo_connection *conn, char *policy, time_t minGenTime);
void CFDB_SaveCachedTotalCompliance(mongo_connection *conn, char *policy, int slot, double kept, double repaired, double notkept, int count, time_t genTime);
/*
 * commenting
 */
int CFDB_AddNote(mongo_connection *conn, char *keyhash, int reportType, char *nid, char *reportData, struct Item *data);
struct Rlist *CFDB_QueryNotes(mongo_connection *conn,char *keyhash, char *nid, struct Item *data);
struct Rlist *CFDB_QueryNoteId(mongo_connection *conn,bson *query);
void CFDBRef_AddToRow(mongo_connection *conn, char *coll,bson *query, char *row_name, char *cid);
int CFDB_GetRow(mongo_connection *conn, char *db, bson *query, char *rowname, char *row, int rowSz, int level);
struct Item *CFDB_QueryDistinctStr(mongo_connection *conn, char *database, char *collection, char *dKey, char *qKey, char *qVal);
struct Item *CFDB_QueryDistinct(mongo_connection *conn, char *database, char *collection, char *dKey, bson *queryBson);
void BsonIteratorToString(char *retBuf, int retBufSz, bson_iterator *i, int depth);
#endif  /* HAVE_LIBMONGOC */

/* db_maintain.c */

void CFDB_Maintenance(void);
#ifdef HAVE_LIBMONGOC
void CFDB_EnsureIndeces(mongo_connection *conn);
void CFDB_PurgeDropReports(mongo_connection *conn);
void CFDB_PurgeTimestampedReports(mongo_connection *conn);
void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now);
void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now, struct Item **purgeKeysPtr, struct Item **purgeNamesPtr);
int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, struct Item **purgeKeysPtr, struct Item **purgeNamesPtr);
void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now, struct Item **purgeKeysPtr);
void DeleteFromBsonArray(bson_buffer *bb, char *arrName, struct Item *elements);
void CFDB_PurgeHost(mongo_connection *conn, char *keyHash);
#endif /* HAVE_LIBMONGOC */


/* datapackaging.c */

void Nova_PackPerformance(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackClasses(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackSetuid(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackFileChanges(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackDiffs(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackMonitorWeek(struct Item **reply,char *header,time_t from,enum cfd_menu type);
void Nova_PackMonitorMag(struct Item **reply,char *header,time_t from,enum cfd_menu type);
void Nova_PackMonitorHist(struct Item **reply,char *header,time_t from,enum cfd_menu type);
void Nova_PackMonitorYear(struct Item **reply,char *header,time_t from,enum cfd_menu type);
void Nova_PackCompliance(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackSoftware(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackAvailPatches(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackPatchStatus(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_Pack_promise_output_common(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackValueReport(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackVariables(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackVariables2(struct Item **reply,char *header,time_t from,enum cfd_menu type);
void Nova_PackLastSeen(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackTotalCompliance(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackRepairLog(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackNotKeptLog(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackMeter(struct Item **reply,char *header,time_t date,enum cfd_menu type);
void Nova_PackBundles(struct Item **reply,char *header,time_t date,enum cfd_menu type);
int Nova_CoarseLaterThan(char *key,char *from);
int Nova_YearSlot(char *day,char *month, char *lifecycle);
int Nova_LaterThan(char *bigger,char *smaller);
char *Nova_ShortArch(char *arch);

/* dataunpack.c */

void Nova_UnPackPerformance(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackClasses(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackSetuid(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackFileChanges(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackDiffs(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackMonitorWeek(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackMonitorMag(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackMonitorHist(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackMonitorYear(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackCompliance(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackSoftware(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackAvailPatches(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackPatchStatus(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPack_promise_output_common(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackValueReport(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackVariables(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackVariables2(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackLastSeen(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackTotalCompliance(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackRepairLog(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackNotKeptLog(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackMeter(mongo_connection *dbconn, char *id, struct Item *data);
void Nova_UnPackBundles(mongo_connection *dbconn, char *id, struct Item *data);
char *Nova_LongArch(char *arch);

/* environments.c */

int Nova_EnvironmentsSanityChecks(struct Attributes a,struct Promise *pp);
void Nova_VerifyEnvironments(struct Attributes a,struct Promise *pp);
void Nova_VerifyEnvironmentsPromise(struct Promise *pp);
void Nova_VerifyVirtDomain(char *uri,enum cfhypervisors envtype,struct Attributes a,struct Promise *pp);
void Nova_VerifyVirtNetwork(char *uri,enum cfhypervisors envtype,struct Attributes a,struct Promise *pp);
void Nova_NewEnvironmentsContext(void);
void Nova_DeleteEnvironmentsContext(void);
#ifdef HAVE_LIBVIRT
int Nova_CreateVirtDom(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int Nova_DeleteVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int Nova_DeleteVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int Nova_RunningVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int Nova_SuspendedVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int Nova_DownVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp);
int VerifyZone(struct Attributes a,struct Promise *pp);
void Nova_EnvironmentErrorHandler(void);
void Nova_ShowRunList(virConnectPtr vc);
void Nova_ShowDormant(virConnectPtr vc);
void Nova_ShowNetworks(virConnectPtr vc,char **networks);
int Nova_CreateVirtNetwork(virConnectPtr vc,char **networks,struct Attributes a,struct Promise *pp);
int Nova_DeleteVirtNetwork(virConnectPtr vc,char **networks,struct Attributes a,struct Promise *pp);
#endif


/* histogram.c */

int Nova_ViewHisto(char *keyhash,enum observables obs,char *buffer,int bufsize);
int Nova_ReadHistogram(struct CfDataView *cfv,char *hostkey,enum observables obs);
void Nova_DrawHistoAxes(struct CfDataView *cfv,int col);
void Nova_PlotHistogram(struct CfDataView *cfv,char *buffer,int bufsize);
struct Item *Nova_MapHistogram(struct CfDataView *cfv,char *keyhash,enum observables obs);
void Nova_AnalyseHistogram(char *keyhash,enum observables obs,char *buffer,int bufsize);

/* html.c */
void Nova_Footer(char *buffer, int bufsize);
void Nova_Header(char *title,char *device_type, char *buffer, int bufsize);
void Nova_TabMenu(char *s,char *buffer, int bufsize);
void Nova_IncludeFile(char *name,char *buffer,int bufsize);

/* hub.c */

void Nova_StartHub(int argc,char **argv);
void Nova_ParallelizeScan(struct Item *masterlist,struct Attributes a,struct Promise *pp);
void Nova_ScanList(struct Item *list,struct Attributes a,struct Promise *pp);
int Nova_HailPeer(char *host,struct Attributes a,struct Promise *pp);
struct Item *Nova_ScanClients(void);
void Nova_HubLog(char *s);
void Nova_CountMonitoredClasses(void);
void Nova_CacheTotalCompliance(bool allSlots);
void Nova_CacheTotalComplianceEnv(mongo_connection *conn, char *envName, char *envClass, int slot, time_t start, time_t now);
int Nova_ShiftChange(void);

/* install.c */

void PrependPromiserList(struct PromiseIdent **list,char *s,struct Promise *pp);
struct HubQuery *NewHubQuery(struct Rlist *hosts,struct Rlist *records);
void DeleteHubQuery(struct HubQuery *hq,void (*fnptr)());
struct HubHost *NewHubHost(char *keyhash,char *host,char *ipaddr);
struct HubHost *GetHubHostIn(struct Rlist *host_list, char *keyhash);
void DeleteHubHost(struct HubHost *hp);
struct HubSoftware *NewHubSoftware(struct HubHost *hh,char *name,char *version,char *arch);
void DeleteHubSoftware(struct HubSoftware *hs);
struct HubClass *NewHubClass(struct HubHost *hh,char *class,double p, double dev, time_t t);
void DeleteHubClass(struct HubClass *hc);
struct HubClassSum *NewHubClassSum(struct HubHost *hh,char *class,int frequency);
void DeleteHubClassSum(struct HubClassSum *hc);
struct HubTotalCompliance *NewHubTotalCompliance(struct HubHost *hh,time_t t,char *v,int k,int r,int n);
void DeleteHubTotalCompliance(struct HubTotalCompliance *ht);
struct HubVariable *NewHubVariable(struct HubHost *hh,char *type,char *scope,char *lval,void *rval,char rtype,time_t t);
void DeleteHubVariable(struct HubVariable *hv);
struct HubPromiseLog *NewHubPromiseLog(struct HubHost *hh, char *handle,char *cause,time_t t, char *noteId,char *oid);
void DeleteHubPromiseLog(struct HubPromiseLog *hp);
struct HubPromiseSum *NewHubPromiseSum(struct HubHost *hh,char *handle,char *cause,int occurences, int hostOccurences);
void DeleteHubPromiseSum(struct HubPromiseSum *hs);
struct HubLastSeen *NewHubLastSeen(struct HubHost *hh,char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev,time_t t);
void DeleteHubLastSeen(struct HubLastSeen *hp);
struct HubMeter *NewHubMeter(struct HubHost *hh,char type,double kept,double repaired);
void DeleteHubMeter(struct HubMeter *hp);
struct HubPerformance *NewHubPerformance(struct HubHost *hh,char *event,time_t t,double q,double e,double d,char *noteid, char *handle);
void DeleteHubPerformance(struct HubPerformance *hp);
struct HubSetUid *NewHubSetUid(struct HubHost *hh,char *file);
void DeleteHubSetUid(struct HubSetUid *hp);
struct HubPromiseCompliance *NewHubCompliance(struct HubHost *hh,char *handle,char status,double e,double d,time_t t);
void DeleteHubPromiseCompliance(struct HubPromiseCompliance *hp);
struct HubBundleSeen *NewHubBundleSeen(struct HubHost *hh,char *rname,double ago,double avg,double dev,time_t t,char *noteid);
void DeleteHubBundleSeen(struct HubBundleSeen *hp);
struct HubFileChanges *NewHubFileChanges(struct HubHost *hh,char *file,time_t t, char *noteid,char *handle);
void DeleteHubFileChanges(struct HubFileChanges *hp);
struct HubFileDiff *NewHubFileDiff(struct HubHost *hh,char *file,char *diff,time_t t);
void DeleteHubFileDiff(struct HubFileDiff *hp);
struct HubValue *NewHubValue(struct HubHost *hh,char *day,double kept,double repaired,double notkept,char *noteid, char *handle);
void DeleteHubValue(struct HubValue *hp);
struct HubPromise *NewHubPromise(char *bn,char *bt,char *ba,char *pt, char *pr, char *pe, char *cl, char *ha, char *co, char *fn, int lno, char **cons);
void DeleteHubPromise(struct HubPromise *hp);
struct HubBody *NewHubBody(char *bodyName,char *bodyType,char *bodyArgs);
void DeleteHubBody(struct HubBody *hb);
struct HubBodyAttr *NewHubBodyAttr(struct HubBody *hb,char *lval,char *rval,char *classContext);
void DeleteHubBodyAttributes(struct HubBodyAttr *ha);
struct HubNote *NewHubNote(char *user,char *msg,time_t t);
struct HubNoteInfo *NewHubNoteInfo(struct HubHost *hh,char *nid,char *user,char *msg,time_t t,char *reportData, int reportType);
void DeleteHubNote(struct HubNote *hc);
void DeleteHubNoteInfo(struct HubNoteInfo *hci);

struct HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int count, double kept, double repaired, double notkept, time_t genTime);
void DeleteHubCacheTotalCompliance(struct HubCacheTotalCompliance *tc);

int SortPromiseLog(void *p1, void *p2);
int SortBusinessValue(void *p1, void *p2);
int SortTotalCompliance(void *p1, void *p2);
int SortFileChanges(void *p1, void *p2);
int SortFileDiff(void *p1, void *p2);
int SortLastSeen(void *p1, void *p2);
int SortPerformance(void *p1, void *p2);
int SortPromiseCompliance(void *p1, void *p2);
int SortClasses(void *p1, void *p2);
int SortClassSum(void *p1, void *p2);
int SortSoftware(void *p1, void *p2);
int SortBundleSeen(void *p1, void *p2);
int SortPromisePopularAscending(void *p1, void *p2);
int SortPromisePopularDescending(void *p1, void *p2);
struct HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(struct Rlist *records, int slot);

int PageRecords(struct Rlist **records_p, struct PageInfo *page,void (*fnptr)());
void CountMarginRecordsVars(struct Rlist **records_p, struct PageInfo *page,int *start_count,int *end_count);

/* knowledge.c */

char *Name2Id(char *s);
void Nova_StoreKMDB(struct Topic **topichash,struct Occurrence *occurrences,struct Inference *inferences);
void Nova_SyntaxCompletion(char *s);
void Nova_ListAgents(void);
void Nova_ListFunctions(void);
void Nova_ListFunction(struct FnCallType f,int full);
void Nova_ListPromiseTypes(void);

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,const char *version);
void Nova_ShowTopicRepresentation(FILE *fp);
void Nova_BundleReference(FILE *fp,char *bundle);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_PromiseNode(FILE *fp,struct Promise *pp,int calltype);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_BundleNode(FILE *fp,char *bundle);
void Nova_BodyNode(FILE *fp,char *body,int calltype);
char *Nova_PromiseID(struct Promise *pp);
void Nova_DependencyGraph(struct Topic *map);
void Nova_PlotTopicDependencies(int topic,double **adj,char **names,int dim);
void Nova_MapClassParameterAssociations(FILE *fp, struct Promise *pp,char *promise_id);
double NovaShiftAverage(double new,double old);
double NovaExtractValueFromStream(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp);
void NovaLogSymbolicValue(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp);
void Nova_ShowBundleDependence(FILE *fp);
void Nova_RegisterBundleDepedence(char *name,struct Promise *pp);
char *NovaEscape(char *s);
void NovaShowValues(FILE *fp,struct BodySyntax bs);

/* license.c */

int Nova_HashKey(char *filename,char *buffer,unsigned char digest[EVP_MAX_MD_SIZE+1],char *hash);
int Nova_EnterpriseExpiry(char *day,char *month,char *year,char *company);
int Nova_EnterpriseModuleExpiry(char *day,char *month,char *year);
void Nova_CheckLicensePromise(void);
void Nova_LogLicenseStatus(void);
int Nova_CheckLicenseWin(char *pos);
char *Nova_LicenseOwner(void);

/* logging.c */

void Nova_RemoteSyslog(struct Attributes a,struct Promise *pp);

/* magnify.c */

int Nova_ViewMag(char *keyhash,enum observables obs,char *buffer,int bufsize);
int Nova_ReadMagTimeSeries(struct CfDataView *cfv,char *hostkey,enum observables obs);
void Nova_PlotMagQFile(struct CfDataView *cfv,char *buffer,int bufsize);
void Nova_AnalyseMag(char *keyhash,enum observables obs,char *buffer,int bufsize);

/* monitoring.c */

void Nova_GetClassName(int i,char *name,char *desc);
void NovaInitMeasurements(void);
void Nova_HistoryUpdate(time_t time, const struct Averages *newvals);
void Nova_UpdateShiftAverage(struct Averages *shift_value,struct Averages *newvals);
void Nova_ResetShiftAverage(struct Averages *shift_value);
double ShiftAverage(double new,double old);
void Nova_VerifyMeasurement(double *this,struct Attributes a,struct Promise *pp);
void Nova_LongHaul(time_t starttime);
int NovaRegisterSlot(const char *name, const char *description);
const char *NovaGetSlotName(int idx);
struct Item *NovaGetMeasurementStream(struct Attributes a,struct Promise *pp);
struct Item *NovaReSample(int slot,struct Attributes a,struct Promise *pp);
void NovaNamedEvent(char *eventname,double value,struct Attributes a,struct Promise *pp);
void Nova_SetMeasurementPromises(struct Item **classlist);
void Nova_LoadSlowlyVaryingObservations(void);
void Nova_DumpSlowlyVaryingObservations(void);
void Nova_LookupClassName(int n,char *name, char *desc);
void Nova_SaveFilePosition(char *filename,long fileptr);
long Nova_RestoreFilePosition(char *filename);
void Nova_LookupAggregateClassName(int n,char *name,char *desc);

/* - date-related functions - */

time_t WeekBegin(time_t time);
time_t SubtractWeeks(time_t time, int weeks);
time_t NextShift(time_t time);
bool GetRecordForTime(CF_DB *db, time_t time, struct Averages *result);
void PutRecordForTime(CF_DB *db, time_t time, const struct Averages *values);
double BoundedValue(double value, double defval);

/* outputs.c */

void Nova_VerifyOutputsPromise(struct Promise *pp);
void Nova_SetPromiseOutputs(struct Promise *pp);
void Nova_SetBundleOutputs(char *name);
void Nova_ResetBundleOutputs(char *name);

/* packages.c */

int Nova_GetInstalledPkgsRpath(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp);
int Nova_ExecPackageCommandRpath(char *command,int verify,int setCmdClasses,struct Attributes a,struct Promise *pp);

/* processes.c */

char *Nova_GetProcessOptions(void);
void Nova_LogFileChange(char *file,int change,struct Attributes a,struct Promise *pp);
void Nova_DoFileDiff(char *file,char *destination,struct stat sb,struct stat dsb);
int Nova_GetFirstChangePosition(char *file,char *destination);
int Nova_FileIsBinary(char *name,int size,int maxsize);
void Nova_ReportFileChange(FILE *fp,char *file,char *destination,int maxsize);
int Nova_LoadFileHunks(char *file,char *destination,struct CfFileLine **list1,struct CfFileLine **list2,int *l1,int *l2,int maxsize);
struct CfFileLine *AppendFileLine(struct CfFileLine **liststart,char *item,int pos);
void DeleteAllFileLines(struct CfFileLine *item);
void DeleteFileLine(struct CfFileLine  **liststart,struct CfFileLine *item);

/* promise_db.c */

void Nova_StoreExpandedPromise(struct Promise *pp);
void Nova_StoreUnExpandedPromises(struct Bundle *bundles,struct Body *bodies);
void Nova_StoreBody(mongo_connection *dbconn, struct Body *body);

/* promises.c */

void Nova_Version(void);
char *Nova_GetVersion(void);
char * Nova_StrVersion(void);
void Nova_NotePromiseCompliance(struct Promise *pp,double val,enum cf_status status,char *reason);
time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *t);
void Nova_CheckAutoBootstrap(void);
void Nova_SetPolicyServer(char *name);
void Nova_CreateFailSafe(char *name);
void Nova_SetDocRoot(char *name);
void Nova_EnterpriseContext(void);
void Nova_PreSanitizePromise(struct Promise *pp);
void Nova_TrackValue(char *date,double kept,double repaired, double notkept);
void Nova_LastSawBundle(char *name);
void Nova_EnterpriseDiscovery(void);
void Nova_AnalyzePromiseConflicts(void);
void Nova_NewPromiser(struct Promise *pp);
int Nova_ClassesIntersect(struct Rlist *contexts1,struct Rlist *contexts2);

/* pscalar.c */

int Nova_GetPersistentScalar(char *lval,char *rval,int size,time_t timeout);
void Nova_SetPersistentScalar(char *lval,char *rval);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s,struct Attributes a,struct Promise *pp);
int Nova_CopyRegistryValue(char *key,char *value,char *buffer);


/* reporting.c */

void Nova_CSV2XML(struct Rlist *list);
void Nova_SummarizePromiseRepaired(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizePromiseNotKept(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizePerPromiseCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_ReportSoftware(struct CfPackageManager *list);
void Nova_ReportPatches(struct CfPackageManager *list);
void Nova_SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeVariables(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeValue(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_NoteVarUsage(void);
void Nova_GrandSummary(void);
void Nova_SummarizeComms(void);
void Nova_SummarizeLicense(char *stylesheet,char *banner,char *footer,char *webdriver);
void Nova_ZenossSummary(char *docroot);
void Nova_NoteVarUsageDB(void);

void Nova_OpenCompilationReportFiles(const char *fname);
void Nova_ShowPromises(struct Bundle *bundles, struct Body *bodies);
void Nova_ShowPromise(const char *version, struct Promise *pp, int indent);
int Nova_ExportReports(char *reportName);
int Nova_ImportHostReports(char *filePath);


/* request.c */

void Nova_CfQueryCFDB(char *query);

/* scorecards.c */

void Nova_BarMeter(int pos,double kept,double rep,char *name,char *buffer,int bufsize);
void Nova_PerformancePage(char *docroot,char *hostkey,char *buffer,int bufsize);
void Nova_Meter(char *hostkey,char *buffer,int bufsize);
struct Item *Nova_RankHosts(char *search_string,int regex,enum cf_rank_method method,int max_return);
struct Item *Nova_GreenHosts(void);
struct Item *Nova_YellowHosts(void);
struct Item *Nova_RedHosts(void);
struct Item *Nova_BlueHosts(void);
struct Item *Nova_ClassifyHostState(char *search_string,int regex,enum cf_rank_method method,int max_return);
int Nova_GetComplianceScore(enum cf_rank_method method,double *k,double *rep);
int Nova_IsGreen(int level);
int Nova_IsYellow(int level);
int Nova_IsRed(int level);
int Nova_IsBlue(int level);
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize);
void Nova_DrawComplianceAxes(struct CfDataView *cfv,int col);
int Nova_GetHostColour(char *lkeyhash);

/* server.c */

const EVP_CIPHER *Nova_CfengineCipher(char type);
char Nova_CfEnterpriseOptions(void);
int Nova_CfSessionKeySize(char c);
void  Nova_RegisterLiteralServerData(char *handle,struct Promise *pp);
int Nova_ReturnLiteralData(char *handle,char *retval);
int Nova_ReturnQueryData(struct cfd_connection *conn,char *menu);
void Nova_PackAllReports(struct Item **reply, time_t from, time_t delta1, enum cfd_menu type);
char *Nova_GetRemoteScalar(char *proto,char *handle,char *server,int encrypted,char *rcv);
int Nova_ParseHostname(char *name,char *hostname);

int Nova_RetrieveUnreliableValue(char *caller,char *handle,char *buffer);
void Nova_CacheUnreliableValue(char *caller,char *handle,char *buffer);
pid_t Nova_StartTwin(int argc,char **argv);
void Nova_SignalTwin(void);
void Nova_SignalOther(void);
void Nova_ReviveOther(int argc,char **argv);
void Nova_TranslatePath(char *new, const char *old);
RSA *Nova_SelectKeyRing(char *name);
void Nova_IdempAddToKeyRing(char *name,char *ip,RSA *key);
void Nova_PurgeKeyRing(void);

/* services.c */

void Nova_VerifyServices(struct Attributes a,struct Promise *pp);

/* services_win.c */

void NovaWin_VerifyServices(struct Attributes a,struct Promise *pp);
#ifdef MINGW
int NovaWin_CheckServiceStatus(char *srvName, enum cf_srv_policy policy, char *argStr,int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs);
int NovaWin_CheckServiceStart(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int argc, char **argv, int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs);
int NovaWin_CheckServiceStop(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps, int isDependency, int unDisable, struct Attributes a,struct Promise *pp, int setCfPs);
int NovaWin_CheckServiceDisable(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps, int isDependency, struct Attributes a,struct Promise *pp, int setCfPs);
int NovaWin_ServiceDepsRunning(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int *allDepsRunning);
int NovaWin_StopDependentServices(SC_HANDLE managerHandle, SC_HANDLE srvHandle, int onlyCheckDeps);
int NovaWin_SetServiceStartTime(SC_HANDLE srvHandle, DWORD setState, int onlyFrom, DWORD fromState, int *changeRes);
int NovaWin_SetSrvDepsStartTime(SC_HANDLE managerHandle, SC_HANDLE srvHandle, DWORD setState, int onlyFrom, DWORD fromState);
int NovaWin_ServiceStateWait(SC_HANDLE srvHandle, DWORD state);
QUERY_SERVICE_CONFIG *NovaWin_AllocServiceConfig(SC_HANDLE srvHandle);
void NovaWin_AllocSplitServiceArgs(char *argStr, int *argcp, char ***argvp);
#endif  /* MINGW */

/* sql.c */

int Nova_ValidateSQLTableName(char *table_path,char *db,char *table);
int Nova_VerifyTablePromise(CfdbConn *cfdb,char *table_path,struct Rlist *columns,struct Attributes a,struct Promise *pp);
int Nova_ValidateSQLTableName(char *table_path,char *db,char *table);
void Nova_QueryTableColumns(char *s,char *db,char *table);
int Nova_NewSQLColumns(char *table,struct Rlist *columns,char ***name_table,char ***type_table,int **size_table,int **done);
void Nova_DeleteSQLColumns(char **name_table,char **type_table,int *size_table,int *done,int len);
void Nova_CreateDBQuery(enum cfdbtype type,char *query);
int Nova_CreateTableColumns(CfdbConn *cfdb,char *table,struct Rlist *columns,struct Attributes a,struct Promise *pp);
int NovaCheckSQLDataType(char *type,char *ref_type,struct Promise *pp);
int Nova_TableExists(CfdbConn *cfdb,char *name);
struct Rlist *Nova_GetSQLTables(CfdbConn *cfdb);
void Nova_ListTables(int type,char *query);

/* syntax.c */

void Nova_SyntaxTree2JavaScript(void);
void Nova_ShowBundleTypes(void);
void Nova_ShowControlBodies(void);
void Nova_ShowPromiseTypesFor(char *s, struct SubTypeSyntax *commonMerge);
void Nova_ShowBodyParts(struct BodySyntax *bs,int i);
void Nova_ShowRange(char *s,enum cfdatatype type);
void Nova_ShowBuiltinFunctions(void);
void Nova_Indent(int i);

/* topicmap.c */

void Nova_DumpTopics(void);
void Nova_FillInGoalComment(struct Item *ip);
char *Nova_GetBundleComment(char *bundle);
void Nova_WebTopicMap_Initialize(void);
int Nova_GetTopicIdForTopic(char *typed_topic);
void Nova_LookupUniqueAssoc(int pid,char *buffer,int bufsize);
void Nova_ScanTheRest(int pid,char *buffer, int bufsize);
int Nova_SearchTopicMap(char *typed_topic,char *buffer,int bufsize);
struct Item *Nova_ScanLeadsAssociations(int pid,char *view);
void Nova_ScanOccurrences(int this_id,char *buffer, int bufsize);

int Nova_GetTopicByTopicId(int pid,char *topic_name,char *topic_id,char *topic_type);
int Nova_AddTopicSearchBuffer(int pid,char *topic_name,char *topic_type,char *buffer,int bufsize);
int Nova_AddAssocSearchBuffer(char *from_assoc,char *to_assoc,char *buffer,int bufsize);
void Nova_AddOccurrenceBuffer(char *context,char *locator,enum representations locator_type,char *represents,char *buffer,int bufsize);
void AtomizeTopicContext(struct AlphaList *context_list,char *topic_context);
char *Nova_TopicIdURL(int pid,char *s);
char *Nova_AssocURL(char *s);
char *Nova_URL(char *s,char *rep);
struct Item *Nova_NearestNeighbours(int topic_id,char *assoc_mask);
struct Item *Nova_GetTopicsInContext(char *context);
struct Item *Nova_GetBusinessGoals(char *handle);
struct Item *Nova_GetUniqueBusinessGoals(void);

void Nova_SpecialQuote(char *name,char *type);
void Nova_PlotTopicCosmos(int topic,char *view,char *buf,int size);
int Nova_GetTribe(int *tribe_id,struct CfGraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int pid,char *v);
void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE],double *v,int dim);
void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE],double *v,int dim);
int Nova_AlreadyInTribe(int node, int *tribe_id);
void Nova_InitVertex(struct CfGraphNode *tribe,int i);
int Nova_NewVertex(struct CfGraphNode *tribe,int node,int distance,int real,char *name,char *context);

/* web_api.c */

#include "cf.nova.web_api.h"

/* weekly.c */

double Num(double x);
int Nova_ViewWeek(char *keyhash,enum observables obs,char *buffer,int bufsize);
int Nova_ReadTimeSeries(struct CfDataView *cfv,char *keyhash,enum observables obs);
void Nova_DrawQAxes(struct CfDataView *cfv,int col);
void Nova_PlotQFile(struct CfDataView *cfv,char *buffer,int bufsze);
void Nova_AnalyseWeek(char *keyhash,enum observables obs,char *buffer,int bufsize);

#ifdef MINGW
/* win_api.c */

uid_t getuid(void);
gid_t getgid(void);
int lstat(const char *file_name, struct stat *buf);
unsigned int sleep(unsigned int seconds);
int chown(const char *path, uid_t owner, gid_t group);
int NovaWin_chmod(const char *path, mode_t mode);
int fchmod(int fildes, mode_t mode);
long int random(void);
void srandom(unsigned int seed);
void setlinebuf(FILE *stream);
int NovaWin_stat(const char *path, struct stat *buf);
void NovaWin_OpenNetwork(void);
void NovaWin_CloseNetwork(void);
char *NovaWin_GetErrorStr(void);
time_t NovaWin_FileTimeToTimet(FILETIME *ft);
int NovaWin_uname(struct utsname *buf);

/* win_file.c */

void NovaWin_CreateEmptyFile(char *name);
FILE *NovaWin_FileHandleToStream(HANDLE fHandle, char *mode);
int NovaWin_IsExecutable(char *file);
int NovaWin_mkdir(const char *path, mode_t mode);
int NovaWin_rename(const char *oldpath, const char *newpath);
int NovaWin_FileExists(const char *fileName);
int NovaWin_IsDir(char *fileName);
int NovaWin_VerifyOwner(char *file,struct Promise *pp,struct Attributes attr);
int NovaWin_TakeFileOwnership(char *path);
int NovaWin_SetFileOwnership(char *path, SID *sid);
int NovaWin_GetOwnerName(char *path, char *owner, int ownerSz);
void NovaWin_VerifyFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp);
void NovaWin_VerifyCopiedFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp);
off_t NovaWin_GetDiskUsage(char *file,enum cfsizes type);
int NovaWin_GetNumHardlinks(char *path, int *numHardLinks);

/* win_log.c */

void NovaWin_OpenLog(void);
int NovaWin_CheckRegistryLogKey(void);
void NovaWin_CloseLog(void);
void NovaWin_MakeLog(struct Item *mess,enum cfreport level);
void NovaWin_LogPromiseResult(char *promiser, char peeType, void *promisee, char status, enum cfreport log_level, struct Item *mess);

/* win_mon.c */

void GatherCPUData(double *CF_THIS);

/* win_net.c */

void NovaWin_GetInterfaceInfo(void);
int NovaWin_TryConnect(struct cfagent_connection *conn, struct timeval *tvp, struct sockaddr *cinp, int cinpSz);

/* win_pipe.c */

FILE *NovaWin_cf_popen(char *command,char *type);
FILE *NovaWin_cf_popen_sh(char *command,char *type);
FILE *NovaWin_cf_popensetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv,int background);
FILE *NovaWin_cf_popen_shsetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv,int background);
int NovaWin_cf_pclose(FILE *pp);
int NovaWin_cf_pclose_def(FILE *pfp, struct Attributes a, struct Promise *pp);

/* win_proc.c */

int NovaWin_DoAllSignals(struct Item *siglist,struct Attributes a,struct Promise *pp);
int NovaWin_GracefulTerminate(pid_t pid);
int NovaWin_IsProcessRunning(pid_t pid);
int NovaWin_ShellCommandReturnsZero(char *comm, int useshell);
int NovaWin_RunCmd(char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle);
int NovaWin_GetCurrentProcessOwner(SID *sid, int sidSz);
int NovaWin_GetCurrentUserName(char *userName, int userNameLen);
int NovaWin_SetTokenPrivilege(HANDLE token, char *privilegeName, int enablePriv);

/* win_ps.c */

int NovaWin_LoadProcessTable(struct Item **procdata);
int NovaWin_GetProcessSnapshot(struct Item **procdata);
int NovaWin_GatherProcessUsers(struct Item **userList, int *userListSz, int *numRootProcs, int *numOtherProcs);

/* win_service_exec.c */

void NovaWin_StartExecService(void);

/* win_sysinfo.c */

int NovaWin_GetWinDir(char *winDir, int winDirSz);
int NovaWin_GetSysDir(char *sysDir, int sysDirSz);
int NovaWin_GetProgDir(char *progDir, int progDirSz);
int NovaWin_GetEnv(char *varName, char *varContents, int varContentsSz);

/* win_user.c */

int NovaWin_UserNameToSid(char *userName, SID *sid, DWORD sidSz, int shouldExist);
int NovaWin_GroupNameToSid(char *groupName, SID *sid, DWORD sidSz, int shouldExist);
int NovaWin_NameToSid(char *name, SID *sid, DWORD sidSz);
int NovaWin_SidToName(SID* sid, char *name, int nameSz);
int NovaWin_SidToString(SID *sid, char *stringSid, int stringSz);
int NovaWin_StringToSid(char *stringSid, SID *sid, int sidSz);
struct UidList *NovaWin_Rlist2SidList(struct Rlist *uidnames, struct Promise *pp);
struct Rval NovaWin_FnCallUserExists(struct FnCall *fp,struct Rlist *finalargs);
struct Rval NovaWin_FnCallGroupExists(struct FnCall *fp,struct Rlist *finalargs);

/* win_wmi.c */

int NovaWin_GetInstalledPkgs(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp);
void NovaWin_PrintWmiError(char *str);
int NovaWin_WmiInitialize(void);
int NovaWin_WmiDeInitialize(void);

#endif  /* MINGW */


/* yearly.c */

int Nova_ViewLongHistory(char *keyhash,enum observables obs,char *buffer,int bufsize);
int Nova_ReadLongHistory(struct CfDataView *cfv,char *keyhash,enum observables obs);
void Nova_DrawLongHAxes(struct CfDataView *cfv,int col);
void Nova_PlotLongHFile(struct CfDataView *cfv,char *buffer,int bufsize);
void Nova_AnalyseLongHistory(char *keyname,enum observables obs,char *buffer,int bufsize);

/***************************************************************************/

#define CF_METER_HEIGHT 80
#define CF_METER_WIDTH  500
#define CF_METER_MARGIN 5

/***************************************************************************/
/* Knowledge relationships                                                 */
/***************************************************************************/

// See also similar in cf3.defs.h

#define NOVA_USES "relies on promise from"
#define NOVA_GIVES "makes promise to"
#define NOVA_ACTIVATED "is activated by class context"
#define NOVA_ACTIVATES "activates promises"
#define NOVA_IMPACTS "has direct influence on"
#define NOVA_ISIMPACTED "is directly influenced by"
#define NOVA_BUNDLE_DATA "uses variables defined in"
#define NOVA_BUNDLE_DATA_INV_B "promises variables used by bundle"
#define NOVA_BUNDLE_DATA_INV_P "promises variables used by promise"
#define NOVA_HANDLE "is the handle for"
#define NOVA_HANDLE_INV "has a promise with handle"
#define NOVA_SYNONYM KM_SYNONYM
#define NOVA_GOAL "contributes to business goal"
#define NOVA_GOAL_INV "is supported by a promise"
#define NOVA_GEN "is derived from"
#define NOVA_REPORTED "are reported in"
#define NOVA_SEEALSO "see also"

/***************************************************************************/
/* DBs                                                                     */
/***************************************************************************/

#define NOVA_HISTORYDB "history" "." DB_FEXT
#define NOVA_MEASUREDB "nova_measures" "." DB_FEXT
#define NOVA_STATICDB  "nova_static" "." DB_FEXT
#define NOVA_PSCALARDB  "nova_pscalar" "." DB_FEXT
#define NOVA_COMPLIANCE "promise_compliance" "." DB_FEXT
#define NOVA_REGISTRY "mswin" "." DB_FEXT
#define NOVA_CACHE "nova_cache" "." DB_FEXT
#define NOVA_LICENSE "nova_track" "." DB_FEXT
#define NOVA_VALUE "nova_value" "." DB_FEXT
#define NOVA_NETWORK "nova_network" "." DB_FEXT
#define NOVA_GLOBALCOUNTERS "nova_counters" "." DB_FEXT

#define NOVA_BUNDLE_LOG "bundles" "." DB_FEXT
#define NOVA_DIFF_LOG "nova_diff.log"
#define NOVA_PATCHES_INSTALLED "software_patch_status.csv"
#define NOVA_PATCHES_AVAIL "software_patches_avail.csv"
/* #define NOVA_SOFTWARE_INSTALLED "software_packages.csv" Moved to cf3.defs.h */

#define CF_BIGNUMBER 999999

#define CF_RED_THRESHOLD 1000
#define CF_AMBER_THRESHOLD 100
#define CF_GREEN 0
#define CF_CODE_BLUE -1

#define CF_HUB_HORIZON 900 // 15 mins
#define CF_HUB_PURGESECS 604800  // one week

/* Long-term monitoring constants */

#define HOURS_PER_SHIFT 6

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_SHIFT (HOURS_PER_SHIFT * SECONDS_PER_HOUR)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define SECONDS_PER_WEEK (7 * SECONDS_PER_DAY)

#define SHIFTS_PER_DAY 4

#define SHIFTS_PER_WEEK (4*7)

#define MONITORING_HISTORY_LENGTH_YEARS 3
#define MONITORING_HISTORY_LENGTH_WEEKS (3*52)
/* Not a real year, but a "monitoring year" */
#define MONITORING_WEEKS_PER_YEAR 52

/***************************************************************************/

struct month_days
   {
   char *m;
   int d;
   };

struct promise_value
   {
   double kept;
   double repaired;
   double notkept;
   };

struct cf_pscalar
   {
   char rval[CF_MAXVARSIZE];
   time_t time;
   };

/***************************************************************************/

#define CF_REPAIR_LOG    "cf_repair.log"
#define CF_NOTKEPT_LOG   "cf_notkept.log"

/* Protocol codebook for reports */

#define CFR_PERF "PRF"
#define CFR_CLASS "CLS"
#define CFR_SETUID "SUI"
#define CFR_FCHANGE "CHG"
#define CFR_MONITOR_WEEK "MNW"
#define CFR_FDIFF "DIF"
#define CFR_MONITOR_MAG "MNM"
#define CFR_MONITOR_HIST "MNH"
#define CFR_MONITOR_YEAR "MNY"
#define CFR_PCOMPLIANCE "PCP"
#define CFR_TCOMPLIANCE "TCP"
#define CFR_SOFTWARE "SOF"
#define CFR_AVAILPATCH "AVL"
#define CFR_PATCHSTATUS "PST"
#define CFR_PROMISEOUT "POT"
#define CFR_VALUE "VAL"
#define CFR_VARS "VAR"
#define CFR_VARD "VRD"  // with date stamp
#define CFR_LASTSEEN "SEN"
#define CFR_REPAIRLOG "PRL"
#define CFR_NOTKEPTLOG "NKL"
#define CFR_METER "MET"
#define CFR_BUNDLES "BUN"

/* Keynames */

#define cfr_keyhash      "kH"
#define cfr_ip_array     "IP"
#define cfr_host_array   "ha"
#define cfr_histo        "hs"
#define cfr_perf_event    "P"
#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"
#define cfr_cause        "ca"
#define cfr_class        "cl"
#define cfr_class_keys   "ck"
#define cfr_class_jobs   "cj"
#define cfr_total_compliance "tc"
#define cfr_time          "t"
#define cfr_timeslot      "ts"
#define cfr_version       "v"
#define cfr_name          "n"
#define cfr_arch          "a"
#define cfr_kept          "k"
#define cfr_repaired      "r"
#define cfr_notkept       "N"
#define cfr_vars          "vr"
//#define cfr_var_keys      "vk"  // would optimize reads
#define cfr_type          "T"
#define cfr_rval          "V"
#define cfr_lval          "lval"
#define cfr_promisehandle "h"
#define cfr_lastseen      "ls"
#define cfr_ipaddr        "i"
#define cfr_hrsago        "a"
#define cfr_hrsavg        "v"
#define cfr_hrsdev        "d"
#define cfr_meter         "me"
#define cfr_meterkept     "K"
#define cfr_meterrepaired "R"
#define cfr_performance   "pf"
#define cfr_obs_q         "q"
#define cfr_obs_E         "e"
#define cfr_obs_sigma     "d"
#define cfr_setuid        "su"
#define cfr_promisecompl  "pc"
#define cfr_promisecompl_keys "pck"
#define cfr_promisestatus "s"
#define cfr_filechanges   "fc"
#define cfr_filediffs     "fd"
#define cfr_diff          "FD"
#define cfr_bundles       "B"
#define cfr_day           "t" // Substitute for time in value report
#define cfr_valuereport   "va"
#define cfr_netmeasure    "ne"
#define cfr_environment   "env"

#define cfr_netmeasure    "ne"


/* Promise DB */
#define cfp_bundlename    "bn"
#define cfp_bundletype    "bt"
#define cfp_bundleargs    "ba"
#define cfp_promisetype   "pt"
#define cfp_classcontext  "c"
#define cfp_file          "f"
#define cfp_lineno        "l"
#define cfp_promiser      "p"
#define cfp_promiser_exp  "px"
#define cfp_promisee      "e"
#define cfp_promisee_exp  "ex"
#define cfp_handle        "h"
#define cfp_handle_exp    "hx"
#define cfp_comment       "o"
#define cfp_comment_exp   "ox"
#define cfp_constraints   "n"
#define cfp_constraints_exp "nx"

/* Content-Driven Policy bundle names (from cdp_* policies) */
#define cfp_cdp_bundle_acls "cdp_acls"
#define cfp_cdp_bundle_commands "cdp_commands"
#define cfp_cdp_bundle_filechanges "cdp_file_changes"
#define cfp_cdp_bundle_filediffs "cdp_file_diffs"
#define cfp_cdp_bundle_registry "cdp_registry"
#define cfp_cdp_bundle_services "cdp_services"


/* Promise body DB */
#define cfb_bodyname      "bn"
#define cfb_bodytype      "bt"
#define cfb_bodyargs      "ba"
#define cfb_classcontext  "cc"

/* monitoring (vital sign) DB */

#define cfm_magobs        "mo"
#define cfm_weekobs       "wo"
#define cfm_yearobs       "yo"
#define cfm_data          "dt"

/*commenting*/
#define cfn_keyhash "_kh"
#define cfn_nid "nid"
#define cfn_reportdata "_rD"
#define cfn_note "n"
#define cfn_username "u"
#define cfn_datetime "d"
#define cfn_message "m"
#define cfn_reporttype "_t"

/* cache collection */
#define cfc_cachetype "ct"
#define cfc_cachecompliance "cc"  // total compliance
#define cfc_count "cn"
#define cfc_timegen "tg"

/* knowlegde/topic map collection */

#define cfk_topicname "n"
#define cfk_topiccontext "c"
#define cfk_topicid "id"

#define cfk_associations "a"
#define cfk_fwd "f"
#define cfk_bwd "b"
#define cfk_assocname "an"
#define cfk_assoccontext "ac"
#define cfk_associd "ai"

#define cfk_fwdsearch "a.f"

#define cfk_occurcontext "oc"
#define cfk_occurlocator "ol"
#define cfk_occurtype "ot"
#define cfk_occurrep "or"

#define cfk_precedent "p"
#define cfk_qualifier "q"
#define cfk_inference "i"

// Report types

#define CFREPORT_HOSTS 1
#define CFREPORT_REPAIRED 2
#define CFREPORT_PRSUMMARY 3
#define CFREPORT_PERFORMANCE 4
#define CFREPORT_VALUE 5
#define CFREPORT_FILECHANGES 6
#define CFREPORT_FILEDIFFS 7
#define CFREPORT_BUNDLE 8
#define CFREPORT_NOTKEPT 9

#define CF_NONOTE "NO_NOTE"
#define CF_SHOWNOTE "Show Note"
#define CF_ADDNOTE "Add Note"

#define CFDB_GREATERTHANEQ 4
#define CFDB_LESSTHANEQ 5

/*****************************************************************************/
/* Report DB API Structs                                                     */
/*****************************************************************************/

struct HubHost
   {
   char *keyhash;
   char *ipaddr;
   char *hostname;
   };

struct HubFileChanges
   {
   struct HubHost *hh;
   char *path;
   time_t t;
     char *nid;
   char *handle;
   };

struct HubFileDiff
   {
   struct HubHost *hh;
   char *path;
   char *diff;
   time_t t;
   };

struct HubSoftware
   {
   struct HubHost *hh;
   char *name;
   char *version;
   char *arch;
   };

struct HubClass
   {
   struct HubHost *hh;
   char *class;
   double prob;
   double dev;
   time_t t;
   };

struct HubClassSum
   {
   struct HubHost *hh;
   char *class;
   int frequency;  // across all hosts
   };

struct HubTotalCompliance
   {
   struct HubHost *hh;
   time_t t;
   char *version;
   int kept;
   int repaired;
   int notkept;
   };

struct HubVariable
   {
   struct HubHost *hh;
   char *scope;
   char *lval;
   void *rval;
   char *dtype;
   char rtype;
   time_t t;
   };

struct HubPromiseLog // promise kept,repaired or not kept
   {
   struct HubHost *hh;
     //   char *policy;
   char *handle;
   char *cause;
   time_t t;
   char *nid;  
   char *oid;  
   };

struct HubPromiseSum // promise not kept/repaired summary
   {
   struct HubHost *hh;
   char *policy;
   char *handle;
   char *cause;
   int occurences;
   int hostOccurences;
   };

struct HubLastSeen
   {
   struct HubHost *hh;      
   struct HubHost *rhost;
   char io;
   double hrsago;
   double hrsavg;
   double hrsdev;
   time_t t;
   };

struct HubBundleSeen
   {
   struct HubHost *hh;      
   char *bundle;
   double hrsago;
   double hrsavg;
   double hrsdev;
   time_t t;
   char *nid;
   };

struct HubValue
   {
   struct HubHost *hh;      
   char* day;
   double kept;
   double repaired;
   double notkept;
     char *nid;
     char *handle;
   };

struct HubMeter
   {
   struct HubHost *hh;      
   char type;
   double kept;
   double repaired;
   double notkept;
   };

#define cfmeter_hour 'H'
#define cfmeter_week 'W'
#define cfmeter_day  'D'
#define cfmeter_perf 'P'
#define cfmeter_comms 'C'
#define cfmeter_anomaly 'A'
#define cfmeter_other 'S'

struct HubPerformance
   {
   struct HubHost *hh;      
   char *event;
   double q;
   double e;
   double d;
   time_t t;
   char *nid;
     char *handle;  
   };
  
struct HubSetUid
   {
   struct HubHost *hh;      
   char *path;
   };

struct HubPromiseCompliance
   {
   struct HubHost *hh;
   char *handle;
   char status; // 'r' / 'k' / 'n'
   double e;
   double d;
   time_t t;
   };

struct HubQuery
   {
   struct Rlist *hosts;
   struct Rlist *records;
   };


struct HubPromise
   {
   char *bundleName;
   char *bundleType;
   char *bundleArgs;  // comma separated
   char *promiseType;
   char *promiser;
   char *promisee;
   char *classContext;
   char *handle;
   char *comment;
   char *file;
   int lineNo;
   char **constraints;
   double popularity;  // optional
   };


struct HubBody
  {
  char *bodyName;
  char *bodyType;
  char *bodyArgs; // comma separated
  struct HubBodyAttr *attr;
  };


struct HubBodyAttr
  {
  char *classContext;
  char *lval;
  char *rval;
  struct HubBodyAttr *next;
  };

/* cfreport.cache */

struct HubCacheTotalCompliance
  {
  char *policy;
  int slot;
  int count;
  double kept;
  double repaired;
  double notkept;
  time_t genTime;
  };


/*
 * Commenting on reports
 */ 
struct HubNote
{
  char *user;
  char *msg;
  time_t t;
  struct HubNote *next;
};

struct HubNoteInfo
{
  struct HubHost *hh;
  char *nid;
  char *user;
  char *msg;
  time_t t;
  char *report;
  int reportType;
  struct HubNote *note;
};


#endif


#ifdef HAVE_CONSTELLATION
#include <cf.constellation.h>
#endif

#endif
