#include "test.h"
#include "cf.nova.h"
#include "db_save.h"
#include "db_maintain.h"
#include "item_lib.c"

#include <setjmp.h>
#include <cmockery.h>
#include <assert.h>

#if defined(HAVE_LIBMONGOC)

bool CFDB_ValidateVariablesReport(mongo_connection *conn, const char *keyhash)
{
    bson_buffer bb;
    bson query;

    if(!NULL_OR_EMPTY(keyhash))
    {
        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, keyhash);
        bson_from_buffer(&query, &bb);
    }
    else
    {
        bson_empty(&query);
    }

    bson field;

    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_vars, 1);
    bson_from_buffer(&field, &bb);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    if(!NULL_OR_EMPTY(keyhash))
    {
        bson_destroy(&query);
    }
    bson_destroy(&field);

    while (mongo_cursor_next(cursor))
    {
        bson_iterator it1 = {0};
        bson_iterator_init(&it1, cursor->current.data);

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_vars) == 0)
            {
                bson_iterator it2 = {0};
                bson_iterator_init(&it2, bson_iterator_value(&it1));

                while (bson_iterator_next(&it2))
                {
                    bson_iterator it3 = {0};
                    bson_iterator_init(&it3, bson_iterator_value(&it2));

                    char rscope[CF_MAXVARSIZE] = "\0";
                    strncpy(rscope, bson_iterator_key(&it2), CF_MAXVARSIZE);

                    while (bson_iterator_next(&it3))
                    {
                        bson_iterator it4 = {0};
                        bson_iterator_init(&it4, bson_iterator_value(&it3));

                        char rlval[CF_MAXVARSIZE] = "\0";
                        strncpy(rlval, bson_iterator_key(&it3), CF_MAXVARSIZE - 1);

                        while (bson_iterator_next(&it4))
                        {
                            if (strcmp(bson_iterator_key(&it4), cfr_rval) == 0)
                            {
                                bson_iterator it5 = {0};
                                switch (bson_iterator_type(&it4))
                                {
                                case bson_array:
                                case bson_object:

                                    bson_iterator_init(&it5, bson_iterator_value(&it4));

                                    while (bson_iterator_next(&it5))
                                    {
                                        assert((char *) bson_iterator_string(&it5));
                                    }

                                    break;

                                default:
                                    assert((bson_iterator_string(&it4)));
                                    break;
                                }
                            }
                            else if (strcmp(bson_iterator_key(&it4), cfr_type) == 0)
                            {
                                assert(bson_iterator_string(&it4));
                            }
                            else if (strcmp(bson_iterator_key(&it4), cfr_time) == 0)
                            {
                                assert(bson_iterator_int(&it4));
                            }
                            else
                            {
                                mongo_cursor_destroy(cursor);
                                printf("Data corrpution in variables report: %s.%s\n", rscope, rlval);
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);
    return true;
}

static void test_validate_variables_data_db(void **state)
{
    mongo_connection conn[1];
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
    bson_buffer bb;
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson host_key;
    bson_from_buffer(&host_key, &bb);

    mongo_remove(conn, MONGO_DATABASE, &host_key);

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
