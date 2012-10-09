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
// these error codes are also used in PHP-land, be careful with changes
typedef enum cfapi_errid
{
    ERRID_UNKNOWN = 0,
    ERRID_SUCCESS = 1,
    ERRID_DBCONNECT = 2,
    ERRID_DBCLOSE = 3,
    ERRID_DB_OPERATION = 4,
    ERRID_ARGUMENT_MISSING = 5,
    ERRID_ARGUMENT_WRONG = 6,
    ERRID_BUFFER_FULL = 7,
    ERRID_ITEM_NONEXISTING = 8,
    ERRID_ITEM_EXISTS = 9,
    ERRID_ITEM_MULTIPLE = 10,
    ERRID_RBAC_DISABLED = 11,
    ERRID_RBAC_ACCESS_DENIED = 12,
    ERRID_DATA_UNAVAILABLE = 13,
    ERRID_HOST_NOT_FOUND = 14,
    ERRID_ACCESS_DENIED = 15,
    ERRID_ACCESS_DENIED_EXTERNAL = 16,
    ERRID_MAX
} cfapi_errid;

/* reports */

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
    Rlist *namespaceIncludes;
    Rlist *namespaceExcludes;
    Rlist *namespaceRxIncludes;
    Rlist *namespaceRxExcludes;
} PromiseFilter;

/* misc */

extern int AM_PHP_MODULE;

// Separation needed because it is included by php extension

/*
 * These functions are apparently used by PHP module
 */
#ifdef HAVE_LIBMONGOC
bool CFDB_GetValue( EnterpriseDB *conn, char *lval, char *rval, int size, char *db_name );
bool CFDB_PutValue(EnterpriseDB *conn, const char *lval, const char *rval, const char *db_name);
int Nova_GetTopicIdForTopic(char *typed_topic);
#endif

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2PHP_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize);

void Con2PHP_get_story_by_id(int id,char *buffer,int bufsize);
void Con2PHP_get_story_by_name(char *typed_topic,char *buffer,int bufsize);

int Nova2PHP_get_value_graph(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                             int bufsize);
int Nova2PHP_list_all_goals(char *buffer, int bufsize);
JsonElement *Nova2PHP_list_services(void);
JsonElement *Nova2PHP_list_service_ports(void);
JsonElement *Nova2PHP_list_promises_with_promisee(char *name);
JsonElement *Nova2PHP_list_types_in_bundle(char *name);
JsonElement *Nova2PHP_get_open_port_histograms(void);
JsonElement *Nova2PHP_get_service_histogram(void);
JsonElement *Nova2PHP_get_service_level_histogram(char *srv);
JsonElement *Nova2PHP_get_open_port_histograms(void);
JsonElement *Nova2PHP_get_goal_progress(char *handle);
JsonElement *Nova2PHP_get_knowledge_view(int pid, char *view);
JsonElement *Nova_FormatDiff(const char *diffStr);
JsonElement *Nova2PHP_network_speed(char *hostkey);
void Nova2PHP_GetPromiseBody(char *name, char *type, char *returnval, int bufsize);
int Nova2PHP_body_list(char *name, char *type, char *returnval, int bufsize);
char *Nova_LongState(char s);
int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz);
void Nova_EnterpriseModuleTrick(void);
void Nova2PHP_getlastupdate(char *hostkey, char *buffer, int bufsize);
void Nova2PHP_get_host_colour(char *hostkey, char *buffer, int bufsize, PromiseContextMode promise_context);
JsonElement *Nova2PHP_promiselog_summary(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_promiselog(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                        HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_value_report(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                                   PageInfo *page);
void Nova2PHP_summary_meter(char *buffer, int bufsize);
void Nova2PHP_meter(char *hostkey, char *buffer, int bufsize, PromiseContextMode promise_context);
int Nova2PHP_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize);
JsonElement *Nova2PHP_software_report(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                             HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_classes_report(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                            time_t from, time_t to, PromiseContextMode promise_context);
JsonElement *Nova2PHP_classes_summary(char **classes);
int Nova2PHP_countclasses(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, char *returnval,
                          int bufsize, PromiseContextMode promise_context);
JsonElement *Nova2PHP_vars_report(const char *hostkey, const char *scope, const char *lval, const char *rval, const char *type,
                         bool regex, const HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_compliance_promises(char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter,
                                 HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddr, time_t lago, int lregex,
                             HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_performance_report(char *hostkey, char *job, bool regex, HostClassFilter *hostClassFilter, PageInfo *page);
JsonElement *Nova2PHP_setuid_report(char *hostkey, char *file, bool regex, HostClassFilter *hostClassFilter, PageInfo *page);
JsonElement *Nova2PHP_bundle_report(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *host_colour_filter, bool lastRunOnly,
                           PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to,
                                HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
JsonElement *Nova2PHP_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                              HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context);
int Nova2PHP_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg,
                            HostClassFilter *hostClassFilter, char *returnval, int bufsize);

/*
 * Host only reports
 */
JsonElement *Nova2PHP_software_hosts(char *hostkey, char *name, char *value,
                                     char *arch, bool regex, char *type,
                                     HostClassFilter *hostClassFilter, PageInfo *page,
                                     PromiseContextMode promise_context);
JsonElement *Nova2PHP_classes_hosts(char *hostkey, char *name, bool regex,
                                    HostClassFilter *hostClassFilter, PageInfo *page, time_t from, time_t to, PromiseContextMode promise_conetext);
JsonElement *Nova2PHP_vars_hosts(char *hostkey, char *scope, char *lval,
                                 char *rval, char *type, bool regex,
                                 HostClassFilter *hostClassFilter, PageInfo *page,
                                 PromiseContextMode promise_context);
JsonElement *Nova2PHP_compliance_hosts(char *hostkey, char *version, time_t from,
                                       time_t to, int k, int nk, int rep,
                                       HostClassFilter *hostClassFilter, PageInfo *page,
                                       PromiseContextMode promise_context);
JsonElement *Nova2PHP_promise_hosts(char *hostkey, char *handle, char *status,
                                    bool regex, HostClassFilter *hostClassFilter,
                                    HostColourFilter *hostColourFilter,
                                    bool lastRunOnly, PageInfo *page,
                                    PromiseContextMode promise_context);
JsonElement *Nova2PHP_lastseen_hosts(char *hostkey, char *lhash, char *lhost,
                                     char *laddress, time_t lago, int lregex,
                                     HostClassFilter *hostClassFilter, PageInfo *page,
                                     PromiseContextMode promise_context);
JsonElement *Nova2PHP_performance_hosts(char *hostkey, char *job, bool regex,
                                        HostClassFilter *hostClassFilter, PageInfo *page);
JsonElement *Nova2PHP_setuid_hosts(char *hostkey, char *file, bool regex,
                                   HostClassFilter *hostClassFilter, PageInfo *page);
JsonElement *Nova2PHP_bundle_hosts(char *hostkey, char *bundle, bool regex,
                                   HostClassFilter *hostClassFilter,
                                   HostColourFilter *hostColourFilter,
                                   bool lastRunOnly, PageInfo *page,
                                   PromiseContextMode promise_context);
JsonElement *Nova2PHP_filechanges_hosts(char *hostkey, char *file, bool regex,
                                        time_t from, time_t to,
                                        HostClassFilter *hostClassFilter, PageInfo *page,
                                        PromiseContextMode promise_context);
JsonElement *Nova2PHP_filediffs_hosts(char *hostkey, char *file, char *diffs,
                                      bool regex, time_t from, time_t to,
                                      HostClassFilter *hostClassFilter, PageInfo *page,
                                      PromiseContextMode promise_context);
JsonElement *Nova2PHP_promiselog_hosts(char *hostkey, char *handle, char *cause,
                                       PromiseLogState state, time_t from, time_t to,
                                       HostClassFilter *hostClassFilter, PageInfo *page,
                                       PromiseContextMode promise_context);
JsonElement *Nova2PHP_value_hosts(char *hostkey, char *day, char *month, char *year,
                                  HostClassFilter *hostClassFilter, PageInfo *page);

/*
 * Summary reports
 */
JsonElement *Nova2PHP_promise_compliance_summary (char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter, PromiseContextMode promise_context);
JsonElement *Nova2PHP_bundle_compliance_summary (char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, PromiseContextMode promise_context);

JsonElement *Nova2PHP_list_topics_for_bundle(char *name);
JsonElement *Nova2PHP_list_knowledge_bundles(void);
void Nova2PHP_bundle_for_topic(int topic_id, char *buffer, int bufsize);
JsonElement *Nova2PHP_search_topics(char *search, bool regex);
Item *Nova2PHP_search(char *search, bool regex, char *user);
JsonElement *Nova2PHP_show_topic(int id);
JsonElement *Nova2PHP_show_topic_leads(int id);
JsonElement *Nova2PHP_show_all_context_leads(char *unqualified_topic);
JsonElement *Nova2PHP_show_topic_hits(int id);
JsonElement *Nova2PHP_show_topic_category(int id);
void Nova2PHP_host_compliance_list(EnterpriseDB *conn, char *colour, HostClassFilter *host_class_filter, PageInfo *page, char *buffer, int bufsize, PromiseContextMode promise_context);
void Nova2PHP_host_compliance_list_all(EnterpriseDB *conn, HostClassFilter *host_class_filter, PageInfo *page, char *buffer, int bufsize, PromiseContextMode promise_context);
int Nova2PHP_show_hosts(char *hostNameRegex, char *ipRegex, HostClassFilter *hostClassFilter, PageInfo *page, char *buf,
                        int bufsize);
void Nova2PHP_select_reports(char *reportId, char *buffer, int bufsize);

int Nova2PHP_get_bluehost_threshold(char *buffer, int bufsize);
int Nova2PHP_list_promise_handles_with_comments(char *bundle, char *btype, char *returnval, int bufsize);
int Nova2PHP_promise_list(PromiseFilter *promiseFilter, char *returnval, int bufsize, PageInfo *page, PromiseContextMode promise_context);

int Nova2PHP_promise_details(PromiseFilter *filter, char *returnval, int bufsize);

char *Nova2PHP_GetPromiseComment(char *handle);
JsonElement *Nova2PHP_bundle_classes_used(const PromiseFilter *promiseFilter);
int Nova2PHP_bundle_agent_goals(PromiseFilter *filter, char *buffer, int bufsize);
int Nova2PHP_bundle_list_by_bundle_usage(PromiseFilter *promiseFilter, char *bNameReferenced, char *buffer,
                                         int bufsize);
void Nova2PHP_ComplianceSummaryGraph(char *policy, char *buffer, int bufsize);
int Nova2PHP_report_description(char *reportName, char *returnval, int bufsize);

void Nova2PHP_GetLibraryDocuments(char *pth, char *buffer, int bufsize);
int Nova_AddReportHeader(char *header, int truncated, char *buffer, int bufsize);

/* svn helper functions */
int Nova2PHP_validate_policy(char *file, char *buffer, int bufsize);

/*
 * commenting
 */

int Nova2PHP_add_note(char *noteid, char *keyhash, char *username, time_t datetime, char *note, char *returnval, int bufsize);
int Nova2PHP_get_notes(char *keyhash, char *cid, char *username, char *filter_username, time_t from, time_t to, PageInfo *page,
                       char *returnval, int bufsize);
int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize);

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
int Nova2PHP_vars_report_test(const char *hostkey, const char *scope, const char *lval, const char *rval, const char *type, bool regex,
                              const HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_report_test(char *hostkey, char *version, time_t t, int k, int nk, int rep,
                                    HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_compliance_promises_test(char *hostkey, char *handle, char *status, bool regex,
                                      HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize);



#endif // NDEBUG

#endif
