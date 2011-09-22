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

// Separation needed because it is included by php extension

/*
 * Returned error strings are statically allocated and do not need freeing.
 * NULL is placed to errstr if no error occured.
 */
int Nova2PHP_LDAPAuthenticate(char *uri,char *basedn,char *passwd, bool starttls, const char **const errstr);
void *CfLDAPValue(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec);

int Nova2PHP_LDAPGetSeveralAttributes(char *uri,char *user,char *basedn,char *filter,struct Rlist *names,char *scopes,char *sec,char *passwd,bool starttls,int page,int linesperpage,char *buffer, int bufsize, const char **const errstr);
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
int Nova2PHP_promiselog_summary(char *hostkey,char *handle,enum promiselog_rep type,time_t from, time_t to,char *classreg,struct PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_promiselog(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,struct PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_value_report(char *hostkey,char *day,char *month,char *year,char *classreg,struct PageInfo *page,char *buffer,int bufsize);
void Nova2PHP_summary_meter(char *buffer,int bufsize);
void Nova2PHP_meter(char *hostkey,char *buffer,int bufsize);
int Nova2PHP_hostinfo(char *hostkey,char *hostnameOut,char *ipaddrOut,int bufsize);
int Nova2PHP_software_report(char *key,char *name,char *value, char *arch,int regex,char *type,char *classreg,struct PageInfo *page,char *returnval,int bufsize);
int Nova2PHP_classes_report(char *hostkey,char *name,int regex,char *classreg,struct PageInfo *page,char *buffer,int bufsize);
int Nova2PHP_classes_summary(char **classes, char *buf, int bufsize);
int Nova2PHP_vars_report(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_compliance_report(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_compliance_promises(char *hostkey,char *handle,char *status,int regex,char *classreg,struct PageInfo *page,char *buffer,int bufsize);
int Nova2PHP_lastseen_report(char *hostkey,char *lhash,char *lhost,char *laddr,time_t lago,int lregex,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_performance_report(char *hostkey,char *job,int regex,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_setuid_report(char *hostkey,char *file,int regex,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_bundle_report(char *hostkey,char *bundle,int regex,char *classreg,struct PageInfo *page, char *returnval,int bufsize);
int Nova2PHP_filechanges_report(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,struct PageInfo *page, int lookInArchive, char *returnval,int bufsize);
int Nova2PHP_filediffs_report(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,struct PageInfo *page, int lookInArchive, char *returnval,int bufsize);
int Nova2PHP_summary_report(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize);

int Nova2PHP_software_hosts(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize);
int Nova2PHP_classes_hosts(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_vars_hosts(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_compliance_hosts(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize);
int Nova2PHP_promise_hosts(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_lastseen_hosts(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_performance_hosts(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_setuid_hosts(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_bundle_hosts(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_filechanges_hosts(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize);
int Nova2PHP_filediffs_hosts(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize);
int Nova2PHP_promiselog_hosts(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,char *returnval,int bufsize);
int Nova2PHP_value_hosts(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize);

int Nova2PHP_search_topics(char *search,int regex,char *buffer,int bufsize);
void Nova2PHP_show_topic(int id,char * buffer,int bufsize);
void Nova2PHP_show_topic_leads(int id,char *buffer,int bufsize);
void Nova2PHP_show_topic_hits(int id,char *buffer,int bufsize);
void Nova2PHP_show_topic_category(int id,char *buffer,int bufsize);
void Nova2PHP_show_topN(char *policy,int n,struct PageInfo *page,char *buffer,int bufsize);
void Nova2PHP_show_all_hosts(char *policy,int n,char *buffer,int bufsize);
int Nova2PHP_show_hosts(char *hostNameRegex,char *ipRegex,char *classRegex,struct PageInfo *page,char *buf,int bufsize);
void Nova2PHP_select_hosts(char *match,char *sel,int n,char *buffer,int bufsize);
void Nova2PHP_select_reports(char *buffer,int bufsize);

long Nova2PHP_count_hosts(void);
long Nova2PHP_count_blue_hosts(void);
long Nova2PHP_count_red_hosts(void);
long Nova2PHP_count_yellow_hosts(void);
long Nova2PHP_count_green_hosts(void);
void Nova2PHP_show_col_hosts(char *colour,int n,struct PageInfo *page,char *buffer,int bufsize);
int Nova2PHP_list_promise_handles(char *promiser,char *ptype,char *bundle,char *btype,int regex,char *returnval,int bufsize);
int Nova2PHP_list_promise_handles_with_comments(char *bundle,char *btype,char *returnval,int bufsize);
int Nova2PHP_list_handles_policy_finder(char *handle,char *promiser,char *bundle,int regex,char *returnval,int bufsize);

char *Nova_HostProfile(char *key);
void Nova2PHP_show_all_hosts(char *policy,int n,char *buffer,int bufsize);
int Nova2PHP_summarize_promise(char *handle, char *returnval,int bufsize);

char *Nova2PHP_GetPromiseComment(char *handle);
char *Nova2PHP_GetPromiseBundle(char *handle);
char *Nova2PHP_GetPromiseType(char *handle);
char *Nova2PHP_GetPromiser(char *handle);
int Nova2PHP_get_variable(char *hostkey,char *scope,char *lval,char *returnval,int bufsize);
int Nova2PHP_get_classes_for_bundle(char *name,char *type,char *buffer,int bufsize);
int Nova2PHP_get_args_for_bundle(char *name,char *type,char *buffer,int bufsize);
int Nova2PHP_list_all_bundles(char *type,char *buffer,int bufsize);
int Nova2PHP_get_bundle_type(char *name,char *buffer,int bufsize); 
int Nova2PHP_list_bundles_using(char *name,char *buffer,int bufsize);
int Nova2PHP_get_bundle_count(char *buffer, int bufsize);
int Nova2PHP_get_promise_count(char *buffer, int bufsize);
void Nova2PHP_ComplianceSummaryGraph(char *policy,char *buffer,int bufsize);
int Nova2PHP_report_description(char *reportName,char *returnval,int bufsize);
int Nova2PHP_enterprise_version(char *buf, int bufsize);
int Nova2PHP_community_version(char *buf, int bufsize);

void Nova2PHP_GetLibraryDocuments(char *pth,char *buffer,int bufsize);
int Nova_AddReportHeader(char *header,int truncated,char *buffer,int bufsize);

// constellation function stubs

void Con2PHP_ComplianceSummaryGraph(char *hubKeyHash, char *policy, char *buffer,int bufsize);
int Con2PHP_compliance_virtualbundle_graph(char *hubKeyHash, char *bundleName, char *buf, int bufsize);;
int Con2PHP_count_hubs(char *classification, char *buf, int bufsize);
int Con2PHP_show_hubs(char *classification, char *buf, int bufsize);
int Con2PHP_aggr_promiselog(char *hubKeyHash, enum promiselog_rep log_type, char *buf, int bufsize);
int Con2PHP_count_promiselog(char *hubKeyHash, char *promiseHandle, enum promiselog_rep log_type, char *buf, int bufsize);
int Con2PHP_reasons_promiselog(char *hubKeyHash, char *promiseHandle, enum promiselog_rep log_type, char *buf, int bufsize);

int Con2PHP_environments_list(char *hubKeyHash, char *buf, int bufsize);

int Con2PHP_promise_popularity(char *promiseHandle, char *buf, int bufsize);
int Con2PHP_rank_promise_popularity(bool sortAscending, char *buf, int bufsize);

int Con2PHP_aggr_filechange(char *hubKeyHash, char *filePath, char *buf, int bufsize);

int Con2PHP_list_subscriptions(char *user, char *buf, int bufsize);
int Con2PHP_subscribe_software(char *user, char *subHandle, char *hubClassRegex, char *pkgName, int pkgRegex, char *hostClassRegex, char *buf, int bufsize);
int Con2PHP_report_software(char *user, char *subHandle, char *hubClassRegex, struct PageInfo *page, char *buf, int bufsize);


/* pdf report */
int Nova2PHP_compliance_report_pdf(char *hostkey,char *version,time_t t,int k,int nk,int rep,char *cmp,char *classreg,char *returnval,int bufsize); 
int Nova2PHP_promiselog_pdf(char *hostkey,char *handle,enum promiselog_rep type,time_t from,time_t to,char *classreg,char *returnval,int bufsize);
int Nova2PHP_bundle_report_pdf(char *hostkey,char *bundle,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_value_report_pdf(char *hostkey,char *day,char *month,char *year,char *classreg,char *returnval,int bufsize);
int Nova2PHP_classes_report_pdf(char *hostkey,char *name,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_compliance_promises_pdf(char *hostkey,char *handle,char *status,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_filechanges_report_pdf(char *hostkey,char *file,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize);
int Nova2PHP_lastseen_report_pdf(char *hostkey,char *lhash,char *lhost,char *laddress,time_t lago,int lregex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_software_report_pdf(char *hostkey,char *name,char *value, char *arch,int regex,char *type,char *classreg,char *returnval,int bufsize);
int Nova2PHP_performance_report_pdf(char *hostkey,char *job,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_promiselog_summary_pdf(char *hostkey,char *handle,enum promiselog_rep type,time_t from, time_t to,char *classreg,char *returnval,int bufsize);
int Nova2PHP_vars_report_pdf(char *hostkey,char *scope,char *lval,char *rval,char *type,int regex,char *classreg,char *returnval,int bufsize);
int Nova2PHP_filediffs_report_pdf(char *hostkey,char *file,char *diffs,int regex,time_t t,char *cmp,char *classreg,char *returnval,int bufsize);
char *Nova_FormatDiff_pdf(char *s);
int Nova2PHP_setuid_report_pdf(char *hostkey,char *file,int regex,char *classreg,char *returnval,int bufsize);
/* end pdf report */

/* svn helper functions */
int Nova2PHP_validate_policy(char *file,char *buffer,int bufsize);

/*
 * commenting
 */

char *Nova2PHP_get_help_text(char *key);

int Nova2PHP_add_new_note(char *keyhash, char *repid, int reportType, char *username, time_t datetime, char *note,char *returnval, int bufsize);
int Nova2PHP_add_note(char *noteid,char *username, time_t datetime, char* note,char *returnval, int bufsize);
int Nova2PHP_get_notes(char *keyhash, char *cid, char *username, time_t from, time_t to,struct PageInfo *page, char *returnval, int bufsize);
int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize);

void Nova2PHP_cdp_reportnames(char *buf,int bufSz);
int Nova2PHP_cdp_report(char *hostkey, char *reportName, struct PageInfo *page, char *buf, int bufSz);
char *GetCdpTableHeader(cdp_t cdpType);
cdp_t CdpReportNameToType(char *reportName);


int Nova2PHP_delete_host(char *keyHash);

/*
 * replica set
 */
int Nova2PHP_replica_status(char *buffer, int bufsize);
int Nova2PHP_GetHubKey(char *buffer,int bufsize);
int Nova2PHP_GetHubMaster(char *buffer,int bufsize);

struct EnvironmentsList
{
    char *name;
    struct EnvironmentsList *next;
};

bool Nova2PHP_environments_list(struct EnvironmentsList **out);

struct HostsList
{
    char *keyhash;
    struct HostsList *next;
};

bool Nova2PHP_environment_contents(const char *environment,
                                   struct HostsList **out);

char *Nova2PHP_get_host_environment(const char *keyhash);

void FreeEnvironmentsList(struct EnvironmentsList *list);
void FreeHostsList(struct HostsList *list);

#endif // CFENGINE_NOVA_WEB_API_H
