#include "db-export-csv.h"
#include "conversion.h"
#include "bson_lib.h"
#include "bson_hub_conversion.h"
#include "db_common.h"
#include "db_query.h"
#include "scope.h"

#include <assert.h>

#define CHECKPOINT 1000

/*****************************************************************************/

bool ExportWebReportStatusInitialize( WebReportFileInfo *wr_info )
{
    assert(wr_info);
    assert(wr_info->write_data == true);

    bool ret = ExportWebReportWriteStatusString( "0", wr_info->csv_path, true );

    if( !ret )
    {
      wr_info->error_in_update = true;
        //      write to syslog / apache error log
    }

    return ret;
}

/*****************************************************************************/

bool ExportWebReportStatusFinalize( WebReportFileInfo *wr_info )
{
  if(!wr_info)
    {
      return false;
    }

    assert(wr_info->write_data == true);

    int done = 100;

    if( wr_info->error_in_update )
    {
        done = -1;
    }

    char done_str[CF_MAXVARSIZE];
    snprintf( done_str, CF_MAXVARSIZE -1 , "%d", done );
    bool ret = ExportWebReportWriteStatusString( "100", wr_info->csv_path, false );

    if( !ret )
    {
      wr_info->error_in_update = true;
    }

    return ret;
}

/*****************************************************************************/

bool ExportWebReportWriteStatusString( char *string, char *full_file_path_csv, bool create_file )
{
    assert(string);
    assert(full_file_path_csv);

    const char *file_opt = NULL;
    if( create_file )
    {
        file_opt = "w";
    }
    else
    {
        file_opt = "r+";
    }

    char status_file[CF_MAXVARSIZE] = "\0";
    snprintf(status_file, CF_MAXVARSIZE - 1, "%s.status", full_file_path_csv);

    Writer *writer = FileWriter( fopen( status_file, file_opt ) );
    assert(writer);
    if(!writer)
    {
        return false;
    }

    WriterWriteF(writer, "%s", string);

    WriterClose( writer );
    return true;
}

/*****************************************************************************/

bool ExportWebReportUpdateStatus( WebReportFileInfo *wr_info )
{
    assert(wr_info);
    assert(wr_info->write_data == true);

    if( wr_info->lines_written <= 0 || wr_info->total_lines <= 0 )
    {
        return false;
    }

    long double lines_written = wr_info->lines_written;
    long double total_lines =  wr_info->total_lines;

    int percent_done = (int) (lines_written / total_lines) * 100;

    char status_str[CF_MAXVARSIZE];
    snprintf( status_str, CF_MAXVARSIZE - 1, "%d", percent_done );
    return ExportWebReportWriteStatusString( status_str, wr_info->csv_path, false );
}

/*****************************************************************************/

void GetHostNameAndIP( const bson *b, char *hostname, char *address, int bufsize )
{
    Rlist *rp_host = BsonStringArrayAsRlist( b, cfr_host_array );

    char *hostname_db = NULL;
    if(rp_host)
    {
        hostname_db = (char *)rp_host->item;
    }
    snprintf( hostname, bufsize, "%s", NULLStringToEmpty(hostname_db) );
    DeleteRlist(rp_host);

    Rlist *rp_address = BsonStringArrayAsRlist( b, cfr_ip_array );

    char *ip_db = NULL;
    if(rp_address)
    {
        ip_db = (char *)rp_address->item;
    }

    snprintf( address, bufsize, "%s", NULLStringToEmpty(ip_db) );
    DeleteRlist(rp_address);
}

/*****************************************************************************/

void HubClassToCSV( void *data, char buffer[CF_BUFSIZE] )
{
    assert(data);
    HubClass *hc = (HubClass *) data;

    assert(hc);
    assert(hc->hh);

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%f,%f,%ld\n",
             NULLStringToEmpty(hc->hh->hostname),
             NULLStringToEmpty(hc->class),
             hc->prob * 100.0,
             hc->dev * 100.0,
             hc->t);
}

/*****************************************************************************/

void HubSoftwareToCSV( void *data, char buffer[CF_BUFSIZE] )
{
    HubSoftware *hs = (HubSoftware *) data;
    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",\"%s\",%ld\n",
             NULLStringToEmpty(hs->hh->hostname),
             NULLStringToEmpty(hs->name),
             NULLStringToEmpty(hs->version),
             NULLStringToEmpty(hs->arch),
             hs->t);
}

/*****************************************************************************/

void HubPatchesToCSV( void *data, char buffer[CF_BUFSIZE] )
{
    HubSoftware *hs = (HubSoftware *) data;
    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",\"%s\"\n",
             NULLStringToEmpty(hs->hh->hostname),
             NULLStringToEmpty(hs->name),
             NULLStringToEmpty(hs->version),
             NULLStringToEmpty(hs->arch));
}

/*****************************************************************************/

void HubVariablesToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubVariable *hv = (HubVariable *) data;

    assert(hv);

    if( NULL_OR_EMPTY(hv->ns) && NULL_OR_EMPTY(hv->bundle) )
    {
        return;
    }

    char scope[CF_MAXVARSIZE] = { 0 };
    JoinScopeName(hv->ns, hv->bundle, scope);

    char rval[CF_MAXVARSIZE];
    if (strlen(hv->dtype) > 1) // list
    {
        PrintRlist(rval, CF_MAXVARSIZE - 1, hv->rval.item);
    }
    else
    {
        snprintf(rval, CF_MAXVARSIZE, "%s", (char *) hv->rval.item);
    }

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%ld\n",
             NULLStringToEmpty(hv->hh->hostname),
             scope,
             DataTypeShortToType(hv->dtype),
             NULLStringToEmpty(hv->lval),
             NULLStringToEmpty(rval),
             hv->t);
}

/*****************************************************************************/

void HubTotalComplianceToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubTotalCompliance *ht = (HubTotalCompliance *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%d,%d,%d,%ld\n",
             NULLStringToEmpty(ht->hh->hostname),
             NULLStringToEmpty(ht->version),
             ht->kept,
             ht->repaired,
             ht->notkept,
             ht->t);
}

/*****************************************************************************/

void HubPromiseComplianceToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubPromiseCompliance *hp = (HubPromiseCompliance *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",%f,%f,%ld\n",
             NULLStringToEmpty(hp->hh->hostname),
             NULLStringToEmpty(hp->handle),
             NULLStringToEmpty(Nova_LongState(hp->status)),
             hp->e,
             hp->d,
             hp->t);
}

/*****************************************************************************/

void HubPromiseComplianceWeightedToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubPromiseCompliance *hp = (HubPromiseCompliance *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",%f,%f,%ld,\"%s\"\n",
             NULLStringToEmpty(hp->hh->hostname),
             NULLStringToEmpty(hp->handle),
             NULLStringToEmpty(Nova_LongState(hp->status)),
             hp->e,
             hp->d,
             hp->t,
             Nova_HostColourToString(hp->hh->colour));
}

/*****************************************************************************/

void HubLastseenToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubLastSeen *hl = (HubLastSeen *) data;

    char inout[CF_SMALLBUF];
    switch (hl->direction)
    {
    case LAST_SEEN_DIRECTION_OUTGOING:
        snprintf(inout, CF_SMALLBUF, "by us (+)");
        break;
    case LAST_SEEN_DIRECTION_INCOMING:
        snprintf(inout, CF_SMALLBUF, "by them (-)");
        break;
    }

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",\"%s\",%ld,%f,%f,%f,\"%s\"\n",
             NULLStringToEmpty(hl->hh->hostname),
             NULLStringToEmpty(inout),
             NULLStringToEmpty(hl->rhost->hostname),
             NULLStringToEmpty(hl->rhost->ipaddr),
             hl->t,
             hl->hrsago,
             hl->hrsavg,
             hl->hrsdev,
             NULLStringToEmpty(hl->rhost->keyhash));
}

/*****************************************************************************/

void HubPerformanceToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubPerformance *hp = (HubPerformance *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%f,%f,%f,%ld\n",
             NULLStringToEmpty(hp->hh->hostname),
             NULLStringToEmpty(hp->event),
             hp->q,
             hp->e,
             hp->d,
             hp->t);
}

/*****************************************************************************/

void HubSetuidToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubSetUid *hu = (HubSetUid *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\"\n",
             NULLStringToEmpty(hu->hh->hostname),
             NULLStringToEmpty(hu->path));
}

/*****************************************************************************/

void HubFileChangesToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubFileChanges *hC = (HubFileChanges *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",%ld\n",
             NULLStringToEmpty(hC->hh->hostname),
             NULLStringToEmpty(hC->path),
             NULLStringToEmpty(hC->msg),
             hC->t);
}

/*****************************************************************************/

void HubValueToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubValue *hV = (HubValue *) data;

    snprintf(buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%f,%f,%f\n",
             NULLStringToEmpty(hV->hh->hostname),
             NULLStringToEmpty(hV->day),
             hV->kept,
             hV->repaired,
             hV->notkept);
}

/*****************************************************************************/

void HubPromiseLogToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubPromiseLog *hp = (HubPromiseLog *) data;

    snprintf( buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\",%ld\n",
              NULLStringToEmpty(hp->hh->hostname),
              NULLStringToEmpty(hp->handle),
              NULLStringToEmpty(hp->cause),
              hp->t);
}

/*****************************************************************************/
void HubPromiseSumToCSV( void *data, char buffer[CF_MAXVARSIZE])
{
    HubPromiseSum *hS = (HubPromiseSum *) data;

    snprintf( buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%d\n",
              hS->handle,
              hS->cause,
              hS->occurences);
}

/*****************************************************************************/
void HubFileDiffToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubFileDiff *hd = (HubFileDiff *) data;

    assert( hd );

    const char *diff_str = NULLStringToEmpty(hd->diff);

    char tline[CF_BUFSIZE] = { 0 };
    buffer[0] = '\0';
    for (const char *sp = diff_str; *sp != '\0'; sp += strlen(tline) + 1)
    {
        int line = 0;
        char pm[2] = { 0 };

        char diff[CF_BUFSIZE] = { 0 };
        sscanf(sp, "%c,%d,%2047[^\n]", pm, &line, diff);
        sscanf(sp, "%2047[^\n]", tline);

        char csv_line[CF_BUFSIZE];
        snprintf(csv_line, CF_BUFSIZE - 1, "\"%s\",\"%s\",%ld,\"%s\",%d,\"%s\"\n",
                 NULLStringToEmpty(hd->hh->hostname),
                 NULLStringToEmpty(hd->path),
                 hd->t,
                 pm,
                 line,
                 diff);

        strncat(buffer, csv_line, CF_BUFSIZE - 1);
    }
}

/*****************************************************************************/

void HubBundleSeenToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubBundleSeen *hb = (HubBundleSeen *) data;

    snprintf( buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%ld,%f,%f,%f\n",
              NULLStringToEmpty(hb->hh->hostname),
              NULLStringToEmpty(hb->bundle),
              hb->t,
              hb->bundlecomp,
              hb->bundleavg,
              hb->bundledev );
}

/*****************************************************************************/

void HubBundleSeenWeightedToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubBundleSeen *hb = (HubBundleSeen *) data;

    snprintf( buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",%ld,%f,%f,%f,\"%s\"\n",
              NULLStringToEmpty(hb->hh->hostname),
              NULLStringToEmpty(hb->bundle),
              hb->t,
              hb->bundlecomp,
              hb->bundleavg,
              hb->bundledev,
              Nova_HostColourToString(hb->hh->colour));
}

/*****************************************************************************/

void HubHostToCSV( void *data, char buffer[CF_BUFSIZE])
{
    HubHost *hh = (HubHost *) data;

    snprintf( buffer, CF_BUFSIZE - 1, "\"%s\",\"%s\",\"%s\"\n",
              NULLStringToEmpty(hh->keyhash),
              NULLStringToEmpty(hh->hostname),
              NULLStringToEmpty(hh->ipaddr) );
}

/*****************************************************************************/

Writer *ExportWebReportStart( WebReportFileInfo *wr_info )
{  
  assert( wr_info );
  assert( wr_info->write_data );

    if( !ExportWebReportStatusInitialize( wr_info ) )
    {
      return NULL;
    }

     Writer *writer = NULL;

     struct stat s;
     if (stat(wr_info->csv_path, &s) == -1)
     {
         writer = FileWriter( fopen( wr_info->csv_path, "w" ) );
     }
     else
     {
         writer = FileWriter( fopen( wr_info->csv_path, "r+" ) );
     }

     // TODO: write header 
    return writer;
}

/*****************************************************************************/

bool ExportWebReportUpdate( Writer *writer, void *data,
                            void (*fn_ptr_get_csv)(void *, char *),
                            WebReportFileInfo *wr_info )
{
    assert( wr_info );
    assert( wr_info->total_lines >= 0 );

    if( !wr_info->write_data )
    {
        wr_info->total_lines++;
        return true;
    }

    assert( writer );
    assert( wr_info->lines_written >= 0 );
    assert( wr_info->lines_since_last_update >= 0 );

    char buffer[CF_BUFSIZE] = "\0";
    (*fn_ptr_get_csv)( data, buffer );

    WriterWriteF(writer, "%s", buffer);

    wr_info->lines_written++;
    
    if( wr_info->lines_since_last_update++ >= CHECKPOINT )
    {
        wr_info->lines_since_last_update = 0;

        if( !ExportWebReportUpdateStatus( wr_info ))
        {
            return false;
        }
    }

    return true;
}

/*****************************************************************************/
