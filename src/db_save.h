#ifndef CFENGINE_NOVA_DB_SAVE_H
#define CFENGINE_NOVA_DB_SAVE_H

#include "cf.nova.h"
#include "db_common.h"

#include <mongo.h>

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
void CFDB_SaveLastHostUpdate(mongo_connection *conn, char *hostKey);
void CFDB_SaveLastUpdate(mongo_connection *conn, char *database, char *keyField, char *keyhash);
void CFDB_SaveLastHostUpdateSize(mongo_connection *conn, char *hostkey, int update_size);

void CFDB_SaveCachedTotalCompliance(mongo_connection *conn, char *policy, int slot, double kept, double repaired,
                                    double notkept, int count, time_t genTime);
int CFDB_SaveLastseenCache(Item *lastseen);
void CFDB_SaveGoalsCache(char *goal_patterns, char *goal_categories);
int CFDB_MarkAsDeleted(mongo_connection *dbconn, char *keyHash);
void CFDB_SaveExecutionStatus(mongo_connection *conn, char *keyhash, bool is_black, long delta_schedule);
void CFDB_SaveLastAgentExecution(mongo_connection *conn, char *keyhash, long last_agent_exec);

#endif
