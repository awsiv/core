/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/
#include "conf.h"

#include "cf.nova.h"
#include "hub.h"
#include "env_context.h"
#include "db_common.h"
#include "bson_lib.h"
#include "reporting-engine.h"
#include "hub-scheduled-reports.h"
#include "db_query.h"
#include "conversion.h"
#include "files_interfaces.h"

#include <assert.h>

static pid_t REPORT_SCHEDULER_CHILD_PID = -1;

static void ScheduleRunScheduledReports(void);
static bool IsProcRunning(pid_t pid);
static bool SetDefaultPathsForSchedulingReports(EnterpriseDB *conn);

static void CFDB_QueryGenerateScheduledReports( EnterpriseDB *conn, Rlist *query_ids );
static bool CreateScheduledReport( EnterpriseDB *conn, const char *user, const char *email,
                                   const char *query_id, const char *query, const char *title,
                                   const char *description, int output_type );
static bool CreateScheduledReportCSV( EnterpriseDB *conn, const char *user, const char *query_id,
                                      const char *query, _Bool copy_to_webdir, char *path_buffer, int bufsize );
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

    if (SetDefaultPathsForSchedulingReports(conn))
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
    while (MongoCursorNext(cursor))
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
    BsonAppendStringNonEmpty( query, cfr_user_id, user_id );
    BsonAppendStringNonEmpty( query, cfr_query_id, query_id );
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

    while (MongoCursorNext(cursor))
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

        const char *title = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_title, &title );

        const char *description = NULL;
        BsonStringGet( mongo_cursor_bson( cursor ), cfr_description, &description );

        int output_type = REPORT_FORMAT_CSV;
        BsonIntGet( mongo_cursor_bson( cursor ), cfr_report_output_type, &output_type );

        if( CreateScheduledReport( conn, user, email, query_id, query, title, description, output_type ) )
        {
            CFDB_SaveScheduledRunHistory( conn, user, query_id, time( NULL ) );
            CFDB_SaveAlreadyRun( conn, user, query_id, true );
        }
    }

    mongo_cursor_destroy( cursor );
}

/*******************************************************************/

static bool CreateScheduledReport( EnterpriseDB *conn, const char *user, const char *email,
                                   const char *query_id, const char *query,
                                   const char *title, const char *description, int output_type )
{
    assert( !NULL_OR_EMPTY(user) );
    assert( email );
    assert( query_id );
    assert( query );

    Nova_HubLog("Starting DBScheduledReportGeneration");
    struct timespec measure_start = BeginMeasure();

    const char *report_format = NULL;

    bool pdf = output_type & REPORT_FORMAT_PDF;
    bool csv = output_type & REPORT_FORMAT_CSV;

    char path_to_csv[CF_MAXVARSIZE] = "\0";

    bool copy_to_webroot = !NULL_OR_EMPTY(email) || pdf;

    bool retval = CreateScheduledReportCSV( conn, user, query_id, query, copy_to_webroot, path_to_csv, CF_MAXVARSIZE - 1 );

    if( pdf && retval )
    {
        if( csv )
        {
            report_format = "csv+pdf";
        }
        else
        {
            report_format = "pdf";
        }
    }

    char cmd[CF_BUFSIZE] = {0};
    char docroot[CF_MAXVARSIZE] = {0};
    char php_path[CF_MAXVARSIZE] = {0};

    if( !CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        CfOut( cf_error, "DBScheduledReportGeneration", "!! Cannot find doc_root" );
        return false;
    }

    if( !CFDB_HandleGetValue(cfr_php_bin_dir, php_path, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        return false;
    }

    char *query_expanded = SqlVariableExpand(query);

    snprintf(cmd, CF_BUFSIZE - 1,
             "%s/php %s/index.php advancedreports generatescheduledreport "
             "\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
             php_path, docroot, user, query_id, query_expanded, report_format, title, description, email, path_to_csv );

    free(query_expanded);

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

    retval = cf_pclose(pp) == 0;

    Nova_HubLog("Finished DBScheduledReportGeneration");
    EndMeasure( "DBScheduledReportGeneration", measure_start );

    return retval;
}

/*******************************************************************/

static bool CreateScheduledReportCSV( EnterpriseDB *conn, const char *user, const char *query_id,
                                      const char *query, bool copy_to_webdir, char *path_buffer, int bufsize )
{
    assert( user );
    assert( query_id );

    sqlite3 *db;
    bool retval = true;
    char filename[CF_MAXVARSIZE] = {0};
    char path_origin[CF_MAXVARSIZE] = { 0 };

    if( Sqlite3_DBOpen( &db ) )
    {
        if( GenerateAllTables( db ) )
        {
            Rlist *tables = GetTableNamesInQuery( query );

            if( tables )
            {
                LoadSqlite3Tables( db, tables, user );
                DeleteRlist( tables );

                char *err_msg = 0;

                snprintf( filename, CF_MAXVARSIZE - 1, "%s-%s-%ld.csv", user, query_id, time( NULL ) );
                snprintf( path_origin, CF_MAXVARSIZE - 1, "%s/reports/%s", CFWORKDIR, filename );

                Writer *writer = FileWriter( fopen( path_origin, "w" ) );

                if ( !Sqlite3_Execute( db, query, ( void * ) BuildCSVOutput, ( void * ) writer, err_msg ) )
                {
                    CfOut( cf_error, "DBScheduledCSVReportGeneration", "%s", err_msg );
                    retval = false;
                }

                WriterClose( writer );
                Sqlite3_FreeString( err_msg );
            }
        }

        Sqlite3_DBClose( db );
    }

    if( copy_to_webdir && retval )
    {
        char docroot[CF_MAXVARSIZE] = {0};

        if( !CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
        {
            CfOut( cf_error, "DBScheduledCSVReportGeneration", "!! Cannot find doc_root" );
            return false;
        }

        /* copy file to web root */
        char path_dest[CF_MAXVARSIZE] = {0};
        snprintf( path_dest, CF_MAXVARSIZE - 1, "%s/tmp/%s", docroot, filename );
        if( !CopyRegularFileDisk(path_origin, path_dest, false) )
        {
            CfOut( cf_error, "DBScheduledCSVReportGeneration", "!! Cannot create file: \"%s\" ", path_dest );
            return false;
        }

        snprintf( path_buffer, bufsize, "%s", path_dest );
    }

    return retval;
}

/*******************************************************************/

static void CFDB_SaveScheduledRunHistory( EnterpriseDB *conn, const char *user,
                                          const char *query_id, time_t completed_time )
{
    bson query[1];

    bson_init( query );
    BsonAppendStringNonEmpty( query, cfr_user_id, user );
    BsonAppendStringNonEmpty( query, cfr_query_id, query_id );
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
/* Stores default paths for  MP install dir and php */
static bool SetDefaultPathsForSchedulingReports(EnterpriseDB *conn)
{
    assert(conn);

    char path_db[CF_MAXVARSIZE] = {0};

    if( !CFDB_HandleGetValue( cfr_mp_install_dir, path_db, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        Rval ret;
        if( GetVariable( "sys", "doc_root", &ret ) != cf_notype )
        {
            char docroot[CF_MAXVARSIZE] = {0};
            snprintf( docroot, CF_MAXVARSIZE - 1, "%s", ( char * ) ret.item );

            if( !CFDB_PutValue( conn, cfr_mp_install_dir, docroot, MONGO_SCRATCH) )
            {
                CfOut( cf_error, "", "!! Couldn't set default path for mp_install_dir");
                return false;
            }
        }
    }

    path_db[0] = '\0';

    if( !CFDB_HandleGetValue( cfr_php_bin_dir, path_db, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH ) )
    {
        const char *path_env = getenv( "PATH" );

        Rlist *path_list = SplitStringAsRList( path_env, ':' );

        char path_str[CF_MAXVARSIZE] = {0};

        for( Rlist *path_itr = path_list; path_itr != NULL; path_itr = path_itr->next )
        {
            char path_php[CF_MAXVARSIZE] = {0};
            snprintf( path_php, CF_MAXVARSIZE - 1, "%s/php", (char *) path_itr->item );

            struct stat stat_buf;
            if( cfstat( path_php, &stat_buf ) == 0)
            {
                strncpy( path_str, (char *) path_itr->item, CF_MAXVARSIZE - 1 );
                break;
            }
        }

        DeleteRlist( path_list );

        if( !CFDB_PutValue( conn, cfr_php_bin_dir, path_str, MONGO_SCRATCH) )
        {
            CfOut( cf_error, "", "!! Couldn't set default path for php_bin_dir");
            return false;
        }
    }

    return true;
}
/*******************************************************************/
