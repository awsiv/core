#include "platform.h"
#include "test.h"
#include "bson_lib.h"
#include <bson.h>
#include "json.h"
#include "writer.h"
#include <assert.h>


/* CAUTION: */
/* Beware that these currently operate on the actual db(cfreport) */
/* Please backup important data before running these tests */

#ifdef HAVE_LIBMONGOC
#include "cf.nova.h"
#include "db_query.h"
#include "db_save.h"

static void test_CFDB_QueryScheduledReport(void **state)
{
    EnterpriseDB conn[1];

    if( !CFDB_Open( conn ) )
    {
        printf("Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    const char *username = "admin_test";
    const char *scheduled_query_id = "hosts_query_test";
    const char *email = "admin@cfengine.com";
    const char *scheduled_query = "SELECT * FROM Hosts LIMIT 10;";
    const char *schedule = "Monday.Hr13";
    bool enabled = true;

    bson query[1];
    bson_init( query );
    bson_append_string( query, cfr_user_id, username );
    bson_append_string( query, cfr_query_id, scheduled_query_id );
    bson_finish( query );

    assert( query->finished );

    /* Cleanup test data */
    /* query is freed (by bson_destroy) towards the end */
    mongo_remove( conn, MONGO_SCHEDULED_REPORTS, query, NULL );

    bson insert_op[1];
    bson_init( insert_op );
    bson_append_string( insert_op, cfr_user_id, username );
    bson_append_string( insert_op, cfr_query_id, scheduled_query_id );
    bson_append_string( insert_op, cfr_user_email, email );
    bson_append_string( insert_op, cfr_query, scheduled_query);
    bson_append_string( insert_op, cfr_run_classes, schedule);
    bson_append_bool( insert_op, cfr_enabled, enabled );
    bson_finish( insert_op );

    assert( insert_op->finished );

    assert( mongo_insert( conn , MONGO_SCHEDULED_REPORTS, insert_op, NULL ) == MONGO_OK );

    bson_destroy( insert_op );

    HubQuery *hq =
            CFDB_QueryScheduledReport(
                conn,
                username,
                email,
                scheduled_query_id,
                scheduled_query,
                schedule
                );

    assert( hq );
    assert( hq->records );

    for( Rlist *rp = hq->records; rp != NULL; rp = rp->next )
    {
        HubScheduledReport *hs = ( HubScheduledReport * ) rp->item;

        assert( hs );

        assert_string_equal( hs->username, username );
        assert_string_equal( hs->email, email );
        assert_string_equal( hs->query_id, scheduled_query_id );
        assert_string_equal( hs->query, scheduled_query );
        assert_string_equal( hs->schedule, schedule );

        if( enabled )
        {
            assert( hs->enabled );
        }
        else
        {
            assert( !hs->enabled );
        }

    }

    DeleteHubQuery( hq, DeleteScheduledReport );

    /* Cleanup test data */
    mongo_remove( conn, MONGO_SCHEDULED_REPORTS, query, NULL );

    bson_destroy( query );

    CFDB_Close( conn );
}

#endif

int main()
{
    const UnitTest tests[] =
    {
#ifdef HAVE_LIBMONGOC
        unit_test(test_CFDB_QueryScheduledReport)
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
