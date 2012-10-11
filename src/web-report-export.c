#include "web-report-export.h"
#include "db-export-csv.h"
#include "db_query.h"
#include "scope.h"

#include <assert.h>

/*****************************************************************************/

WebReportFileInfo *NewWebReportFileInfo( int report_type, const char *report_path,
                                         const char *report_filename, const char *request_id )
{
    assert( report_path );
    assert( report_filename );
    assert( request_id );

    WebReportFileInfo *w;
    w = xmalloc( sizeof( WebReportFileInfo ) );

    w->report_type = report_type;
    w->report_path = SafeStringDuplicate( report_path );
    w->report_filename = SafeStringDuplicate( report_filename );
    w->request_id = SafeStringDuplicate( request_id );

    char csv_path[CF_MAXVARSIZE];
    snprintf(csv_path, CF_MAXVARSIZE - 1, "%s/%s.csv", w->report_path, w->report_filename);
    w->csv_path = SafeStringDuplicate( csv_path );

    w->total_lines = 0;
    w->lines_written = 0;
    w->lines_since_last_update = 0;
    w->write_data = false;
    w->error_in_update = false;

    return w;
}

/*****************************************************************************/

void DeleteWebReportFileInfo( WebReportFileInfo *w )
{
    if( w )
    {
        free( w->report_path );
        free( w->report_filename );
        free( w->request_id );
        free( w->csv_path );
    }

    free(w);
}

/*****************************************************************************/

JsonElement *WebExportSoftwareReport( char *hostkey, char *name, char *version, char *arch,
                                      bool regex, char *type, HostClassFilter *filter,
                                      PromiseContextMode promise_context, WebReportFileInfo *wr_info )
{
    assert( filter && wr_info);

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QuerySoftware(&dbconn, hostkey, type, name, version, arch, regex,
                       filter, false, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;

        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QuerySoftware(&dbconn, hostkey, type, name, version, arch, regex,
                           filter, false, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportClassesReport( const char *hostkey, const char *context, bool regex,
                                     HostClassFilter *filter, time_t from, time_t to,
                                     PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);
    wr_info->write_data = false;
    CFDB_QueryClasses(&dbconn, hostkey, context, regex, from, to,
                      filter, false, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryClasses(&dbconn, hostkey, context, regex, from, to,
                          filter, false, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}


/*****************************************************************************/

JsonElement *WebExportVariablesReport(const char *hostkey, const char *scope, const char *lval,
                                      const char *rval, const char *type, bool regex,
                                      const HostClassFilter *filter,
                                      PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    char ns[CF_MAXVARSIZE] = { 0 };
    char bundle[CF_MAXVARSIZE] = { 0 };
    if (scope)
    {
        SplitScopeName(scope, ns, bundle);
    }

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);
    wr_info->write_data = false;
    CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type,
                        regex, 0, time(NULL), filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type,
                            regex, 0, time(NULL), filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/
JsonElement *WebExportComplianceReport(char *hostkey, char *version, time_t from, time_t to,
                                       int k, int nk, int rep, HostClassFilter *filter,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep,
                              false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep,
                                  false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportPromiseComplianceReport(char *hostkey, char *handle, char *status, bool regex,
                                              HostClassFilter *host_class_filter, HostColourFilter *host_color_filter,
                                              bool last_run_only, PromiseContextMode promise_context,
                                              WebReportFileInfo *wr_info)
{
    assert( host_class_filter && wr_info );

    if (!status)
    {
        status = "x";
    }

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    if(last_run_only)
    {
        CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                            regex, 0, time(NULL), false, host_class_filter,
                                            host_color_filter, promise_context, wr_info);
    }
    else
    {
        CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0,
                                    time(NULL), false, host_class_filter, promise_context, wr_info);
    }

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        if(last_run_only)
        {
            CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                                regex, 0, time(NULL), false, host_class_filter,
                                                host_color_filter, promise_context, wr_info);
        }
        else
        {
            CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0,
                                        time(NULL), false, host_class_filter, promise_context, wr_info);
        }

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportBundleComplianceReport(char *hostkey, char *bundle, bool regex,
                                             HostClassFilter *host_class_filter,
                                             HostColourFilter *host_colour_filter, bool last_run_only,
                                             PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( host_class_filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    if(last_run_only)
    {
        CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, host_class_filter,
                                     host_colour_filter, false, promise_context, wr_info);
    }
    else
    {
        CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, host_class_filter,
                             false, promise_context, wr_info);
    }

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        if(last_run_only)
        {
            CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, host_class_filter,
                                         host_colour_filter, false, promise_context, wr_info);
        }
        else
        {
            CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, host_class_filter,
                                 false, promise_context, wr_info);
        }

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportLastseenReport(char *hostkey, char *lhash, char *lhost, char *laddress,
                                     time_t lago, int lregex, HostClassFilter *filter,
                                     PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex,
                       0, time(NULL), false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex,
                           0, time(NULL), false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportPerformanceReport(char *hostkey, char *job, bool regex,
                                        HostClassFilter *filter,
                                        PromiseContextMode promise_context,
                                        WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryPerformance(&dbconn, hostkey, job, regex, false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryPerformance(&dbconn, hostkey, job, regex, false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportSetuidReport(char *hostkey, char *file, bool regex,
                                   HostClassFilter *filter,
                                   PromiseContextMode promise_context,
                                   WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QuerySetuid(&dbconn, hostkey, file, regex, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QuerySetuid(&dbconn, hostkey, file, regex, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportFileChangesReport(char *hostkey, char *file, bool regex,
                                        time_t from, time_t to, HostClassFilter *filter,
                                        PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to, false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to, false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/
JsonElement *WebExportValueReport(char *hostkey, char *day, char *month, char *year,
                                  HostClassFilter *filter,
                                  PromiseContextMode promise_context,
                                  WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}
/*****************************************************************************/
JsonElement *WebExportPromiseLogReport(char *hostkey, char *handle, char *causeRx,
                                       PromiseLogState state, time_t from, time_t to, HostClassFilter *filter,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, causeRx,
                         from, to, false, filter, NULL, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, causeRx,
                             from, to, false, filter, NULL, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportPromiseLogSummaryReport(char *hostkey, char *handle, char *causeRx, PromiseLogState state,
                                              time_t from, time_t to, HostClassFilter *filter,
                                              PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryPromiseLogSummary(&dbconn, hostkey, state, handle, true, causeRx,
                                from, to, false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryPromiseLogSummary(&dbconn, hostkey, state, handle, true, causeRx,
                                    from, to, false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/
JsonElement *WebExportFileDiffsReport(char *hostkey, char *file, char *diffs, bool regex,
                                      time_t from, time_t to, HostClassFilter *filter,
                                      PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    assert( filter && wr_info );

    EnterpriseDB dbconn;
    DATABASE_OPEN_WR(&dbconn);

    wr_info->write_data = false;
    CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex,
                       from, to, false, filter, promise_context, wr_info);

    DATABASE_CLOSE_WR(&dbconn);

    RETURN_WITH_ERROR_JSON_WR( wr_info->total_lines < 1, "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;
        EnterpriseDB dbconn;

        if( !CFDB_Open(&dbconn) )
        {
            _exit(0);
        }

        wr_info->write_data = true;
        CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex,
                           from, to, false, filter, promise_context, wr_info);

        CFDB_Close(&dbconn);

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

JsonElement *WebExportHostOnlyReport( Rlist *records_p, WebReportFileInfo *wr_info )
{
    assert( records_p );
    assert( wr_info );

    int host_count = 0;
    if( records_p && wr_info )
    {
        host_count = RlistLen( records_p );
        wr_info->total_lines = host_count;
        wr_info->write_data = true;
    }

    RETURN_WITH_ERROR_JSON_WR( host_count == 0,
                               "The query returned empty results. Please try different filters." );

    pid_t pid = fork();

    RETURN_WITH_ERROR_JSON_WR( pid == -1, "Unable to start CSV exporter process." );

    if (pid == 0)
    {
        ALARM_PID = -1;

        if( wr_info->write_data == true )
        {
            Writer *writer = ExportWebReportStart(wr_info);
            assert( writer );
            if(!writer)
            {
                _exit(0);
            }

            ExportWebReportWriteHeader(writer, HubHostToCSV, wr_info);

            for (Rlist *rp = records_p; rp != NULL; rp = rp->next)
            {
                HubHost *hh = (HubHost *) rp->item;
                ExportWebReportUpdate( writer, (void *) hh, HubHostToCSV, wr_info );
            }

            ExportWebReportStatusFinalize( wr_info );
            WriterClose(writer);
        }

        _exit(0);
    }

    JsonElement *retval = JsonObjectCreate(1);
    JsonObjectAppendInteger( retval, "total_result", wr_info->total_lines );
    return retval;
}

/*****************************************************************************/

