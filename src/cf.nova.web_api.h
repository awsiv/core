/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.web_api.h                                                   */
/*                                                                           */
/* Created: Tue Feb 22 14:35:16 2011                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef CFENGINE_CF_NOVA_WEB_API_H
#define CFENGINE_CF_NOVA_WEB_API_H

#include <stdbool.h>

/* error handling and description*/

/* Corresponding description is in web_api.c:ERRID_DESCRIPTION */
typedef enum cfapi_errid
   {
   ERRID_SUCCESS,
   ERRID_DBCONNECT,
   ERRID_DBCLOSE,
   ERRID_DB_OPERATION,
   ERRID_ARGUMENT_MISSING,
   ERRID_ARGUMENT_WRONG,
   ERRID_BUFFER_FULL,
   ERRID_ITEM_NONEXISTING,
   ERRID_ITEM_EXISTS,
   ERRID_ITEM_MULTIPLE,
   ERRID_RBAC_DISABLED,
   ERRID_RBAC_ACCESS_DENIED,
   ERRID_CONSTELLATION_LICENSE,
   ERRID_MAX
   } cfapi_errid;

/* reports */

/* Corresponding names and descriptions are in web_api.c:CDP_REPORTS */
typedef enum cdp_report
   {
   cdp_acls,
   cdp_commands,
   cdp_filechanges,
   cdp_filediffs,
   cdp_registry,
   cdp_services,
   cdp_unknown
   } cdp_report;

typedef struct
   {
   Rlist *classRxIncludes;
   Rlist *classRxExcludes;
   } HostClassFilter;

typedef struct
   {
   char *handleInclude;
   char *handleRxInclude;
   char *promiserInclude;
   char *promiserRxInclude;
   char *bundleTypeInclude;
   Rlist *bundleIncludes;
   Rlist *bundleRxIncludes;
   Rlist *bundleExcludes;
   Rlist *bundleRxExcludes;
   } PromiseFilter;


/* misc */

extern int AM_PHP_MODULE;

// Separation needed because it is included by php extension

/*
 * These functions are apparently used by PHP module
 */
#ifdef HAVE_LIBMONGOC
int CFDB_GetValue(char *lval, char *rval, int size, char *db_name);
int Nova_GetTopicIdForTopic(char *typed_topic);
#endif

/*
 * Returned error strings are statically allocated and do not need freeing.
 * NULL is placed to errstr if no error occured.
 */
int Nova2PHP_LDAPAuthenticate(char *uri,char *basedn,char *passwd, bool starttls, const char **const errstr);
void *CfLDAPValue(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec);

int Nova2PHP_LDAPGetSeveralAttributes(char *uri,char *user,char *basedn,char *filter,Rlist *names,char *scopes,char *sec,char *passwd,bool starttls,int page,int linesperpage,char *buffer, int bufsize, const char **const errstr);
int Nova2PHP_LDAPGetSingleAttributeList(char *uri,char *user,char *basedn,char *filter,char *name,char *scopes,char *sec,char *passwd,bool starttls,int page,int linesperpage,char *buffer, int bufsize, const char **const errstr);

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize);

int Nova2PHP_get_value_graph(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize);
int Nova2PHP_list_all_goals(char *buffer,int bufsize);

// class finder
int Nova2PHP_listclasses_time(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_listclasses_soft(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_listclasses_ip(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_listclasses_all(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_listclasses_host(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);

void Nova2PHP_get_knowledge_view(int pid,char *view,char *buffer,int bufsize);
char *Nova_FormatDiff(char *s);
void Nova2PHP_get_network_speed(char *hostkey,char *buffer, int bufsize);
void Nova2PHP_GetPromiseBody(char *name,char *type,char *returnval,int bufsize);
int Nova2PHP_list_bodies(char *name,char *type,char *buffer,int bufsize);
char *Nova_LongState(char s);
char *Nova_LongStateWarn(char s);
int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz);
int Nova2PHP_docroot(char *buffer,int bufsize);
void Nova_EnterpriseModuleTrick(void);
void Nova2PHP_getlastupdate(char *hostkey,char *buffer,int bufsize);
void Nova2PHP_get_host_colour(char *hostkey,char *buffer,int bufsize);
int Nova2PHP_promiselog_summary(char *hostkey,char *handle, PromiseLogState state,time_t from, time_t to,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_promiselog(char *hostkey,char *handle, PromiseLogState state,time_t from,time_t to,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_value_report(char *hostkey,char *day,char *month,char *year,HostClassFilter *hostClassFilter,PageInfo *page,char *buffer,int bufsize);
void Nova2PHP_summary_meter(char *buffer,int bufsize);
void Nova2PHP_meter(char *hostkey,char *buffer,int bufsize);
int Nova2PHP_hostinfo(char *hostkey,char *hostnameOut,char *ipaddrOut,int bufsize);
int Nova2PHP_software_report(char *hostkey,char *name,char *value, char *arch,int regex,char *type,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_classes_report(char *hostkey,char *name,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_classes_summary(char **classes, char *buf, int bufsize);
int Nova2PHP_countclasses(char *hostkey,char *name,int regex, HostClassFilter *hostClassFilter, char *returnval,int bufsize);
int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_compliance_promises(char *hostkey,char *handle,char *status,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddr,time_t lago,int lregex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_performance_report(char *hostkey,char *job,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_setuid_report(char *hostkey,char *file,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,HostClassFilter *hostClassFilter,PageInfo *page, int lookInArchive, char *returnval,int bufsize);
int Nova2PHP_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,HostClassFilter *hostClassFilter,PageInfo *page, int lookInArchive, char *returnval,int bufsize);
int Nova2PHP_summary_report(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_classes_hosts(char *hostkey,char *name,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_vars_hosts(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_compliance_hosts(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_promise_hosts(char *hostkey,char *handle,char *status,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_lastseen_hosts(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_performance_hosts(char *hostkey,char *job,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_setuid_hosts(char *hostkey,char *file,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_bundle_hosts(char *hostkey,char *bundle,int regex,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_filechanges_hosts(char *hostkey,char *file,int regex,time_t t,char *cmp,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_filediffs_hosts(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_promiselog_hosts(char *hostkey,char *handle, PromiseLogState state,time_t from,time_t to,HostClassFilter *hostClassFilter,char *returnval,int bufsize);
int Nova2PHP_value_hosts(char *hostkey,char *day,char *month,char *year,HostClassFilter *hostClassFilter,char *returnval,int bufsize);

int Nova2PHP_search_topics(char *search,int regex,char *buffer,int bufsize);
void Nova2PHP_show_topic(int id,char * buffer,int bufsize);
void Nova2PHP_show_topic_leads(int id,char *buffer,int bufsize);
void Nova2PHP_show_topic_hits(int id,char *buffer,int bufsize);
void Nova2PHP_show_topic_category(int id,char *buffer,int bufsize);
void Nova2PHP_show_topN(char *policy,int n,PageInfo *page,char *buffer,int bufsize);
void Nova2PHP_show_all_hosts(char *policy,int n,char *buffer,int bufsize);
int Nova2PHP_show_hosts(char *hostNameRegex,char *ipRegex,char *classRegex,PageInfo *page,char *buf,int bufsize);
void Nova2PHP_select_hosts(char *match,char *sel,int n,char *buffer,int bufsize);
void Nova2PHP_select_reports(char *reportId, char *buffer, int bufsize);

long Nova2PHP_count_hosts(void);
long Nova2PHP_count_blue_hosts(void);
long Nova2PHP_count_red_hosts(void);
long Nova2PHP_count_yellow_hosts(void);
long Nova2PHP_count_green_hosts(void);
void Nova2PHP_show_col_hosts(char *colour,int n,PageInfo *page,char *buffer,int bufsize);
int Nova2PHP_list_promise_handles(char *promiser,char *ptype,char *bundle,char *btype,int regex,char *returnval,int bufsize);
int Nova2PHP_list_promise_handles_with_comments(char *bundle,char *btype,char *returnval,int bufsize);
int Nova2PHP_promise_list(PromiseFilter *promiseFilter ,char *returnval, int bufsize);

char *Nova_HostProfile(char *key);
void Nova2PHP_show_all_hosts(char *policy,int n,char *buffer,int bufsize);
int Nova2PHP_promise_details(PromiseFilter *filter, char *returnval,int bufsize);

char *Nova2PHP_GetPromiseComment(char *handle);
char *Nova2PHP_GetPromiseType(char *handle);
char *Nova2PHP_GetPromiser(char *handle);
int Nova2PHP_get_variable(char *hostkey,char *scope,char *lval,char *returnval,int bufsize);
int Nova2PHP_bundle_classes_used(PromiseFilter *promiseFilter, char *buffer, int bufsize);
int Nova2PHP_bundle_agent_goals(PromiseFilter *filter, char *buffer, int bufsize);
int Nova2PHP_list_bundles_using(char *name,char *buffer,int bufsize);
void Nova2PHP_ComplianceSummaryGraph(char *policy,char *buffer,int bufsize);
int Nova2PHP_report_description(char *reportName,char *returnval,int bufsize);
void Nova2PHP_enterprise_version(char *buf, int bufsize);
int Nova2PHP_community_version(char *buf, int bufsize);

void Nova2PHP_GetLibraryDocuments(char *pth,char *buffer,int bufsize);
int Nova_AddReportHeader(char *header,int truncated,char *buffer,int bufsize);

/* svn helper functions */
int Nova2PHP_validate_policy(char *file,char *buffer,int bufsize);

/*
 * commenting
 */

char *Nova2PHP_get_help_text(char *key);

int Nova2PHP_add_new_note(char *keyhash, char *repid, int reportType, char *username, time_t datetime, char *note,char *returnval, int bufsize);
int Nova2PHP_add_note(char *noteid,char *username, time_t datetime, char* note,char *returnval, int bufsize);
int Nova2PHP_get_notes(char *keyhash, char *cid, char *username, time_t from, time_t to,PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize);

void Nova2PHP_cdp_reportnames(char *buf,int bufSz);
int Nova2PHP_cdp_report(char *hostkey, char *reportName, PageInfo *page, char *buf, int bufSz);
char *GetCdpTableHeader(cdp_report cdpType);
cdp_report CdpReportNameToType(char *reportName);


int Nova2PHP_delete_host(char *keyHash);

/*
 * replica set
 */
int Nova2PHP_replica_status(char *buffer, int bufsize);
int Nova2PHP_GetHubKey(char *buffer,int bufsize);
int Nova2PHP_GetHubMaster(char *buffer,int bufsize);

typedef struct EnvironmentsList_ EnvironmentsList;

struct EnvironmentsList_
   {
   char *name;
   EnvironmentsList *next;
   };

bool Nova2PHP_environments_list(EnvironmentsList **out);

typedef struct HostsList_ HostsList;

struct HostsList_
   {
   char *keyhash;
   HostsList *next;
   };

bool Nova2PHP_environment_contents(const char *environment,
                                   HostsList **out);

char *Nova2PHP_get_host_environment(const char *keyhash);

void FreeEnvironmentsList(EnvironmentsList *list);
void FreeHostsList(HostsList *list);


const char *GetErrorDescription(cfapi_errid errid);
char *FormatErrorJsonAttribute(char *out, int outSz, cfapi_errid errid);
char *FormatSingletonErrorJson(char *out, int outSz, cfapi_errid errid);
void EndJsonBuffer(char *buf, int bufsize, cfapi_errid errid);

/* For volume testing Mission portal */
#ifndef NDEBUG
int Nova2PHP_promise_list_test(PromiseFilter *promiseFilter ,char *returnval, int bufsize);
int Nova2PHP_classes_report_test(char *hostkey,char *name,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_classes_summary_test(char **classes, char *buf, int bufsize);
int Nova2PHP_show_hosts_test(char *hostNameRegex,char *ipRegex,char *classRegex,PageInfo *page,char *buf,int bufsize);
int Nova2PHP_bundle_report_test(char *hostkey,char *bundle,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize);
#endif
#endif // CFENGINE_NOVA_WEB_API_H
