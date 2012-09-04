#ifndef CFENGINE_NOVA_DB_MAINTAIN_H
#define CFENGINE_NOVA_DB_MAINTAIN_H

void CFDB_Maintenance(mongo_connection *dbconn);
void CFDB_ReIndexAll(void);
void CFDB_ConnectAndEnsureIndices(void);
void CFDB_EnsureIndices(mongo_connection *conn);
void CFDB_PurgeDropReports(mongo_connection *conn);
void CFDB_PurgeTimestampedReports( mongo_connection *conn, const char *hostkey );
void CFDB_PurgeTimestampedLongtermReports( mongo_connection *conn, const char *hostkey );
void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now);
void CFDB_PurgePromiseLogsFromMain(mongo_connection *conn, const char *hostkey, char *promiseLogReportKey, time_t oldThreshold, time_t now);
void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                    Item **purgeKeysPtr, Item **purgeNamesPtr);
int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, Item **purgeKeysPtr,
                  Item **purgeNamesPtr);
void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                           Item **purgeKeysPtr);
void CFDB_PurgeHost(mongo_connection *conn, char *keyHash);
void CFDB_PurgeDeprecatedVitals(mongo_connection *conn);

void CFDB_RemoveTestData(char *db, char *keyhash);
int CFDB_PurgeDeletedHosts(void);

bool CFDB_CollectionHasData(mongo_connection *conn, const char *fullCollectionName);

/*
  Reads the total-compliance subrecord for a host for the last shift interval, and adds
  the aggregated result in the compliance-shifts subrecord.
*/
void CFDB_RefreshLastHostComplianceShift(mongo_connection *conn, const char *hostkey);

#endif
