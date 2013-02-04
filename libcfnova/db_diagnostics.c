/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "db_diagnostics.h"

#include "bson_lib.h"
#include "alloc.h"
#include "sequence.h"
#include "cfstream.h"

#include <bson.h>
#include <mongo.h>
#include <assert.h>

/*
 * Database keys
 */

/* server status */
#define diagnostic_dbk_server_status "serverStatus"
#define diagnostic_dbk_host_name "host"
#define diagnostic_dbk_version "version"
#define diagnostic_dbk_uptime "uptime"
#define diagnostic_dbk_lock "globalLock"
#define diagnostic_dbk_lock_total_time "totalTime"
#define diagnostic_dbk_lock_lock_time "lockTime"
#define diagnostic_dbk_lock_queue_total "currentQueueTotal"
#define diagnostic_dbk_lock_queue_readers "currentQueueReaders"
#define diagnostic_dbk_lock_queue_writers "currentQueueWriters"
#define diagnostic_dbk_mem "mem"
#define diagnostic_dbk_mem_resident "resident"
#define diagnostic_dbk_mem_virtual "virtual"
#define diagnostic_dbk_mem_mapped "mapped"
#define diagnostic_dbk_bgflush "backgroungFlushing"
#define diagnostic_dbk_bgflush_count "flushes"
#define diagnostic_dbk_bgflush_total_ms "total_ms"
#define diagnostic_dbk_bgflush_avg_ms "average_ms"
#define diagnostic_dbk_bgflush_last_ms "last_ms"

/* db status */
#define diagnostic_dbk_db "databases"
#define diagnostic_dbk_db_name "name"
#define diagnostic_dbk_db_obj_cnt "objectCount"
#define diagnostic_dbk_db_avg_obj_size "avgObjSize"
#define diagnostic_dbk_db_data_size "dataSize"
#define diagnostic_dbk_db_storage_size "storageSize"
#define diagnostic_dbk_db_file_size "fileSize"

/* collection status */
#define diagnostic_dbk_collections "collections"
#define diagnostic_dbk_collection_name "name"
#define diagnostic_dbk_collection_obj_count "objCount"
#define diagnostic_dbk_collection_data_size "objSize"
#define diagnostic_dbk_collection_avg_obj_size "avgObjSize"
#define diagnostic_dbk_collection_storage_size "storageSize"
#define diagnostic_dbk_collection_index_count "indexCount"
#define diagnostic_dbk_collection_padding_factor "paddingFactor"
#define diagnostic_dbk_collection_total_index_size "totalIndexSize"

/*
 * Internal utilities
 */

static DiagnosticServerStatus *DiagnosticServerStatusCreate(void);
static void DiagnosticServerStatusFree(DiagnosticServerStatus *ptr);
/* transfer ownership */
static void DiagnosticServerStatusAppendDatabaseStatus(DiagnosticServerStatus *server,
                                                       DiagnosticDatabaseStatus *database);
static void DiagnosticServerStatusToBson(DiagnosticServerStatus *in, bson *out);
static DiagnosticServerStatus *DiagnosticServerStatusFromBson(bson *in);


static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusCreate(void);
static void DiagnosticDatabaseStatusFree(DiagnosticDatabaseStatus *ptr);
/* transfer ownership */
static void DiagnosticDatabaseStatusAppendCollectionStatus(DiagnosticDatabaseStatus *database,
                                                           DiagnosticCollectionStatus *collection);
static void DiagnosticDatabaseStatusToBson(DiagnosticDatabaseStatus *in, bson *out);
static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusFromBson(bson *in);


static DiagnosticCollectionStatus *DiagnosticCollectionStatusCreate(void);
static void DiagnosticCollectionStatusFree(DiagnosticCollectionStatus *ptr);
static void DiagnosticCollectionStatusToBson(DiagnosticCollectionStatus *stat, bson *out);
static DiagnosticCollectionStatus *DiagnosticCollectionStatusFromBson(bson *in);


static DiagnosticServerStatus *DiagnosticServerStatusGet(EnterpriseDB *conn);
static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusGet(EnterpriseDB *conn,
                                                             const char *db_name);
static DiagnosticCollectionStatus *DiagnosticCollectionStatusGet(EnterpriseDB *conn ,
                                                                 const char *db_name,
                                                                 const char *collection_name);


static DiagnosticMongoSnaphot *DiagnosticMongoSnaphotCreate(void);
static void DiagnosticMongoSnaphotFree(DiagnosticMongoSnaphot *ptr);

/*
 * This function contain hardcoded MongoDB architecture schema (dbs/collections)
 * and need to be updated by hand with the changes made to the schema.
 */
static DiagnosticServerStatus *DiagnosticStatusPull(EnterpriseDB *conn);
static void DiagnosticStatusPush(EnterpriseDB *conn, DiagnosticServerStatus *status,
                                 time_t time_id);


/*
 * Implementation
 */

static DiagnosticMongoSnaphot *DiagnosticMongoSnaphotCreate(void)
{
    DiagnosticMongoSnaphot *snapshot = NULL;
    snapshot = xmalloc(sizeof(DiagnosticMongoSnaphot));

    snapshot->status = NULL;
    snapshot->time = 0;

    return snapshot;
}
static void DiagnosticMongoSnaphotFree(DiagnosticMongoSnaphot *ptr)
{
    if (ptr)
    {
        if (ptr->status)
        {
            DiagnosticServerStatusFree(ptr->status);
        }
    }

    free(ptr);
}

static DiagnosticServerStatus *DiagnosticServerStatusCreate(void)
{
    DiagnosticServerStatus *status = NULL;
    status = xmalloc(sizeof(DiagnosticServerStatus));

    status->host = NULL;
    status->version = NULL;
    status->server_uptime = 0.0;

    status->global_lock_lock_time_us = 0.0;
    status->global_lock_queue_readers = 0;
    status->global_lock_queue_total = 0;
    status->global_lock_queue_writers = 0;
    status->global_lock_total_time_us = 0.0;

    status->mem_mapped = 0;
    status->mem_resident = 0;
    status->mem_virtual = 0;

    status->bg_flush_avg_ms = 0;
    status->bg_flush_count = 0;
    status->bg_flush_last_ms = 0;
    status->bg_flush_total_ms = 0;

    status->db_list = NULL;

    return status;
}

static void DiagnosticServerStatusFree(DiagnosticServerStatus *ptr)
{
    if (ptr)
    {
        free(ptr->host);
        free(ptr->version);

        if (ptr->db_list)
        {
            SeqDestroy(ptr->db_list);
        }
    }

    free(ptr);
}

static void DiagnosticServerStatusAppendDatabaseStatus(DiagnosticServerStatus *server,
                                                       DiagnosticDatabaseStatus *database)
{
    assert(server);
    assert(database);

    if (!server->db_list)
    {
        server->db_list = SeqNew(2, DiagnosticDatabaseStatusFree);
    }

    SeqAppend(server->db_list, database);
}

static void DiagnosticServerStatusToBson(DiagnosticServerStatus *in, bson *out)
{
    assert(in);
    assert(out);

    BsonAppendStartObject(out, diagnostic_dbk_server_status);
    {
        BsonAppendString(out, diagnostic_dbk_host_name, in->host);
        BsonAppendString(out, diagnostic_dbk_version, in->version);
        BsonAppendDouble(out, diagnostic_dbk_uptime, in->server_uptime);

        BsonAppendStartObject(out, diagnostic_dbk_lock);
        {
            BsonAppendDouble(out, diagnostic_dbk_lock_lock_time, in->global_lock_lock_time_us);
            BsonAppendDouble(out, diagnostic_dbk_lock_total_time, in->global_lock_total_time_us);
            BsonAppendInt(out, diagnostic_dbk_lock_queue_readers, in->global_lock_queue_readers);
            BsonAppendInt(out, diagnostic_dbk_lock_queue_writers, in->global_lock_queue_writers);
            BsonAppendInt(out, diagnostic_dbk_lock_queue_total, in->global_lock_queue_total);
        }
        BsonAppendFinishObject(out);

        BsonAppendStartObject(out, diagnostic_dbk_mem);
        {
            BsonAppendInt(out, diagnostic_dbk_mem_mapped, in->mem_mapped);
            BsonAppendInt(out, diagnostic_dbk_mem_resident, in->mem_resident);
            BsonAppendInt(out, diagnostic_dbk_mem_virtual, in->mem_virtual);
        }
        BsonAppendFinishObject(out);

        BsonAppendStartObject(out, diagnostic_dbk_bgflush);
        {
            BsonAppendInt(out, diagnostic_dbk_bgflush_avg_ms, in->bg_flush_avg_ms);
            BsonAppendInt(out, diagnostic_dbk_bgflush_count, in->bg_flush_count);
            BsonAppendInt(out, diagnostic_dbk_bgflush_last_ms, in->bg_flush_last_ms);
            BsonAppendInt(out, diagnostic_dbk_bgflush_total_ms, in->bg_flush_total_ms);
        }
        BsonAppendFinishObject(out);

        if (in->db_list)
        {
            BsonAppendStartObject(out, diagnostic_dbk_db);
            for (int i = 0; i < SeqLength(in->db_list); i++)
            {
                DiagnosticDatabaseStatus *database =
                        (DiagnosticDatabaseStatus*)SeqAt(in->db_list, i);

                DiagnosticDatabaseStatusToBson(database, out);
            }
            BsonAppendFinishObject(out);
        }
    }
    BsonAppendFinishObject(out);
}

static DiagnosticServerStatus *DiagnosticServerStatusFromBson(bson *in)
{
    DiagnosticServerStatus *status = DiagnosticServerStatusCreate();

    bson status_b;
    if (BsonObjectGet(in, diagnostic_dbk_server_status, &status_b))
    {
        const char *host = NULL;
        BsonStringGet(&status_b, diagnostic_dbk_host_name, &host);
        status->host = xstrdup(host);

        const char *version = NULL;
        BsonStringGet(&status_b, diagnostic_dbk_version, &version);
        status->version = xstrdup(version);

        BsonDoubleGet(&status_b, diagnostic_dbk_uptime, &(status->server_uptime));

        bson global_lock;
        if (BsonObjectGet(&status_b, diagnostic_dbk_lock, &global_lock))
        {
            BsonDoubleGet(&global_lock, diagnostic_dbk_lock_total_time,
                          &(status->global_lock_total_time_us));
            BsonDoubleGet(&global_lock, diagnostic_dbk_lock_lock_time,
                          &(status->global_lock_lock_time_us));
            BsonIntGet(&global_lock, diagnostic_dbk_lock_queue_total,
                       &(status->global_lock_queue_total));
            BsonIntGet(&global_lock, diagnostic_dbk_lock_queue_readers,
                       &(status->global_lock_queue_readers));
            BsonIntGet(&global_lock, diagnostic_dbk_lock_queue_writers,
                       &(status->global_lock_queue_writers));
        }

        bson mem;
        if (BsonObjectGet(&status_b, diagnostic_dbk_mem, &mem))
        {
            BsonIntGet(&mem, diagnostic_dbk_mem_mapped, &(status->mem_mapped));
            BsonIntGet(&mem, diagnostic_dbk_mem_resident, &(status->mem_resident));
            BsonIntGet(&mem, diagnostic_dbk_mem_virtual, &(status->mem_virtual));
        }

        bson flush;
        if (BsonObjectGet(&status_b, diagnostic_dbk_bgflush, &flush))
        {
            BsonIntGet(&flush, diagnostic_dbk_bgflush_count, &(status->bg_flush_count));
            BsonIntGet(&flush, diagnostic_dbk_bgflush_total_ms, &(status->bg_flush_total_ms));
            BsonIntGet(&flush, diagnostic_dbk_bgflush_avg_ms, &(status->bg_flush_avg_ms));
            BsonIntGet(&flush, diagnostic_dbk_bgflush_last_ms, &(status->bg_flush_last_ms));
        }

        bson dbs;
        if (BsonObjectGet(&status_b, diagnostic_dbk_db, &dbs))
        {
            bson_iterator it;
            bson_iterator_init(&it, &dbs);

            while (BsonIsTypeValid(bson_iterator_next(&it)) > 0)
            {
                if (bson_iterator_type(&it) == BSON_OBJECT)
                {
                    bson db_b;
                    bson_iterator_subobject(&it, &db_b);

                    DiagnosticDatabaseStatus *db_stat =
                            DiagnosticDatabaseStatusFromBson(&db_b);

                    if (!db_stat)
                    {
                        continue;
                    }

                    DiagnosticServerStatusAppendDatabaseStatus(status, db_stat);
                }
            }
        }
    }

    return status;
}

static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusCreate(void)
{
    DiagnosticDatabaseStatus *status = NULL;
    status = xmalloc(sizeof(DiagnosticDatabaseStatus));

    status->db_name = NULL;
    status->object_count = 0;
    status->avg_object_size = 0.0;
    status->data_size = 0;
    status->storage_size = 0;
    status->file_size = 0;

    status->collection_list = NULL;

    return status;
}

static void DiagnosticDatabaseStatusFree(DiagnosticDatabaseStatus *ptr)
{
    if (ptr)
    {
        free (ptr->db_name);
        if (ptr->collection_list)
        {
            SeqDestroy(ptr->collection_list);
        }
    }

    free(ptr);
}

static void DiagnosticDatabaseStatusAppendCollectionStatus(DiagnosticDatabaseStatus *database,
                                                           DiagnosticCollectionStatus *collection)
{
    assert(database);
    assert(collection);

    if (!database->collection_list)
    {
        database->collection_list = SeqNew(20, DiagnosticCollectionStatusFree);
    }

    SeqAppend(database->collection_list, collection);
}

static void DiagnosticDatabaseStatusToBson(DiagnosticDatabaseStatus *in, bson *out)
{
    assert(in);
    assert(out);

    BsonAppendStartObject(out, in->db_name);
    {
        BsonAppendString(out, diagnostic_dbk_db_name, in->db_name);
        BsonAppendInt(out, diagnostic_dbk_db_obj_cnt, in->object_count);
        BsonAppendDouble(out, diagnostic_dbk_db_avg_obj_size, in->avg_object_size);
        BsonAppendInt(out, diagnostic_dbk_db_data_size, in->data_size);
        BsonAppendInt(out, diagnostic_dbk_db_storage_size, in->storage_size);
        BsonAppendInt(out, diagnostic_dbk_db_file_size, in->file_size);

        if (in->collection_list)
        {
            BsonAppendStartObject(out, diagnostic_dbk_collections);
            for (int i = 0; i < SeqLength(in->collection_list); i++)
            {
                DiagnosticCollectionStatus *collection =
                        (DiagnosticCollectionStatus*)SeqAt(in->collection_list, i);

                DiagnosticCollectionStatusToBson(collection, out);
            }
            BsonAppendFinishObject(out);
        }
    }
    BsonAppendFinishObject(out);
}

static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusFromBson(bson *in)
{
    DiagnosticDatabaseStatus *database = DiagnosticDatabaseStatusCreate();

    const char *name = NULL;
    BsonStringGet(in, diagnostic_dbk_db_name, &name);
    database->db_name = xstrdup(name);

    BsonIntGet(in, diagnostic_dbk_db_obj_cnt, &(database->object_count));
    BsonDoubleGet(in, diagnostic_dbk_db_avg_obj_size, &(database->avg_object_size));
    BsonIntGet(in, diagnostic_dbk_db_data_size, &(database->data_size));
    BsonIntGet(in, diagnostic_dbk_db_storage_size, &(database->storage_size));
    BsonIntGet(in, diagnostic_dbk_db_file_size, &(database->file_size));

    bson colls;
    if (BsonObjectGet(in, diagnostic_dbk_collections, &colls))
    {
        bson_iterator it;
        bson_iterator_init(&it, &colls);

        while (BsonIsTypeValid(bson_iterator_next(&it)) > 0)
        {
            if (bson_iterator_type(&it) == BSON_OBJECT)
            {
                bson curr_coll;
                bson_iterator_subobject(&it, &curr_coll);

                DiagnosticCollectionStatus *coll_stat =
                        DiagnosticCollectionStatusFromBson(&curr_coll);

                if (!coll_stat)
                {
                    continue;
                }

                DiagnosticDatabaseStatusAppendCollectionStatus(database, coll_stat);
            }
        }
    }

    return database;
}

static DiagnosticCollectionStatus *DiagnosticCollectionStatusCreate(void)
{
    DiagnosticCollectionStatus *status = NULL;
    status = xmalloc(sizeof(DiagnosticCollectionStatus));

    status->name = NULL;
    status->object_count = 0;
    status->data_size = 0;
    status->avg_object_size = 0.0;
    status->storage_size = 0;
    status->index_count = 0;
    status->padding_factor = 0.0;
    status->total_index_size = 0;

    return status;
}

static void DiagnosticCollectionStatusFree(DiagnosticCollectionStatus *ptr)
{
    if (ptr)
    {
        free(ptr->name);
    }

    free(ptr);
}

static void DiagnosticCollectionStatusToBson(DiagnosticCollectionStatus *in,
                                             bson *out)
{
    assert(in);
    assert(out);

    BsonAppendStartObject(out, in->name);
    {
        BsonAppendString(out, diagnostic_dbk_collection_name, in->name);
        BsonAppendInt(out, diagnostic_dbk_collection_obj_count, in->object_count);
        BsonAppendInt(out, diagnostic_dbk_collection_data_size, in->data_size);
        BsonAppendDouble(out, diagnostic_dbk_collection_avg_obj_size, in->avg_object_size);
        BsonAppendInt(out, diagnostic_dbk_collection_storage_size, in->storage_size);
        BsonAppendInt(out, diagnostic_dbk_collection_index_count, in->index_count);
        BsonAppendDouble(out, diagnostic_dbk_collection_padding_factor, in->padding_factor);
        BsonAppendInt(out, diagnostic_dbk_collection_total_index_size, in->total_index_size);

    }
    BsonAppendFinishObject(out);
}

static DiagnosticCollectionStatus *DiagnosticCollectionStatusFromBson(bson *in)
{
    DiagnosticCollectionStatus *collection = DiagnosticCollectionStatusCreate();

    const char *name = NULL;
    if (BsonStringGet(in, diagnostic_dbk_collection_name, &name))
    {
        collection->name = xstrdup(name);
    }

    BsonIntGet(in, diagnostic_dbk_collection_obj_count, &(collection->object_count));
    BsonIntGet(in, diagnostic_dbk_collection_data_size, &(collection->data_size));
    BsonDoubleGet(in, diagnostic_dbk_collection_avg_obj_size, &(collection->avg_object_size));
    BsonIntGet(in, diagnostic_dbk_collection_storage_size, &(collection->storage_size));
    BsonIntGet(in, diagnostic_dbk_collection_index_count, &(collection->index_count));
    BsonDoubleGet(in, diagnostic_dbk_collection_padding_factor,
                  &(collection->padding_factor));
    BsonIntGet(in, diagnostic_dbk_collection_total_index_size,
               &(collection->total_index_size));

    return collection;
}

static DiagnosticServerStatus *DiagnosticServerStatusGet(EnterpriseDB *conn)
{
    assert(conn);

    DiagnosticServerStatus *status = NULL;

    bson cmd;
    bson out = {0};

    bson_init(&cmd);
    BsonAppendInt(&cmd, "serverStatus", 1);
    BsonAppendInt(&cmd, "scale", 1024);
    BsonFinish(&cmd);

    if (MongoRunCommand(conn, "admin", &cmd, &out) == MONGO_OK)
    {
        status = DiagnosticServerStatusCreate();

        const char *host = NULL;
        if (BsonStringGet(&out, "host", &host))
        {
           status->host = xstrdup(host);
        }

        const char *version = NULL;
        if (BsonStringGet(&out, "version", &version))
        {
            status->version = xstrdup(version);
        }

        BsonDoubleGet(&out, "uptime", &(status->server_uptime));

        bson global_lock;
        if (BsonObjectGet(&out, "globalLock", &global_lock))
        {
            BsonDoubleGet(&global_lock, "totalTime", &(status->global_lock_total_time_us));
            BsonDoubleGet(&global_lock, "lockTime", &(status->global_lock_lock_time_us));

            bson currentQueue;
            if (BsonObjectGet(&global_lock, "currentQueue", &currentQueue))
            {
                BsonIntGet(&currentQueue, "total", &(status->global_lock_queue_total));
                BsonIntGet(&currentQueue, "readers", &(status->global_lock_queue_readers));
                BsonIntGet(&currentQueue, "writers", &(status->global_lock_queue_writers));
            }
        }

        bson mem;
        if (BsonObjectGet(&out, "mem", &mem))
        {
            BsonIntGet(&mem, "resident", &(status->mem_resident));
            BsonIntGet(&mem, "virtual", &(status->mem_virtual));
            BsonIntGet(&mem, "mapped", &(status->mem_mapped));
        }

        bson bg_flush;
        if (BsonObjectGet(&out, "backgroundFlushing", &bg_flush))
        {
            BsonIntGet(&bg_flush, "flushes", &(status->bg_flush_count));
            BsonIntGet(&bg_flush, "total_ms", &(status->bg_flush_total_ms));
            BsonIntGet(&bg_flush, "last_ms", &(status->bg_flush_last_ms));
        }

        bson_destroy(&out);
    }

    bson_destroy(&cmd);

    return status;
}

static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusGet(EnterpriseDB *conn,
                                                             const char *db_name)
{
    assert(conn);
    assert(db_name);

    DiagnosticDatabaseStatus *database = NULL;

    bson cmd;
    bson out = {0};

    bson_init(&cmd);
    BsonAppendString(&cmd, "dbStats", "1");
    BsonAppendInt(&cmd, "scale", 1024);
    BsonFinish(&cmd);

    if (MongoRunCommand(conn, db_name, &cmd, &out) == MONGO_OK)
    {
        database = DiagnosticDatabaseStatusCreate();

        const char *db_name = NULL;
        if (BsonStringGet(&out, "db", &db_name))
        {
           database->db_name = xstrdup(db_name);
        }

        BsonIntGet(&out, "objects", &(database->object_count));
        BsonDoubleGet(&out, "avgObjSize", &(database->avg_object_size));
        BsonIntGet(&out, "dataSize", &(database->data_size));
        BsonIntGet(&out, "storageSize", &(database->storage_size));
        BsonIntGet(&out, "fileSize", &(database->file_size));

        bson_destroy(&out);
    }

    bson_destroy(&cmd);

    return database;
}

static DiagnosticCollectionStatus *DiagnosticCollectionStatusGet(EnterpriseDB *conn,
                                                                 const char *db_name,
                                                                 const char *collection_name)
{
    assert(conn);
    assert(db_name);
    assert(collection_name);

    DiagnosticCollectionStatus *collection = NULL;

    bson cmd;
    bson out = {0};

    bson_init(&cmd);
    BsonAppendString(&cmd, "collStats", collection_name);
    BsonAppendInt(&cmd, "scale", 1024);
    BsonFinish(&cmd);


    if (MongoRunCommand(conn, db_name, &cmd, &out) == MONGO_OK)
    {
        collection = DiagnosticCollectionStatusCreate();

        collection->name = xstrdup(collection_name);

        BsonIntGet(&out, "count", &(collection->object_count));
        BsonIntGet(&out, "size", &(collection->data_size));
        BsonDoubleGet(&out, "avgObjSize", &(collection->avg_object_size));
        BsonIntGet(&out, "storageSize", &(collection->storage_size));
        BsonIntGet(&out, "nindexes", &(collection->index_count));
        BsonDoubleGet(&out, "paddingFactor", &(collection->padding_factor));
        BsonIntGet(&out, "totalIndexSize", &(collection->total_index_size));

        bson_destroy(&out);
    }

    bson_destroy(&cmd);

    return collection;

}

static DiagnosticServerStatus *DiagnosticStatusPull(EnterpriseDB *conn)
{
    assert(conn);

    /* fetch service status from mongo */
    DiagnosticServerStatus *service_status = NULL;
    service_status = DiagnosticServerStatusGet(conn);
    if (!service_status)
    {
        CfOut(cf_error, "",
              "Diagnostics: failed to fetch server status from MongoDB");
        return NULL;
    }

    /* fetch cfreport status tree */
    DiagnosticDatabaseStatus *db_cfreport = NULL;
    db_cfreport = DiagnosticDatabaseStatusGet(conn, "cfreport");
    if (!db_cfreport)
    {
        CfOut(cf_error, "",
              "Diagnostics: failed to fetch database (cfreport) status from MongoDB");
    }
    else
    {
        const char *cfreport_coll_list[] =
        {
            "archive",
            "bodies",
            "hosts",
            "monitoring_mg",
            "monitoring_wk",
            "monitoring_yr",
            "promises_exp",
            "promises_unexp",
            NULL
        };

        for (int i = 0; cfreport_coll_list[i] != NULL; i++)
        {
            DiagnosticCollectionStatus *coll_archive = NULL;
            coll_archive = DiagnosticCollectionStatusGet(conn, "cfreport",
                                                         cfreport_coll_list[i]);
            if (!coll_archive)
            {
                CfOut(cf_error, "",
                      "Diagnostics: failed to fetch collection (cfreport.%s) status from MongoDB",
                      cfreport_coll_list[i]);
            }
            else
            {
                DiagnosticDatabaseStatusAppendCollectionStatus(db_cfreport, coll_archive);
            }
        }

        DiagnosticServerStatusAppendDatabaseStatus(service_status, db_cfreport);
    }

    return service_status;
}

void DiagnosticsMakeMongoSnapshot(time_t time_id)
{
    EnterpriseDB conn;
    if (CFDB_Open(&conn))
    {
        /* Get Snapshot */
        DiagnosticServerStatus *service_status = NULL;
        service_status = DiagnosticStatusPull(&conn);

        if (service_status)
        {
            /* Save Snapshot */
            DiagnosticStatusPush(&conn, service_status, time_id);
            DiagnosticServerStatusFree(service_status);
        }

        CFDB_Close(&conn);
    }
}

static void DiagnosticStatusPush(EnterpriseDB *conn, DiagnosticServerStatus *status,
                                 time_t time_id)
{
    assert(conn);
    assert(status);

    bson query;
    {
        bson_init(&query);
        BsonAppendInt(&query, diagnostic_dbk_time, (int)time_id);
        BsonFinish(&query);
    }

    bson set_op;
    {
        bson_init(&set_op);
        BsonAppendStartObject(&set_op, "$set");
        {
            DiagnosticServerStatusToBson(status, &set_op);
        }
        BsonAppendFinishObject(&set_op);
        BsonFinish(&set_op);
    }

    int err = MongoUpdate(conn, diagnostic_db_mongo_coll, &query, &set_op,
                          MONGO_UPDATE_UPSERT, NULL);

    if (err != MONGO_CONN_SUCCESS)
    {
        CfOut(cf_error, "",
              "Diagnostics: failed to save snapshot (time_id: %ld) to database with error code: %d",
              time_id, err);
    }

    bson_destroy(&query);
    bson_destroy(&set_op);
}

Seq *DiagnosticsQueryMongoSnapshot(EnterpriseDB *conn)
{
    assert(conn);

    Seq *snapshot_seq = NULL;
    snapshot_seq = SeqNew(1020, DiagnosticMongoSnaphotFree); // this is around 3.5 days of snapshots with 5min interval to limit memory realication

    bson query;
    bson_init(&query);
    BsonFinish(&query);

    bson fields;
    bson_init(&fields);
    BsonFinish(&fields);

    mongo_cursor *cursor = MongoFind(conn, diagnostic_db_mongo_coll, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))
    {
        bson *main_obj = (bson*) mongo_cursor_bson(cursor);
        if (BsonIsEmpty(main_obj))
        {
            continue;
        }

        time_t time_id = 0;
        BsonIntGet(main_obj, diagnostic_dbk_time, (int*)(&time_id));

        DiagnosticServerStatus *status = DiagnosticServerStatusFromBson(main_obj);

        DiagnosticMongoSnaphot *snapshot = DiagnosticMongoSnaphotCreate();
        snapshot->status = status;
        snapshot->time = time_id;

        SeqAppend(snapshot_seq, snapshot);
    }

    mongo_cursor_destroy(cursor);

    return snapshot_seq;
}
