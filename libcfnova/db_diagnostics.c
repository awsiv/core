/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "db_diagnostics.h"

#include "db_common.h"
#include "bson_lib.h"
#include "alloc.h"
#include "sequence.h"
#include "cfstream.h"

#include <bson.h>
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
#define diagnostic_dbk_bgflush_last_finish "lastFinished"

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
#define diagnostic_dbk_collection_index_details "indexSizes"

/*
 * Internal data structures
 */

typedef struct
{
    char *name;
    int size;
} DiagnosticIndexInfo;

typedef struct
{
    char *name;
    int object_count;
    int data_size;
    double avg_object_size;
    int storage_size;
    int index_count;
    double padding_factor;
    int total_index_size;
    Seq *index_list; /* DiagnosticsIndexInfo list */
} DiagnosticCollectionStatus;

typedef struct
{
    char *db_name;
    int object_count;
    double avg_object_size;
    int data_size;
    int storage_size;
    int file_size;
    Seq *collection_list; /* DiagnosticCollectionStatus list */
} DiagnosticDatabaseStatus;

typedef struct
{
    char *host;
    char *version;
    double server_uptime;
    double global_lock_total_time_us;
    double global_lock_lock_time_us;
    int global_lock_queue_total;
    int global_lock_queue_readers;
    int global_lock_queue_writers;
    int mem_resident;
    int mem_virtual;
    int mem_mapped;
    int bg_flush_count;
    int bg_flush_total_ms;
    int bg_flush_avg_ms;
    int bg_flush_last_ms;
    Seq *db_list; /* DiagnosticDatabaseStatus list */
} DiagnosticServerStatus;

/*
 * Internal utilities
 */

static DiagnosticServerStatus *DiagnosticServerStatusCreate(void);
static void DiagnosticServerStatusFree(DiagnosticServerStatus *ptr);
/* transfer ownership */
static void DiagnosticServerStatusAppendDatabaseStatus(DiagnosticServerStatus *server,
                                                       DiagnosticDatabaseStatus *database);
static void DiagnosticServerStatusToBson(DiagnosticServerStatus *in, bson *out);

static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusCreate(void);
static void DiagnosticDatabaseStatusFree(DiagnosticDatabaseStatus *ptr);
/* transfer ownership */
static void DiagnosticDatabaseStatusAppendCollectionStatus(DiagnosticDatabaseStatus *database,
                                                           DiagnosticCollectionStatus *collection);
static void DiagnosticDatabaseStatusToBson(DiagnosticDatabaseStatus *in, bson *out);

static DiagnosticIndexInfo *DiagnosticIndexInfoCreate(void);
static void DiagnosticIndexInfoFree(DiagnosticIndexInfo *ptr);
static void DiagnosticIndexInfoToBson(DiagnosticIndexInfo *info, bson *out);

static DiagnosticCollectionStatus *DiagnosticCollectionStatusCreate(void);
static void DiagnosticCollectionStatusFree(DiagnosticCollectionStatus *ptr);
/* transfer ownership */
static void DiagnosticCollectionStatusAppendIndexInfo(DiagnosticCollectionStatus *collection,
                                                      DiagnosticIndexInfo *info);
static void DiagnosticCollectionStatusToBson(DiagnosticCollectionStatus *stat, bson *out);

static DiagnosticServerStatus *DiagnosticServerStatusGet(EnterpriseDB *conn);
static DiagnosticDatabaseStatus *DiagnosticDatabaseStatusGet(EnterpriseDB *conn,
                                                             const char *db_name);
static DiagnosticCollectionStatus *DiagnosticCollectionStatusGet(EnterpriseDB *conn ,
                                                                 const char *db_name,
                                                                 const char *collection_name);

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

static DiagnosticIndexInfo *DiagnosticIndexInfoCreate(void)
{
    DiagnosticIndexInfo *index_info = NULL;
    index_info = xmalloc(sizeof(DiagnosticIndexInfo));

    index_info->name = NULL;

    return index_info;
}

static void DiagnosticIndexInfoFree(DiagnosticIndexInfo *ptr)
{
    if (!ptr)
    {
        free(ptr->name);
    }

    free(ptr);
}

static void DiagnosticIndexInfoToBson(DiagnosticIndexInfo *info, bson *out)
{
    assert(info);
    assert(out);

    BsonAppendInt(out, info->name, info->size);
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
    status->index_list = NULL;

    return status;
}

static void DiagnosticCollectionStatusFree(DiagnosticCollectionStatus *ptr)
{
    if (ptr)
    {
        free(ptr->name);
        if (ptr->index_list)
        {
            SeqDestroy(ptr->index_list);
        }
    }

    free(ptr);
}

static void DiagnosticCollectionStatusAppendIndexInfo(DiagnosticCollectionStatus *collection,
                                                      DiagnosticIndexInfo *info)
{
    assert(info);
    assert(collection);

    if (!collection->index_list)
    {
        collection->index_list = SeqNew(5, DiagnosticIndexInfoFree);
    }

    SeqAppend(collection->index_list, info);
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

        if (in->index_list)
        {
            BsonAppendStartObject(out, diagnostic_dbk_collection_index_details);

            for (int i = 0; i < SeqLength(in->index_list); i++)
            {
                DiagnosticIndexInfoToBson(SeqAt(in->index_list, i), out);
            }
            BsonAppendFinishObject(out);
        }
    }
    BsonAppendFinishObject(out);
}


static DiagnosticServerStatus *DiagnosticServerStatusGet(EnterpriseDB *conn)
{
    assert(conn);

    DiagnosticServerStatus *status = NULL;

    bson cmd;
    bson out = {0};

    bson_init(&cmd);
    BsonAppendString(&cmd, "serverStatus", "1");
    BsonAppendInt(&cmd, "scale", 1024);
    BsonFinish(&cmd);

    if (MongoRunCommand(conn, "", &cmd, &out) == MONGO_OK)
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

        bson index_sizes;
        if (BsonObjectGet(&out, "indexSizes", &index_sizes))
        {
            bson_iterator it;
            bson_iterator_init(&it, &index_sizes);

            while (bson_iterator_next(&it))
            {
                if (bson_iterator_type(&it) == BSON_INT)
                {
                    DiagnosticIndexInfo *index_info = DiagnosticIndexInfoCreate();

                    index_info->name = xstrdup(bson_iterator_key(&it));
                    index_info->size = bson_iterator_int(&it);

                    DiagnosticCollectionStatusAppendIndexInfo(collection, index_info);
                }
            }
        }

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
        BsonAppendInt(&set_op, diagnostic_dbk_time, (int)time_id);
        DiagnosticServerStatusToBson(status, &set_op);
        BsonFinish(&set_op);
    }

    int err = MongoUpdate(conn, diagnostic_db_mongo_coll, &query, &set_op,
                          MONGO_UPDATE_UPSERT, NULL);

    if (err != MONGO_CONN_SUCCESS)
    {
        CfOut(cf_error, "",
              "Diagnostics: failed to save snapshot (time_id: %d) to database with error code: %d",
              time_id, err);
    }

    bson_destroy(&query);
    bson_destroy(&set_op);
}


