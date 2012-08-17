/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.h                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef CFENGINE_NOVA_H
#define CFENGINE_NOVA_H

#include "cf3.defs.h"

#define CF_DUNBAR_WORK 30

#define CF_TIME_SIZE 32
#define CF_FIPS_SIZE 32

#ifdef HAVE_LIBLDAP
/* LDAP functionality uses several functions which are only provided if this macro is defined */
# define LDAP_DEPRECATED 1
# include <ldap.h>
#endif

#ifdef HAVE_LIBMONGOC
// WARNING: If changing collection names: grep through source
# define MONGO_HAVE_STDINT
# define MONGO_BASE "cfreport"
# define MONGO_KMBASE "cfknow"
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
# define MONGO_KM_BUNDLES MONGO_KMBASE ".""KB"
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

typedef mongo EnterpriseDB;
#else
typedef void EnterpriseDB;
#endif

#undef PACKAGE

// New licensing policy

#define MAX_FREE_LICENSES 25

//

#define CF_INFSIZE 35
#define CF_TIMESERIESDATA 168   /* (24*7) */
#define CF_MAGDATA 48           /* (4*12) */
#define CF_MAX_SLOTS 2016       /* (24*7*12) */
#define CF_LHISTORYDATA 1464
#define CF_MAG_SLOTS (12*24*7)
#define CF_YEAR_SLOTS (3*52)
#define CF_TRIBE_SIZE 25
#define CF_MAX_LIMIT 999999
#define CF_N_CODE '\07'
#define MONITOR_CLASS_PREFIX "mXC_"
#define CF_CHANGE_HORIZON 10
#define NOVA_EXPORT_HEADER "NOVA_EXPORT"
#define CF_CODEBOOK_SIZE 31
#define NOVA_MAXDIFFSIZE (80 * 1024 * 1024)

// for pdf reports
#define CF_NOTICE_TRUNCATED "Data truncated due to internal buffer limit"
#define CF_STR_NOT_AVAILABLE "N/A"

//Search
#define CF_SEARCH_REGEX 1
#define CF_SEARCH_EXACT 2

/************************************************************************************/
/* Storyboarding (is not a form of torture)                                         */
/************************************************************************************/

enum cf_direction
   {
   cfi_forwards,
   cfi_backwards
   };

#define CF_MAX_STORY_DEPTH 20
#define HOSTKEY_SIZE 100        // length of SHA=...

#define CF_MAXTRANSSIZE (CF_BUFSIZE - CF_INBAND_OFFSET - 64)

// A node in a storyboard is a stateful path node that contains its history, like
// a BGP path route. Only the end nodes of a search tree are used, as these
// contains a complete path-route from start to end.

typedef struct StoryLine_ StoryLine;
typedef struct Chapter_ Chapter;

struct Chapter_
   {
   char *assoc;                                                 // How did we get here?

   char *topic;                                 // Cache db results - where are we now?
   char *context;
   int topic_id;

   Item *indirect_inferences;            // Used to make notes about confluences

   int certainty;

   Chapter *history[CF_MAX_STORY_DEPTH]; // chain of pointers, history BGP style
   };


/*****************************************************************************/

// list of stories
struct StoryLine_
   {
   Chapter *story;
   long checksum;
   int done;
   StoryLine *next;
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
    double data_dq[CF_LHISTORYDATA];
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

#ifdef HAVE_LIBMONGOC
#include "scorecards.h"

typedef struct
{
    char *keyhash;
    char *hubkey;
    char *ipaddr;
    char *hostname;
    time_t last_report;
    HostColour colour;
} HubHost;

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
    char *path;
    time_t t;
    char *handle;
    char *msg;
    char type;

    HubFileDiff *diff_record;
} HubFileChanges;

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
    char *hostkey;
    int kept[SHIFTS_PER_WEEK];
    int repaired[SHIFTS_PER_WEEK];
    int notkept[SHIFTS_PER_WEEK];
    int num_samples[SHIFTS_PER_WEEK];
} HubHostComplianceShifts;

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
} HubBundleSeen;

typedef struct
{
    HubHost *hh;
    char *day;                  /* 'DD Month YYYY' */
    double kept;
    double repaired;
    double notkept;
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
    Rlist *promisees;
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
    bool external;
    char *username;
    char *email;

    Rlist *roles;
} HubUser;

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
#endif

/*****************************************************************************/

/* Valid ACL syntax values, from documentation */

#ifdef MINGW
# define CF_GENERIC_READ_NT (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL)
# define CF_GENERIC_WRITE_NT (FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
# define CF_GENERIC_EXECUTE_NT (FILE_EXECUTE)
# define CF_MINIMUM_PERMS_NT (SYNCHRONIZE)      // access is always denied if synchronize is not set
#endif /* MINGW */

/************************************************************************************/
/* Prototypes                                                                       */
/************************************************************************************/

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

#if defined(__MINGW32__)
bool BootstrapAllowed(void);
#endif

/* client_code.c */
#ifdef HAVE_LIBMONGOC
int Nova_QueryClientForReports(EnterpriseDB *dbconn, AgentConnection *conn, const char *menu, time_t since);
void UnpackReportBook(EnterpriseDB *dbconn, char *id, Item **reports);

int Nova_StoreIncomingReports(char *reply, Item **reports, int current_report);
void NewReportBook(Item **reports);
void DeleteReportBook(Item **reports);
#endif
int Nova_PlaceCollectCall(AgentConnection *conn);

/* copernicus.c */

JsonElement *Nova_DrawTribe(int *tribe_id, GraphNode *tribe_node, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],
                            int tribe_size, double *tribe_evc, int topic);

#ifdef HAVE_LIBMONGOC
/* histogram.c */

int Nova_ReadHistogram2(EnterpriseDB *conn, DataView *cfv, char *hostkey, char *monId);
Item *Nova_MapHistogram(DataView *cfv, char *keyhash);
void Nova_AnalyseHistogram(char *keyhash, enum observables obs, char *buffer, int bufsize);

/* install.c */

HubQuery *NewHubQuery(Rlist *hosts, Rlist *records);
HubQuery *NewHubQueryErrid(Rlist *hosts, Rlist *records, cfapi_errid errid);
void DeleteHubQuery(HubQuery *hq, void (*fnptr) ());
int CountRecords(HubQuery *hq);
void *HubQueryGetFirstRecord(HubQuery *hq);
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
HubHostComplianceShifts *NewHubHostComplianceShifts(const char *hostkey);
void DeleteHubHostComplianceShifts(HubHostComplianceShifts *record);
HubVariable *NewHubVariable(HubHost *hh, char *type, char *scope, char *lval, Rval rval, time_t t);
void DeleteHubVariable(HubVariable *hv);
HubPromiseLog *NewHubPromiseLog(HubHost *hh, char *handle, char *cause, time_t t);
unsigned int HubPromiseLogHash(const void *hp_record);
bool HubPromiseLogEqual(const void *hp_record_a, const void *hp_record_b);
void DeleteHubPromiseLog(HubPromiseLog *hp);
HubPromiseSum *NewHubPromiseSum(HubHost *hh, char *handle, char *cause, int occurences, int hostOccurences);
int HubPromiseSumCompare(void *hps_a, void *hps_b);
void DeleteHubPromiseSum(HubPromiseSum *hs);
HubLastSeen *NewHubLastSeen(HubHost *hh, LastSeenDirection direction, char *kh, char *rhost, char *ip, double ago,
                            double avg, double dev, time_t t);
void DeleteHubLastSeen(HubLastSeen *hp);
HubMeter *NewHubMeter(HubHost *hh, char type, double kept, double repaired);
void DeleteHubMeter(HubMeter *hp);
HubPerformance *NewHubPerformance(HubHost *hh, char *event, time_t t, double q, double e, double d, char *handle);
void DeleteHubPerformance(HubPerformance *hp);
HubSetUid *NewHubSetUid(HubHost *hh, char *file);
void DeleteHubSetUid(HubSetUid *hp);
HubPromiseCompliance *NewHubCompliance(HubHost *hh, char *handle, PromiseState status, double e, double d, time_t t);
void DeleteHubPromiseCompliance(HubPromiseCompliance *hp);
HubBundleSeen *NewHubBundleSeen(HubHost *hh, char *rname, double ago, double avg, double dev, time_t t);
void DeleteHubBundleSeen(HubBundleSeen *hp);
HubFileChanges *NewHubFileChanges(HubHost *hh, char *file, time_t t, char *handle, char change_type, char *change_msg);
void DeleteHubFileChanges(HubFileChanges *hp);
HubFileDiff *NewHubFileDiff(HubHost *hh, char *file, char *diff, time_t t);
void DeleteHubFileDiff(HubFileDiff *hp);
HubValue *NewHubValue(HubHost *hh, char *day, double kept, double repaired, double notkept, char *handle);
void DeleteHubValue(HubValue *hp);
HubPromise *NewHubPromise(char *bn, char *bt, Rlist *ba, char *pt, char *pr, Rlist *pe, char *cl, char *ha, char *co,
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
HubUser *NewHubUser(bool external, const char *username, const char *email, const Rlist *roles);
void DeleteHubUser(HubUser *user);
HubUserRBAC *NewHubUserRBAC(const char *userName, const char *classRxInclude, const char *classRxExclude,
                            const char *bundleRxInclude, const char *bundleRxExclude);
int HubUserHash(const HubUser *user);
bool HubUserEqual(const HubUser *a, const HubUser *b);
void DeleteHubUserRBAC(HubUserRBAC *userRbac);
HubRole *NewHubRole(char *name, char *description,
                    char *classRxInclude, char *classRxExclude, char *bundleRxInclude, char *bundleRxExclude);
void DeleteHubRole(HubRole *role);


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
void PromiseFilterAddPromiseTypeRx(PromiseFilter *filter, const char *promiseTypeRxInclude);
void PromiseFilterAddBundleTypeRx(PromiseFilter *filter, const char *bundleTypeRxInclude);
void PromiseFilterAddBundles(PromiseFilter *filter, const char *bundleInclude, const char *bundleExclude);
void PromiseFilterAddBundlesRx(PromiseFilter *filter, const char *bundleRxInclude, const char *bundleRxExclude);
void DeletePromiseFilter(PromiseFilter *filter);

HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount,
                                                    double kept, double repaired, double notkept, time_t genTime);
void DeleteHubCacheTotalCompliance(HubCacheTotalCompliance *tc);

void HubQuerySortPromiseBundles(HubQuery *hqPromiseBundles);

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
#endif

int PageRecords(Rlist **records_p, PageInfo *page, void (*fnptr) ());
void CountMarginRecordsVars(Rlist **records_p, PageInfo *page, int *start_count, int *end_count);

/* knowledge.c */

char *Name2Id(char *s);
void Nova_StoreKMDB(Topic **topichash, Occurrence *occurrences, Inference *inferences);

void Nova_MapPromiseToTopic(const ReportContext *report_context, const Promise *pp, const char *version);
void Nova_BundleReference(FILE *fp, char *bundle);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_PromiseNode(FILE *fp, Promise *pp, int calltype);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_BundleNode(FILE *fp, char *bundle);
void Nova_BodyNode(FILE *fp, char *body, int calltype);
void Nova_DependencyGraph(Topic *map);
void Nova_PlotTopicDependencies(int topic, double **adj, char **names, int dim);

// generating test data
void Nova_GenerateTestData(int count);
void Nova_RemoveTestData(void);
void Nova_UpdateTestData(void);
char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE + 1]);
void ThisHashString(char *str, char *buffer, int len, unsigned char digest[EVP_MAX_MD_SIZE + 1]);

/* license.c */

int Nova_CheckLicenseWin(char *pos);
char *Nova_LicenseOwner(void);

/* magnify.c */

#ifdef HAVE_LIBMONGOC
bool Nova_ReadMagTimeSeries2(EnterpriseDB *conn, DataView *cfv, char *hostkey, char *vitalId);
#endif

/* monitoring.c */

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
void NovaNamedEvent(char *eventname, double value, Attributes a, Promise *pp);
void Nova_MonOtherInit(void);
void Nova_MonOtherGatherData(double *cf_this);
void Nova_LookupAggregateClassName(int n, char *name, char *desc);

/* - date-related functions - */

time_t WeekBegin(time_t time);
time_t SubtractWeeks(time_t time, int weeks);
time_t NextShift(time_t time);
bool GetRecordForTime(CF_DB *db, time_t time, Averages *result);
double BoundedValue(double value, double defval);

/* mon_cumulative.c */

unsigned GetInstantUint32Value(const char *name, const char *subname, unsigned value, time_t timestamp);
unsigned long long GetInstantUint64Value(const char *name, const char *subname, unsigned long long value,
                                         time_t timestamp);

/* promise_db.c */

#ifdef HAVE_LIBMONGOC
void CFDB_SaveExpandedPromise(const Promise *pp);
void CFDB_SaveUnExpandedPromises(const Bundle *bundles, const Body *bodies);
#endif

/* promises.c */

const char *Nova_Version(void);
const char *Nova_NameVersion(void);

/* pscalar.c */

int Nova_GetPersistentScalar(char *lval, char *rval, int size, time_t timeout);
void Nova_SetPersistentScalar(char *lval, char *rval);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s, Attributes a, Promise *pp);

int CheckRegistrySanity(Attributes a, Promise *pp);


void Nova_TrackExecution();

/* reporting.c */

void Nova_ImportReports(const char *input_file);

/* runagent.c */

bool Nova_ExecuteRunagent(AgentConnection *conn, const char *menu);

/* server.c */

void Nova_PackAllReports(Item **reply, time_t from, time_t delta1, enum cfd_menu type);

pid_t Nova_StartTwin(int argc, char **argv);
void Nova_SignalTwin(void);
void Nova_SignalOther(void);
void Nova_ReviveOther(int argc, char **argv);
void Nova_DoTryCollectCall(void);

/* stories.c */

enum storytype
{
    cfi_cause,
    cfi_connect,
    cfi_part,
    cfi_access,
    cfi_none
};

#ifdef HAVE_LIBMONGOC
int PossibleInference(char *assoc,enum storytype type,enum cf_direction direction,Chapter *this,char *topic,char *context);
void Nova_GetWeatherReport(char *hostkey,Item **low, Item **high, Item **anomaly, Item **focused, Item **unfocused,Item **ldt);
void Nova_ListPossibleStoriesCmdLine(void);
void Nova_GenerateStoriesCmdLine(char *typed_topic,enum storytype type);
int Nova_GenerateStories_by_name_JSON(char *typed_topic,enum storytype type,char *buffer,int bufsize);
int Nova_GenerateStories_by_id_JSON(int topic_id,enum storytype type,char *buffer,int bufsize);
void GenerateStoriesAbout(Chapter **story_topic,enum storytype type,StoryLine **finished,int depth,enum cf_direction direction);

Chapter *AppendToStory(Chapter **last,char *topic,char *context,char *assoc,int topic_id);
StoryLine *PrependStoryLine(StoryLine **start,Chapter *story);
Chapter *NewChapter(char *topic,char *context,int topic_id);
int SameStoryDifferentOutcome(Chapter *s1,Chapter *s2);
void DeleteStoryLine(StoryLine *start);
void DeleteStory(Chapter *start);
void Nova_ShowStoryLine(StoryLine *list,enum storytype x);
void Nova_ShowStoryLine_JSON(StoryLine *list,enum storytype type,char *buffer,int bufsize);

void Nova_ScanAccessRelationships(FILE *fp, Promise *pp,char *promise_id);
char *Nova_Relevant3Inference(char *prev,char *this,char *table[CF_INFSIZE][3]);
char *Nova_Relevant2Inference(char *this,char *table[CF_INFSIZE][2]);
char *Nova_Possible3Inference(char *this,char *table[CF_INFSIZE][3]);
double Nova_Inference(Chapter *path[CF_MAX_STORY_DEPTH],enum storytype x,enum cf_direction y,int quiet);
void Nova_HostStory(const Policy *policy,char *hostkey,char *buffer, int bufsize);
int AlreadySaidThis(Chapter *history[CF_MAX_STORY_DEPTH],int topic_id);
void Nova_GetLocations(const Policy *policy,char *hostkey, Rlist **locations);
#endif
/* topicmap.c */

#ifdef HAVE_LIBMONGOC
void Nova_DumpTopics(void);
void Nova_FillInGoalComment(Item *ip);
const char *Nova_GetBundleComment(char *bundle);
void Nova_ShowTopic(char *qualified_topic);

void Nova_LookupUniqueAssoc(int pid, char *buffer, int bufsize);
JsonElement *Nova_ScanTheRest(int pid);
Item *Nova_SearchTopicMap(char *typed_topic,int type,int merge);
Item *Nova_ScanLeadsAssociations(int pid, char *view);
JsonElement *Nova_ScanOccurrences(int this_id);
int Nova_GetTopicComment(char *topic_name, char *topic_context, char *buffer, int bufsize);

int Nova_GetTopicByTopicId(int pid, char *topic_name, char *topic_id, char *topic_type, char *buffer);
int Nova_AddTopicSearchBuffer(int pid, char *topic_name, char *topic_type, char *buffer, int bufsize);
int Nova_AddAssocSearchBuffer(char *from_assoc, char *to_assoc, char *buffer, int bufsize);
JsonElement *Nova_AddOccurrenceBuffer(char *context, char *locator, enum representations locator_type, char *represents);
void AtomizeTopicContext(AlphaList *context_list, char *topic_context);
char *Nova_TopicIdURL(int pid, char *s);
char *Nova_AssocURL(char *s);
char *Nova_URL(char *s, char *rep);
Item *Nova_NearestNeighbours(int topic_id, char *assoc_mask);
Item *Nova_GetTopicsInContext(char *context);
Item *Nova_GetBusinessGoals(char *handle);
int Nova_GetUniqueBusinessGoals(char *buffer, int bufsize);
int Nova_GetApplicationServices(char *buffer, int bufsize);

JsonElement *Nova_PlotTopicCosmos(int topic, char *view);
int Nova_GetTribe(int *tribe_id, GraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int pid,
                  char *v);
void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
int Nova_AlreadyInTribe(int node, int *tribe_id);
void Nova_InitVertex(GraphNode *tribe, int i);
char *Nova_StripString(char *source, char *substring);
void Nova_DeClassifyTopic(char *typed_topic, char *topic, char *type);
#endif

/* weekly.c */

#ifdef HAVE_LIBMONGOC
double Num(double x);
bool Nova_ReadWeekTimeSeries2(EnterpriseDB *conn, DataView *cfv, char *keyhash, char *vitalId);
#endif

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
time_t NovaWin_FileTimeToTimet(FILETIME *ft);
int NovaWin_uname(struct utsname *buf);

/* win_file.c */

FILE *NovaWin_FileHandleToStream(HANDLE fHandle, char *mode);
int NovaWin_mkdir(const char *path, mode_t mode);
int NovaWin_rename(const char *oldpath, const char *newpath);
int NovaWin_FileExists(const char *fileName);
int NovaWin_IsDir(char *fileName);
int NovaWin_TakeFileOwnership(char *path);
int NovaWin_SetFileOwnership(char *path, SID *sid);
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

int TryConnect(AgentConnection *conn, struct timeval *tvp, struct sockaddr *cinp, int cinpSz);

/* win_proc.c */

int NovaWin_IsProcessRunning(pid_t pid);
int NovaWin_RunCmd(const char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle);
int NovaWin_GetCurrentProcessOwner(SID *sid, int sidSz);
int NovaWin_SetTokenPrivilege(HANDLE token, char *privilegeName, int enablePriv);

/* win_ps.c */

int NovaWin_GetProcessSnapshot(Item **procdata);
int GatherProcessUsers(Item **userList, int *userListSz, int *numRootProcs, int *numOtherProcs);

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

int NovaWin_PackageListInstalledFromAPI(PackageItem ** pkgList, Attributes a, Promise *pp);
void NovaWin_PrintWmiError(char *str);
int NovaWin_WmiInitialize(void);
int NovaWin_WmiDeInitialize(void);

#endif /* MINGW */

/* yearly.c */

#ifdef HAVE_LIBMONGOC
int Nova_ReadYearTimeSeries(EnterpriseDB *conn, DataView *cfv, char *keyhash, char *monId);
void Nova_DrawLongHAxes(DataView *cfv, int col);
void Nova_AnalyseLongHistory(char *keyname, enum observables obs, char *buffer, int bufsize);
#endif

#include "ldap_lib.h"

/***************************************************************************/
/* Knowledge relationships                                                 */
/***************************************************************************/

#define KM_AFFECTS_CERT_F "affects"
#define KM_AFFECTS_CERT_B "is affected by"
#define KM_CAUSE_CERT_F "causes"
#define KM_CAUSE_CERT_B "is caused by"
#define KM_PARTOF_CERT_F "part of"
#define KM_PARTOF_CERT_B "consists of"
#define KM_DETERMINES_CERT_F "determine(s)"
#define KM_DETERMINES_CERT_B "is/are determined by"
#define KM_CONTRIBUTES_CERT_F "contibutes to"
#define KM_CONTRIBUTES_CERT_B "is contibuted to by"
#define KM_USES_CERT_F "use(s)"
#define KM_USES_CERT_B "is/are used by"
#define KM_PROVIDES_CERT_F "provide(s)"
#define KM_PROVIDES_CERT_B "is/are provided by"
#define KM_BELONGS_CERT_F "belongs to"
#define KM_BELONGS_CERT_B "owns"
#define KM_CONNECTS_CERT_F "connects to"
#define KM_CONNECTS_CERT_B "connects to"
#define KM_NEEDS_CERT_F "need(s)"
#define KM_NEEDS_CERT_B "is needed by"
#define KM_EQUIV_CERT_F "is equivalent to"
#define KM_EQUIV_CERT_B "is equivalent to"
#define KM_SHIELD_CERT_F "denies access from"
#define KM_SHIELD_CERT_B "is not allowed access to"
#define KM_ACCESS_CERT_F "grants access to"
#define KM_ACCESS_CERT_B "is allowed to access"
#define KM_MONITOR_CERT_F "monitor(s)"
#define KM_MONITOR_CERT_B "is/are monitored by"
#define KM_LOCATED_CERT_F "is located in"
#define KM_LOCATED_CERT_B "situates"
#define KM_REPAIR_CERT_F "repairs"
#define KM_REPAIR_CERT_B "is repaired by"
#define KM_FOLLOW_CERT_F "is followed by"
#define KM_FOLLOW_CERT_B "is preceded by"
#define KM_INVOLVES_CERT_F "involves"
#define KM_INVOLVES_CERT_B "is involved in"
#define KM_IMPLEMENTS_CERT_F "implements"
#define KM_IMPLEMENTS_CERT_B "is implemented by"

#define KM_MENTIONS_F "is mentioned in"
#define KM_MENTIONS_B "mentions"

#define KM_CAUSE_POSS_F "can cause"
#define KM_CAUSE_POSS_B "can be caused by"
#define KM_PARTOF_POSS_F "can be a part of"
#define KM_PARTOF_POSS_B "can consist of"
#define KM_DETERMINES_POSS_F "can determine"
#define KM_DETERMINES_POSS_B "can be determined by"
#define KM_CONTRIBUTES_POSS_F "can contibute to"
#define KM_CONTRIBUTES_POSS_B "can be contibuted to by"
#define KM_USES_POSS_F "can use"
#define KM_USES_POSS_B "can be used by"
#define KM_PROVIDES_POSS_F "can provide"
#define KM_PROVIDES_POSS_B "can be provided by"
#define KM_BELONGS_POSS_F "can belong to"
#define KM_BELONGS_POSS_B "can own"
#define KM_AFFECTS_POSS_F "can affect"
#define KM_AFFECTS_POSS_B "can be affected by"
#define KM_CONNECTS_POSS_F "can connect to"
#define KM_CONNECTS_POSS_B "can connect to"
#define KM_NEEDS_POSS_F "can need"
#define KM_NEEDS_POSS_B "can be needed by"
#define KM_EQUIV_POSS_F "can be equivalent to"
#define KM_EQUIV_POSS_B "can be equivalent to"
#define KM_MONITOR_POSS_F "can monitor"
#define KM_MONITOR_POSS_B "can be monitored by"
#define KM_ACCESS_POSS_F "can access to"
#define KM_ACCESS_POSS_B "can be allowed to access"
#define KM_LOCATED_POSS_F "can be located in"
#define KM_LOCATED_POSS_B "can situate"
#define KM_REPAIR_POSS_F "can repair"
#define KM_REPAIR_POSS_B "can be repaired by"
#define KM_FOLLOW_POSS_F "can be followed by"
#define KM_FOLLOW_POSS_B "can be preceded by"
#define KM_INVOLVES_POSS_F "can involve"
#define KM_INVOLVES_POSS_B "can be involved in"
#define KM_IMPLEMENTS_POSS_F "can implement"
#define KM_IMPLEMENTS_POSS_B "can be implemented by"

#define KM_CAUSE_UNCERT_F "might cause"
#define KM_CAUSE_UNCERT_B "might be caused by"
#define KM_PARTOF_UNCERT_F "might be a part of"
#define KM_PARTOF_UNCERT_B "might consist of"
#define KM_DETERMINES_UNCERT_F "might determine"
#define KM_DETERMINES_UNCERT_B "might be determined by"
#define KM_CONTRIBUTES_UNCERT_F "might contibute to"
#define KM_CONTRIBUTES_UNCERT_B "might be contibuted to by"
#define KM_USES_UNCERT_F "might use"
#define KM_USES_UNCERT_B "might be used by"
#define KM_PROVIDES_UNCERT_F "might provide"
#define KM_PROVIDES_UNCERT_B "might be provided by"
#define KM_BELONGS_UNCERT_F "might belong to"
#define KM_BELONGS_UNCERT_B "might own"
#define KM_AFFECTS_UNCERT_F "might affect"
#define KM_AFFECTS_UNCERT_B "might be affected by"
#define KM_CONNECTS_UNCERT_F "might connect to"
#define KM_CONNECTS_UNCERT_B "might connect to"
#define KM_NEEDS_UNCERT_F "might need"
#define KM_NEEDS_UNCERT_B "might be needed by"
#define KM_EQUIV_UNCERT_F "might be equivalent to"
#define KM_EQUIV_UNCERT_B "might be equivalent to"
#define KM_SHIELD_UNCERT_F "might deny access from"
#define KM_SHIELD_UNCERT_B "might not be allowed access to"
#define KM_MONITOR_UNCERT_F "might monitor"
#define KM_MONITOR_UNCERT_B "might be monitored by"
#define KM_ACCESS_UNCERT_F "might grant access to"
#define KM_ACCESS_UNCERT_B "might be allowed to access"
#define KM_LOCATED_UNCERT_F "might be located in"
#define KM_LOCATED_UNCERT_B "might situate"
#define KM_REPAIR_UNCERT_F "might repair"
#define KM_REPAIR_UNCERT_B "might be repaired by"
#define KM_FOLLOW_UNCERT_F "might be followed by"
#define KM_FOLLOW_UNCERT_B "might be preceded by"
#define KM_INVOLVES_UNCERT_F "might involve"
#define KM_INVOLVES_UNCERT_B "might be involved in"
#define KM_IMPLEMENTS_UNCERT_F "might implement"
#define KM_IMPLEMENTS_UNCERT_B "might be implemented by"


#define KM_GENERALIZES_F "is a generalization of"
#define KM_GENERALIZES_B "is a special case of"
#define KM_SYNONYM "is a synonym for"

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
#define NOVA_GOAL "contributes to goal"
#define NOVA_GOAL_INV "is a goal supported by"
#define NOVA_GEN "is derived from"
#define NOVA_REPORTED "are reported in"
#define NOVA_SEEALSO "might also refer to"


enum knowledgecertainty
{
    cfk_certain,
    cfk_uncertain,
    cfk_possible
};

/***************************************************************************/
/* DBs                                                                     */
/***************************************************************************/

#define NOVA_DIFF_LOG "nova_diff.log"
#define NOVA_PATCHES_INSTALLED "software_patch_status.csv"
#define NOVA_PATCHES_AVAIL "software_patches_avail.csv"
/* #define NOVA_SOFTWARE_INSTALLED "software_packages.csv" Moved to cf3.defs.h */

#define NOVA_TRACK_LAST_EXEC "last_exec"
#define NOVA_TRACK_DELTA_SCHEDULE "delta_gavr"

#define CF_BIGNUMBER 999999

#define CF_RED_THRESHOLD 1000
#define CF_AMBER_THRESHOLD 100
#define CF_GREEN 0
#define CF_CODE_BLUE -1
#define CF_CODE_BLACK -2
#define CF_BLUEHOST_THRESHOLD_DEFAULT 900       // 15 minutes

#define CF_BLACKHOST_THRESHOLD 3 // number of missed averaged schedules
#define CF_BLACKHOST_THRESHOLD_VARIATION 10 // additional mesurment error tolerance [%]

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
#define CFR_FCHANGE_OLD "CHG"
#define CFR_FCHANGE "CNG"
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
#define CFR_EXECUTION_STATUS "EXS"

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
#define cfr_compliance_shifts "cs"
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
#define cfr_bundle_namespace "ns"
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
#define cfr_filechangetype    "fs"
#define cfr_filechangemsg    "fm"
#define cfr_filediffs     "fd"
#define cfr_diff          "FD"
#define cfr_bundles       "B"
#define cfr_day           "t"   // Substitute for time in value report
#define cfr_valuereport   "va"
#define cfr_netmeasure    "nee"
#define cfr_netmeasure_expect "nx"
#define cfr_netmeasure_var "nv"
#define cfr_netmeasure_dq "ndq"
#define cfr_netmeasure_q  "nq"
#define cfr_environment   "env"
#define cfr_score_comp    "scC"
#define cfr_score_anom    "scA"
#define cfr_score_perf    "scP"
#define cfr_score_lastseen "scL"
#define cfr_score_mixed   "scM"
#define cfr_schedule      "sch"
#define cfr_is_black      "bh"
#define cfr_last_execution "lx"
#define cfr_last_update_size "us"
#define cfr_count "cn"
#define cfr_license "license"
#define cfr_license_expires "expires"
#define cfr_license_install_time "installTime"
#define cfr_license_owner "owner"
#define cfr_license_granted "granted"
#define cfr_license_promised "promised"
#define cfr_license_usage "licenseUsage"
#define cfr_license_usage_last_measured "lastMeasured"
#define cfr_license_usage_samples "samples"
#define cfr_license_usage_max_observed_level "maxObservedLevel"
#define cfr_license_usage_min_observed_level "minObservedLevel"
#define cfr_license_usage_mean_usage "meanUsage"
#define cfr_license_usage_mean_utilization_cumulative "meanCumulativeUtilization"
#define cfr_license_usage_used_today "usedToday"


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
#define cfm_grad_arr      "g"

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
#define cfk_bundle "tb"

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
#define cfk_occurtopic "oT"

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
#define CFRID_WEAKEST_HOSTS "weakest-hosts"

#define CFRID_PATCHES_AVAILABLE "patches-available"
#define CFRID_PATCHES_INSTALLED "patches-installed"
#define CFRID_SOFTWARE_INSTALLED "software-installed"

#define CFRID_FILECHANGE_LOG "file-change-log"
#define CFRID_FILECHANGE_DIFFS "file-change-diffs"
#define CFRID_SETUID_PROGRAMS "setuid-programs"

#define CFRID_BUSINESS_VALUE "business-value"
#define CFRID_BENCHMARKS "benchmarks"
#define CFRID_COMPLIANCE_SUMMARY "compliance-summary"

#ifdef HAVE_CONSTELLATION
# include <cf.constellation.h>
#endif

#endif
