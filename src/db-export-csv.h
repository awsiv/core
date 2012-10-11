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
void HubClassToCSV( void *data, Writer *w );
void HubSoftwareToCSV( void *data, Writer *w );
void HubPatchesToCSV( void *data, Writer *w );
void HubVariablesToCSV( void *data, Writer *w );
void HubTotalComplianceToCSV( void *data, Writer *w);
void HubPromiseComplianceToCSV( void *data, Writer *w);
void HubPromiseComplianceWeightedToCSV( void *data, Writer *w);
void HubLastseenToCSV( void *data, Writer *w);
void HubPerformanceToCSV( void *data, Writer *w);
void HubSetuidToCSV( void *data, Writer *w);
void HubFileChangesToCSV( void *data, Writer *w);
void HubValueToCSV( void *data, Writer *w);
void HubPromiseLogToCSV( void *data, Writer *w);
void HubPromiseSumToCSV( void *data, Writer *w);
void HubFileDiffToCSV( void *data, Writer *w);
void HubBundleSeenToCSV( void *data, Writer *w);
void HubBundleSeenWeightedToCSV( void *data, Writer *w);
void HubHostToCSV( void *data, Writer *w);

Writer *ExportWebReportStart( WebReportFileInfo *wr_info );

bool ExportWebReportUpdate( Writer *writer, void *data,
                            void (*fn_ptr_get_csv)(void *, Writer *),
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
