#include "platform.h"
#include "test.h"
#include "cf.nova.h"
#include "db_save.h"
#include "db_maintain.h"
#include "item_lib.c"

#include <setjmp.h>
#include <cmockery.h>

#if defined(HAVE_LIBMONGOC)

static void test_validate_variables_data_db(void **state)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    char *keyhash = "SHA=correct5088276321157171c72f6f1330744f9b0439db162cf35c73ea4459c31";

    Item *data = NULL;
    PrependItem(&data, "s,1234567890,test_lval_correct,test_rval_correct", NULL);
    PrependItem(&data, "s,1234567890,test_lval.incorrect,test_rval_correct\n", NULL);
    //problematic data from customer
    PrependItem(&data, "s,1234567890,x3dtsgounix_etrust_log_host_fix_replace_hosts.oldtonew[cmc3asfn28.ny.customer.com],cmc3asfn28.svr.us.customer.net\n",NULL);
    PrependItem(&data, "S: test_scope_canonified", NULL);

    // load test data
    CFDB_SaveVariables2(conn, keyhash, data);

    DeleteItemList(data);

    // check for valid data
    bool valid = CFDB_ValidateVariablesReport(conn, keyhash);

    assert_true(valid);

    // cleanup
    bson host_key;
    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    bson_finish(&host_key);

    mongo_remove(conn, MONGO_DATABASE, &host_key, NULL);

    bson_destroy(&host_key);
    assert_true(CFDB_Close(conn));
}

#endif

int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBMONGOC)
        unit_test(test_validate_variables_data_db)
#endif
    };

    //PRINT_TEST_BANNER();
    return run_tests(tests);
}
