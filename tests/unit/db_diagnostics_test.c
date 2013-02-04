#include "cf3.defs.h"
#include "platform.h"
#include "test.h"
#include "db_diagnostics.c"

#include <bson.h>

static DiagnosticCollectionStatus *BuildTestCollection(void);
static void TestCollection(DiagnosticCollectionStatus *in);
static DiagnosticDatabaseStatus *BuildTestDatabase(void);
static void TestDatabase(DiagnosticDatabaseStatus *in);
static DiagnosticServerStatus *BuildTestServer(void);
static void TestServer(DiagnosticServerStatus *in);

static DiagnosticCollectionStatus *BuildTestCollection(void)
{
    DiagnosticCollectionStatus *in = DiagnosticCollectionStatusCreate();
    in->name = xstrdup("collection123");
    in->avg_object_size = 123.123;
    in->data_size = 123;
    in->index_count = 12;
    in->object_count = 1234;
    in->padding_factor = 12.12;
    in->storage_size = 12345;
    in->total_index_size = 321;

    return in;
}

static void TestCollection(DiagnosticCollectionStatus *in)
{
    assert_string_equal(in->name, "collection123");
//    assert_double_close(in->avg_object_size, 123.123);
    assert_int_equal(in->data_size, 123);
    assert_int_equal(in->index_count, 12);
    assert_int_equal(in->object_count, 1234);
//    assert_double_close(in->padding_factor, 12.12);
    assert_int_equal(in->storage_size, 12345);
    assert_int_equal(in->total_index_size, 321);
}

static DiagnosticDatabaseStatus *BuildTestDatabase(void)
{
    DiagnosticDatabaseStatus *in = DiagnosticDatabaseStatusCreate();
    in->avg_object_size = 12.12;
    in->data_size = 23;
    in->db_name = xstrdup("cfreport");
    in->file_size = 123;
    in->object_count = 321;
    in->storage_size = 765;

    return in;
}

static void TestDatabase(DiagnosticDatabaseStatus *in)
{
//    assert_double_close(in->avg_object_size, 12.12);
    assert_int_equal(in->data_size, 23);
    assert_string_equal(in->db_name, "cfreport");
    assert_int_equal(in->file_size, 123);
    assert_int_equal(in->object_count, 321);
    assert_int_equal(in->storage_size, 765);
}

static DiagnosticServerStatus *BuildTestServer(void)
{
    DiagnosticServerStatus *in = DiagnosticServerStatusCreate();
    in->host = xstrdup("test.cfengine.com");
    in->version = xstrdup("2.2.2");
    in->server_uptime = 1.1;
    in->global_lock_total_time_us = 12.12;
    in->global_lock_lock_time_us = 123.12;
    in->global_lock_queue_total = 123;
    in->global_lock_queue_readers = 12;
    in->global_lock_queue_writers = 1234;
    in->mem_resident = 12345;
    in->mem_virtual = 54321;
    in->mem_mapped = 321;
    in->bg_flush_count = 7654;
    in->bg_flush_total_ms = 98765;
    in->bg_flush_avg_ms = 432;
    in->bg_flush_last_ms = 678;

    return in;
}

static void TestServer(DiagnosticServerStatus *in)
{
    assert_string_equal(in->host, "test.cfengine.com");
    assert_string_equal(in->version, "2.2.2");
//    assert_double_close_equal(in->server_uptime, 1.1);
//    assert_double_close_equal(in->global_lock_total_time_us, 12.12);
//    assert_double_close_equal(in->global_lock_lock_time_us, 123.12);
    assert_int_equal(in->global_lock_queue_total, 123);
    assert_int_equal(in->global_lock_queue_readers, 12);
    assert_int_equal(in->global_lock_queue_writers, 1234);
    assert_int_equal(in->mem_resident, 12345);
    assert_int_equal(in->mem_virtual, 54321);
    assert_int_equal(in->mem_mapped, 321);
    assert_int_equal(in->bg_flush_count, 7654);
    assert_int_equal(in->bg_flush_total_ms, 98765);
    assert_int_equal(in->bg_flush_avg_ms, 432);
    assert_int_equal(in->bg_flush_last_ms, 678);
}

static void test_create_server_status(void **state)
{
    DiagnosticServerStatus *tmp = BuildTestServer();
    DiagnosticServerStatusFree(tmp);
}

static void test_create_server_status_empty(void **state)
{
    DiagnosticServerStatus *tmp = DiagnosticServerStatusCreate();
    DiagnosticServerStatusFree(tmp);
}

static void test_create_database_status(void **state)
{
    DiagnosticDatabaseStatus *tmp = BuildTestDatabase();
    DiagnosticDatabaseStatusFree(tmp);
}

static void test_create_database_status_empty(void **state)
{
    DiagnosticDatabaseStatus *tmp = DiagnosticDatabaseStatusCreate();
    DiagnosticDatabaseStatusFree(tmp);
}

static void test_create_collection_status(void **state)
{
    DiagnosticCollectionStatus *tmp = BuildTestCollection();
    DiagnosticCollectionStatusFree(tmp);
}

static void test_create_collection_status_empty(void **state)
{
    DiagnosticCollectionStatus *tmp = DiagnosticCollectionStatusCreate();
    DiagnosticCollectionStatusFree(tmp);
}

static void test_create_snapshot(void **state)
{
    DiagnosticMongoSnaphot *tmp = DiagnosticMongoSnaphotCreate();
    DiagnosticServerStatus *db = BuildTestServer();
    tmp->status = db;

    DiagnosticMongoSnaphotFree(tmp);
}

static void test_create_snapshot_empty(void **state)
{
    DiagnosticMongoSnaphot *tmp = DiagnosticMongoSnaphotCreate();
    DiagnosticMongoSnaphotFree(tmp);
}

static void test_append_collection(void **state)
{
    DiagnosticDatabaseStatus *tmp_db = DiagnosticDatabaseStatusCreate();

    for (int i = 0; i < 10; i++)
    {
        DiagnosticCollectionStatus *tmp_coll = BuildTestCollection();
        DiagnosticDatabaseStatusAppendCollectionStatus(tmp_db, tmp_coll);
    }

    for (int i = 0; i < 10; i++)
    {
        TestCollection(SeqAt(tmp_db->collection_list, i));
    }

    DiagnosticDatabaseStatusFree(tmp_db);
}

static void test_append_database(void **state)
{
    DiagnosticServerStatus *tmp = DiagnosticServerStatusCreate();

    for (int i = 0; i < 10; i++)
    {
        DiagnosticDatabaseStatus *db = BuildTestDatabase();
        DiagnosticServerStatusAppendDatabaseStatus(tmp, db);
    }

    for (int i = 0; i < 10; i++)
    {
        TestDatabase(SeqAt(tmp->db_list, i));
    }

    DiagnosticServerStatusFree(tmp);
}

static void test_collection_from_to_bson(void **state)
{
    DiagnosticCollectionStatus *in = BuildTestCollection();

    bson bb = {0};
    bson_init(&bb);
    DiagnosticCollectionStatusToBson(in, &bb);
    BsonFinish(&bb);

    bson bb_o = {0};
    BsonObjectGet(&bb, in->name, &bb_o);
    DiagnosticCollectionStatus *out = DiagnosticCollectionStatusFromBson(&bb_o);
    bson_destroy(&bb);

    TestCollection(out);

    DiagnosticCollectionStatusFree(in);
    DiagnosticCollectionStatusFree(out);
}

static void test_database_from_to_bson(void **state)
{
    DiagnosticDatabaseStatus *in = BuildTestDatabase();

    for (int i = 0; i < 10; i++)
    {
        DiagnosticCollectionStatus *tmp_coll = BuildTestCollection();
        DiagnosticDatabaseStatusAppendCollectionStatus(in, tmp_coll);
    }

    bson bb = {0};
    bson_init(&bb);
    DiagnosticDatabaseStatusToBson(in, &bb);
    BsonFinish(&bb);

    bson bb_o = {0};
    BsonObjectGet(&bb, in->db_name, &bb_o);
    DiagnosticDatabaseStatus *out = DiagnosticDatabaseStatusFromBson(&bb_o);
    bson_destroy(&bb);

    TestDatabase(out);

    for (int i = 0; i < 10; i++)
    {
        TestCollection(SeqAt(out->collection_list, i));
    }

    DiagnosticDatabaseStatusFree(in);
    DiagnosticDatabaseStatusFree(out);
}

static void test_server_from_to_bson(void **state)
{
    DiagnosticServerStatus *in = BuildTestServer();

    for (int i = 0; i < 10; i++)
    {
        DiagnosticDatabaseStatus *tmp_db = BuildTestDatabase();
        DiagnosticServerStatusAppendDatabaseStatus(in, tmp_db);
    }

    bson bb = {0};
    bson_init(&bb);
    DiagnosticServerStatusToBson(in, &bb);
    BsonFinish(&bb);

    DiagnosticServerStatus *out = DiagnosticServerStatusFromBson(&bb);
    bson_destroy(&bb);

    TestServer(out);

    for (int i = 0; i < 10; i++)
    {
        TestDatabase(SeqAt(out->db_list, i));
    }

    DiagnosticServerStatusFree(in);
    DiagnosticServerStatusFree(out);
}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_create_server_status),
        unit_test(test_create_server_status_empty),
        unit_test(test_create_database_status),
        unit_test(test_create_database_status_empty),
        unit_test(test_create_collection_status),
        unit_test(test_create_collection_status_empty),
        unit_test(test_create_snapshot),
        unit_test(test_create_snapshot_empty),
        unit_test(test_append_collection),
        unit_test(test_append_database),
        unit_test(test_collection_from_to_bson),
        unit_test(test_database_from_to_bson),
        unit_test(test_server_from_to_bson)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
