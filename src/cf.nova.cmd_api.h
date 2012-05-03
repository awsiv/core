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

#ifndef CFENGINE_CF_NOVA_CMD_API_H
#define CFENGINE_CF_NOVA_CMD_API_H

#include <stdbool.h>

bool Nova2Txt_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize);
bool Nova2Txt_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize);

int Nova2Txt_list_all_goals(char *buffer, int bufsize);
void Nova2Txt_getlicense(char *buffer, int bufsize);

void Nova2Txt_get_knowledge_view(int pid, char *view, char *buffer, int bufsize);
char *Nova_FormatDiff(char *s);
void Nova2Txt_GetPromiseBody(char *name, char *type, char *returnval, int bufsize);
char *Nova_LongState(char s);
char *Nova_LongStateWarn(char s);
int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz);
int Nova2Txt_docroot(char *buffer, int bufsize);
void Nova_EnterpriseModuleTrick(void);
void Nova2Txt_get_host_colour(char *hostkey, char *buffer, int bufsize);
int Nova2Txt_promiselog(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to, char *classreg,
                        PageInfo *page, char *returnval, int bufsize);
int Nova2Txt_value_report(char *hostkey, char *day, char *month, char *year, char *classreg, PageInfo *page,
                          char *buffer, int bufsize);
void Nova2Txt_summary_meter(char *buffer, int bufsize);
void Nova2Txt_meter(char *hostkey, char *buffer, int bufsize);
int Nova2Txt_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize);
int Nova2Txt_software_report(char *key, char *name, char *value, char *arch, bool regex, char *type, char *classreg);
int Nova2Txt_classes_report(char *hostkey, char *name, bool regex, char *classreg);
int Nova2Txt_vars_report(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex, char *classreg);
int Nova2Txt_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               char *classreg);
int Nova2Txt_compliance_promises(char *hostkey, char *handle, char *status, bool regex, char *classreg);
int Nova2Txt_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddr, time_t lago, int lregex,
                             char *classreg);
int Nova2Txt_deadclient_report(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                               char *classreg);
int Nova2Txt_setuid_report(char *hostkey, char *file, bool regex, char *classreg);
int Nova2Txt_bundle_report(char *hostkey, char *bundle, bool regex, char *classreg, PageInfo *page, char *returnval,
                           int bufsize);
int Nova2Txt_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to, char *classreg);
int Nova2Txt_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to, char *classreg);

int Nova2Txt_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg);

int Nova2Txt_software_hosts(char *hostkey, char *name, char *value, char *arch, bool regex, char *type, char *classreg,
                            char *returnval, int bufsize);
int Nova2Txt_classes_hosts(char *hostkey, char *name, bool regex, char *classreg, char *returnval, int bufsize);
int Nova2Txt_vars_hosts(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex, char *classreg,
                        char *returnval, int bufsize);
int Nova2Txt_promise_hosts(char *hostkey, char *handle, char *status, bool regex, char *classreg, char *returnval,
                           int bufsize);
int Nova2Txt_lastseen_hosts(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                            char *classreg, char *returnval, int bufsize);
int Nova2Txt_performance_hosts(char *hostkey, char *job, bool regex, char *classreg, char *returnval, int bufsize);
int Nova2Txt_setuid_hosts(char *hostkey, char *file, bool regex, char *classreg, char *returnval, int bufsize);
int Nova2Txt_bundle_hosts(char *hostkey, char *bundle, bool regex, char *classreg, char *returnval, int bufsize);
int Nova2Txt_promiselog_hosts(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                              char *classreg, char *returnval, int bufsize);
int Nova2Txt_value_hosts(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                         int bufsize);

int Nova2Txt_search_topics(char *search, bool regex, char *buffer, int bufsize);
void Nova2Txt_show_topic(int id, char *buffer, int bufsize);
void Nova2Txt_show_topic_leads(int id, char *buffer, int bufsize);
void Nova2Txt_show_topic_hits(int id, char *buffer, int bufsize);
void Nova2Txt_show_topic_category(int id, char *buffer, int bufsize);
void Nova2Txt_select_reports(char *buffer, int bufsize);

int Nova2Txt_list_promise_handles_with_comments(char *bundle, char *btype, char *returnval, int bufsize);

char *Nova_HostProfile(char *key);

char *Nova2Txt_GetPromiseComment(char *handle);
int Nova2Txt_get_bundle_count(char *buffer, int bufsize);
int Nova2Txt_get_promise_count(char *buffer, int bufsize);
void Nova2Txt_ComplianceSummaryGraph(char *policy, char *buffer, int bufsize);
int Nova2Txt_report_description(char *reportName, char *returnval, int bufsize);
int Nova2Txt_enterprise_version(char *buf, int bufsize);
int Nova2Txt_community_version(char *buf, int bufsize);

void Nova2Txt_GetLibraryDocuments(char *pth, char *buffer, int bufsize);
int Nova_AddReportHeader(char *header, int truncated, char *buffer, int bufsize);

/* svn helper functions */
int Nova2Txt_validate_policy(char *file, char *buffer, int bufsize);

/*
 * commenting
 */

#endif
