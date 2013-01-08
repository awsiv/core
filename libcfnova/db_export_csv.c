#include "db_export_csv.h"
#include "conversion.h"
#include "bson_lib.h"
#include "bson_hub_conversion.h"
#include "db_common.h"
#include "db_query.h"
#include "scope.h"
#include "csv_writer.h"
#include "string_lib.h"

#include <assert.h>

/*****************************************************************************/
static void ConvertUnixTimeToDateString(time_t *timestamp,char *buffer,int bufsize)
{
    strftime (buffer, bufsize, "%FT%H:%M:%S", gmtime(timestamp));
}

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
    bool ret = ExportWebReportWriteStatusString( done_str, wr_info->csv_path, false );

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

    double lines_written = wr_info->lines_written;
    double total_lines = wr_info->total_lines;

    int percent_done = (lines_written / total_lines) * 100.0;

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

void HubClassToCSV( void *data, Writer *w )
{
    assert(w);
    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Class or Context");
        CsvWriterField(c, "in %% runs");
        CsvWriterField(c, "+/- %%");
        CsvWriterField(c, "Last occured");

        CsvWriterClose(c);
        return;
    }

    HubClass *hc = (HubClass *) data;

    assert(hc);
    assert(hc->hh);

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hc->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hc->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hc->class));
    CsvWriterFieldF(c, "%f", hc->prob * 100.0);
    CsvWriterFieldF(c, "%f", hc->dev * 100.0);
    CsvWriterFieldF(c, "%s", time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubSoftwareToCSV( void *data, Writer *w )
{
    assert(w);
    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Package name");
        CsvWriterField(c, "Version");
        CsvWriterField(c, "Architecture");
        CsvWriterField(c, "Last seen");

        CsvWriterClose(c);
        return;
    }

    HubSoftware *hs = (HubSoftware *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hs->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->name));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->version));
    CsvWriterFieldF(c, "%s", Nova_LongArch(hs->arch));
    CsvWriterFieldF(c, "%s", time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubPatchesToCSV( void *data, Writer *w )
{
    assert(w);
    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Package name");
        CsvWriterField(c, "Version");
        CsvWriterField(c, "Architecture");

        CsvWriterClose(c);
        return;
    }

    HubSoftware *hs = (HubSoftware *) data;

    CsvWriter *c = CsvWriterOpen(w);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->name));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hs->version));
    CsvWriterFieldF(c, "%s", Nova_LongArch(hs->arch));

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubVariablesToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Scope or bundle");
        CsvWriterField(c, "Type");
        CsvWriterField(c, "Name");
        CsvWriterField(c, "Value");
        CsvWriterField(c, "Last seen");

        CsvWriterClose(c);
        return;
    }

    HubVariable *hv = (HubVariable *) data;

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

    CsvWriter *c = CsvWriterOpen(w);

    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hv->t, time_buffer, CF_SMALLBUF);


    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hv->hh->hostname));
    CsvWriterFieldF(c, "%s", scope);
    CsvWriterFieldF(c, "%s", DataTypeShortToType(hv->dtype));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hv->lval));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(rval));
    CsvWriterFieldF(c, "%s", time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubTotalComplianceToCSV(void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Policy version");
        CsvWriterField(c, "Kept");
        CsvWriterField(c, "Repaired");
        CsvWriterField(c, "Not kept");
        CsvWriterField(c, "Last verified");

        CsvWriterClose(c);
        return;
    }

    HubTotalCompliance *ht = (HubTotalCompliance *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&ht->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(ht->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(ht->version));
    CsvWriterFieldF(c, "%d", ht->kept);
    CsvWriterFieldF(c, "%d", ht->repaired);
    CsvWriterFieldF(c, "%d", ht->notkept);
    CsvWriterFieldF(c, "%s",time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubPromiseComplianceToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Promise handle");
        CsvWriterField(c, "Last known state");
        CsvWriterField(c, "%% Runs kept");
        CsvWriterField(c, "+/- %%");
        CsvWriterField(c, "Last verified");

        CsvWriterClose(c);
        return;
    }

    HubPromiseCompliance *hp = (HubPromiseCompliance *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hp->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->handle));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(Nova_LongState(hp->status)));
    CsvWriterFieldF(c, "%f", hp->e);
    CsvWriterFieldF(c, "%f", hp->d);
    CsvWriterFieldF(c, "%s",time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubPromiseComplianceWeightedToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Promise handle");
        CsvWriterField(c, "Last known state");
        CsvWriterField(c, "%% Runs kept");
        CsvWriterField(c, "+/- %%");
        CsvWriterField(c, "Last verified");
        CsvWriterField(c, "Host compliance status");

        CsvWriterClose(c);
        return;
    }

    HubPromiseCompliance *hp = (HubPromiseCompliance *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hp->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->handle));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(Nova_LongState(hp->status)));
    CsvWriterFieldF(c, "%f", hp->e);
    CsvWriterFieldF(c, "%f", hp->d);
    CsvWriterFieldF(c, "%s", time_buffer);
    CsvWriterFieldF(c, "%s", Nova_HostColourToString(hp->hh->colour));

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubLastseenToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Comms initiated");
        CsvWriterField(c, "Remote host name");
        CsvWriterField(c, "Remote IP address");
        CsvWriterField(c, "Was last seen at");
        CsvWriterField(c, "Hrs ago");
        CsvWriterField(c, "Avg Comms interval");
        CsvWriterField(c, "+/- hrs");
        CsvWriterField(c, "Remote host's key");

        CsvWriterClose(c);
        return;
    }

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

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hl->t, time_buffer, CF_SMALLBUF);


    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hl->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(inout));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hl->rhost->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hl->rhost->ipaddr));
    CsvWriterFieldF(c, "%s", time_buffer);
    CsvWriterFieldF(c, "%f", hl->hrsago);
    CsvWriterFieldF(c, "%f", hl->hrsavg);
    CsvWriterFieldF(c, "%f", hl->hrsdev);
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hl->rhost->keyhash));

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubPerformanceToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Event");
        CsvWriterField(c, "Last completion time (seconds)");
        CsvWriterField(c, "Avg completion time (seconds)");
        CsvWriterField(c, "+/- seconds");
        CsvWriterField(c, "Last performed");

        CsvWriterClose(c);
        return;
    }

    HubPerformance *hp = (HubPerformance *) data;

    CsvWriter *c = CsvWriterOpen(w);

    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hp->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->event));
    CsvWriterFieldF(c, "%f", hp->q);
    CsvWriterFieldF(c, "%f", hp->e);
    CsvWriterFieldF(c, "%f", hp->d);
    CsvWriterFieldF(c, "%s", time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubSetuidToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "File");

        CsvWriterClose(c);
        return;
    }

    HubSetUid *hu = (HubSetUid *) data;

    CsvWriter *c = CsvWriterOpen(w);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hu->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hu->path));

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubFileChangesToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "File");
        CsvWriterField(c, "Change type");
        CsvWriterField(c, "Change detected at");

        CsvWriterClose(c);
        return;
    }

    HubFileChanges *hC = (HubFileChanges *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hC->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hC->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hC->path));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hC->msg));
    CsvWriterFieldF(c, "%s", time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubValueToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Summary of day");
        CsvWriterField(c, "Value of promises kept");
        CsvWriterField(c, "Value of repairs");
        CsvWriterField(c, "Loss for promises not kept");

        CsvWriterClose(c);
        return;
    }

    HubValue *hV = (HubValue *) data;

    CsvWriter *c = CsvWriterOpen(w);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hV->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hV->day));
    CsvWriterFieldF(c, "%f", hV->kept);
    CsvWriterFieldF(c, "%f", hV->repaired);
    CsvWriterFieldF(c, "%f", hV->notkept);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubPromiseLogToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Promise handle");
        CsvWriterField(c, "Report");
        CsvWriterField(c, "Time");

        CsvWriterClose(c);
        return;
    }

    HubPromiseLog *hp = (HubPromiseLog *) data;

    CsvWriter *c = CsvWriterOpen(w);
    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hp->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->handle));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hp->cause));
    CsvWriterFieldF(c, "%s",time_buffer);

    CsvWriterClose(c);
}

/*****************************************************************************/
void HubPromiseSumToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Promise handle");
        CsvWriterField(c, "Report");
        CsvWriterField(c, "Occurences");

        CsvWriterClose(c);
        return;
    }

    HubPromiseSum *hS = (HubPromiseSum *) data;

    CsvWriter *c = CsvWriterOpen(w);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hS->handle));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hS->cause));
    CsvWriterFieldF(c, "%d", hS->occurences);

    CsvWriterClose(c);
}

/*****************************************************************************/
void HubFileDiffToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "File");
        CsvWriterField(c, "Change detected at");
        CsvWriterField(c, "plusminus");
        CsvWriterField(c, "line");
        CsvWriterField(c, "diff");

        CsvWriterClose(c);
        return;
    }

    HubFileDiff *hd = (HubFileDiff *) data;

    assert( hd );

    const char *diff_str = NULLStringToEmpty(hd->diff);

    CsvWriter *c = CsvWriterOpen(w);

    char tline[CF_BUFSIZE] = { 0 };
    for (const char *sp = diff_str; *sp != '\0'; sp += strlen(tline) + 1)
    {
        int line = 0;
        char pm[2] = { 0 };

        char diff[CF_BUFSIZE] = { 0 };

        char time_buffer[CF_SMALLBUF] = {0};
        ConvertUnixTimeToDateString(&hd->t, time_buffer, CF_SMALLBUF);

        sscanf(sp, "%c,%d,%2047[^\n]", pm, &line, diff);
        sscanf(sp, "%2047[^\n]", tline);



        CsvWriterFieldF(c, "%s", NULLStringToEmpty(hd->hh->hostname));
        CsvWriterFieldF(c, "%s", NULLStringToEmpty(hd->path));
        CsvWriterFieldF(c, "%s", time_buffer);
        CsvWriterFieldF(c, "%s", NULLStringToEmpty(pm));
        CsvWriterFieldF(c, "%d", line);
        CsvWriterFieldF(c, "%s", NULLStringToEmpty(diff));

        CsvWriterNewRecord(c);
    }

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubBundleSeenToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Bundle");
        CsvWriterField(c, "Last verified");
        CsvWriterField(c, "%% Compliance");
        CsvWriterField(c, "Avg %% compliance");
        CsvWriterField(c, "+/- %%");

        CsvWriterClose(c);
        return;
    }

    HubBundleSeen *hb = (HubBundleSeen *) data;

    char time_buffer[CF_SMALLBUF] = {0};

    CsvWriter *c = CsvWriterOpen(w);
    ConvertUnixTimeToDateString(&hb->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hb->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hb->bundle));
    CsvWriterFieldF(c, "%s", time_buffer);
    CsvWriterFieldF(c, "%f", hb->bundlecomp);
    CsvWriterFieldF(c, "%f", hb->bundleavg);
    CsvWriterFieldF(c, "%f", hb->bundledev);

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubBundleSeenWeightedToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Host");
        CsvWriterField(c, "Bundle");
        CsvWriterField(c, "Last verified");
        CsvWriterField(c, "%% Compliance");
        CsvWriterField(c, "Avg %% compliance");
        CsvWriterField(c, "+/- %%");
        CsvWriterField(c, "Host compliance status");

        CsvWriterClose(c);
        return;
    }

    HubBundleSeen *hb = (HubBundleSeen *) data;

    CsvWriter *c = CsvWriterOpen(w);

    char time_buffer[CF_SMALLBUF] = {0};
    ConvertUnixTimeToDateString(&hb->t, time_buffer, CF_SMALLBUF);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hb->hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hb->bundle));
    CsvWriterFieldF(c, "%s", time_buffer);
    CsvWriterFieldF(c, "%f", hb->bundlecomp);
    CsvWriterFieldF(c, "%f", hb->bundleavg);
    CsvWriterFieldF(c, "%f", hb->bundledev);
    CsvWriterFieldF(c, "%s", Nova_HostColourToString(hb->hh->colour));

    CsvWriterClose(c);
}

/*****************************************************************************/

void HubHostToCSV( void *data, Writer *w)
{
    assert(w);

    if (!data)
    {
        CsvWriter *c = CsvWriterOpen(w);

        CsvWriterField(c, "Hostkey");
        CsvWriterField(c, "Hostname");
        CsvWriterField(c, "IP address");

        CsvWriterClose(c);
        return;
    }

    HubHost *hh = (HubHost *) data;

    CsvWriter *c = CsvWriterOpen(w);

    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hh->keyhash));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hh->hostname));
    CsvWriterFieldF(c, "%s", NULLStringToEmpty(hh->ipaddr));

    CsvWriterClose(c);
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

    return writer;
}

/*****************************************************************************/

void ExportWebReportWriteHeader(Writer *writer,
                                void (*fn_ptr_get_csv)(void *, Writer *),
                                WebReportFileInfo *wr_info)
{
    assert(wr_info);
    assert(wr_info->total_lines >= 0);

    if( !wr_info->write_data )
    {
        return;
    }

    assert(writer);

    (*fn_ptr_get_csv) (NULL, writer);
}

/*****************************************************************************/

bool ExportWebReportUpdate( Writer *writer, void *data,
                            void (*fn_ptr_get_csv)(void *, Writer *),
                            WebReportFileInfo *wr_info )
{
    assert( wr_info );
    assert( wr_info->total_lines >= 0 );

    if( !wr_info->write_data )
    {
        wr_info->total_lines++;
        return true;
    }

    ExportWebReportCheckAbort(wr_info, writer);

    assert( writer );
    assert( wr_info->lines_written >= 0 );
    assert( wr_info->lines_since_last_update >= 0 );

    (*fn_ptr_get_csv) (data, writer);

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

void ExportWebReportCheckAbort(WebReportFileInfo *wr_info, Writer *w)
{
    assert(wr_info);

    struct stat s;
    if(stat(wr_info->abort_file, &s) == -1)
    {
        return;
    }

    // cleanup
    WriterClose(w);

    ExportWebReportWriteStatusString("-1", wr_info->csv_path, false);
    remove(wr_info->csv_path);

    DeleteWebReportFileInfo(wr_info);

    _exit(0);
}

/*****************************************************************************/
bool WebExportWriteChildPid(WebReportFileInfo *wr_info)
{
    assert(wr_info);

    char pid_file[CF_MAXVARSIZE] = "\0";
    snprintf(pid_file, CF_MAXVARSIZE - 1, "%s.pid", wr_info->csv_path);

    FILE *stream = fopen(pid_file, "w");
    if (!stream)
    {
        syslog(LOG_ERR, "Cannot open report file for writing: %s", pid_file);
        return false;
    }

    Writer *writer = FileWriter(stream);
    assert(writer);
    if(!writer)
    {
        return false;
    }

    WriterWriteF(writer, "%d", wr_info->child_pid);

    WriterClose( writer );
    return true;
}
/*****************************************************************************/
