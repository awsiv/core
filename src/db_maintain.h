#ifndef CFENGINE_NOVA_DB_MAINTAIN_H
#define CFENGINE_NOVA_DB_MAINTAIN_H

void CFDB_Maintenance(void);
void CFDB_ReIndexAll(void);
void CFDB_ConnectAndEnsureIndices(void);

#ifdef HAVE_LIBMONGOC
void CFDB_EnsureIndices(mongo_connection *conn);
void CFDB_PurgeDropReports(mongo_connection *conn);
void CFDB_PurgeTimestampedReports(mongo_connection *conn);
void CFDB_PurgeTimestampedLongtermReports(mongo_connection *conn);
void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now);
void CFDB_PurgePromiseLogsFromMain(mongo_connection *conn, char *promiseLogReportKey, time_t oldThreshold, time_t now);
void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                    Item **purgeKeysPtr, Item **purgeNamesPtr);
int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, Item **purgeKeysPtr,
                  Item **purgeNamesPtr);
void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                           Item **purgeKeysPtr);
void DeleteFromBsonArray(bson_buffer *bb, char *arrName, Item *elements);
void CFDB_PurgeHost(mongo_connection *conn, char *keyHash);
void CFDB_PurgeDeprecatedVitals(mongo_connection *conn);

void CFDB_RemoveTestData(char *db, char *keyhash);
int CFDB_PurgeDeletedHosts(void);

bool CFDB_CollectionHasData(mongo_connection *conn, const char *fullCollectionName);

#endif

#endif
