#include "cf3.defs.h"
#include "platform.h"
#include "test.h"
#include "hub_diagnostics.c"
#include "buffer.h"

#define test_str_1 "test string 1"
#define test_str_2 "test string 2"
#define test_str_3 "test string 3"

static void test_create_reporting_hub_empty(void **state)
{
    DiagnosticReportingHost *tmp = DiagnosticReportingHostNew();
    DiagnosticReportingHostFree(tmp);
}

static void test_create_reporting_hub(void **state)
{
    DiagnosticReportingHost *tmp = DiagnosticReportingHostNew();

    tmp->kh = BufferNewFrom(test_str_1, strlen(test_str_1));

    DiagnosticReportingHostFree(tmp);
}

static void test_create_hub_snapshot_empty(void **state)
{
    DiagnosticHubSnapshot *tmp = DiagnosticHubSnapshotNew();
    DiagnosticHubSnapshotFree(tmp);
}

static void test_create_hub_snapshot(void **state)
{
    DiagnosticHubSnapshot *tmp = DiagnosticHubSnapshotNew();

    tmp->largest_data_size_host_id = BufferNewFrom(test_str_1, strlen(test_str_1));
    tmp->largest_duration_time_host_id = BufferNewFrom(test_str_2, strlen(test_str_2));

    Seq *list = SeqNew(1, DiagnosticReportingHostFree);
    for (size_t i = 0; i < 10; i++)
    {
        DiagnosticReportingHost *host_stats = DiagnosticReportingHostNew();
        host_stats->data_len = 123;
        host_stats->kh = BufferNewFrom(test_str_3, strlen(test_str_3));
        host_stats->status = 3;
        host_stats->time = 123;

        SeqAppend(list, host_stats);
    }

    tmp->collection_failed_list = list;

    DiagnosticHubSnapshotFree(tmp);
}

static void test_analyze(void **state)
{
    Seq *list = SeqNew(1, DiagnosticReportingHostFree);

    {
        DiagnosticHubSnapshot *tmp = DiagnosticAnalizeHostRecords(list);
        assert_true( tmp == NULL );
    }

    for (size_t i = 0; i < 10; i++)
    {
        DiagnosticReportingHost *host_stats = DiagnosticReportingHostNew();
        host_stats->data_len = 100;
        host_stats->kh = BufferNewFrom(test_str_1, strlen(test_str_1));
        host_stats->status = HUBDIAG_SUCCESS;
        host_stats->time = 10;

        SeqAppend(list, host_stats);
    }

    {
        DiagnosticHubSnapshot *tmp = DiagnosticAnalizeHostRecords(list);
        assert_true( tmp != NULL );

        assert_int_equal( tmp->avg_data_size, 100 );
        assert_int_equal( (int)(tmp->avg_duration_time), 10 );
        assert_int_equal( tmp->collected_report_count, 10 );
        assert_true( tmp->collection_failed_list == NULL);
        assert_int_equal( tmp->largest_data_size, 100 );
        assert_string_equal( BufferData(tmp->largest_data_size_host_id), test_str_1 );
        assert_int_equal( (int)(tmp->largest_duration_time), 10);
        assert_string_equal( BufferData(tmp->largest_duration_time_host_id), test_str_1);

        DiagnosticHubSnapshotFree(tmp);
    }

    for (size_t i = 0; i < 10; i++)
    {
        DiagnosticReportingHost *host_stats = DiagnosticReportingHostNew();
        host_stats->data_len = 200;
        host_stats->kh = BufferNewFrom(test_str_2, strlen(test_str_2));
        host_stats->status = HUBDIAG_SUCCESS;
        host_stats->time = 20;

        SeqAppend(list, host_stats);
    }

    {
        DiagnosticHubSnapshot *tmp = DiagnosticAnalizeHostRecords(list);
        assert_true( tmp != NULL );

        assert_int_equal( tmp->avg_data_size, 150 );
        assert_int_equal( (int)(tmp->avg_duration_time), 15 );
        assert_int_equal( tmp->collected_report_count, 20 );
        assert_int_equal( tmp->largest_data_size, 200);
        assert_string_equal( BufferData(tmp->largest_data_size_host_id), test_str_2 );
        assert_int_equal( (int)(tmp->largest_duration_time), 20);
        assert_string_equal( BufferData(tmp->largest_duration_time_host_id), test_str_2);
        assert_true( tmp->collection_failed_list == NULL );

        DiagnosticHubSnapshotFree(tmp);
    }

    for (size_t i = 0; i < 10; i++)
    {
        DiagnosticReportingHost *host_stats = DiagnosticReportingHostNew();
        host_stats->data_len = 200;
        host_stats->kh = BufferNewFrom(test_str_3, strlen(test_str_3));
        host_stats->status = HUBDIAG_SERVER_NO_REPLY;
        host_stats->time = 20;

        SeqAppend(list, host_stats);
    }

    {
        DiagnosticHubSnapshot *tmp = DiagnosticAnalizeHostRecords(list);
        assert_true( tmp != NULL );

        assert_int_equal( tmp->avg_data_size, 150 );
        assert_int_equal( (int)(tmp->avg_duration_time), 15 );
        assert_int_equal( tmp->collected_report_count, 20 );
        assert_int_equal( tmp->largest_data_size, 200 );
        assert_string_equal( BufferData(tmp->largest_data_size_host_id), test_str_2 );
        assert_int_equal( (int)(tmp->largest_duration_time), 20);
        assert_string_equal( BufferData(tmp->largest_duration_time_host_id), test_str_2);
        assert_true( tmp->collection_failed_list != NULL);
        assert_int_equal( SeqLength(tmp->collection_failed_list), 10);

        for (size_t i = 0; i < SeqLength(tmp->collection_failed_list); i++)
        {
            DiagnosticReportingHost *host_stats = SeqAt(tmp->collection_failed_list, i);

            assert_int_equal(host_stats->status, HUBDIAG_SERVER_NO_REPLY);
            assert_string_equal(BufferData(host_stats->kh), test_str_3);
        }

        DiagnosticHubSnapshotFree(tmp);
    }

    SeqDestroy(list);
}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_create_reporting_hub_empty),
        unit_test(test_create_reporting_hub),
        unit_test(test_create_hub_snapshot_empty),
        unit_test(test_create_hub_snapshot),
        unit_test(test_analyze)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}

