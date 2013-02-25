/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "hub_diagnostics.h"

#include "alloc.h"
#include "bson_lib.h"
#include "instrumentation.h"
#include "platform.h"

#include <bson.h>
#include <assert.h>
#include <cfstream.h>
#include <mongo.h>

/* db namespace */
#define diagnostic_db_tmp "tmp"
#define diagnostic_mongo_tmp diagnostic_db "."diagnostic_db_tmp

#define diagnostic_db_tmp_keyhash "_id"
#define diagnostic_db_tmp_data_size "data_size"
#define diagnostic_db_tmp_status_code "status"
#define diagnostic_db_tmp_duration "duration"

#define diagnostic_db_hub_total_duration "total_duration"
#define diagnostic_db_hub_avg_duration "avg_duration"
#define diagnostic_db_hub_largest_duration "lg_duration"
#define diagnostic_db_hub_largest_duration_kh "lg_duration_kh"
#define diagnostic_db_hub_avg_size "avg_size"
#define diagnostic_db_hub_largest_size "lg_size"
#define diagnostic_db_hub_largest_size_kh "lg_size_kh"
#define diagnostic_db_hub_hosts_reported "reported_cnt"
#define diagnostic_db_hub_failed "failed_reports"
#define diagnostic_db_hub_status "status"
#define diagnostic_db_hub_analyze "analyze_performance"

#define LABEL_HUBDIAG_SERVER_NO_REPLY "ServerNoReply"
#define LABEL_HUBDIAG_SERVER_AUTH_ERROR "ServerAuthenticationError"
#define LABEL_HUBDIAG_INVALID_DATA "InvalidData"
#define LABEL_HUBDIAG_SUCCESS "Success"
#define LABEL_HUBDIAG_Unknown "Unknown"

static DiagnosticReportingHost *DiagnosticReportingHostNew();
static void DiagnosticReportingHostFree(DiagnosticReportingHost *ptr);
static void  DiagnosticReportingHostStore(EnterpriseDB *conn,
                                          DiagnosticReportingHost *diagnose);
static Seq *DiagnosticReportingHostsFetch(EnterpriseDB *conn);
static DiagnosticHubSnapshot *DiagnosticHubSnapshotNew(void);
static void DiagnosticHubSnapshotFree(DiagnosticHubSnapshot *ptr);
static DiagnosticHubSnapshot *DiagnosticFetchAndAnalizeHostRecords(EnterpriseDB *conn);
static void DiagnosticHubSnapshotStore(EnterpriseDB *conn, time_t time_id,
                                       DiagnosticHubSnapshot *data);
static void DiagnosticHubSnapshotToBson(DiagnosticHubSnapshot *data, bson *out);
static DiagnosticHubSnapshot *DiagnosticAnalizeHostRecords(Seq *sample_list);
static DiagnosticHubSnapshot *DiagnosticHubSnapshotFromBson(bson *in);


static DiagnosticReportingHost *DiagnosticReportingHostNew()
{
    DiagnosticReportingHost *ptr = NULL;
    ptr = (DiagnosticReportingHost*) xmalloc(sizeof(DiagnosticReportingHost));

    ptr->data_len = -1;
    ptr->time = -1;
    ptr->kh = NULL;
    ptr->status = HUBDIAG_UNKNOWN;

    return ptr;
}

static void DiagnosticReportingHostFree(DiagnosticReportingHost *ptr)
{
    if (ptr)
    {
        if (ptr->kh)
        {
            BufferDestroy(&(ptr->kh));
        }
    }

    free(ptr);
}

void DiagnosticReportHostSave(EnterpriseDB *conn, Buffer *kh, double duration,
                              int data_len, ReportCollectionStatus status)
{
    assert(conn);
    assert(kh);

    if (!conn)
    {
        return;
    }

    if (!kh || BufferSize(kh) == 0)
    {
        return;
    }

    DiagnosticReportingHost *diagnose = DiagnosticReportingHostNew();

    diagnose->data_len = (int) (data_len / 1024); // convert form bytes to kb
    diagnose->time = duration;
    diagnose->status = status;
    diagnose->kh = kh;

    DiagnosticReportingHostStore(conn, diagnose);

    DiagnosticReportingHostFree(diagnose);
}

static void  DiagnosticReportingHostStore(EnterpriseDB *conn,
                                          DiagnosticReportingHost *diagnose)
{
    assert(conn);
    assert(diagnose);

    bson insert;
    {
        bson_init(&insert);
        BsonAppendString(&insert, diagnostic_db_tmp_keyhash, BufferData(diagnose->kh));
        BsonAppendInt(&insert, diagnostic_db_tmp_status_code, (int)diagnose->status);
        BsonAppendDouble(&insert, diagnostic_db_tmp_duration, diagnose->time);
        BsonAppendInt(&insert, diagnostic_db_tmp_data_size, diagnose->data_len);
        BsonFinish(&insert);
    }

    MongoInsert(conn, diagnostic_mongo_tmp, &insert, NULL);

    bson_destroy(&insert);
}

static DiagnosticHubSnapshot *DiagnosticHubSnapshotNew(void)
{
    DiagnosticHubSnapshot *ptr = NULL;
    ptr = (DiagnosticHubSnapshot*) xmalloc(sizeof(DiagnosticHubSnapshot));

    ptr->total_duration_time = 0.0;
    ptr->avg_duration_time = 0.0;
    ptr->largest_duration_time = 0.0;
    ptr->largest_duration_time_host_id = NULL;
    ptr->avg_data_size = 0;
    ptr->largest_data_size = 0;
    ptr->largest_data_size_host_id = NULL;
    ptr->timestamp = 0;
    ptr->collected_report_count = 0;
    ptr->collection_failed_list = NULL;
    ptr->sample_analyze_duration = 0.0;

    return ptr;
}

static void DiagnosticHubSnapshotFree(DiagnosticHubSnapshot *ptr)
{
    if (ptr)
    {
        if (ptr->largest_duration_time_host_id)
        {
            BufferDestroy(&(ptr->largest_duration_time_host_id));
        }

        if (ptr->largest_data_size_host_id)
        {
            BufferDestroy(&(ptr->largest_data_size_host_id));
        }

        if (ptr->collection_failed_list)
        {
            SeqDestroy(ptr->collection_failed_list);
        }
    }

    free(ptr);
}

void DropReportingHostsTmp(void)
{
    EnterpriseDB conn;
    if (CFDB_Open(&conn))
    {
        bson result = { 0 };
        if (!MongoDropCollection(&conn, diagnostic_db, diagnostic_db_tmp, &result))
        {
            CfOut(OUTPUT_LEVEL_ERROR, "","Diagnostic: Failed to remove tmp sample database (%s) -- db error", diagnostic_mongo_tmp);
        }

        CFDB_Close(&conn);
    }
    else
    {
        CfOut(OUTPUT_LEVEL_ERROR, "","Diagnostic: Failed to remove tmp sample database (%s) -- failed to connect", diagnostic_mongo_tmp);
    }
}

static Seq *DiagnosticReportingHostsFetch(EnterpriseDB *conn)
{
    assert(conn);

    if (!conn)
    {
        return NULL;
    }

    Seq *ret = SeqNew(1000, DiagnosticReportingHostFree);

    bson query;
    bson_init(&query);
    BsonFinish(&query);

    bson fields;
    bson_init(&fields);
    BsonFinish(&fields);

    mongo_cursor *cursor = MongoFind(conn, diagnostic_mongo_tmp,
                                     &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))
    {
        bson *main_obj = (bson*) mongo_cursor_bson(cursor);
        if (BsonIsEmpty(main_obj))
        {
            continue;
        }

        int data_len = 0;
        if (!BsonIntGet(main_obj, diagnostic_db_tmp_data_size, &data_len))
        {
            continue;
        }

        double time = 0.0;
        if (!BsonDoubleGet(main_obj, diagnostic_db_tmp_duration, &time))
        {
            continue;
        }

        const char *kh = NULL;
        if (!BsonStringGet(main_obj, diagnostic_db_tmp_keyhash, &kh))
        {
            continue;
        }

        ReportCollectionStatus status = HUBDIAG_UNKNOWN;
        if (!BsonIntGet(main_obj, diagnostic_db_tmp_status_code, (int*)&status))
        {
            continue;
        }

        DiagnosticReportingHost *host_stats = DiagnosticReportingHostNew();
        host_stats->data_len = data_len;
        host_stats->kh = BufferNewFrom(kh, strlen(kh));
        host_stats->status = status;
        host_stats->time = time;

        SeqAppend(ret, host_stats);
    }

    mongo_cursor_destroy(cursor);

    return ret;
}

static DiagnosticHubSnapshot *DiagnosticFetchAndAnalizeHostRecords(EnterpriseDB *conn)
{
    assert(conn);

    Seq *sample_list = DiagnosticReportingHostsFetch(conn);

    if (!sample_list)
    {
        return NULL;
    }

    DiagnosticHubSnapshot *snapshot = DiagnosticAnalizeHostRecords(sample_list);

    SeqDestroy(sample_list);

    return snapshot;
}

static DiagnosticHubSnapshot *DiagnosticAnalizeHostRecords(Seq *sample_list)
{
    assert(sample_list);

    if (!sample_list)
    {
        return NULL;
    }

    int cnt_reported = 0;
    long double duration_sum = 0.0;
    long int data_size_sum = 0;

    int largest_duration_id = 0;
    int largest_data_size_id = 0;

    Seq *failed_list = SeqNew(20, DiagnosticReportingHostFree);

    bool has_records = false;

    for (size_t i = 0; i < SeqLength(sample_list); i++)
    {
        DiagnosticReportingHost *sample = SeqAt(sample_list, i);
        if (!sample)
        {
            continue;
        }

        has_records = true;

        if (sample->status == HUBDIAG_SUCCESS)
        {
            cnt_reported++;
            duration_sum += sample->time;
            data_size_sum += sample->data_len;

            DiagnosticReportingHost *old_time = SeqAt(sample_list, largest_duration_id);
            if (old_time->time < sample->time)
            {
                largest_duration_id = i;
            }

            DiagnosticReportingHost *old_size = SeqAt(sample_list, largest_data_size_id);
            if (old_size->data_len < sample->data_len)
            {
                largest_data_size_id = i;
            }
        }
        else
        {
            DiagnosticReportingHost *host_stat = DiagnosticReportingHostNew();
            host_stat->status = sample->status;
            BufferCopy(sample->kh, &(host_stat->kh));

            SeqAppend(failed_list, (void*)host_stat);
        }
    }

    if (has_records)
    {
        DiagnosticHubSnapshot *snapshot = DiagnosticHubSnapshotNew();

        snapshot->avg_data_size = (int) (data_size_sum / cnt_reported);
        snapshot->avg_duration_time = (double) (duration_sum / (double) cnt_reported);
        snapshot->collected_report_count = cnt_reported;

        DiagnosticReportingHost *old_time = SeqAt(sample_list, largest_duration_id);
        snapshot->largest_duration_time = old_time->time;
        BufferCopy(old_time->kh, &(snapshot->largest_duration_time_host_id));

        DiagnosticReportingHost *old_size = SeqAt(sample_list, largest_data_size_id);
        snapshot->largest_data_size = old_size->data_len;
        BufferCopy(old_size->kh, &(snapshot->largest_data_size_host_id));

        if (SeqLength(failed_list) > 0)
        {
            snapshot->collection_failed_list = failed_list;
        }

        return snapshot;
    }

    SeqDestroy(failed_list);
    return NULL;
}

void DiagnosticMakeHubSnapshot(time_t time_id, double total_time)
{
    EnterpriseDB conn;
    if (CFDB_Open(&conn))
    {
        struct timespec perf_time = BeginMeasure();
        DiagnosticHubSnapshot *snap = DiagnosticFetchAndAnalizeHostRecords(&conn);
        if (snap)
        {
            snap->sample_analyze_duration = EndMeasureValue(perf_time);
            snap->total_duration_time = total_time;
            snap->timestamp = time_id;
            DiagnosticHubSnapshotStore(&conn, time_id, snap);
            DiagnosticHubSnapshotFree(snap);
        }
        else
        {
            CfOut(OUTPUT_LEVEL_VERBOSE, "",
                  "Diagnostic: Failed to make hub statistics snapshot -- no data to analize (spend time: %f.3 sec)",
                  EndMeasureValue(perf_time));
        }

        CFDB_Close(&conn);
    }
    else
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "",
              "Diagnostic: Failed to make hub statistics snapshot -- failed to connect to database");
    }
}

static void DiagnosticHubSnapshotStore(EnterpriseDB *conn, time_t time_id,
                                       DiagnosticHubSnapshot *data)
{
    assert(conn);
    assert(data);

    bson query;
    {
        bson_init(&query);
        BsonAppendInt(&query, diagnostic_db_hub_time, (int)time_id);
        BsonFinish(&query);
    }

    bson set_op;
    {
        bson_init(&set_op);
        BsonAppendStartObject(&set_op, "$set");
        {
            DiagnosticHubSnapshotToBson(data, &set_op);
        }
        BsonAppendFinishObject(&set_op);
        BsonFinish(&set_op);
    }

    int err = MongoUpdate(conn, diagnostic_db_hub, &query, &set_op,
                          MONGO_UPDATE_UPSERT, NULL);

    if (err != MONGO_CONN_SUCCESS)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "",
              "Diagnostics: failed to save snapshot (time_id: %ld) to database with error code: %d",
              time_id, err);
    }

    bson_destroy(&query);
    bson_destroy(&set_op);
}

static void DiagnosticHubSnapshotToBson(DiagnosticHubSnapshot *data, bson *out)
{
    assert(data);
    assert(out);

    if (!data || !out)
    {
        return;
    }

    BsonAppendInt(out, diagnostic_db_hub_time, (int)data->timestamp);
    BsonAppendDouble(out, diagnostic_db_hub_total_duration, data->total_duration_time);
    BsonAppendDouble(out, diagnostic_db_hub_avg_duration, data->avg_duration_time);
    BsonAppendDouble(out, diagnostic_db_hub_largest_duration, data->largest_duration_time);
    BsonAppendInt(out, diagnostic_db_hub_avg_size, (int)data->avg_data_size);
    BsonAppendInt(out, diagnostic_db_hub_largest_size, (int)data->largest_data_size);
    BsonAppendInt(out, diagnostic_db_hub_hosts_reported, (int)data->collected_report_count);
    BsonAppendDouble(out, diagnostic_db_hub_analyze, data->sample_analyze_duration);

    if ((data->largest_data_size_host_id) && (BufferSize(data->largest_data_size_host_id) > 0))
    {
        BsonAppendString(out, diagnostic_db_hub_largest_size_kh,
                         BufferData(data->largest_data_size_host_id));
    }

    if ((data->largest_duration_time_host_id) && (BufferSize(data->largest_duration_time_host_id) > 0))
    {
        BsonAppendString(out, diagnostic_db_hub_largest_duration_kh,
                         BufferData(data->largest_duration_time_host_id));
    }

    if (data->collection_failed_list)
    {
        BsonAppendStartObject(out, diagnostic_db_hub_failed);
        for (size_t i = 0; i < SeqLength(data->collection_failed_list); i++)
        {
            DiagnosticReportingHost *host = SeqAt(data->collection_failed_list, i);

            if ((host) && (host->kh) && (BufferSize(host->kh) > 0))
            {
                BsonAppendStartObject(out, BufferData(host->kh));
                BsonAppendInt(out, diagnostic_db_hub_status, (int)host->status);
                BsonAppendFinishObject(out);
            }
        }
        BsonAppendFinishObject(out);
    }
}

Seq *DiagnosticQueryHubSnapshot(EnterpriseDB *conn)
{
    assert(conn);

    if (!conn)
    {
        return NULL;
    }

    Seq *ret_list = NULL;
    ret_list = SeqNew(2100, DiagnosticHubSnapshotFree); // this is around 7 days + 6h of snapshots with 5min interval to limit memory realication

    bson query;
    bson_init(&query);
    BsonFinish(&query);

    bson fields;
    bson_init(&fields);
    BsonFinish(&fields);

    mongo_cursor *cursor = MongoFind(conn, diagnostic_db_hub, &query, &fields,
                                     0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))
    {
        bson *main_obj = (bson*) mongo_cursor_bson(cursor);
        if (BsonIsEmpty(main_obj))
        {
            continue;
        }

        DiagnosticHubSnapshot * snapshot = DiagnosticHubSnapshotFromBson(main_obj);
        if (snapshot)
        {
            SeqAppend(ret_list, snapshot);
        }
    }

    mongo_cursor_destroy(cursor);

    return ret_list;

}

static DiagnosticHubSnapshot *DiagnosticHubSnapshotFromBson(bson *in)
{
    assert(in);

    bool err = false;
    DiagnosticHubSnapshot *snap = DiagnosticHubSnapshotNew();

    if (!BsonIntGet(in, diagnostic_db_hub_time, (int*)&(snap->timestamp)))
    {
        err = true;
    }

    if (!BsonDoubleGet(in, diagnostic_db_hub_total_duration, &(snap->total_duration_time)))
    {
        err = true;
    }

    if (!BsonDoubleGet(in, diagnostic_db_hub_avg_duration, &(snap->avg_duration_time)))
    {
        err = true;
    }

    if (!BsonDoubleGet(in, diagnostic_db_hub_largest_duration, &(snap->largest_duration_time)))
    {
        err = true;
    }

    const char *lg_duration_kh = NULL;
    if (!BsonStringGet(in, diagnostic_db_hub_largest_duration_kh, &lg_duration_kh))
    {
        err = true;
    }
    snap->largest_duration_time_host_id = BufferNewFrom(lg_duration_kh, strlen(lg_duration_kh));

    if (!BsonIntGet(in, diagnostic_db_hub_avg_size, &(snap->avg_data_size)))
    {
        err = true;
    }

    if (!BsonIntGet(in, diagnostic_db_hub_largest_size, &(snap->largest_data_size)))
    {
        err = true;
    }

    const char *lg_size_kh = NULL;
    if (!BsonStringGet(in, diagnostic_db_hub_largest_size_kh, &lg_size_kh))
    {
        err = true;
    }
    snap->largest_data_size_host_id = BufferNewFrom(lg_size_kh, strlen(lg_size_kh));

    if (!BsonIntGet(in, diagnostic_db_hub_hosts_reported, &(snap->collected_report_count)))
    {
        err = true;
    }

    if (!BsonDoubleGet(in, diagnostic_db_hub_analyze, &(snap->sample_analyze_duration)))
    {
        err = true;
    }

    bson failed;
    if (BsonObjectGet(in, diagnostic_db_hub_failed, &failed))
    {
        Seq *failed_list = SeqNew(20, DiagnosticReportingHostFree);

        bson_iterator it;
        bson_iterator_init(&it, &failed);

        while (BsonIsTypeValid(bson_iterator_next(&it)) > 0)
        {
            const char *key = bson_iterator_key(&it);
            if (key)
            {
                if (bson_iterator_type(&it) == BSON_OBJECT)
                {
                    bson status_b;
                    bson_iterator_subobject(&it, &status_b);

                    ReportCollectionStatus code = HUBDIAG_UNKNOWN;
                    if (BsonIntGet(&status_b, diagnostic_db_hub_status, (int*)&(code)))
                    {
                        DiagnosticReportingHost *host = DiagnosticReportingHostNew();
                        host->kh = BufferNewFrom(key, strlen(key));
                        host->status = code;

                        SeqAppend(failed_list, host);
                    }
                }
            }
        }

        if (SeqLength(failed_list) > 0)
        {
            snap->collection_failed_list = failed_list;
        }
        else
        {
            SeqDestroy(failed_list);
        }

    }

    if (err)
    {
        DiagnosticHubSnapshotFree(snap);
        return NULL;
    }

    return snap;
}

const char *ReportCollectionStatusToString(ReportCollectionStatus status)
{
    switch (status)
    {
        case HUBDIAG_SERVER_NO_REPLY:
            return LABEL_HUBDIAG_SERVER_NO_REPLY;
        case HUBDIAG_SERVER_AUTH_FAILURE:
            return LABEL_HUBDIAG_SERVER_AUTH_ERROR;
        case HUBDIAG_INVALID_DATA:
            return LABEL_HUBDIAG_INVALID_DATA;
        case HUBDIAG_SUCCESS:
            return LABEL_HUBDIAG_SUCCESS;
        default:
            break;
    }

    return LABEL_HUBDIAG_SUCCESS;
}
