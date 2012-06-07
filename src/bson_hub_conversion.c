/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: bson_hub_conversion.c                                               */
/*                                                                           */
/* Created: Fri May 04 11:43:22 2012                                         */
/*                                                                           */
/* Conversion functions from bson_iterator to CFE data structures            */
/*                                                                           */
/*****************************************************************************/

#include "bson_hub_conversion.h"

#include "bson_lib.h"
#include "files_names.h"

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

HubBundleSeen *BsonIteratorGetBundleSeen(bson_iterator *it, HubHost *hh, char *rname)
{
    time_t rt;
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
    return NewHubBundleSeen(hh, rname, rcomp, ravg, rdev, rt);
}

/*****************************************************************************/

bool BsonIterGetPromiseComplianceDetails(bson_iterator *it, char *lhandle, bool regex, PromiseState lstatus, time_t from, time_t to, time_t blueHorizonTime, HubHost *hh, Rlist **record_list )
{
    char keyHashDb[CF_MAXVARSIZE] = {0};
    char hostnames[CF_MAXVARSIZE] = {0};
    char addresses[CF_MAXVARSIZE] = {0};

    HostColour colour = HOST_COLOUR_GREEN_YELLOW_RED;
    time_t last_report = 0;
    bool found = false;

    while (bson_iterator_next(it))
    {
        CFDB_ScanHubHost(it, keyHashDb, addresses, hostnames);

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

                bson_iterator it3;
                bson_iterator_subiterator(&it2, &it3);
                HubPromiseCompliance *hp = BsonIteratorGetPromiseCompliance(&it3, hh, rhandle);

                bool matched = true;
                matched &=  CompareStringOrRegex(hp->handle, lhandle, regex);
                matched &= IsTimeWithinRange(from, to, hp->t);

                if (lstatus != PROMISE_STATE_ANY && lstatus != hp->status)
                {
                    matched = false;
                }

                if (matched)
                {
                    found = true;
                    PrependRlistAlien(record_list, hp);
                }
            }
        }
    }

    if (found)
    {
        UpdateHubHost(hh, keyHashDb, addresses, hostnames);
        hh->colour = colour;
    }

    return found;
}
/*****************************************************************************/

bool BsonIterGetBundleReportDetails(bson_iterator *it, char *lname, bool regex, time_t blueHorizonTime, HubHost *hh, Rlist **record_list )
{
    char keyhash[CF_MAXVARSIZE] = {0},
         hostnames[CF_MAXVARSIZE] = {0},
         addresses[CF_MAXVARSIZE] = {0},
         rname[CF_MAXVARSIZE] = {0};

    HostColour colour = HOST_COLOUR_GREEN_YELLOW_RED;
    time_t last_report = 0;
    bool found = false;

    while (bson_iterator_next(it))
    {
        CFDB_ScanHubHost(it, keyhash, addresses, hostnames);

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
                strncpy(rname, bson_iterator_key(&iterAllBundles), CF_MAXVARSIZE - 1);

                if (strcmp(rname, "QUERY") == 0)
                {
                    continue;
                }

                bson_iterator iterBundleData;
                bson_iterator_subiterator(&iterAllBundles, &iterBundleData);

                HubBundleSeen *hb = BsonIteratorGetBundleSeen(&iterBundleData, hh, rname);

                if(CompareStringOrRegex(hb->bundle, lname, regex))
                {
                    found = true;
                    PrependRlistAlien(record_list, hb);
                }
            }
        }
    }

    if (found)
    {
        UpdateHubHost(hh, keyhash, addresses, hostnames);
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

