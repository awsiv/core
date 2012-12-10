#include "test.h"
#include "cf.nova.h"

#include "string_lib.h"

#include <setjmp.h>
#include <cmockery.h>
#include <assert.h>

#if defined(HAVE_LIBMONGOC)

#include "db_save.h"
#include "db_query.h"
#include "bson_lib.h"

static void test_promiselog_report_unpack_db(void **state)
{
    mongo_connection conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    char *keyhash = "SHA=promiselog8276321157171c72f6f1330744f9b0439db162cf35c73ea4459c31";

    char *handle[5] = {"promise_handle_correct", "promise_handle_empty_cause", "","", NULL};
    char *cause[5] = {"cause_correct", "", "cause_empty_handle","", NULL};
    time_t t = 123456789;

    char packed_promiselog_correct[CF_MAXTRANSSIZE] = {0};
    char packed_promiselog_empty_cause[CF_MAXTRANSSIZE] = {0};
    char packed_promiselog_empty_handle[CF_MAXTRANSSIZE] = {0};
    char packed_promiselog_empty_handle_and_cause[CF_MAXTRANSSIZE] = {0};
    char packed_promiselog_invalid[CF_MAXTRANSSIZE] = {0};

    snprintf(packed_promiselog_correct, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[0], cause[0]);
    snprintf(packed_promiselog_empty_cause, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[1], cause[1]);
    snprintf(packed_promiselog_empty_handle, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[2], cause[2]);
    snprintf(packed_promiselog_empty_handle_and_cause, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[3], cause[3]);
    // example entry found at a customer:
    snprintf(packed_promiselog_invalid, CF_MAXTRANSSIZE, "/bin/sed 's/^warning\.//;s/^fatal\\.//;s/^critical\.//;' | /usr/bin/tr '=' ':' |\n");

    Item *data[CF_CODEBOOK_SIZE] = { 0 };
    NewReportBook(data);
    AppendItem(&(data[23]), packed_promiselog_correct, NULL);
    AppendItem(&(data[23]), packed_promiselog_empty_cause,NULL);
    AppendItem(&(data[23]), packed_promiselog_empty_handle, NULL);
    AppendItem(&(data[23]), packed_promiselog_empty_handle_and_cause, NULL);
    AppendItem(&(data[23]), packed_promiselog_invalid, NULL);

    UnpackReportBook(conn, keyhash, data);
    DeleteReportBook(data);

    CFDB_SaveLastHostUpdate(conn, keyhash);
    CFDB_SaveHostID(conn, MONGO_DATABASE, cfr_keyhash, keyhash, "10.0.0.79", "test_host");

    HubQuery *hq = CFDB_QueryPromiseLog(conn, keyhash, PROMISE_LOG_STATE_REPAIRED, ".*",
                                        true, ".*", 0, t, true, NULL,
                                        NULL);

    int total_matched = 0;

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubPromiseLog *hp = (HubPromiseLog *) rp->item;

        for(int i = 0; i < 4; i++)
        {
            if((strcmp(handle[i], NULLStringToEmpty(hp->handle)) == 0)
                && (strcmp(cause[i], NULLStringToEmpty(hp->cause)) == 0))
            {
                total_matched++;
            }
        }
    }

    // Only one of the above entries should make it to the database.
    int total_entries = 1;
    assert_int_equal(total_matched, total_entries);

    DeleteHubQuery(hq, DeleteHubPromiseLog);

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
        unit_test(test_promiselog_report_unpack_db)
#endif
    };

    //PRINT_TEST_BANNER();
    return run_tests(tests);
}
