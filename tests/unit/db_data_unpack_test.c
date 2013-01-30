#include "platform.h"
#include "test.h"
#include "cf.nova.h"

#include "item_lib.h"
#include "conversion.h"
#include "string_lib.h"

#include <setjmp.h>
#include <cmockery.h>
#include <assert.h>

#if defined(HAVE_LIBMONGOC)

#include "db_save.h"
#include "db_query.h"
#include "bson_lib.h"
#include "dataunpack.c"
#include "install.h"

static void test_software_report_unpack_db(void **state)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        fprintf(stderr, "Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    char *keyhash = "SHA=vars3345088276321157171c72f6f1330744f9b0439db162cf35c73ea4459c31";

    char *software_name[4] = {"vim_x_arch", "vim_no_arch", "vim_unknown_arch", NULL};
    char *software_arch[4] = {"x", "-", "", NULL};
    char *version = "1.2.3";

    char packed_software_x_arch[CF_MAXTRANSSIZE] = {0};
    char packed_software_no_arch[CF_MAXTRANSSIZE] = {0};
    char packed_software_unknown_arch[CF_MAXTRANSSIZE] = {0};

    snprintf(packed_software_x_arch, CF_MAXTRANSSIZE, "%s,%s,%s\n", software_name[0], version, software_arch[0]);
    snprintf(packed_software_no_arch, CF_MAXTRANSSIZE, "%s,%s,%s\n", software_name[1], version, software_arch[1]);
    snprintf(packed_software_unknown_arch, CF_MAXTRANSSIZE, "%s,%s,%s\n", software_name[2], version, software_arch[2]);

    Item **data = NewReportBook();
    AppendItem(&(data[16]), packed_software_x_arch, NULL);
    AppendItem(&(data[16]), packed_software_no_arch,NULL);
    AppendItem(&(data[16]), packed_software_unknown_arch, NULL);

    UnpackReportBook(conn, keyhash, data);
    DeleteReportBook(data);

    CFDB_SaveLastHostUpdate(conn, keyhash);
    CFDB_SaveHostID(conn, MONGO_DATABASE, cfr_keyhash, keyhash, "10.0.0.79", "test_host");

    HubQuery *hq = CFDB_QuerySoftware(conn, keyhash, cfr_software, NULL, NULL, NULL, NULL,
                                      PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX | QUERY_FLAG_SORT_RESULT);

    int total_matched = 0;
    int total_entries = 3;
    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubSoftware *hs = (HubSoftware *) rp->item;
        for(int i = 0; i < 3; i++)
        {
            // for every software name, make sure the arch is correct
            if(strcmp(software_name[i], NULLStringToEmpty(hs->name)) == 0)
            {
                assert_string_equal(Nova_LongArch(software_arch[i]), Nova_LongArch(hs->arch));
                total_matched++;
            }
        }
    }

    assert_int_equal(total_matched, total_entries);

    DeleteHubQuery(hq, DeleteHubSoftware);

    // cleanup
    bson host_key;
    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    bson_finish(&host_key);

    mongo_remove(conn, MONGO_DATABASE, &host_key, NULL);

    bson_destroy(&host_key);
    assert_true(CFDB_Close(conn));
}

static void test_promiselog_report_unpack_db(void **state)
{
    EnterpriseDB conn[1];
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

    snprintf(packed_promiselog_correct, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[0], cause[0]);
    snprintf(packed_promiselog_empty_cause, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[1], cause[1]);
    snprintf(packed_promiselog_empty_handle, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[2], cause[2]);
    snprintf(packed_promiselog_empty_handle_and_cause, CF_MAXTRANSSIZE, "%ld,%s,%s\n", t, handle[3], cause[3]);

    Item **data = NewReportBook();
    AppendItem(&(data[24]), packed_promiselog_correct, NULL);
    AppendItem(&(data[24]), packed_promiselog_empty_cause,NULL);
    AppendItem(&(data[24]), packed_promiselog_empty_handle, NULL);
    AppendItem(&(data[24]), packed_promiselog_empty_handle_and_cause, NULL);

    UnpackReportBook(conn, keyhash, data);
    DeleteReportBook(data);

    CFDB_SaveLastHostUpdate(conn, keyhash);
    CFDB_SaveHostID(conn, MONGO_DATABASE, cfr_keyhash, keyhash, "10.0.0.79", "test_host");

    HubQuery *hq = CFDB_QueryPromiseLog(conn, keyhash, PROMISE_LOG_STATE_REPAIRED, ".*",
                                        true, ".*", 0, t, true, NULL,
                                        NULL, PROMISE_CONTEXT_MODE_ALL, NULL);

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
        unit_test(test_software_report_unpack_db),
        unit_test(test_promiselog_report_unpack_db)
#endif
    };

    //PRINT_TEST_BANNER();
    return run_tests(tests);
}
