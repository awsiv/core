/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/
#include "conf.h"

#if defined( HAVE_LIBSQLITE3 )

#include "cf.nova.h"
#include "hub.h"
#include "env_context.h"
#include "db_common.h"
#include "bson_lib.h"
#include "reporting-engine.h"
#include "hub-scheduled-reports.h"
#include "db_query.h"
#include <assert.h>

static pid_t REPORT_SCHEDULER_CHILD_PID = -1;

static void ScheduleRunScheduledReports(void);
static bool IsProcRunning(pid_t pid);

static void CFDB_QueryGenerateScheduledReports( EnterpriseDB *conn, Rlist *query_ids );
static bool CreateScheduledReport( EnterpriseDB *conn, const char *user, const char *email,
                                   const char *query_id, const char *query, int output_type );
static void CFDB_SaveAlreadyRun( EnterpriseDB *conn, const char *user_id, const char *query_id, const bool already_run );
static void CFDB_SaveScheduledRunHistory( EnterpriseDB *conn, const char *user, const char *query_id, time_t completed_time );
static Rlist *CFDB_QueryPendingSchedulesList( EnterpriseDB *conn );

/*******************************************************************/

void RunScheduledEnterpriseReports(void)
{
    if( !CFDB_QueryIsMaster() )
    {
        return;
    }

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
        HardClass("am_policy_hub");
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

static void ScheduleRunScheduledReports(void)
{
    time_t now = time( NULL );

    EnterpriseDB conn[1];

    if( !CFDB_Open( conn ) )
    {
        return;
    }

    Rlist *pending_schedules = NULL;
    pending_schedules = CFDB_QueryPendingSchedulesList( conn );

    if( !pending_schedules )
    {
        CFDB_Close( conn );
        return;
    }

    CFDB_QueryGenerateScheduledReports( conn, pending_schedules );
    DeleteRlist( pending_schedules );

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

static Rlist *CFDB_QueryPendingSchedulesList( EnterpriseDB *conn )
{
    bson query[1];
    bson_init( query );
    BsonAppendBool( query, cfr_enabled, true );
    BsonFinish( query );

    bson fields[1];
    BsonSelectReportFields( fields, 4,
                            cfr_user_id,
                            cfr_query_id,
                            cfr_run_classes,
                            cfr_already_run );

    mongo_cursor *cursor = MongoFind( conn, MONGO_SCHEDULED_REPORTS,
                                      query, fields, 0, 0, CF_MONGO_SLAVE_OK );

    bson_destroy(query);
    bson_destroy(fields);

    Rlist *pending_queries = NULL;
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
            PrependRlist( &pending_queries, (void *) run_class, CF_SCALAR );
        }
        else if( !is_class_defined && already_run )
        {
            /* reset already run value for next run */
            CFDB_SaveAlreadyRun( conn, user_id, query_id, false );
        }
    }

    mongo_cursor_destroy( cursor );

    return pending_queries;
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
}

/*******************************************************************/

static void CFDB_QueryGenerateScheduledReports( EnterpriseDB *conn, Rlist *query_ids )
{
    bson query[1];
    bson_init( query );
    BsonAppendBool( query, cfr_already_run, false );
    BsonAppendBool( query, cfr_enabled, true );
    BsonFilterInStringArrayRlist( query, cfr_run_classes, query_ids );
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

        char email[CF_MAXVARSIZE] = {0};
        email[0] = '\0';
        BsonStringWrite( email, CF_MAXVARSIZE - 1, mongo_cursor_bson( cursor ), cfr_user_email);

        const char *query_id = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_query_id, &query_id );

        const char *query = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_query, &query );

        int output_type = REPORT_FORMAT_CSV;
        BsonIntGet( mongo_cursor_bson( cursor ), cfr_report_output_type, &output_type );

        if( CreateScheduledReport( conn, user, email, query_id, query, output_type ) )
        {
            CFDB_SaveScheduledRunHistory( conn, user, query_id, time( NULL ) );
            CFDB_SaveAlreadyRun( conn, user, query_id, true );
        }
    }

    mongo_cursor_destroy( cursor );
}

/*******************************************************************/

static bool CreateScheduledReport( EnterpriseDB *conn, const char *user, const char *email,
                                   const char *query_id, const char *query, int output_type )
{
    assert( user );
    assert( email );
    assert( query_id );
    assert( query );

    Nova_HubLog("Starting DBScheduledReportGeneration");
    struct timespec measure_start = BeginMeasure();

    const char *report_format = NULL;

    bool pdf = output_type & REPORT_FORMAT_PDF;
    bool csv = output_type & REPORT_FORMAT_CSV;

    if( pdf && csv )
    {
        report_format = "csv+pdf";
    }
    else if( csv )
    {
        report_format = "csv";
    }
    else if( pdf )
    {
        report_format = "pdf";
    }

    char cmd[CF_BUFSIZE] = {0};
    char docroot[CF_MAXVARSIZE] = {0};
    char php_path[CF_MAXVARSIZE] = {0};

    const char *report_title = "Custom scheduled report";
    const char *report_description = "Scheduled report";

    if( !CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        CfOut( cf_error, "DBScheduledReportGeneration", "!! Cannot find doc_root" );
        return false;
    }

    if( !CFDB_HandleGetValue(cfr_php_bin_dir, php_path, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        return false;
    }

    snprintf(cmd, CF_BUFSIZE - 1,
             "%s/php %s/index.php advancedreports generatescheduledreport "
             "\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
             php_path, docroot, user, query_id, query, report_format, report_title, report_description, email );

    Nova_HubLog( "DBScheduledReportGeneration: command = %s", cmd );
    FILE *pp;

    if ((pp = cf_popen(cmd, "r")) == NULL)
    {
        CfOut( cf_error, "DBScheduledReportGeneration", "!! Could not run command \"%s\": %s \n", cmd, GetErrorStr() );
        return false;
    }

    char line[CF_BUFSIZE] = {0};
    int line_num = 0;

    while (!feof(pp))
    {
        if (fgets(line, sizeof(line), pp))
        {
            Nova_HubLog("%d: %s", ++line_num, line);
        }
    }

    bool retval = cf_pclose(pp) == 0;

    Nova_HubLog("Finished DBScheduledReportGeneration");
    EndMeasure( "DBScheduledReportGeneration", measure_start );

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
