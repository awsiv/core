/*****************************************************************************/
/*                                                                           */
/* File: db_maintain.c                                                       */
/*                                                                           */
/* Created: Wed Sept  29 14:47:33 2010                                       */
/*                                                                           */
/* MongoDB implementation of database maintenance functions                  */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "db_maintain.h"

#include "db_query.h"
#include "db_save.h"
#include "bson_lib.h"
#include "granules.h"
#include "item_lib.h"
#include "conversion.h"

#include <assert.h>

static void CFDB_DropAllIndices(EnterpriseDB *conn);
static void PurgePromiseLogWithEmptyTimestamps(EnterpriseDB *conn, const char *hostkey, char *promiseLogKey);
static Item *GetUniquePromiseLogEntryKeys(EnterpriseDB *conn, const char *hostkey, char *promiseLogKey);

// WHAT: CFDB_PurgeSoftwareInvalidTimestamp
//       removes software reports from all hosts with 0 timestamp
//
// WHY:  In Nova v2.1 there was a bug which populated timestamp = 0
//       for software installed report
//       As of Nova 2.2, the bug no longer exists and
//       this function can be removed in future releases
//       when all hubs have been upgraded to 2.3 for instance

static void CFDB_PurgeSoftwareInvalidTimestamp(EnterpriseDB *conn);

static void CFDB_PurgeHostReports(EnterpriseDB *dbconn, const char *hostkey);

/*****************************************************************************/

void CFDB_Maintenance(EnterpriseDB *dbconn)
{
    struct timespec maintenance_start = BeginMeasure();

    CFDB_EnsureIndices(dbconn);

    Item *hosts = CFDB_GetAllHostKeys(dbconn);

    struct timespec purge_hosts_start = BeginMeasure();

    for(Item *ip = hosts; ip != NULL; ip = ip->next)
    {
        CFDB_PurgeHostReports(dbconn, ip->name);
    }

    EndMeasure("DBPurgeHostsAll", purge_hosts_start);

    DeleteItemList(hosts);

    // support for old DB PromiseLogs format
    CFDB_PurgePromiseLogs(dbconn, CF_HUB_PURGESECS, time(NULL));

    CFDB_PurgeSoftwareInvalidTimestamp(dbconn);
    CFDB_PurgeDeprecatedVitals(dbconn);

    EndMeasure("DBMaintenance", maintenance_start);
}

/*****************************************************************************/

static void CFDB_PurgeHostReports(EnterpriseDB *dbconn, const char *hostkey)
{
    CFDB_PurgeTimestampedReports(dbconn, hostkey);

    CFDB_PurgePromiseLogsFromMain(dbconn, hostkey, MONGO_LOGS_NOTKEPT_COLL, CF_HUB_PURGESECS, time(NULL));
    CFDB_PurgePromiseLogsFromMain(dbconn, hostkey, MONGO_LOGS_REPAIRED_COLL, CF_HUB_PURGESECS, time(NULL));

    CFDB_PurgeTimestampedLongtermReports(dbconn, hostkey);
}

/*****************************************************************************/

void CFDB_ReIndexAll(void)
/**
 *  WARNING: May take a long time if there is much data.
 *           Should only be called when indices have changed,
 *           e.g. due to Nova upgrade.
 */
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    CFDB_DropAllIndices(&dbconn);
    CFDB_EnsureIndices(&dbconn);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_ConnectAndEnsureIndices(void)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    CFDB_EnsureIndices(&dbconn);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_EnsureIndices(EnterpriseDB *conn)
/**
 *  Makes sure certain keys have an index to optimize querying and updating.
 **/
{
    CfOut(cf_verbose, "", "Ensuring database indices are in place");

    // main host collection

    bson b;
    BsonSelectReportFields(&b, 1, cfr_keyhash);

    if (mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE);
    }

    if (mongo_create_index(conn, MONGO_ARCHIVE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_ARCHIVE);
    }

    bson_destroy(&b);

    BsonSelectReportFields(&b, 1, cfr_class_keys);

    if (mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE);
    }

    if (mongo_create_index(conn, MONGO_ARCHIVE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_ARCHIVE);
    }

    bson_destroy(&b);

    BsonSelectReportFields(&b, 1, cfr_time);

    if (mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE);
    }

    if (mongo_create_index(conn, MONGO_ARCHIVE, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_ARCHIVE);
    }

    bson_destroy(&b);

    // monitoring collections

    BsonSelectReportFields(&b, 2, cfr_keyhash, cfm_id);

    if (mongo_create_index(conn, MONGO_DATABASE_MON_MG, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE_MON_MG);
    }

    if (mongo_create_index(conn, MONGO_DATABASE_MON_WK, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE_MON_WK);
    }

    if (mongo_create_index(conn, MONGO_DATABASE_MON_YR, &b, 0, NULL) != MONGO_OK)
    {
        CfOut(cf_error, "", "!! Could not create index on %s", MONGO_DATABASE_MON_YR);
    }

    bson_destroy(&b);
}

/*****************************************************************************/

static void CFDB_DropAllIndices(EnterpriseDB *conn)
{
    char *indexedCollections[] = { MONGO_HOSTS_COLLECTION,
        MONGO_LOGS_REPAIRED_COLL,
        MONGO_LOGS_NOTKEPT_COLL,
        MONGO_MON_MG_COLLECTION,
        MONGO_MON_WK_COLLECTION,
        MONGO_MON_YR_COLLECTION,
        MONGO_ARCHIVE_COLLECTION,
        NULL
    };

    int i;

    for (i = 0; indexedCollections[i] != NULL; i++)
    {
        char *collection = indexedCollections[i];

        bson dropAllCommand;
        bson_init(&dropAllCommand);
        bson_append_string(&dropAllCommand, "dropIndexes", collection);
        bson_append_string(&dropAllCommand, "index", "*");
        BsonFinish(&dropAllCommand);

        bson result;

        if (mongo_run_command(conn, MONGO_BASE, &dropAllCommand, bson_empty(&result)) != MONGO_OK)
        {
            CfOut(cf_error, "", "mongo_run_command: Could not drop index on collection %s", collection);
        }
        else
        {
            bson_destroy(&result);
        }

        bson_destroy(&dropAllCommand);
    }

}

/*****************************************************************************/

// TODO: looks like something pre BSON-lib era
static void DeleteFromBsonArray(bson *bb, char *arrName, Item *elements)
{
    Item *ip = NULL;
    char iStr[64];
    int i;

    if (!elements)
    {
        return;
    }

    {
        bson_append_start_object(bb, "$pullAll");

        {
            bson_append_start_array(bb, arrName);

            for (ip = elements, i = 0; ip != NULL; ip = ip->next, i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(bb, iStr, ip->name);
            }

            bson_append_finish_object(bb);
        }

        bson_append_finish_object(bb);
    }
}

void CFDB_PurgeTimestampedReports(EnterpriseDB *conn, const char *hostkey)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
    Item *purgeKeys = NULL, *ip;
    Item *purgePcNames = NULL, *purgeClassNames = NULL;
    mongo_cursor *cursor;
    bson query, field;
    bson_iterator it1;
    char keyHash[CF_MAXVARSIZE];
    time_t now;

    struct timespec maintenance_timestamp_start = BeginMeasure();

    CfOut(cf_verbose, "", " -> Purge timestamped reports (keyhash = %s)", hostkey);

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    // only retrieve the purgable reports

    BsonSelectReportFields(&field, 10,
                           cfr_keyhash,
                           cfr_class,
                           cfr_vars,
                           cfr_performance,
                           cfr_filechanges,
                           cfr_filediffs,
                           cfr_promisecompl,
                           cfr_lastseen,
                           cfr_bundles,
                           cfr_valuereport);

    cursor = MongoFind(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    now = time(NULL);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        memset(keyHash, 0, sizeof(keyHash));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {

            if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
            {
                snprintf(keyHash, sizeof(keyHash), "%s", bson_iterator_string(&it1));
            }

            CFDB_PurgeScan(conn, &it1, cfr_class, CF_HUB_PURGESECS, now, &purgeKeys, &purgeClassNames);
            CFDB_PurgeScan(conn, &it1, cfr_vars, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_performance, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_filechanges, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_filediffs, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_promisecompl, CF_HUB_PURGESECS, now, &purgeKeys, &purgePcNames);
            CFDB_PurgeScan(conn, &it1, cfr_lastseen, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_bundles, CF_HUB_PURGESECS, now, &purgeKeys, NULL);
            CFDB_PurgeScanStrTime(conn, &it1, cfr_valuereport, CF_HUB_PURGESECS, now, &purgeKeys);
        }

        bson hostQuery;
        bson_init(&hostQuery);
        bson_append_string(&hostQuery, cfr_keyhash, keyHash);
        BsonFinish(&hostQuery);

        // keys
        bson op;
        bson_init(&op);
        {
            bson_append_start_object(&op, "$unset");

            for (ip = purgeKeys; ip != NULL; ip = ip->next)
            {
                bson_append_int(&op, ip->name, 1);
            }

            bson_append_finish_object(&op);
        }

        // key array elements
        DeleteFromBsonArray(&op, cfr_class_keys, purgeClassNames);
        DeleteFromBsonArray(&op, cfr_promisecompl_keys, purgePcNames);

        BsonFinish(&op);

        MongoUpdate(conn, MONGO_DATABASE, &hostQuery, &op, 0, NULL); /*TODO: Use mongo write concern instead of MongoGetLastError*/

        DeleteItemList(purgeClassNames);
        purgeClassNames = NULL;

        DeleteItemList(purgePcNames);
        purgePcNames = NULL;

        DeleteItemList(purgeKeys);
        purgeKeys = NULL;

        bson_destroy(&hostQuery);
        bson_destroy(&op);

        MongoCheckForError(conn, "PurgeTimestampedReports", keyHash, NULL);
    }

    mongo_cursor_destroy(cursor);

    EndMeasure("DBMaintenanceTimestampsSingleHost", maintenance_timestamp_start);
}

/*****************************************************************************/

void CFDB_PurgeTimestampedLongtermReports(EnterpriseDB *conn, const char *hostkey)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
    Item *purgeKeys = NULL, *ip;
    Item *purgePcNames = NULL, *purgeClassNames = NULL;
    mongo_cursor *cursor;
    bson query, field;
    bson_iterator it1;
    char keyHash[CF_MAXVARSIZE];
    time_t now;

    long threshold = 365 * 24 * 3600;

    CfOut(cf_verbose, "", " -> Purge longterm reports (keyhash = %s)", hostkey);

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    // only retrieve the purgable reports
    BsonSelectReportFields(&field, 3, cfr_keyhash, cfr_filechanges, cfr_filediffs);

    cursor = MongoFind(conn, MONGO_ARCHIVE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    now = time(NULL);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        memset(keyHash, 0, sizeof(keyHash));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {

            if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
            {
                snprintf(keyHash, sizeof(keyHash), "%s", bson_iterator_string(&it1));
            }

            CFDB_PurgeScan(conn, &it1, cfr_filechanges, threshold, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_filediffs, threshold, now, &purgeKeys, NULL);
        }

        bson hostQuery;
        bson_init(&hostQuery);
        bson_append_string(&hostQuery, cfr_keyhash, keyHash);
        BsonFinish(&hostQuery);

        // keys
        bson op;
        bson_init(&op);
        {
            bson_append_start_object(&op, "$unset");

            for (ip = purgeKeys; ip != NULL; ip = ip->next)
            {
                bson_append_int(&op, ip->name, 1);
            }

            bson_append_finish_object(&op);
        }

        BsonFinish(&op);

        MongoUpdate(conn, MONGO_ARCHIVE, &hostQuery, &op, 0, NULL);

        DeleteItemList(purgeClassNames);
        purgeClassNames = NULL;

        DeleteItemList(purgePcNames);
        purgePcNames = NULL;

        DeleteItemList(purgeKeys);
        purgeKeys = NULL;

        bson_destroy(&hostQuery);
        bson_destroy(&op);

        MongoCheckForError(conn, "PurgeTimestampedLongtermReports", keyHash, NULL);
    }

    mongo_cursor_destroy(cursor);
}

/*****************************************************************************/
void CFDB_PurgePromiseLogs(EnterpriseDB *conn, time_t oldThreshold, time_t now)
/**
 * Deletes old repair and not kept log entries.
 **/
{
    time_t oldStamp;
    bson cond;

    oldStamp = now - oldThreshold;

    CfOut(cf_verbose, "", " -> Purge promise logs from old (deprecated) collections");

    bson_init(&cond);
    {
        bson_append_start_object(&cond, cfr_time);
        bson_append_int(&cond, "$lte", oldStamp);
        bson_append_finish_object(&cond);
    }

    BsonFinish(&cond);

    if(CFDB_CollectionHasData(conn, MONGO_LOGS_REPAIRED))
    {
        MongoRemove(conn, MONGO_LOGS_REPAIRED, &cond, NULL);
        MongoCheckForError(conn,"timed delete host from repair logs collection",NULL,NULL);
    }

    if(CFDB_CollectionHasData(conn, MONGO_LOGS_NOTKEPT))
    {
        MongoRemove(conn, MONGO_LOGS_NOTKEPT, &cond, NULL);
        MongoCheckForError(conn,"timed delete host from not kept logs collection",NULL,NULL);
    }

    bson_destroy(&cond);
}

/*****************************************************************************/

static Item *GetUniquePromiseLogEntryKeys(EnterpriseDB *conn, const char *hostkey, char *promiseLogKey)
{
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    bson field;
    BsonSelectReportFields(&field, 1, promiseLogKey);    

    mongo_cursor *cursor = MongoFind(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    Item *uniquePromiseKeysList = NULL;
    char rhandle[CF_BUFSIZE] = {0};

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator itHostData;
        bson_iterator_init(&itHostData, mongo_cursor_bson( cursor ) );

        rhandle[0] = '\0';

        while (bson_iterator_next(&itHostData))
        {
            if (strcmp(bson_iterator_key(&itHostData), promiseLogKey) == 0)
            {
                bson_iterator iterPromiseLogElement;
                bson_iterator_subiterator(&itHostData, &iterPromiseLogElement);

                while (bson_iterator_next(&iterPromiseLogElement))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&iterPromiseLogElement));
                    IdempPrependItem(&uniquePromiseKeysList, rhandle, NULL);
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return uniquePromiseKeysList;
}
/*****************************************************************************/
static void PurgePromiseLogWithEmptyTimestamps(EnterpriseDB *conn, const char *hostkey, char *promiseLogKey)
{
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    bson field;
    BsonSelectReportFields(&field, 2, cfr_keyhash, promiseLogKey);
    mongo_cursor *cursor = MongoFind(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    Item *promiseKeysList = NULL;
    char rhandle[CF_BUFSIZE] = {0};
    char keyhash[CF_BUFSIZE] = {0};

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator itHostData;
        bson_iterator_init(&itHostData, mongo_cursor_bson( cursor ) );

        rhandle[0] = '\0';
        keyhash[0] = '\0';

        while (bson_iterator_next(&itHostData))
        {
            if (strcmp(bson_iterator_key(&itHostData), cfr_keyhash) == 0)
            {
                snprintf(keyhash,sizeof(keyhash),"%s",bson_iterator_string(&itHostData));
            }
            else if (strcmp(bson_iterator_key(&itHostData), promiseLogKey) == 0)
            {
                bson_iterator iterPromiseComplexKey;
                bson_iterator_subiterator(&itHostData, &iterPromiseComplexKey);

                while (bson_iterator_next(&iterPromiseComplexKey))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&iterPromiseComplexKey));

                    bson objPromiseLogData;
                    bson_iterator_subobject( &iterPromiseComplexKey, &objPromiseLogData);

                    if( BsonIsEmpty(&objPromiseLogData) || BsonIsArrayNonExistentOrEmpty(&objPromiseLogData, cfr_time))
                    {
                        PrependItem(&promiseKeysList,rhandle, NULL);
                    }
                }
            }

            bson hostQuery;
            bson_init(&hostQuery);
            bson_append_string(&hostQuery, cfr_keyhash, keyhash);
            BsonFinish(&hostQuery);

            bson op;
            bson_init(&op);
            {
                bson_append_start_object(&op, "$unset");

                for (Item *ip = promiseKeysList; ip != NULL; ip = ip->next)
                {
                    char key[CF_MAXVARSIZE];
                    snprintf(key,sizeof(key),"%s.%s",promiseLogKey,ip->name);

                    bson_append_int(&op, key, 1);
                }

                bson_append_finish_object(&op);
            }

            BsonFinish(&op);

            DeleteItemList(promiseKeysList);
            promiseKeysList = NULL;            

            MongoUpdate(conn, MONGO_DATABASE, &hostQuery, &op, 0, NULL);

            bson_destroy(&op);
            bson_destroy(&hostQuery);

            MongoCheckForError(conn, "PurgePromiseLogWithEmptyTimestamps", keyhash, NULL);
        }
    }

    mongo_cursor_destroy(cursor);
}

/*****************************************************************************/

void CFDB_PurgePromiseLogsFromMain(EnterpriseDB *conn, const char *hostkey, char *promiseLogReportKey, time_t oldThreshold, time_t now)
/**
 * Deletes old repair and not kept log entries.
 **/
{
    time_t oldStamp;
    bson cond;
    bson query;

    CfOut(cf_verbose, "", " -> Purge promise logs (%s) from main collection (keyhash = %s)", promiseLogReportKey, hostkey);

    Item *promiseLogComplexKeysList = GetUniquePromiseLogEntryKeys(conn, hostkey, promiseLogReportKey);

    oldStamp = now - oldThreshold;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    bson_init(&cond);
    {
        bson_append_start_object(&cond, "$pull");

        for(Item *ip = promiseLogComplexKeysList; ip != NULL; ip = ip->next)
        {
            char timeKey[CF_MAXVARSIZE] = { 0 };
            snprintf(timeKey, sizeof(timeKey), "%s.%s.%s",promiseLogReportKey,ip->name,cfr_time);

            {
                bson_append_start_object(&cond, timeKey);
                bson_append_int(&cond, "$lte", oldStamp);
                bson_append_finish_object(&cond);
            }
        }
        bson_append_finish_object(&cond);
    }

    BsonFinish(&cond);

    DeleteItemList(promiseLogComplexKeysList);
    promiseLogComplexKeysList = NULL;

    MongoUpdate(conn, MONGO_DATABASE, &query, &cond, MONGO_UPDATE_MULTI, NULL);

    bson_destroy(&query);
    bson_destroy(&cond);

    MongoCheckForError(conn, "Purge old entries in hosts collection", promiseLogReportKey, NULL);

    //now check for empty arrays and remove them

    PurgePromiseLogWithEmptyTimestamps(conn, hostkey, promiseLogReportKey);
}

/*****************************************************************************/

void CFDB_PurgeDropReports(EnterpriseDB *conn)
/**
 *  Remove certain reports completely.
 *  UNUSED - currently overwritten on save.
 **/
{
    char *DROP_REPORTS[] = { cfr_setuid, cfr_vars, NULL };
    int i;

    CfOut(cf_verbose, "", " -> Purge droppable reports");

    // query all hosts
    bson empty;
    bson_empty(&empty);

    // define reports to drop (unset)
    bson op;
    bson_init(&op);
    {
        bson_append_start_object(&op, "$unset");
        for (i = 0; DROP_REPORTS[i] != NULL; i++)
        {
            bson_append_int(&op, DROP_REPORTS[i], 1);
        }
        bson_append_finish_object(&op);
    }
    BsonFinish(&op);

    // run update
    MongoUpdate(conn, MONGO_DATABASE, &empty, &op, MONGO_UPDATE_MULTI, NULL);
    bson_destroy(&op);

    MongoCheckForError(conn, "PurgeDropReports", NULL, NULL);
}

/*****************************************************************************/

void CFDB_PurgeScan(EnterpriseDB *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                    Item **purgeKeysPtr, Item **purgeNamesPtr)
{
    bson_iterator it1, it2, it3;
    char var[CF_MAXVARSIZE], key[CF_MAXVARSIZE];
    int deep;
    int foundStamp;
    int emptyLev2 = true;

    if (strcmp(bson_iterator_key(itp), reportKey) != 0)
    {
        return;
    }

    // some reports have time deeper into the db structure
    if (strcmp(reportKey, cfr_vars) == 0)
    {
        deep = true;
    }
    else
    {
        deep = false;
    }

    bson_iterator_subiterator(itp, &it1);

    while (bson_iterator_next(&it1))
    {
        bson_iterator_subiterator(&it1, &it2);

        emptyLev2 = true;

        while (bson_iterator_next(&it2))
        {
            emptyLev2 = false;

            if (deep)           // one level extra
            {
                bson_iterator_subiterator(&it2, &it3);

                snprintf(var, sizeof(var), "%s.%s.%s", reportKey, bson_iterator_key(&it1), bson_iterator_key(&it2));
                snprintf(key, sizeof(key), "%s", (char *) bson_iterator_key(&it2));
                foundStamp = false;

                while (bson_iterator_next(&it3))
                {
                    foundStamp = CFDB_CheckAge(var, key, &it3, now, oldThreshold, purgeKeysPtr, purgeNamesPtr);

                    if (foundStamp)
                    {
                        break;
                    }
                }

                if (!foundStamp)        // remove keys with missing timestamps
                {
                    PrependItem(purgeKeysPtr, var, NULL);
                    if (purgeNamesPtr)
                    {
                        PrependItem(purgeNamesPtr, key, NULL);
                    }
                }
            }
            else                // not deep
            {
                snprintf(var, sizeof(var), "%s.%s", reportKey, bson_iterator_key(&it1));
                snprintf(key, sizeof(key), "%s", (char *) bson_iterator_key(&it1));
                CFDB_CheckAge(var, key, &it2, now, oldThreshold, purgeKeysPtr, purgeNamesPtr);
            }
        }

        if (emptyLev2)
        {
            snprintf(var, sizeof(var), "%s.%s", reportKey, bson_iterator_key(&it1));
            PrependItem(purgeKeysPtr, var, NULL);
        }
    }
}

/*****************************************************************************/

int CFDB_CheckAge(char *var, char *key, bson_iterator *it, time_t now, time_t oldThreshold, Item **purgeKeysPtr,
                  Item **purgeNamesPtr)
{
    time_t then;
    int found;

    if (strcmp(bson_iterator_key(it), cfr_time) == 0)
    {
        found = true;

        then = bson_iterator_int(it);

        if (now - then >= oldThreshold) // definition of old
        {
            PrependItem(purgeKeysPtr, var, NULL);

            if (purgeNamesPtr)
            {
                PrependItem(purgeNamesPtr, key, NULL);
            }

            CfDebug("Report key \"%s\" needs to be purged (%lu seconds old)\n", var, now - then);
        }

    }
    else
    {
        found = false;
    }

    return found;
}

/*****************************************************************************/

void CFDB_PurgeScanStrTime(EnterpriseDB *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
                           Item **purgeKeysPtr)
/**
 * Like PurgeScan but uses time in format "30 Sep 2010" instead of time_t.
 */
{
    bson_iterator it1, it2;
    char thenStr[CF_SMALLBUF];
    char purgeKey[CF_MAXVARSIZE];
    char earliest[CF_SMALLBUF];

    if (strcmp(bson_iterator_key(itp), reportKey) != 0)
    {
        return;
    }

    TimeToDateStr(now - oldThreshold, earliest, sizeof(earliest));

    bson_iterator_subiterator(itp, &it1);

    while (bson_iterator_next(&it1))
    {
        bson_iterator_subiterator(&it1, &it2);

        while (bson_iterator_next(&it2))
        {
            if (strcmp(bson_iterator_key(&it2), cfr_time) == 0)
            {
                if (bson_iterator_type(&it2) != BSON_STRING)
                {
                    CfOut(cf_error, "", "!! Date is not string type in purge");
                    continue;
                }

                snprintf(thenStr, sizeof(thenStr), "%s", bson_iterator_string(&it2));

                if (Nova_CoarseLaterThan(earliest, thenStr))    // definition of old
                {
                    snprintf(purgeKey, sizeof(purgeKey), "%s.%s", reportKey, bson_iterator_key(&it1));
                    PrependItem(purgeKeysPtr, purgeKey, NULL);

                    CfDebug("Report key \"%s\" needs to be purged (date is %s)\n", purgeKey, thenStr);
                }
            }
        }
    }
}

/*****************************************************************************/

void CFDB_PurgeHost(EnterpriseDB *conn, const char *keyHash)
{
    Rlist *hostKeyList = SplitStringAsRList(keyHash, ',');

    for (Rlist *rp = hostKeyList; rp != NULL; rp = rp->next)
    {
        bson cond;

        bson_init(&cond);
        bson_append_string(&cond, cfr_keyhash, ScalarValue(rp));
        BsonFinish(&cond);

        MongoRemove(conn, MONGO_DATABASE, &cond, NULL);

        MongoCheckForError(conn, "delete host from main collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_ARCHIVE, &cond, NULL);

        MongoCheckForError(conn, "delete host from archive collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_DATABASE_MON_MG, &cond, NULL);

        MongoCheckForError(conn, "delete host from mag monitord collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_DATABASE_MON_WK, &cond, NULL);

        MongoCheckForError(conn, "delete host from week monitord collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_DATABASE_MON_YR, &cond, NULL);

        MongoCheckForError(conn, "delete host from year monitord collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_LOGS_REPAIRED, &cond, NULL);

        MongoCheckForError(conn, "delete host from repair logs collection", ScalarValue(rp), NULL);

        MongoRemove(conn, MONGO_LOGS_NOTKEPT, &cond, NULL);

        MongoCheckForError(conn, "delete host from not kept logs collection", ScalarValue(rp), NULL);

        bson_destroy(&cond);
    }

    DeleteRlist(hostKeyList);
}

/*****************************************************************************/

void CFDB_PurgeDeprecatedVitals(EnterpriseDB *conn)
/*
 * Remove pre-Nova 2.1.0 vitals (cf-monitord) structures.
 * Can be taken out when everyone has upgraded to 2.1.0 or newer.
 * DEPRECATED
 */
{
    char var[16];
    int i;

    if (mongo_cmd_drop_collection(conn, MONGO_BASE, "monitoring", NULL))
    {
        CfOut(cf_verbose, "", " -> Removed old monitoring collection");
    }

    // remove all hisograms from main collection
    bson unset_op;
    bson_init(&unset_op);
    {
        bson_append_start_object(&unset_op, "$unset");

        for (i = 0; i < CF_OBSERVABLES; i++)
        {
            snprintf(var, sizeof(var), "hs%d", i);
            bson_append_int(&unset_op, var, 1);
        }

        bson_append_finish_object(&unset_op);
    }
    BsonFinish(&unset_op);

    bson empty;
    MongoUpdate(conn, MONGO_DATABASE, bson_empty(&empty), &unset_op, MONGO_UPDATE_MULTI, NULL);

    bson_destroy(&unset_op);

    MongoCheckForError(conn, "purge deprecated monitoring data", NULL, NULL);    
}

/*****************************************************************************/

void CFDB_RemoveTestData(char *db, char *keyhash)
{
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyhash);
    BsonFinish(&query);

    MongoRemove(&conn, db, &query, NULL);

    bson_destroy(&query);

    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }
}

/*****************************************************************************/
int CFDB_PurgeDeletedHosts(void)
{
    bson op, empty;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson_init(&op);
    {
        bson_append_start_object(&op, "$unset");
        bson_append_int(&op, cfr_deleted_hosts, 1);
        bson_append_finish_object(&op);
    }
    BsonFinish(&op);

    MongoUpdate(&conn, MONGO_SCRATCH, bson_empty(&empty), &op, 0, NULL);

    bson_destroy(&op);

    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

void CFDB_RefreshLastHostComplianceShift(EnterpriseDB *conn, const char *hostkey,
                                         PromiseContextMode promise_context_mode)
{
    assert(hostkey);

    time_t now = time(NULL);
    time_t to = GetShiftSlotStart(now);
    time_t from = to - SECONDS_PER_SHIFT;

    HubQuery *result = CFDB_QueryTotalCompliance(conn, hostkey, NULL, from, to, -1, -1, -1, false, NULL, promise_context_mode);

    if (!result->records)
    {
        return;
    }

    int kept = 0, repaired = 0, notkept = 0, num_samples = 0;

    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubTotalCompliance *record = (const HubTotalCompliance *)rp->item;
        assert(GetShiftSlot(record->t) == GetShiftSlot(from));

        kept += record->kept;
        repaired += record->repaired;
        notkept += record->notkept;
        num_samples++;
    }

    DeleteHubQuery(result, DeleteHubTotalCompliance);

    kept /= num_samples;
    repaired /= num_samples;
    notkept /= num_samples;

    CFDB_SaveHostComplianceShift(conn, hostkey, kept, repaired, notkept, num_samples, from, promise_context_mode);
}

/*****************************************************************************/

static void CFDB_PurgeSoftwareInvalidTimestamp(EnterpriseDB *conn)
{
    CfOut(cf_verbose, "", " -> Purge invalid timestamps in software reports");

    bson query;

    bson_init(&query);
    bson_append_int(&query, cfr_software_t, 0);
    BsonFinish(&query);

    bson unset_op;

    bson_init(&unset_op);
    {
        bson_append_start_object(&unset_op, "$unset");
        bson_append_int(&unset_op, cfr_software, 1);
        bson_append_int(&unset_op, cfr_software_t, 1);
        bson_append_finish_object(&unset_op);
    }
    BsonFinish(&unset_op);

    MongoUpdate(conn, MONGO_DATABASE, &query, &unset_op, MONGO_UPDATE_MULTI, NULL);

    bson_destroy(&unset_op);
    bson_destroy(&query);

    MongoCheckForError(conn, "PurgeSoftwareInvalidTimestamp", NULL, NULL);
}
