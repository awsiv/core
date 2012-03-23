/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "scorecards.h"
#include "bson_lib.h"
#include "string_lib.h"

#if defined(HAVE_LIBMONGOC)
#include "db_query.h"

#include <assert.h>

HostColourFilter *NewHostColourFilter(HostRankMethod method, HostColour colours)
{
    long bluehost_threshold = 0;
    if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
    {
        return NULL;
    }

    HostColourFilter *filter = xmalloc(sizeof(HostColourFilter));
    filter->method = method;
    filter->colour = colours;

    time_t now = time(NULL);
    filter->blue_time_horizon = (time_t)(now - bluehost_threshold);

    return filter;
}

const char *Nova_HostColourToString(HostColour colour)
{
    switch (colour)
    {
    case HOST_COLOUR_BLUE:
        return "blue";
    case HOST_COLOUR_RED:
        return "red";
    case HOST_COLOUR_YELLOW:
        return "yellow";
    case HOST_COLOUR_GREEN:
        return "green";
    case HOST_COLOUR_GREEN_YELLOW_RED:
        return "green_yellow_red";
    case HOST_COLOUR_BLACK:
        return "black";
    default:
        return "unknown";
    }
}

HostColour HostColourFromString(const char *colour)
{
    if (StringSafeCompare(colour, "blue") == 0)
    {
        return HOST_COLOUR_BLUE;
    }
    else if (StringSafeCompare(colour, "red") == 0)
    {
        return HOST_COLOUR_RED;
    }
    else if (StringSafeCompare(colour, "yellow") == 0)
    {
        return HOST_COLOUR_YELLOW;
    }
    else if (StringSafeCompare(colour, "green") == 0)
    {
        return HOST_COLOUR_GREEN;
    }
    else if (StringSafeCompare(colour, "green_yellow_red") == 0)
    {
        return HOST_COLOUR_GREEN_YELLOW_RED;
    }
    else if (StringSafeCompare(colour, "black") == 0)
    {
        return HOST_COLOUR_BLACK;
    }
    else
    {
        assert(false && "Could not parse host colour");
        return HOST_COLOUR_GREEN_YELLOW_RED;
    }
}

void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize)
// Read the cached compliance summary (either from Hub or
// Constellation DB)
{
    char work[CF_BUFSIZE];
    mongo_connection dbconn;
    HubCacheTotalCompliance *tc;
    HubQuery *hq;
    double kept, repaired, notkept, nodata;
    time_t now = time(NULL), start, one_week = (time_t) SECONDS_PER_WEEK;
    int i, slot, count;
    char buf[CF_MAXVARSIZE];

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    if (constellation)
    {
# ifdef HAVE_CONSTELLATION

        hq = CFDBCon_QueryComp(&dbconn, hubKeyHash, policy, now - SECONDS_PER_WEEK);

# else
        CfOut(cf_error, "",
              "!! Trying to get constellation data from data ComplianceSummaryGraph() without libcfconstellation");
        CFDB_Close(&dbconn);
        return;
# endif

    }
    else                        // Nova
    {
        hq = CFDB_QueryCachedTotalCompliance(&dbconn, policy, now - SECONDS_PER_WEEK);
    }

    CFDB_Close(&dbconn);

    snprintf(buffer, bufsize, "[");

    start = GetShiftSlotStart(now - one_week);

    if (start == -1)
    {
        CfOut(cf_error, "", "!! Could not get start of interval");
        DeleteHubQuery(hq, DeleteHubCacheTotalCompliance);
        return;
    }

    for (i = 0; i < 28; start += CF_SHIFT_INTERVAL, i++)
    {
        slot = GetShiftSlot(start);

        tc = GetHubCacheTotalComplianceSlot(hq->records, slot);

        if (tc && tc->hostCount > 0)
        {
            kept = tc->kept;
            repaired = tc->repaired;
            notkept = tc->notkept;
            nodata = 0;
            count = tc->hostCount;
        }
        else
        {
            kept = 0;
            repaired = 0;
            notkept = 0;
            nodata = 100.0;
            count = 0;
        }

        UtcShiftInterval(start, buf, sizeof(buf));

        snprintf(work, CF_BUFSIZE,
                 "{ \"title\": \"%s\", \"start\": %ld, \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf, \"nodata\": %lf, \"count\": %d },",
                 buf, start, i, kept, repaired, notkept, nodata, count);

        if (!Join(buffer, work, bufsize))
        {
            break;
        }
    }

    buffer[strlen(buffer) - 1] = ']';

    DeleteHubQuery(hq, DeleteHubCacheTotalCompliance);
}

/*****************************************************************************/

void Nova_Meter(bson *query, char *db, char *buffer, int bufsize)
{
    double kept = 0, repaired = 0;
    double kept_week = 0, kept_day = 0, kept_hour = 0, kept_comms = 0, kept_anom = 0, kept_perf = 0, kept_other = 0;
    double rep_week = 0, rep_day = 0, rep_hour = 0, rep_comms = 0, rep_anom = 0, rep_perf = 0, rep_other = 0;
    double num_week = 0, num_day = 0, num_hour = 0, num_comms = 0, num_anom = 0, num_perf = 0, num_other = 0;
    HubMeter *hm;
    HubQuery *hq;
    mongo_connection dbconn;
    Rlist *rp;

    strcpy(buffer, "[");

    if (!CFDB_Open(&dbconn))
    {
        Join(buffer, "]", bufsize);
        return;
    }

    hq = CFDB_QueryMeter(&dbconn, query, db);

    CFDB_Close(&dbconn);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hm = (HubMeter *) rp->item;
        CfDebug("Meter result: (%c) %lf,%lf,%lf\n", hm->type, hm->kept, hm->repaired, hm->notkept);
        CfDebug("found on (%s=%s=%s)\n", hm->hh->keyhash, hm->hh->hostname, hm->hh->ipaddr);

        kept = hm->kept;
        repaired = hm->repaired;

        switch (hm->type)
        {
        case cfmeter_week:
            kept_week += kept;
            rep_week += repaired;
            num_week++;
            break;

        case cfmeter_hour:
            kept_hour += kept;
            rep_hour += repaired;
            num_hour++;
            break;

        case cfmeter_day:
            kept_day += kept;
            rep_day += repaired;
            num_day++;
            break;

        case cfmeter_perf:
            kept_perf += kept;
            rep_perf += repaired;
            num_perf++;
            break;

        case cfmeter_comms:
            kept_comms += kept;
            rep_comms += repaired;
            num_comms++;
            break;

        case cfmeter_anomaly:
            kept_anom += kept;
            rep_anom += repaired;
            num_anom++;
            break;

        case cfmeter_other:
            kept_other += kept;
            rep_other += repaired;
            num_other++;
            break;
        }
    }

    if (hq->records != NULL)
    {
        if (num_week > 0)
        {
            Nova_BarMeter(1, kept_week / num_week, rep_week / num_week, "Week", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_day > 0)
        {
            Nova_BarMeter(2, kept_day / num_day, rep_day / num_day, "Day", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_hour > 0)
        {
            Nova_BarMeter(3, kept_hour / num_hour, rep_hour / num_hour, "Hour", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_perf > 0)
        {
            Nova_BarMeter(4, kept_perf / num_perf, rep_perf / num_perf, "Perf", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_other > 0)
        {
            Nova_BarMeter(5, kept_other / num_other, rep_other / num_other, "Chng", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_comms > 0)
        {
            Nova_BarMeter(6, kept_comms / num_comms, rep_comms / num_comms, "Seen", buffer, bufsize);
            strcat(buffer, ",");
        }
        if (num_anom > 0)
        {
            Nova_BarMeter(7, kept_anom / num_anom, rep_anom / num_anom, "Anom", buffer, bufsize);
        }
        else
        {
            buffer[strlen(buffer) - 1] = '\0';
        }
    }

// Clean up

    DeleteHubQuery(hq, DeleteHubMeter);
    Join(buffer, "]", bufsize);
}

/*****************************************************************************/

int Nova_GetHostColour(char *lkeyhash, HostRankMethod method, HostColour *result)
{
    if (lkeyhash == NULL)
    {
        return -1;
    }

    unsigned long bluehost_threshold;
    if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
    {
        return -1;
    }
    time_t now = time(NULL);

    char *score_field = HostRankMethodToMongoCode(method);

    mongo_connection conn;
    if (!CFDB_Open(&conn))
    {
        return -1;
    }

    /* query */
    bson_buffer bb;
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, lkeyhash);
    bson query;
    bson_from_buffer(&query, &bb);

   /* result document */
    bson_buffer_init(&bb);
    bson_append_int(&bb, cfr_day, 1);
    bson_append_int(&bb, score_field, 1);
    bson_append_int(&bb, cfr_is_black, 1);
    bson field;
    bson_from_buffer(&field, &bb);

    bson out;
    bson_bool_t found = mongo_find_one(&conn, MONGO_DATABASE, &query, &field, &out);

    bson_destroy(&query);
    bson_destroy(&field);

    /* if no records are found it's seen are host with unknown state (blue) */
    if (!found)
    {
        *result = HOST_COLOUR_BLUE;
    }
    else
    {
        time_t then = 0;
        BsonTimeGet(&out, cfr_day, &then);

        bool is_black;
        if ((is_black = BsonBoolGet(&out, cfr_is_black)) == -1)
        {
            is_black = false;
        }

        int score = 0;
        if (BsonIntGet(&out, score_field, &score))
        {
            *result = HostColourFromScore(now, then, bluehost_threshold, score, is_black);
        }
        else
        {
            // special case: could not find the score, return blue
            *result = HOST_COLOUR_BLUE;
        }
    }

    free(score_field);
    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return 0;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

char *HostRankMethodToMongoCode(HostRankMethod method)
{
    char *score_field = NULL;

    switch (method)
    {
        case HOST_RANK_METHOD_COMPLIANCE:
            xasprintf(&score_field, "%s", cfr_score_comp);
            break;

        case HOST_RANK_METHOD_ANOMALY:
            xasprintf(&score_field, "%s", cfr_score_anom);
            break;

        case HOST_RANK_METHOD_PERFORMANCE:
            xasprintf(&score_field, "%s", cfr_score_perf);
            break;

        case HOST_RANK_METHOD_LASTSEEN:
            xasprintf(&score_field, "%s", cfr_score_lastseen);
            break;

        case HOST_RANK_METHOD_MIXED:
            xasprintf(&score_field, "%s", cfr_score_mixed);
            break;

        default:
            xasprintf(&score_field, "%s", cfr_score_comp);
            break;
    }

    return score_field;
}

/*****************************************************************************/

int Nova_GetComplianceScore(HostRankMethod method, double *k, double *r)
{
    int result = CF_GREEN;
    double kav, rav, notkept;

    switch (method)
    {
    default:
    case HOST_RANK_METHOD_COMPLIANCE:

        notkept = 100 - k[meter_compliance_hour] - r[meter_compliance_hour];

        if (notkept > 20)       // 20% of promises are not kept => RED!
        {
            result = CF_RED_THRESHOLD + 100 + notkept;  // Make red override all variations in amber/green
        }

        if (r[meter_compliance_hour] > 20)      // If more than 20% of promises were repaired => AMBER
        {
            result = CF_AMBER_THRESHOLD + 100 + r[meter_compliance_hour];
        }

        result -= k[meter_compliance_hour];     // Adjust the Green Value relative
        break;

    case HOST_RANK_METHOD_ANOMALY:

        notkept = 100 - k[meter_anomalies_day] - r[meter_anomalies_day];

        // If red or yellow, add the "badness" to the base threshold
        // to ensure the worst host is ranked on top. We also add
        // 100 in case k[meter_anomalies_day] pulls us back under the
        // threshold

        if (notkept > 20)
        {
            result = CF_RED_THRESHOLD + 100 + notkept;
        }

        if (r[meter_anomalies_day] > 20)
        {
            result = CF_AMBER_THRESHOLD + 100 + r[meter_anomalies_day];
        }

        // We want worse hosts to have higher scores so they appear on top
        // so subtract what is "good" about kept promises

        result -= k[meter_anomalies_day];
        break;

    case HOST_RANK_METHOD_PERFORMANCE:

        notkept = 100 - k[meter_perf_day] - r[meter_perf_day];

        if (notkept > 20)
        {
            result = CF_RED_THRESHOLD + 100 + notkept;
        }

        if (r[meter_perf_day] > 20)
        {
            result = CF_AMBER_THRESHOLD + 100 + r[meter_perf_day];
        }

        result -= k[meter_perf_day];
        break;

    case HOST_RANK_METHOD_LASTSEEN:

        notkept = 100 - k[meter_comms_hour] - r[meter_comms_hour];

        if (notkept > 20)
        {
            result = CF_RED_THRESHOLD + 100 + notkept;
        }

        if (r[meter_comms_hour] > 20)
        {
            result = CF_AMBER_THRESHOLD + 100 + r[meter_comms_hour];
        }

        result -= k[meter_comms_hour];
        break;

    case HOST_RANK_METHOD_MIXED:
        kav = (k[meter_comms_hour] + k[meter_compliance_hour] + k[meter_anomalies_day]) / 3;
        rav = (r[meter_comms_hour] + r[meter_compliance_hour] + r[meter_anomalies_day]) / 3;

        notkept = 100 - kav - rav;

        if (notkept > 20)
        {
            result = CF_RED_THRESHOLD + 100 + notkept;
        }

        if (rav > 20)
        {
            result = CF_AMBER_THRESHOLD + 100 + rav;
        }

        result -= kav;
        break;

    }

    return result;
}

/*****************************************************************************/

HostColour HostColourFromScore(time_t now, time_t last_report, time_t blue_horizon, int score, bool is_black)
{
    if (last_report < (now - blue_horizon))
    {
        return HOST_COLOUR_BLUE;
    }
    if (is_black)
    {
        return HOST_COLOUR_BLACK;
    }
    else if (score < CF_AMBER_THRESHOLD)
    {
        return HOST_COLOUR_GREEN;
    }
    else if (score >= CF_AMBER_THRESHOLD && score < CF_RED_THRESHOLD)
    {
        return HOST_COLOUR_YELLOW;
    }
    else
    {
        return HOST_COLOUR_RED;
    }
}

HostColour HostColourFromScoreForConnectedHost(int score)
{
    return HostColourFromScore(1, 1, 1, score, false);
}

/*****************************************************************************/

void Nova_BarMeter(int pos, double kept, double rep, char *name, char *buffer, int bufsize)
{
    char work[CF_BUFSIZE];

    snprintf(work, CF_BUFSIZE,
             "{ \"title\": \"%s\", \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf }", name, pos,
             kept, rep, 100 - kept - rep);
    Join(buffer, work, bufsize);
}

#endif
