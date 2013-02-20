/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"

#include "scorecards.h"
#include "bson_lib.h"
#include "db_save.h"
#include "db_query.h"
#include "db_maintain.h"
#include "web_rbac.h"
#include "files_names.h"
#include "item_lib.h"
#include "sort.h"
#include "conversion.h"
#include "scope.h"
#include "web_report_export.h"
#include "matching.h"
#include "cfstream.h"
#include "string_lib.h"
#include "communication.h"
#include "pipes.h"
#include "install.h"
#include "nova_deprecated.h"

#include <assert.h>

static const char *ERRID_DESCRIPTION[] =
{
    [ERRID_UNKNOWN] = "Unknown error",
    [ERRID_SUCCESS] = "Success",
    [ERRID_DBCONNECT] = "Could not open connection to reporting database",
    [ERRID_DBCLOSE] = "Could not close a connection to reporting database",
    [ERRID_DB_OPERATION] = "A database operation failed",
    [ERRID_ARGUMENT_MISSING] = "A mandatory argument is missing",
    [ERRID_ARGUMENT_WRONG] = "An argument is incorrect",
    [ERRID_BUFFER_FULL] = "The JSON-buffer is too small to hold the report data",
    [ERRID_ITEM_NONEXISTING] = "The given item does not exist",
    [ERRID_ITEM_EXISTS] = "The given item already exists",
    [ERRID_ITEM_MULTIPLE] = "There are multiple items matching the request",
    [ERRID_RBAC_DISABLED] = "Role-based access control is disabled",
    [ERRID_RBAC_ACCESS_DENIED] = "The given user has no access to this data",
    [ERRID_DATA_UNAVAILABLE] = "Insufficient data / Data unavailable",
    [ERRID_HOST_NOT_FOUND] = "Host not found",
    [ERRID_MAX] = "Unknown error - description out of bounds",
};

/*****************************************************************************/
// deprecate in favour of JSONErrorFromId
static cfapi_errid FormatReportInfoAsJson(char *reportId, ReportInfo *reports, char *buf, int bufsize);
static JsonElement *CreateJsonHostOnlyReport(Rlist **records_p, PageInfo *page);
static JsonElement *GenerateHostOnlyReport( Rlist **records_p, PageInfo *page,
                                            WebReportFileInfo *wr_info );
JsonElement *JSONErrorFromId(cfapi_errid errid);

/*****************************************************************************/

static void Nova_RegisterImg(Item **list, char *dir, char *pic)
{
    char work[CF_MAXVARSIZE], *sp;

    strcpy(work, pic);

    for (sp = work; *sp != '\0'; sp++)
    {
        if (*sp == '.')
        {
            *sp = '\0';
            break;
        }
    }

    PrependItem(list, pic, work);
}

/*********************************************************************/

static void Nova_RegisterDoc(Item **list, char *dir, char *doc)
{
    char title[CF_BUFSIZE] = { 0 }, path[CF_BUFSIZE], line[CF_BUFSIZE];
    int i;
    FILE *fp;

    snprintf(path, CF_BUFSIZE, "%s/%s", dir, doc);

    if ((fp = fopen(path, "r")) == NULL)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "fopen", "Couldn't open %s for reading", path);
        return;
    }

/* The title should be on line 3, but just in case something changes
   read only a few lines of these long docs
*/

    for (i = 0; i < 10; i++)
    {
        fgets(line, CF_BUFSIZE, fp);

        if (strncmp(line, "<title>", strlen("<title>")) == 0)
        {
            sscanf(line + strlen("<title>"), "%[^<]", title);
            break;
        }
    }

    fclose(fp);

    if (strlen(title) > 0)
    {
        PrependItem(list, doc, title);
    }
}

void Con2PHP_ComplianceSummaryGraph(char *hubKeyHash, char *policy, char *buffer,int bufsize)
{
    ComplianceSummaryGraph(hubKeyHash,policy,buffer,bufsize);
}


/*****************************************************************************/

void Nova2PHP_GetLibraryDocuments(char *path, char *buffer, int bufsize)
{
    char namedir[CF_MAXVARSIZE], work[CF_BUFSIZE];
    struct dirent *dirp;
    Item *refs = NULL, *guides = NULL, *others = NULL, *pics = NULL, *ip;
    DIR *dirh;

    assert(path && "NULL path passed to Nova2PHP_GetLibraryDocuments");

    snprintf(namedir, CF_MAXVARSIZE, "%s", path);

    if ((dirh = opendir(namedir)) == NULL)
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", " !! Could not open connection to report database");
        return;
    }

    for (dirp = readdir(dirh); dirp != NULL; dirp = readdir(dirh))
    {

        if (strcmp("cf3-upgrade.html", dirp->d_name) == 0)
        {
            continue;
        }
    
        if (strcmp("cf3-Reference.html", dirp->d_name) == 0 || FullTextMatch("Enterprise.*\\.html", dirp->d_name))
        {
            Nova_RegisterDoc(&refs, namedir, dirp->d_name);
        }
        else if (StringMatchFull("cf3.*\\.html", dirp->d_name))
        {
            Nova_RegisterDoc(&others, namedir, dirp->d_name);
        }
        else if (StringMatchFull("st-.*\\.html", dirp->d_name))
        {
            Nova_RegisterDoc(&guides, namedir, dirp->d_name);
        }
        else if (StringMatchFull(".*\\.png", dirp->d_name))
        {
            Nova_RegisterImg(&pics, namedir, dirp->d_name);
        }
    }

    closedir(dirh);

// Format the buffer

    refs = SortItemListNames(refs);
    pics = SortItemListNames(pics);
    guides = SortItemListNames(guides);
    others = SortItemListNames(others);

    strcpy(buffer, "[");

// Reference docs

    snprintf(work, sizeof(work), "{ \"category\" : \"Reference Section\",");
    Join(buffer, work, bufsize);

    snprintf(work, sizeof(work), "\"files\" : [");
    Join(buffer, work, bufsize);

    for (ip = refs; ip != NULL; ip = ip->next)
    {
        snprintf(work, CF_MAXVARSIZE, "{ \"filename\": \"%s\",  \"title\": \"%s\"}", ip->name, ip->classes);
        Join(buffer, work, bufsize);
        if (ip->next)
        {
            Join(buffer, ",", bufsize);
        }
    }

    Join(buffer, "]},\n", bufsize);

// Concept guide etc

    snprintf(work, sizeof(work), "{ \"category\" : \"Further Reading\",");
    Join(buffer, work, bufsize);

    snprintf(work, sizeof(work), "\"files\" : [");
    Join(buffer, work, bufsize);

    for (ip = others; ip != NULL; ip = ip->next)
    {
        snprintf(work, CF_MAXVARSIZE, "{ \"filename\": \"%s\",  \"title\": \"%s\"}", ip->name, ip->classes);
        Join(buffer, work, bufsize);
        if (ip->next)
        {
            Join(buffer, ",\n", bufsize);
        }
    }

    Join(buffer, "]},\n", bufsize);

// Concept guides

    snprintf(work, sizeof(work), "{ \"category\" : \"Special Topics\",");
    Join(buffer, work, bufsize);

    snprintf(work, sizeof(work), "\"files\" : [");
    Join(buffer, work, bufsize);

    for (ip = guides; ip != NULL; ip = ip->next)
    {
        snprintf(work, CF_MAXVARSIZE, "{ \"filename\": \"%s\",  \"title\": \"%s\"}", ip->name, ip->classes);
        Join(buffer, work, bufsize);
        if (ip->next)
        {
            Join(buffer, ",\n", bufsize);
        }
    }

    Join(buffer, "]},\n", bufsize);

// Pictures

    snprintf(work, sizeof(work), "{ \"category\" : \"Illustrations\",");
    Join(buffer, work, bufsize);

    snprintf(work, sizeof(work), "\"files\" : [");
    Join(buffer, work, bufsize);

    for (ip = pics; ip != NULL; ip = ip->next)
    {
        snprintf(work, CF_MAXVARSIZE, "{ \"filename\": \"%s\",  \"title\": \"%s\"}", ip->name, ip->classes);
        Join(buffer, work, bufsize);
        if (ip->next)
        {
            Join(buffer, ",\n", bufsize);
        }
    }

    Join(buffer, "]}\n", bufsize);

    Join(buffer, "]", bufsize);

// Cleanup

    DeleteItemList(refs);
    DeleteItemList(pics);
    DeleteItemList(guides);
    DeleteItemList(others);
}

void Nova2PHP_getlastupdate(char *hostkey, char *buffer, int bufsize)
{
    time_t then;
    EnterpriseDB dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    if (hostkey && strlen(hostkey) > 0)
    {
        then = 0;
        int size = 0;
        CFDB_QueryLastUpdate(&dbconn, MONGO_DATABASE, cfr_keyhash, hostkey, &then, &size);

        if (then > 0)
        {
            snprintf(buffer, bufsize, "%ld", then);
        }
        else
        {
            snprintf(buffer, bufsize, "never");
        }
    }
    else
    {
        snprintf(buffer, bufsize, "never");
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }
}

/*****************************************************************************/
void Nova2PHP_summary_meter(char *buffer, int bufsize)
{
    bson query[1];

    bson_empty(query);

    Nova_Meter(query, MONGO_DATABASE, buffer, bufsize, PROMISE_CONTEXT_MODE_ALL);
}

/*****************************************************************************/

JsonElement *Nova2PHP_promise_compliance_summary (char *hostkey, char *handle,
                                                  char *status, bool regex,
                                                  HostClassFilter *hostClassFilter,
                                                  PromiseContextMode promise_context)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    EnterpriseDB dbconn;
    char *current_host = "x";

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;

    HubQuery *hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                                       0, time(NULL),  hostClassFilter, NULL,
                                                       promise_context, NULL, db_options);

    int blue_hosts = 0,
        tot_hosts = 0,
        black_hosts = 0,
        red_hosts = 0,
        green_hosts = 0,
        yellow_hosts = 0;

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *hp = (HubPromiseCompliance *) rp->item;

        if (current_host && strcmp(hp->hh->keyhash, current_host) != 0)     // New host
        {
            switch(hp->hh->colour)
            {
            case HOST_COLOUR_BLUE:
                blue_hosts++;
                break;

            case HOST_COLOUR_BLACK:
                black_hosts++;
                break;

            case HOST_COLOUR_GREEN:
                green_hosts++;
                break;

            case HOST_COLOUR_RED:
                red_hosts++;
                break;

            default:
                tot_hosts--;
                break;
            }
            tot_hosts++;
            current_host = hp->hh->keyhash;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *jsonRetval = JsonObjectCreate(6);
    JsonObjectAppendInteger(jsonRetval,"green", green_hosts);
    JsonObjectAppendInteger(jsonRetval, "yellow", yellow_hosts);
    JsonObjectAppendInteger(jsonRetval, "red", red_hosts);
    JsonObjectAppendInteger(jsonRetval, "blue_hosts", blue_hosts);
    JsonObjectAppendInteger(jsonRetval, "black_hosts", black_hosts);
    JsonObjectAppendInteger(jsonRetval, "host_count", tot_hosts);

    return jsonRetval;
}

/*****************************************************************************/

JsonElement *Nova2PHP_bundle_compliance_summary (char *hostkey, char *scope, bool regex,
                                                 HostClassFilter *hostClassFilter,
                                                 PromiseContextMode promise_context)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    char ns[CF_MAXVARSIZE] = "\0";
    char bundle[CF_MAXVARSIZE] = "\0";
    if (scope)
    {
        SplitScopeName(scope, ns, bundle);
    }

    int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;

    HubQuery *hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, ns, bundle,
                                                hostClassFilter, NULL,
                                                promise_context, NULL, db_options);

    int tot_hosts = 0;
    int blue_hosts = 0,
        black_hosts = 0,
        red_hosts = 0,
        green_hosts = 0,
        yellow_hosts = 0;
    char *current_host = "x";

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubBundleSeen *hb = (HubBundleSeen *) rp->item;

        if (current_host && strcmp(hb->hh->keyhash, current_host) != 0)     // New host
        {
            switch(hb->hh->colour)
            {
            case HOST_COLOUR_BLUE:
                blue_hosts++;
                break;

            case HOST_COLOUR_BLACK:
                black_hosts++;
                break;

            case HOST_COLOUR_GREEN:
                green_hosts++;
                break;

            case HOST_COLOUR_RED:
                red_hosts++;
                break;

            default:
                tot_hosts--;
                break;
            }
            tot_hosts++;
            current_host = hb->hh->keyhash;
        }
    }

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *jsonRetval = JsonObjectCreate(6);
    JsonObjectAppendInteger(jsonRetval,"green", green_hosts);
    JsonObjectAppendInteger(jsonRetval, "yellow", yellow_hosts);
    JsonObjectAppendInteger(jsonRetval, "red", red_hosts);
    JsonObjectAppendInteger(jsonRetval, "blue_hosts", blue_hosts);
    JsonObjectAppendInteger(jsonRetval, "black_hosts", black_hosts);
    JsonObjectAppendInteger(jsonRetval, "host_count", tot_hosts);

    return jsonRetval;
}

/*****************************************************************************/

void Nova2PHP_meter(char *hostkey, char *buffer, int bufsize, PromiseContextMode promise_context)
{
    bson query;

    bson_init(&query);

    if (!NULL_OR_EMPTY(hostkey))
    {
        bson_append_string(&query, cfr_keyhash, hostkey);
    }

    BsonAppendClassFilterFromPromiseContext(&query, promise_context);

    BsonFinish(&query);

    Nova_Meter(&query, MONGO_DATABASE, buffer, bufsize, promise_context);

    bson_destroy(&query);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

JsonElement *Nova2PHP_vitals_list(char *hostkey)
{
    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        return false;
    }

    JsonElement *payload = JsonObjectCreate(4);
    {
        char hostname[CF_MAXVARSIZE] = { 0 },
             ip[CF_MAXVARSIZE] = { 0 };
        Nova2PHP_hostinfo(hostkey, hostname, ip, sizeof(hostname));

        int size = 0;
        time_t last_update = 0;
        CFDB_QueryLastUpdate(conn, MONGO_DATABASE, cfr_keyhash, hostkey, &last_update, &size);

        JsonObjectAppendString(payload, "hostname", NULLStringToEmpty(hostname));
        JsonObjectAppendString(payload, "ip", NULLStringToEmpty(ip));
        JsonObjectAppendInteger(payload, "ls", last_update);
    }
    {
        HubQuery *result = CFDB_QueryVitalsMeta(conn, hostkey);
        CFDB_Close(conn);
        JsonElement *observations = JsonArrayCreate(RlistLen(result->records));

        for (const Rlist *rp = result->records; rp; rp = rp->next)
        {
            const HubVital *vital = rp->item;

            JsonElement *entry = JsonObjectCreate(3);
            JsonObjectAppendString(entry, "id", NULLStringToEmpty(vital->id));
            JsonObjectAppendString(entry, "units", NULLStringToEmpty(vital->units));
            JsonObjectAppendString(entry, "desc", NULLStringToEmpty(vital->description));
            JsonArrayAppendArray(observations, entry);
        }

        DeleteHubQuery(result, DeleteHubVital);
        JsonObjectAppendArray(payload, "obs", observations);
    }

    return payload;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_magnified(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    DataView cfv = { 0 };
    char work[CF_MAXVARSIZE];
    bool haveData = false;
    int i;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    haveData = Nova_ReadMagTimeSeries2(&dbconn, &cfv, keyHash, vitalId);

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    if (haveData)
    {
        for (i = 0; i < CF_MAGDATA; i++)
        {
            snprintf(work, sizeof(work), " [%d,%lf,%lf,%lf],", i, cfv.data_q[i], cfv.data_E[i], cfv.bars[i]);
            Join(buffer, work, bufsize);
        }
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]", bufsize);

    return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_week(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    DataView cfv = { 0 };
    char work[CF_MAXVARSIZE];
    bool haveData = false;
    int i;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    haveData = Nova_ReadWeekTimeSeries2(&dbconn, &cfv, keyHash, vitalId);

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    if (haveData)
    {
        for (i = 0; i < CF_TIMESERIESDATA; i++)
        {
            snprintf(work, sizeof(work), " [%d,%lf,%lf,%lf],", i, cfv.data_q[i], cfv.data_E[i], cfv.bars[i]);
            Join(buffer, work, bufsize);
        }
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]", bufsize);

    return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_year(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    DataView cfv = { 0 };
    char work[CF_MAXVARSIZE];
    bool haveData = false;
    int i;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    haveData = Nova_ReadYearTimeSeries(&dbconn, &cfv, keyHash, vitalId);

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    if (haveData)
    {
        for (i = 0; i < CF_YEAR_SLOTS; i++)
        {
            snprintf(work, sizeof(work), " [%d,%lf,%lf,%lf],", i, cfv.data_q[i], cfv.data_E[i], cfv.bars[i]);
            Join(buffer, work, bufsize);
        }
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]", bufsize);

    return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_view_histogram(char *keyHash, char *vitalId, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    DataView cfv = { 0 };
    char work[CF_MAXVARSIZE];
    bool haveData = false;
    int i;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    haveData = Nova_ReadHistogram2(&dbconn, &cfv, keyHash, vitalId);

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    if (haveData)
    {
        for (i = 0; i < CF_GRAINS; i++)
        {
            snprintf(work, sizeof(work), " [%d,%d],", i, (int) round(cfv.data_E[i]));
            Join(buffer, work, bufsize);
        }
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]", bufsize);

    return haveData;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_magnified(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    char work[CF_BUFSIZE];
    DataView cfv = { 0 };

    buffer[0] = '\0';

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!Nova_ReadMagTimeSeries2(&dbconn, &cfv, hostkey, vitalId))
    {
        CFDB_Close(&dbconn);
        return false;
    }

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    if (cfv.max - cfv.min < cfv.error_scale * 2)
    {
        snprintf(work, sizeof(work), "\"No significant variations\",");
        Join(buffer, work, bufsize);
    }
    else
    {
        snprintf(work, sizeof(work), "\"Significant variations\",");
        Join(buffer, work, bufsize);
    }

    snprintf(work, sizeof(work), "\"Maximum value: %.2lf\",", cfv.max);
    Join(buffer, work, bufsize);
    snprintf(work, sizeof(work), "\"Minimum value: %.2lf\",", cfv.min);
    Join(buffer, work, bufsize);
    snprintf(work, sizeof(work), "\"Average variability: %.2lf\",", cfv.error_scale);
    Join(buffer, work, bufsize);

    ReplaceTrailingChar(buffer, ',', '\0');
    Join(buffer, "]", bufsize);

    return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_week(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
    char work[CF_BUFSIZE];
    double x;
    DataView cfv = { 0 };
    EnterpriseDB dbconn;

    buffer[0] = '\0';

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!Nova_ReadWeekTimeSeries2(&dbconn, &cfv, hostkey, vitalId))
    {
        CFDB_Close(&dbconn);
        return false;
    }

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    snprintf(work, CF_BUFSIZE - 1, "\"Maximum value: %.2lf\",", cfv.max);
    Join(buffer, work, bufsize);
    snprintf(work, CF_BUFSIZE - 1, "\"Minimum value %.2lf\",", cfv.min);
    Join(buffer, work, bufsize);
    snprintf(work, CF_BUFSIZE, "\"Average variability: %.2lf\",", cfv.error_scale);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.over / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage over average/normal: %.2lf%%\",", x);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.under / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage under average/normal: %.2lf%%\",", x);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.over_dev1 / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage 1 deviation over mean: %.2lf%%\",", x);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.under_dev1 / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage 1 deviation under mean: %.2lf%%\",", x);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.over_dev2 / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage 2 deviations over mean: %.2lf%%\",", x);
    Join(buffer, work, bufsize);
    x = 100 * (double) cfv.under_dev2 / (double) CF_TIMESERIESDATA;
    snprintf(work, CF_BUFSIZE - 1, "\"Percentage 2 deviations under mean: %.2lf%%\",", x);
    Join(buffer, work, bufsize);

    ReplaceTrailingChar(buffer, ',', '\0');
    Join(buffer, "]", bufsize);

    return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_year(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
    char work[CF_BUFSIZE];
    DataView cfv = { 0 };
    EnterpriseDB dbconn;

    buffer[0] = '\0';

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!Nova_ReadYearTimeSeries(&dbconn, &cfv, hostkey, vitalId))
    {
        CFDB_Close(&dbconn);
        return false;
    }

    CFDB_Close(&dbconn);

    strcpy(buffer, "[");

    snprintf(work, CF_BUFSIZE, "\"Maximum value: %.2lf\",", cfv.max);
    Join(buffer, work, bufsize);
    snprintf(work, CF_BUFSIZE, "\"Minimum value: %.2lf\",", cfv.min);
    Join(buffer, work, bufsize);
    snprintf(work, CF_BUFSIZE, "\"Average variability: %.2lf\",", cfv.error_scale);
    Join(buffer, work, bufsize);

    ReplaceTrailingChar(buffer, ',', '\0');
    Join(buffer, "]", bufsize);

    return true;
}

/*****************************************************************************/

bool Nova2PHP_vitals_analyse_histogram(char *hostkey, char *vitalId, char *buffer, int bufsize)
{
    double sx, delta, sum = 0, sigma2;
    int new_gradient = 0, past_gradient = 0, max = 0;
    int redshift = 0, blueshift = 0;
    int above_noise = false;
    char work[CF_BUFSIZE];
    double sensitivity_factor = 1.2;
    DataView cfv = { 0 };
    Item *spectrum;
    EnterpriseDB dbconn;

    buffer[0] = '\0';

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!Nova_ReadHistogram2(&dbconn, &cfv, hostkey, vitalId))
    {
        CFDB_Close(&dbconn);
        return false;
    }

    CFDB_Close(&dbconn);

    spectrum = Nova_MapHistogram(&cfv, hostkey);

    strcpy(buffer, "[");

    for (sx = 1; sx < CF_GRAINS; sx++)
    {
        delta = cfv.data_E[(int) sx] - cfv.data_E[(int) (sx - 1)];
        sum += delta * delta;
    }

    sigma2 = sum / (double) CF_GRAINS;

    snprintf(work, CF_BUFSIZE - 1, "\"Maximum frequency %s = %.2lf\",", vitalId, cfv.max);
    Join(buffer, work, bufsize);
    snprintf(work, CF_BUFSIZE - 1, "\"Minimum frequency %s = %.2lf\",", vitalId, cfv.min);
    Join(buffer, work, bufsize);

    for (sx = 1; sx < CF_GRAINS; sx++)
    {
        delta = cfv.data_E[(int) sx] - cfv.data_E[(int) (sx - 1)];

        above_noise = (delta * delta > sigma2) * sensitivity_factor;

        if (above_noise)
        {
            new_gradient = delta;

            if (new_gradient < 0 && past_gradient >= 0)
            {
                max++;

                snprintf(work, CF_BUFSIZE - 1, "\"%d: Spectral mode with peak at %.0lf/%.0lf grains\", ", max, sx - 1,
                         (double) CF_GRAINS);
                Join(buffer, work, bufsize);

                if (sx < ((double) CF_GRAINS) / 2.0 - 1.0)
                {
                    redshift++;
                    snprintf(work, CF_BUFSIZE - 1,
                             "\"red-shifted, e.g. a retardation process where usage is declining. "
                             "If the distribution is skewed, it has a long ramp, indicating "
                             "a possible resource ceiling, a well-utilized system. "
                             "Or there could be outliers of low value, because data are incomplete.\",");
                    Join(buffer, work, bufsize);
                }
                else if (sx > ((double) CF_GRAINS) / 2.0 + 1.0)
                {
                    blueshift++;
                    snprintf(work, CF_BUFSIZE - 1,
                             "\"blue-shifted, e.g. an acceleration process where usage is increasing. "
                             "If the distribution is skewed, it has a long tail, indicating "
                             "plenty of resources, or an under-used system. "
                             "Or there could be outliers of low value, because data are incomplete.\",");
                    Join(buffer, work, bufsize);
                }
            }
        }

        past_gradient = new_gradient;
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    Join(buffer, "]", bufsize);
    DeleteItemList(spectrum);

    return true;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

JsonElement *Nova2PHP_promiselog(char *hostkey, char *handle, char *causeRx, PromiseLogState state,
                                 time_t from, time_t to, HostClassFilter *hostClassFilter,
                                 PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, causeRx,
                                        from, to, true, hostClassFilter, NULL, promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPromiseLog);

    JsonElement *payload = JsonObjectCreate(3);
    {
        JsonElement *meta = JsonObjectCreate(2);
        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(5);
            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Promise Handle", 1);
            JsonObjectAppendInteger(header, "Report", 2);
            JsonObjectAppendInteger(header, "Time", 3);
            JsonObjectAppendObject(meta, "header", header);
        }
        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            HubPromiseLog *hp = (HubPromiseLog *) rp->item;

            JsonElement *entry = JsonArrayCreate(4);

            JsonArrayAppendString(entry, NULLStringToEmpty(hp->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hp->handle));
            JsonArrayAppendString(entry, NULLStringToEmpty(hp->cause));
            JsonArrayAppendInteger(entry, hp->t);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }


    DeleteHubQuery(hq, DeleteHubPromiseLog);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promiselog_summary(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page,
                                         PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPromiseLogSummary(&dbconn, hostkey, state, handle,
                                               true, causeRx, from, to, true,
                                               hostClassFilter, promise_context, NULL);

    CFDB_Close(&dbconn);

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = (page->resultsPerPage * page->pageNum) - 1;

    JsonElement *meta = JsonObjectCreate(2);
    {
        JsonObjectAppendInteger(meta, "count", RlistLen(hq->records));
        JsonObjectAppendInteger(meta, "related", RlistLen(hq->hosts));

        JsonElement *header = JsonObjectCreate(3);

        JsonObjectAppendInteger(header, "Promise Handle", 0);
        JsonObjectAppendInteger(header, "Report", 1);
        JsonObjectAppendInteger(header, "Occurences", 2);

        JsonObjectAppendObject(meta, "header", header);
    }

    JsonElement *data = JsonArrayCreate(1000);
    int i = 0;
    for (const Rlist *rp = hq->records; rp; rp = rp->next, i++)
    {
        if (i >= startIndex && (i <= endIndex || endIndex < 0))
        {
            const HubPromiseSum *record = (const HubPromiseSum *)rp->item;

            JsonElement *entry = JsonArrayCreate(3);
            JsonArrayAppendString(entry, record->handle);
            JsonArrayAppendString(entry, record->cause);
            JsonArrayAppendInteger(entry, record->occurences);

            JsonArrayAppendArray(data, entry);
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseSum);

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data);

    return output;
}

/*****************************************************************************/

JsonElement *Nova2PHP_value_report(char *hostkey, char *day, char *month, char *year,
                                   HostClassFilter *hostClassFilter, PageInfo *page,
                                   PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year,
                                         hostClassFilter, promise_context, NULL,
                                         QUERY_FLAG_SORT_RESULT);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubValue);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Summary of Day", 1);
            JsonObjectAppendInteger(header, "Value of Promises Kept", 2);
            JsonObjectAppendInteger(header, "Value of Repairs", 3);
            JsonObjectAppendInteger(header, "Loss for Promises Not Kept", 4);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubValue *hp = (HubValue *) rp->item;

            JsonElement *entry = JsonArrayCreate(5);

            JsonArrayAppendString(entry, NULLStringToEmpty(hp->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hp->day));
            JsonArrayAppendReal(entry, hp->kept);
            JsonArrayAppendReal(entry, hp->repaired);
            JsonArrayAppendReal(entry, hp->notkept);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubValue);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/
int Nova2PHP_get_value_graph(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                             int bufsize)
{
    HubValue *hp;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
    char buffer[CF_BUFSIZE] = { 0 };

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryValueGraph(&dbconn, hostkey, day, month, year, true, classreg);

    strcpy(returnval, "[");

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubValue *) rp->item;

        snprintf(buffer, sizeof(buffer), "[%s,%.1lf,%.1lf,%.1lf],", hp->day, hp->kept, hp->repaired, hp->notkept);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]\n", bufsize);

    DeleteHubQuery(hq, DeleteHubValue);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

JsonElement *Nova2PHP_classes_summary(char **classes)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryClassSum(&dbconn, classes);

    JsonElement *payload = JsonObjectCreate(2);

    if (hq && hq->hosts)
    {
        {
            JsonElement *hosts = JsonArrayCreate(RlistLen(hq->hosts));
            for (const Rlist *rp = hq->hosts; rp != NULL; rp = rp->next)
            {
                const HubHost *hh = (HubHost *) rp->item;

                JsonElement *entry = JsonArrayCreate(2);

                JsonArrayAppendString(entry, NULLStringToEmpty(hh->hostname));
                JsonArrayAppendString(entry, NULLStringToEmpty(hh->keyhash));

                JsonArrayAppendArray(hosts, entry);
            }
            JsonObjectAppendArray(payload, "hosts", hosts);
        }

        {
            JsonElement *classes = JsonArrayCreate(RlistLen(hq->records));
            for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
            {
                const HubClassSum *hc = (HubClassSum *) rp->item;

                JsonElement *entry = JsonArrayCreate(2);

                JsonArrayAppendString(entry, NULLStringToEmpty(hc->class));
                JsonArrayAppendInteger(entry, hc->frequency);

                JsonArrayAppendArray(classes, entry);
            }
            JsonObjectAppendArray(payload, "classes", classes);
        }
    }

    DeleteHubQuery(hq, DeleteHubClassSum);

    CFDB_Close(&dbconn);

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_vars_report(const char *hostkey, const char *scope, const char *lval,
                                  const char *rval, const char *type, bool regex,
                                  const HostClassFilter *hostClassFilter, PageInfo *page,
                                  PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq;
    {
        char ns[CF_MAXVARSIZE] = { 0 };
        char bundle[CF_MAXVARSIZE] = { 0 };
        if (scope)
        {
            SplitScopeName(scope, ns, bundle);
        }

        int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;
        hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type,
                                 0, time(NULL), hostClassFilter, promise_context, NULL, db_options);
    }
    assert(hq);
    if (!hq)
    {
        return NULL;
    }

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubVariable);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        char lscope[CF_MAXVARSIZE] = { 0 };
        JsonElement *scope_entry = NULL;
        JsonElement *scope_data = NULL;
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubVariable *hv = (HubVariable *) rp->item;
            char scope[CF_MAXVARSIZE] = { 0 };
            JoinScopeName(hv->ns, hv->bundle, scope);

            if (!StringSafeEqual(lscope, scope))
            {
                if (scope_entry)
                {
                    assert(scope_data);
                    JsonObjectAppendInteger(scope_entry, "count", JsonElementLength(scope_data));
                    scope_entry = NULL;
                    scope_data = NULL;
                }

                strcpy(lscope, scope);

                scope_entry = JsonObjectCreate(2);
                JsonObjectAppendObject(payload, lscope, scope_entry);

                scope_data = JsonArrayCreate(50);
                JsonObjectAppendArray(scope_entry, "data", scope_data);

                JsonElement *header = JsonObjectCreate(5);
                JsonObjectAppendInteger(header, "Host", 0);
                JsonObjectAppendInteger(header, "Type", 1);
                JsonObjectAppendInteger(header, "Name", 2);
                JsonObjectAppendInteger(header, "Value", 3);
                JsonObjectAppendInteger(header, "Last seen", 4);

                JsonObjectAppendObject(scope_entry, "header", header);
            }

            assert(scope_data);
            if (!scope_data)
            {
                // don't crash in the unlikely event of an empty-string scope
                continue;
            }

            char rvalBuf[CF_MAXVARSIZE];
            if (strlen(hv->dtype) > 1) // list
            {
                RlistPrint(rvalBuf, sizeof(rvalBuf), hv->rval.item);
            }
            else
            {
                snprintf(rvalBuf, sizeof(rvalBuf), "%s", (char *) hv->rval.item);
            }

            JsonElement *data_entry = JsonArrayCreate(5);

            JsonArrayAppendString(data_entry, NULLStringToEmpty(hv->hh->hostname));
            JsonArrayAppendString(data_entry, DataTypeShortToType(hv->dtype));
            JsonArrayAppendString(data_entry, NULLStringToEmpty(hv->lval));
            JsonArrayAppendString(data_entry, NULLStringToEmpty(rvalBuf));
            JsonArrayAppendInteger(data_entry, hv->t);

            JsonArrayAppendArray(scope_data, data_entry);
        }

        if (scope_entry)
        {
            assert(scope_data);
            JsonObjectAppendInteger(scope_entry, "count", JsonElementLength(scope_data));
            scope_entry = NULL;
            scope_data = NULL;
        }
    }


    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_setuid_report(char *hostkey, char *file, bool regex,
                                    HostClassFilter *hostClassFilter, PageInfo *page,
                                    PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;

    HubQuery *hq = CFDB_QuerySetuid(&dbconn, hostkey, file, hostClassFilter,
                                    promise_context, NULL, db_options);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubSetUid);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(9);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "File", 1);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            HubSetUid *hS = (HubSetUid *) rp->item;

            JsonElement *entry = JsonArrayCreate(2);

            JsonArrayAppendString(entry, NULLStringToEmpty(hS->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hS->path));

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubSetUid);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/
int Nova_AddReportHeader(char *header, int truncated, char *buffer, int bufsize)
{
    buffer[0] = '\0';
    if (truncated)
    {
        snprintf(buffer, bufsize, ",%s,\"truncated\":\"%s\"", header, CF_NOTICE_TRUNCATED);
    }
    else
    {
        snprintf(buffer, bufsize, ",%s", header);
    }
    return true;
}

/*****************************************************************************/
/* Search for hosts with property X,Y,Z                                      */
/*****************************************************************************/

static JsonElement *GenerateHostOnlyReport( Rlist **records_p, PageInfo *page,
                                            WebReportFileInfo *wr_info )
{
    if( wr_info )
    {
        return WebExportHostOnlyReport( *records_p, wr_info );
    }
    else
    {
        return CreateJsonHostOnlyReport( records_p, page );
    }
}

/*****************************************************************************/

static JsonElement *CreateJsonHostOnlyReport(Rlist **records_p, PageInfo *page)
{
    assert(records_p);
    assert(page);

    Rlist *rp;
    HubHost *hh;

    PageRecords(records_p, page, DeleteHubHost);

    JsonElement *json_obj_out = JsonObjectCreate(2);

    JsonElement *json_arr_data = JsonArrayCreate(10);
    for (rp = *records_p; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        JsonElement *json_obj_tmp = JsonObjectCreate(3);
        JsonObjectAppendString(json_obj_tmp, "hostkey", hh->keyhash);
        JsonObjectAppendString(json_obj_tmp, "hostname", hh->hostname);
        JsonObjectAppendString(json_obj_tmp, "ip", hh->ipaddr);

        JsonArrayAppendObject(json_arr_data, json_obj_tmp);
    }
    JsonObjectAppendArray(json_obj_out, "data", json_arr_data);

    JsonElement *json_obj_meta = JsonObjectCreate(2);
    JsonObjectAppendInteger(json_obj_meta, "count", page->totalResultCount);
    JsonObjectAppendInteger(json_obj_meta, "related", page->totalResultCount);

    JsonObjectAppendObject(json_obj_out, "meta", json_obj_meta);

    return json_obj_out;
}
/****************************************************************************/

int Nova2PHP_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize)
{
    char buffer1[CF_BUFSIZE], buffer2[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int count1 = 0, count2 = 0, tmpsize1, tmpsize2;
    EnterpriseDB dbconn;

/* BEGIN query document */
    bson query;

    if (hostkey && strlen(hostkey) != 0)
    {
        bson_init(&query);
        bson_append_string(&query, cfr_keyhash, hostkey);
        BsonFinish(&query);
    }
    else
    {
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryHosts(&dbconn, MONGO_DATABASE, &query);
    bson_destroy(&query);

    hostnameOut[0] = '\0';
    ipaddrOut[0] = '\0';

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        snprintf(buffer1, CF_MAXVARSIZE - 1, "%s ", hh->hostname);
        snprintf(buffer2, CF_MAXVARSIZE - 1, "%s ", hh->ipaddr);

        tmpsize1 = strlen(buffer1);
        tmpsize2 = strlen(buffer2);

        if (count1 + tmpsize1 <= bufsize - 1)
        {
            strcat(hostnameOut, buffer1);
            count1 += tmpsize1;
        }

        if (count2 + tmpsize2 <= bufsize - 1)
        {
            strcat(ipaddrOut, buffer2);
            count2 += tmpsize2;
        }
    }

    DeleteHubQuery(hq, free);

    ReplaceTrailingChar(hostnameOut, ' ', '\0');
    ReplaceTrailingChar(ipaddrOut, ' ', '\0');

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

JsonElement *Nova2PHP_value_hosts(char *hostkey, char *day, char *month, char *year,
                                  HostClassFilter *hostClassFilter, PageInfo *page,
                                  PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year,
                                         hostClassFilter, promise_context, NULL,
                                         QUERY_FLAG_HOSTONLY);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubValue);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_setuid_hosts(char *hostkey, char *file, bool regex,
                                   HostClassFilter *hostClassFilter, PageInfo *page,
                                   PromiseContextMode promise_context,
                                   WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;
    db_options |= QUERY_FLAG_HOSTONLY;

    HubQuery *hq = CFDB_QuerySetuid(&dbconn, hostkey, file, hostClassFilter,
                                    promise_context, NULL, db_options);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubSetUid);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promiselog_hosts(char *hostkey, char *handle, char *causeRx,
                                       PromiseLogState state, time_t from, time_t to,
                                       HostClassFilter *hostClassFilter, PageInfo *page,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true,
                                        causeRx, from, to, false, hostClassFilter,
                                        NULL, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts ), page, wr_info );

    DeleteHubQuery(hq, DeleteHubPromiseLog);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_bundle_classes_used(const PromiseFilter *promiseFilter)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    Rlist *classList = CFDB_QueryBundleClasses(&dbconn, promiseFilter);

    Item *list = NULL;
    if (classList)
    {
        for (const Rlist *rp = classList; rp != NULL; rp = rp->next)
        {
            PrependItem(&list, rp->item, NULL);
        }
        RlistDestroy(classList);
        IdempPrependItem(&list, "any", NULL);
    }

    JsonElement *payload = JsonArrayCreate(ItemListSize(list));
    if (list)
    {
        list = SortItemListNames(list);

        for (const Item *ip = list; ip != NULL; ip = ip->next)
        {
            JsonElement *entry = JsonArrayCreate(2);
            JsonArrayAppendInteger(entry, 0); /* Deprecated pid to topicmap */
            JsonArrayAppendString(entry, NULLStringToEmpty(ip->name));

            JsonArrayAppendArray(payload, entry);
        }

        DeleteItemList(list);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

int Nova2PHP_bundle_list_by_bundle_usage(PromiseFilter *promiseFilter, char *bNameReferenced, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    char work[CF_MAXVARSIZE];
    Item *matched, *ip;

    if (!CFDB_Open(&dbconn))
    {
        return -1;
    }

    matched = CFDB_QueryBundlesUsing(&dbconn, promiseFilter, bNameReferenced);

    matched = SortItemListClasses(matched);

    if (matched)
    {
        snprintf(buffer, bufsize, "[");

        for (ip = matched; ip != NULL; ip = ip->next)
        {
            snprintf(work, CF_MAXVARSIZE, "{\"bundletype\":\"%s\",\"bundlename\":\"%s\"},", ip->classes, ip->name);

            if (!Join(buffer, work, bufsize))
            {
                break;
            }
        }

        ReplaceTrailingChar(buffer, ',', '\0');
        EndJoin(buffer, "]", bufsize);
        DeleteItemList(matched);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/
/* Hosts stats                                                               */
/*****************************************************************************/

void Nova2PHP_host_compliance_list_all(EnterpriseDB *conn, HostClassFilter *host_class_filter,
                                       PageInfo *page, char *buffer, int bufsize,
                                       PromiseContextMode promise_context)
{
    Item *clist = NULL;
    char work[CF_BUFSIZE] = { 0 };
    int startIndex = 0;
    int endIndex = 0;
    int count = -1;;

    HostColourFilter *host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                                               HOST_COLOUR_GREEN_YELLOW_RED,
                                                               promise_context);

    clist = CFDB_GetHostByColour(conn, host_class_filter, host_colour_filter);
    free(host_colour_filter);

    /* sort by score */
    clist = SortItemListCounters(clist);

    buffer[0] = '\0';
    strcat(buffer, "{\"data\":[");

    startIndex = page->resultsPerPage * (page->pageNum - 1);
    endIndex = (page->resultsPerPage * page->pageNum) - 1;

    for (Item *ip = clist; (ip != NULL); ip = ip->next)
    {
        count++;

        if (count >= startIndex && (count <= endIndex || endIndex <= 0))
        {
            work[0] = '\0';

            if (HostColourFromScoreForConnectedHost(ip->counter) == HOST_COLOUR_GREEN)
            {
                snprintf(work, sizeof(work), "{ \"colour\": \"green\", \"key\": \"%s\", \"id\": \"%s\"},", ip->name,
                         ip->classes);
            }
            else if (HostColourFromScoreForConnectedHost(ip->counter) == HOST_COLOUR_YELLOW)
            {
                snprintf(work, sizeof(work), "{ \"colour\": \"yellow\", \"key\": \"%s\", \"id\": \"%s\"},", ip->name,
                         ip->classes);
            }
            else if (HostColourFromScoreForConnectedHost(ip->counter) == HOST_COLOUR_RED)
            {
                snprintf(work, sizeof(work), "{ \"colour\": \"red\", \"key\": \"%s\", \"id\": \"%s\"},", ip->name,
                         ip->classes);
            }

            if (*work)
            {
                if (!Join(buffer, work, bufsize))
                {
                    break;
                }
            }
        }
    }
    ReplaceTrailingChar(buffer, ',', '\0');

    snprintf(work, sizeof(work), "],\"meta\":{\"count\":%d, \"related\": %d}}", count+1, count + 1);
    EndJoin(buffer, work, bufsize);

    DeleteItemList(clist);
}

/*****************************************************************************/

int Nova2PHP_show_hosts(char *hostNameRegex, char *ipRegex, HostClassFilter *hostClassFilter, PageInfo *page, char *buf,
                        int bufsize)
{
    HubQuery *hq;
    HubHost *hh;
    Rlist *rp;
    EnterpriseDB dbconn;
    char work[CF_MAXVARSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryHostsByAddress(&dbconn, hostNameRegex, ipRegex, hostClassFilter);

    CFDB_Close(&dbconn);

    if (!(hq && hq->hosts))
    {
        StartJoin(buf, "{}", bufsize);
        return true;
    }

    PageRecords(&(hq->hosts), page, DeleteHubHost);

    snprintf(work, sizeof(work),
             "{\"meta\":{\"count\" : %d,"
             "\"header\": {\"Key Hash\":0,\"Host name\":1,\"IP address\":2" "}},\n\"data\":[", page->totalResultCount);

    StartJoin(buf, work, bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        snprintf(work, sizeof(work), "[\"%s\", \"%s\", \"%s\"]\n,", hh->keyhash, hh->hostname, hh->ipaddr);

        if (!Join(buf, work, bufsize))
        {
            break;
        }
    }

    DeleteHubQuery(hq, free);

    ReplaceTrailingChar(buf, ',', '\0');

    EndJoin(buf, "]}", bufsize);

    return true;

}

/*****************************************************************************/

void Nova2PHP_host_compliance_list(EnterpriseDB *conn, char *colour, HostClassFilter *host_class_filter,
                                   PageInfo *page, char *buffer, int bufsize, PromiseContextMode promise_context)
{
    Item *ip, *clist;
    char work[CF_MAXVARSIZE], lastseen[CF_MAXVARSIZE] = { 0 };
    int counter = 0, startIndex, endIndex;

    HostColour host_colour;

    if (strcmp(colour, "green") == 0)
    {
        host_colour = HOST_COLOUR_GREEN;
    }
    else if (strcmp(colour, "yellow") == 0)
    {
        host_colour = HOST_COLOUR_YELLOW;
    }
    else if (strcmp(colour, "red") == 0)
    {
        host_colour = HOST_COLOUR_RED;
    }
    else if (strcmp(colour, "blue") == 0)
    {
        host_colour = HOST_COLOUR_BLUE;
    }
    else if (strcmp(colour, "black") == 0)
    {
        host_colour = HOST_COLOUR_BLACK;
    }
    else
    {
        return;
    }

    HostColourFilter *host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                                               host_colour,
                                                               promise_context);

    clist = CFDB_GetHostByColour(conn, host_class_filter, host_colour_filter);
    free(host_colour_filter);

    buffer[0] = '\0';
    strcat(buffer, "{ \"data\":[");

    if (clist)
    {
        clist = SortItemListNames(clist);

        startIndex = page->resultsPerPage * (page->pageNum - 1);
        endIndex = (page->resultsPerPage * page->pageNum) - 1;

        for (ip = clist, counter = 0; ip != NULL; ip = ip->next, counter++)
        {
            if (counter >= startIndex && (counter <= endIndex || endIndex < 0))
            {
                if (host_colour == HOST_COLOUR_BLUE)
                {
                    Nova2PHP_getlastupdate(ip->name, lastseen, sizeof(lastseen));
                    snprintf(work, CF_MAXVARSIZE, "{ \"key\": \"%s\", \"id\": \"%s\",\"lastseen\": \"%s\"},", ip->name,
                             ip->classes, lastseen);
                }
                else if (host_colour == HOST_COLOUR_BLACK)
                {
                    long last_exec_time = 0;
                    last_exec_time = CFDB_GetLastAgentExecution(conn, ip->name);
                    snprintf(work, CF_MAXVARSIZE, "{ \"key\": \"%s\", \"id\": \"%s\",\"lastexec\": \"%ld\"},", ip->name,
                             ip->classes, last_exec_time);
                }
                else
                {
                    snprintf(work, CF_MAXVARSIZE, "{ \"key\": \"%s\", \"id\": \"%s\"},", ip->name, ip->classes);
                }

                if (!Join(buffer, work, bufsize))
                {
                    break;
                }
            }
        }
    }
    ReplaceTrailingChar(buffer, ',', '\0');

    snprintf(work, sizeof(work), "],\"meta\":{\"count\":%d}}", counter);
    Join(buffer, work, bufsize);

    DeleteItemList(clist);
}

/*****************************************************************************/

void Nova2PHP_get_host_colour(char *hostkey, char *buffer, int bufsize,
                              PromiseContextMode promise_context)
{
    HostColour colour = HOST_COLOUR_BLUE;
    CFDB_GetHostColour(hostkey, HOST_RANK_METHOD_COMPLIANCE, &colour,
                       promise_context);

    strncpy(buffer, Nova_HostColourToString(colour), bufsize);
}

/*****************************************************************************/

char *Nova2PHP_GetPromiseComment(char *handle)
{
    static char buffer[CF_BUFSIZE];
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (CFDB_QueryPromiseAttr(&dbconn, handle, cfp_comment, buffer, CF_BUFSIZE))
    {
        return buffer;
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return "No comment";
}

/*****************************************************************************/

JsonElement *Nova2PHP_GetPromiseBody(char *name, char *type)
{
    EnterpriseDB dbconn;
    HubBody *hb;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    hb = CFDB_QueryBody(&dbconn, type, name);

    if (!hb)
    {
        CFDB_Close(&dbconn);
        return NULL;
    }

    JsonElement *payload = JsonObjectCreate(3);
    {
        JsonObjectAppendString(payload, "Type", NULLStringToEmpty(hb->bodyType));
        JsonObjectAppendString(payload, "Name", NULLStringToEmpty(hb->bodyName));

        if (hb->bodyArgs)
        {
            JsonObjectAppendString(payload, "Arguments", NULLStringToEmpty(hb->bodyArgs));
        }

        if (hb->attr)
        {
            HubBodyAttr *ha;
            JsonElement *attributes = JsonArrayCreate(5);

            for (ha = hb->attr; ha != NULL; ha = ha->next)
            {
                JsonElement *entry = JsonObjectCreate(3);
                JsonObjectAppendString(entry, "lval", NULLStringToEmpty(ha->lval));
                JsonObjectAppendString(entry, "rval", NULLStringToEmpty(ha->rval));
                JsonObjectAppendString(entry, "class_context", NULLStringToEmpty(ha->classContext));

                JsonArrayAppendArray(attributes, entry);
            }

            JsonObjectAppendArray(payload, "attributes", attributes);
        }

        DeleteHubBody(hb);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

int Nova2PHP_body_list(char *name, char *type, char *returnval, int bufsize)
{
    EnterpriseDB dbconn;
    char work[CF_MAXVARSIZE];
    Item *all_bodies, *ip;

    if (!CFDB_Open(&dbconn))
    {
        return -1;
    }

    all_bodies = CFDB_QueryAllBodies(&dbconn, type, name);
    CFDB_Close(&dbconn);

    all_bodies = SortItemListNames(all_bodies);

    if (all_bodies)
    {
        snprintf(returnval, CF_MAXVARSIZE - 1, "[");
        for (ip = all_bodies; ip != NULL; ip = ip->next)
        {
            snprintf(work, CF_MAXVARSIZE - 1, "{\"body\":\"%s\",\"type\":\"%s\"},", ip->name, ip->classes);
            Join(returnval, work, bufsize);
        }

        ReplaceTrailingChar(returnval, ',', '\0');
        EndJoin(returnval, "]", bufsize);
    }

    DeleteItemList(all_bodies);

    return true;
}

/*****************************************************************************/
/* Reports                                                                   */
/*****************************************************************************/

void Nova2PHP_select_reports(char *reportId, char *buffer, int bufsize)
{
    char novaListJson[CF_BUFSIZE] = { 0 };
    char errbuf[CF_MAXVARSIZE] = { 0 };
    cfapi_errid errid;

    novaListJson[0] = '\0';

    errid = FormatReportInfoAsJson(reportId, BASIC_REPORTS, novaListJson, sizeof(novaListJson));
    snprintf(buffer, bufsize, "{\"meta\":{},\"data\":[%s],%s}", novaListJson,
             FormatErrorJsonAttribute(errbuf, sizeof(errbuf), errid));
}

/*****************************************************************************/

static cfapi_errid FormatReportInfoAsJson(char *reportId, ReportInfo *reports, char *buf, int bufsize)
{
    char work[CF_MAXVARSIZE] = { 0 };
    int i;
    cfapi_errid errid = ERRID_ITEM_NONEXISTING;

    for (i = 0; reports[i].id != NULL; i++)
    {
        if (NULL_OR_EMPTY(reportId) || strcmp(reportId, reports[i].id) == 0)
        {
            snprintf(work, sizeof(work),
                     "{\"id\":\"%s\",\"category\":\"%s\",\"name\":\"%s\",\"old-name\":\"%s\",\"description\":\"%s\"},",
                     reports[i].id, reports[i].category, reports[i].name, reports[i].name_old, reports[i].description);
            Join(buf, work, bufsize);
            errid = ERRID_SUCCESS;
        }
    }

    ReplaceTrailingChar(buf, ',', '\0');

    return errid;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promise_details(PromiseFilter *filter)
{
    EnterpriseDB dbconn;
    char promiseeText[CF_MAXVARSIZE];
    char commentText[CF_MAXVARSIZE];


    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hqPromise = CFDB_QueryPromisesUnexpanded(&dbconn, filter,
                                                       PROMISE_CONTEXT_MODE_ALL);

    if (CountRecords(hqPromise) == 0)
    {
        DeleteHubQuery(hqPromise, DeleteHubPromise);
        CFDB_Close(&dbconn);
        return NULL;
    }

    JsonElement *payload = JsonObjectCreate(10);
    {

    HubPromise *hp = HubQueryGetFirstRecord(hqPromise);

    JsonObjectAppendString(payload, "bundletype", NULLStringToEmpty(hp->bundleType));
    JsonObjectAppendString(payload, "bundlename", NULLStringToEmpty(hp->bundleName));
    JsonObjectAppendString(payload, "handle", NULLStringToEmpty(hp->handle));
    JsonObjectAppendString(payload, "promiser", NULLStringToEmpty(hp->promiser));






    if (RlistLen(hp->promisees) == 0)
    {
        strcpy(promiseeText, "None mentioned");
    }
    else
    {
        RlistPrint(promiseeText, sizeof(promiseeText), hp->promisees);
    }

    JsonObjectAppendString(payload, "promisee", NULLStringToEmpty(promiseeText));


    if (NULL_OR_EMPTY(hp->comment))
    {
        strcpy(commentText, "No comment");
    }
    else
    {
        snprintf(commentText, sizeof(commentText), "%s", hp->comment);
    }

    JsonObjectAppendString(payload, "comment", NULLStringToEmpty(commentText));
    JsonObjectAppendString(payload, "promise_type", NULLStringToEmpty(hp->promiseType));
    JsonObjectAppendString(payload, "class_context", NULLStringToEmpty(hp->classContext));
    JsonObjectAppendString(payload, "file", NULLStringToEmpty(hp->file));
    JsonObjectAppendInteger(payload, "line_num", hp->lineNo);

    if (hp->constraints)
    {


        JsonElement *body = JsonArrayCreate(5);

        for (Rlist *rp = hp->constraints; rp != NULL; rp = rp->next)
        {
            char lval[CF_MAXVARSIZE], rval[CF_MAXVARSIZE], args[CF_MAXVARSIZE];

            args[0] = '\0';
            sscanf(RlistScalarValue(rp), "%255s => %1023[^(,;]%[^\n]", lval, rval, args);
            JsonElement *entry = JsonObjectCreate(4);
            if (strcmp(lval, "usebundle") == 0)
            {

                JsonObjectAppendString(entry, "constraint_type", "bundle");
            }
            else
            {

                 JsonObjectAppendString(entry, "constraint_type", "body");
            }

            JsonObjectAppendString(entry, "type", NULLStringToEmpty(lval));
            JsonObjectAppendString(entry, "name", NULLStringToEmpty(rval));
            JsonObjectAppendString(entry, "args", NULLStringToEmpty(args));
            JsonArrayAppendArray(body, entry);

        }

        JsonObjectAppendArray(payload, "body", body);
    }



    DeleteHubQuery(hqPromise, DeleteHubPromise);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/
int Nova2PHP_list_promise_handles_with_comments(char *bundle, char *btype, char *returnval, int bufsize)
{
    EnterpriseDB dbconn;
    char work[CF_BUFSIZE];
    Rlist *rp;
    HubQuery *hq;
    HubPromise *hp;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryHandlesForBundlesWithComments(&dbconn, btype, bundle);

    CFDB_Close(&dbconn);

    returnval[0] = '\0';

    if (hq)
    {
        StartJoin(returnval, "[", bufsize);

        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            hp = (HubPromise *) rp->item;
            snprintf(work, CF_BUFSIZE - 1, "[\"%s\", \"%s\"],", (char *) hp->handle, (char *) hp->comment);
            Join(returnval, work, bufsize);
        }

        ReplaceTrailingChar(returnval, ',', '\0');
        EndJoin(returnval, "]", bufsize);

        DeleteHubQuery(hq, DeleteHubPromise);

        return true;
    }
    else                        // no result
    {
        return false;
    }

}

/*****************************************************************************/

JsonElement *JSONErrorFromId(cfapi_errid errid)
{
    if (errid >= ERRID_MAX)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! JSONErrorFromId: errid out of bounds");
        errid = ERRID_MAX;
    }

    JsonElement *jsonError = JsonObjectCreate(2);
    JsonObjectAppendInteger(jsonError, "errid", errid);
    JsonObjectAppendString(jsonError, "msg", ERRID_DESCRIPTION[errid]);

    return jsonError;
}
/*****************************************************************************/

JsonElement *Nova2PHP_network_speed(char *hostkey)
{
    EnterpriseDB dbconn;
    int found = false;

    double net_expected = 0;
    double net_var = 0;

    if (!CFDB_Open(&dbconn))
    {        
        return JSONErrorFromId(ERRID_DBCONNECT);
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    bson field;

    bson_init(&field);
    bson_append_int(&field, cfr_netmeasure, 1);
    BsonFinish(&field);

    mongo_cursor *cursor = MongoFind(&dbconn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&field);

    cfapi_errid errid = ERRID_SUCCESS;

    if (MongoCursorNext(cursor))      // not more than one record
    {
        bson_iterator it1;

        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (StringSafeCompare(bson_iterator_key(&it1), cfr_netmeasure) == 0)
            {
                bson_iterator it2;
                bson_iterator_subiterator(&it1, &it2);

                found = true;
                while (bson_iterator_next(&it2))
                {
                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_expect) == 0)
                    {
                        net_expected = (double)bson_iterator_double(&it2);
                    }

                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_var) == 0)
                    {
                        net_var = (double)bson_iterator_double((&it2));
                    }
                }

            }
            else
            {
                CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Existing network measurement incorrect - ignoring");
            }
        }
    }
    else
    {
        errid = ERRID_HOST_NOT_FOUND;
    }

    mongo_cursor_destroy(cursor);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    JsonElement *jsonNetworkSpeed = JsonObjectCreate(2);

    if (found)
    {
        JsonElement *jsonNetworkData = JsonObjectCreate(2);
        JsonObjectAppendReal(jsonNetworkData, "speed", net_expected);
        JsonObjectAppendReal(jsonNetworkData, "delta", sqrt(net_var));
        JsonObjectAppendObject(jsonNetworkSpeed, "data", jsonNetworkData);

        errid = ERRID_SUCCESS;
    }
    else if(errid != ERRID_HOST_NOT_FOUND)
    {
        errid = ERRID_DATA_UNAVAILABLE;
    }


    JsonObjectAppendObject(jsonNetworkSpeed, "error", JSONErrorFromId(errid));

    return jsonNetworkSpeed;
}

/*****************************************************************************/

int Nova2PHP_countclasses(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter,
                          char *returnval, int bufsize, PromiseContextMode promise_context)
{
    HubQuery *hq;
    Rlist *rp;
    Item *order_results = NULL, *ip;
    int count = 0;
    EnterpriseDB dbconn;
    unsigned long bluehost_threshold;

    CFDB_GetBlueHostThreshold(&bluehost_threshold);

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    int db_options = regex ? QUERY_FLAG_IS_REGEX : QUERY_FLAG_DISABLE_ALL;

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, hostkey, name, now - (time_t)bluehost_threshold,
                           now, hostClassFilter, promise_context, NULL, db_options);

    returnval[0] = '\0';

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        IdempItemCount(&order_results, hc->class, NULL);
    }

    snprintf(returnval, bufsize, "[");

/* Make a histogram of job vs number of hosts */

    order_results = SortItemListNames(order_results);

    for (ip = order_results; ip != NULL; ip = ip->next)
    {
        char work[CF_BUFSIZE];

        snprintf(work, CF_BUFSIZE, "{\"name\":\"%s\",\"count\":%d},", ip->name + strlen(MONITOR_CLASS_PREFIX),
                 ip->counter + 1);
        Join(returnval, work, bufsize);
        count += ip->counter + 1;
    }

    DeleteHubQuery(hq, DeleteHubClass);
    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    if (count == 0)
    {
        snprintf(returnval, bufsize, "[{\"count\":0}]");
    }

    return count;
}

/*****************************************************************************/

char *Nova_LongState(char s)
{
    switch (s)
    {
    case 'c':
        return "Compliant";
    case 'r':
        return "Repaired";
    case 'n':
    default:
        return "Not Compliant";
    }
}

/*****************************************************************************/

int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz)
{

    snprintf(outStr, outStrSz, "%s", cf_ctime(&then));

    if (now > then + threshold)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/

void Nova2PHP_ComplianceSummaryGraph(char *policy, char *buffer, int bufsize)
{
    ComplianceSummaryGraph(NULL, policy, buffer, bufsize);
}

/*****************************************************************************/

JsonElement *Nova_FormatDiff(const char *diffStr)
{
    JsonElement *returnval = JsonArrayCreate(16);

    char tline[CF_BUFSIZE] = { 0 };
    for (const char *sp = diffStr; *sp != '\0'; sp += strlen(tline) + 1)
    {
        int line = 0;
        char pm[2] = { 0 };

        char diff[CF_BUFSIZE] = { 0 };
        sscanf(sp, "%c,%d,%2047[^\n]", pm, &line, diff);
        sscanf(sp, "%2047[^\n]", tline);

        JsonElement *entry = JsonArrayCreate(3);
        JsonArrayAppendString(entry, pm);
        JsonArrayAppendInteger(entry, line);
        JsonArrayAppendString(entry, diff);

        JsonArrayAppendArray(returnval, entry);
    }

    return returnval;
}

/*****************************************************************************/

int Nova2PHP_delete_host(char *keyHash)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }
    CFDB_MarkAsDeleted(&dbconn, keyHash);
    CFDB_PurgeHost(&dbconn, keyHash);
    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2PHP_replica_status(char *buffer, int bufsize)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    CFDB_QueryReplStatus(&dbconn, buffer, bufsize);

    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/
/* Multiple policy environments                                              */
/*****************************************************************************/

bool Nova2PHP_environment_list(EnvironmentsList **out, HostClassFilter *hostClassFilter)
{
    EnterpriseDB dbconn;

    *out = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    bson query;

    bson_init(&query);
    BsonAppendHostClassFilter(&query, hostClassFilter);
    BsonFinish(&query);

    Item *item_envs = CFDB_QueryDistinct(&dbconn, MONGO_BASE, MONGO_HOSTS_COLLECTION, cfr_environment, &query);

    bson_destroy(&query);

    CFDB_Close(&dbconn);

    for (Item *ip = item_envs; ip != NULL; ip = ip->next)
    {
        EnvironmentsList *node = xmalloc(sizeof(EnvironmentsList));

        node->next = *out;
        node->name = xstrdup(ip->name);
        *out = node;
    }

    DeleteItemList(item_envs);

    return true;
}

/*****************************************************************************/

bool Nova2PHP_host_list_by_environment(HostsList **out, const char *environment, HostClassFilter *hostClassFilter)
{
    EnterpriseDB dbconn;

    *out = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

/* { env: $environment } */
    bson query;

    bson_init(&query);
    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_append_string(&query, cfr_environment, environment);
    BsonFinish(&query);

/* { kH: 1 } */
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    BsonFinish(&fields);

    mongo_cursor *cursor = MongoFind(&dbconn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    CFDB_Close(&dbconn);

    while (MongoCursorNext(cursor))
    {
        bson_iterator i;

        if (!bson_find(&i, &cursor->current, cfr_keyhash))
        {
            CfOut(OUTPUT_LEVEL_VERBOSE, "", "Malformed query result in Nova2PHP_environment_contents");
            mongo_cursor_destroy(cursor);
            return false;
        }

        HostsList *node = xmalloc(sizeof(HostsList));

        node->next = *out;
        node->keyhash = xstrdup(bson_iterator_string(&i));
        *out = node;
    }

    mongo_cursor_destroy(cursor);
    return true;
}

/*****************************************************************************/

char *Nova2PHP_environment_by_hostkey(const char *hostkey)
{
    EnterpriseDB dbconn;
    bson_iterator i;
    char *environment = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

/* { kH: $hostkey } */
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

/* { env: 1 } */
    bson fields;

    bson_init(&fields);
    bson_append_int(&fields, cfr_environment, 1);
    BsonFinish(&fields);

    bson result = { 0 };

    if( MongoFindOne( &dbconn, MONGO_DATABASE, &query, &fields, &result ) == MONGO_OK )
    {
        if (bson_find(&i, &result, cfr_environment))
        {
            environment = xstrdup(bson_iterator_string(&i));
        }
        bson_destroy(&result);
    }

    bson_destroy(&query);
    bson_destroy(&fields);

    CFDB_Close(&dbconn);
    return environment;
}

/*****************************************************************************/

void FreeEnvironmentsList(EnvironmentsList *list)
{
    while (list)
    {
        EnvironmentsList *next = list->next;

        free(list->name);
        free(list);
        list = next;
    }
}

/*****************************************************************************/
/* for commenting functionality */
/*****************************************************************************/

int Nova2PHP_add_note(char *noteid, char *keyhash, char *username, time_t datetime, char *note, char *returnval, int bufsize)
{
    int is_new = false;
    if (!noteid || strlen(noteid) == 0)
    {
        if (!keyhash || strlen(keyhash) == 0 )
        {
            CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Hostkey and noteid is not given. Nothing to look for");
            return false;
        }
        is_new = true;
    }

    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    char nid[CF_MAXVARSIZE] = { 0 };

    int ret;
    if (is_new) // create a new note with entry
    {
        char report_data[CF_SMALLBUF] = { 0 };
        snprintf(report_data, CF_SMALLBUF, "%d", CFREPORT_HOSTS);
        ret = CFDB_AddNote(&dbconn, keyhash, CFREPORT_HOSTS, nid, report_data,
                           username, datetime, note);
    }
    else // add entry to existing note
    {
        snprintf(nid, CF_MAXVARSIZE, "%s", noteid);
        ret = CFDB_AddNote(&dbconn, NULL, 0, nid, NULL, username, datetime, note);
    }

    if (ret)
    {
        snprintf(returnval, bufsize, "%s", nid);
    }
    else
    {
        snprintf(returnval, bufsize, "%d", ret);
    }

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_get_notes(char *keyhash, char *nid, char *username, char *filter_username,
                       time_t from, time_t to, PageInfo *page, char *returnval, int bufsize)
{
    Item *data = NULL;
    char msg[CF_BUFSIZE] = { 0 };
    char buffer[CF_BUFSIZE] = { 0 };
    EnterpriseDB dbconn;
    HubNoteInfo *hni;
    HubNote *hn;
    Rlist *result, *rp;

    char fuser[CF_MAXVARSIZE] = { 0 };
    char kh[CF_MAXVARSIZE] = { 0 };
    char noteId[CF_MAXVARSIZE] = { 0 };

    char jsonEscapedReport[CF_BUFSIZE] = { 0 };
    char jsonEscapedMsg[CF_BUFSIZE] = { 0 };
    int startIndex = 0, endIndex = 0, count = 0;

    if (filter_username)
    {
        snprintf(fuser, CF_MAXVARSIZE, "%s", filter_username);
    }

    if (keyhash)
    {
        snprintf(kh, CF_MAXVARSIZE, "%s", keyhash);
    }

    if (nid)
    {
        snprintf(noteId, CF_MAXVARSIZE, "%s", nid);
    }

    snprintf(msg, CF_BUFSIZE, "%s,%ld,%ld\n", fuser, from, to);

    AppendItem(&data, msg, NULL);

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    result = CFDB_QueryNotes(&dbconn, kh, noteId, data);

    returnval[0] = '\0';
    StartJoin(returnval, "{\"data\":[", bufsize);
    startIndex = page->resultsPerPage * (page->pageNum - 1);
    endIndex = (page->resultsPerPage * page->pageNum) - 1;
    for (rp = result; rp != NULL; rp = rp->next)
    {
        hni = (HubNoteInfo *) rp->item;

        EscapeJson(hni->report, jsonEscapedReport, sizeof(jsonEscapedReport));

        if (CFDB_HasHostAccessFromUserRBAC(username, hni->hh->keyhash) == ERRID_SUCCESS)
        {
            for (hn = hni->note; hn != NULL; hn = hn->next, count++)
            {
                if (count >= startIndex && (count <= endIndex || endIndex < 0))
                {
                    EscapeJson(hn->msg, jsonEscapedMsg, sizeof(jsonEscapedMsg));
                    ReplaceTrailingChar(jsonEscapedMsg, '\n', '\0');

                    snprintf(buffer, sizeof(buffer),
                             "{\"user\":\"%s\",\"date\":%ld,\"message\":\"%s\",\"report\":\"%s\",\"report_type\":%d, \"host\":{\"name\":\"%s\",\"ip\":\"%s\",\"kh\":\"%s\"}},",
                             hn->user, hn->t, jsonEscapedMsg, jsonEscapedReport, hni->reportType, hni->hh->hostname,
                             hni->hh->ipaddr, hni->hh->keyhash);
                    if (!Join(returnval, buffer, bufsize))
                    {
                        break;
                    }
                }
            }
        }
        hni = NULL;
    }

    ReplaceTrailingChar(returnval, ',', '\0');

    snprintf(buffer, sizeof(buffer), "],\"meta\":{\"count\":%d}}\n", count);
    EndJoin(returnval, buffer, bufsize);

    RlistDestroy(result);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_get_host_noteid(char *hostkey, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    Rlist *rp, *result;
    EnterpriseDB dbconn;

    if (NULL_OR_EMPTY(hostkey))
    {
        return false;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, cfn_keyhash, hostkey);
    bson_append_int(&query, cfn_reporttype, CFREPORT_HOSTS);
    BsonFinish(&query);

    if (!CFDB_Open(&dbconn))
    {
        bson_destroy(&query);
        return false;
    }

    result = CFDB_QueryNoteId(&dbconn, &query);

    bson_destroy(&query);
    returnval[0] = '\0';

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    if (!result)
    {
        return false;
    }

    for (rp = result; rp != NULL; rp = rp->next)
    {
        snprintf(buffer, CF_MAXVARSIZE, "%s", (char *) rp->item);
        snprintf(returnval, CF_MAXVARSIZE, "%s ", buffer);
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_promise_list(PromiseFilter *promiseFilter, char *returnval, int bufsize,
                          PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    char work[CF_MAXVARSIZE] = { 0 };
    char promiserJson[CF_MAXVARSIZE];
    Rlist *rp;
    HubPromise *hp;
    HubQuery *hq;

    /* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryPromisesUnexpanded(&dbconn, promiseFilter, promise_context);

    CFDB_Close(&dbconn);

    if (!hq)
    {
        // no results
        snprintf(returnval, sizeof(returnval),"{\"meta\":{\"count\" : 0},\"data\":[]}");
        return false;
    }

    PageRecords(&(hq->records), page, DeleteHubPromise);

    char header[CF_MAXVARSIZE] = { 0 };
    snprintf(header, sizeof(header), "\"meta\":{\"count\" : %d}", page->totalResultCount);

    int headerLen = strlen(header);
    int noticeLen = strlen(CF_NOTICE_TRUNCATED);
    bool truncated = false;

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromise *) rp->item;
        snprintf(work, sizeof(work), "[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"],",
                 (char *) hp->handle,
                 (char *) hp->promiseType,
                 (char *) hp->bundleName,
                 (char *) hp->bundleType, (char *) EscapeJson(hp->promiser, promiserJson, sizeof(promiserJson)));

        int margin = headerLen + noticeLen + strlen(work);

        if (!JoinMargin(returnval, work, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }


    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubPromise);

    return true;
}

/*****************************************************************************/
int Nova2PHP_GetHubKey(char *buffer, int bufsize)
{
    char name[CF_MAXVARSIZE] = { 0 }, policy_server[CF_MAXVARSIZE] = { 0 };
    FILE *fp;
    Item *list = NULL, *ip = NULL;
    int ret = false;

    if (strlen(CFWORKDIR) < 1)
    {
        snprintf(CFWORKDIR, CF_MAXVARSIZE, "%s", "/var/cfengine");
    }
    snprintf(name, sizeof(name), "%s%cpolicy_server.dat", CFWORKDIR, FILE_SEPARATOR);

    if ((fp = fopen(name, "r")) != NULL)
    {
        fscanf(fp, "%s", policy_server);
        fclose(fp);
    }

    list = CFDB_GetLastseenCache();

    for (ip = list; ip != NULL; ip = ip->next)
    {
        if (strcmp(ip->classes, policy_server) == 0)
        {
            snprintf(buffer, bufsize, "%s", ip->name);
            ret = true;
        }
    }
    DeleteItemList(list);
    return ret;
}

/*****************************************************************************/

int Nova2PHP_GetHubMaster(char *buffer, int bufsize)
{
    char master[CF_MAXVARSIZE] = { 0 };

    if (CFDB_QueryIsMaster())
    {
        snprintf(buffer, bufsize, "am_hub_master");
        return true;
    }
    else
    {
        char *sp;

        CFDB_QueryMasterIP(master, sizeof(master));

        if ((sp = strchr(master, ':')))
        {
            *sp = '\0';
        }

        snprintf(buffer, bufsize, "%s", master);
        return true;
    }
    snprintf(buffer, bufsize, "Unknown hub_master");
    return false;
}

/*****************************************************************************/

int Nova2PHP_get_bluehost_threshold(char *buffer, int buffsize)
{
    unsigned long threshold;

    CFDB_GetBlueHostThreshold(&threshold);

    snprintf(buffer, buffsize, "%lu", threshold);
    return true;
}

/*****************************************************************************/

const char *GetErrorDescription(cfapi_errid errid)
{
    return ERRID_DESCRIPTION[errid];
}

/*****************************************************************************/

char *FormatErrorJsonAttribute(char *out, int outSz, cfapi_errid errid)
{
    if (errid >= ERRID_MAX)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! FormatErrorJsonAttribute: errid out of bounds");
        errid = ERRID_MAX;
    }

    snprintf(out, outSz, "\"error\":{\"errid\":%d,\"msg\":\"%s\"}", errid, ERRID_DESCRIPTION[errid]);

    return out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_get_service_histogram()
{
    return Nova_GetServiceHistogram();
}

/*****************************************************************************/

JsonElement *Nova2PHP_get_service_level_histogram(char *srv)
{
    return Nova_GetServiceLevels(srv);
}
