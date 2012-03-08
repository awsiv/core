/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.h                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef CFENGINE_NOVA_H
#define CFENGINE_NOVA_H

#ifdef HAVE_CONFIG_H
# include "conf.h"
#endif

#include "cf3.defs.h"

#ifdef HAVE_LIBLDAP
/* LDAP functionality uses several functions which are only provided if this macro is defined */
# define LDAP_DEPRECATED 1
# include <ldap.h>
#endif

#ifdef HAVE_ACL_H
# include <acl.h>
#endif

#ifdef HAVE_SYS_ACL_H
# include <sys/acl.h>
#endif

#ifdef HAVE_ACL_LIBACL_H
# include <acl/libacl.h>
#endif

#ifdef HAVE_LIBMONGOC
// WARNING: If changing collection names: grep through source
# define MONGO_HAVE_STDINT
# define MONGO_BASE "cfreport"
# define MONGO_KMBASE "cfknow"
# define MONGO_MPBASE "phpcfengine"
# define MONGO_DATABASE MONGO_BASE ".hosts"
# define MONGO_HOSTS_COLLECTION "hosts"
# define MONGO_MON_MG_COLLECTION "monitoring_mg"
# define MONGO_DATABASE_MON_MG MONGO_BASE ".monitoring_mg"
# define MONGO_MON_WK_COLLECTION "monitoring_wk"
# define MONGO_DATABASE_MON_WK MONGO_BASE ".monitoring_wk"
# define MONGO_MON_YR_COLLECTION "monitoring_yr"
# define MONGO_DATABASE_MON_YR MONGO_BASE ".monitoring_yr"
# define MONGO_PROMISES_EXP MONGO_BASE ".promises_exp"
# define MONGO_PROMISES_UNEXP_COLLECTION "promises_unexp"
# define MONGO_PROMISES_UNEXP MONGO_BASE "." MONGO_PROMISES_UNEXP_COLLECTION
# define MONGO_KM_TOPICS MONGO_KMBASE ".""KT"
# define MONGO_KM_OCCURRENCES MONGO_KMBASE ".""KO"
# define MONGO_KM_INFERENCES MONGO_KMBASE ".""KI"
# define MONGO_BODIES   MONGO_BASE ".bodies"
# define MONGO_SCRATCH MONGO_BASE ".scratch"
# define MONGO_CACHE MONGO_BASE ".cache"
# define MONGO_LOGS_REPAIRED_COLL "logs_rep"
# define MONGO_LOGS_REPAIRED MONGO_BASE ".logs_rep"
# define MONGO_LOGS_NOTKEPT_COLL "logs_nk"
# define MONGO_LOGS_NOTKEPT MONGO_BASE ".logs_nk"
# define MONGO_NOTEBOOK MONGO_BASE ".notebook"
# define MONGO_MP_SETTINGS_COLLECTION MONGO_MPBASE ".appsettings"
# define MONGO_ARCHIVE_COLLECTION "archive"
# define MONGO_ARCHIVE MONGO_BASE ".archive"
# define CF_MONGO_SLAVE_OK 4
# include <mongo.h>
#else
# define mongo_connection char
#endif

#undef PACKAGE

#define CF_SHADES 25
#define CF_TIMESERIESDATA 168   /* (24*7) */
#define CF_MAGDATA 48           /* (4*12) */
#define CF_MAX_SLOTS 2016       /* (24*7*12) */
#define CF_LHISTORYDATA 1464
#define CF_MAG_SLOTS (12*24*7)
#define CF_YEAR_SLOTS (3*52)
#define CF_MAGMARGIN 0
#define CF_LHISTORYMARGIN 0
#define CF_TRIBE_SIZE 25
#define CF_MAX_ERROR_LIMIT 50
#define CF_MAX_LIMIT 999999
#define CF_N_CODE '\07'
#define MONITOR_CLASS_PREFIX "mXC_"
#define CF_CHANGE_HORIZON 10
#define NOVA_EXPORT_HEADER "NOVA_EXPORT"
#define CF_CODEBOOK_SIZE 29
#define NOVA_MAXDIFFSIZE (80 * 1024 * 1024)
#define HOSTKEY_SIZE 100        // length of SHA=....

// for pdf reports
#define CF_NOTICE_TRUNCATED "Data truncated due to internal buffer limit"

/*****************************************************************************/

enum cfl_view
{
    cfl_view_comp,
    cfl_view_virtbundlecomp,
    cfl_view_aggr_repaired,
    cfl_view_aggr_notkept,
    cfl_view_aggr_repairedreason,
    cfl_view_aggr_notkeptreason,
    cfl_view_aggr_filechange,
    cfl_view_aggr_software,
    cfl_view_aggr_classes,
    cfl_view_software,
    cfl_view_log_repair,
    cfl_view_log_notkept,
    cfl_view_hub_status,
    cfl_view_hub_meter,
    cfl_view_value_graph,
    cfl_view_hub_details,
    cfl_view_error
};

/*****************************************************************************/

typedef struct
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
} DataView;

typedef struct
{
    int real_id;
    int tribe_id;
    char *shortname;
    char *fullname;
    char *context;
    int distance_from_centre;
} GraphNode;

#define CF_MIN_RADIUS    15.0   // Reduce this at your peril, could hang gd
#define CF_RADIUS_SCALE  10.0

/*****************************************************************************/

typedef enum
{
    PROMISE_LOG_STATE_REPAIRED,
    PROMISE_LOG_STATE_NOTKEPT
} PromiseLogState;

/*****************************************************************************/

typedef struct FileLine_ FileLine;

struct FileLine_
{
    char *text;
    int counter;
    FileLine *next;
    FileLine *prev;
};

/*****************************************************************************/

typedef struct
{
    long resultsPerPage;
    long pageNum;
    int totalResultCount;
} PageInfo;

/*****************************************************************************/

/* Used to represent contents of var in DBM file -
   scope.name is key */
typedef struct
{
    Event e;
    enum cfdatatype dtype;
    char rtype;
    char rval[CF_MAXVARSIZE];   // as string, \0-terminated
} Variable;

#define VARSTRUCTUSAGE(v) (sizeof(v) - sizeof(v.rval) + strlen(v.rval) + 1)

/*******************************************************************/

#ifdef MINGW

# define EVENTSOURCE_NAME "Cfengine Nova"       // appears in e.g. Event Viewer
# define EVENTSOURCE_HKLM_REGPATH "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\System\\" EVENTSOURCE_NAME
# define EVENT_COUNT 9          // mapped to "TypesSupported" registry log entry
# include "cf.events.h"         // defines events for logging on windows

# define WINSERVICE_NAME "CfengineNovaExec"

#endif

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
# include "cf.nova.web_api.h"
#endif

/*****************************************************************************/

typedef enum basic_reports
{
    cfrep_bundle,
    cfrep_business,
    cfrep_classes,
    cfrep_promise_compliance,
    cfrep_total_compliance,
    cfrep_change,
    cfrep_diff,
    cfrep_lastseen,
    cfrep_patch_avail,
    cfrep_patch_status,
    cfrep_performance,
    cfrep_repaired,
    cfrep_repair_summary,
    cfrep_notkept,
    cfrep_notkept_summary,
    cfrep_setuid,
    cfrep_software_installed,
    cfrep_variables,
    cfrep_unknown
} basic_reports;

typedef struct
{
    const char *id;
    const char *category;
    const char *name;
    const char *name_old;
    const char *description;
} ReportInfo;

extern ReportInfo BASIC_REPORTS[];

/*****************************************************************************/
typedef struct
{
    const char *reportId;
    const char *reportColumnKey;
    const char *dbKey;
} ReportColumnInfo;

/*****************************************************************************/
/* Report DB API Structs                                                     */
/*****************************************************************************/

typedef enum
{
    HOST_RANK_METHOD_COMPLIANCE   = 0x00000001,
    HOST_RANK_METHOD_ANOMALY      = 0x00000010,
    HOST_RANK_METHOD_PERFORMANCE  = 0x00000100,
    HOST_RANK_METHOD_LASTSEEN     = 0x00001000,
    HOST_RANK_METHOD_MIXED        = 0x00010000
} HostRankMethod;

typedef enum
{
    HOST_COLOUR_GREEN,
    HOST_COLOUR_YELLOW,
    HOST_COLOUR_RED,
    HOST_COLOUR_BLUE,
    HOST_COLOUR_GREEN_YELLOW_RED
} HostColour;

typedef struct
{
    HostRankMethod method;
    HostColour colour;
    time_t blue_time_horizon;
} HostColourFilter;

typedef struct
{
    char *keyhash;
    char *hubkey;
    char *ipaddr;
    char *hostname;
} HubHost;

typedef struct
{
    HubHost *hh;
    char *path;
    time_t t;
    char *nid;
    char *handle;
} HubFileChanges;

typedef struct
{
    HubHost *hh;
    char *path;
    char *diff;
    time_t t;
} HubFileDiff;

typedef struct
{
    HubHost *hh;
    char *name;
    char *version;
    char *arch;
    time_t t;
} HubSoftware;

typedef struct
{
    HubHost *hh;
    char *class;
    double prob;
    double dev;
    time_t t;
} HubClass;

typedef struct
{
    HubHost *hh;
    char *class;
    int frequency;              // across all hosts
} HubClassSum;

typedef struct
{
    HubHost *hh;
    time_t t;
    char *version;
    int kept;
    int repaired;
    int notkept;
} HubTotalCompliance;

typedef struct
{
    HubHost *hh;
    char *scope;
    char *lval;
    Rval rval;
    char *dtype;
    time_t t;
} HubVariable;

// promise kept,repaired or not kept
typedef struct
{
    HubHost *hh;
    //   char *policy;
    char *handle;
    char *cause;
    time_t t;
    char *nid;
    char *oid;
} HubPromiseLog;

// promise not kept/repaired summary
typedef struct
{
    HubHost *hh;
    char *policy;
    char *handle;
    char *cause;
    int occurences;
    int hostOccurences;
} HubPromiseSum;

typedef struct
{
    HubHost *hh;
    HubHost *rhost;
    LastSeenDirection direction;
    double hrsago;
    double hrsavg;
    double hrsdev;
    time_t t;
} HubLastSeen;

typedef struct
{
    HubHost *hh;
    char *bundle;
    double bundlecomp;
    double bundleavg;
    double bundledev;
    time_t t;
    char *nid;
} HubBundleSeen;

typedef struct
{
    HubHost *hh;
    char *day;                  /* 'DD Month YYYY' */
    double kept;
    double repaired;
    double notkept;
    char *nid;
    char *handle;
} HubValue;

typedef struct
{
    HubHost *hh;
    char type;
    double kept;
    double repaired;
    double notkept;
} HubMeter;

#define cfmeter_hour 'H'
#define cfmeter_week 'W'
#define cfmeter_day  'D'
#define cfmeter_perf 'P'
#define cfmeter_comms 'C'
#define cfmeter_anomaly 'A'
#define cfmeter_other 'S'

typedef struct
{
    HubHost *hh;
    char *event;
    double q;
    double e;
    double d;
    time_t t;
    char *nid;
    char *handle;
} HubPerformance;

typedef struct
{
    HubHost *hh;
    char *path;
} HubSetUid;

typedef struct
{
    HubHost *hh;
    char *handle;
    PromiseState status;
    double e;
    double d;
    time_t t;
} HubPromiseCompliance;

#ifdef HAVE_LIBMONGOC
typedef struct
{
    Rlist *hosts;
    Rlist *records;
    cfapi_errid errid;
} HubQuery;
#endif

typedef struct
{
    char *bundleName;
    char *bundleType;
    Rlist *bundleArgs;
    char *promiseType;
    char *promiser;
    char *promisee;
    char *classContext;
    char *handle;
    char *comment;
    char *file;
    int lineNo;
    Rlist *constraints;
    double popularity;          // optional
} HubPromise;

typedef struct
{
    char *bundleName;
    char *bundleType;
    Rlist *bundleArgs;
} HubPromiseBundle;

typedef struct HubBodyAttr_ HubBodyAttr;

typedef struct
{
    char *bodyName;
    char *bodyType;
    char *bodyArgs;             // comma separated
    HubBodyAttr *attr;
} HubBody;

struct HubBodyAttr_
{
    char *classContext;
    char *lval;
    char *rval;
    HubBodyAttr *next;
};

/* cfreport.cache */

typedef struct
{
    char *policy;
    int slot;
    int hostCount;              // how many hosts matched
    int totalHostCount;         // optional, how many hosts could possibly match
    double kept;
    double repaired;
    double notkept;
    time_t genTime;
} HubCacheTotalCompliance;

typedef struct HubNote_ HubNote;

/*
 * Commenting on reports
 */
struct HubNote_
{
    char *user;
    char *msg;
    time_t t;
    HubNote *next;
};

typedef struct
{
    HubHost *hh;
    char *nid;
    char *user;
    char *msg;
    time_t t;
    char *report;
    int reportType;
    HubNote *note;
} HubNoteInfo;

typedef struct HubVital_ HubVital;

struct HubVital_
{
    char *id;
    char *units;
    char *description;
    HubVital *next;
};

typedef struct
{
    char *userName;
    char *classRxInclude;
    char *classRxExclude;
    char *bundleRxInclude;
    char *bundleRxExclude;
} HubUserRBAC;

typedef struct
{
    char *name;
    char *description;
    char *classRxInclude;
    char *classRxExclude;
    char *bundleRxInclude;
    char *bundleRxExclude;
} HubRole;

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
# define CF_GENERIC_READ_NT (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL)
# define CF_GENERIC_WRITE_NT (FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
# define CF_GENERIC_EXECUTE_NT (FILE_EXECUTE)
# define CF_MINIMUM_PERMS_NT (SYNCHRONIZE)      // access is always denied if synchronize is not set
#endif /* MINGW */

/************************************************************************************/
/* Prototypes                                                                       */
/************************************************************************************/

/* acl.c */

void Nova_SetACLDefaults(char *path, Acl *acl);
int Nova_CheckACESyntax(char *ace, char *valid_nperms, char *valid_ops, int deny_support, int mask_support,
                        Promise *pp);
int Nova_CheckModeSyntax(char **mode_p, char *valid_nperms, char *valid_ops, Promise *pp);
int Nova_CheckPermTypeSyntax(char *permt, int deny_support, Promise *pp);
int Nova_CheckDirectoryInherit(char *path, Acl *acl, Promise *pp);

/* acl_linux.c */

int Nova_CheckPosixLinuxACL(char *file_path, Acl acl, Attributes a, Promise *pp);

#ifdef HAVE_LIBACL
int Nova_CheckPosixLinuxAccessACEs(Rlist *aces, enum cf_acl_method method, char *file_path, Attributes a, Promise *pp);
int Nova_CheckPosixLinuxInheritACEs(Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit,
                                    char *file_path, Attributes a, Promise *pp);
int Nova_CheckPosixLinuxACEs(Rlist *aces, enum cf_acl_method method, char *file_path, acl_type_t acl_type, Attributes a,
                             Promise *pp);
int Nova_CheckDefaultEqualsAccessACL(char *file_path, Attributes a, Promise *pp);
int Nova_CheckDefaultClearACL(char *file_path, Attributes a, Promise *pp);
int Nova_ParseEntityPosixLinux(char **str, acl_entry_t ace, int *is_mask);
int Nova_ParseModePosixLinux(char *mode, acl_permset_t old_perms);
acl_entry_t Nova_FindACE(acl_t acl, acl_entry_t ace_find);
int Nova_ACLEquals(acl_t first, acl_t second);
int Nova_ACECount(acl_t acl);
int Nova_PermsetEquals(acl_permset_t first, acl_permset_t second);
#endif

/* acl_nt.c */

int Nova_CheckNtACL(char *file_path, Acl acl, Attributes a, Promise *pp);

#ifdef MINGW
int Nova_CheckNtACEs(char *file_path, Rlist *aces, inherit_t inherit, enum cf_acl_method method, Attributes a,
                     Promise *pp);
int Nova_CheckNtInheritACEs(char *file_path, Rlist *aces, enum cf_acl_method method,
                            enum cf_acl_inherit directory_inherit, Attributes a, Promise *pp);
int Nova_CheckNtDefaultClearACL(char *file_path, Attributes a, Promise *pp);
int Nova_CheckNtDefaultEqualsAccessACL(char *file_path, Attributes a, Promise *pp);
void Nova_RemoveEasByInheritance(EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
void Nova_RemoveEmptyEas(EXPLICIT_ACCESS *eas, int *eaCount);
int Nova_ACLEquals(int *aclsEqual, EXPLICIT_ACCESS *firstEas, int eaCount, ACL *acl);
int Nova_AclToExplicitAccess(EXPLICIT_ACCESS *eas, int eaCount, ACL *acl);
int Nova_ParseAcl(char *file_path, Rlist *aces, EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
int Nova_SetEas(char *file_path, EXPLICIT_ACCESS *eas, int eaCount);
EXPLICIT_ACCESS *Nova_FindAceNt(EXPLICIT_ACCESS *eas, EXPLICIT_ACCESS *endEa, SID *findSid, ACCESS_MODE findPermt,
                                inherit_t findInherit);
void Nova_FreeSids(EXPLICIT_ACCESS *eas, int num);
int Nova_EntityToSid(char **acePtr, SID *sid, DWORD sidSz);
int Nova_ParseModeNt(char **modePtr, ACCESS_MASK *perms);
ACCESS_MODE Nova_ParsePermTypeNt(char *ace);
#endif /* MINGW */

/* cfnova.c */

void Nova_Initialize(void);
void Nova_SaveDocumentRoot(void);

#if defined(__MINGW32__)
bool BootstrapAllowed(void);
#endif

/* client_code.c */
#ifdef HAVE_LIBMONGOC
int Nova_QueryClientForReports(mongo_connection *dbconn, AgentConnection *conn, const char *menu, time_t since);
void UnpackReportBook(mongo_connection *dbconn, char *id, Item **reports);
#endif

int Nova_StoreIncomingReports(char *reply, Item **reports, int current_report);
void NewReportBook(Item **reports);
void DeleteReportBook(Item **reports);

/* cmd_api.c */

#include "cf.nova.cmd_api.h"

/* conversion.c */

enum cfl_view Str2View(const char *s);
const char *View2Str(enum cfl_view view);

#ifdef HAVE_LIBMONGOC
int Nova_GetReportedScalar(char *hostkey, char *scope, char *lval, char *returnval, int bufsize);
int Nova_GetReportedList(char *hostkey, char *scope, char *lval, Rlist **list);
#endif

/* copernicus.c */

void Nova_PrimeGraph(Rlist **semantic);
void Nova_DrawTribe_PNG(char *filename, int *tribe_id, GraphNode *tribe_node,
                        double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int tribe_size, double *tribe_evc, int topic,
                        char *buffer, int bufsize);
void Nova_DrawTribe(int *tribe_id, GraphNode *tribe_node, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],
                    int tribe_size, double *tribe_evc, int topic, char *buffer, int bufsize);
int Nova_GetMaxEvcNode(double *evc, int tribe_size);
int Nova_GetAdjacent(int i, double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int size, GraphNode *tribe,
                     GraphNode *neighbours);
int Nova_SplayAdjacent(int i, double adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int tribe_size, GraphNode *tribe, int *trail,
                       GraphNode *neighbours);
void Nova_CopyNeighbours2(GraphNode *from, int tribe_size, GraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int index1);
void Nova_CopyNeighbours3(GraphNode *from, int tribe_size, GraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],
                          int index1, int index2);
int Nova_GetEvcTops(double **adj, int size, double *evc, int *tops);
void Nova_CentreScale(DataView *cfv, double min_x, double max_x, double min_y, double max_y);
int Nova_X(DataView cfv, double x);
int Nova_Y(DataView cfv, double y);
int Nova_TribeUnion(int *array1, GraphNode *array2, int size1, int size2);
void Nova_ClearTrail(int *array);
void Nova_AnchorTrail(int *array, int node);
int Nova_InTrail(int *trail, int node);
void Nova_Line(DataView cfv, double x1, double y1, double x2, double y2, int colour);
void Nova_Disc(DataView cfv, double x1, double y1, double radius, int colour);
void Nova_Print(DataView cfv, double x, double y, char *s, int colour);
void Nova_BigPrint(DataView cfv, double x, double y, char *s, int colour);
double Nova_SignPerturbation(int i);
void Nova_ColdBall(DataView cfv, double x, double y, double radius, int *shade);
void Nova_HotBall(DataView cfv, double x, double y, double radius, int *shade);
void Nova_AlignmentCorrection(double *x, double *y, double cx, double cy);
void Nova_MapHorizon(DataView cfv, double x, double y, double *min_x, double *min_y, double *max_x, double *max_y);
double Nova_Orbit(DataView cfv, double radius, double min_x, double max_x, double min_y, double max_y);
void Nova_MapBall(DataView cfv, GraphNode n, char *buffer, int bufsize);
void Nova_Annealing(GraphNode neighbours1[CF_TRIBE_SIZE], GraphNode neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE],
                    GraphNode neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE], int size1,
                    int size2[CF_TRIBE_SIZE], int size3[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int tribe_size);
int Overlap(double x1, double y1, double x2, double y2);
int Nova_InRange(DataView cfv, int x, int y);
void Nova_BoundaryCheck(DataView *cfv, int *x1, int *y1, int *x2, int *y2);

#ifdef HAVE_LIBMONGOC

# include "db_query.h"

Item *CFDB_QueryVitalIds(mongo_connection *conn, char *keyHash);
HubVital *CFDB_QueryVitalsMeta(mongo_connection *conn, char *keyHash);
int CFDB_QueryMagView2(mongo_connection *conn, char *keyhash, char *monId, time_t start_time, double *qa, double *ea,
                       double *da);
int CFDB_QueryMonView(mongo_connection *conn, char *keyhash, char *monId, enum monitord_rep rep_type, double *qa,
                      double *ea, double *da);
int CFDB_QueryWeekView(mongo_connection *conn, char *keyhash, enum observables obs, double *qa, double *ea, double *da);
bool CFDB_QueryHistogram(mongo_connection *conn, char *keyhash, char *monId, double *histo);
int CFDB_QueryLastUpdate(mongo_connection *conn, char *db, char *dbkey, char *keyhash, time_t *date);

int CFDB_QueryPromiseAttr(mongo_connection *conn, char *handle, char *attrKey, char *attrVal, int attrValSz);
Item *CFDB_QueryExpandedPromiseAttr(mongo_connection *conn, char *handle, char *attrKey);
HubQuery *CFDB_QueryPromiseHandles(mongo_connection *conn, char *promiser, char *promiserType, char *bType, char *bName,
                                   int regex, bool filter);
HubQuery *CFDB_QueryHandlesForBundlesWithComments(mongo_connection *conn, char *bType, char *bName);
HubQuery *CFDB_QueryPromises(mongo_connection *conn, PromiseFilter *filter);
HubQuery *CFDB_QueryPromiseBundles(mongo_connection *conn, PromiseFilter *filter);
int CFDB_QueryBundleCount(mongo_connection *conn);
int CFDB_QueryPromiseCount(mongo_connection *conn);
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

void CFDB_ListEverything(mongo_connection *conn);
void CFDB_ScanHubHost(bson_iterator *it, char *keyhash, char *ipaddr, char *hostnames);
int QueryHostsWithClass(mongo_connection *conn, bson_buffer *bb, char *classRegex);
int QueryInsertHostInfo(mongo_connection *conn, Rlist *host_list);
void PrintCFDBKey(bson_iterator *it, int depth);
int CFDB_IteratorNext(bson_iterator *it, bson_type valType);
int Nova_MagViewOffset(int start_slot, int dbslot, int wrap);
int CFDB_CountHostsGeneric(mongo_connection *conn, bson *query);
int CFDB_QueryHostName(mongo_connection *conn, char *ipAddr, char *hostName, int hostNameSz);
bool MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra, bool *checkUpdate);

//replica set
Item *CFDB_GetLastseenCache(void);
int CFDB_QueryIsMaster(void);
int CFDB_QueryMasterIP(char *buffer, int bufsize);
int CFDB_QueryReplStatus(mongo_connection *conn, char *buffer, int bufsize);
Item *CFDB_GetDeletedHosts(void);
#endif

/* db_save.c */

#ifdef HAVE_LIBMONGOC

int CFDB_Open(mongo_connection *conn);
int CFDB_Close(mongo_connection *conn);
void CFDB_Initialize(void);

int CFDB_PutValue(char *lval, char *rval, char *db_name);

void CFDB_SaveSoftware(mongo_connection *conn, enum software_rep sw, char *kH, Item *data);

//void CFDB_SaveMonitorData(mongo_connection *conn, char *kH, enum monitord_rep rep_type, Item *data);
void CFDB_SaveMonitorData2(mongo_connection *conn, char *keyHash, enum monitord_rep rep_type, Item *data);
void CFDB_SaveMonitorHistograms(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveClasses(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveVariables(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveVariables2(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveTotalCompliance(mongo_connection *conn, char *kH, Item *data);
void CFDB_SavePromiseLog(mongo_connection *conn, char *kH, PromiseLogState state, Item *data);
void CFDB_SaveLastSeen(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveMeter(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveScore(mongo_connection *conn, char *kH, Item *data, HostRankMethod method);
void CFDB_SaveSoftwareDates(mongo_connection *conn, char *kH, Item *data);
void CFDB_SavePerformance(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveSetUid(mongo_connection *conn, char *kH, Item *data);
void CFDB_SavePromiseCompliance(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveFileChanges(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveFileDiffs(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveBundles(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveValueReport(mongo_connection *conn, char *kH, Item *data);
void CFDB_SaveHostID(mongo_connection *conn, char *database, char *keyField, char *keyhash, char *ipaddr,
                     char *hostname);
void Nova_CheckGlobalKnowledgeClass(char *name, char *key);
bool GetBsonBool(char *data, char *boolKey, bool *val);
void CFDB_SaveLastHostUpdate(mongo_connection *conn, char *hostKey);
void CFDB_SaveLastUpdate(mongo_connection *conn, char *database, char *keyField, char *keyhash);

HubQuery *CFDB_QueryCachedTotalCompliance(mongo_connection *conn, char *policy, time_t minGenTime);
void CFDB_SaveCachedTotalCompliance(mongo_connection *conn, char *policy, int slot, double kept, double repaired,
                                    double notkept, int count, time_t genTime);
int CFDB_SaveLastseenCache(Item *lastseen);
void CFDB_SaveGoalsCache(char *goal_patterns, char *goal_categories);
int CFDB_MarkAsDeleted(mongo_connection *dbconn, char *keyHash);

/*
 * commenting
 */
int CFDB_AddNote(mongo_connection *conn, char *keyhash, int reportType, char *nid, char *reportData, Item *data);
Rlist *CFDB_QueryNotes(mongo_connection *conn, char *keyhash, char *nid, Item *data);
Rlist *CFDB_QueryNoteId(mongo_connection *conn, bson *query);
void CFDBRef_AddToRow(mongo_connection *conn, char *coll, bson *query, char *row_name, char *cid);
int CFDB_GetRow(mongo_connection *conn, char *db, int reportType, bson *query, char *rowname, char *row, int rowSz,
                int level);
Item *CFDB_QueryDistinctStr(mongo_connection *conn, char *database, char *collection, char *dKey, char *qKey,
                            char *qVal);
Item *CFDB_QueryDistinct(mongo_connection *conn, char *database, char *collection, char *dKey, bson *queryBson);
void BsonIteratorToString(char *retBuf, int retBufSz, bson_iterator *i, int depth, int reportType);
void GetReportKeyMapping(int reportType, char *key, char *retBuf, int retBufSz);
#endif /* HAVE_LIBMONGOC */

/* db_maintain.c */

void CFDB_Maintenance(void);
void CFDB_ReIndexAll(void);
void CFDB_ConnectAndEnsureIndices(void);

#ifdef HAVE_LIBMONGOC
void CFDB_EnsureIndices(mongo_connection *conn);
void CFDB_PurgeDropReports(mongo_connection *conn);
void CFDB_PurgeTimestampedReports(mongo_connection *conn);
void CFDB_PurgeTimestampedLongtermReports(mongo_connection *conn);
void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now);
void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                    Item **purgeKeysPtr, Item **purgeNamesPtr);
int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, Item **purgeKeysPtr,
                  Item **purgeNamesPtr);
void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                           Item **purgeKeysPtr);
void DeleteFromBsonArray(bson_buffer *bb, char *arrName, Item *elements);
void CFDB_PurgeHost(mongo_connection *conn, char *keyHash);
void CFDB_PurgeDeprecatedVitals(mongo_connection *conn);

void CFDB_RemoveTestData(char *db, char *keyhash);
int CFDB_PurgeDeletedHosts(void);
#endif /* HAVE_LIBMONGOC */

/* datapackaging.c */

void Nova_PackPerformance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackClasses(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSetuid(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackFileChanges(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackDiffs(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackMonitorMg(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackMonitorWk(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackMonitorYr(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_FormatMonitoringReply(Item **datap, Item **reply, enum cfd_menu type);
void Nova_PackMonitorHist(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackCompliance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSoftware(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackAvailPatches(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackPatchStatus(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_Pack_promise_output_common(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackValueReport(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackVariables(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackVariables2(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackLastSeen(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackTotalCompliance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackRepairLog(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackNotKeptLog(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackMeter(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSoftwareDates(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackBundles(Item **reply, char *header, time_t date, enum cfd_menu type);
int Nova_CoarseLaterThan(char *key, char *from);
int Nova_YearSlot(char *day, char *month, char *lifecycle);
int Nova_LaterThan(char *bigger, char *smaller);
char *Nova_ShortArch(char *arch);

/* dataunpack.c */

void Nova_UnPackPerformance(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackClasses(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackSetuid(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackFileChanges(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackDiffs(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorWeek(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorMag(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorHist(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorYear(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorMg(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorWk(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorYr(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMonitorHg(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackCompliance(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackSoftware(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackAvailPatches(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackPatchStatus(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPack_promise_output_common(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackValueReport(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackVariables(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackVariables2(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackLastSeen(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackTotalCompliance(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackRepairLog(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackNotKeptLog(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackMeter(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackSoftwareDates(mongo_connection *dbconn, char *id, Item *data);
void Nova_UnPackBundles(mongo_connection *dbconn, char *id, Item *data);
char *Nova_LongArch(char *arch);

/* histogram.c */

int Nova_ReadHistogram2(mongo_connection *conn, DataView *cfv, char *hostkey, char *monId);
Item *Nova_MapHistogram(DataView *cfv, char *keyhash);
void Nova_AnalyseHistogram(char *keyhash, enum observables obs, char *buffer, int bufsize);

/* html.c */
void Nova_Footer(char *buffer, int bufsize);
void Nova_Header(char *title, char *device_type, char *buffer, int bufsize);
void Nova_TabMenu(char *s, char *buffer, int bufsize);
void Nova_IncludeFile(char *name, char *buffer, int bufsize);

/* hub.c */

Item *Nova_ScanClients(void);
void Nova_CountMonitoredClasses(void);
void Nova_CacheTotalCompliance(bool allSlots);
void Nova_CacheTotalComplianceEnv(mongo_connection *conn, char *envName, char *envClass, int slot, time_t start,
                                  time_t now);
int Nova_ShiftChange(void);
void Nova_UpdateMongoHostList(Item **list);

/* install.c */

void PrependPromiserList(PromiseIdent **list, char *s, Promise *pp);

#ifdef HAVE_LIBMONGOC
HubQuery *NewHubQuery(Rlist *hosts, Rlist *records);
HubQuery *NewHubQueryErrid(Rlist *hosts, Rlist *records, cfapi_errid errid);
void DeleteHubQuery(HubQuery *hq, void (*fnptr) ());
int CountRecords(HubQuery *hq);
void *HubQueryGetFirstRecord(HubQuery *hq);
#endif
HubHost *NewHubHost(const char *hubkey, const char *keyhash, const char *ipaddr, const char *hostname);
HubHost *CreateEmptyHubHost(void);
HubHost *UpdateHubHost(HubHost *hubHost, char *keyhash, char *ipaddr, char *hostname);
HubHost *GetHubHostIn(Rlist *host_list, char *keyhash);
bool HubHostEqual(const void *hubhost_a, const void *hubhost_b);
unsigned int HubHostHash(const void *hubhost);
void DeleteHubHost(HubHost *hp);
HubSoftware *NewHubSoftware(HubHost *hh, char *name, char *version, char *arch, time_t timeseen);
void DeleteHubSoftware(HubSoftware *hs);
HubClass *NewHubClass(HubHost *hh, char *class, double p, double dev, time_t t);
void DeleteHubClass(HubClass *hc);
HubClassSum *NewHubClassSum(HubHost *hh, char *class, int frequency);
void DeleteHubClassSum(HubClassSum *hc);
HubTotalCompliance *NewHubTotalCompliance(HubHost *hh, time_t t, char *v, int k, int r, int n);
void DeleteHubTotalCompliance(HubTotalCompliance *ht);
HubVariable *NewHubVariable(HubHost *hh, char *type, char *scope, char *lval, Rval rval, time_t t);
void DeleteHubVariable(HubVariable *hv);
HubPromiseLog *NewHubPromiseLog(HubHost *hh, char *handle, char *cause, time_t t, char *noteId, char *oid);
void DeleteHubPromiseLog(HubPromiseLog *hp);
HubPromiseSum *NewHubPromiseSum(HubHost *hh, char *handle, char *cause, int occurences, int hostOccurences);
void DeleteHubPromiseSum(HubPromiseSum *hs);
HubLastSeen *NewHubLastSeen(HubHost *hh, LastSeenDirection direction, char *kh, char *rhost, char *ip, double ago,
                            double avg, double dev, time_t t);
void DeleteHubLastSeen(HubLastSeen *hp);
HubMeter *NewHubMeter(HubHost *hh, char type, double kept, double repaired);
void DeleteHubMeter(HubMeter *hp);
HubPerformance *NewHubPerformance(HubHost *hh, char *event, time_t t, double q, double e, double d, char *noteid,
                                  char *handle);
void DeleteHubPerformance(HubPerformance *hp);
HubSetUid *NewHubSetUid(HubHost *hh, char *file);
void DeleteHubSetUid(HubSetUid *hp);
HubPromiseCompliance *NewHubCompliance(HubHost *hh, char *handle, PromiseState status, double e, double d, time_t t);
void DeleteHubPromiseCompliance(HubPromiseCompliance *hp);
HubBundleSeen *NewHubBundleSeen(HubHost *hh, char *rname, double ago, double avg, double dev, time_t t, char *noteid);
void DeleteHubBundleSeen(HubBundleSeen *hp);
HubFileChanges *NewHubFileChanges(HubHost *hh, char *file, time_t t, char *noteid, char *handle);
void DeleteHubFileChanges(HubFileChanges *hp);
HubFileDiff *NewHubFileDiff(HubHost *hh, char *file, char *diff, time_t t);
void DeleteHubFileDiff(HubFileDiff *hp);
HubValue *NewHubValue(HubHost *hh, char *day, double kept, double repaired, double notkept, char *noteid, char *handle);
void DeleteHubValue(HubValue *hp);
HubPromise *NewHubPromise(char *bn, char *bt, Rlist *ba, char *pt, char *pr, char *pe, char *cl, char *ha, char *co,
                          char *fn, int lno, Rlist *cons);
HubPromiseBundle *NewHubPromiseBundle(char *bundleName, char *bundleType, Rlist *bundleArgs);
void DeleteHubPromiseBundle(HubPromiseBundle * hb);
void DeleteHubPromise(HubPromise * hp);
HubBody *NewHubBody(char *bodyName, char *bodyType, char *bodyArgs);
void DeleteHubBody(HubBody * hb);
HubBodyAttr *NewHubBodyAttr(HubBody * hb, char *lval, char *rval, char *classContext);
void DeleteHubBodyAttributes(HubBodyAttr *ha);
HubNote *NewHubNote(char *user, char *msg, time_t t);
HubNoteInfo *NewHubNoteInfo(HubHost *hh, char *nid, char *user, char *msg, time_t t, char *reportData, int reportType);
void DeleteHubNote(HubNote *hc);
void DeleteHubNoteInfo(HubNoteInfo *hci);
HubVital *PrependHubVital(HubVital **first, char *id, char *units, char *description);
void DeleteHubVital(HubVital *hv);
HubUserRBAC *NewHubUserRBAC(char *userName, char *classRxInclude, char *classRxExclude, char *bundleRxInclude,
                            char *bundleRxExclude);
void DeleteHubUserRBAC(HubUserRBAC *userRbac);
HubRole *NewHubRole(char *name, char *description,
                    char *classRxInclude, char *classRxExclude, char *bundleRxInclude, char *bundleRxExclude);
void DeleteHubRole(HubRole *role);

#ifdef HAVE_LIBMONGOC
HostClassFilter *NewHostClassFilter(const char *classRxInclude, const char *classRxExclude);
HostClassFilter *NewHostClassFilterLists(Rlist *classRxInclude, Rlist *classRxExclude);
void HostClassFilterAddClasses(HostClassFilter *filter, const char *classRxInclude, const char *classRxExclude);
void HostClassFilterAddClassLists(HostClassFilter *filter, const Rlist *classRxIncludes, const Rlist *classRxExcludes);
bool HostClassFilterMatch(const HostClassFilter *filter, const char *classRx);
void DeleteHostClassFilter(HostClassFilter *filter);
PromiseFilter *NewPromiseFilter(void);
void PromiseFilterAddPromiseBody(PromiseFilter *filter, const char *handleInclude, const char *promiserInclude);
void PromiseFilterAddPromiseBodyRx(PromiseFilter *filter, const char *handleRxInclude, const char *promiserRxInclude);
void PromiseFilterAddPromiseType(PromiseFilter *filter, const char *promiseTypeInclude);
void PromiseFilterAddBundleType(PromiseFilter *filter, const char *bundleTypeInclude);
void PromiseFilterAddBundles(PromiseFilter *filter, const char *bundleInclude, const char *bundleExclude);
void PromiseFilterAddBundlesRx(PromiseFilter *filter, const char *bundleRxInclude, const char *bundleRxExclude);
void DeletePromiseFilter(PromiseFilter *filter);
HostColourFilter *NewHostColourFilter(HostRankMethod method, HostColour colours);
#endif
HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount,
                                                    double kept, double repaired, double notkept, time_t genTime);
void DeleteHubCacheTotalCompliance(HubCacheTotalCompliance *tc);

#ifdef HAVE_LIBMONGOC
void HubQuerySortPromiseBundles(HubQuery *hqPromiseBundles);
#endif
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
int SortPromiseBundle(void *p1, void *p2);
HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(Rlist *records, int slot);

int PageRecords(Rlist **records_p, PageInfo *page, void (*fnptr) ());
void CountMarginRecordsVars(Rlist **records_p, PageInfo *page, int *start_count, int *end_count);

/* knowledge.c */

char *Name2Id(char *s);
void Nova_StoreKMDB(Topic **topichash, Occurrence *occurrences, Inference *inferences);
void Nova_ListAgents(void);
void Nova_ListFunctions(void);
void Nova_ListFunction(const FnCallType *f, int full);
void Nova_ListPromiseTypes(void);

void Nova_MapPromiseToTopic(FILE *fp, Promise *pp, const char *version);
void Nova_BundleReference(FILE *fp, char *bundle);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_PromiseNode(FILE *fp, Promise *pp, int calltype);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_BundleNode(FILE *fp, char *bundle);
void Nova_BodyNode(FILE *fp, char *body, int calltype);
void Nova_DependencyGraph(Topic *map);
void Nova_PlotTopicDependencies(int topic, double **adj, char **names, int dim);
void Nova_MapClassParameterAssociations(FILE *fp, Promise *pp, char *promise_id);
double NovaShiftAverage(double new, double old);
double NovaExtractValueFromStream(char *handle, Item *stream, Attributes a, Promise *pp);
void NovaLogSymbolicValue(char *handle, Item *stream, Attributes a, Promise *pp);
void Nova_ShowBundleDependence(FILE *fp);
char *NovaEscape(const char *s);        /* Thread-unsafe */
void NovaShowValues(FILE *fp, BodySyntax bs);
void Nova_RegisterImg(Item **list, char *dir, char *pic);
void Nova_RegisterDoc(Item **list, char *dir, char *doc);

// generating test data
void Nova_GenerateTestData(int count);
void Nova_RemoveTestData(void);
void Nova_UpdateTestData(void);
Rlist *Nova_GetTestMachines(void);
char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE + 1]);
void ThisHashString(char *str, char *buffer, int len, unsigned char digest[EVP_MAX_MD_SIZE + 1]);

/* license.c */

int Nova_HashKey(char *filename, char *buffer, unsigned char digest[EVP_MAX_MD_SIZE + 1], char *hash);
int Nova_EnterpriseModuleExpiry(char *day, char *month, char *year);
void Nova_LogLicenseStatus(void);
int Nova_CheckLicenseWin(char *pos);
char *Nova_LicenseOwner(void);

/* logging.c */

#include "logging.h"

/* magnify.c */

bool Nova_ReadMagTimeSeries2(mongo_connection *conn, DataView *cfv, char *hostkey, char *vitalId);

/* monitoring.c */

void Nova_HistoryUpdate(time_t time, const Averages *newvals);
void Nova_UpdateShiftAverage(Averages *shift_value, Averages *newvals);
void Nova_ResetShiftAverage(Averages *shift_value);
double ShiftAverage(double new, double old);
int NovaRegisterSlot(const char *name, const char *description, const char *units,
                     double expected_minimum, double expected_maximum, bool consolidable);
bool NovaHasSlot(int idx);
const char *NovaGetSlotName(int idx);
const char *NovaGetSlotDescription(int index);
const char *NovaGetSlotUnits(int index);
double NovaGetSlotExpectedMinimum(int index);
double NovaGetSlotExpectedMaximum(int index);
bool NovaIsSlotConsolidable(int index);
Item *NovaGetMeasurementStream(Attributes a, Promise *pp);
Item *NovaReSample(int slot, Attributes a, Promise *pp);
void NovaNamedEvent(char *eventname, double value, Attributes a, Promise *pp);
void Nova_DumpSlowlyVaryingObservations(void);
void Nova_MonOtherInit(void);
void Nova_MonOtherGatherData(double *cf_this);
void Nova_SaveFilePosition(char *filename, long fileptr);
long Nova_RestoreFilePosition(char *filename);
void Nova_LookupAggregateClassName(int n, char *name, char *desc);

/* - date-related functions - */

time_t WeekBegin(time_t time);
time_t SubtractWeeks(time_t time, int weeks);
time_t NextShift(time_t time);
bool GetRecordForTime(CF_DB *db, time_t time, Averages *result);
void PutRecordForTime(CF_DB *db, time_t time, const Averages *values);
double BoundedValue(double value, double defval);

/* mon_cumulative.c */

unsigned GetInstantUint32Value(const char *name, const char *subname, unsigned value, time_t timestamp);
unsigned long long GetInstantUint64Value(const char *name, const char *subname, unsigned long long value,
                                         time_t timestamp);

/* processes.c */

void Nova_DoFileDiff(char *file, char *destination, struct stat sb, struct stat dsb);
int Nova_GetFirstChangePosition(char *file, char *destination);
int Nova_FileIsBinary(char *name, int size, int maxsize);
void Nova_ReportFileChange(FILE *fp, char *file, char *destination, int maxsize);
int Nova_LoadFileHunks(char *file, char *destination, FileLine **list1, FileLine **list2, int *l1, int *l2,
                       int maxsize);
FileLine *AppendFileLine(FileLine **liststart, char *item, int pos);
void DeleteAllFileLines(FileLine *item);
void DeleteFileLine(FileLine **liststart, FileLine *item);

/* promise_db.c */

void Nova_StoreExpandedPromise(Promise *pp);
void Nova_StoreUnExpandedPromises(Bundle *bundles, Body *bodies);
void Nova_StoreBody(mongo_connection *dbconn, Body *body);

/* promises.c */

const char *Nova_Version(void);
const char *Nova_NameVersion(void);

void Nova_EnterpriseDiscovery(void);
int Nova_ClassesIntersect(Rlist *contexts1, Rlist *contexts2);
void Nova_DefineHubMaster(void);

/* pscalar.c */

int Nova_GetPersistentScalar(char *lval, char *rval, int size, time_t timeout);
void Nova_SetPersistentScalar(char *lval, char *rval);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s, Attributes a, Promise *pp);

int CheckRegistrySanity(Attributes a, Promise *pp);

/* reporting.c */

void Nova_SummarizePromiseRepaired(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot,
                                   char *web);
void Nova_NoteVarUsage(void);
void Nova_SummarizeComms(void);
void Nova_SummarizeLicense(char *stylesheet, char *banner, char *footer, char *webdriver);
void Nova_ZenossSummary(char *docroot);
void Nova_NoteVarUsageDB(void);

void Nova_OpenCompilationReportFiles(const char *fname);
void Nova_ShowPromises(Bundle *bundles, Body *bodies);
void Nova_ShowPromise(const char *version, Promise *pp, int indent);
int Nova_ExportReports(char *reportName);
void Nova_CommandAPI(char *lsdata, char *name, char *handle, char *hostkey, char *classregex);

#ifdef HAVE_LIBMONGOC
void Nova_ImportReports(const char *input_file);

int Nova_ImportHostReports(mongo_connection *dbconnp, const char *filePath);
int Nova_ImportHostReportsFromStream(mongo_connection *dbconn, char *header, FILE *fin);
#endif

/* reporting.c */

void Nova_ImportReports(const char *input_file);

/* runagent.c */

bool Nova_ExecuteRunagent(AgentConnection *conn, const char *menu);

#include "scorecards.h"
char *HostRankMethodToMongoCode(HostRankMethod method);

/* server.c */

void Nova_PackAllReports(Item **reply, time_t from, time_t delta1, enum cfd_menu type);
int Nova_ParseHostname(char *name, char *hostname);

pid_t Nova_StartTwin(int argc, char **argv);
void Nova_SignalTwin(void);
void Nova_SignalOther(void);
void Nova_ReviveOther(int argc, char **argv);

/* syntax.c */

void SyntaxTree2JavaScript(void);
void Nova_ShowBundleTypes(void);
void Nova_ShowControlBodies(void);
void Nova_ShowPromiseTypesFor(char *s, SubTypeSyntax *commonMerge);
void Nova_ShowBodyParts(const BodySyntax *bs, int i);
void Nova_ShowRange(char *s, enum cfdatatype type);
void Nova_ShowBuiltinFunctions(void);
void Nova_Indent(int i);

/* topicmap.c */

void Nova_DumpTopics(void);
void Nova_FillInGoalComment(Item *ip);
char *Nova_GetBundleComment(char *bundle);

#ifdef HAVE_LIBMONGOC
void Nova_WebTopicMap_Initialize(void);
#endif
void Nova_LookupUniqueAssoc(int pid, char *buffer, int bufsize);
void Nova_ScanTheRest(int pid, char *buffer, int bufsize);
int Nova_SearchTopicMap(char *typed_topic, char *buffer, int bufsize);
Item *Nova_ScanLeadsAssociations(int pid, char *view);
void Nova_ScanOccurrences(int this_id, char *buffer, int bufsize);
int Nova_GetReportDescription(int this_id, char *buffer, int bufsize);

int Nova_GetTopicByTopicId(int pid, char *topic_name, char *topic_id, char *topic_type);
int Nova_AddTopicSearchBuffer(int pid, char *topic_name, char *topic_type, char *buffer, int bufsize);
int Nova_AddAssocSearchBuffer(char *from_assoc, char *to_assoc, char *buffer, int bufsize);
void Nova_AddOccurrenceBuffer(char *context, char *locator, enum representations locator_type, char *represents,
                              char *buffer, int bufsize);
void AtomizeTopicContext(AlphaList *context_list, char *topic_context);
char *Nova_TopicIdURL(int pid, char *s);
char *Nova_AssocURL(char *s);
char *Nova_URL(char *s, char *rep);
Item *Nova_NearestNeighbours(int topic_id, char *assoc_mask);
Item *Nova_GetTopicsInContext(char *context);
Item *Nova_GetBusinessGoals(char *handle);
int Nova_GetUniqueBusinessGoals(char *buffer, int bufsize);

void Nova_PlotTopicCosmos(int topic, char *view, char *buf, int size);
int Nova_GetTribe(int *tribe_id, GraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int pid,
                  char *v);
void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
int Nova_AlreadyInTribe(int node, int *tribe_id);
void Nova_InitVertex(GraphNode *tribe, int i);
char *Nova_StripString(char *source, char *substring);
void Nova_DeClassifyTopic(char *typed_topic, char *topic, char *type);

/* weekly.c */

double Num(double x);
bool Nova_ReadWeekTimeSeries2(mongo_connection *conn, DataView *cfv, char *keyhash, char *vitalId);

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
int NovaWin_VerifyOwner(char *file, Promise *pp, Attributes attr);
int NovaWin_TakeFileOwnership(char *path);
int NovaWin_SetFileOwnership(char *path, SID *sid);
int NovaWin_GetOwnerName(char *path, char *owner, int ownerSz);
void NovaWin_VerifyFileAttributes(char *file, struct stat *dstat, Attributes attr, Promise *pp);
void NovaWin_VerifyCopiedFileAttributes(char *file, struct stat *dstat, Attributes attr, Promise *pp);
off_t NovaWin_GetDiskUsage(char *file, enum cfsizes type);
int NovaWin_GetNumHardlinks(char *path, int *numHardLinks);

/* win_log.c */

void OpenLog(int facility);
void MakeLog(Item *mess, enum cfreport level);
void LogPromiseResult(char *promiser, char peeType, void *promisee, char status, enum cfreport log_level, Item *mess);
void CloseLog(void);

/* win_mon.c */

void GatherCPUData(double *CF_THIS);

/* win_net.c */

void NovaWin_GetInterfaceInfo(void);
int TryConnect(AgentConnection *conn, struct timeval *tvp, struct sockaddr *cinp, int cinpSz);

/* win_pipe.c */

FILE *NovaWin_cf_popen(char *command, char *type);
FILE *NovaWin_cf_popen_sh(char *command, char *type);
FILE *NovaWin_cf_popensetuid(char *command, char *type, uid_t uid, gid_t gid, char *chdirv, char *chrootv,
                             int background);
FILE *NovaWin_cf_popen_shsetuid(char *command, char *type, uid_t uid, gid_t gid, char *chdirv, char *chrootv,
                                int background);
int NovaWin_cf_pclose(FILE *pp);
int NovaWin_cf_pclose_def(FILE *pfp, Attributes a, Promise *pp);

/* win_proc.c */

int NovaWin_DoAllSignals(Item *siglist, Attributes a, Promise *pp);
int NovaWin_GracefulTerminate(pid_t pid);
int NovaWin_IsProcessRunning(pid_t pid);
int NovaWin_ShellCommandReturnsZero(char *comm, int useshell);
int NovaWin_RunCmd(char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle);
int NovaWin_GetCurrentProcessOwner(SID *sid, int sidSz);
int NovaWin_GetCurrentUserName(char *userName, int userNameLen);
int NovaWin_SetTokenPrivilege(HANDLE token, char *privilegeName, int enablePriv);

/* win_ps.c */

int NovaWin_LoadProcessTable(Item **procdata);
int NovaWin_GetProcessSnapshot(Item **procdata);
int NovaWin_GatherProcessUsers(Item **userList, int *userListSz, int *numRootProcs, int *numOtherProcs);

/* win_service_exec.c */

void NovaWin_StartExecService(void);

/* win_sysinfo.c */

int NovaWin_GetWinDir(char *winDir, int winDirSz);
int NovaWin_GetSysDir(char *sysDir, int sysDirSz);
int NovaWin_GetProgDir(char *progDir, int progDirSz);
int NovaWin_GetEnv(char *varName, char *varContents, int varContentsSz);

# if defined(__MINGW32__)
const char *GetDefaultWorkDir(void);
# endif

/* win_user.c */

int NovaWin_UserNameToSid(char *userName, SID *sid, DWORD sidSz, int shouldExist);
int NovaWin_GroupNameToSid(char *groupName, SID *sid, DWORD sidSz, int shouldExist);
int NovaWin_NameToSid(char *name, SID *sid, DWORD sidSz);
int NovaWin_SidToName(SID *sid, char *name, int nameSz);
int NovaWin_SidToString(SID *sid, char *stringSid, int stringSz);
int NovaWin_StringToSid(char *stringSid, SID *sid, int sidSz);
UidList *NovaWin_Rlist2SidList(Rlist *uidnames, Promise *pp);

# if defined(__MINGW32__)
FnCallResult FnCallUserExists(FnCall *fp, Rlist *finalargs);
FnCallResult FnCallGroupExists(FnCall *fp, Rlist *finalargs);
# endif

/* win_wmi.c */

int NovaWin_GetInstalledPkgs(PackageItem ** pkgList, Attributes a, Promise *pp);
void NovaWin_PrintWmiError(char *str);
int NovaWin_WmiInitialize(void);
int NovaWin_WmiDeInitialize(void);

#endif /* MINGW */

/* yearly.c */

int Nova_ReadYearTimeSeries(mongo_connection *conn, DataView *cfv, char *keyhash, char *monId);
void Nova_DrawLongHAxes(DataView *cfv, int col);
void Nova_AnalyseLongHistory(char *keyname, enum observables obs, char *buffer, int bufsize);

/* ldap.c */
bool CfLDAPAuthenticate(char *uri, char *basedn, char *passwd, bool starttls, const char **const errstr);
int CfLDAP_JSON_GetSeveralAttributes(char *uri, char *user, char *basedn, char *filter, Rlist *names, char *scopes,
                                     char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                     int bufsize, const char **const errstr);
int CfLDAP_JSON_GetSingleAttributeList(char *uri, char *user, char *basedn, char *filter, char *name, char *scopes,
                                       char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                       int bufsize, const char **const errstr);

/***************************************************************************/

#define CF_METER_HEIGHT 80
#define CF_METER_WIDTH  500
#define CF_METER_MARGIN 5

/***************************************************************************/
/* Knowledge relationships                                                 */
/***************************************************************************/

// See also similar in cf3.defs.h

#define NOVA_USES_PR "relies on promise from"
#define NOVA_GIVES_PR "makes promise to"
#define NOVA_USES "uses"
#define NOVA_GIVES "is used by"
#define NOVA_ACTIVATED "is activated by class context"
#define NOVA_ACTIVATES "activates promises with handle"
#define NOVA_IMPACTS "has direct influence on"
#define NOVA_ISIMPACTED "is directly influenced by"
#define NOVA_BUNDLE_DATA "uses variables defined in"
#define NOVA_BUNDLE_DATA_INV_B "promises variables used by bundle"
#define NOVA_BUNDLE_DATA_INV_P "promises variables used by promise"
#define NOVA_HANDLE "is the promise handle affecting"
#define NOVA_HANDLE_INV "makes a promise with handle"
#define NOVA_SYNONYM KM_SYNONYM
#define NOVA_GOAL "contributes to business goal"
#define NOVA_GOAL_INV "is supported by a promise"
#define NOVA_GEN "is derived from"
#define NOVA_REPORTED "are reported in"
#define NOVA_SEEALSO "might also refer to"

/***************************************************************************/
/* DBs                                                                     */
/***************************************************************************/

#define NOVA_DIFF_LOG "nova_diff.log"
#define NOVA_PATCHES_INSTALLED "software_patch_status.csv"
#define NOVA_PATCHES_AVAIL "software_patches_avail.csv"
/* #define NOVA_SOFTWARE_INSTALLED "software_packages.csv" Moved to cf3.defs.h */

#define CF_BIGNUMBER 999999

#define CF_RED_THRESHOLD 1000
#define CF_AMBER_THRESHOLD 100
#define CF_GREEN 0
#define CF_CODE_BLUE -1
#define CF_BLUEHOST_THRESHOLD_DEFAULT 900       // 15 minutes

#define CF_HUB_HORIZON 900      // 15 mins / 3 connection attempts
#define CF_HUB_PURGESECS 604800 // one week

#define MONITORING_HISTORY_LENGTH_YEARS 3
#define MONITORING_HISTORY_LENGTH_WEEKS (3*52)
/* Not a real year, but a "monitoring year" */
#define MONITORING_WEEKS_PER_YEAR 52

/***************************************************************************/

typedef struct
{
    char *m;
    int d;
} MonthDays;

typedef struct
{
    double kept;
    double repaired;
    double notkept;
} PromiseValue;

typedef struct
{
    char rval[CF_MAXVARSIZE];
    time_t time;
} PersistentScalar;

/***************************************************************************/

#define CF_REPAIR_LOG    "cf_repair.log"
#define CF_NOTKEPT_LOG   "cf_notkept.log"

/* Protocol codebook for reports */

#define CFR_PERF "PRF"
#define CFR_CLASS "CLS"
#define CFR_SETUID "SUI"
#define CFR_FCHANGE "CHG"
#define CFR_FDIFF "DIF"
#define CFR_MONITOR_MAG "MNM"   // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_WEEK "MNW"  // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_YEAR "MNY"  // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_HIST "MNH"  // DEPRECATED from Nova 2.1.0 onwards

#define CFR_MONITOR_MG "MOM"
#define CFR_MONITOR_WK "MOW"
#define CFR_MONITOR_YR "MOY"
#define CFR_MONITOR_HG "MOH"

#define CFR_PCOMPLIANCE "PCP"
#define CFR_TCOMPLIANCE "TCP"
#define CFR_SOFTWARE "SOF"
#define CFR_AVAILPATCH "AVL"
#define CFR_PATCHSTATUS "PST"
#define CFR_PROMISEOUT "POT"
#define CFR_VALUE "VAL"
#define CFR_VARS "VAR"          // DEPRECATED from Nova 2.0.3 onwards
#define CFR_VARD "VRD"          // with date stamp
#define CFR_LASTSEEN "SEN"
#define CFR_REPAIRLOG "PRL"
#define CFR_NOTKEPTLOG "NKL"
#define CFR_METER "MET"
#define CFR_SWDATES "SWD"
#define CFR_BUNDLES "BUN"

/* Keynames */

#define cfr_keyhash      "kH"
#define cfr_ip_array     "IP"
#define cfr_host_array   "ha"
#define cfr_histo        "hs"
#define cfr_perf_event    "P"
#define cfr_software     "sw"
#define cfr_software_t   "lswt"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"
#define cfr_cause        "ca"
#define cfr_class        "cl"
#define cfr_class_keys   "ck"
#define cfr_class_jobs   "cj"
#define cfr_total_compliance "tc"
#define cfr_time          "t"
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
#define cfr_bundlecomp    "bc"
#define cfr_bundleavg     "bv"
#define cfr_bundledev     "bd"
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
#define cfr_day           "t"   // Substitute for time in value report
#define cfr_valuereport   "va"
#define cfr_netmeasure    "ne"
#define cfr_environment   "env"
#define cfr_score_comp    "scC"
#define cfr_score_anom    "scA"
#define cfr_score_perf    "scP"
#define cfr_score_lastseen "scL"
#define cfr_score_mixed   "scM"

#define cfr_netmeasure    "ne"
#define cfr_lastseen_hosts "lastseen_hosts"
#define cfr_deleted_hosts  "deleted_hosts"

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

#define cfm_magobs        "mo"  // DEPRECATED
#define cfm_weekobs       "wo"  // DEPRECATED
#define cfm_yearobs       "yo"  // DEPRECATED
#define cfm_data          "dt"  // DEPRECATED
#define cfm_id            "id"
#define cfm_description   "ds"
#define cfm_units         "us"
#define cfm_global        "gl"
#define cfm_expmin        "en"
#define cfm_expmax        "ex"
#define cfm_q_arr         "q"
#define cfm_expect_arr    "e"
#define cfm_deviance_arr  "d"

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
#define cfc_cachecompliance "cc"        // total compliance
#define cfc_cachebundlecompliance "vb"  // virtual bundle compliance
#define cfc_count_hosts "ch"
#define cfc_compliance_avg "ae"
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

// Report Keys
#define CFRID_BUNDLE_PROFILE "bundle-profile"
#define CFRID_PROMISE_COMPLIANCE "promise-compliance"
#define CFRID_PROMISES_REPAIRED_LOG "promises-repaired-log"
#define CFRID_PROMISES_REPAIRED_SUMMARY "promises-repaired-summary"
#define CFRID_PROMISES_NOTKEPT_LOG "promises-not-kept-log"
#define CFRID_PROMISES_NOTKEPT_SUMMARY "promises-not-kept-summary"

#define CFRID_CONTEXTS "contexts"
#define CFRID_NEIGHBORS "neighbors"
#define CFRID_VALUES "values"

#define CFRID_PATCHES_AVAILABLE "patches-available"
#define CFRID_PATCHES_INSTALLED "patches-installed"
#define CFRID_SOFTWARE_INSTALLED "software-installed"

#define CFRID_FILECHANGE_LOG "file-change-log"
#define CFRID_FILECHANGE_DIFFS "file-change-diffs"
#define CFRID_SETUID_PROGRAMS "setuid-programs"

#define CFRID_BUSINESS_VALUE "business-value"
#define CFRID_BENCHMARKS "benchmarks"
#define CFRID_COMPLIANCE_SUMMARY "compliance-summary"
#define CFRID_VIRTUAL_BUNDLES "virtual-bundles"

// appsettings keys
#define CFMP_BLUEHOST_THRESHOLD "bluehost_threshold_global"

#ifdef HAVE_CONSTELLATION
# include <cf.constellation.h>
#endif

#endif
