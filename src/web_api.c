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
#include "web-report-export.h"

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
static void WriteDouble2Str_MP(double x, char *buffer, int bufsize);
JsonElement *JSONErrorFromId(cfapi_errid errid);

/*****************************************************************************/

#ifndef NDEBUG
static bool IsEnvMissionPortalTesting(void);
#endif

/*****************************************************************************/
/* Helper functions                                                          */
/*****************************************************************************/

static int MightBeTopic(const char *search);
static int MightBeHostId(const char *search);
static int MightBeClass(const char *search);
static int MightBeVariable(const char *search);
static int MightBeReportType(const char *search);
static int MightBeHostColour(const char *search);

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
        CfOut(cf_error, "fopen", "Couldn't open %s for reading", path);
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

/*****************************************************************************/

void Con2PHP_get_story_by_id(int id,char *buffer,int bufsize)

{ 
    buffer[0] = '\0';

    if (Nova_GenerateStories_by_id_JSON(id,cfi_cause,buffer,bufsize))
    {
    }
    else
    {
        snprintf(buffer,bufsize,"No stories about this topic\n");
    }
}

/*****************************************************************************/

void Con2PHP_get_story_by_name(char *typed_topic,char *buffer,int bufsize)

{ 
    buffer[0] = '\0';

    if (Nova_GenerateStories_by_name_JSON(typed_topic,cfi_cause,buffer,bufsize))
    {
    }
    else
    {
        snprintf(buffer,bufsize,"No stories about %s\n",typed_topic);
    }
}

/*****************************************************************************/

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
        CfOut(cf_verbose, "", " !! Could not open connection to report database");
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

/****************************************************************************/

JsonElement *Nova2PHP_get_knowledge_view(int pid, char *view)
{
    return Nova_PlotTopicCosmos(pid, view);
}

/****************************************************************************/

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

int Nova2PHP_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg,
                            HostClassFilter *hostClassFilter, char *returnval, int bufsize)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
    time_t now = time(NULL), from = now, to = now - SECONDS_PER_WEEK;
    int code_blue = 0, tot_hosts, code_black = 0;
    double n, r, k, n_av, k_av, r_av, tot_promises;
    char *current_host = "x";
    unsigned long bluehost_threshold;

    CFDB_GetBlueHostThreshold(&bluehost_threshold);

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL),
                                     false, hostClassFilter, PROMISE_CONTEXT_MODE_ALL, NULL);

    n = k = r = 0;
    n_av = k_av = r_av = 0;
    tot_promises = 1;
    tot_hosts = 0;

// For each promise record (from multiple matching hosts)

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseCompliance *) rp->item;

        // If data have passed the time horizon, we should not claim to know their state
        // The system might or might not be compliant, hp->e is the expected schedule for this promise

        if (hp->t < now - bluehost_threshold - hp->e)
        {
            if (current_host && strcmp(hp->hh->keyhash, current_host) != 0)     // New host
            {
                code_blue++;
                current_host = hp->hh->keyhash;
            }
            continue;
        }

        HostColour colour;
        CFDB_GetHostColour(hp->hh->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour, PROMISE_CONTEXT_MODE_ALL);
        if (colour == HOST_COLOUR_BLACK)
        {
            if (current_host && strcmp(hp->hh->keyhash, current_host) != 0)     // New host
            {
                code_black++;
                current_host = hp->hh->keyhash;
            }
            continue;
        }

        // How do we know how many promises are supposed to be kept on a given host?
        // compute the average number per host      
        if (current_host && strcmp(hp->hh->keyhash, current_host) != 0) // New host
        {
            n_av += n / tot_promises;   // Average not kept on available hosts in class
            r_av += r / tot_promises;   // Average repaired on available hosts in class
            k_av += k / tot_promises;   // Average compliant/kept on available hosts in class

            n = k = r = 0;
            tot_promises = 1;
            tot_hosts++;        // The number of hosts in the class that have fresh data
            current_host = hp->hh->keyhash;
        }
        else
        {
            tot_promises++;
        }

        // Get minimax boundary of last measured time region so we can compute uncertainty

        if (hp->t < from)
        {
            from = hp->t;
        }

        if (hp->t > to)
        {
            to = hp->t;
        }

        // Get status of this matching promise (n,r,k = 1 if unique handle)

        switch (hp->status)
        {
        case PROMISE_STATE_KEPT:
            k++;
            break;
        case PROMISE_STATE_REPAIRED:
            r++;
            break;
        case PROMISE_STATE_NOTKEPT:
        default:
            n++;
            break;
        }
    }

    n_av += n / tot_promises;   // Average not kept on available hosts in class
    r_av += r / tot_promises;   // Average repaired on available hosts in class
    k_av += k / tot_promises;   // Average compliant/kept on available hosts in class

    if (tot_hosts == 0)
    {
        from = now - bluehost_threshold;
        to = now;
    }
// Return current best-knowledge of average compliance for the class of hosts and promises selected

    snprintf(returnval, bufsize,
             "{\"kept\":%.2lf,\"not_kept\":%.2lf,\"repaired\":%.2lf,\"host_count\":%d,\"code_blue\":\"%d\",\"class\":\"%s\",\"start\":%ld,\"end\":%ld,\"code_black\":\"%d\"}",
             k_av, n_av, r_av, tot_hosts, code_blue, classreg, from, to, code_black);

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
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

    HubQuery *hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                                       regex, 0, time(NULL), false,
                                                       hostClassFilter, NULL, promise_context, NULL);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

JsonElement *Nova2PHP_bundle_compliance_summary (char *hostkey, char *bundle, bool regex,
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

    HubQuery *hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex,
                                                hostClassFilter, NULL, false,
                                                promise_context, NULL);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    bool ret = false;
    char work[CF_MAXVARSIZE];
    time_t lastUpdate = 0;
    char hostName[CF_MAXVARSIZE], ipAddress[CF_MAXVARSIZE];
    HubVital *res, *hv;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    res = CFDB_QueryVitalsMeta(&dbconn, keyHash, 0);

    strcpy(buffer, "{");

    Nova2PHP_hostinfo(keyHash, hostName, ipAddress, sizeof(hostName));

    int size = 0;
    CFDB_QueryLastUpdate(&dbconn, MONGO_DATABASE, cfr_keyhash, keyHash, &lastUpdate, &size);

    CFDB_Close(&dbconn);

    snprintf(work, sizeof(work), "\"hostname\" : \"%s\", \"ip\" : \"%s\", \"ls\" : %ld, \n\"obs\" : [",
             hostName, ipAddress, lastUpdate);

    Join(buffer, work, bufsize);

    for (hv = res; hv != NULL; hv = hv->next)
    {
        snprintf(work, sizeof(work), "{\"id\":\"%s\", \"units\":\"%s\", \"desc\":\"%s\"},",
                 hv->id, hv->units, hv->description);
        Join(buffer, work, bufsize);

        ret = true;
    }

    DeleteHubVital(res);

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]}", bufsize);

    return ret;
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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promiselog_summary(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page,
                                         PromiseContextMode promise_context)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_promiselog_summary_test(hostkey, handle, causeRx, state, from, to, hostClassFilter, page);
    }
# endif

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

    HubQuery *hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, true,
                                         hostClassFilter, promise_context, NULL);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

JsonElement *Nova2PHP_software_report(char *hostkey, char *name, char *value, char *arch,
                                      bool regex, char *type, HostClassFilter *hostClassFilter,
                                      PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex,
                            hostClassFilter, true, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

	
    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubSoftware);

    bool add_timestamp = strcmp( type, cfr_software ) == 0;

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Package Name", 1);
            JsonObjectAppendInteger(header, "Version", 2);
            JsonObjectAppendInteger(header, "Architecture", 3);

            if(add_timestamp)
            {
                JsonObjectAppendInteger(header, "Last seen", 4);
            }

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubSoftware *hs = (HubSoftware *) rp->item;

            JsonElement *entry = JsonArrayCreate(5);
            JsonArrayAppendString(entry, NULLStringToEmpty(hs->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hs->name));
            JsonArrayAppendString(entry, NULLStringToEmpty(hs->version));
            JsonArrayAppendString(entry, Nova_LongArch(hs->arch));

            if(strcmp(type, cfr_software) == 0)
            {
                JsonArrayAppendInteger(entry, hs->t);
            }

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubSoftware);

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_classes_report(char *hostkey, char *name, bool regex,
                                     HostClassFilter *hostClassFilter, PageInfo *page,
                                     time_t from, time_t to, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, from, to,
                                     hostClassFilter, true, promise_context, NULL);

    PageRecords(&(hq->records), page, DeleteHubClass);
    int related_host_cnt = RlistLen(hq->hosts);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Class or Context", 1);
            JsonObjectAppendInteger(header, "in %% runs", 2);
            JsonObjectAppendInteger(header, "+/- %%", 3);
            JsonObjectAppendInteger(header, "Last occured", 4);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubClass *hc = (HubClass *) rp->item;

            JsonElement *entry = JsonArrayCreate(5);

            JsonArrayAppendString(entry, NULLStringToEmpty(hc->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hc->class));
            JsonArrayAppendReal(entry, hc->prob*100.0);
            JsonArrayAppendReal(entry, hc->dev*100.0);
            JsonArrayAppendInteger(entry, hc->t);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubClass);

    CFDB_Close(&dbconn);

    return payload;
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

        hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type,
                                 regex, 0, time(NULL), hostClassFilter, promise_context, NULL);
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
                PrintRlist(rvalBuf, sizeof(rvalBuf), hv->rval.item);
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
    }


    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/
JsonElement *Nova2PHP_compliance_report(char *hostkey, char *version, time_t from, time_t to,
                                        int k, int nk, int rep, HostClassFilter *hostClassFilter,
                                        PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep,
                                             true, hostClassFilter, promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubTotalCompliance);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Policy Version", 1);
            JsonObjectAppendInteger(header, "Kept", 2);
            JsonObjectAppendInteger(header, "Repaired", 3);
            JsonObjectAppendInteger(header, "Not Kept", 4);
            JsonObjectAppendInteger(header, "Last verified", 5);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubTotalCompliance *ht = (HubTotalCompliance *) rp->item;

            JsonElement *entry = JsonArrayCreate(6);

            JsonArrayAppendString(entry, NULLStringToEmpty(ht->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(ht->version));
            JsonArrayAppendInteger(entry, ht->kept);
            JsonArrayAppendInteger(entry, ht->repaired);
            JsonArrayAppendInteger(entry, ht->notkept);
            JsonArrayAppendInteger(entry, ht->t);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_compliance_promises(char *hostkey, char *handle, char *status, bool regex,
                                          HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter,
                                          bool lastRunOnly, PageInfo *page, PromiseContextMode promise_context)
{
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    if(lastRunOnly)
    {
        hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                                 regex, 0, time(NULL), false, hostClassFilter,
                                                 hostColourFilter, promise_context, NULL);
    }
    else
    {
        hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0,
                                         time(NULL), true, hostClassFilter, promise_context, NULL);
    }

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPromiseCompliance);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Promise Handle", 1);
            JsonObjectAppendInteger(header, "Last Known State", 2);
            JsonObjectAppendInteger(header, "%% Runs Kept", 3);
            JsonObjectAppendInteger(header, "+/- %%", 4);
            JsonObjectAppendInteger(header, "Last verified", 5);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            hp = (HubPromiseCompliance *) rp->item;

            char E[CF_SMALLBUF] = { 0 };
            char D[CF_SMALLBUF] = { 0 };

            WriteDouble2Str_MP(hp->e, E, sizeof(E));
            WriteDouble2Str_MP(hp->d, D, sizeof(D));

            JsonElement *entry = JsonArrayCreate(6);
            JsonArrayAppendString(entry, NULLStringToEmpty(hp->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hp->handle));
            JsonArrayAppendString(entry, NULLStringToEmpty(Nova_LongState(hp->status)));
            JsonArrayAppendString(entry, E);
            JsonArrayAppendString(entry, D);
            JsonArrayAppendInteger(entry, hp->t);

            JsonArrayAppendArray(data, entry);
        }

        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddress,
                                      time_t lago, int lregex, HostClassFilter *hostClassFilter,
                                      PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex,
                                      0, time(NULL), true, hostClassFilter, promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubLastSeen);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(9);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Comms Initiated", 1);
            JsonObjectAppendInteger(header, "Remote host name", 2);
            JsonObjectAppendInteger(header, "Remote IP address", 3);
            JsonObjectAppendInteger(header, "Was Last Seen At", 4);
            JsonObjectAppendInteger(header, "Hrs ago", 5);
            JsonObjectAppendInteger(header, "Avg Comms Interval", 6);
            JsonObjectAppendInteger(header, "+/- hrs", 7);
            JsonObjectAppendInteger(header, "Remote host's key", 8);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubLastSeen *hl = (HubLastSeen *) rp->item;

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

            char hrsAgo[CF_SMALLBUF];
            char hrsAvg[CF_SMALLBUF];
            char hrsDev[CF_SMALLBUF];

            WriteDouble2Str_MP(hl->hrsago, hrsAgo, sizeof(hrsAgo));
            WriteDouble2Str_MP(hl->hrsavg, hrsAvg, sizeof(hrsAvg));
            WriteDouble2Str_MP(hl->hrsdev, hrsDev, sizeof(hrsDev));

            JsonElement *entry = JsonArrayCreate(9);

            JsonArrayAppendString(entry, NULLStringToEmpty(hl->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(inout));
            JsonArrayAppendString(entry, NULLStringToEmpty(hl->rhost->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hl->rhost->ipaddr));
            JsonArrayAppendInteger(entry, hl->t);
            JsonArrayAppendString(entry, hrsAgo);
            JsonArrayAppendString(entry, hrsAvg);
            JsonArrayAppendString(entry, hrsDev);
            JsonArrayAppendString(entry, NULLStringToEmpty(hl->rhost->keyhash));

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubLastSeen);

    CFDB_Close(&dbconn);

    return payload;
}

/*****************************************************************************/
JsonElement *Nova2PHP_performance_report(char *hostkey, char *job, bool regex,
                                         HostClassFilter *hostClassFilter, PageInfo *page,
                                         PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPerformance(&dbconn, hostkey, job, regex, true,
                                         hostClassFilter, promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPerformance);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(9);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Event", 1);
            JsonObjectAppendInteger(header, "Last completion time (seconds)", 2);
            JsonObjectAppendInteger(header, "Avg completion time (seconds)", 3);
            JsonObjectAppendInteger(header, "+/- seconds", 4);
            JsonObjectAppendInteger(header, "Last performed", 5);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            const HubPerformance *hP = (HubPerformance *) rp->item;

            char Q[CF_SMALLBUF];
            char E[CF_SMALLBUF];
            char D[CF_SMALLBUF];

            WriteDouble2Str_MP(hP->q, Q, sizeof(Q));
            WriteDouble2Str_MP(hP->e, E, sizeof(E));
            WriteDouble2Str_MP(hP->d, D, sizeof(D));

            JsonElement *entry = JsonArrayCreate(6);

            JsonArrayAppendString(entry, NULLStringToEmpty(hP->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hP->event));
            JsonArrayAppendString(entry, Q);
            JsonArrayAppendString(entry, E);
            JsonArrayAppendString(entry, D);
            JsonArrayAppendInteger(entry, hP->t);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubPerformance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

    HubQuery *hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, hostClassFilter, promise_context, NULL);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/
/*
 * Converts double to str => "%.2lf"
 * if NaN then writes N/A
 * Suffix MP = Mission Portal (MP specific foormatting %.2lf)
 */
/*****************************************************************************/
static void WriteDouble2Str_MP(double x, char *buffer, int bufsize)
{
    if (isnan(x))
    {
        snprintf(buffer, bufsize, "%s", CF_STR_NOT_AVAILABLE);
    }
    else
    {
        snprintf(buffer, bufsize, "%.2lf", x);
    }
}

/*****************************************************************************/

JsonElement *Nova2PHP_bundle_report(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter,
                           HostColourFilter *hostColourFilter, bool lastRunOnly,
                           PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn = { 0 };
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq;
    if(lastRunOnly)
    {        
        hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter,
                                          hostColourFilter, false, promise_context, NULL);
    }
    else
    {
        hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter,
                                  true, promise_context, NULL);
    }

    int skipped_host_cnt = CFDB_CountSkippedOldAgents(&dbconn, hostkey, hostClassFilter);
    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubBundleSeen);

    JsonElement *payload = JsonObjectCreate(2);
    {
        JsonElement *meta = JsonObjectCreate(4);

        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        JsonObjectAppendInteger(meta, "old_skipped", skipped_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(6);

            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "Bundle", 1);
            JsonObjectAppendInteger(header, "Last Verified", 2);
            JsonObjectAppendInteger(header, "%% Compliance", 3);
            JsonObjectAppendInteger(header, "Avg %% Compliance", 4);
            JsonObjectAppendInteger(header, "+/- %%", 5);

            JsonObjectAppendObject(meta, "header", header);
        }

        JsonObjectAppendObject(payload, "meta", meta);
    }

    JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubBundleSeen *hb = (HubBundleSeen *) rp->item;

        char bundleComp[CF_SMALLBUF];
        char bundleAvg[CF_SMALLBUF];
        char bundleDev[CF_SMALLBUF];

        WriteDouble2Str_MP(hb->bundlecomp * 100, bundleComp, sizeof(bundleComp));
        WriteDouble2Str_MP(hb->bundleavg * 100, bundleAvg, sizeof(bundleAvg));
        WriteDouble2Str_MP(hb->bundledev * 100, bundleDev, sizeof(bundleDev));

        JsonElement *entry = JsonArrayCreate(6);

        JsonArrayAppendString(entry, NULLStringToEmpty(hb->hh->hostname));
        JsonArrayAppendString(entry, NULLStringToEmpty(hb->bundle));
        JsonArrayAppendInteger(entry, hb->t);
        JsonArrayAppendString(entry, bundleComp);
        JsonArrayAppendString(entry, bundleAvg);
        JsonArrayAppendString(entry, bundleDev);

        JsonArrayAppendArray(data, entry);
    }
    JsonObjectAppendArray(payload, "data", data);

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_filechanges_report(char *hostkey, char *file, bool regex,
                                         time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page,
                                         PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to,
                                         true, hostClassFilter, promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubFileChanges);

    JsonElement *payload = JsonObjectCreate(3);
    {
        JsonElement *meta = JsonObjectCreate(2);
        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(5);
            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "File", 1);
            JsonObjectAppendInteger(header, "Change Type", 2);
            JsonObjectAppendInteger(header, "Change Detected at", 3);
            JsonObjectAppendObject(meta, "header", header);
        }
        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            HubFileChanges *hC = (HubFileChanges *) rp->item;

            JsonElement *entry = JsonArrayCreate(4);

            JsonArrayAppendString(entry, NULLStringToEmpty(hC->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hC->path));
            JsonArrayAppendString(entry, NULLStringToEmpty(hC->msg));
            JsonArrayAppendInteger(entry, hC->t);

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
    CFDB_Close(&dbconn);

    return payload;
}

/*****************************************************************************/

JsonElement *Nova2PHP_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                                       HostClassFilter *hostClassFilter, PageInfo *page, PromiseContextMode promise_context)
{
    EnterpriseDB dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex,
                                      from, to, true, hostClassFilter,
                                      promise_context, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubFileDiff);

    JsonElement *payload = JsonObjectCreate(3);
    {
        JsonElement *meta = JsonObjectCreate(2);
        JsonObjectAppendInteger(meta, "count", page->totalResultCount);
        JsonObjectAppendInteger(meta, "related", related_host_cnt);
        {
            JsonElement *header = JsonObjectCreate(5);
            JsonObjectAppendInteger(header, "Host", 0);
            JsonObjectAppendInteger(header, "File", 1);
            JsonObjectAppendInteger(header, "Change Detected at", 2);
            {
                JsonElement *change_details = JsonObjectCreate(2);
                JsonObjectAppendInteger(change_details, "index", 3);
                {
                    JsonElement *subkeys = JsonObjectCreate(3);
                    JsonObjectAppendInteger(subkeys, "plusminus", 0);
                    JsonObjectAppendInteger(subkeys, "line", 1);
                    JsonObjectAppendInteger(subkeys, "diff", 2);
                    JsonObjectAppendObject(change_details, "subkeys", subkeys);
                }
                JsonObjectAppendObject(header, "Change Details", change_details);
            }
            JsonObjectAppendObject(meta, "header", header);
        }
        JsonObjectAppendObject(payload, "meta", meta);
    }

    {
        JsonElement *data = JsonArrayCreate(RlistLen(hq->records));
        for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
        {
            HubFileDiff *hd = (HubFileDiff *) rp->item;
            JsonElement *entry = JsonArrayCreate(4);

            JsonArrayAppendString(entry, NULLStringToEmpty(hd->hh->hostname));
            JsonArrayAppendString(entry, NULLStringToEmpty(hd->path));
            JsonArrayAppendInteger(entry, hd->t);
            JsonArrayAppendArray(entry, Nova_FormatDiff(NULLStringToEmpty(hd->diff)));

            JsonArrayAppendArray(data, entry);
        }
        JsonObjectAppendArray(payload, "data", data);
    }

    DeleteHubQuery(hq, DeleteHubFileDiff);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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
                                         true, hostClassFilter, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubValue);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_software_hosts(char *hostkey, char *name, char *value,
                                     char *arch,  bool regex, char *type,
                                     HostClassFilter *hostClassFilter, PageInfo *page,
                                     PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch,
                                      regex, hostClassFilter, false, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubSoftware);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_classes_hosts(char *hostkey, char *name, bool regex,
                                    HostClassFilter *hostClassFilter, PageInfo *page,
                                    time_t from, time_t to, PromiseContextMode promise_context,
                                    WebReportFileInfo *wr_info)
{    
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, from, to,
                                     hostClassFilter, false, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubClass);    

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_vars_hosts(char *hostkey, char *scope, char *lval, char *rval,
                                 char *type, bool regex, HostClassFilter *hostClassFilter,
                                 PageInfo *page, PromiseContextMode promise_context,
                                 WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    char ns[CF_MAXVARSIZE] = { 0 };
    char bundle[CF_MAXVARSIZE] = { 0 };
    if (scope)
    {
        SplitScopeName(scope, ns, bundle);
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type,
                                       regex, 0, time(NULL), hostClassFilter, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubVariable);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_compliance_hosts(char *hostkey, char *version, time_t from,
                                       time_t to, int k, int nk, int rep,
                                       HostClassFilter *hostClassFilter, PageInfo *page,
                                       PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to,
                                             k, nk, rep, false, hostClassFilter,
                                             promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promise_hosts(char *hostkey, char *handle, char *status,
                                    bool regex, HostClassFilter *hostClassFilter,
                                    HostColourFilter *hostColourFilter,
                                    bool lastRunOnly, PageInfo *page,
                                    PromiseContextMode promise_context,
                                    WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HubQuery *hq = NULL;

    if(lastRunOnly)
    {
        hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status,
                                                 regex, 0, time(NULL), false,
                                                 hostClassFilter, hostColourFilter,
                                                 promise_context, NULL);
    }
    else
    {
        hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex,
                                         0, time(NULL), false, hostClassFilter,
                                         promise_context, NULL);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_lastseen_hosts(char *hostkey, char *lhash, char *lhost,
                                     char *laddress, time_t lago, int lregex,
                                     HostClassFilter *hostClassFilter, PageInfo *page,
                                     PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress,
                                      lago, lregex, 0, time(NULL), false,
                                      hostClassFilter, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubLastSeen);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_performance_hosts(char *hostkey, char *job, bool regex,
                                        HostClassFilter *hostClassFilter, PageInfo *page,
                                        PromiseContextMode promise_context,
                                        WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPerformance(&dbconn, hostkey, job, regex,
                                         false, hostClassFilter, promise_context, NULL);


    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubPerformance);

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

    HubQuery *hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, hostClassFilter,
                                    promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubSetUid);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_bundle_hosts(char *hostkey, char *bundle, bool regex,
                                   HostClassFilter *hostClassFilter,
                                   HostColourFilter *hostColourFilter,
                                   bool lastRunOnly, PageInfo *page,
                                   PromiseContextMode promise_context,
                                   WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq;

    if(lastRunOnly)
    {
        hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex,
                                          hostClassFilter, hostColourFilter, false,
                                          promise_context, NULL);
    }
    else
    {
        hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex,
                                  hostClassFilter, true, promise_context, NULL);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if( wr_info )
    {
        return json_out;
    }

    int skipped_cnt = CFDB_CountSkippedOldAgents(&dbconn, hostkey, hostClassFilter);
    JsonElement *json_obj_meta = JsonObjectGetAsObject(json_out, "meta");
    JsonObjectAppendInteger(json_obj_meta, "old_skipped", skipped_cnt);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_filechanges_hosts(char *hostkey, char *file, bool regex,
                                        time_t from, time_t to,
                                        HostClassFilter *hostClassFilter, PageInfo *page,
                                        PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to,
                                         false, hostClassFilter, promise_context, NULL);

    CFDB_Close(&dbconn);

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubFileChanges);

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_filediffs_hosts(char *hostkey, char *file, char *diffs,
                                      bool regex, time_t from, time_t to,
                                      HostClassFilter *hostClassFilter, PageInfo *page,
                                      PromiseContextMode promise_context, WebReportFileInfo *wr_info)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, from,
                                      to, false, hostClassFilter, promise_context, NULL);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_out = GenerateHostOnlyReport( (&hq->hosts), page, wr_info );

    DeleteHubQuery(hq, DeleteHubFileDiff);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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
        DeleteRlist(classList);
        IdempPrependItem(&list, "any", NULL);
    }

    JsonElement *payload = JsonArrayCreate(ItemListSize(list));
    if (list)
    {
        list = SortItemListNames(list);

        for (const Item *ip = list; ip != NULL; ip = ip->next)
        {
            char context[CF_MAXVARSIZE] = { 0 };
            snprintf(context, CF_MAXVARSIZE, "class_contexts::%s", ip->name);
            int pid = Nova_GetTopicIdForTopic(context);

            JsonElement *entry = JsonArrayCreate(2);
            JsonArrayAppendInteger(entry, pid);
            JsonArrayAppendString(entry, NULLStringToEmpty(ip->name));

            JsonArrayAppendArray(payload, entry);
        }

        DeleteItemList(list);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return payload;
}

/*****************************************************************************/

int Nova2PHP_bundle_agent_goals(PromiseFilter *filter, char *buffer, int bufsize)
{
    EnterpriseDB dbconn;
    char work[CF_BUFSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hqBundles = CFDB_QueryPromiseBundles(&dbconn, filter, PROMISE_CONTEXT_MODE_ALL);

    if (CountRecords(hqBundles) == 0)
    {
        DeleteHubQuery(hqBundles, DeleteHubPromiseBundle);
        CFDB_Close(&dbconn);
        return false;
    }

    HubQuerySortPromiseBundles(hqBundles);

    snprintf(buffer, bufsize,
             "{\"meta\":{\"header\": {\"Type\":0,\"Service bundle name\":1,\"Description\":2,"
             "\"Contributing to Goals\":{\"index\":3,\"subkeys\":{\"pid\":0,\"name\":1,\"description\":2}},"
             "\"\":4" "}},\"data\":[");

    for (Rlist *rp = hqBundles->records; rp != NULL; rp = rp->next)
    {
        HubPromiseBundle *bundle = rp->item;

        Item *ip2, *bundleGoals = Nova_GetBusinessGoals(bundle->bundleName);
        char goals[CF_BUFSIZE];
        char colour[CF_SMALLBUF];

        if (bundleGoals)
        {
            snprintf(goals, sizeof(goals), "[");

            for (ip2 = bundleGoals; ip2 != NULL; ip2 = ip2->next)
            {
                snprintf(work, sizeof(work), "[%d,\"%s\",\"%s\"],", ip2->counter, ip2->name, ip2->classes);

                if (!Join(goals, work, CF_BUFSIZE))
                {
                    break;
                }
            }

            ReplaceTrailingChar(goals, ',', '\0');
            EndJoin(goals, "]", sizeof(goals));

            snprintf(colour, CF_SMALLBUF, "green");
        }
        else
        {
            snprintf(goals, CF_MAXVARSIZE, "[[-1,\"Unknown\",\"Unknown\"]]");
            snprintf(colour, CF_SMALLBUF, "yellow");
        }

        char bundlename_lower[CF_BUFSIZE];
        strlcpy(bundlename_lower, bundle->bundleName, CF_BUFSIZE);
        ToLowerStrInplace(bundlename_lower);

        snprintf(work, sizeof(work), "[\"%s\",\"%s\",\"%s\",%s,\"%s\"],",
                 bundle->bundleType, bundle->bundleName, Nova_GetBundleComment(bundlename_lower), goals,
                 colour);

        if (!Join(buffer, work, bufsize))
        {
            break;
        }

        DeleteItemList(bundleGoals);
    }

    ReplaceTrailingChar(buffer, ',', '\0');

    EndJoin(buffer, "]}\n", bufsize);

    DeleteHubQuery(hqBundles, DeleteHubPromiseBundle);

    CFDB_Close(&dbconn);

    return true;
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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

Item *Nova2PHP_search(char *search, bool regex, char *username)
{
    Item *ip, *ipt, *results = NULL, *topics;
    Item *words = SplitStringAsItemList(search, ' ');
    char url[CF_BUFSIZE], text[CF_BUFSIZE];
    static char *excludes[] = { "tell", "me", "about", "host", "hosts", "report", "topic", "with", "running", NULL };

    for (ip = words; ip != NULL; ip = ip->next)
       {
          if (StringInArray(excludes, ip->name))
             {
             continue;
             }

       // KM gets us anything about policy or syntax
       
       if (MightBeTopic(ip->name))
          {
          topics = Nova_SearchTopicMap(ip->name,CF_SEARCH_REGEX, false);

          for (ipt = topics; ipt != NULL; ipt = ipt->next)
             {
             snprintf(url, CF_BUFSIZE, "%s/%d", KM_CONTROLLER_PREFIX, ipt->counter);
             snprintf(text, CF_BUFSIZE, "%s appears in context '%s'", ipt->name, ipt->classes);
             PrependFullItem(&results, text, url, CF_CATEGORY_TOPIC, 0);
             }

          DeleteItemList(topics);
          }
       
       // Now add hosts with attributes
       
       if (MightBeHostId(ip->name))
          {
          Nova_SearchHosts(&results, ip->name,CF_SEARCH_REGEX, username);
          }

       if (MightBeHostColour(ip->name))
          {
          Nova_SearchHosts(&results, ip->name,CF_SEARCH_REGEX, username);
          }
       
       if (MightBeClass(ip->name))
          {
          Nova_SearchClasses(&results, ip->name,CF_SEARCH_REGEX, username);
          }
       
       if (MightBeVariable(ip->name))
          {
          Nova_SearchVariables(&results, ip->name,CF_SEARCH_REGEX, username);
          }
       
       if (MightBeReportType(ip->name))
          {
          Nova_SearchReports(&results, ip->name);
          }
       }

    results = SortItemListNames(results);    

    if (results == NULL)
       {
       PrependItem(&results, "Nothing matched the search", "any");
       }
        
    return results;
}


/*****************************************************************************/

JsonElement *Nova2PHP_search_topics(char *search, bool regex)
{
    Item *ip,*results = NULL;
    JsonElement *json_array_out = JsonArrayCreate(100);

    results = Nova_SearchTopicMap(search,CF_SEARCH_REGEX,true);
    if (!results)
    {
        return json_array_out;
    }

    results = SortItemListNames(results);

    for (ip = results; ip != NULL; ip = ip->next)
    {
        JsonElement *json_obj = JsonObjectCreate(3);
        JsonObjectAppendString(json_obj, "context", ip->classes);
        JsonObjectAppendString(json_obj, "topic", ip->name);
        JsonObjectAppendInteger(json_obj, "id", ip->counter);

        JsonArrayAppendObject(json_array_out, json_obj);
    }

    return json_array_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_list_knowledge_bundles(void)
{
    Item *ip,*results = NULL;
    JsonElement *json_array_out = JsonArrayCreate(30);
    bson_iterator it1;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson query;
    bson_empty(&query);

    bson fields;

    BsonSelectReportFields(&fields, 1, cfk_bundle);
        
    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_BUNDLES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_bundle) == 0)
            {
                IdempPrependItem(&results, bson_iterator_string(&it1), NULL);
            }
        }
    }
    mongo_cursor_destroy(cursor);

    results = SortItemListNames(results);

    for (ip = results; ip != NULL; ip = ip->next)
    {
        JsonArrayAppendString(json_array_out, ip->name);
    }

    return json_array_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_list_topics_for_bundle(char *name)
{
    Item *results = NULL;
    JsonElement *json_array_out = JsonArrayCreate(100);
    bson_iterator it1;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson query;
    bson_init(&query);
    bson_append_string(&query, cfk_bundle, name);
    BsonFinish(&query);

    bson fields;
    BsonSelectReportFields(&fields, 3, cfk_topicname, cfk_topiccontext, cfk_topicid);
        
    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        char topic[CF_BUFSIZE] = {0}, context[CF_BUFSIZE] = {0};
        int topic_id = 0;
    
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                strncpy(topic, bson_iterator_string(&it1),CF_BUFSIZE);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                strncpy(context, bson_iterator_string(&it1),CF_BUFSIZE);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = bson_iterator_int(&it1);
            }
            
        }

        if (!IsItemIn(results, topic))
           {
           JsonElement *json_obj_subtopic = JsonObjectCreate(3);
           JsonObjectAppendString(json_obj_subtopic, "context", context);
           JsonObjectAppendString(json_obj_subtopic, "topic", topic);
           JsonObjectAppendInteger(json_obj_subtopic, "topic_id", topic_id);
           JsonArrayAppendObject(json_array_out, json_obj_subtopic);
           PrependItem(&results, topic, NULL);
           }
    }

    DeleteItemList(results);
    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);    
    return json_array_out;
}


/*****************************************************************************/

void Nova2PHP_bundle_for_topic(int topic_id, char *buffer, int bufsize)
{
    bson_iterator it1;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return;
    }

    bson query;
    bson_init(&query);
    bson_append_int(&query, cfk_topicid, topic_id);
    BsonFinish(&query);

    bson fields;
    BsonSelectReportFields(&fields, 1, cfk_bundle);
        
    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);

    buffer[0] = '\0';

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_bundle) == 0)
            {
                strncpy(buffer, (char *)bson_iterator_string(&it1), bufsize);
            }            
        }

    }

    mongo_cursor_destroy(cursor);

    CFDB_Close(&conn);
}

/*****************************************************************************/

JsonElement *Nova2PHP_show_topic(int id)
{
    char topic_name[CF_BUFSIZE], topic_id[CF_BUFSIZE], topic_context[CF_BUFSIZE];
    char bundle[CF_BUFSIZE], description[CF_BUFSIZE];
    JsonElement *json_out = NULL;

    if (Nova_GetTopicByTopicId(id, topic_name, topic_id, topic_context, bundle))
    {
        Nova_GetTopicComment(topic_name, topic_context, description, CF_BUFSIZE);
        json_out = JsonObjectCreate(4);
        JsonObjectAppendString(json_out, "topic", topic_name);
        JsonObjectAppendString(json_out, "context", topic_context);
        JsonObjectAppendString(json_out, "description", description);
        JsonObjectAppendString(json_out, "bundle", bundle);
    }
    else
    {
        json_out = JsonArrayCreate(1);
    }

    return json_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_show_all_context_leads(char *unqualified_topic)
{
    char reconstructed[CF_BUFSIZE];
    Item *ip,*candidates;
    int id;

    JsonElement *json_array_out = JsonArrayCreate(100);

    candidates = Nova_SearchTopicMap(unqualified_topic,CF_SEARCH_EXACT,false);
    if (candidates == NULL)
    {
       return json_array_out;
    }

    for (ip = candidates; ip != NULL; ip=ip->next)
    {
        JsonElement *json_obj = JsonObjectCreate(2);
        JsonObjectAppendString(json_obj, "context", ip->classes);

        snprintf(reconstructed,CF_BUFSIZE,"%s::%s",ip->classes,ip->name);
        id = Nova_GetTopicIdForTopic(reconstructed);

        JsonElement *json_array_topic = Nova2PHP_show_topic_leads(id);
        JsonObjectAppendArray(json_obj, "leads", json_array_topic);

        JsonArrayAppendObject(json_array_out, json_obj);
    }
    
    DeleteItemList(candidates);

    return json_array_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_show_topic_leads(int id)
{
    Item *ip;
    Item *list = Nova_ScanLeadsAssociations(id, NULL);

    JsonElement *json_array_out = JsonArrayCreate(100);
    if (list == NULL)
    {
        return json_array_out;
    }

// name contains the association
// classes contains the related topic
// counter contains the topic id

// Aggregate all contexts

    char *last_name = NULL;
    for (ip = list; ip != NULL; ip = ip->next)
    {
        if (StringSafeCompare(ip->name, last_name) == 0)
        {
            last_name = ip->name;
            continue;
        }
        last_name = ip->name;

        JsonElement *json_obj = JsonObjectCreate(2);
        JsonObjectAppendString(json_obj, "assoc", ip->name);

        JsonElement *json_topic_arr = JsonArrayCreate(10);

        for (Item *ip2 = ip; ip2 != NULL; ip2 = ip2->next)
        {
            if (StringSafeCompare(ip->name, ip2->name) == 0)
            {
                JsonElement *json_topic_obj = JsonObjectCreate(2);
                JsonObjectAppendString(json_topic_obj, "topic", ip2->classes);
                JsonObjectAppendInteger(json_topic_obj, "id", ip2->counter);

                JsonArrayAppendObject(json_topic_arr, json_topic_obj);
            }
        }

        JsonObjectAppendArray(json_obj, "topics", json_topic_arr);
        JsonArrayAppendObject(json_array_out, json_obj);
    }

    DeleteItemList(list);

    return json_array_out;
}

/*****************************************************************************/

JsonElement *Nova2PHP_show_topic_hits(int id)
{
    return Nova_ScanOccurrences(id);
}

/*****************************************************************************/

JsonElement *Nova2PHP_show_topic_category(int id)
{
    return Nova_ScanTheRest(id);
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
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_show_hosts_test(hostNameRegex, ipRegex, NULL, page, buf, bufsize);
    }
# endif

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return "No comment";
}

/*****************************************************************************/

void Nova2PHP_GetPromiseBody(char *name, char *type, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE];
    EnterpriseDB dbconn;
    HubBody *hb;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    hb = CFDB_QueryBody(&dbconn, type, name);

    if (hb)
    {
        snprintf(returnval, CF_MAXVARSIZE - 1, "{");

        snprintf(work, CF_MAXVARSIZE - 1, "\"Type\":\"%s\",", hb->bodyType);
        Join(returnval, work, bufsize);

        snprintf(work, CF_MAXVARSIZE - 1, "\"Name\":\"%s\",", hb->bodyName);
        Join(returnval, work, bufsize);

        if (hb->bodyArgs)
        {
            snprintf(work, CF_MAXVARSIZE - 1, "\"Arguments\":\"%s\",", hb->bodyArgs);
            Join(returnval, work, bufsize);
        }

        if (hb->attr)
        {
            HubBodyAttr *ha;

            Join(returnval, "\"attributes\":[", bufsize);
            for (ha = hb->attr; ha != NULL; ha = ha->next)
            {
                snprintf(work, CF_MAXVARSIZE - 1, "{\"lval\":\"%s\",\"rval\":\"%s\",\"class_context\":\"%s\"},",
                         ha->lval, ha->rval, ha->classContext);
                Join(returnval, work, bufsize);
            }

            ReplaceTrailingChar(returnval, ',', '\0');
            EndJoin(returnval, "]", bufsize);
        }

        DeleteHubBody(hb);
        EndJoin(returnval, "}", bufsize);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }
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

int Nova2PHP_promise_details(PromiseFilter *filter, char *returnval, int bufsize)
{
    EnterpriseDB dbconn;
    char promiseeText[CF_MAXVARSIZE];
    char commentText[CF_MAXVARSIZE];
    char work[CF_BUFSIZE], escaped[CF_BUFSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hqPromise = CFDB_QueryPromisesUnexpanded(&dbconn, filter,
                                                       PROMISE_CONTEXT_MODE_ALL);

    if (CountRecords(hqPromise) == 0)
    {
        snprintf(returnval, bufsize, " Promise was not found in the database.");
        DeleteHubQuery(hqPromise, DeleteHubPromise);
        CFDB_Close(&dbconn);
        return false;
    }

    HubPromise *hp = HubQueryGetFirstRecord(hqPromise);

    returnval[0] = '\0';

    strcat(returnval, "{");

    snprintf(work, sizeof(work), "\"bundletype\":\"%s\",\"bundlename\":\"%s\",", hp->bundleType, hp->bundleName);
    Join(returnval, work, bufsize);

    snprintf(work, sizeof(work), "\"handle\":\"%s\",", hp->handle);
    Join(returnval, work, bufsize);

    snprintf(work, sizeof(work), "\"promiser\":\"%s\",", EscapeJson(hp->promiser, escaped, sizeof(escaped)));
    Join(returnval, work, bufsize);

    if (RlistLen(hp->promisees) == 0)
    {
        strcpy(promiseeText, "None mentioned");
    }
    else
    {
        PrintRlist(promiseeText, sizeof(promiseeText), hp->promisees);
    }

    snprintf(work, sizeof(work), "\"promisee\":\"%s\",", promiseeText);
    Join(returnval, work, bufsize);

    if (NULL_OR_EMPTY(hp->comment))
    {
        strcpy(commentText, "No comment");
    }
    else
    {
        snprintf(commentText, sizeof(commentText), "%s", hp->comment);
    }

    snprintf(work, sizeof(work), "\"comment\":\"%s\",", EscapeJson(commentText, escaped, sizeof(escaped)));
    Join(returnval, work, bufsize);

    snprintf(work, sizeof(work), "\"promise_type\":\"%s\",", hp->promiseType);
    Join(returnval, work, bufsize);

    snprintf(work, sizeof(work), "\"class_context\":\"%s\",", hp->classContext);
    Join(returnval, work, bufsize);

    snprintf(work, sizeof(work), "\"file\":\"%s\",\"line_num\":%d,", hp->file, hp->lineNo);
    Join(returnval, work, bufsize);

    if (hp->constraints)
    {
        snprintf(work, sizeof(work), "\"body\":[");
        Join(returnval, work, bufsize);

        for (Rlist *rp = hp->constraints; rp != NULL; rp = rp->next)
        {
            char lval[CF_MAXVARSIZE], rval[CF_MAXVARSIZE], args[CF_MAXVARSIZE];

            args[0] = '\0';
            sscanf(ScalarValue(rp), "%255s => %1023[^(,;]%[^\n]", lval, rval, args);

            if (strcmp(lval, "usebundle") == 0)
            {
                snprintf(work, sizeof(work),
                         "{\"constraint_type\":\"bundle\",\"type\":\"%s\",\"name\":\"%s\",\"args\":\"%s\"},", lval,
                         rval, args);
            }
            else
            {
                snprintf(work, sizeof(work),
                         "{\"constraint_type\":\"body\",\"type\":\"%s\",\"name\":\"%s\",\"args\":\"%s\"},", lval, rval,
                         args);
            }

            Join(returnval, work, bufsize);
        }

        ReplaceTrailingChar(returnval, ',', '\0');
        strcat(returnval, "]");
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    strcat(returnval, "}");

    DeleteHubQuery(hqPromise, DeleteHubPromise);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
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
        CfOut(cf_error, "", "!! JSONErrorFromId: errid out of bounds");
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

    if (mongo_cursor_next(cursor) == MONGO_OK)      // not more than one record
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
                CfOut(cf_verbose, "", "!! Existing network measurement incorrect - ignoring");
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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, now - (time_t)bluehost_threshold,
                           now, hostClassFilter, false, promise_context, NULL);

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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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

int Nova2PHP_validate_policy(char *file, char *buffer, int bufsize)
{
    char cmd[CF_BUFSIZE];
    char tmp[CF_MAXVARSIZE];
    int retVal;
    FILE *pp;

    // NOTE: this is run as the web user

    snprintf(cmd, CF_BUFSIZE, "/var/cfengine/bin/cf-promises -n -f \"%s\"", file);

    if ((pp = cf_popen(cmd, "r")) == NULL)
    {
        snprintf(buffer, bufsize, "Could not run command \"%s\": %s", cmd, GetErrorStr());
        return -1;
    }

    buffer[0] = '\0';

    while (!feof(pp))
    {
        if (fgets(tmp, sizeof(tmp), pp))
        {
            Join(buffer, tmp, bufsize);
        }
    }

    retVal = cf_pclose(pp);
    return retVal;              // 0 on success
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

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator i;

        if (!bson_find(&i, &cursor->current, cfr_keyhash))
        {
            CfOut(cf_verbose, "", "Malformed query result in Nova2PHP_environment_contents");
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

void FreeHostsList(HostsList *list)
{
    while (list)
    {
        HostsList *next = list->next;

        free(list->keyhash);
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
            CfOut(cf_verbose, "", "!! Hostkey and noteid is not given. Nothing to look for");
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

    DeleteRlist(result);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
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
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_promise_list_test(promiseFilter, returnval, bufsize);
    }
# endif

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
        CfOut(cf_error, "", "!! FormatErrorJsonAttribute: errid out of bounds");
        errid = ERRID_MAX;
    }

    snprintf(out, outSz, "\"error\":{\"errid\":%d,\"msg\":\"%s\"}", errid, ERRID_DESCRIPTION[errid]);

    return out;
}

/*****************************************************************************/
#ifndef NDEBUG
bool IsEnvMissionPortalTesting()
{
    const char *total_env = getenv("CFENGINE_TEST_MISSION_PORTAL");

    if (total_env == NULL)
    {
        return false;
    }

    return (atoi(total_env));
}
#endif // NDEBUG

/*****************************************************************************/
/* Service page                                                              */
/*****************************************************************************/

void Nova2PHP_get_service_activity_for(char *hostkey)
{
/*    JsonElement *jsonRetval = JsonObjectCreate(6);
    JsonObjectAppendInteger(jsonRetval,"green", green_hosts);
    JsonObjectAppendInteger(jsonRetval, "yellow", yellow_hosts);
    JsonObjectAppendInteger(jsonRetval, "red", red_hosts);
    JsonObjectAppendInteger(jsonRetval, "blue_hosts", blue_hosts);
    JsonObjectAppendInteger(jsonRetval, "black_hosts", black_hosts);
    JsonObjectAppendInteger(jsonRetval, "host_count", tot_hosts);

    return jsonRetval;
*/
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


/*****************************************************************************/

JsonElement *Nova2PHP_get_goal_progress(char *handle)
{
    char name[CF_MAXVARSIZE];
    snprintf(name, CF_MAXVARSIZE, "handles::%s", handle);
    int goal_id = Nova_GetTopicIdForTopic(name);
    Item *ip, *handles = Nova_GetHandlesForGoal(goal_id);
    int hosts = 0;
    double have_people = false;
    EnterpriseDB dbconn;
    double average_compliance = 0, total_compliance = 0;
         
    // The number of people who seem to be involved - some of these are people, some are not people:: or "@" address
    Item *people = Nova_GetStakeHolders(goal_id);

    if (!CFDB_Open(&dbconn))
    {
        return JSONErrorFromId(ERRID_DBCONNECT);
    }

    handles = SortItemListNames(handles);

    JsonElement *json = JsonObjectCreate(4);
    JsonElement *array_promises = JsonArrayCreate(100);
    JsonElement *array_users = JsonArrayCreate(10);
    JsonElement *array_stakeholders = JsonArrayCreate(10);
 
    for (ip = handles; ip != NULL; ip = ip->next)
    {
        HubPromiseCompliance *hp;
        HubQuery *hq;
        Rlist *rp;
        time_t now = time(NULL);
        char status[CF_MAXVARSIZE];
         
        //printf(" Involves promise %s::%s %d\n", ip->classes,ip->name,ip->counter);
         
        hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, NULL, ip->name, 'x', false, 0, now, false,
                                                 NULL, NULL, PROMISE_CONTEXT_MODE_ALL, NULL);

        hosts = RlistLen(hq->hosts);

        if (hosts == 0)
        {
            snprintf(status, CF_MAXVARSIZE, "No hosts have kept this promise yet");
        }
        else
        {
            average_compliance = 0;
            
            for (rp = hq->records; rp != NULL; rp = rp->next)
            {
                hp = (HubPromiseCompliance *) rp->item;
               
                //printf("Host %s has tried to keep this promise\n", hp->hh->hostname);

                average_compliance += hp->e;
            }

            total_compliance += average_compliance;
        }

        JsonElement *promise = JsonObjectCreate(3);
        JsonObjectAppendString(promise, "name", ip->name);
        JsonObjectAppendString(promise, "context", ip->classes);
        JsonObjectAppendInteger(promise, "topic_id", ip->counter);
        JsonObjectAppendInteger(promise, "compliance", average_compliance);
        JsonArrayAppendObject(array_promises, promise);         
    }

    CFDB_Close(&dbconn);

    Item *users = NULL, *stake = NULL;
   
    for (ip = people; ip != NULL; ip = ip->next)
    {
        if (strstr(ip->name, "@") || strcmp(ip->classes, "users") == 0)
        {
            //printf(" Person responsible %s::%s %d\n", ip->classes,ip->name, ip->counter);
            have_people = true;
            PrependFullItem(&users, ip->name, ip->classes, ip->counter, 0);
        }
        else
        {
            //printf(" Other stakeholders %s::%s \n", ip->classes,ip->name);
            PrependFullItem(&stake, ip->name, ip->classes, ip->counter, 0);
        }
    }

    DeleteItemList(people);
    users = SortItemListNames(users);
    stake = SortItemListNames(stake);

    for (ip = users; ip != NULL; ip=ip->next)
    {
        JsonElement *user = JsonObjectCreate(3);
        JsonObjectAppendString(user, "name", ip->name);
        JsonObjectAppendString(user, "context", ip->classes);
        JsonObjectAppendInteger(user, "topic_id", ip->counter);
        JsonArrayAppendObject(array_users, user);         
    }
  
    for (ip = stake; ip != NULL; ip=ip->next)
    {
        JsonElement *stake = JsonObjectCreate(3);
        JsonObjectAppendString(stake, "name", ip->name);
        JsonObjectAppendString(stake, "context", ip->classes);
        JsonObjectAppendInteger(stake, "topic_id", ip->counter);
        JsonArrayAppendObject(array_stakeholders, stake);
    }

    DeleteItemList(users);
    DeleteItemList(stake);
  
    double goal_score = (total_compliance + 100.0*have_people)/2.0;
    
    JsonObjectAppendArray(json, "promises", array_promises);
    JsonObjectAppendArray(json, "users", array_users);
    JsonObjectAppendArray(json, "stakeholders", array_stakeholders);
    JsonObjectAppendInteger(json, "score", (int)goal_score);
    
    return json;
}

/*****************************************************************************/

int Nova2PHP_list_all_goals(char *buffer, int bufsize)
{
    if (Nova_GetUniqueBusinessGoals(buffer, bufsize))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/

JsonElement *Nova2PHP_list_services()
{
 return Nova_GetApplicationServices();
}

/*****************************************************************************/

JsonElement *Nova2PHP_list_service_ports()
{
 return Nova_GetMeasuredServices();
}

/*****************************************************************************/

JsonElement *Nova2PHP_list_promises_with_promisee(char *name)
{
 JsonElement *array_promises = JsonArrayCreate(20);
 int id = Nova_GetTopicIdForTopic(name);
 Item *list = Nova_NearestNeighbours(id, NOVA_USES_PR);
 
 list = SortItemListNames(list);

  for (Item *ip = list; ip != NULL; ip = ip->next)
    {
    if (strcmp(ip->classes, "promises") == 0)
       {
       JsonElement *user = JsonObjectCreate(3);
       JsonObjectAppendString(user, "handle", ip->name);
       JsonObjectAppendString(user, "context", ip->classes);
       JsonObjectAppendInteger(user, "topic_id", ip->counter);
       JsonArrayAppendObject(array_promises, user);         
       }
    }

 DeleteItemList(list);

 return array_promises;
}


/*****************************************************************************/

JsonElement *Nova2PHP_list_types_in_bundle(char *name)
{
     EnterpriseDB conn;
     PromiseFilter *filter = NewPromiseFilter();
     PromiseFilterAddBundles(filter, name, NULL);
     
     enum typesequence type;
     int total = 0, type_counter[kp_none] = {0};
     
     if (!CFDB_Open(&conn))
     {
         return false;
     }
  
     HubQuery *hq = CFDB_QueryPromisesExpanded(&conn, filter);

     CFDB_Close(&conn);
 
     for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
     {
         HubPromise *hp = (HubPromise *) rp->item;
        
         for (type = 0; type != kp_none; type++)
         {
             if (strcmp(hp->promiseType, AGENT_TYPESEQUENCE[type]) == 0)
             {
                 type_counter[type]++;
                 total++;
             }
         }
     }
     
     DeleteHubQuery(hq, DeleteHubPromise);
     
     JsonElement *types = JsonObjectCreate(kp_none);
     
     for (type = 0; type != kp_none; type++)
     {
         if (type_counter[type] > 0)
         {
             JsonObjectAppendInteger(types, AGENT_TYPESEQUENCE[type], type_counter[type]);
         }
     }
     
     JsonObjectAppendInteger(types, "total", total);     
     return types;
}

/********************************************************************/
/* Autodetect free text intentions                                  */
/********************************************************************/

static int MightBeTopic(const char *search)
{
 if (strncmp(search, "SHA=", 4) == 0 || strncmp(search, "sha=", 4) == 0)
    {
    return false;
    }
       
 return true;
}

/********************************************************************/

static int MightBeHostId(const char *search)
{
 if (strncmp(search, "SHA=", 4) == 0 || strncmp(search, "sha=", 4) == 0)
    {
    return true;
    }

 if (IsIPV6Address((char *)search) || IsIPV4Address((char *)search))
    {
    return true;
    }

 return false;
}

/********************************************************************/

static int MightBeHostColour(const char *search)
{
 const char *colours[] = { "red", "yellow", "green", "blue", "black", NULL };

 for (int i = 0; colours[i] != NULL; i++)
    {
    if (strcmp(search, colours[i]) == 0)
       {
       return true;
       }
    }

 return false;
}

/********************************************************************/

static int MightBeClass(const char *search)
{
 if (strncmp(search, "SHA=", 4) == 0 || strncmp(search, "sha=", 4) == 0)
    {
    return false;
    }

 if (FullTextMatch(CF_CLASSRANGE, search))
    {
    return true;
    }

 return false;
}

/********************************************************************/

static int MightBeVariable(const char *search)
{
 if (strncmp(search, "SHA=", 4) == 0 || strncmp(search, "sha=", 4) == 0)
    {
    return false;
    }

 if (strstr(search, "::"))
    {
    return false;
    }
 
 return true;
}

/********************************************************************/

static int MightBeReportType(const char *search)
{
 if (strncmp(search, "SHA=", 4) == 0 || strncmp(search, "sha=", 4) == 0)
    {
    return false;
    }

 return true;
}

