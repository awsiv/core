/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_hub_conversion.h"

#include "bson_lib.h"
#include "files_names.h"
#include "db_export_csv.h"
#include "cfstream.h"
#include "string_lib.h"
#include "scope.h"
#include "install.h"

#include <assert.h>



/*****************************************************************************/


HubPromiseCompliance *BsonIteratorGetPromiseCompliance(bson_iterator *it, HubHost *hh, char *rhandle)
{
    double rex = 0;
    double rsigma = 0;
    time_t timestamp = 0;
    char rstatus = 'x';
    char *prstat;

    while (bson_iterator_next(it))
    {
        if (strcmp(bson_iterator_key(it), cfr_obs_E) == 0)
        {
            rex = bson_iterator_double(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_obs_sigma) == 0)
        {
            rsigma = bson_iterator_double(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_time) == 0)
        {
            timestamp = bson_iterator_int(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_promisestatus) == 0)
        {
            prstat = (char *) bson_iterator_string(it);
            rstatus = *prstat;
        }
        else
        {
            CfOut(cf_inform, "", " !! Unknown key \"%s\" in promise compliance",
                  bson_iterator_key(it));
        }
    }

    return NewHubCompliance(hh, rhandle, rstatus, rex, rsigma, timestamp);
}
/*****************************************************************************/

HubBundleSeen *BsonIteratorGetBundleSeen(bson_iterator *it, HubHost *hh, char *rns, char *rbundle)
{
    time_t rt = (time_t)0;
    double rcomp = 0,
           ravg = 0,
           rdev = 0;

    while (bson_iterator_next(it))
    {
        if (strcmp(bson_iterator_key(it), cfr_bundleavg) == 0)
        {
            ravg = bson_iterator_double(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_bundledev) == 0)
        {
            rdev = bson_iterator_double(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_bundlecomp) == 0)
        {
            rcomp = bson_iterator_double(it);
        }
        else if (strcmp(bson_iterator_key(it), cfr_time) == 0)
        {
            rt = bson_iterator_int(it);
        }
        else
        {
            CfOut(cf_inform, "", " !! Unknown key \"%s\" in bundle seen", bson_iterator_key(it));
        }
    }
    return NewHubBundleSeen(hh, rns, rbundle, rcomp, ravg, rdev, rt);
}

/*****************************************************************************/

bool BsonIterGetPromiseComplianceDetails(bson_iterator *it, char *lhandle,
                                         PromiseState lstatus,
                                         time_t from, time_t to,
                                         time_t blueHorizonTime,
                                         HubHost *hh, Rlist **record_list,
                                         PromiseContextMode promise_context,
                                         WebReportFileInfo *wr_info, Writer *writer,
                                         int db_options)
{
    HostColour colour = HOST_COLOUR_GREEN_YELLOW_RED;
    time_t last_report = 0;
    bool found = false;

    while (bson_iterator_next(it))
    {
        if (strcmp(bson_iterator_key(it), cfr_is_black) == 0)
        {
            if(bson_iterator_bool(it))
            {
                colour = HOST_COLOUR_BLACK;
            }
        }
        else if (strcmp(bson_iterator_key(it), cfr_time) == 0)
        {
            last_report = bson_iterator_int(it);
            if(last_report < blueHorizonTime)
            {
                colour = HOST_COLOUR_BLUE;
            }
        }
        else if (strcmp(bson_iterator_key(it), cfr_promisecompl) == 0)
        {
            bson_iterator it2;
            bson_iterator_subiterator(it, &it2);

            while (bson_iterator_next(&it2))
            {
                char rhandle[CF_MAXVARSIZE] = {0};
                strncpy(rhandle, bson_iterator_key(&it2), CF_MAXVARSIZE - 1);

                switch (promise_context)
                {
                    case PROMISE_CONTEXT_MODE_USER:
                        if (CompareStringOrRegex(rhandle, CF_INTERNAL_PROMISE_RX_HANDLE, true))
                        {
                            continue;
                        }
                        break;

                    case PROMISE_CONTEXT_MODE_INTERNAL:
                        if (!CompareStringOrRegex(rhandle, CF_INTERNAL_PROMISE_RX_HANDLE, true))
                        {
                            continue;
                        }
                        break;

                    case PROMISE_CONTEXT_MODE_ALL:
                        break;
                }

                bson_iterator it3;
                bson_iterator_subiterator(&it2, &it3);
                HubPromiseCompliance *hp = BsonIteratorGetPromiseCompliance(&it3, hh, rhandle);

                bool matched = true;
                matched &=  CompareStringOrRegex(hp->handle, lhandle, (db_options & QUERY_FLAG_IS_REGEX));
                matched &= IsTimeWithinRange(from, to, hp->t);

                if (lstatus != PROMISE_STATE_ANY && lstatus != hp->status)
                {
                    matched = false;
                }

                if (matched)
                {
                    found = true;

                    if( wr_info )
                    {
                        ExportWebReportUpdate( writer, (void *) hp, HubPromiseComplianceToCSV, wr_info);
                        DeleteHubPromiseCompliance(hp);
                    }
                    else if (!(db_options & QUERY_FLAG_HOSTONLY))
                    {
                        PrependRlistAlien(record_list, hp);
                    }
                    else
                    {
                        DeleteHubPromiseCompliance(hp);
                    }
                }
            }
        }
    }

    if (found)
    {
        hh->colour = colour;
    }

    return found;
}
/*****************************************************************************/

bool BsonIterGetBundleReportDetails(bson_iterator *it, char *ns, char *lname,
                                    time_t blueHorizonTime, HubHost *hh,
                                    Rlist **record_list, PromiseContextMode promise_context,
                                    WebReportFileInfo *wr_info, Writer *writer, int db_options)
{
    char rscope[CF_MAXVARSIZE] = {0};

    HostColour colour = HOST_COLOUR_GREEN_YELLOW_RED;
    time_t last_report = 0;
    bool found = false;

    while (bson_iterator_next(it))
    {
        if (strcmp(bson_iterator_key(it), cfr_is_black) == 0)
        {
            if(bson_iterator_bool(it))
            {
                colour = HOST_COLOUR_BLACK;
            }
        }
        else if (strcmp(bson_iterator_key(it), cfr_time) == 0)
        {
            last_report = bson_iterator_int(it);
            if(last_report < blueHorizonTime)
            {
                colour = HOST_COLOUR_BLUE;
            }
        }
        else if (strcmp(bson_iterator_key(it), cfr_bundles) == 0)
        {
            bson_iterator iterAllBundles;
            bson_iterator_subiterator(it, &iterAllBundles);

            while (bson_iterator_next(&iterAllBundles))
            {
                strncpy(rscope, bson_iterator_key(&iterAllBundles), CF_MAXVARSIZE - 1);

                if (strcmp(rscope, "QUERY") == 0)
                {
                    continue;
                }

                char rns[CF_MAXVARSIZE] = "\0";
                char rbundle[CF_MAXVARSIZE] = "\0";

                SplitScopeName(rscope, rns, rbundle);

                bson_iterator iterBundleData;
                bson_iterator_subiterator(&iterAllBundles, &iterBundleData);

                HubBundleSeen *hb = BsonIteratorGetBundleSeen(&iterBundleData, hh, rns, rbundle);

                if (!IsKeyWithinPromiseContext(rns, rbundle, promise_context))
                {
                    continue;
                }

                if(CompareStringOrRegex(hb->bundle, lname, (db_options & QUERY_FLAG_IS_REGEX)))
                {
                    found = true;

                    if( wr_info )
                    {
                        ExportWebReportUpdate( writer, (void *) hb, HubBundleSeenToCSV, wr_info);
                        DeleteHubBundleSeen(hb);
                    }
                    else if (db_options & QUERY_FLAG_HOSTONLY)
                    {
                        DeleteHubBundleSeen(hb);
                    }
                    else
                    {
                        PrependRlistAlien(record_list, hb);
                    }
                }
            }
        }
    }

    if (found)
    {
        hh->colour = colour;
    }

    return found;
}
/*****************************************************************************/

void CFDB_ScanHubHost(bson_iterator *it1, char *keyhash, char *ipaddr, char *hostnames)
{
    bson_iterator it2;

    int ipFound = false;
    int hostFound = false;

    if (bson_iterator_type(it1) == BSON_STRING && strcmp(bson_iterator_key(it1), cfr_keyhash) == 0)
    {
        strncpy(keyhash, bson_iterator_string(it1), CF_MAXVARSIZE - 1);
    }

    if (strcmp(bson_iterator_key(it1), cfr_ip_array) == 0)
    {
        bson_iterator_subiterator(it1, &it2);

        while (bson_iterator_next(&it2))
        {
            ipFound = true;
            Join(ipaddr, (char *) bson_iterator_string(&it2), CF_MAXVARSIZE);
            Join(ipaddr, ", ", CF_MAXVARSIZE);
        }
    }

    if (strcmp(bson_iterator_key(it1), cfr_host_array) == 0)
    {
        bson_iterator_subiterator(it1, &it2);

        while (bson_iterator_next(&it2))
        {
            hostFound = true;
            Join(hostnames, (char *) bson_iterator_string(&it2), CF_MAXVARSIZE);
            Join(hostnames, ", ", CF_MAXVARSIZE);
        }
    }

// remove any trailing ", "

    if (ipFound)
    {
        if (ipaddr[strlen(ipaddr) - 2] == ',' && ipaddr[strlen(ipaddr) - 1] == ' ')
        {
            ipaddr[strlen(ipaddr) - 2] = '\0';
        }
    }

    if (hostFound)
    {
        if (hostnames[strlen(hostnames) - 2] == ',' && hostnames[strlen(hostnames) - 1] == ' ')
        {
            hostnames[strlen(hostnames) - 2] = '\0';
        }
    }
}

/*****************************************************************************/

bool CompareStringOrRegex(const char *value, const char *compareTo, bool regex)
{
    if (regex)
    {
        if (!NULL_OR_EMPTY(compareTo) && !StringMatchFull(compareTo, value))
        {
            return false;
        }
    }
    else
    {
        if (!NULL_OR_EMPTY(compareTo)  && strcmp(compareTo, value) != 0)
        {
            return false;
        }
    }
    return true;
}

/*****************************************************************************/

bool IsTimeWithinRange(time_t from, time_t to, time_t t)
{
    if (t < from || t > to)
    {
        return false;
    }

    return true;
}
/*****************************************************************************/

