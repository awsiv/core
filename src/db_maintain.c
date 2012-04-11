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

#include "db_common.h"
#include "bson_lib.h"

static void CFDB_DropAllIndices(mongo_connection *conn);
static void PurgePromiseLogWithEmptyTimestamps(mongo_connection *conn, char *promiseLogKey);
static Item *GetUniquePromiseLogEntryKeys(mongo_connection *conn, char *promiseLogKey);


void CFDB_Maintenance(void)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    CFDB_EnsureIndices(&dbconn);
    CFDB_PurgeTimestampedReports(&dbconn);

    // support for old DB PromiseLogs format
    CFDB_PurgePromiseLogs(&dbconn, CF_HUB_PURGESECS, time(NULL));

    CFDB_PurgePromiseLogsFromMain(&dbconn, MONGO_LOGS_NOTKEPT_COLL, CF_HUB_PURGESECS, time(NULL));
    CFDB_PurgePromiseLogsFromMain(&dbconn, MONGO_LOGS_REPAIRED_COLL, CF_HUB_PURGESECS, time(NULL));

    CFDB_PurgeTimestampedLongtermReports(&dbconn);
    CFDB_PurgeDeprecatedVitals(&dbconn);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_ReIndexAll(void)
/**
 *  WARNING: May take a long time if there is much data.
 *           Should only be called when indices have changed,
 *           e.g. due to Nova upgrade.
 */
{
    mongo_connection dbconn;

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
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    CFDB_EnsureIndices(&dbconn);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_EnsureIndices(mongo_connection *conn)
/**
 *  Makes sure certain keys have an index to optimize querying and updating.
 **/
{
    bson_buffer bb;
    bson b;

    CfOut(cf_verbose, "", "Ensuring database indices are in place");

    // main host collection

    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_from_buffer(&b, &bb);

    if (!mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE);
    }

    if (!mongo_create_index(conn, MONGO_ARCHIVE, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_ARCHIVE);
    }

    bson_destroy(&b);

    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_class_keys, 1);
    bson_from_buffer(&b, &bb);

    if (!mongo_create_index(conn, MONGO_DATABASE, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE);
    }

    if (!mongo_create_index(conn, MONGO_ARCHIVE, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_ARCHIVE);
    }

    bson_destroy(&b);

    // monitoring collections
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_append_int(&bb, cfm_id, 1);
    bson_from_buffer(&b, &bb);

    if (!mongo_create_index(conn, MONGO_DATABASE_MON_MG, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE_MON_MG);
    }

    if (!mongo_create_index(conn, MONGO_DATABASE_MON_WK, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE_MON_WK);
    }

    if (!mongo_create_index(conn, MONGO_DATABASE_MON_YR, &b, 0, NULL))
    {
        CfOut(cf_error, "mongo_create_index", "!! Could not create index on %s", MONGO_DATABASE_MON_YR);
    }

    bson_destroy(&b);
}

/*****************************************************************************/

static void CFDB_DropAllIndices(mongo_connection *conn)
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
    bson_buffer bb;
    int i;

    for (i = 0; indexedCollections[i] != NULL; i++)
    {
        char *collection = indexedCollections[i];

        bson_buffer_init(&bb);
        bson_append_string(&bb, "dropIndexes", collection);
        bson_append_string(&bb, "index", "*");

        bson dropAllCommand;

        bson_from_buffer(&dropAllCommand, &bb);

        bson result;

        if (!mongo_run_command(conn, MONGO_BASE, &dropAllCommand, &result))
        {
            CfOut(cf_error, "", "mongo_run_command: Could not drop index on collection %s", collection);
        }

        bson_destroy(&dropAllCommand);
        bson_destroy(&result);
    }

}

/*****************************************************************************/

// TODO: looks like something pre BSON-lib era
static void DeleteFromBsonArray(bson_buffer *bb, char *arrName, Item *elements)
{
    Item *ip = NULL;
    bson_buffer *pullAll, *arr;
    char iStr[64];
    int i;

    if (!elements)
    {
        return;
    }

    pullAll = bson_append_start_object(bb, "$pullAll");
    arr = bson_append_start_array(pullAll, arrName);

    for (ip = elements, i = 0; ip != NULL; ip = ip->next, i++)
    {
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(arr, iStr, ip->name);
    }

    bson_append_finish_object(arr);
    bson_append_finish_object(pullAll);

}

void CFDB_PurgeTimestampedReports(mongo_connection *conn)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
    Item *purgeKeys = NULL, *ip;
    Item *purgePcNames = NULL, *purgeClassNames = NULL;
    mongo_cursor *cursor;
    bson query, field, hostQuery, op;
    bson_iterator it1;
    bson_buffer bb, *unset;
    char keyHash[CF_MAXVARSIZE];
    time_t now;

    CfOut(cf_verbose, "", " -> Purge timestamped reports");

    // query all hosts
    bson_empty(&query);

    // only retrieve the purgable reports
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_append_int(&bb, cfr_class, 1);
    bson_append_int(&bb, cfr_vars, 1);
    bson_append_int(&bb, cfr_performance, 1);
    bson_append_int(&bb, cfr_filechanges, 1);
    bson_append_int(&bb, cfr_filediffs, 1);
    bson_append_int(&bb, cfr_promisecompl, 1);
    bson_append_int(&bb, cfr_lastseen, 1);
    bson_append_int(&bb, cfr_bundles, 1);
    bson_append_int(&bb, cfr_valuereport, 1);
    bson_from_buffer(&field, &bb);

    cursor = mongo_find(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&field);

    now = time(NULL);

    while (mongo_cursor_next(cursor))   // iterate over docs
    {
        bson_iterator_init(&it1, cursor->current.data);

        memset(keyHash, 0, sizeof(keyHash));

        while (bson_iterator_next(&it1))
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

        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, keyHash);
        bson_from_buffer(&hostQuery, &bb);

        // keys
        bson_buffer_init(&bb);
        unset = bson_append_start_object(&bb, "$unset");

        for (ip = purgeKeys; ip != NULL; ip = ip->next)
        {
            bson_append_int(unset, ip->name, 1);
        }

        bson_append_finish_object(unset);

        // key array elements
        DeleteFromBsonArray(&bb, cfr_class_keys, purgeClassNames);
        DeleteFromBsonArray(&bb, cfr_promisecompl_keys, purgePcNames);

        bson_from_buffer(&op, &bb);

        mongo_update(conn, MONGO_DATABASE, &hostQuery, &op, 0);
        MongoCheckForError(conn, "PurgeTimestampedReports", keyHash, NULL);

        DeleteItemList(purgeClassNames);
        purgeClassNames = NULL;

        DeleteItemList(purgePcNames);
        purgePcNames = NULL;

        DeleteItemList(purgeKeys);
        purgeKeys = NULL;

        bson_destroy(&hostQuery);
        bson_destroy(&op);
    }

    mongo_cursor_destroy(cursor);
}

/*****************************************************************************/

void CFDB_PurgeTimestampedLongtermReports(mongo_connection *conn)
/**
 * Remove old data from reports with timestamp Usually "old" means one week.
 * For each host: collect keys to delete in a list, and call update once.
 *
 **/
{
    Item *purgeKeys = NULL, *ip;
    Item *purgePcNames = NULL, *purgeClassNames = NULL;
    mongo_cursor *cursor;
    bson query, field, hostQuery, op;
    bson_iterator it1;
    bson_buffer bb, *unset;
    char keyHash[CF_MAXVARSIZE];
    time_t now;

    long threshold = 365 * 24 * 3600;

    CfOut(cf_verbose, "", " -> Purge timestamped reports (longterm archive)");

    // query all hosts
    bson_empty(&query);

    // only retrieve the purgable reports
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_append_int(&bb, cfr_filechanges, 1);
    bson_append_int(&bb, cfr_filediffs, 1);
    bson_from_buffer(&field, &bb);

    cursor = mongo_find(conn, MONGO_ARCHIVE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&field);

    now = time(NULL);

    while (mongo_cursor_next(cursor))   // iterate over docs
    {
        bson_iterator_init(&it1, cursor->current.data);

        memset(keyHash, 0, sizeof(keyHash));

        while (bson_iterator_next(&it1))
        {

            if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
            {
                snprintf(keyHash, sizeof(keyHash), "%s", bson_iterator_string(&it1));
            }

            CFDB_PurgeScan(conn, &it1, cfr_filechanges, threshold, now, &purgeKeys, NULL);
            CFDB_PurgeScan(conn, &it1, cfr_filediffs, threshold, now, &purgeKeys, NULL);
        }

        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, keyHash);
        bson_from_buffer(&hostQuery, &bb);

        // keys
        bson_buffer_init(&bb);
        unset = bson_append_start_object(&bb, "$unset");

        for (ip = purgeKeys; ip != NULL; ip = ip->next)
        {
            bson_append_int(unset, ip->name, 1);
        }

        bson_append_finish_object(unset);
        bson_from_buffer(&op, &bb);

        mongo_update(conn, MONGO_ARCHIVE, &hostQuery, &op, 0);
        MongoCheckForError(conn, "PurgeTimestampedLongtermReports", keyHash, NULL);

        DeleteItemList(purgeClassNames);
        purgeClassNames = NULL;

        DeleteItemList(purgePcNames);
        purgePcNames = NULL;

        DeleteItemList(purgeKeys);
        purgeKeys = NULL;

        bson_destroy(&hostQuery);
        bson_destroy(&op);
    }

    mongo_cursor_destroy(cursor);
}

/*****************************************************************************/
void CFDB_PurgePromiseLogs(mongo_connection *conn, time_t oldThreshold, time_t now)
/**
 * Deletes old repair and not kept log entries.
 **/
{
    bson_buffer bb, *sub;
    time_t oldStamp;
    bson cond;

    oldStamp = now - oldThreshold;

    bson_buffer_init(&bb);

    sub = bson_append_start_object(&bb,cfr_time);
    bson_append_int(sub, "$lte", oldStamp);
    bson_append_finish_object(sub);

    bson_from_buffer(&cond, &bb);

    if(CFDB_CollectionHasData(conn, MONGO_LOGS_REPAIRED))
    {
        mongo_remove(conn, MONGO_LOGS_REPAIRED, &cond);
        MongoCheckForError(conn,"timed delete host from repair logs collection",NULL,NULL);
    }

    if(CFDB_CollectionHasData(conn, MONGO_LOGS_NOTKEPT))
    {
        mongo_remove(conn, MONGO_LOGS_NOTKEPT, &cond);
        MongoCheckForError(conn,"timed delete host from not kept logs collection",NULL,NULL);
    }
    bson_destroy(&cond);
}

/*****************************************************************************/

static Item *GetUniquePromiseLogEntryKeys(mongo_connection *conn, char *promiseLogKey)
{
    bson empty;
    bson field;

    bson_buffer bb;
    bson_buffer_init(&bb);
    bson_append_int(&bb, promiseLogKey, 1);
    bson_from_buffer(&field, &bb);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, bson_empty(&empty), &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&empty);
    bson_destroy(&field);

    Item *uniquePromiseKeysList = NULL;
    char rhandle[CF_BUFSIZE] = {0};

    while (mongo_cursor_next(cursor))
    {
        bson_iterator itHostData;
        bson_iterator_init(&itHostData, cursor->current.data);

        rhandle[0] = '\0';

        while (bson_iterator_next(&itHostData))
        {
            if (strcmp(bson_iterator_key(&itHostData), promiseLogKey) == 0)
            {
                bson_iterator iterPromiseLogElement;
                bson_iterator_init(&iterPromiseLogElement, bson_iterator_value(&itHostData));

                while (bson_iterator_next(&iterPromiseLogElement))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&iterPromiseLogElement));
                    IdempPrependItem(&uniquePromiseKeysList, rhandle, NULL);
                }
            }
        }
    }

    return uniquePromiseKeysList;
}
/*****************************************************************************/
static void PurgePromiseLogWithEmptyTimestamps(mongo_connection *conn, char *promiseLogKey)
{
    bson empty;
    bson field;

    bson_buffer bb;
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_append_int(&bb, promiseLogKey, 1);
    bson_from_buffer(&field, &bb);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, bson_empty(&empty), &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&empty);
    bson_destroy(&field);

    Item *promiseKeysList = NULL;
    char rhandle[CF_BUFSIZE] = {0};
    char keyhash[CF_BUFSIZE] = {0};

    while (mongo_cursor_next(cursor))
    {
        bson_iterator itHostData;
        bson_iterator_init(&itHostData, cursor->current.data);

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
                bson_iterator_init(&iterPromiseComplexKey, bson_iterator_value(&itHostData));

                while (bson_iterator_next(&iterPromiseComplexKey))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&iterPromiseComplexKey));

                    bson_iterator iterPromiseLogData;
                    bson_iterator_init(&iterPromiseLogData, bson_iterator_value(&iterPromiseComplexKey));

                    bson objPromiseLogData;
                    bson_iterator_subobject( &iterPromiseComplexKey, &objPromiseLogData);

                    if(BsonIsArrayNonExistentOrEmpty(&objPromiseLogData,cfr_time))
                    {
                        PrependItem(&promiseKeysList,rhandle, NULL);
                    }
                }
            }

            bson hostQuery;
            bson_buffer_init(&bb);
            bson_append_string(&bb, cfr_keyhash, keyhash);
            bson_from_buffer(&hostQuery, &bb);

            bson op;
            bson_buffer_init(&bb);
            bson_buffer *unset = bson_append_start_object(&bb, "$unset");

            for (Item *ip = promiseKeysList; ip != NULL; ip = ip->next)
            {
                char key[CF_MAXVARSIZE];
                snprintf(key,sizeof(key),"%s.%s",promiseLogKey,ip->name);

                bson_append_int(unset, key, 1);
            }

            DeleteItemList(promiseKeysList);
            promiseKeysList = NULL;

            bson_append_finish_object(unset);
            bson_from_buffer(&op, &bb);

            mongo_update(conn, MONGO_DATABASE, &hostQuery, &op, 0);

            bson_destroy(&op);
            bson_destroy(&hostQuery);

            MongoCheckForError(conn, "PurgePromiseLogWithEmptyTimestamps", keyhash, NULL);
        }
    }
}

/*****************************************************************************/

void CFDB_PurgePromiseLogsFromMain(mongo_connection *conn, char *promiseLogReportKey, time_t oldThreshold, time_t now)
/**
 * Deletes old repair and not kept log entries.
 **/
{
    bson_buffer bb;
    time_t oldStamp;
    bson cond;
    bson query;

    Item *promiseLogComplexKeysList = GetUniquePromiseLogEntryKeys(conn, promiseLogReportKey);

    oldStamp = now - oldThreshold;

    bson_buffer_init(&bb);
    bson_buffer *pull = bson_append_start_object(&bb, "$pull");

    for(Item *ip = promiseLogComplexKeysList; ip != NULL; ip = ip->next)
    {
        char timeKey[CF_MAXVARSIZE] = { 0 };
        snprintf(timeKey, sizeof(timeKey), "%s.%s.%s",promiseLogReportKey,ip->name,cfr_time);

        bson_buffer *bbTimeStamp = bson_append_start_object(pull, timeKey);
        bson_append_int(bbTimeStamp, "$lte", oldStamp);
        bson_append_finish_object(bbTimeStamp);
    }

    DeleteItemList(promiseLogComplexKeysList);
    promiseLogComplexKeysList = NULL;

    bson_append_finish_object(pull);
    bson_from_buffer(&cond, &bb);

    mongo_update(conn, MONGO_DATABASE, bson_empty(&query), &cond, MONGO_UPDATE_MULTI);

    MongoCheckForError(conn, "Purge old entries in hosts collection", promiseLogReportKey, NULL);
    bson_destroy(&cond);
    bson_destroy(&query);

    //now check for empty arrays and remove them

    PurgePromiseLogWithEmptyTimestamps(conn, promiseLogReportKey);
}

/*****************************************************************************/

void CFDB_PurgeDropReports(mongo_connection *conn)
/**
 *  Remove certain reports completely.
 *  UNUSED - currently overwritten on save.
 **/
{
    bson_buffer bb, *unset;
    bson empty, op;
    char *DROP_REPORTS[] = { cfr_setuid, cfr_vars, NULL };
    int i;

    CfOut(cf_verbose, "", " -> Purge droppable reports");

    // query all hosts
    bson_empty(&empty);

    // define reports to drop (unset)
    bson_buffer_init(&bb);
    unset = bson_append_start_object(&bb, "$unset");
    for (i = 0; DROP_REPORTS[i] != NULL; i++)
    {
        bson_append_int(unset, DROP_REPORTS[i], 1);
    }
    bson_append_finish_object(unset);
    bson_from_buffer(&op, &bb);

    // run update
    mongo_update(conn, MONGO_DATABASE, &empty, &op, MONGO_UPDATE_MULTI);
    MongoCheckForError(conn, "PurgeDropReports", NULL, NULL);

    bson_destroy(&op);
}

/*****************************************************************************/

void CFDB_PurgeScan(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
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

    bson_iterator_init(&it1, bson_iterator_value(itp));

    while (bson_iterator_next(&it1))
    {
        bson_iterator_init(&it2, bson_iterator_value(&it1));

        emptyLev2 = true;

        while (bson_iterator_next(&it2))
        {
            emptyLev2 = false;

            if (deep)           // one level extra
            {
                bson_iterator_init(&it3, bson_iterator_value(&it2));

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

void CFDB_PurgeScanStrTime(mongo_connection *conn, bson_iterator *itp, char *reportKey, time_t oldThreshold, time_t now,
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

    bson_iterator_init(&it1, bson_iterator_value(itp));

    while (bson_iterator_next(&it1))
    {
        bson_iterator_init(&it2, bson_iterator_value(&it1));

        while (bson_iterator_next(&it2))
        {
            if (strcmp(bson_iterator_key(&it2), cfr_time) == 0)
            {
                if (bson_iterator_type(&it2) != bson_string)
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

void CFDB_PurgeHost(mongo_connection *conn, char *keyHash)
{
    bson_buffer bb;
    bson cond;

    Rlist *hostKeyList = SplitStringAsRList(keyHash, ',');

    for (Rlist *rp = hostKeyList; rp != NULL; rp = rp->next)
    {
        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, ScalarValue(rp));
        bson_from_buffer(&cond, &bb);

        mongo_remove(conn, MONGO_DATABASE, &cond);

        MongoCheckForError(conn, "delete host from main collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_ARCHIVE, &cond);

        MongoCheckForError(conn, "delete host from archive collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_DATABASE_MON_MG, &cond);

        MongoCheckForError(conn, "delete host from mag monitord collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_DATABASE_MON_WK, &cond);

        MongoCheckForError(conn, "delete host from week monitord collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_DATABASE_MON_YR, &cond);

        MongoCheckForError(conn, "delete host from year monitord collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_LOGS_REPAIRED, &cond);

        MongoCheckForError(conn, "delete host from repair logs collection", ScalarValue(rp), NULL);

        mongo_remove(conn, MONGO_LOGS_NOTKEPT, &cond);

        MongoCheckForError(conn, "delete host from not kept logs collection", ScalarValue(rp), NULL);

        bson_destroy(&cond);
    }

    DeleteRlist(hostKeyList);
}

/*****************************************************************************/

void CFDB_PurgeDeprecatedVitals(mongo_connection *conn)
/*
 * Remove pre-Nova 2.1.0 vitals (cf-monitord) structures.
 * Can be taken out when everyone has upgraded to 2.1.0 or newer.
 * DEPRECATED
 */
{
    bson_buffer bb;
    bson empty, unsetOp;
    bson_buffer *obj;
    char var[16];
    int i;

    if (mongo_cmd_drop_collection(conn, MONGO_BASE, "monitoring", NULL))
    {
        CfOut(cf_verbose, "", " -> Removed old monitoring collection");
    }

    // remove all hisograms from main collection

    bson_buffer_init(&bb);
    obj = bson_append_start_object(&bb, "$unset");

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        snprintf(var, sizeof(var), "hs%d", i);
        bson_append_int(obj, var, 1);
    }

    bson_append_finish_object(obj);

    bson_from_buffer(&unsetOp, &bb);

    mongo_update(conn, MONGO_DATABASE, bson_empty(&empty), &unsetOp, MONGO_UPDATE_MULTI);

    MongoCheckForError(conn, "purge deprecated monitoring data", NULL, NULL);

    bson_destroy(&unsetOp);

}

/*****************************************************************************/

void CFDB_RemoveTestData(char *db, char *keyhash)
{
    bson query;
    bson_buffer bb;
    mongo_connection conn;

    if (!CFDB_Open(&conn))
    {
        return;
    }

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&query, &bb);

    mongo_remove(&conn, db, &query);

    bson_destroy(&query);
    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }
}

/*****************************************************************************/
int CFDB_PurgeDeletedHosts(void)
{
    bson_buffer bb, *unset;
    bson op, empty;
    mongo_connection conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson_buffer_init(&bb);

    unset = bson_append_start_object(&bb, "$unset");
    bson_append_int(unset, cfr_deleted_hosts, 1);
    bson_append_finish_object(unset);
    bson_from_buffer(&op, &bb);

    mongo_update(&conn, MONGO_SCRATCH, bson_empty(&empty), &op, 0);

    bson_destroy(&op);

    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

