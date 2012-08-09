#ifndef CFENGINE_NOVA_DB_MAINTAIN_H
#define CFENGINE_NOVA_DB_MAINTAIN_H

void CFDB_Maintenance(EnterpriseDB *dbconn);
void CFDB_ReIndexAll(void);
void CFDB_ConnectAndEnsureIndices(void);
void CFDB_EnsureIndices(EnterpriseDB *conn);
void CFDB_PurgeDropReports(EnterpriseDB *conn);
void CFDB_PurgeTimestampedReports(EnterpriseDB *conn);
void CFDB_PurgeTimestampedLongtermReports(EnterpriseDB *conn);
void CFDB_PurgePromiseLogs(EnterpriseDB *conn, time_t oldThreshold, time_t now);
void CFDB_PurgePromiseLogsFromMain(EnterpriseDB *conn, char *promiseLogReportKey, time_t oldThreshold, time_t now);
void CFDB_PurgeScan(EnterpriseDB *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                    Item **purgeKeysPtr, Item **purgeNamesPtr);
int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, Item **purgeKeysPtr,
                  Item **purgeNamesPtr);
void CFDB_PurgeScanStrTime(EnterpriseDB *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                           Item **purgeKeysPtr);
void CFDB_PurgeHost(EnterpriseDB *conn, char *keyHash);
void CFDB_PurgeDeprecatedVitals(EnterpriseDB *conn);

void CFDB_RemoveTestData(char *db, char *keyhash);
int CFDB_PurgeDeletedHosts(void);

bool CFDB_CollectionHasData(EnterpriseDB *conn, const char *fullCollectionName);

/*
  Reads the total-compliance subrecord for a host for the last shift interval, and adds
  the aggregated result in the compliance-shifts subrecord.
*/
void CFDB_RefreshLastHostComplianceShift(EnterpriseDB *conn, const char *hostkey);

#endif
