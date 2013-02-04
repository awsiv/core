/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_ENTERPRISE_INSTALL_H
#define CFENGINE_ENTERPRISE_INSTALL_H

#include "cf.nova.h"

const HubVitalPoint *HubVitalLastValue(const HubVital *vital);
int HubVitalPointCompare(const void *a, const void *b, void *user_data);

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
HubVariable *NewHubVariable(HubHost *hh, const char *type, const char *ns, const char *bundle, const char *lval, Rval rval, time_t t);
void DeleteHubVariable(HubVariable *hv);
HubPromiseLog *NewHubPromiseLog(HubHost *hh, char *handle, char *cause, time_t t);
unsigned int HubPromiseLogHash(const void *hp_record, unsigned int max);
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
HubBundleSeen *NewHubBundleSeen(HubHost *hh, char *ns, char *rname, double ago, double avg, double dev, time_t t);
void DeleteHubBundleSeen(HubBundleSeen *hp);
HubFileChanges *NewHubFileChanges(HubHost *hh, char *file, time_t t, char *handle, char change_type, char *change_msg);
void DeleteHubFileChanges(HubFileChanges *hp);
HubFileDiff *NewHubFileDiff(HubHost *hh, char *promise_handle, char *file, char *diff, time_t t);
void DeleteHubFileDiff(HubFileDiff *hp);
HubValue *NewHubValue(HubHost *hh, char *day, double kept, double repaired, double notkept, char *handle);
void DeleteHubValue(HubValue *hp);
HubPromise *NewHubPromise(const char *ns, char *bn, char *bt, Rlist *ba, char *pt, char *pr, Rlist *pe, char *cl, char *ha, char *co,
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
HubVital *PrependHubVital(HubVital **first, char *id, char *units, char *description, time_t last_update);
HubVital *NewHubVital(const char *hostkey, const char *id, const char *units, const char *description, time_t last_update);
void DeleteHubVital(HubVital *hv);
HubVitalPoint *NewHubVitalPoint(time_t t, double value);
void DeleteHubVitalPoint(HubVitalPoint *point);
HubUser *NewHubUser(bool external, const char *username, const char *name, const char *email, const Rlist *roles);
int HubUserCompare(const void *_hub_user_a, const void *_hub_user_b);
void DeleteHubUser(void *_hub_user);
HubUserRBAC *NewHubUserRBAC(const char *userName, const char *classRxInclude, const char *classRxExclude,
                            const char *bundleRxInclude, const char *bundleRxExclude);
int HubUserHash(const HubUser *user);
bool HubUserEqual(const HubUser *a, const HubUser *b);
void DeleteHubUserRBAC(HubUserRBAC *userRbac);
HubRole *NewHubRole(char *name, char *description,
                    char *classRxInclude, char *classRxExclude, char *bundleRxInclude, char *bundleRxExclude);
void DeleteHubRole(HubRole *role);
HubSettings *NewHubSettings(void);
HubSettings *NewHubSettingsDefaults(void);
void DeleteHubSettings(HubSettings *settings);
HubSettings *HubSettingsCopy(const HubSettings *settings);
HubSettings *HubSettingsUpdate(const HubSettings *existing_settings, const HubSettings *new_settings);
HubSettingsLDAPMode HubSettingsLDAPModeFromString(const char *ldap_mode);
const char *HubSettingsLDAPModeToString(HubSettingsLDAPMode mode);

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
void PromiseFilterAddNamespaces(PromiseFilter *filter, const char *namespaceInclude, const char *namespaceExclude);
void PromiseFilterAddNamespacesRx(PromiseFilter *filter, const char *namespaceRxInclude, const char *namespaceRxExclude);
void DeletePromiseFilter(PromiseFilter *filter);

HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount,
                                                    double kept, double repaired, double notkept, time_t genTime);
void DeleteHubCacheTotalCompliance(HubCacheTotalCompliance *tc);

HubScheduledReport *NewHubScheduledReport( const char *user, const char *email, const char *query_id, const char *query, const char *schedule, const char *title, const char *description);
void DeleteHubScheduledReport( HubScheduledReport *sr );

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
int SortPromiseBundle(void *p1, void *p2);

HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(Rlist *records, int slot);

bool FileRecordsEqual(const HubFileChanges *change_record, const HubFileDiff *diff_record);
int PageRecords(Rlist **records_p, PageInfo *page, void (*fnptr) ());

#endif
