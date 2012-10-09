#include "test.h"
#include "cf.nova.h"
#include "db_query.h"

#if defined(HAVE_LIBMONGOC)

static void test_query_promiselog_repaired(void **state)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(conn, NULL, PROMISE_LOG_STATE_REPAIRED, NULL, false, NULL,
                                        0, time(NULL), false, NULL, NULL, PROMISE_CONTEXT_MODE_ALL, NULL);
    assert_true(hq != NULL);

    DeleteHubQuery(hq, DeleteHubPromiseLog);
    assert_true(CFDB_Close(conn));
}

static void test_query_total_compliance(void **state)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    HubQuery *hq = CFDB_QueryTotalCompliance(conn, NULL, NULL, 0, time(NULL), -1, -1, -1, false, NULL, PROMISE_CONTEXT_MODE_ALL, NULL, NULL);
    assert_true(hq != NULL);

    DeleteHubQuery(hq, DeleteHubTotalCompliance);
    assert_true(CFDB_Close(conn));
}

#endif


int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBMONGOC)
        unit_test(test_query_promiselog_repaired),
        unit_test(test_query_total_compliance)
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
