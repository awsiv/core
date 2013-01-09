#ifndef CFENGINE_NOVA_DB_SAVE_H
#define CFENGINE_NOVA_DB_SAVE_H

#include "cf.nova.h"
#include "db_common.h"

#include <mongo.h>

bool CFDB_PutValue(EnterpriseDB *conn, const char *lval, const char *rval, const char *db_name);

void CFDB_SaveSoftware(EnterpriseDB *conn, enum software_rep sw, char *kH, Item *data);

//void CFDB_SaveMonitorData(EnterpriseDB *conn, char *kH, enum monitord_rep rep_type, Item *data);
void CFDB_SaveMonitorData2(EnterpriseDB *conn, char *keyHash, enum monitord_rep rep_type, Item *data);
void CFDB_SaveMonitorHistograms(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveClasses(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveVariables(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveVariables2(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveTotalCompliance(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SavePromiseLog(EnterpriseDB *conn, char *kH, PromiseLogState state, Item *data);
void CFDB_SaveLastSeen(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveMeter(EnterpriseDB *conn, char *kH, Item *data);

void CFDB_SaveScore(EnterpriseDB *conn, char *kH, Item *data, HostRankMethod method);
void CFDB_SaveSoftwareDates(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SavePerformance(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveSetUid(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SavePromiseCompliance(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveFileChangesOld(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveFileChanges(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveFileDiffs(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveBundles(EnterpriseDB *conn, char *kH, const Item *data);
void CFDB_SaveValueReport(EnterpriseDB *conn, char *kH, Item *data);
void CFDB_SaveHostID(EnterpriseDB *conn, char *database, char *keyField, char *keyhash, char *ipaddr,
                     char *hostname);
void Nova_CheckGlobalKnowledgeClass(char *name, char *key);
void CFDB_SaveLastHostUpdate(EnterpriseDB *conn, char *hostKey);
void CFDB_SaveLastUpdate(EnterpriseDB *conn, char *database, char *keyField, char *keyhash);
void CFDB_SaveLastHostUpdateSize(EnterpriseDB *conn, char *hostkey, int update_size);

void CFDB_SaveHostComplianceShift(EnterpriseDB *conn, const char *hostkey, int kept, int repaired,
                                  int notkept, int num_samples, time_t shift_start,
                                  PromiseContextMode promise_context_mode);

void CFDB_SaveLicense(EnterpriseDB *conn, time_t expires, time_t install_time, const char *owner, size_t num_granted, char *license_type_str);
void CFDB_SaveLicenseNumberPromised(EnterpriseDB *conn, size_t num_promised);
void CFDB_SaveLicenseUsage(EnterpriseDB *conn, time_t last_measured, size_t num_samples, size_t min_observed_level,
                           size_t max_observed_level, double mean_usage, double mean_utilization_cumulative,
                           size_t num_used_today);

void CFDB_SaveScheduledReport(EnterpriseDB *conn, const char *user, const char *email,
                              const char *scheduled_query_id, const char *scheduled_query,
                              const char *schedule, const bool enabled, const int report_output_type, const char *title, const char *description );

void CFDB_RemoveScheduledReport(EnterpriseDB *conn, const char *user, const char *scheduled_query_id);


// TODO: deprecate
void CFDB_SaveCachedTotalCompliance(EnterpriseDB *conn, char *policy, int slot, double kept, double repaired,
                                    double notkept, int count, time_t genTime,
                                    PromiseContextMode promise_context_mode);

int CFDB_SaveLastseenCache(Item *lastseen);
void CFDB_SaveGoalsCache(char *goal_patterns);
int CFDB_MarkAsDeleted(EnterpriseDB *dbconn, const char *keyHash);
void CFDB_SaveExecutionStatus(EnterpriseDB *conn, char *keyhash, bool is_black);
void CFDB_SaveLastAgentExecution(EnterpriseDB *conn, char *keyhash, long last_agent_exec);
void CFDB_SaveDeltaAgentExecution(EnterpriseDB *conn, char *keyhash, long delta);

/**
  * Pre-condition: settings are validated for integrity
  */
cfapi_errid CFDB_SaveSettings(EnterpriseDB *conn, const HubSettings *settings);

int CFDB_AddNote(EnterpriseDB *conn, char *keyhash, int reportType, char *nid,
                 char *reportData, char *username, long datetime, char *msg);
#endif
