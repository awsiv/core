#include "platform.h"
#include "test.h"
#include "cf.nova.h"

#include "db_save.h"
#include "db_query.h"
#include "item_lib.c"
#include "bson_lib.h"
#include "cfstream.h"
#include "string_lib.h"

#include <setjmp.h>
#include <cmockery.h>
#include <assert.h>

#if defined(HAVE_LIBMONGOC)
#include "install.h"

bool CFDB_ValidateVariablesReport(EnterpriseDB *conn, const char *keyhash)
{
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyhash))
    {
        bson_append_string(&query, cfr_keyhash, keyhash);
    }

    BsonFinish(&query);

    bson fields;

    BsonSelectReportFields(&fields, 1, cfr_vars);

    mongo_cursor *cursor = MongoFind(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))
    {
        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfr_vars) == 0)
            {
                bson_iterator it2;
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator it3;
                    bson_iterator_subiterator(&it2, &it3);

                    const char *scope = bson_iterator_key(&it2);

                    if(BsonIsKeyCorrupt(scope))
                    {
                        printf(" !! Corrupted field name scope = \"%s\" in variables", scope);
                        mongo_cursor_destroy(cursor);
                        return false;
                    }

                    while (bson_iterator_next(&it3))
                    {
                        char rlval[CF_MAXVARSIZE] = "\0";

                        bson_iterator it4;
                        bson_iterator_subiterator(&it3, &it4);

                        strncpy(rlval, bson_iterator_key(&it3), CF_MAXVARSIZE - 1);

                        while (bson_iterator_next(&it4))
                        {
                            if (strcmp(bson_iterator_key(&it4), cfr_rval) == 0)
                            {
                                bson_iterator it5;

                                switch (bson_iterator_type(&it4))
                                {
                                case BSON_ARRAY:
                                case BSON_OBJECT:
                                    bson_iterator_subiterator(&it4, &it5);
                                    while (bson_iterator_next(&it5))
                                    {
                                        assert(bson_iterator_string(&it5));
                                    }

                                    break;

                                default:
                                    assert(bson_iterator_string(&it4));
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
                                CfOut(cf_inform, "", " !! Possible data-corruption in variables report: %s.%s.%s "
                                      "(skipping...)", scope, rlval, bson_iterator_key(&it4));

                                mongo_cursor_destroy(cursor);
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

static void test_variables_with_newline(void **state)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    char *keyhash = "SHA=vars3345088276321157171c72f6f1330744f9b0439db162cf35c73ea4459c31";

    char *rval_with_newline = "line1\nline2\nline3\nline4\n";

    char rval_without_newline[CF_MAXTRANSSIZE] = "\0";
    ReplaceChar(rval_with_newline, rval_without_newline, sizeof(rval_without_newline), '\n', CF_N_CODE);

    char variable_data[CF_MAXTRANSSIZE] = "\0";
    snprintf(variable_data, CF_MAXTRANSSIZE, "s,1234567890,test_lval,%s\n", rval_without_newline);

    Item *data = NULL;
    PrependItem(&data, variable_data, NULL);
    PrependItem(&data, "S: test_scope", NULL);

    // load test data
    CFDB_SaveVariables2(conn, keyhash, data);

    DeleteItemList(data);

    HubQuery *hq = CFDB_QueryVariables(conn, keyhash, NULL, NULL, NULL, NULL, NULL,
                             0, time(NULL), NULL, PROMISE_CONTEXT_MODE_ALL, NULL,
                                       QUERY_FLAG_DISABLE_ALL);

    assert(hq);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubVariable *hv = (HubVariable *) rp->item;
        assert_true(hv->dtype[0] == RVAL_TYPE_SCALAR);
        assert_string_equal(rval_with_newline, hv->rval.item);
    }

    DeleteHubQuery(hq, DeleteHubVariable);

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
        unit_test(test_validate_variables_data_db),
        unit_test(test_variables_with_newline)
#endif
    };

    //PRINT_TEST_BANNER();
    return run_tests(tests);
}
