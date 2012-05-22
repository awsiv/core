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

#include <assert.h>

static const char *CDP_REPORTS[][2] =
{
    [cdp_acls] = {"ACLs", "File access controls"},
    [cdp_commands] = {"Commands", "Scheduled commands to execute"},
    [cdp_filechanges] = {"File Changes", "File changes observed on the system"},
    [cdp_filediffs] = {"File Diffs", "Delta/difference comparison showing file changes"},
    [cdp_registry] = {"Registry", "Promised Windows registry setting status"},
    [cdp_services] = {"Services", "System services status"},
};

static const char *ERRID_DESCRIPTION[] =
{
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
    [ERRID_CONSTELLATION_LICENSE] = "This functionality requires a Constellation license",
    [ERRID_DATA_UNAVAILABLE] = "Insufficient data / Data unavailable",
    [ERRID_HOST_NOT_FOUND] = "Host not found",
    [ERRID_MAX] = "Unknown error - description out of bounds",
};

/*****************************************************************************/
// deprecate in favour of JSONErrorFromId
static cfapi_errid FormatReportInfoAsJson(char *reportId, ReportInfo *reports, char *buf, int bufsize);
static int CreateJsonHostOnlyReport(Rlist **records_p, PageInfo *page, char *returnval, int bufsize);
static void WriteDouble2Str_MP(double x, char *buffer, int bufsize);
JsonElement *JSONErrorFromId(cfapi_errid errid);

/*****************************************************************************/
#ifndef NDEBUG
static bool IsEnvMissionPortalTesting(void);
#endif
/*****************************************************************************/

/*****************************************************************************/
/* Helper functions                                                          */
/*****************************************************************************/

void Nova2PHP_GetLibraryDocuments(char *path, char *buffer, int bufsize)
{
    char namedir[CF_MAXVARSIZE], work[CF_BUFSIZE];
    struct dirent *dirp;
    Item *refs = NULL, *guides = NULL, *others = NULL, *pics = NULL, *ip;
    DIR *dirh;

    if (path)
    {
        snprintf(namedir, CF_MAXVARSIZE, "%s", path);
    }
    else
    {
        snprintf(namedir, CF_MAXVARSIZE, "%s/docs", DOCROOT);
    }

    if ((dirh = opendir(namedir)) == NULL)
    {
        CfOut(cf_verbose, "", " !! Could not open connection to report database");
        return;
    }

    for (dirp = readdir(dirh); dirp != NULL; dirp = readdir(dirh))
    {
        if (strcmp("cf3-reference.html", dirp->d_name) == 0 || strcmp("NovaOwnersManual.html", dirp->d_name) == 0)
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
    mongo_connection dbconn;

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

    Nova_Meter(query, MONGO_DATABASE, buffer, bufsize);
    bson_destroy(query);
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
    mongo_connection dbconn;
    time_t now = time(NULL), from = now, to = now - SECONDS_PER_WEEK;
    int code_blue = 0, tot_hosts, code_black = 0;
    double n, r, k, n_av, k_av, r_av, tot_promises;
    char *current_host = "x";
    unsigned long bluehost_threshold;

    if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
    {
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, hostClassFilter);

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
        CFDB_GetHostColour(hp->hh->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour);
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

JsonElement *Nova2PHP_promise_compliance_summary (char *hostkey, char *handle, char *status, bool regex,
                            HostClassFilter *hostClassFilter)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    mongo_connection dbconn;
    char *current_host = "x";

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HubQuery *hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, hostClassFilter, NULL);

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
                            HostClassFilter *hostClassFilter)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    mongo_connection dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter,NULL, false);

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

void Nova2PHP_meter(char *hostkey, char *buffer, int bufsize)
{
    bson_buffer bb;
    bson query;

    bson_buffer_init(&bb);

    if (!NULL_OR_EMPTY(hostkey))
    {
        bson_append_string(&bb, cfr_keyhash, hostkey);
    }

    bson_from_buffer(&query, &bb);

    Nova_Meter(&query, MONGO_DATABASE, buffer, bufsize);

    bson_destroy(&query);
}

/*****************************************************************************/

int Nova2PHP_LDAPAuthenticate(char *uri, char *basedn, char *passwd, bool starttls, const char **const errstr)
{
# ifdef HAVE_LIBLDAP
    return CfLDAPAuthenticate(uri, basedn, passwd, starttls, errstr);
# else
    *errstr = "LDAP support is disabled";
    return false;
# endif
}

/*****************************************************************************/
int Nova2PHP_LDAPGetSeveralAttributes(char *uri, char *user, char *basedn, char *filter, Rlist *names, char *scopes,
                                      char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                      int bufsize, const char **const errstr)
{
# ifdef HAVE_LIBLDAP
    return CfLDAP_JSON_GetSeveralAttributes(uri, user, basedn, filter, names, scopes, sec, passwd, starttls, page,
                                            linesperpage, buffer, bufsize, errstr);
# else
    *errstr = "LDAP support is disabled";
    return false;
# endif
}

/*****************************************************************************/
int Nova2PHP_LDAPGetSingleAttributeList(char *uri, char *user, char *basedn, char *filter, char *name, char *scopes,
                                        char *sec, char *passwd, bool starttls, int page, int linesperpage,
                                        char *buffer, int bufsize, const char **const errstr)
{
# ifdef HAVE_LIBLDAP
    return CfLDAP_JSON_GetSingleAttributeList(uri, user, basedn, filter, name, scopes, sec, passwd, starttls, page,
                                              linesperpage, buffer, bufsize, errstr);
# else
    *errstr = "LDAP support is not enabled in Nova";
    return false;
# endif
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

bool Nova2PHP_vitals_list(char *keyHash, char *buffer, int bufsize)
{
    mongo_connection dbconn;
    bool ret = false;
    char work[CF_MAXVARSIZE];
    time_t lastUpdate = 0;
    char hostName[CF_MAXVARSIZE], ipAddress[CF_MAXVARSIZE];
    HubVital *res, *hv;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    res = CFDB_QueryVitalsMeta(&dbconn, keyHash);

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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;

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
    mongo_connection dbconn;

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
    mongo_connection dbconn;

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

    Join(buffer, work, bufsize);

    ReplaceTrailingChar(buffer, ',', '\0');
    Join(buffer, "]", bufsize);
    DeleteItemList(spectrum);

    return true;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2PHP_promiselog(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                        HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_promiselog_test(hostkey, handle, causeRx, state, from, to, hostClassFilter, page, returnval, bufsize);
    }
# endif

    char buffer[CF_BUFSIZE] = { 0 }, jsonEscapedStr[CF_BUFSIZE] = { 0 }, header[CF_BUFSIZE] = { 0 };
    HubPromiseLog *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    bool truncated = false;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, causeRx, from, to, true, hostClassFilter, NULL);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPromiseLog);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\":{\"Host\":0,\"Promise Handle\":1,\"Report\":2,\"Time\":3}",
             page->totalResultCount, related_host_cnt);

    int headerLen = strlen(header);
    int noticeLen = strlen(CF_NOTICE_TRUNCATED);

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseLog *) rp->item;
        EscapeJson(hp->cause, jsonEscapedStr, sizeof(jsonEscapedStr));

        snprintf(buffer, sizeof(buffer),
                 "[ \"%s\",\"%s\",\"%s\",%ld ],",
                 hp->hh->hostname, hp->handle, jsonEscapedStr, hp->t);

        int margin = headerLen + noticeLen + strlen(buffer);

        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubPromiseLog);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

JsonElement *Nova2PHP_promiselog_summary(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                                         HostClassFilter *hostClassFilter, PageInfo *page)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_promiselog_summary_test(hostkey, handle, causeRx, state, from, to, hostClassFilter, page);
    }
# endif

    mongo_connection dbconn;
    if (!CFDB_Open(&dbconn))
    {
        return NULL;
    }

    HubQuery *hq = CFDB_QueryPromiseLogSummary(&dbconn, hostkey, state, handle, true, causeRx, from, to, true, hostClassFilter);

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

int Nova2PHP_value_report(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                          PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_value_report_test(hostkey, day, month, year, hostClassFilter, page, returnval, bufsize);
    }
# endif

    HubValue *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char buffer[CF_BUFSIZE] = { 0 }, header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubValue);

    snprintf(header, sizeof(header), "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\":{\"Host\":0,\"Summary of Day\":1,\"Value of Promises Kept\":2,\"Value of Repairs\":3,\"Loss for Promises Not Kept\":4}",
             page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubValue *) rp->item;

        snprintf(buffer, sizeof(buffer),
                 "[\"%s\",\"%s\",%.1lf,%.1lf,%.1lf ],",
                 hp->hh->hostname, hp->day, hp->kept, hp->repaired, hp->notkept);

        margin = headerLen + noticeLen + strlen(buffer);

        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubValue);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/
int Nova2PHP_get_value_graph(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                             int bufsize)
{
    HubValue *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
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

int Nova2PHP_software_report(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_software_report_test(hostkey, name, value, arch, regex, type, hostClassFilter, page, returnval,
                                             bufsize);
    }
# endif

    char buffer[CF_BUFSIZE] = { 0 }, header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    HubSoftware *hs;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex, hostClassFilter, true);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubSoftware);

    if(strcmp(type, cfr_software) == 0)
    {
        snprintf(header, sizeof(header),
                 "\"meta\":{\"count\" : %d, \"related\" : %d, "
                 "\"header\": {\"Host\":0,\"Package Name\":1,\"Version\":2,\"Architecture\":3,\"Last seen\":4"
                 "}", page->totalResultCount, related_host_cnt);
    }
    else
    {
        // Patches installed and patches available reports don't have timestamps
        snprintf(header, sizeof(header),
                 "\"meta\":{\"count\" : %d, \"related\" : %d, "
                 "\"header\": {\"Host\":0,\"Package Name\":1,\"Version\":2,\"Architecture\":3"
                 "}", page->totalResultCount, related_host_cnt);
    }

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hs = (HubSoftware *) rp->item;

        if(strcmp(type, cfr_software) == 0)
        {
            snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",\"%s\",\"%s\",%ld],",
                     hs->hh->hostname, hs->name, hs->version, Nova_LongArch(hs->arch), hs->t);
        }
        else
        {
            // Patches installed and patches available reports don't have timestamps
            snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",\"%s\",\"%s\"],",
                     hs->hh->hostname, hs->name, hs->version, Nova_LongArch(hs->arch));
        }
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubSoftware);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_classes_report(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                            char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_classes_report_test(hostkey, name, regex, hostClassFilter, page, returnval, bufsize);
    }
# endif

    char buffer[CF_BUFSIZE] = { 0 }, header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    HubClass *hc;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, now - (time_t)SECONDS_PER_WEEK, now, hostClassFilter, true);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubClass);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Class or Context\":1,\"in %% runs\":2,\"+/- %%\":3,\"Last occurred\":4"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hc = (HubClass *) rp->item;

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%lf,%lf,%ld],", hc->hh->hostname, hc->class, hc->prob*100.0, hc->dev*100.0, hc->t);
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }
    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubClass);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_classes_summary(char **classes, char *buf, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_classes_summary_test(classes, buf, bufsize);
    }
# endif

    mongo_connection dbconn;
    HubQuery *hq;
    HubClassSum *hc;
    HubHost *hh;
    Rlist *rp;
    char work[CF_MAXVARSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryClassSum(&dbconn, classes);

    StartJoin(buf, "{", bufsize);

    if (hq && hq->hosts)
    {

        Join(buf, "\"hosts\":[", bufsize);

        for (rp = hq->hosts; rp != NULL; rp = rp->next)
        {
            hh = (HubHost *) rp->item;
            snprintf(work, sizeof(work), "[\"%s\",\"%s\"]\n,", hh->hostname, hh->keyhash);

            if (!Join(buf, work, bufsize - 10))
            {
                break;
            }
        }

        ReplaceTrailingChar(buf, ',', '\0');
        EndJoin(buf, "]", bufsize);

        Join(buf, ",\n\"classes\":[", bufsize - 10);

        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            hc = (HubClassSum *) rp->item;
            snprintf(work, sizeof(work), "[\"%s\",%d]\n,", hc->class, hc->frequency);

            if (!Join(buf, work, bufsize - 10))
            {
                break;
            }
        }

        ReplaceTrailingChar(buf, ',', '\0');
        EndJoin(buf, "]", bufsize);
    }

    EndJoin(buf, "}", bufsize);

    DeleteHubQuery(hq, DeleteHubClassSum);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_vars_report(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex,
                         HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_vars_report_test(hostkey, scope, lval, rval, type, regex, hostClassFilter, page, returnval,
                                         bufsize);
    }
# endif

    char buffer[CF_BUFSIZE] = { 0 }, lscope[CF_MAXVARSIZE], jsonEscapedStr[CF_BUFSIZE] = { 0 };
    char rvalBuf[CF_MAXVARSIZE];
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    int first = true, countadded = false, found = false;
    int scope_record_count = 0, last_scope_record_count = 0, first_scope_record_count = 0;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, noticeLen = 0, headerLen = 0;
    int truncated = false;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, rval, type, regex, 0, time(NULL), hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    CountMarginRecordsVars(&(hq->records), page, &first_scope_record_count, &last_scope_record_count);
    PageRecords(&(hq->records), page, DeleteHubVariable);

    lscope[0] = '\0';

    snprintf(header, sizeof(header), "\"meta\":{\"count\":%d, \"related\" : %d ",
             page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        found = true;
        char typestr[CF_SMALLBUF];

        hv = (HubVariable *) rp->item;
        if (strcmp(lscope, hv->scope) != 0)
        {
            strcpy(lscope, hv->scope);

            if (strlen(buffer) > 0)
            {
                returnval[strlen(returnval) - 1] = '\0';
                snprintf(buffer, CF_BUFSIZE, "],\"count\":%d},", first ? first_scope_record_count : scope_record_count);
                Join(returnval, buffer, bufsize);       // end scope
                scope_record_count = 0;
                first = false;
            }

            snprintf(buffer, CF_BUFSIZE, "\"%s\":{"
                     "\"header\":{\"Host\":0,\"Type\":1,\"Name\":2,\"Value\":3,\"Last seen\":4},"
                     "\"data\":[", hv->scope);
            Join(returnval, buffer, bufsize);

        }

        switch (*hv->dtype)
        {
        case 's':
            snprintf(typestr, CF_SMALLBUF, "string");
            break;
        case 'i':
            snprintf(typestr, CF_SMALLBUF, "int");
            break;
        case 'r':
            snprintf(typestr, CF_SMALLBUF, "real");
            break;
        case 'm':
            snprintf(typestr, CF_SMALLBUF, "menu");
            break;
        }

        if (strlen(hv->dtype) == 2)
        {
            strcat(typestr, " list");
        }

        if (strlen(hv->dtype) > 1)      // list
        {
            PrintRlist(rvalBuf, sizeof(rvalBuf), hv->rval.item);
        }
        else
        {
            snprintf(rvalBuf, sizeof(rvalBuf), "%s", (char *) hv->rval.item);
        }

        EscapeJson(rvalBuf, jsonEscapedStr, sizeof(jsonEscapedStr));

        snprintf(buffer, CF_BUFSIZE,
                 "[\"%s\",\"%s\",\"%s\",\"%s\",%ld],", hv->hh->hostname, typestr, hv->lval, jsonEscapedStr, hv->t);

        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            countadded = true;
            break;
        }
        scope_record_count++;
    }

    if (!countadded)
    {
        returnval[strlen(returnval) - 1] = '\0';
        snprintf(buffer, CF_BUFSIZE, "],\"count\":%d}", last_scope_record_count);
        Join(returnval, buffer, bufsize);       // end scope
    }
    else if (first)
    {
        returnval[strlen(returnval) - 1] = ']';
    }
    else
    {
        returnval[strlen(returnval) - 1] = '\0';
    }

    if (found)
    {
        Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);
        Join(returnval, buffer, bufsize);
        EndJoin(returnval, "}}\n", bufsize);
    }
    else
    {
        snprintf(returnval, bufsize, "{ \"meta\": { \"count\": 0 }}");
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/
int Nova2PHP_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_compliance_report_test(hostkey, version, from, k, nk, rep, hostClassFilter, page, returnval, bufsize);
    }
# endif

    char buffer[CF_BUFSIZE];
    HubTotalCompliance *ht;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep, true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubTotalCompliance);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Policy Version\":1,\"%% Kept\":2,\"%% Repaired\":3,\"%% Not Kept\":4,\"Last verified\":5"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        ht = (HubTotalCompliance *) rp->item;

        snprintf(buffer, sizeof(buffer),
                 "[\"%s\",\"%s\",%d,%d,%d,%ld],",
                 ht->hh->hostname, ht->version, ht->kept, ht->repaired, ht->notkept, ht->t);
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_promises(char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter,
                                 HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page, char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_compliance_promises_test(hostkey, handle, status, regex, hostClassFilter, page, returnval, bufsize);
    }
# endif

    char buffer[CF_BUFSIZE];
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;

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
        hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, hostClassFilter, hostColourFilter);
    }
    else
    {
        hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), true, hostClassFilter);
    }

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPromiseCompliance);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Promise Handle\":1,\"Last Known State\":2,\"%% Runs Kept\":3,\"+/- %%\":4,\"Last verified\":5"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseCompliance *) rp->item;

        char E[CF_SMALLBUF];
        char D[CF_SMALLBUF];

        WriteDouble2Str_MP(hp->e, E, sizeof(E));
        WriteDouble2Str_MP(hp->d, D, sizeof(D));

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%ld],",
                 hp->hh->hostname, hp->handle, Nova_LongState(hp->status), E, D, hp->t);

        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);
    hq = NULL;

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubLastSeen *hl;
    HubQuery *hq;
    Rlist *rp;
    int count = 0;
    mongo_connection dbconn;
    char inout[CF_SMALLBUF];
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }
    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex, 0, time(NULL), true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubLastSeen);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Comms Initiated\":1,\"Remote host name\":2,\"Remote IP address\":3,\"Was Last Seen At\":4,\"Hrs ago\":5,\"Avg Comms Interval\":6,\"+/- hrs\":7,\"Remote host's key\":8"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    StartJoin(returnval, "{\"data\":[", bufsize);

    count += strlen(returnval);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hl = (HubLastSeen *) rp->item;

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

        snprintf(buffer, sizeof(buffer),
                 "[\"%s\",\"%s\",\"%s\",\"%s\",%ld,"
                 "\"%s\",\"%s\",\"%s\",\"%s\"],",
                 hl->hh->hostname, inout, hl->rhost->hostname, hl->rhost->ipaddr, hl->t,
                 hrsAgo, hrsAvg, hrsDev, hl->rhost->keyhash);
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubLastSeen);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/
int Nova2PHP_performance_report(char *hostkey, char *job, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                                char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubPerformance *hP;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    char jsonEscapedStr[CF_BUFSIZE] = { 0 };

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryPerformance(&dbconn, hostkey, job, regex, true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubPerformance);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Event\":1,\"Last completion time (seconds)\":2,\"Avg completion time (seconds)\":3,\"+/- seconds\":4,\"Last performed\":5"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hP = (HubPerformance *) rp->item;

        char Q[CF_SMALLBUF];
        char E[CF_SMALLBUF];
        char D[CF_SMALLBUF];

        WriteDouble2Str_MP(hP->q, Q, sizeof(Q));
        WriteDouble2Str_MP(hP->e, E, sizeof(E));
        WriteDouble2Str_MP(hP->d, D, sizeof(D));

        EscapeJson(hP->event, jsonEscapedStr, sizeof(jsonEscapedStr));

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%ld ],",
                 hP->hh->hostname, jsonEscapedStr, Q, E, D, hP->t);

        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubPerformance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_report(char *hostkey, char *file, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                           char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubSetUid *hS;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    char jsonEscapedStr[CF_BUFSIZE] = { 0 };

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubSetUid);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"File\":1" "}",
             page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hS = (HubSetUid *) rp->item;

        EscapeJson(hS->path, jsonEscapedStr, sizeof(jsonEscapedStr));

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\"],", hS->hh->hostname, jsonEscapedStr);
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubSetUid);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
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

int Nova2PHP_bundle_report(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page,
                           char *returnval, int bufsize)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_bundle_report_test(hostkey, bundle, regex, hostClassFilter, page, returnval, bufsize);
    }
# endif

    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq;

    if(lastRunOnly)
    {        
        hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter,hostColourFilter, false);
    }
    else
    {
        hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter, true);
    }

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubBundleSeen);

    char header[CF_BUFSIZE] = { 0 };
    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"%% Compliance\":3,\"Avg %% Compliance\":4,\"+/- %%\":5}",
             page->totalResultCount, related_host_cnt);

    int headerLen = strlen(header);
    int noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    int truncated = false;
    char buffer[CF_BUFSIZE] = { 0 };

    Rlist *rp;
    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubBundleSeen *hb = (HubBundleSeen *) rp->item;
        char bundleComp[CF_SMALLBUF];
        char bundleAvg[CF_SMALLBUF];
        char bundleDev[CF_SMALLBUF];

        WriteDouble2Str_MP(hb->bundlecomp * 100, bundleComp, sizeof(bundleComp));
        WriteDouble2Str_MP(hb->bundleavg * 100, bundleAvg, sizeof(bundleAvg));
        WriteDouble2Str_MP(hb->bundledev * 100, bundleDev, sizeof(bundleDev));

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%ld,\"%s\",\"%s\",\"%s\"],",
                 hb->hh->hostname, hb->bundle, hb->t,
                 bundleComp, bundleAvg, bundleDev);

        int margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);
    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to,
                                HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                                int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubFileChanges *hC;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    char jsonEscapedStr[CF_BUFSIZE] = { 0 };

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to, true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubFileChanges);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"File\":1,\"Change Detected at\":2"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hC = (HubFileChanges *) rp->item;

        EscapeJson(hC->path, jsonEscapedStr, sizeof(jsonEscapedStr));

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%ld ],",
                 hC->hh->hostname, jsonEscapedStr, hC->t);

        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubFileChanges);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                              int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubFileDiff *hd;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;
    char jsonEscapedStr[CF_BUFSIZE] = { 0 };

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, from, to, true, hostClassFilter);

    int related_host_cnt = RlistLen(hq->hosts);
    PageRecords(&(hq->records), page, DeleteHubFileDiff);

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : %d, "
             "\"header\": {\"Host\":0,\"File\":1,\"Change Detected at\":2,"
             "\"Change Details\":{\"index\":3,\"subkeys\":{\"plusminus\":0,\"line\":1,\"diff\":2}}"
             "}", page->totalResultCount, related_host_cnt);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hd = (HubFileDiff *) rp->item;

        EscapeJson(hd->path, jsonEscapedStr, sizeof(jsonEscapedStr));

        char diff[CF_BUFSIZE] = {0};

        Nova_FormatDiff(hd->diff, diff, sizeof(diff) - 1);

        snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%ld,%s],",
                 hd->hh->hostname, jsonEscapedStr, hd->t, diff);

        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubFileDiff);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
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

int CreateJsonHostOnlyReport(Rlist **records_p, PageInfo *page, char *returnval, int bufsize)
{
    assert(records_p);
    assert(page);
    assert(returnval);

    char buffer[CF_BUFSIZE] = { 0 };
    char header[CF_BUFSIZE] = { 0 };
    int headerLen = 0;
    int noticeLen = 0;
    bool truncated = false;

    Rlist *rp;
    HubHost *hh;

    PageRecords(records_p, page, DeleteHubHost);

    snprintf(header, sizeof(header), "\"meta\":{\"count\" : %d, \"related\" : %d",
             page->totalResultCount, page->totalResultCount);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = *records_p; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        int margin = headerLen + noticeLen + strlen(buffer);

        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);

    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return page->totalResultCount;
}
/****************************************************************************/

int Nova2PHP_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize)
{
    char buffer1[CF_BUFSIZE], buffer2[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int count1 = 0, count2 = 0, tmpsize1, tmpsize2;
    mongo_connection dbconn;
    bson query;
    bson_buffer bb;

/* BEGIN query document */

    if (hostkey && strlen(hostkey) != 0)
    {
        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, hostkey);
        bson_from_buffer(&query, &bb);
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

    DeleteHubQuery(hq, NULL);

    ReplaceTrailingChar(hostnameOut, ' ', '\0');
    ReplaceTrailingChar(ipaddrOut, ' ', '\0');

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_value_hosts(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                         PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;    

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, true, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubValue);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_software_hosts(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                            HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex, hostClassFilter, false);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubSoftware);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_classes_hosts(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{    
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    time_t now = time(NULL);
    HubQuery *hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, now - (time_t)SECONDS_PER_WEEK, now, hostClassFilter, false);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubClass);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_vars_hosts(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex,
                        HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, rval, type, regex, 0, time(NULL), hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_hosts(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep, false, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_promise_hosts(char *hostkey, char *handle, char *status, bool regex, HostClassFilter *hostClassFilter,
                           HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HubQuery *hq = NULL;

    if(lastRunOnly)
    {
        hq = CFDB_QueryWeightedPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, hostClassFilter, hostColourFilter);
    }
    else
    {
        hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, hostClassFilter);
    }

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_lastseen_hosts(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                            HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex, 0, time(NULL), false, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubLastSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_performance_hosts(char *hostkey, char *job, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                               int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryPerformance(&dbconn, hostkey, job, regex, false, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubPerformance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_setuid_hosts(char *hostkey, char *file, bool regex, HostClassFilter *hostClassFilter, PageInfo *page, char *returnval,
                          int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubSetUid);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_bundle_hosts(char *hostkey, char *bundle, bool regex, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, bool lastRunOnly, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq;

    if(lastRunOnly)
    {
        hq = CFDB_QueryWeightedBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter, hostColourFilter, false);
    }
    else
    {
        hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, hostClassFilter, true);
    }

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_filechanges_hosts(char *hostkey, char *file, bool regex, time_t from, time_t to,
                               HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to, false, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubFileChanges);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_filediffs_hosts(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to,
                             HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, from, to, false, hostClassFilter);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubFileDiff);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_promiselog_hosts(char *hostkey, char *handle, char *causeRx, PromiseLogState state, time_t from, time_t to,
                              HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, causeRx, from, to, false, hostClassFilter, NULL);

    CreateJsonHostOnlyReport(&(hq->hosts), page, returnval, bufsize);

    DeleteHubQuery(hq, DeleteHubPromiseLog);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2PHP_bundle_classes_used(PromiseFilter *promiseFilter, char *buffer, int bufsize)
{
    mongo_connection dbconn;
    Rlist *classList, *rp;
    Item *ip, *list = NULL;
    char work[CF_MAXVARSIZE], context[CF_MAXVARSIZE];
    int pid;
    char jsonEscapedStr[CF_BUFSIZE] = { 0 };

    if (!CFDB_Open(&dbconn))
    {
        return -1;
    }

    classList = CFDB_QueryBundleClasses(&dbconn, promiseFilter);

    if (classList)
    {
        for (rp = classList; rp != NULL; rp = rp->next)
        {
            PrependItem(&list, rp->item, NULL);
        }
        DeleteRlist(classList);
        IdempPrependItem(&list, "any", NULL);
    }

    if (list)
    {
        list = SortItemListNames(list);

        snprintf(buffer, bufsize, "[");

        for (ip = list; ip != NULL; ip = ip->next)
        {
            snprintf(context, CF_MAXVARSIZE, "class_contexts::%s", ip->name);
            pid = Nova_GetTopicIdForTopic(context);

            EscapeJson(ip->name, jsonEscapedStr, sizeof(jsonEscapedStr));
            snprintf(work, CF_MAXVARSIZE, "[%d,\"%s\"],", pid, jsonEscapedStr);

            if (!Join(buffer, work, bufsize))
            {
                break;
            }
        }

        ReplaceTrailingChar(buffer, ',', '\0');
        EndJoin(buffer, "]", bufsize);

        DeleteItemList(list);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2PHP_bundle_agent_goals(PromiseFilter *filter, char *buffer, int bufsize)
{
    mongo_connection dbconn;
    char work[CF_BUFSIZE];

    Nova_WebTopicMap_Initialize();

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hqBundles = CFDB_QueryPromiseBundles(&dbconn, filter);

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

        snprintf(work, sizeof(work), "[\"%s\",\"%s\",\"%s\",%s,\"%s\"],",
                 bundle->bundleType, bundle->bundleName, Nova_GetBundleComment(ToLowerStr(bundle->bundleName)), goals,
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

int Nova2PHP_bundle_list_by_bundle_usage(PromiseFilter *promiseFilter, char *bNameReferenced, char *buffer, int bufsize)
{
    mongo_connection dbconn;
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
/* Topic Map                                                                 */
/*****************************************************************************/

int Nova2PHP_docroot(char *buffer, int bufsize)
{
    Nova_WebTopicMap_Initialize();
    snprintf(buffer, bufsize, "%s", DOCROOT);
    return true;
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

JsonElement *Nova2PHP_show_topic(int id)
{
    char topic_name[CF_BUFSIZE], topic_id[CF_BUFSIZE], topic_context[CF_BUFSIZE];
    JsonElement *json_out = NULL;

    if (Nova_GetTopicByTopicId(id, topic_name, topic_id, topic_context))
    {
        json_out = JsonObjectCreate(2);
        JsonObjectAppendString(json_out, "topic", topic_name);
        JsonObjectAppendString(json_out, "context", topic_context);
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

void Nova2PHP_host_compliance_list_all(mongo_connection *conn, HostClassFilter *host_class_filter, PageInfo *page, char *buffer, int bufsize)
{
    Item *clist = NULL;
    char work[CF_BUFSIZE] = { 0 };
    int startIndex = 0;
    int endIndex = 0;
    int count = -1;;

    HostColourFilter *host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, HOST_COLOUR_GREEN_YELLOW_RED);
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
    mongo_connection dbconn;
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

    DeleteHubQuery(hq, NULL);

    ReplaceTrailingChar(buf, ',', '\0');

    EndJoin(buf, "]}", bufsize);

    return true;

}

/*****************************************************************************/

void Nova2PHP_host_compliance_list(mongo_connection *conn, char *colour, HostClassFilter *host_class_filter,
                                   PageInfo *page, char *buffer, int bufsize)
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

    HostColourFilter *host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, host_colour);
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

void Nova2PHP_get_host_colour(char *hostkey, char *buffer, int bufsize)
{
    HostColour colour = HOST_COLOUR_BLUE;
    CFDB_GetHostColour(hostkey, HOST_RANK_METHOD_COMPLIANCE, &colour);

    strncpy(buffer, Nova_HostColourToString(colour), bufsize);
}

/*****************************************************************************/

char *Nova2PHP_GetPromiseComment(char *handle)
{
    static char buffer[CF_BUFSIZE];
    mongo_connection dbconn;

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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
    char promiseeText[CF_MAXVARSIZE];
    char commentText[CF_MAXVARSIZE];
    char work[CF_BUFSIZE], escaped[CF_BUFSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hqPromise = CFDB_QueryPromises(&dbconn, filter);

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

    if (NULL_OR_EMPTY(hp->promisee))
    {
        strcpy(promiseeText, "None mentioned");
    }
    else
    {
        snprintf(promiseeText, sizeof(promiseeText), "%s", hp->promisee);
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
    mongo_cursor *cursor;
    bson_buffer bb;
    int found = false;
    Event e;

    if (!CFDB_Open(&dbconn))
    {        
        return JSONErrorFromId(ERRID_DBCONNECT);
    }

    bson query;

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, hostkey);
    bson_from_buffer(&query, &bb);

    bson field;

    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_netmeasure, 1);
    bson_from_buffer(&field, &bb);

    cursor = mongo_find(&dbconn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&field);

    cfapi_errid errid = ERRID_SUCCESS;

    if (mongo_cursor_next(cursor))      // not more than one record
    {
        bson_iterator it;

        bson_iterator_init(&it, cursor->current.data);

        while (bson_iterator_next(&it))
        {
            if (strcmp(bson_iterator_key(&it), cfr_netmeasure) == 0)
            {
                if (bson_iterator_bin_len(&it) == sizeof(e))
                {
                    memcpy(&e, bson_iterator_bin_data(&it), sizeof(e));
                    found = true;
                }
                else
                {
                    CfOut(cf_verbose, "", "!! Existing network measurement incorrect - ignoring");
                }
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
        JsonObjectAppendReal(jsonNetworkData, "speed", e.Q.expect);
        JsonObjectAppendReal(jsonNetworkData, "delta", sqrt(e.Q.var));
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

int Nova2PHP_countclasses(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, char *returnval,
                          int bufsize)
{
    HubQuery *hq;
    Rlist *rp;
    Item *order_results = NULL, *ip;
    int count = 0;
    mongo_connection dbconn;
    unsigned long bluehost_threshold;

    if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
    {
        return false;
    }

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    time_t now = time(NULL);
    hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, now - (time_t)bluehost_threshold, now, hostClassFilter, false);

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
    ComplianceSummaryGraph(NULL, policy, false, buffer, bufsize);
}

/*****************************************************************************/

void Nova_FormatDiff(const char *diffStr, char *returnval, int bufsize)
{
    snprintf(returnval, bufsize, "[");

    const char *sp;
    char tline[CF_BUFSIZE];

    for (sp = diffStr; *sp != '\0'; sp += strlen(tline) + 1)
    {
        int line = 0;
        char pm;

        char work[CF_BUFSIZE],
             diff[CF_BUFSIZE],
             jsonEscapedStr[CF_BUFSIZE] = {0};

        sscanf(sp, "%c,%d,%2047[^\n]", &pm, &line, diff);
        sscanf(sp, "%2047[^\n]", tline);

        EscapeJson(diff, jsonEscapedStr, sizeof(jsonEscapedStr));
        snprintf(work, sizeof(work), "[\"%c\",%d,\"%s\"],", pm, line, jsonEscapedStr);

        if (!Join(returnval, work, bufsize))
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');

    EndJoin(returnval, "]\n", bufsize);
}

/*****************************************************************************/

int Nova2PHP_report_description(char *reportName, char *returnval, int bufsize)
{
    int pid;
    char topic_comment[CF_BUFSIZE] = { 0 };
    int found = false;

    pid = Nova_GetTopicIdForTopic(reportName);
    if (pid > 0)
    {
        topic_comment[0] = '\0';
        found = Nova_GetReportDescription(pid, topic_comment, sizeof(topic_comment));
    }

    if (found)
    {
        snprintf(returnval, bufsize, "%s", topic_comment);
        return true;
    }

    snprintf(returnval, bufsize, "(no description for %s report found in topic map)", reportName);
    return false;
}

/*****************************************************************************/
/*                       Content-Driven Policies                             */
/*****************************************************************************/

void Nova2PHP_cdp_reportnames(char *buf, int bufSz)
{
    int i;
    char work[CF_SMALLBUF] = { 0 };

    buf[0] = '[';

    for (i = 0; i < cdp_unknown; i++)
    {
        snprintf(work, sizeof(work), "[\"%s\",\"%s\"],", CDP_REPORTS[i][0], CDP_REPORTS[i][1]);

        if (!Join(buf, work, bufSz))
        {
            break;
        }
    }

    ReplaceTrailingChar(buf, ',', '\0');
    EndJoin(buf, "]", bufSz);
}

/*****************************************************************************/

cdp_report CdpReportNameToType(char *reportName)
{
    int i;

    for (i = 0; i < cdp_unknown; i++)
    {
        if (strcmp(reportName, CDP_REPORTS[i][0]) == 0)
        {
            return (cdp_report) i;
        }
    }

    return cdp_unknown;
}

/*****************************************************************************/

int Nova2PHP_cdp_report(char *hostkey, char *reportName, PageInfo *page, char *buf, int bufSz)
{
    Item *promises = { 0 }, *hosts = { 0 };
    Item *ip = { 0 }, *ip2 = { 0 };
    mongo_connection dbconn;
    char handle[CF_MAXVARSIZE] = { 0 };
    char host[CF_MAXVARSIZE] = { 0 };
    char hostKeyHash[CF_MAXVARSIZE] = { 0 };
    char thenStr[CF_SMALLBUF] = { 0 };
    char statusStr[CF_SMALLBUF];
    char lastChangeStr[CF_SMALLBUF];
    char *urlReportName = "";
    time_t then = 0, now;
    char attributes[CF_MAXVARSIZE] = { 0 };
    char row[CF_MAXVARSIZE] = { 0 };
    int ret = false;
    cdp_report cdpType;
    int count = 0;
    int timewarn = false;
    int startIndex, endIndex;

    memset(buf, 0, bufSz);
    now = time(NULL);

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    startIndex = page->resultsPerPage * (page->pageNum - 1);
    endIndex = (page->resultsPerPage * page->pageNum) - 1;

    cdpType = CdpReportNameToType(reportName);

    switch (cdpType)
    {
    case cdp_acls:
        promises = CFDB_QueryCdpAcls(&dbconn, ",");
        break;

    case cdp_commands:
        promises = CFDB_QueryCdpCommands(&dbconn, ",");
        break;

    case cdp_services:
        promises = CFDB_QueryCdpServices(&dbconn, ",");
        break;

    case cdp_filechanges:
        promises = CFDB_QueryCdpPromiser(&dbconn, ",", cfp_cdp_bundle_filechanges, "files");
        reportName = cfr_filechanges;
        urlReportName = "File change log";
        break;

    case cdp_filediffs:
        promises = CFDB_QueryCdpPromiser(&dbconn, ",", cfp_cdp_bundle_filediffs, "files");
        reportName = cfr_filediffs;
        urlReportName = "File change diffs";
        break;

    case cdp_registry:
        promises = CFDB_QueryCdpRegistry(&dbconn, ",");
        break;

    default:
        snprintf(buf, bufSz, "Unknown Content-Driven Policy Type");
        CFDB_Close(&dbconn);
        return false;
    }

    if (promises)
    {
        snprintf(buf, bufSz, "{\"data\":[");
        for (ip = promises; ip != NULL; ip = ip->next)  // join policy with host reports
        {
            sscanf(ip->name, "%128[^,],%512[^$]", handle, attributes);
            hosts = CFDB_QueryCdpCompliance(&dbconn, handle);

            if (hosts)
            {
                for (ip2 = hosts; ip2 != NULL; ip2 = ip2->next, count++)
                {
                    if (count >= startIndex && (count <= endIndex || endIndex < 0))
                    {
                        sscanf(ip2->name, "%512[^;];%128[^;];%8[^;];%ld[^$]", hostKeyHash, host, statusStr, &then);
                        // todo  depricate timewarn, add coloring on php side
                        timewarn = Nova_TimeWarn(now, then, 900, thenStr, sizeof(thenStr));

                        row[0] = '\0';
                        lastChangeStr[0] = '\0';

                        char fileChangePath[CF_MAXVARSIZE];

                        switch (cdpType)        // include special fields
                        {
                        case cdp_filechanges:
                        case cdp_filediffs:

                            sscanf(attributes, "\"%512[^\",]", fileChangePath);

                            CFDB_QueryLastFileChange(&dbconn, hostKeyHash, reportName, fileChangePath, lastChangeStr,
                                                     sizeof(lastChangeStr));

                            snprintf(row, sizeof(row), "[\"%s\",\"%s\",%s,\"%s\",\"%s\",\"%s\",\"%s\",%d],",
                                     hostKeyHash, host, attributes, lastChangeStr, Nova_LongState(*statusStr),
                                     thenStr, urlReportName, timewarn);
                            break;

                        default:
                            snprintf(row, sizeof(row), "[\"%s\",\"%s\",%s,\"%s\",\"%s\",%d],",
                                     hostKeyHash, host, attributes, Nova_LongState(*statusStr), thenStr, timewarn);
                            break;
                        }
                        if (!*row)
                        {
                            continue;
                        }
                        if (!Join(buf, row, bufSz))
                        {
                            break;
                        }
                    }
                }

                DeleteItemList(hosts);
            }
        }

        ReplaceTrailingChar(buf, ',', '\0');

        snprintf(row, sizeof(row), "],\"meta\":{\"count\" : %d," "\"header\": %s}}", count, GetCdpTableHeader(cdpType));
        EndJoin(buf, row, bufSz);
        DeleteItemList(promises);
        ret = true;
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return ret;
}

/*****************************************************************************/

char *GetCdpTableHeader(cdp_report cdpType)
{
    switch (cdpType)
    {
    case cdp_acls:
        return
            "{\"hostkey\":0,\"Host\":1,\"Path\":2,\"Permission (ACL)\":3,\"Owner\":4,\"Action\":5,\"Class expression\":6,\"State\":7,\"Last checked\":8,\"timewarn\":9}";
    case cdp_commands:
        return
            "{\"hostkey\":0,\"Host\":1,\"Command\":2,\"Failclass\":3,\"Action\":4,\"Class expression\":5,\"State\":6,\"Last checked\":7,\"timewarn\":8}";
    case cdp_filechanges:
    case cdp_filediffs:
        return
            "{\"hostkey\":0,\"Host\":1,\"Path\":2,\"Class expression\":3,\"Last change detected\":4,\"State\":5,\"Last checked\":6,\"urlReport\":7,\"timewarn\":8}";
    case cdp_registry:
        return
            "{\"hostkey\":0,\"Host\":1,\"Key\":2,\"Value\":3,\"Action\":4,\"Class expression\":5,\"State\":6,\"Last checked\":7,\"timewarn\":8}";
    case cdp_services:
        return
            "{\"hostkey\":0,\"Host\":1,\"Service name\":2,\"Runstatus\":3,\"Action\":4,\"Class expression\":5,\"State\":6,\"Last checked\":7,\"timewarn\":8}";
    default:
        return "\"Undefined Special Policy Header\"";
    }
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
    mongo_connection dbconn;

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
    mongo_connection dbconn;

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
    mongo_connection dbconn;
    bson_buffer bb;

    *out = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    bson query;

    bson_buffer_init(&bb);
    BsonAppendHostClassFilter(&bb, hostClassFilter);
    bson_from_buffer(&query, &bb);

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
    mongo_connection dbconn;
    mongo_cursor *cursor;
    bson_buffer bb;
    bson query, fields;

    *out = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

/* { env: $environment } */
    bson_buffer_init(&bb);
    BsonAppendHostClassFilter(&bb, hostClassFilter);
    bson_append_string(&bb, cfr_environment, environment);
    bson_from_buffer(&query, &bb);

/* { kH: 1 } */
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_keyhash, 1);
    bson_from_buffer(&fields, &bb);

    cursor = mongo_find(&dbconn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    CFDB_Close(&dbconn);

    while (mongo_cursor_next(cursor))
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
    mongo_connection dbconn;
    bson_buffer bb;
    bson query, fields;
    bson result;
    bool res;
    bson_iterator i;
    char *environment = NULL;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

/* { kH: $hostkey } */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, hostkey);
    bson_from_buffer(&query, &bb);

/* { env: 1 } */
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_environment, 1);
    bson_from_buffer(&fields, &bb);

    res = mongo_find_one(&dbconn, MONGO_DATABASE, &query, &fields, &result);

    bson_destroy(&query);
    bson_destroy(&fields);

    if (res)
    {
        if (bson_find(&i, &result, cfr_environment))
        {
            environment = xstrdup(bson_iterator_string(&i));
        }
        bson_destroy(&result);
    }

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

    mongo_connection dbconn;
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
    mongo_connection dbconn;
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
    mongo_connection dbconn;
    bson query;
    bson_buffer bb;

    if (NULL_OR_EMPTY(hostkey))
    {
        return false;
    }

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfn_keyhash, hostkey);
    bson_append_int(&bb, cfn_reporttype, CFREPORT_HOSTS);
    bson_from_buffer(&query, &bb);

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

void Nova2PHP_enterprise_version(char *buf, int bufsize)
{
    char *name;
    const char *version;
    cfapi_errid retErrid = ERRID_SUCCESS;

# ifdef HAVE_CONSTELLATION

    retErrid = Constellation_CheckLicenseInDB();

    switch (retErrid)
    {
    case ERRID_SUCCESS:
        name = "Constellation";
        version = Constellation_Version();
        break;

    case ERRID_CONSTELLATION_LICENSE:
        retErrid = ERRID_SUCCESS;       /* not error to have Nova only */
        /* fallthrough */
    default:
        name = "Enterprise";
        version = Nova_Version();
        break;
    }

# else

    name = "Enterprise";
    version = Nova_Version();

# endif

    char errbuf[CF_MAXVARSIZE];

    snprintf(buf, bufsize, "{ \"name\": \"%s\", \"version\" : \"%s\", %s}",
             name, version, FormatErrorJsonAttribute(errbuf, sizeof(errbuf), retErrid));

}

/*****************************************************************************/

int Nova2PHP_community_version(char *buf, int bufsize)
{
    strlcpy(buf, NameVersion(), bufsize);
    return true;
}

/*****************************************************************************/

int Nova2PHP_promise_list(PromiseFilter *promiseFilter, char *returnval, int bufsize, PageInfo *page)
{
# ifndef NDEBUG
    if (IsEnvMissionPortalTesting())
    {
        return Nova2PHP_promise_list_test(promiseFilter, returnval, bufsize);
    }
# endif

    mongo_connection dbconn;
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

    hq = CFDB_QueryPromises(&dbconn, promiseFilter);

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

    if (!CFDB_GetBlueHostThreshold(&threshold))
    {
        return false;
    }

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

char *FormatSingletonErrorJson(char *out, int outSz, cfapi_errid errid)
{
    out[0] = '{';
    FormatErrorJsonAttribute(out + 1, outSz - 1, errid);
    EndJoin(out, "}", outSz);

    return out;
}

/*****************************************************************************/

void EndJsonBuffer(char *buf, int bufsize, cfapi_errid errid)
{
    char work[CF_MAXVARSIZE];

    ReplaceTrailingChar(buf, ',', '\0');
    strlcat(buf, "], ", bufsize);

    FormatErrorJsonAttribute(work, sizeof(work), errid);
    EndJoin(buf, work, bufsize);
    EndJoin(buf, "}", bufsize);
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

