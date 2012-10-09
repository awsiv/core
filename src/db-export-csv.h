#include "conf.h"
#include "cf.nova.h"
#include "scorecards.h"
#include <bool.h>

bool ExportWebReportStatusInitialize( WebReportFileInfo *wr_info );
bool ExportWebReportStatusFinalize( WebReportFileInfo *wr_info );
bool ExportWebReportWriteStatusString( char *string, char *full_file_path_csv, bool create_file );
bool ExportWebReportUpdateStatus( WebReportFileInfo *wr_info );

void GetHostNameAndIP( const bson *b, char *hostname, char *address, int bufsize );

/* Web CSV report export helpers */
void HubClassToCSV( void *data, char buffer[] );
void HubSoftwareToCSV( void *data, char buffer[] );
void HubPatchesToCSV( void *data, char buffer[] );
void HubVariablesToCSV( void *data, char buffer[] );
void HubTotalComplianceToCSV( void *data, char buffer[]);
void HubPromiseComplianceToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubLastseenToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubPerformanceToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubSetuidToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubFileChangesToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubValueToCSV( void *data, char buffer[CF_BUFSIZE]);
void HubPromiseLogToCSV( void *data, char buffer[CF_BUFSIZE]);

Writer *ExportWebReportStart( WebReportFileInfo *wr_info );

bool ExportWebReportUpdate( Writer *writer, void *data,
                            void (*fn_ptr_get_csv)(void *, char *),
                            WebReportFileInfo *wr_info );

Writer *ExportWebReportStart( WebReportFileInfo *wr_info );

#define WEB_REPORT_EXPORT_START( wr_info, writer, cursor ) \
    if( wr_info && wr_info->write_data ) \
    { \
        writer = ExportWebReportStart( wr_info ); \
        assert( writer ); \
        if(!writer) \
        { \
            mongo_cursor_destroy(cursor); \
            return NULL; \
        } \
    }

#define WEB_REPORT_EXPORT_FINISH( wr_info, writer ) \
    if( writer ) \
    { \
        ExportWebReportStatusFinalize( wr_info ); \
        WriterClose(writer); \
    } \
    if(wr_info) \
    { \
        return NULL; \
    }
