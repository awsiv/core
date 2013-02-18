/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "scorecards.h"

#include "bson_lib.h"
#include "string_lib.h"
#include "granules.h"
#include "files_names.h"
#include "conversion.h"
#include "bson_hub_conversion.h"
#include "cfstream.h"
#include "install.h"

#include "db_query.h"

#include <assert.h>

PromiseContextMode PromiseContextModeFromString(const char *mode)
{
    if (mode == NULL)
    {
        return PROMISE_CONTEXT_MODE_ALL;
    }

    if (StringSafeCompare("user", mode) == 0)
    {
        return PROMISE_CONTEXT_MODE_USER;
    }
    else if (StringSafeCompare("system", mode) == 0)
    {
        return PROMISE_CONTEXT_MODE_INTERNAL;
    }
    else
    {
        return PROMISE_CONTEXT_MODE_ALL;
    }
}


/*****************************************************************************/

HostColourFilter *NewHostColourFilter(HostRankMethod method, HostColour colours,
                                      PromiseContextMode promise_context)
{
    long bluehost_threshold = 0;
    CFDB_GetBlueHostThreshold(&bluehost_threshold);

    HostColourFilter *filter = xmalloc(sizeof(HostColourFilter));
    filter->method = method;
    filter->colour = colours;
    filter->promise_context = promise_context;

    time_t now = time(NULL);
    filter->blue_time_horizon = (time_t)(now - bluehost_threshold);

    return filter;
}

/*****************************************************************************/

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
    case HOST_COLOUR_NONE:
        return "none";
    default:
        return "unknown";
    }
}

/*****************************************************************************/

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
    else if (StringSafeCompare(colour, "none") == 0)
    {
        return HOST_COLOUR_NONE;
    }
    else
    {
        assert(false && "Could not parse host colour");
        return HOST_COLOUR_GREEN_YELLOW_RED;
    }
}

/*****************************************************************************/

void ComplianceSummaryGraph(char *hubKeyHash, char *policy, char *buffer, int bufsize)
{
    char work[CF_BUFSIZE];
    EnterpriseDB dbconn;
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

    hq = CFDB_QueryCachedTotalCompliance(&dbconn, policy, now - SECONDS_PER_WEEK);

    CFDB_Close(&dbconn);

    snprintf(buffer, bufsize, "[");

    start = GetShiftSlotStart(now - one_week);

    if (start == -1)
    {
        CfOut(OUTPUT_LEVEL_ERROR, "", "!! Could not get start of interval");
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

static void Nova_BarMeter(int pos, double kept, double rep, char *name, char *buffer, int bufsize)
{
    char work[CF_BUFSIZE];

    snprintf(work, CF_BUFSIZE,
             "{ \"title\": \"%s\", \"position\": %d, \"kept\": %lf, \"repaired\": %lf, \"notkept\": %lf }", name, pos,
             kept, rep, 100 - kept - rep);
    Join(buffer, work, bufsize);
}

void Nova_Meter(bson *query, char *db, char *buffer, int bufsize, PromiseContextMode promise_context)
{
    double kept = 0, repaired = 0;
    double kept_week = 0, kept_day = 0, kept_hour = 0, kept_comms = 0, kept_anom = 0, kept_perf = 0, kept_other = 0;
    double rep_week = 0, rep_day = 0, rep_hour = 0, rep_comms = 0, rep_anom = 0, rep_perf = 0, rep_other = 0;
    double num_week = 0, num_day = 0, num_hour = 0, num_comms = 0, num_anom = 0, num_perf = 0, num_other = 0;
    HubMeter *hm;
    HubQuery *hq;
    EnterpriseDB dbconn;
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
            if (promise_context == PROMISE_CONTEXT_MODE_ALL)
            {
                kept_week += kept;
                rep_week += repaired;
                num_week++;
            }
            break;

        case cfmeter_week_user:
            if (promise_context == PROMISE_CONTEXT_MODE_USER)
            {
                kept_week += kept;
                rep_week += repaired;
                num_week++;
            }
            break;

        case cfmeter_week_internal:
            if (promise_context == PROMISE_CONTEXT_MODE_INTERNAL)
            {
                kept_week += kept;
                rep_week += repaired;
                num_week++;
            }
            break;

        case cfmeter_hour:
            if (promise_context == PROMISE_CONTEXT_MODE_ALL)
            {
                kept_hour += kept;
                rep_hour += repaired;
                num_hour++;
            }
            break;

        case cfmeter_hour_user:
            if (promise_context == PROMISE_CONTEXT_MODE_USER)
            {
                kept_hour += kept;
                rep_hour += repaired;
                num_hour++;
            }
            break;

        case cfmeter_hour_internal:
            if (promise_context == PROMISE_CONTEXT_MODE_INTERNAL)
            {
                kept_hour += kept;
                rep_hour += repaired;
                num_hour++;
            }
            break;

        case cfmeter_day:
            if (promise_context == PROMISE_CONTEXT_MODE_ALL)
            {
                kept_day += kept;
                rep_day += repaired;
                num_day++;
            }
            break;

        case cfmeter_day_user:
            if (promise_context == PROMISE_CONTEXT_MODE_USER)
            {
                kept_day += kept;
                rep_day += repaired;
                num_day++;
            }
            break;

        case cfmeter_day_internal:
            if (promise_context == PROMISE_CONTEXT_MODE_INTERNAL)
            {
                kept_day += kept;
                rep_day += repaired;
                num_day++;
            }
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

int HostComplianceScore(double kept, double repaired)
{
    int result = CF_GREEN;
    double notkept = 100.0 - kept - repaired;

    if (notkept > 20)
    {
        result = CF_RED_THRESHOLD + 100 + notkept;
    }

    if (repaired > 20)
    {
        result = CF_AMBER_THRESHOLD + 100 + repaired;
    }

    result -= kept;

    return result;
}

int Nova_GetComplianceScore(HostRankMethod method, double *k, double *r,
                            PromiseContextMode promise_context)
{
    int result = CF_GREEN;
    double kav, rav, notkept;

    double kc = 0, rc = 0;

    switch(promise_context)
    {
        case PROMISE_CONTEXT_MODE_ALL:
            kc = k[meter_compliance_hour];
            rc = r[meter_compliance_hour];
            break;
        case PROMISE_CONTEXT_MODE_USER:
            kc = k[meter_compliance_hour_user];
            rc = r[meter_compliance_hour_user];
            break;
        case PROMISE_CONTEXT_MODE_INTERNAL:
            kc = k[meter_compliance_hour_internal];
            rc = r[meter_compliance_hour_internal];
            break;
    }

    switch (method)
    {
        default:
        case HOST_RANK_METHOD_COMPLIANCE:

            notkept = 100 - kc - rc;

            if (notkept > 20)       // 20% of promises are not kept => RED!
            {
                result = CF_RED_THRESHOLD + 100 + notkept;  // Make red override all variations in amber/green
            }

            if (rc > 20)      // If more than 20% of promises were repaired => AMBER
            {
                result = CF_AMBER_THRESHOLD + 100 + rc;
            }

            result -= kc;     // Adjust the Green Value relative
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

            kav = (k[meter_comms_hour] + kc + k[meter_anomalies_day]) / 3;
            rav = (r[meter_comms_hour] + rc + r[meter_anomalies_day]) / 3;

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

bool IsKeyWithinPromiseContext(const char *ns, const char *key,
                               PromiseContextMode promise_context)
{
    /*
     * cfe_internal_.* = internal
     * default:cfe_internal_.* = internal
     * cfe_system.*:any = internal
     * any other then above = user
     */

    if (!ns) /* key don't have namespace -- apply the same rule as for handles */
    {
        return IsHandleWithinPromiseContext(key, promise_context);
    }
    else
    {
        switch (promise_context)
        {
            case PROMISE_CONTEXT_MODE_USER:
                if (CompareStringOrRegex(ns, CF_INTERNAL_NAMESPACE_RX, true))
                {
                    return false;
                }
                else if (CompareStringOrRegex(ns, "default", true))
                {
                    return IsHandleWithinPromiseContext(key, promise_context);
                }
                else
                {
                    return true;
                }
                break;

            case PROMISE_CONTEXT_MODE_INTERNAL:

                if (CompareStringOrRegex(ns, CF_INTERNAL_NAMESPACE_RX, true))
                {
                    return true;
                }
                else if (CompareStringOrRegex(ns, "default", true))
                {
                    return IsHandleWithinPromiseContext(key, promise_context);
                }
                else
                {
                    return false;
                }
                break;

            case PROMISE_CONTEXT_MODE_ALL:
                break;
        }
    }

    return true;
}

/*****************************************************************************/

bool IsHandleWithinPromiseContext(const char *handle, PromiseContextMode promise_context)
{
    switch (promise_context)
    {
        case PROMISE_CONTEXT_MODE_USER:
            if (CompareStringOrRegex(handle, CF_INTERNAL_PROMISE_RX_HANDLE, true))
            {
                return false;
            }
            break;

        case PROMISE_CONTEXT_MODE_INTERNAL:
            if (!CompareStringOrRegex(handle, CF_INTERNAL_PROMISE_RX_HANDLE, true))
            {
                return false;
            }
            break;

        case PROMISE_CONTEXT_MODE_ALL:
            break;
    }

    return true;
}

/*****************************************************************************/
