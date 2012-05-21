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
#include "scorecards.h"

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
    ERRID_DATA_UNAVAILABLE,
    ERRID_HOST_NOT_FOUND,
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
    char *promiseTypeInclude;
    char *promiseTypeRxInclude;
    char *bundleTypeInclude;
    char *bundleTypeRxInclude;
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
int Nova2PHP_LDAPAuthenticate(char *uri, char *basedn, char *passwd, bool starttls, const char **const errstr);
void *CfLDAPValue(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec);

int Nova2PHP_LDAPGetSeveralAttributes(char *uri, char *user, char *basedn, char *filter, Rlist *names, char *scopes,
                                      char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                      int bufsize, const char **const errstr);
int Nova2PHP_LDAPGetSingleAttributeList(char *uri, char *user, char *basedn, char *filter, char *name, char *scopes,
                                        char *sec, char *passwd, bool starttls, int page, int linesperpage,
                                        char *buffer, int bufsize, const char **const errstr);

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize);

int Nova2PHP_get_value_graph(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                             int bufsize);
int Nova2PHP_list_all_goals(char *buffer, int bufsize);

JsonElement *Nova2PHP_get_knowledge_view(int pid, char *view);
void Nova_FormatDiff(const char *diffStr, char *returnval, int bufsize);
JsonElement *Nova2PHP_network_speed(char *hostkey);
void Nova2PHP_GetPromiseBody(char *name, char *type, char *returnval, int bufsize);
int Nova2PHP_body_list(char *name, char *type, char *returnval, int bufsize);
char *Nova_LongState(char s);
int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz);
int Nova2PHP_docroot(char *buffer, int bufsize);
void Nova_EnterpriseModuleTrick(void);
void Nova2PHP_getlastupdate(char *hostkey, char *buffer, int bufsize);
void Nova2PHP_get_host_colour(char *hostkey, char *buffer, int bufsize);
JsonElement *Nova2PHP_promiselog_summary(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page);
int Nova2PHP_promiselog(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                        HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_value_report(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                          PageInfo *page, char *buffer, int bufsize);
void Nova2PHP_summary_meter(char *buffer, int bufsize);
void Nova2PHP_meter(char *hostkey, char *buffer, int bufsize);
int Nova2PHP_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize);
int Nova2PHP_software_report(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_classes_report(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                            char *returnval, int bufsize);
int Nova2PHP_classes_summary(char **classes, char *buf, int bufsize);
int Nova2PHP_countclasses(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, char *returnval,
                          int bufsize);
int Nova2PHP_vars_report(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex,
                         HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_promises(char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter,
                                 HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddr, time_t lago, int lregex,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_performance_report(char *hostkey, char *job, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                                char *returnval, int bufsize);
int Nova2PHP_setuid_report(char *hostkey, char *file, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                           char *returnval, int bufsize);
int Nova2PHP_bundle_report(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *host_colour_filter, bool lastRunOnly,
                           PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to,
                                HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg,
                            HostClassFilter *hostClassFilter, char *returnval, int bufsize);
int Nova2PHP_software_hosts(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                            HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_classes_hosts(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_vars_hosts(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex,
                        HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_hosts(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_promise_hosts(char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter,
                           bool lastRunOnly, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_lastseen_hosts(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                            HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_performance_hosts(char *hostkey, char *job, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                               int bufsize);
int Nova2PHP_setuid_hosts(char *hostkey, char *file, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                          int bufsize);
int Nova2PHP_bundle_hosts(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page,
                          char *returnval, int bufsize);
int Nova2PHP_filechanges_hosts(char *hostkey, char *file, bool regex, time_t from, time_t to,
                               HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_filediffs_hosts(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_promiselog_hosts(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_value_hosts(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                         PageInfo *page, char *returnval, int bufsize);

/*
 * Summary reports
 */
JsonElement *Nova2PHP_promise_compliance_summary (char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter);
JsonElement *Nova2PHP_bundle_compliance_summary (char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter);


JsonElement *Nova2PHP_search_topics(char *search, bool regex);
JsonElement *Nova2PHP_show_topic(int id);
JsonElement *Nova2PHP_show_topic_leads(int id);
JsonElement *Nova2PHP_show_all_context_leads(char *unqualified_topic);
JsonElement *Nova2PHP_show_topic_hits(int id);
JsonElement *Nova2PHP_show_topic_category(int id);
void Nova2PHP_host_compliance_list(mongo_connection *conn, char *colour, HostClassFilter *host_class_filter, PageInfo *page, char *buffer, int bufsize);
void Nova2PHP_host_compliance_list_all(mongo_connection *conn, HostClassFilter *host_class_filter, PageInfo *page, char *buffer, int bufsize);
int Nova2PHP_show_hosts(char *hostNameRegex, char *ipRegex, HostClassFilter *hostClassFilter, PageInfo *page, char *buf,
                        int bufsize);
void Nova2PHP_select_reports(char *reportId, char *buffer, int bufsize);

int Nova2PHP_get_bluehost_threshold(char *buffer, int bufsize);
int Nova2PHP_list_promise_handles_with_comments(char *bundle, char *btype, char *returnval, int bufsize);
int Nova2PHP_promise_list(PromiseFilter *promiseFilter, char *returnval, int bufsize, PageInfo *page);

int Nova2PHP_promise_details(PromiseFilter *filter, char *returnval, int bufsize);

char *Nova2PHP_GetPromiseComment(char *handle);
int Nova2PHP_bundle_classes_used(PromiseFilter *promiseFilter, char *buffer, int bufsize);
int Nova2PHP_bundle_agent_goals(PromiseFilter *filter, char *buffer, int bufsize);
int Nova2PHP_bundle_list_by_bundle_usage(PromiseFilter *promiseFilter, char *bNameReferenced, char *buffer,
                                         int bufsize);
void Nova2PHP_ComplianceSummaryGraph(char *policy, char *buffer, int bufsize);
int Nova2PHP_report_description(char *reportName, char *returnval, int bufsize);
void Nova2PHP_enterprise_version(char *buf, int bufsize);
int Nova2PHP_community_version(char *buf, int bufsize);

void Nova2PHP_GetLibraryDocuments(char *pth, char *buffer, int bufsize);
int Nova_AddReportHeader(char *header, int truncated, char *buffer, int bufsize);

/* svn helper functions */
int Nova2PHP_validate_policy(char *file, char *buffer, int bufsize);

/*
 * commenting
 */

char *Nova2PHP_get_help_text(char *key);

int Nova2PHP_add_note(char *noteid, char *keyhash, char *username, time_t datetime, char *note, char *returnval, int bufsize);
int Nova2PHP_get_notes(char *keyhash, char *cid, char *username, char *filter_username, time_t from, time_t to, PageInfo *page,
                       char *returnval, int bufsize);
int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize);

void Nova2PHP_cdp_reportnames(char *buf, int bufSz);
int Nova2PHP_cdp_report(char *hostkey, char *reportName, PageInfo *page, char *buf, int bufSz);
char *GetCdpTableHeader(cdp_report cdpType);
cdp_report CdpReportNameToType(char *reportName);

int Nova2PHP_delete_host(char *keyHash);

/*
 * replica set
 */
int Nova2PHP_replica_status(char *buffer, int bufsize);
int Nova2PHP_GetHubKey(char *buffer, int bufsize);
int Nova2PHP_GetHubMaster(char *buffer, int bufsize);

typedef struct EnvironmentsList_ EnvironmentsList;

struct EnvironmentsList_
{
    char *name;
    EnvironmentsList *next;
};

bool Nova2PHP_environment_list(EnvironmentsList **out, HostClassFilter *hostClassFilter);

typedef struct HostsList_ HostsList;

struct HostsList_
{
    char *keyhash;
    HostsList *next;
};

bool Nova2PHP_host_list_by_environment(HostsList **out, const char *environment, HostClassFilter *hostClassFilter);

char *Nova2PHP_environment_by_hostkey(const char *keyhash);

void FreeEnvironmentsList(EnvironmentsList *list);
void FreeHostsList(HostsList *list);

const char *GetErrorDescription(cfapi_errid errid);
char *FormatErrorJsonAttribute(char *out, int outSz, cfapi_errid errid);
char *FormatSingletonErrorJson(char *out, int outSz, cfapi_errid errid);
void EndJsonBuffer(char *buf, int bufsize, cfapi_errid errid);

/* For volume testing Mission portal */
#ifndef NDEBUG

int Nova2PHP_promise_list_test(PromiseFilter *promiseFilter, char *returnval, int bufsize);
int Nova2PHP_classes_report_test(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                                 char *returnval, int bufsize);
int Nova2PHP_classes_summary_test(char **classes, char *buf, int bufsize);
int Nova2PHP_show_hosts_test(char *hostNameRegex, char *ipRegex, char *classRegex, PageInfo *page, char *buf,
                             int bufsize);
int Nova2PHP_bundle_report_test(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter,
                                PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_promiselog_test(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
JsonElement *Nova2PHP_promiselog_summary_test(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                                     HostClassFilter *hostClassFilter, PageInfo *page);
int Nova2PHP_value_report_test(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                               PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_software_report_test(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                                  HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_vars_report_test(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_report_test(char *hostkey, char *version, time_t t, int k, int nk, int rep,
                                    HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_promises_test(char *hostkey, char *handle, char *status, bool regex,
                                      HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);



#endif // NDEBUG

#endif
