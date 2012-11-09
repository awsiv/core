#include "platform.h"
#include "test.h"
#include "cf.nova.h"
#include "item_lib.h"
#include "conversion.h"

#include <setjmp.h>
#include <cmockery.h>
#include <assert.h>

#if defined(HAVE_LIBMONGOC)

#include "db_save.h"
#include "db_query.h"
#include "bson_lib.h"
#include "dataunpack.c"

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

    HubQuery *hq = CFDB_QuerySoftware(conn, keyhash, cfr_software, NULL, NULL, NULL, true,
                                      NULL, true, PROMISE_CONTEXT_MODE_ALL, NULL);

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

#endif

int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBMONGOC)
        unit_test(test_software_report_unpack_db)
#endif
    };

    //PRINT_TEST_BANNER();
    return run_tests(tests);
}
