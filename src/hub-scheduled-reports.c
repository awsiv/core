/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/
#if defined( HAVE_SQLITE3 )

#include "cf.nova.h"
#include "hub.h"
#include "env_context.h"
#include "db_common.h"
#include "bson_lib.h"
#include "reporting-engine.h"
#include "hub-scheduled-reports.h"
#include <assert.h>

static pid_t REPORT_SCHEDULER_CHILD_PID = -1;

static void ScheduleRunScheduledReports(void);
static bool IsProcRunning(pid_t pid);
static bool CFDB_QueryHasPendingSchedules(EnterpriseDB *conn);

static void CFDB_QueryGenerateScheduledReports( EnterpriseDB *conn );
static bool CreateScheduledReport( const char *user, const char *email, const char *query_id, const char *query );
static void CFDB_SaveAlreadyRun( EnterpriseDB *conn, const char *user_id, const char *query_id, const bool already_run );
static void CFDB_SaveScheduledRunHistory( EnterpriseDB *conn, const char *user, const char *query_id, time_t completed_time );

/*******************************************************************/

void RunScheduledEnterpriseReports(void)
{
    if (REPORT_SCHEDULER_CHILD_PID != -1)
    {
        if (IsProcRunning(REPORT_SCHEDULER_CHILD_PID))
        {
            return;
        }
        else
        {
            REPORT_SCHEDULER_CHILD_PID = -1;
        }
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        CfOut(cf_error, "fork", "Unable to start Enterprise Report Scheduler process");
        return;
    }

    if (pid == 0)
    {
        NewClass("am_policy_hub");
        ALARM_PID = -1;
        ScheduleRunScheduledReports();
        _exit(0);
    }
    else
    {
        REPORT_SCHEDULER_CHILD_PID = pid;

        CfOut(cf_verbose, "", " -> Started new Enterprise Report Scheduler process (pid = %d)", pid);
        Nova_HubLog("-> Started new Enterprise Report Scheduler process (pid = %d)\n", pid);
    }
}

/*******************************************************************/

bool CheckPendingScheduledReports( void )
{
    EnterpriseDB conn[1];

    if( !CFDB_Open( conn ) )
    {
        return;
    }

    bool reports_pending = CFDB_QueryHasPendingSchedules( conn );

    CFDB_Close( conn );

    return reports_pending;
}

/*******************************************************************/

static void ScheduleRunScheduledReports(void)
{
    time_t now = time( NULL );

    EnterpriseDB conn[1];

    if( !CFDB_Open( conn ) )
    {
        return;
    }

    /* This picks up the pending schedules since the last report generation */
    if( !CFDB_QueryHasPendingSchedules( conn ) )
    {
        CFDB_Close( conn );
        return;
    }

    CFDB_QueryGenerateScheduledReports( conn );

    CFDB_Close( conn );

    Nova_HubLog( "Last Report generation took %ld seconds", time( NULL ) - now );
}

/*******************************************************************/

static bool IsProcRunning( pid_t pid )
{
    bool running = waitpid( pid, NULL, WNOHANG ) == 0;

    Nova_HubLog( "Checking if Enterprise Report Scheduler process %d is running: %s\n",
                pid, running ? "yes" : "no" );

    return running;
}

/*******************************************************************/

static bool CFDB_QueryHasPendingSchedules(EnterpriseDB *conn)
{

    bson query[1];
    bson_init( query );
    BsonAppendBool( query, cfr_enabled, true );
    BsonFinish( query );

    bson fields[1];
    BsonSelectReportFields( fields, 5,
                            cfr_run_classes,
                            cfr_already_run,
                            cfr_enabled,
                            cfr_user_id,
                            cfr_query_id );

    mongo_cursor *cursor = MongoFind( conn, MONGO_SCHEDULED_REPORTS,
                                      query, fields, 0, 0, CF_MONGO_SLAVE_OK );

    bson_destroy(query);
    bson_destroy(fields);

    bool has_pending_scheduled_queries = false;

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        const char *user_id = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_user_id, &user_id );

        const char *query_id = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_query_id, &query_id );

        const char *run_class = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_run_classes, &run_class);

        if( NULL_OR_EMPTY( run_class ) )
        {
            continue;
        }        

        bool is_class_defined = IsDefinedClass( run_class, NULL );

        bool already_run = BsonBoolGet( mongo_cursor_bson( cursor ), cfr_already_run );

        if( is_class_defined && !already_run )
        {
            has_pending_scheduled_queries = true;
        }
        else if( !is_class_defined && already_run )
        {
            /* reset already run value for next run */
            CFDB_SaveAlreadyRun( conn, user_id, query_id, false );
        }
    }

    mongo_cursor_destroy( cursor );

    return has_pending_scheduled_queries;
}

/*******************************************************************/

static void CFDB_SaveAlreadyRun( EnterpriseDB *conn, const char *user_id,
                                            const char *query_id, const bool already_run )
{
    bson query[1];

    bson_init( query );
    BsonAppendStringSafe( query, cfr_user_id, user_id );
    BsonAppendStringSafe( query, cfr_query_id, query_id );
    BsonFinish( query );

    bson set_op[1];

    bson_init( set_op );
    {
        bson_append_start_object( set_op, "$set");
        BsonAppendBool( set_op, cfr_already_run, already_run );
        bson_append_finish_object( set_op );
    }
    BsonFinish( set_op );

    MongoUpdate( conn, MONGO_SCHEDULED_REPORTS, query, set_op, 0, NULL );

    bson_destroy( query );
    bson_destroy( set_op );

    MongoCheckForError( conn, "Save already run value", "CFDB_SaveAlreadyRun", NULL );
}

/*******************************************************************/

static void CFDB_QueryGenerateScheduledReports( EnterpriseDB *conn )
{
    bson query[1];
    BsonAppendBool( query, cfr_already_run, false );
    BsonAppendBool( query, cfr_enabled, true );
    BsonFinish( query );

    bson empty[1];
    bson_empty( empty );

    mongo_cursor *cursor = MongoFind( conn, MONGO_SCHEDULED_REPORTS,
                                      query, empty, 0, 0, CF_MONGO_SLAVE_OK );

    bson_destroy( query );

    while( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        const char *user = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_user_id, &user );

        const char *fullname = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_user_fullname, &fullname );

        const char *email = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_user_email, &email );

        const char *query_id = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_query_id, &query_id );

        const char *query = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_query, &query );

        if( CreateScheduledReport( user, email, query_id, query ) )
        {
            CFDB_SaveScheduledRunHistory( conn, user, query_id, time( NULL ) );
            CFDB_SaveAlreadyRun( conn, user, query_id, true );
        }
    }

    mongo_cursor_destroy( cursor );
}

/*******************************************************************/

static bool CreateScheduledReport( const char *user, const char *email,
                                   const char *query_id, const char *query )
{
    assert( user );
    assert( email );
    assert( query_id );
    assert( query );

    sqlite3 *db;
    Rlist *context_include = NULL,
          *context_exclude = NULL;
    bool retval = true;

    if( Sqlite3_DBOpen( &db ) )
    {
        if( GenerateAllTables( db ) )
        {
            Rlist *tables = GetTableNamesInQuery( query );

            if( tables )
            {
                LoadSqlite3Tables( db, tables, user, context_include, context_exclude );
                DeleteRlist( tables );

                char *err_msg = 0;

                char path[CF_MAXVARSIZE] = { 0 };
                snprintf( path, CF_MAXVARSIZE - 1, "%s/reports/%s-%s-%ld.csv", CFWORKDIR, user, query_id, time( NULL ) );

                Writer *writer = FileWriter( fopen( path, "w" ) );

                if ( !Sqlite3_Execute( db, query, ( void * ) BuildCSVOutput, ( void * ) writer, err_msg ) )
                {                    
                    retval = false;
                }

                WriterClose( writer );
                Sqlite3_FreeString( err_msg );
            }
        }

        Sqlite3_DBClose( db );
    }

    return retval;
}

/*******************************************************************/

static void CFDB_SaveScheduledRunHistory( EnterpriseDB *conn, const char *user,
                                          const char *query_id, time_t completed_time )
{
    bson query[1];

    bson_init( query );
    BsonAppendStringSafe( query, cfr_user_id, user );
    BsonAppendStringSafe( query, cfr_query_id, query_id );
    BsonFinish( query );

    bson push_op[1];

    bson_init( push_op );
    {
        bson_append_start_object( push_op, "$push" );
        bson_append_int( push_op, cfr_run_history, completed_time );
        bson_append_finish_object( push_op );
    }
    BsonFinish( push_op );

    MongoUpdate( conn, MONGO_SCHEDULED_REPORTS, query, push_op, 0, NULL );

    bson_destroy( query );
    bson_destroy( push_op );

    MongoCheckForError( conn, "Record scheduled run history", "CFDB_SaveScheduledRunHistory", NULL );
}

/*******************************************************************/
#endif
