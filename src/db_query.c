/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "db_query.h"

#include "env_context.h"
#include "bson_lib.h"
#include "web_rbac.h"
#include "db_save.h"
#include "granules.h"
#include "map.h"
#include "bson_hub_conversion.h"
#include "files_names.h"
#include "item_lib.h"

#include <assert.h>

static bool BsonAppendPromiseFilter(bson *query, PromiseFilter *filter);
static bool AppendHostKeys(EnterpriseDB *conn, bson *b, HostClassFilter *hostClassFilter);
static void GetOldClientVersions(Rlist **rp);

Rlist *PrependRlistAlienUnlocked(Rlist **start, void *item)
{
    Rlist *rp = xmalloc(sizeof(Rlist));

    rp->next = *start;
    *start = rp;

    rp->item = item;
    rp->type = CF_SCALAR;
    return rp;
}

/*****************************************************************************/
bool CFDB_CollectionHasData(EnterpriseDB *conn, const char *fullCollectionName)
{
    bson query;
    bson_empty(&query);

    bson field;
    bson_init(&field);
    bson_append_int(&field, "_id", 1);
    bson_finish(&field);

    if(mongo_find_one(conn,fullCollectionName,&query,&field,NULL) == MONGO_OK)
    {
        bson_destroy(&field);
        return true;
    }

    return false;
}
/*****************************************************************************/
int CFDB_GetValue(char *lval, char *rval, int size, char *db_name)
{
    EnterpriseDB conn;

    // clients do not run mongo server -- will fail to connect

    if (!IsDefinedClass("am_policy_hub") && !AM_PHP_MODULE)
    {
        CfOut(cf_verbose, "", "Ignoring DB get of (%s) - we are not a policy server", lval);
        return false;
    }

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    CFDB_HandleGetValue(lval, rval, size, &conn, db_name);

    CFDB_Close(&conn);
    return true;
}

/*****************************************************************************/

int CFDB_GetBlueHostThreshold(unsigned long *threshold)
{
    unsigned long retval = CF_BLUEHOST_THRESHOLD_DEFAULT;
    char threshold_str[CF_SMALLBUF];

    threshold_str[0] = '\0';

    if (!CFDB_GetValue(CFMP_BLUEHOST_THRESHOLD, threshold_str, CF_SMALLBUF, MONGO_MP_SETTINGS_COLLECTION))
    {
        return false;
    }

    if (strlen(threshold_str) == 0)     // no key in db then insert hardcoded default
    {
        snprintf(threshold_str, CF_SMALLBUF, "%lu", retval);
        if (!CFDB_PutValue(CFMP_BLUEHOST_THRESHOLD, threshold_str, MONGO_MP_SETTINGS_COLLECTION))
        {
            return false;
        }
    }
    else
    {
        sscanf(threshold_str, "%lu", &retval);
    }

    *threshold = retval;
    return true;
}

/*****************************************************************************/

Item *CFDB_GetLastseenCache(void)
{
    bson_iterator it1, it2, it3;
    mongo conn;
    char keyhash[CF_BUFSIZE] = { 0 }, ipAddr[CF_MAXVARSIZE] = { 0 };
    time_t t = time(NULL);
    Item *ip, *list = NULL;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson field;
    bson_init(&field);
    bson_append_int(&field, cfr_lastseen_hosts, 1);
    bson_finish(&field);

    bson query;

    mongo_cursor *cursor = mongo_find(&conn, MONGO_SCRATCH, bson_empty(&query), &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&field);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_lastseen_hosts) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    keyhash[0] = '\0';
                    ipAddr[0] = '\0';
                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_keyhash) == 0)
                        {
                            snpintf(keyhash, sizeof(keyhash), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_ipaddr) == 0)
                        {
                            snprintf(ipAddr, sizeof(ipAddr), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            t = bson_iterator_int(&it3);
                        }
                    }
                    if (strlen(keyhash) > 0 && strlen(ipAddr) > 0)
                    {
                        ip = IdempPrependItem(&list, keyhash, ipAddr);
                        ip->time = t;
                    }
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return list;
}

/*****************************************************************************/

Item *CFDB_GetDeletedHosts(void)
{
    bson_iterator it1, it2;
    mongo_cursor *cursor;
    EnterpriseDB conn;
    Item *list = NULL;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson field;
    bson_init(&field);
    bson_append_int(&field, cfr_deleted_hosts, 1);
    bson_finish(&field);

    bson query;

    mongo_cursor *cursor = mongo_find(&conn, MONGO_SCRATCH, bson_empty(&query), &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&field);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_deleted_hosts) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    IdempPrependItem(&list, (char *) bson_iterator_string(&it2), NULL);
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return list;
}

/*****************************************************************************/

bool CFDB_HandleGetValue(char *lval, char *rval, int size, EnterpriseDB *conn, char *db_name)
{
    bson query;
    bson_iterator it1;
    mongo_cursor *cursor;

    rval[0] = '\0';

    cursor = mongo_find(conn, db_name, bson_empty(&query), 0, 0, 0, CF_MONGO_SLAVE_OK);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), lval) == 0)
            {
                snprintf(rval, size, "%s", bson_iterator_string(&it1));
                break;
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return rval[0] != '\0';
}



HubQuery *CFDB_QueryHosts(EnterpriseDB *conn, char *db, bson *query)
{
    bson_iterator it1;
    HubHost *hh;
    Rlist *host_list = NULL;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE];

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, db, query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';

        while (bson_iterator_next(&it1))
        {
            /* Extract the common HubHost data */

            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);
        }

        hh = NewHubHost(NULL, keyhash, addresses, hostnames);
        PrependRlistAlienUnlocked(&host_list, hh);
    }

    mongo_cursor_destroy(cursor);
    return NewHubQuery(host_list, NULL);
}


/*****************************************************************************/

HubQuery *CFDB_QueryHostsByAddress(EnterpriseDB *conn, char *hostNameRegex, char *ipRegex,
                                   HostClassFilter *hostClassFilter)
{
    HubQuery *hq;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(hostNameRegex))
    {
        bson_append_regex(&query, cfr_host_array, hostNameRegex, "");
    }

    if (!NULL_OR_EMPTY(ipRegex))
    {
        bson_append_regex(&query, cfr_ip_array, ipRegex, "");
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    hq = CFDB_QueryHosts(conn, MONGO_DATABASE, &query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_QueryHostsByHostClassFilter(mongo *conn, HostClassFilter *hostClassFilter)
{   
    bson query;

    bson_init(&query);
    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    HubQuery *hq = CFDB_QueryHosts(conn, MONGO_DATABASE, &query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

HubQuery *CFDB_QueryHostByHostKey(EnterpriseDB *conn, char *hostKey)
{
    assert(SafeStringLength(hostKey) > 0);

    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostKey);
    bson_finish(&query);

    HubQuery *hq = CFDB_QueryHosts(conn, MONGO_DATABASE, &query);

    bson_destroy(&query);

    return hq;
}

/*****************************************************************************/

static const char *HostRankMethodToMongoCode(HostRankMethod method)
{
    switch (method)
    {
        case HOST_RANK_METHOD_COMPLIANCE:
            return cfr_score_comp;

        case HOST_RANK_METHOD_ANOMALY:
            return cfr_score_anom;

        case HOST_RANK_METHOD_PERFORMANCE:
            return cfr_score_perf;

        case HOST_RANK_METHOD_LASTSEEN:
            return cfr_score_lastseen;

        case HOST_RANK_METHOD_MIXED:
            return cfr_score_mixed;

        default:
            return cfr_score_comp;
    }
}

HubQuery *CFDB_QueryColour(EnterpriseDB *conn, const HostRankMethod method, HostClassFilter *host_class_filter)
{
    unsigned long blue_horizon;
    if (!CFDB_GetBlueHostThreshold(&blue_horizon))
    {
        assert(false && "Could not determine blue horizon");
        blue_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;
    }

    bson query;
    bson_init(&query);
    BsonAppendHostClassFilter(&query, host_class_filter);
    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_day, 1);
    bson_append_int(&fields, HostRankMethodToMongoCode(method), 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    time_t now = time(NULL);

    Rlist *host_list = NULL;
    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        HubHost *host = NULL;
        {
            Item *host_names = BsonGetStringArrayAsItemList(&cursor->current, cfr_host_array);
            Item *ip_addresses = BsonGetStringArrayAsItemList(&cursor->current, cfr_ip_array);

            const char *hostkey = NULL;
            BsonStringGet(&cursor->current, cfr_keyhash, &hostkey);
            assert(hostkey);

            {
                const char *ip_address = NULL;
                if (ip_addresses)
                {
                    ip_address = ip_addresses->name;
                }

                const char *host_name = NULL;
                if (host_names)
                {
                    host_name = host_names->name;
                }

                host = NewHubHost(NULL, hostkey, ip_address, host_name);
            }

            DeleteItemList(host_names);
            DeleteItemList(ip_addresses);
        }
        assert(host);

        bool is_black = false;
        BsonBoolGet(&cursor->current, cfr_is_black, &is_black);

        time_t last_report = 0;
        BsonTimeGet(&cursor->current, cfr_day, &last_report);

        int score = 0;
        if (BsonIntGet(&cursor->current, HostRankMethodToMongoCode(method), &score))
        {
            host->colour = HostColourFromScore(now, last_report, blue_horizon, score, is_black);
        }
        else
        {
            // special case: could not find a score, mark as blue
            host->colour = HOST_COLOUR_BLUE;
        }
        PrependRlistAlienUnlocked(&host_list, host);
    }

    mongo_cursor_destroy(cursor);
    return NewHubQuery(host_list, NULL);
}

HubQuery *CFDB_QuerySoftware(EnterpriseDB *conn, char *keyHash, char *type, char *lname, char *lver, char *larch,
                             bool regex, HostClassFilter *hostClassFilter, int sort)
// NOTE: needs to return report from one host before next - not mixed (for Constellation)
{
    bson_iterator it1, it2, it3;
    HubHost *hh = NULL;
    Rlist *record_list = NULL, *host_list = NULL;
    char rname[CF_MAXVARSIZE] = { 0 }, rversion[CF_MAXVARSIZE] = { 0 }, rarch[3] = { 0 }, arch[3] = { 0 };
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE];
    int found = false;

    if (!NULL_OR_EMPTY(larch))
    {
        snprintf(arch, 2, "%c", larch[0]);
    }
/* BEGIN query document */
    bson query;

    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, type, 1);
    bson_append_int(&fields, cfr_software_t, 1);
// TODO: Add support for time of NOVA_PATCHES_INSTALLED and NOVA_PATCHES_AVAIL?
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    time_t lastSeen = 0;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        hh = NULL;
        found = false;

        if (strcmp(type, cfr_software) == 0)
        {
            BsonTimeGet(&(cursor->current), cfr_software_t, &lastSeen);
        }

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), type) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    rname[0] = '\0';
                    rversion[0] = '\0';
                    rarch[0] = '\0';

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_name) == 0)
                        {
                            strncpy(rname, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_version) == 0)
                        {
                            strncpy(rversion, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_arch) == 0)
                        {
                            strncpy(rarch, bson_iterator_string(&it3), 2);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in software packages",
                                  bson_iterator_key(&it3));
                        }
                    }

                    if (strlen(rname) > 0)
                    {
                        int match_name = true, match_version = true, match_arch = true;

                        if (regex)
                        {
                            if (!NULL_OR_EMPTY(lname) && !StringMatchFull(lname, rname))
                            {
                                match_name = false;
                            }

                            if (!NULL_OR_EMPTY(lver) && !StringMatchFull(lver, rversion))
                            {
                                match_version = false;
                            }
                            if (!NULL_OR_EMPTY(larch) && !StringMatchFull(arch, rarch))
                            {
                                match_arch = false;
                            }
                        }
                        else
                        {
                            if (!NULL_OR_EMPTY(lname) && (strcmp(lname, rname) != 0))
                            {
                                match_name = false;
                            }

                            if (!NULL_OR_EMPTY(lver) && (strcmp(lver, rversion) != 0))
                            {
                                match_version = false;
                            }

                            if (!NULL_OR_EMPTY(larch) && (strcmp(arch, rarch) != 0))
                            {
                                match_arch = false;
                            }
                        }
                        if (match_name && match_version && match_arch)
                        {
                            found = true;

                            if (!hh)
                            {
                                hh = CreateEmptyHubHost();
                            }

                            PrependRlistAlienUnlocked(&record_list, NewHubSoftware(hh, rname, rversion, rarch, lastSeen));
                        }
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortSoftware);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryClasses(EnterpriseDB *conn, char *keyHash, char *lclass, bool regex, time_t from, time_t to,
                            HostClassFilter *hostClassFilter, int sort)
{
    mongo_cursor *cursor;
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    double rsigma, rex;
    char rclass[CF_MAXVARSIZE];
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE];
    int match_class, found = false;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_class, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        rclass[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_class) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    strncpy(rclass, bson_iterator_key(&it2), CF_MAXVARSIZE - 1);

                    rex = 0;
                    rsigma = 0;
                    time_t timestamp = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_obs_E) == 0)
                        {
                            rex = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_obs_sigma) == 0)
                        {
                            rsigma = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            timestamp = bson_iterator_int(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in classes", bson_iterator_key(&it3));
                        }
                    }

                    match_class = true;

                    if (regex)
                    {
                        if (!NULL_OR_EMPTY(lclass) && !StringMatchFull(lclass, rclass))
                        {
                            match_class = false;
                        }
                    }
                    else
                    {
                        if (!NULL_OR_EMPTY(lclass) && (strcmp(lclass, rclass) != 0))
                        {
                            match_class = false;
                        }
                    }

                    if (timestamp < from || timestamp > to)
                    {
                        match_class = false;
                    }

                    if (match_class)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list, NewHubClass(hh, rclass, rex, rsigma, timestamp));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortClasses);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryClassSum(EnterpriseDB *conn, char **classes)
/**
 * classes is NULL or NULL-terminated array of strings
 *
 * NOTE: Can probably be made more efficient using group by class keys with a count
 **/
{
    bson_iterator it1;
    Rlist *hostList = NULL, *recordList = NULL;
    Item *classList = NULL, *ip;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_MAXVARSIZE], addresses[CF_MAXVARSIZE];
    char iStr[CF_SMALLBUF];
    int classFrequency;
    int i;

    CfDebug("CFDB_QueryClassSum()\n");

    // query
    bson query;
    bson_init(&query);

    if (classes && classes[0])
    {
        {
            bson_append_start_object(&query, cfr_class_keys);

            {
                bson_append_start_array(&query, "$all");
                for (i = 0; classes[i] != NULL; i++)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);
                    bson_append_string(&query, iStr, classes[i]);
                }
                bson_append_finish_object(&query);
            }

            bson_append_finish_object(&query);
        }
    }

    bson_finish(&query);

// returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);
    // query freed below

    // 1: collect hosts matching new class

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        addresses[0] = '\0';
        hostnames[0] = '\0';

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);
        }

        if (!NULL_OR_EMPTY(keyhash))
        {
            PrependRlistAlienUnlocked(&hostList, NewHubHost(NULL, keyhash, addresses, hostnames));
            CfDebug("matched host %s,%s\n", keyhash, addresses);
        }
    }

    mongo_cursor_destroy(cursor);

    // 2: find all distinct classes in these hosts
    classList = CFDB_QueryDistinct(conn, MONGO_BASE, "hosts", cfr_class_keys, &query);

    bson_destroy(&query);

    // 3: count occurences of each class in subset of hosts
    for (ip = classList; ip != NULL; ip = ip->next)
    {
        bson_init(&query);
        {
            bson_append_start_object(&query, cfr_class_keys);
            {
                bson_append_start_array(&query, "$all");

                for (i = 0; classes && classes[i] != NULL; i++)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);
                    bson_append_string(&query, iStr, classes[i]);
                }
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(&query, iStr, ip->name);

                bson_append_finish_object(&query);
            }

            bson_append_finish_object(&query);
        }

        bson_finish(&query);

        classFrequency = (int) mongo_count(conn, MONGO_BASE, "hosts", &query);

        bson_destroy(&query);

        CfDebug("class (%s,%d)\n", ip->name, classFrequency);

        PrependRlistAlienUnlocked(&recordList, NewHubClassSum(NULL, ip->name, classFrequency));
    }

    DeleteItemList(classList);

    recordList = SortRlist(recordList, SortClassSum);

    return NewHubQuery(hostList, recordList);
}

/*****************************************************************************/
HubQuery *CFDB_QueryTotalCompliance(EnterpriseDB *conn, const char *keyHash, char *lversion, time_t from, time_t to, int lkept,
                                    int lnotkept, int lrepaired, int sort, HostClassFilter *hostClassFilter)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    int rkept, rnotkept, rrepaired, found = false;
    int match_kept, match_notkept, match_repaired, match_version, match_t;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rversion[CF_MAXVARSIZE];

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_total_compliance, 1);
    bson_finish(&fields);

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_total_compliance) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    rkept = -1;
                    rrepaired = -1;
                    rnotkept = -1;
                    rversion[0] = '\0';
                    time_t timestamp = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_kept) == 0)
                        {
                            rkept = bson_iterator_int(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_notkept) == 0)
                        {
                            rnotkept = bson_iterator_int(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_repaired) == 0)
                        {
                            rrepaired = bson_iterator_int(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            timestamp = bson_iterator_int(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_version) == 0)
                        {
                            strncpy(rversion, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in total compliance", bson_iterator_key(&it3));
                        }
                    }

                    match_version = match_t = match_kept = match_notkept = match_repaired = true;

                    if (!NULL_OR_EMPTY(lversion) && !StringMatchFull(lversion, rversion))
                    {
                        match_version = false;
                    }

                    if (timestamp < from || timestamp > to)
                    {
                        match_t = false;
                    }

                    if (lkept != -1 && lkept < rkept)
                    {
                        match_kept = false;
                    }

                    if (lnotkept != -1 && lnotkept < rnotkept)
                    {
                        match_notkept = false;
                    }

                    if (lrepaired != -1 && lrepaired < rrepaired)
                    {
                        match_repaired = false;
                    }

                    if (match_kept && match_notkept && match_repaired && match_t && match_version)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list,
                                          NewHubTotalCompliance(hh, timestamp, rversion, rkept, rrepaired, rnotkept));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortTotalCompliance);
    }

    return NewHubQuery(host_list, record_list);
}

Sequence *CFDB_QueryHostComplianceShifts(EnterpriseDB *conn, HostClassFilter *host_class_filter)
{

    bson query;
    bson_init(&query);
    BsonAppendHostClassFilter(&query, host_class_filter);
    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_compliance_shifts, 1);
    bson_finish(&fields);

    time_t to = GetShiftSlotStart(time(NULL));
    time_t from = to - (SHIFTS_PER_WEEK * SECONDS_PER_SHIFT);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Sequence *records = SequenceCreate(5000, DeleteHubHostComplianceShifts);
    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        const char *hostkey = NULL;
        BsonStringGet(&cursor->current, cfr_keyhash, &hostkey);
        assert(hostkey);

        const bson *compliance_shifts = NULL;
        BsonObjectGet(mongo_cursor_bson( cursor ), cfr_compliance_shifts, &compliance_shifts);
        if (compliance_shifts)
        {
            HubHostComplianceShifts *record = NewHubHostComplianceShifts(hostkey);

            bson_iterator compliance_shifts_iter;
            bson_iterator_init(&compliance_shifts_iter, compliance_shifts);

            while (bson_iterator_next(&compliance_shifts_iter))
            {
                const char *shift_slot_str = bson_iterator_key(&compliance_shifts_iter);
                assert(shift_slot_str);

                int shift_slot = StringToLong(shift_slot_str);
                if (shift_slot < 0 || shift_slot >= SHIFTS_PER_WEEK)
                {
                    // TODO: log error
                    continue;
                }

                bson entry;
                bson_iterator_subobject(&compliance_shifts_iter, &entry);

                time_t timestamp = 0;
                BsonTimeGet(&entry, cfr_time, &timestamp);
                if (timestamp < from || timestamp > to)
                {
                    continue;
                }

                BsonIntGet(&entry, cfr_kept, &record->kept[shift_slot]);
                BsonIntGet(&entry, cfr_repaired, &record->repaired[shift_slot]);
                BsonIntGet(&entry, cfr_notkept, &record->notkept[shift_slot]);
                BsonIntGet(&entry, cfr_count, &record->num_samples[shift_slot]);
            }

            SequenceAppend(records, record);
        }
    }

    mongo_cursor_destroy(cursor);

    return records;
}

/*****************************************************************************/

HubQuery *CFDB_QueryVariables(EnterpriseDB *conn, char *keyHash, char *lscope, char *llval, char *lrval,
                              const char *ltype, bool regex, time_t from, time_t to, HostClassFilter *hostClassFilter)
{
    bson_iterator it1, it2, it3, it4, it5;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL, *newlist = NULL;
    int found = false;
    bool match_type, match_scope, match_lval, match_rval, match_time;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE];
    char rscope[CF_MAXVARSIZE], rlval[CF_MAXVARSIZE], dtype[CF_MAXVARSIZE], rtype;
    void *rrval;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_vars, 1);
    bson_finish(&fields);

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        hh = NULL;
        found = false;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            rlval[0] = '\0';
            rrval = NULL;
            rtype = CF_SCALAR;

            if (strcmp(bson_iterator_key(&it1), cfr_vars) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    strncpy(rscope, bson_iterator_key(&it2), CF_MAXVARSIZE);                    

                    if(BsonIsKeyCorrupt(rscope))
                    {
                        CfOut(cf_inform, "", " !! Corrupted field name scope = \"%s\" in variables", rscope);
                        continue;
                    }

                    while (bson_iterator_next(&it3))
                    {
                        rlval[0] = '\0';
                        rtype = CF_SCALAR;
                        rrval = NULL;
                        newlist = NULL;
                        dtype[0] = '\0';
                        time_t timestamp = 0;

                        bson_iterator_subiterator(&it3, &it4);

                        strncpy(rlval, bson_iterator_key(&it3), CF_MAXVARSIZE - 1);

                        while (bson_iterator_next(&it4))
                        {
                            if (strcmp(bson_iterator_key(&it4), cfr_rval) == 0)
                            {

                                switch (bson_iterator_type(&it4))
                                {
                                case BSON_ARRAY:
                                case BSON_OBJECT:
                                    bson_iterator_subiterator(&it4, &it5);
                                    rtype = CF_LIST;

                                    while (bson_iterator_next(&it5))
                                    {
                                        AppendRScalar(&newlist, (char *) bson_iterator_string(&it5), CF_SCALAR);
                                    }

                                    rrval = newlist;
                                    break;

                                default:
                                    rrval = xstrdup(bson_iterator_string(&it4));
                                    rtype = CF_SCALAR;
                                    break;
                                }
                            }
                            else if (strcmp(bson_iterator_key(&it4), cfr_type) == 0)
                            {
                                strncpy(dtype, bson_iterator_string(&it4), CF_MAXVARSIZE);
                            }
                            else if (strcmp(bson_iterator_key(&it4), cfr_time) == 0)
                            {
                                timestamp = bson_iterator_int(&it4);
                            }
                            else
                            {
                                CfOut(cf_inform, "", " !! Unknown key \"%s\" in variables", bson_iterator_key(&it4));
                            }
                        }

                        // Now do selection

                        match_type = match_scope = match_lval = match_rval = match_time = true;

                        if (regex)
                        {
                            if (!NULL_OR_EMPTY(llval) && !StringMatchFull(llval, rlval))
                            {
                                match_lval = false;
                            }

                            if (!NULL_OR_EMPTY(lrval))
                            {
                                if (rtype == CF_LIST)
                                {
                                    for (Rlist *it = rrval; it != NULL; it = it->next)
                                    {
                                        if (!StringMatchFull(lrval, (char*)it->item))
                                        {
                                            match_rval = false;
                                        }
                                        else
                                        {
                                            match_rval = true;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    if (!StringMatchFull(lrval, rrval))
                                    {
                                        match_rval = false;
                                    }
                                }
                            }

                            if (!NULL_OR_EMPTY(lscope) && !StringMatchFull(lscope, rscope))
                            {
                                match_scope = false;
                            }

                            if (!NULL_OR_EMPTY(ltype) && !StringMatchFull(ltype, dtype))
                            {
                                match_type = false;
                            }
                        }
                        else
                        {
                            if (!NULL_OR_EMPTY(llval) && strcmp(llval, rlval) != 0)
                            {
                                match_lval = false;
                            }

                            if (!NULL_OR_EMPTY(lrval))
                            {
                                if (rtype == CF_LIST)
                                {
                                    for (Rlist *it = rrval; it != NULL; it = it->next)
                                    {
                                        if (strcmp(lrval, (char*)it->item) != 0)
                                        {
                                            match_rval = false;
                                        }
                                        else
                                        {
                                            match_rval = true;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    if (strcmp(lrval, rrval) != 0)
                                    {
                                        match_rval = false;
                                    }
                                }
                            }

                            if (!NULL_OR_EMPTY(lscope) && strcmp(lscope, rscope) != 0)
                            {
                                match_scope = false;
                            }

                            if (!NULL_OR_EMPTY(ltype) && strcmp(ltype, dtype) != 0)
                            {
                                match_type = false;
                            }
                        }

                        if (timestamp < from || timestamp > to)
                        {
                            match_time = false;
                        }

                        Rval rval = (Rval) { rrval, rtype };

                        // NOTE: rrval's ownership (deallocation) is either transferred, or it is freed here
                        if (match_type && match_scope && match_lval && match_rval && match_time)
                        {
                            found = true;

                            if (!hh)
                            {
                                hh = CreateEmptyHubHost();
                            }

                            PrependRlistAlienUnlocked(&record_list, NewHubVariable(hh, dtype, rscope, rlval, rval, timestamp));
                        }
                        else
                        {
                            DeleteRvalItem(rval);
                        }
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);
    return NewHubQuery(host_list, record_list);
}

HubQuery *CFDB_QueryPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter)
// status = c (compliant), r (repaired) or n (not kept), x (any)
{
    unsigned long blue_horizon;
    if (!CFDB_GetBlueHostThreshold(&blue_horizon))
    {
        assert(false && "Could not determine blue horizon");
        blue_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;
    }

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_promisecompl, 1);
    bson_append_int(&fields, cfr_day, 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
          *host_list = NULL;

    time_t blueHorizonTime = time(NULL) - blue_horizon;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator it;
        bson_iterator_init(&it, mongo_cursor_bson(cursor));

        HubHost *hh = CreateEmptyHubHost();

        bool found = BsonIterGetPromiseComplianceDetails(&it, lhandle, regex, lstatus, from, to, blueHorizonTime, hh, &record_list );

        if (found)
        {
            PrependRlistAlienUnlocked(&host_list, hh);
        }
        else
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortPromiseCompliance);
    }

    return NewHubQuery(host_list, record_list);
}
/*****************************************************************************/

HubQuery *CFDB_QueryWeightedPromiseCompliance(EnterpriseDB *conn, char *keyHash, char *lhandle, PromiseState lstatus,
                                      bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter)
// status = c (compliant), r (repaired) or n (not kept), x (any)
{
    unsigned long blue_horizon;
    if (!CFDB_GetBlueHostThreshold(&blue_horizon))
    {
       // assert(false && "Could not determine blue horizon");
        blue_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;
    }

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_promisecompl, 1);
    bson_append_int(&fields, cfr_day, 1);
    bson_append_int(&fields, cfr_score_comp, 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
            *host_list = NULL;

    time_t blueHorizonTime = time(NULL) - blue_horizon;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator it;
        bson_iterator_init(&it, mongo_cursor_bson( cursor ));

        Rlist *record_list_single_host = NULL;
        HubHost *hh = CreateEmptyHubHost();
        bool found = BsonIterGetPromiseComplianceDetails(&it, lhandle, regex, lstatus, from, to, blueHorizonTime, hh, &record_list_single_host );

        HubQuery *hq = NewHubQuery(NULL, record_list_single_host);

        bool hostDataAdded = false;

        if(found)
        {
            long promiseCount = 0;
            double totalCompliance = 0.0;
            bool blueOrBlackHost = true;

            if(hh->colour == HOST_COLOUR_GREEN_YELLOW_RED)
            {
                /* Calculate compliance score for current host */

                for(Rlist *rp = hq->records; rp != NULL; rp = rp->next)
                {
                    HubPromiseCompliance *hp = (HubPromiseCompliance *) rp->item;

                    if(isnan(hp->e))
                    {
                        continue;
                    }

                    totalCompliance += hp->e;
                    promiseCount++;
                }

                /* Evaluate compliance level based on the most recent data */
                if(promiseCount)
                {
                    double avgCompliance = totalCompliance / promiseCount;

                    if (avgCompliance > 0.8)
                    {
                        // Compliant
                        hh->colour = HOST_COLOUR_GREEN;
                    }
                    else
                    {
                        // Non-compliant
                        // Repaired state cannot be determined from the compliance percentage for now

                        hh->colour = HOST_COLOUR_RED;
                    }

                    blueOrBlackHost = false;
                }
                else
                {
                    hh->colour = HOST_COLOUR_BLUE;
                }

                if(!hostColourFilter ||  (hostColourFilter && (hostColourFilter->colour == hh->colour)))
                {
                    for(Rlist *rp = hq->records; rp != NULL; rp = rp->next)
                    {
                        HubPromiseCompliance *hp = (HubPromiseCompliance *) rp->item;

                        /*  Don't include nan values in the calculation */
                        if(isnan(hp->e))
                        {
                            continue;
                        }

                        PrependRlistAlienUnlocked(&record_list, NewHubCompliance(hp->hh, hp->handle, hp->status, hp->e, hp->d, hp->t));
                        hostDataAdded = true;
                    }
                }
            }

            if(blueOrBlackHost)  // blue and black hosts
            {
                for(Rlist *rp = hq->records; rp != NULL; rp = rp->next)
                {
                    HubPromiseCompliance *hp = (HubPromiseCompliance *) rp->item;

                    PrependRlistAlienUnlocked(&record_list, NewHubCompliance(hp->hh, hp->handle, hp->status, hp->e, hp->d, hp->t));
                    hostDataAdded = true;
                }
            }
        }

        /* Cleanup */
        DeleteHubQuery(hq, DeleteHubPromiseCompliance);
        hq = NULL;
        record_list_single_host = NULL;

        if(!hostDataAdded)
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
        else
        {
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(host_list, record_list);
}
/*****************************************************************************/

HubQuery *CFDB_QueryLastSeen(EnterpriseDB *conn, char *keyHash, char *lhash, char *lhost, char *laddr, time_t lago,
                             bool regex, time_t from, time_t to, int sort, HostClassFilter *hostClassFilter)
{
    mongo_cursor *cursor;
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    double rago, ravg, rdev;
    char rhash[CF_MAXVARSIZE], rhost[CF_MAXVARSIZE], raddr[CF_MAXVARSIZE];
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE];
    bool match_host, match_hash, match_addr, match_ago, match_timestamp, found = false;

/* BEGIN query document */
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_lastseen, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        rhash[0] = '\0';
        raddr[0] = '\0';
        rhost[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_lastseen) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    strncpy(rhash, bson_iterator_key(&it2), CF_MAXVARSIZE - 1);

                    ravg = 0;
                    rdev = 0;
                    rago = 0;
                    time_t timestamp = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_hrsavg) == 0)
                        {
                            ravg = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_hrsdev) == 0)
                        {
                            rdev = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_hrsago) == 0)
                        {
                            rago = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_ipaddr) == 0)
                        {
                            strncpy(raddr, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);

                            // try to find host name of IP from local DB
                            // FIXME: this is very slow on large queries (one new query per result)
                            CFDB_QueryHostName(conn, raddr, rhost, sizeof(rhost));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            timestamp = (time_t) bson_iterator_int(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in last seen", bson_iterator_key(&it3));
                        }
                    }

                    match_host = match_addr = match_hash = match_ago = match_timestamp = true;

                    if (regex)
                    {
                        if (!NULL_OR_EMPTY(lhost) && !StringMatchFull(lhost, rhost))
                        {
                            match_host = false;
                        }

                        // Doesn't make sense to do regex on a key
                        if (!NULL_OR_EMPTY(lhash) && (strcmp(lhash, rhash + 1) != 0))
                        {
                            match_hash = false;
                        }

                        if (!NULL_OR_EMPTY(laddr) && !StringMatchFull(laddr, raddr))
                        {
                            match_addr = false;
                        }
                    }
                    else
                    {
                        if (!NULL_OR_EMPTY(lhost) && (strcmp(lhost, rhost) != 0))
                        {
                            match_host = false;
                        }

                        if (!NULL_OR_EMPTY(lhash) && (strcmp(lhash, rhash + 1) != 0))
                        {
                            match_hash = false;
                        }

                        if (!NULL_OR_EMPTY(laddr) && (strcmp(laddr, raddr) != 0))
                        {
                            match_addr = false;
                        }
                    }

                    if (lago > rago)
                    {
                        match_ago = false;
                    }

                    if (timestamp < from || timestamp > to)
                    {
                        match_timestamp = false;
                    }

                    if (match_hash && match_host && match_addr && match_ago && match_timestamp)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        LastSeenDirection direction = *rhash;

                        PrependRlistAlienUnlocked(&record_list,
                                          NewHubLastSeen(hh, direction, rhash + 1, rhost, raddr, rago, ravg, rdev, timestamp));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortLastSeen);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryMeter(EnterpriseDB *conn, bson *query, char *db)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    double rkept, rrepaired;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rcolumn[CF_SMALLBUF];
    int found = false;

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_meter, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, db, query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_meter) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    strncpy(rcolumn, bson_iterator_key(&it2), CF_SMALLBUF - 1);

                    rkept = 0;
                    rrepaired = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_meterkept) == 0)
                        {
                            rkept = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_meterrepaired) == 0)
                        {
                            rrepaired = bson_iterator_double(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in meter", bson_iterator_key(&it3));
                        }
                    }

                    found = true;

                    if (!hh)
                    {
                        hh = CreateEmptyHubHost();
                    }

                    PrependRlistAlienUnlocked(&record_list, NewHubMeter(hh, *rcolumn, rkept, rrepaired));
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryPerformance(EnterpriseDB *conn, char *keyHash, char *lname, bool regex, int sort,
                                HostClassFilter *hostClassFilter)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rname[CF_MAXVARSIZE],
        rhandle[CF_MAXVARSIZE];
    int match_name, found = false;
    double rsigma, rex, rq;
    time_t rtime;
/* BEGIN query document */

    bson query;

    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_performance, 1);
    bson_finish(&fields);

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_performance) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&it2));

                    snprintf(rname, sizeof(rname), "(unknown)");
                    rex = 0;
                    rsigma = 0;
                    rtime = 0;
                    rq = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_perf_event) == 0)
                        {
                            snprintf(rname, sizeof(rname), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_obs_q) == 0)
                        {
                            rq = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_obs_E) == 0)
                        {
                            rex = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_obs_sigma) == 0)
                        {
                            rsigma = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            rtime = bson_iterator_int(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in performance", bson_iterator_key(&it3));
                        }
                    }

                    match_name = true;

                    if (regex)
                    {
                        if (!NULL_OR_EMPTY(lname) && !StringMatchFull(lname, rname))
                        {
                            match_name = false;
                        }
                    }
                    else
                    {
                        if (!NULL_OR_EMPTY(lname) && (strcmp(lname, rname) != 0))
                        {
                            match_name = false;
                        }
                    }

                    if (match_name)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list,
                                          NewHubPerformance(hh, rname, rtime, rq, rex, rsigma, rhandle));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortPerformance);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QuerySetuid(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                           HostClassFilter *hostClassFilter)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rname[CF_MAXVARSIZE];
    int match_name, found = false;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_setuid, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_setuid) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    strncpy(rname, bson_iterator_string(&it2), CF_SMALLBUF - 1);

                    match_name = true;

                    if (regex)
                    {
                        if (!NULL_OR_EMPTY(lname) && !StringMatchFull(lname, rname))
                        {
                            match_name = false;
                        }
                    }
                    else
                    {
                        if (!NULL_OR_EMPTY(lname) && (strcmp(lname, rname) != 0))
                        {
                            match_name = false;
                        }
                    }

                    if (match_name)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list, NewHubSetUid(hh, rname));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);
    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryFileChanges(EnterpriseDB *conn, char *keyHash, char *lname, bool regex, time_t from, time_t to,
                                int sort, HostClassFilter *hostClassFilter)
{
/* BEGIN query document */
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_filechanges, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_ARCHIVE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
          *host_list = NULL;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {        
        char keyhash[CF_MAXVARSIZE] = {0},
             hostnames[CF_BUFSIZE] = {0},
             addresses[CF_BUFSIZE] = {0},
             rname[CF_BUFSIZE] = {0},
             handle[CF_MAXVARSIZE];

        bool found = false;
        HubHost *hh = CreateEmptyHubHost();

        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_filechanges) == 0)
            {
                bson_iterator it2;
                bson_iterator_subiterator(&it1, &it2);

                rname[0] = '\0';
                time_t timestamp = 0;

                while (bson_iterator_next(&it2))
                {
                    bson_iterator it3;
                    bson_iterator_subiterator(&it2, &it3);

                    snprintf(handle, CF_MAXVARSIZE, "%s", bson_iterator_key(&it2));

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_name) == 0)
                        {
                            strncpy(rname, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            timestamp = bson_iterator_int(&it3);
                        }
                    }

                    bool matched = true;

                    matched &= IsTimeWithinRange(from, to, timestamp);
                    matched &= CompareStringOrRegex(rname, lname, regex);

                    if (matched)
                    {
                        found = true;

                        PrependRlistAlienUnlocked(&record_list, NewHubFileChanges(hh, rname, timestamp, handle));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
        else
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortFileChanges);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryFileDiff(EnterpriseDB *conn, char *keyHash, char *lname, char *ldiff, bool regex,
                             time_t from, time_t to, int sort, HostClassFilter *hostClassFilter)
{
/* BEGIN query document */
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_filediffs, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_ARCHIVE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
          *host_list = NULL;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        char keyhash[CF_MAXVARSIZE] = {0},
             hostnames[CF_BUFSIZE] = {0},
             addresses[CF_BUFSIZE] = {0};

        HubHost *hh = CreateEmptyHubHost();
        bool found = false;

        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {                
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_filediffs) == 0)
            {
                bson_iterator it2;
                bson_iterator_subiterator(&it1, &it2);

                char rname[CF_MAXVARSIZE] = {0},
                     rdiff[CF_BUFSIZE] = {0};

                time_t timestamp = 0;

                while (bson_iterator_next(&it2))
                {
                    bson_iterator it3;
                    bson_iterator_subiterator(&it2, &it3);

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_name) == 0)
                        {
                            snprintf(rname, sizeof(rname), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_diff) == 0)
                        {
                            snprintf(rdiff, sizeof(rdiff), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            timestamp = bson_iterator_int(&it3);
                        }
                    }

                    bool matched = true;

                    matched &= IsTimeWithinRange(from, to, timestamp);
                    matched &= CompareStringOrRegex(rname, lname, regex);
                    matched &= CompareStringOrRegex(rdiff, ldiff, regex);

                    if (matched)
                    {
                        found = true;

                        PrependRlistAlienUnlocked(&record_list, NewHubFileDiff(hh, rname, rdiff, timestamp));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
        else
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortFileDiff);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

static int QueryInsertHostInfo(EnterpriseDB *conn, Rlist *host_list)
/**
 * Do a db lookup from keyhash to (hostname,ip) for all hosts in the list.
 **/
{
    HubHost *hh;
    bson_iterator it1;
    char keyHash[CF_MAXVARSIZE], hostNames[CF_MAXVARSIZE], ipAddrs[CF_MAXVARSIZE];

    if (host_list == NULL)
    {
        return false;
    }

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_finish(&fields);

    // use empty query for now - filter result manually
    bson query;

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, bson_empty(&query), &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        keyHash[0] = '\0';
        ipAddrs[0] = '\0';
        hostNames[0] = '\0';

        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyHash, ipAddrs, hostNames);
        }

        hh = GetHubHostIn(host_list, keyHash);

        if (hh)
        {
            if (*ipAddrs != '\0')
            {
                hh->ipaddr = xstrdup(ipAddrs);
            }

            if (*hostNames != '\0')
            {
                hh->hostname = xstrdup(hostNames);
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return true;
}

/*****************************************************************************/
int CFDB_QueryPromiseLogFromMain(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                                 const char *lhandle, bool regex, const char *lcause_rx, time_t from, time_t to, int sort,
                                 HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list)
{

    char *promiseLogKey;
    switch (state)
    {
    case PROMISE_LOG_STATE_REPAIRED:
        promiseLogKey = MONGO_LOGS_REPAIRED_COLL;
        break;
    case PROMISE_LOG_STATE_NOTKEPT:
    default:
        promiseLogKey = MONGO_LOGS_NOTKEPT_COLL;
        break;
    }

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, promiseLogKey, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    int count = 0;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator itHostData;
        bson_iterator_init(&itHostData, mongo_cursor_bson( cursor ));

        HubHost *hh = NULL;
        char keyhash[CF_MAXVARSIZE] = {0};
        char addresses[CF_MAXVARSIZE] = {0};
        char hostnames[CF_MAXVARSIZE] = {0};
        char rhandle[CF_MAXVARSIZE] = {0};
        char rcause[CF_BUFSIZE] = {0};
        time_t rt = 0;
        bool found = false;

        while (bson_iterator_next(&itHostData))
        {

            CFDB_ScanHubHost(&itHostData, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&itHostData), promiseLogKey) == 0)    // new format
            {
                bson_iterator iterPromiseLogElement;
                bson_iterator_subiterator(&itHostData, &iterPromiseLogElement);

                while (bson_iterator_next(&iterPromiseLogElement))
                {
                    bson_iterator iterPromiseLogData;

                    bson_iterator_subiterator(&iterPromiseLogElement, &iterPromiseLogData);

                    bson objPromiseLogData;
                    bson_iterator_subobject( &iterPromiseLogElement, &objPromiseLogData);

                    BsonStringWrite(rhandle, sizeof(rhandle) - 1, &objPromiseLogData, cfr_promisehandle);
                    BsonStringWrite(rcause, sizeof(rcause) - 1, &objPromiseLogData, cfr_cause);

                    if(!CompareStringOrRegex(rhandle, lhandle, regex) || !CompareStringOrRegex(rcause, lcause_rx, regex))
                    {
                        continue;
                    }

                    const bson *array = BsonGetArrayValue(&objPromiseLogData, cfr_time);

                    bson_iterator iterTimestamps;
                    bson_iterator_init(&iterTimestamps, array);

                    while (bson_iterator_next(&iterTimestamps))
                    {
                        rt = bson_iterator_int(&iterTimestamps);

                        if(!IsTimeWithinRange(from, to, rt))
                        {
                            continue;
                        }

                        found = true;
                        count++;

                        if(!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(record_list, NewHubPromiseLog(hh, rhandle, rcause, rt));
                    }
                }
            }
        }

        if(found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if(count > 0 && sort)
    {
        *record_list = SortRlist(*record_list, SortPromiseLog);
    }

    return count;
}


HubQuery *CFDB_QueryPromiseLogSummary(EnterpriseDB *conn, const char *hostkey, PromiseLogState state, const char *handle,
                                      bool regex, const char *cause, time_t from, time_t to, bool sort, HostClassFilter *host_class_filter)
{
    HubQuery *hq = CFDB_QueryPromiseLog(conn, hostkey, state, handle, regex, cause, from, to, false, host_class_filter, NULL);

    Map *log_counts = MapNew(HubPromiseLogHash, HubPromiseLogEqual, NULL, free);
    for (const Rlist *rp = hq->records; rp; rp = rp->next)
    {
        HubPromiseLog *record = (HubPromiseLog *)rp->item;
        int *count = MapGet(log_counts, record);

        if (!count)
        {
            count = xmalloc(sizeof(int));
            *count = 0;
            MapInsert(log_counts, record, count);
        }

        *count = *count + 1;
    }

    Rlist *sum_records = NULL;
    MapIterator iter = MapIteratorInit(log_counts);
    MapKeyValue *item;
    while ((item = MapIteratorNext(&iter)))
    {
        const HubPromiseLog *record = (const HubPromiseLog *)item->key;
        const int *count = (const int *)item->value;
        PrependRlistAlienUnlocked(&sum_records, NewHubPromiseSum(NULL, record->handle, record->cause, *count, 0));
    }

    for (Rlist *rp = hq->records; rp; rp = rp->next)
    {
        DeleteHubPromiseLog(rp->item);
        rp->item = NULL;
    }
    DeleteRlist(hq->records);
    MapDestroy(log_counts);

    hq->records = sort ? SortRlist(sum_records, HubPromiseSumCompare) : sum_records;

    return hq;
}


HubQuery *CFDB_QueryPromiseLog(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                               const char *lhandle, bool regex, const char *lcause_rx, time_t from, time_t to, int sort,
                               HostClassFilter *hostClassFilter, int *total_results_out)
{
    Rlist *record_list = NULL;
    Rlist *host_list = NULL;

    int new_data_count = CFDB_QueryPromiseLogFromMain(conn, keyHash, state, lhandle, regex, lcause_rx, from, to, sort, hostClassFilter, &host_list, &record_list);

    int old_data_count = CFDB_QueryPromiseLogFromOldColl(conn, keyHash, state, lhandle, regex, lcause_rx, from, to, sort, hostClassFilter, &host_list, &record_list);

    if(old_data_count > 0)
    {
        record_list = SortRlist(record_list, SortPromiseLog);
    }

    if (total_results_out)
    {
        *total_results_out = new_data_count + old_data_count;
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/
int CFDB_QueryPromiseLogFromOldColl(EnterpriseDB *conn, const char *keyHash, PromiseLogState state,
                                    const char *lhandle, bool regex, const char *lcause_rx, time_t from, time_t to, int sort,
                                    HostClassFilter *hostClassFilter, Rlist **host_list, Rlist **record_list)
{
    char *collName;

    switch (state)
    {
    case PROMISE_LOG_STATE_REPAIRED:
        collName = MONGO_LOGS_REPAIRED;
        break;
    case PROMISE_LOG_STATE_NOTKEPT:
    default:
        collName = MONGO_LOGS_NOTKEPT;
        break;
    }

    if(!CFDB_CollectionHasData(conn, collName))
    {
        CfOut(cf_inform, "", " !! Old (deprecated) Collection \"%s\" is empty.", collName);
        return 0;
    }

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query,cfr_keyhash,keyHash);
    }


    if (!NULL_OR_EMPTY(lhandle))  // promise handle
    {
        if(regex)
        {
            bson_append_regex(&query,cfr_promisehandle,lhandle,"");
        }
        else
        {
            bson_append_string(&query,cfr_promisehandle,lhandle);
        }
    }

    if(from || to)
    {
        bson_append_start_object(&query, cfr_time);

        if(from)
        {
            bson_append_int(&query, "$gte",from);
        }

        if(to)
        {
            bson_append_int(&query, "$lte",to);
        }

        bson_append_finish_object(&query);
    }

    AppendHostKeys(conn, &query, hostClassFilter);

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields,cfr_keyhash,1);
    bson_append_int(&fields,cfr_cause,1);
    bson_append_int(&fields,cfr_promisehandle,1);
    bson_append_int(&fields,cfr_time,1);

    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn,collName,&query,&fields,0,0,CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    int count = 0;
    HubHost *hh = NULL;

    char rhandle[CF_MAXVARSIZE] = {0};
    char rcause[CF_BUFSIZE] = {0};
    char keyhash[CF_MAXVARSIZE] = {0};

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson( cursor ) );

        keyhash[0] = '\0';
        rhandle[0] = '\0';
        rcause[0] = '\0';
        time_t rt = 0;

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1),cfr_keyhash) == 0)
            {
                snprintf(keyhash,sizeof(keyhash),"%s",bson_iterator_string(&it1));

                hh = GetHubHostIn(*host_list,keyhash);

                if(!hh)
                {
                    hh = NewHubHost(NULL,keyhash,NULL,NULL);  // we get more host info later
                    PrependRlistAlienUnlocked(host_list,hh);
                }
            }
            else if (strcmp(bson_iterator_key(&it1),cfr_promisehandle) == 0)
            {
                snprintf(rhandle,sizeof(rhandle),"%s",bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1),cfr_cause) == 0)
            {
                snprintf(rcause,sizeof(rcause),"%s",bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1),cfr_time) == 0)
            {
                rt = bson_iterator_int(&it1);
            }
        }

        if(CompareStringOrRegex(rhandle, lhandle, regex) && CompareStringOrRegex(rcause, lcause_rx, true))
        {
            count++;
            PrependRlistAlienUnlocked(record_list,NewHubPromiseLog(hh,rhandle,rcause,rt));
        }
    }

    mongo_cursor_destroy(cursor);

    // now fill in hostnames and ips of the hosts
    if(count > 0)
    {
        QueryInsertHostInfo(conn,*host_list);
    }

    return count;
}

/*****************************************************************************/

HubQuery *CFDB_QueryValueReport(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                                HostClassFilter *hostClassFilter)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    double rkept, rnotkept, rrepaired;
    char rday[CF_MAXVARSIZE], rmonth[CF_MAXVARSIZE], ryear[CF_MAXVARSIZE];
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rhandle[CF_MAXVARSIZE];
    int match_day, match_month, match_year, found = false;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    bson_finish(&query);

    // Turn start_time into Day Month Year

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_valuereport, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_valuereport) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&it2));
                    bson_iterator_subiterator(&it2, &it3);

                    rday[0] = '\0';
                    rkept = 0;
                    rnotkept = 0;
                    rrepaired = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_day) == 0)
                        {
                            strncpy(rday, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_kept) == 0)
                        {
                            rkept = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_notkept) == 0)
                        {
                            rnotkept = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_repaired) == 0)
                        {
                            rrepaired = bson_iterator_double(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in value report", bson_iterator_key(&it3));
                        }
                    }

                    match_day = match_month = match_year = true;

                    if (lday && (strcmp(lday, rday) != 0))
                    {
                        match_day = false;
                    }

                    if (lmonth && (strcmp(lmonth, rmonth) != 0))
                    {
                        match_month = false;
                    }

                    if (lyear && (strcmp(lyear, ryear) != 0))
                    {
                        match_year = false;
                    }

                    if (match_day && match_month && match_year)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list,
                                          NewHubValue(hh, rday, rkept, rrepaired, rnotkept, rhandle));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortBusinessValue);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryValueGraph(EnterpriseDB *conn, char *keyHash, char *lday, char *lmonth, char *lyear, int sort,
                               char *classRegex)
{
    bson_iterator it1, it2, it3;
    HubHost *hh;
    Rlist *record_list = NULL, *host_list = NULL;
    double rkept, rnotkept, rrepaired;
    char rday[CF_MAXVARSIZE], rmonth[CF_MAXVARSIZE], ryear[CF_MAXVARSIZE];
    char keyhash[CF_MAXVARSIZE], hostnames[CF_BUFSIZE], addresses[CF_BUFSIZE], rhandle[CF_MAXVARSIZE];
    int match_day, match_month, match_year, found = false;
    char classRegexAnch[CF_MAXVARSIZE];
    struct tm tm = { 0 };
    time_t epoch;

/* BEGIN query document */

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    if (!NULL_OR_EMPTY(classRegex))
    {
        AnchorRegex(classRegex, classRegexAnch, sizeof(classRegexAnch));
        bson_append_regex(&query, cfr_class_keys, classRegexAnch, "");
    }

    bson_finish(&query);

    // Turn start_time into Day Month Year

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_valuereport, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    hostnames[0] = '\0';
    addresses[0] = '\0';

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        found = false;
        hh = NULL;

        while (bson_iterator_next(&it1))
        {
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), cfr_valuereport) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    snprintf(rhandle, CF_MAXVARSIZE, "%s", bson_iterator_key(&it2));
                    bson_iterator_subiterator(&it2, &it3);

                    rday[0] = '\0';
                    rkept = 0;
                    rnotkept = 0;
                    rrepaired = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_day) == 0)
                        {
                            if (strptime(bson_iterator_string(&it3), "%d %b %Y", &tm) != 0)
                            {
                                epoch = mktime(&tm);
                                snprintf(rday, CF_MAXVARSIZE - 1, "%ld", epoch);
                            }
                            else
                            {
                                strcpy(rday, "");
                            }
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_kept) == 0)
                        {
                            rkept = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_notkept) == 0)
                        {
                            rnotkept = bson_iterator_double(&it3);
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_repaired) == 0)
                        {
                            rrepaired = bson_iterator_double(&it3);
                        }
                        else
                        {
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in value report", bson_iterator_key(&it3));
                        }
                    }

                    match_day = match_month = match_year = true;

                    if (lday && (strcmp(lday, rday) != 0))
                    {
                        match_day = false;
                    }

                    if (lmonth && (strcmp(lmonth, rmonth) != 0))
                    {
                        match_month = false;
                    }

                    if (lyear && (strcmp(lyear, ryear) != 0))
                    {
                        match_year = false;
                    }

                    if (match_day && match_month && match_year)
                    {
                        found = true;

                        if (!hh)
                        {
                            hh = CreateEmptyHubHost();
                        }

                        PrependRlistAlienUnlocked(&record_list, NewHubValue(hh, rday, rkept, rrepaired, rnotkept, ""));
                    }
                }
            }
        }

        if (found)
        {
            UpdateHubHost(hh, keyhash, addresses, hostnames);
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortBusinessValue);
    }

    return NewHubQuery(host_list, record_list);
}
/*****************************************************************************/

static void GetOldClientVersions(Rlist **rp)
{
    PrependRScalar(rp, (void *) "cfengine_3_2.*", CF_SCALAR);
    PrependRScalar(rp, (void *) "cfengine_3_1.*", CF_SCALAR);
    PrependRScalar(rp, (void *) "cfengine_3_0.*", CF_SCALAR);
}

static void SkipOldClientVersionsFilter(bson *b)
/* NOTE: Ignore data from agent versions < 3.3.0 */
{
    if (b == NULL)
    {
        return;
    }

    Rlist *old_client_versions = NULL;
    GetOldClientVersions(&old_client_versions);

    {
        bson_append_start_object(b, cfr_class_keys);
        BsonAppendArrayRx(b, "$nin", old_client_versions);
        bson_append_finish_object(b);
    }

    DeleteRlist(old_client_versions);
}

/*****************************************************************************/

int CFDB_CountSkippedOldAgents(EnterpriseDB *conn, char *keyhash,
                               HostClassFilter *host_class_filter)
/* NOTE: BundleSeen report is not compatible with agent versions < 3.3.0
 * and they are ignored during report generation. This fucntion count this skipped
 * hosts from the query.
 */
{
    int result = -1;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyhash))
    {
        bson_append_string(&query, cfr_keyhash, keyhash);
    }

    BsonAppendHostClassFilter(&query, host_class_filter);

    /* Search only for old agents (< 3.3.0) */
    Rlist *old_client_versions = NULL;
    GetOldClientVersions(&old_client_versions);

    bson_append_start_object(&query, cfr_class_keys);
    BsonAppendArrayRx(&query, "$in", old_client_versions);
    bson_append_finish_object(&query);

    DeleteRlist(old_client_versions);

    bson_finish(&query);

    result = mongo_count(conn, MONGO_BASE, MONGO_HOSTS_COLLECTION, &query);

    bson_destroy(&query);

    return result;
}

/*****************************************************************************/

HubQuery *CFDB_QueryBundleSeen(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, int sort)
{
    unsigned long blue_horizon;
    if (!CFDB_GetBlueHostThreshold(&blue_horizon))
    {
        assert(false && "Could not determine blue horizon");
        blue_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;
    }

    /* BEGIN query document */
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    /* Ignore data from client versions < 3.3.0
     * Old clients reported time (hours ago),
     * whereas from version 3.3.0 the clients report compliance level
     *
     * NOTE: this check can be removed after all clients are upgraded to version >= 3.3.0
     */

    SkipOldClientVersionsFilter(&query);

    bson_finish(&query);

    /* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_bundles, 1);
    bson_append_int(&fields, cfr_time, 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

    /* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
            *host_list = NULL;

    time_t blueHorizonTimestamp = time(NULL) - blue_horizon;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {        
        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        HubHost *hh = CreateEmptyHubHost();
        bool found = BsonIterGetBundleReportDetails(&it1, lname, regex, blueHorizonTimestamp, hh, &record_list );

        if (found)
        {           
            PrependRlistAlienUnlocked(&host_list, hh);
        }
        else
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
    }

    mongo_cursor_destroy(cursor);

    if (sort)
    {
        record_list = SortRlist(record_list, SortBundleSeen);
    }

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

HubQuery *CFDB_QueryWeightedBundleSeen(EnterpriseDB *conn, char *keyHash, char *lname, bool regex,
                               HostClassFilter *hostClassFilter, HostColourFilter *hostColourFilter, int sort)
{
    unsigned long blue_horizon;
    if (!CFDB_GetBlueHostThreshold(&blue_horizon))
    {
        assert(false && "Could not determine blue horizon");
        blue_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;
    }

/* BEGIN query document */

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(keyHash))
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
    }

    BsonAppendHostClassFilter(&query, hostClassFilter);

    /* Ignore data from client versions < 3.3.0
     * Old clients reported time (hours ago),
     * whereas from version 3.3.0 the clients report compliance level
     *
     * NOTE: this check can be removed after all clients are upgraded to version >= 3.3.0
     */

    SkipOldClientVersionsFilter(&query);

    bson_finish(&query);

    /* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_bundles, 1);
    bson_append_int(&fields, cfr_time, 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

    /* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *record_list = NULL,
            *host_list = NULL;

    time_t blueHorizonTime = time(NULL) - blue_horizon;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator it1;
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        Rlist *record_list_single_host = NULL;

        HubHost *hh = CreateEmptyHubHost();
        bool found = BsonIterGetBundleReportDetails(&it1, lname, regex, blueHorizonTime, hh, &record_list_single_host );

        HubQuery *hq = NewHubQuery(NULL, record_list_single_host);

        bool hostDataAdded = false;

        if(found)
        {
            double totalHostCompliance = 0;
            long totalRelevantBundlesInHost = 0;
            bool blueOrBlackHost = true;

            if(hh->colour == HOST_COLOUR_GREEN_YELLOW_RED)
            {
                /* count bundles for current host */
                Rlist *rp = NULL;

                for(rp = hq->records; rp != NULL; rp = rp->next)
                {
                    HubBundleSeen *hbTemp = (HubBundleSeen *) rp->item;

                    if(isnan(hbTemp->bundlecomp)) /* discard nan values*/
                    {
                        continue;
                    }

                    totalHostCompliance += hbTemp->bundlecomp;
                    totalRelevantBundlesInHost++;                    
                }

                /* calculate average bundle compliance for current host */

                if(totalRelevantBundlesInHost)
                {                    
                    double avgCompliance = totalHostCompliance / totalRelevantBundlesInHost;

                    if(avgCompliance > 0.8)
                    {
                        hh->colour = HOST_COLOUR_GREEN;
                    }
                    else
                    {
                        hh->colour = HOST_COLOUR_RED;
                    }

                    blueOrBlackHost = false;
                }
                else
                {
                     /* If host has no data within the blue_horizon, mark it as blue */

                    hh->colour = HOST_COLOUR_BLUE;
                }

                if(!hostColourFilter ||  (hostColourFilter && (hostColourFilter->colour == hh->colour)))
                {
                    for(Rlist *rp = hq->records; rp != NULL; rp = rp->next)
                    {
                        HubBundleSeen *hbTemp = (HubBundleSeen *) rp->item;

                        /*  Don't include nan values in the calculation */
                        if(isnan(hbTemp->bundlecomp))
                        {
                            continue;
                        }

                        PrependRlistAlienUnlocked(&record_list, NewHubBundleSeen(hbTemp->hh, hbTemp->bundle, hbTemp->bundlecomp, hbTemp->bundleavg, hbTemp->bundledev, hbTemp->t));
                        hostDataAdded = true;
                    }
                }
            }

            if(blueOrBlackHost)
            {
                if(!hostColourFilter ||  (hostColourFilter && (hostColourFilter->colour == hh->colour)))
                {
                    for(Rlist *rp = hq->records; rp != NULL; rp = rp->next)
                    {
                        HubBundleSeen *hbTemp = (HubBundleSeen *) rp->item;
                        PrependRlistAlienUnlocked(&record_list, NewHubBundleSeen(hbTemp->hh, hbTemp->bundle, hbTemp->bundlecomp, hbTemp->bundleavg, hbTemp->bundledev, hbTemp->t));
                        hostDataAdded = true;
                    }
                }
            }
        }

        /* cleanup */

        DeleteHubQuery(hq, DeleteHubBundleSeen);
        hq = NULL;
        record_list_single_host = NULL;

        if(!hostDataAdded)
        {
            DeleteHubHost(hh);
            hh = NULL;
        }
        else
        {
            PrependRlistAlienUnlocked(&host_list, hh);
        }
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(host_list, record_list);
}

/*****************************************************************************/

Item *CFDB_QueryVitalIds(EnterpriseDB *conn, char *keyHash)
/**
 * Return a list of mag probe ids, possibly restrict to one host.
 * Can be extended to query for only global probes, etc.
 */
{
    Item *retVal;

    bson query;
    bson_init(&query);

    if (keyHash != NULL)
    {
        bson_append_string(&query, cfr_keyhash, keyHash);
        bson_finish(&query);
    }
    else
    {
        bson_empty(&query);
    }

    // we use the magnified collection as this is updated most often
    retVal = CFDB_QueryDistinct(conn, MONGO_BASE, "monitoring_mg", cfm_id, &query);

    bson_destroy(&query);

    return retVal;
}

/*****************************************************************************/

HubVital *CFDB_QueryVitalsMeta(EnterpriseDB *conn, char *keyHash)
/**
 * Return a list of mag vital ids and meta-data, restricted to one host.
 */
{
    bson_iterator it1;
    HubVital *hv = NULL;
    char id[CF_MAXVARSIZE];
    char units[CF_MAXVARSIZE];
    char description[CF_MAXVARSIZE];

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyHash);
    bson_finish(&query);

    // field
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfm_id, 1);
    bson_append_int(&fields, cfm_units, 1);
    bson_append_int(&fields, cfm_description, 1);
    bson_finish(&fields);

    // use mag collection since it is updated most frequently
    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE_MON_MG, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        id[0] = '\0';
        units[0] = '\0';
        description[0] = '\0';

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfm_id) == 0)
            {
                snprintf(id, sizeof(id), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_units) == 0)
            {
                snprintf(units, sizeof(units), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_description) == 0)
            {
                snprintf(description, sizeof(description), "%s", bson_iterator_string(&it1));
            }
        }

        hv = PrependHubVital(&hv, id, units, description);
    }

    mongo_cursor_destroy(cursor);

    return hv;
}

/*****************************************************************************/

static int Nova_MagViewOffset(int start_slot, int db_slot, int wrap)
{
    int offset = CF_MAX_SLOTS - start_slot;

// Offset is the non-wrapped data size

    if (wrap >= 0 && db_slot < start_slot)
    {
        return offset + db_slot;        // assumes db_slot is now < CF_MAGDATA
    }
    else
    {
        return db_slot - start_slot;
    }
}

int CFDB_QueryMagView2(EnterpriseDB *conn, char *keyhash, char *monId, time_t start_time, double *qa, double *ea, double *da, double *ga)
{
    bson_iterator it1, it2;
    int ok = false, i, start_slot, wrap_around, windowSlot;
    double *monArr = NULL;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyhash);
    bson_append_string(&query, cfm_id, monId);
    bson_finish(&query);

    // result
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfm_q_arr, 1);
    bson_append_int(&fields, cfm_expect_arr, 1);
    bson_append_int(&fields, cfm_deviance_arr, 1);
    bson_append_int(&bb, cfm_grad_arr, 1);
    bson_finish(&fields);

/* Check from wrap around */

    start_slot = GetTimeSlot(start_time);

// Check that start + 4 hours is not greater than the week buffer

    wrap_around = (int) start_slot + CF_MAGDATA - CF_MAX_SLOTS;

// Initialize as there might be missing values

    for (i = 0; i < CF_MAGDATA; i++)
    {
        qa[i] = -1;
        ea[i] = 0;
        da[i] = 0;
        ga[i] = 0;
    }

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE_MON_MG, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // max one document
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))        // q, e, or d array
        {

            if (strcmp(bson_iterator_key(&it1), cfm_q_arr) == 0)
            {
                monArr = qa;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_expect_arr) == 0)
            {
                monArr = ea;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_deviance_arr) == 0)
            {
                monArr = da;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_grad_arr) == 0)
            {
                monArr = ga;
            }
            else
            {
                monArr = NULL;
                continue;
            }

            bson_iterator_subiterator(&it1, &it2);

            for (i = 0; bson_iterator_next(&it2); i++)  // array elements
            {
                // Select the past 4 hours

                if (wrap_around >= 0)
                {
                    if (i >= wrap_around || i < start_slot)
                    {
                        continue;
                    }
                }
                else
                {
                    if (i < start_slot || i >= start_slot + CF_MAGDATA)
                    {
                        continue;
                    }
                }

                ok = true;      // Have some relevant data

                // index 0 is 4 hrs ago, 71 is now (4 * 12 - 1)
                windowSlot = Nova_MagViewOffset(start_slot, i, wrap_around);

                //printf("getting index %d <- %d\n", windowSlot, i); 

                monArr[windowSlot] = bson_iterator_double(&it2);
            }

        }
    }

// Now we should transform the data to re-order during wrap-around,
// since at the boundary the data come in the wrong order

    mongo_cursor_destroy(cursor);
    return ok;
}

/*****************************************************************************/

int CFDB_QueryMonView(EnterpriseDB *conn, char *keyhash, char *monId, enum monitord_rep rep_type, double *qa, double *ea, double *da, double *ga)
{
    bson_iterator it1, it2;
    double *monArr = NULL;
    int ok = false;
    char *db;
    int numSlots = 0;
    int i;

    switch (rep_type)
    {                           // mag handled by separate function
    case mon_rep_week:
        db = MONGO_DATABASE_MON_WK;
        numSlots = CF_TIMESERIESDATA;   // every hour
        break;
    case mon_rep_yr:
        db = MONGO_DATABASE_MON_YR;
        numSlots = CF_YEAR_SLOTS;       // every week
        break;
    default:
        CfOut(cf_error, "", "!! Undefined monitord type in query (%d)", rep_type);
        FatalError("Software error");
    }

// Initialize as there might be missing values

    for (i = 0; i < numSlots; i++)
    {
        qa[i] = -1;
        ea[i] = 0;
        da[i] = 0;
        ga[i] = 0;
    }

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyhash);
    bson_append_string(&query, cfm_id, monId);
    bson_finish(&query);

    // result

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfm_q_arr, 1);
    bson_append_int(&fields, cfm_expect_arr, 1);
    bson_append_int(&fields, cfm_deviance_arr, 1);
    bson_append_int(&bb, cfm_grad_arr, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, db, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // only one document
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))        // q, e, or d array
        {

            if (strcmp(bson_iterator_key(&it1), cfm_q_arr) == 0)
            {
                monArr = qa;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_expect_arr) == 0)
            {
                monArr = ea;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_deviance_arr) == 0)
            {
                monArr = da;
            }
            else if (strcmp(bson_iterator_key(&it1), cfm_grad_arr) == 0)
            {
                monArr = ga;
            }
            else
            {
                monArr = NULL;
                continue;
            }

            bson_iterator_subiterator(&it1, &it2);

            for (i = 0; bson_iterator_next(&it2); i++)  // array elements
            {
                if (i >= numSlots)
                {
                    CfOut(cf_error, "", "!! Index %d out of bounds when querying monitoring data", i);
                    break;
                }

                ok = true;      // Have some relevant data

                monArr[i] = bson_iterator_double(&it2);
            }
        }
    }

    mongo_cursor_destroy(cursor);
    return ok;
}

/*****************************************************************************/

int CFDB_CountHosts(EnterpriseDB *conn, HostClassFilter *host_class_filter, HostColourFilter *host_colour_filter)
{
    bson query;
    bson_init(&query);

    BsonAppendHostClassFilter(&query, host_class_filter);
    BsonAppendHostColourFilter(&query, host_colour_filter);
    bson_finish(&query);

    int count = CFDB_CountHostsGeneric(conn, &query);

    bson_destroy(&query);

    return count;
}

/*****************************************************************************/

bool CFDB_HasMatchingHost(EnterpriseDB *conn, char *hostKey, HostClassFilter *hostClassFilter)
{
    assert(SafeStringLength(hostKey) > 0);

    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostKey);
    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    int count = CFDB_CountHostsGeneric(conn, &query);

    bson_destroy(&query);

    assert(count >= 0 && count <= 1);

    return (count == 1);
}

/*****************************************************************************/

int CFDB_CountHostsGeneric(EnterpriseDB *conn, bson *query)
/**
 * Counts number of hosts matching the given query.
 **/
{
    return (int) mongo_count(conn, MONGO_BASE, MONGO_HOSTS_COLLECTION, query);
}

/*****************************************************************************/

int CFDB_QueryHostName(EnterpriseDB *conn, char *ipAddr, char *hostName, int hostNameSz)
/*
 * Scan DB to try to find the hostname of the given IP.
 * Falls back to ip addres (parameter).
 */
{
    bson_iterator it1, it2;
    int ret = false;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_ip_array, ipAddr);
    bson_finish(&query);

    // result
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // take first match
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_host_array) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                if (bson_iterator_next(&it2))
                {
                    snprintf(hostName, hostNameSz, "%s", bson_iterator_string(&it2));
                    ret = true;
                    break;
                }

            }
        }

    }

    mongo_cursor_destroy(cursor);

    // fallback
    if (ret == false)
    {
        snprintf(hostName, hostNameSz, "%s", ipAddr);
    }

    return ret;

}

/*****************************************************************************/

bool CFDB_QueryLastUpdate(EnterpriseDB *conn, char *db, char *dbkey, char *keyhash, time_t *date, int *size)
{
    bson_iterator it1;
    bool ok = false;

/* BEGIN query document */

    bson query;
    bson_init(&query);
    bson_append_string(&query, dbkey, keyhash);
    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, dbkey, 1);
    bson_append_int(&fields, cfr_day, 1);
    bson_append_int(&fields, cfr_last_update_size, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, db, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_day) == 0)
            {
                *date = (time_t) bson_iterator_int(&it1);
                ok = true;
            }

            if (strcmp(bson_iterator_key(&it1), cfr_last_update_size) == 0)
            {
                *size = bson_iterator_int(&it1);
            }
            
        }
    }

    mongo_cursor_destroy(cursor);
    return ok;
}

 /*****************************************************************************/

bool CFDB_QueryHistogram(EnterpriseDB *conn, char *keyhash, char *monId, double *histo)
{
    bson_iterator it1, it2;
    bool found = false;
    int i;

    for (i = 0; i < CF_GRAINS; i++)
    {
        histo[i] = 0;
    }

/* BEGIN query document */
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyhash);
    bson_append_string(&query, cfm_id, monId);
    bson_finish(&query);

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_histo, 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE_MON_MG, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // only one doc
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfr_histo) == 0)
            {
                i = 0;
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (i >= CF_GRAINS)
                    {
                        CfOut(cf_error, "", "!! Index %d out of bounds when querying histograms", i);
                        break;
                    }

                    histo[i] = bson_iterator_double(&it2);
                    i++;
                    found = true;
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);
    return found;
}

/*****************************************************************************/
/* Promises collections                                                      */
/*****************************************************************************/

int CFDB_QueryPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey, char *attrVal, int attrValSz)
/*
 * For the promise with the given handle, returns the given field
 * (e.g. comment, promisee, etc.)
 */
{
    bson_iterator it1;
    int found = false;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_handle, handle);
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, attrKey, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // take first doc should be (unique)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), attrKey) == 0)
            {
                snprintf(attrVal, attrValSz, "%s", bson_iterator_string(&it1));
                found = true;
                break;
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return found;
}

/*****************************************************************************/

Item *CFDB_QueryExpandedPromiseAttr(EnterpriseDB *conn, char *handle, char *attrKey)
/*
 * For the promise with the given (expanded) handle, returns a list of
 * the given field (e.g. cfp_comment_exp, cfp_promisee_exp, etc.) expanded.  
 * MEMORY NOTE: Caller must free returned val (!=NULL) with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *matched = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_handle_exp, handle);
    bson_finish(&query);

// returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, attrKey, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), attrKey) == 0)
            {

                // constraints are stored in an array
                if (strcmp(attrKey, cfp_constraints_exp) == 0)
                {
                    bson_iterator_subiterator(&it1, &it2);

                    while (bson_iterator_next(&it2))
                    {
                        IdempPrependItem(&matched, (char *) bson_iterator_string(&it2), NULL);
                    }
                }
                else
                {
                    IdempPrependItem(&matched, (char *) bson_iterator_string(&it1), NULL);
                }

                break;
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return matched;
}

/*****************************************************************************/

HubQuery *CFDB_QueryHandlesForBundlesWithComments(EnterpriseDB *conn, char *bType, char *bName)
{
    bson_iterator it1;
    Rlist *recordList = NULL;
    char handle[CF_MAXVARSIZE] = { 0 }, comment[CF_BUFSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(bType))
    {
        bson_append_string(&query, cfp_bundletype, bType);
        bson_append_string(&query, cfp_bundlename, bName);
    }

    bson_finish(&query);

// returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_handle, 1);
    bson_append_int(&fields, cfp_comment, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        handle[0] = '\0';
        comment[0] = '\0';
        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_handle) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", (char *) bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_comment) == 0)
            {
                snprintf(comment, sizeof(comment), "%s", (char *) bson_iterator_string(&it1));
            }

        }

        if (strlen(handle) > 0 || strlen(comment) > 0)
        {
            PrependRlistAlienUnlocked(&recordList,
                              NewHubPromise(NULL, NULL, NULL, NULL, NULL, NULL, NULL, handle, comment, NULL, 0, NULL));
        }
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

HubQuery *CFDB_QueryPromiseHandles(EnterpriseDB *conn, char *promiser, char *promiserType, char *bType, char *bName,
                                   bool regex, bool filter)
/*
 * Returns a set of handles of promises matching given promiser regex
 * XOR promise type XOR (bundle type, bundle name) XOR all.  All
 * promiser types of vars and classes, and bundle types of edit_line
 * and server may optinally be excluded since they often only lead to
 * mess in Knowledge Management. 
 */
{
    bson_iterator it1;
    Rlist *recordList = NULL;

    bson query;
    bson_init(&query);

    if (regex)
    {
        if (!NULL_OR_EMPTY(promiser))
        {
            bson_append_regex(&query, cfp_promiser, promiser, "");
        }
        else if (!NULL_OR_EMPTY(promiserType))
        {
            bson_append_regex(&query, cfp_promisetype, promiserType, "");
        }
        else if (!NULL_OR_EMPTY(bType))
        {
            bson_append_regex(&query, cfp_bundletype, bType, "");
            bson_append_regex(&query, cfp_bundlename, bName, "");
        }
    }
    else
    {
        if (!NULL_OR_EMPTY(promiser))
        {
            bson_append_string(&query, cfp_promiser, promiser);
        }
        else if (!NULL_OR_EMPTY(promiserType))
        {
            bson_append_string(&query, cfp_promisetype, promiserType);
        }
        else if (!NULL_OR_EMPTY(bType))
        {
            bson_append_string(&query, cfp_bundletype, bType);
            bson_append_string(&query, cfp_bundlename, bName);
        }
    }

    if (filter)
    {
        // filter promises of type vars and classes
        {
            bson_append_start_object(&query, cfp_promisetype);
            {
                bson_append_start_array(&query, "$nin");

                bson_append_string(&query, "0", "vars");
                bson_append_string(&query, "1", "classes");

                bson_append_finish_object(&query);
            }
            bson_append_finish_object(&query);
        }

        // filter promises in edit_line and server bundles
        {
            bson_append_start_object(&query, cfp_bundletype);
            {
                bson_append_start_array(&query, "$nin");

                bson_append_string(&query, "0", "edit_line");
                bson_append_string(&query, "1", "server");

                bson_append_finish_object(&query);
            }
            bson_append_finish_object(&query);
        }
    }

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_handle, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_handle) == 0)
            {
                PrependRlistAlienUnlocked(&recordList,
                                  NewHubPromise(NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                (char *) bson_iterator_string(&it1), NULL, NULL, 0, NULL));
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

HubQuery *CFDB_QueryPromises(EnterpriseDB *conn, PromiseFilter *filter)
/*
 * Using PromiseFilter, can over time replace the other promise query functions.
 * FIXME: If not found by handle: may want to do second lookup in expanded promise db
 *        using file,lineno as keys (in case the handle contains a variable).
 *        - see previous CFDB_QueryPromise() (VCS history)
 */
{
    bson query;

    bson_init(&query);
    BsonAppendPromiseFilter(&query, filter);
    bson_finish(&query);

    bson fields;

    bson_init(&fields);
    bson_append_int(&fields, cfp_bundlename, 1);
    bson_append_int(&fields, cfp_bundletype, 1);
    bson_append_int(&fields, cfp_handle, 1);
    bson_append_int(&fields, cfp_promiser, 1);
    bson_append_int(&fields, cfp_promisee, 1);
    bson_append_int(&fields, cfp_promisetype, 1);
    bson_append_int(&fields, cfp_comment, 1);
    bson_append_int(&fields, cfp_classcontext, 1);
    bson_append_int(&fields, cfp_file, 1);
    bson_append_int(&fields, cfp_lineno, 1);
    bson_append_int(&fields, cfp_bundleargs, 1);
    bson_append_int(&fields, cfp_constraints, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *recordList = NULL;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        char bundleName[CF_MAXVARSIZE], bundleType[CF_MAXVARSIZE];
        char promiseHandle[CF_MAXVARSIZE], promiser[CF_MAXVARSIZE], promisee[CF_MAXVARSIZE];
        char promiseType[CF_MAXVARSIZE], comment[CF_MAXVARSIZE], classContext[CF_MAXVARSIZE];
        char file[CF_MAXVARSIZE];

        BsonStringWrite(bundleName, sizeof(bundleName), &(cursor->current), cfp_bundlename);
        BsonStringWrite(bundleType, sizeof(bundleType), &(cursor->current), cfp_bundletype);
        BsonStringWrite(promiseHandle, sizeof(promiseHandle), &(cursor->current), cfp_handle);
        BsonStringWrite(promiser, sizeof(promiser), &(cursor->current), cfp_promiser);
        BsonStringWrite(promisee, sizeof(promisee), &(cursor->current), cfp_promisee);
        BsonStringWrite(promiseType, sizeof(promiseType), &(cursor->current), cfp_promisetype);
        BsonStringWrite(comment, sizeof(comment), &(cursor->current), cfp_comment);
        BsonStringWrite(classContext, sizeof(classContext), &(cursor->current), cfp_classcontext);
        BsonStringWrite(file, sizeof(file), &(cursor->current), cfp_file);

        int lineNumber = 0;
        BsonIntGet(&(cursor->current), cfp_lineno, &lineNumber);

        Rlist *bundleArgs = BsonStringArrayAsRlist(&(cursor->current), cfp_bundleargs);
        Rlist *constraints = BsonStringArrayAsRlist(&(cursor->current), cfp_constraints);

        PrependRlistAlienUnlocked(&recordList, NewHubPromise(bundleName, bundleType, bundleArgs,
                                                     promiseType, promiser, promisee,
                                                     classContext, promiseHandle, comment,
                                                     file, lineNumber, constraints));
    }

    mongo_cursor_destroy(cursor);

    return NewHubQuery(NULL, recordList);
}

/*****************************************************************************/

HubQuery *CFDB_QueryPromiseBundles(EnterpriseDB *conn, PromiseFilter *filter)
/**
 * Differs from CFDB_QueryPromises() in that it only returns distinct bundles.
 */
{
    bson query;

    bson_init(&query);
    BsonAppendPromiseFilter(&query, filter);
    bson_finish(&query);

    bson fields;

    bson_init(&fields);
    bson_append_int(&fields, cfp_bundlename, 1);
    bson_append_int(&fields, cfp_bundletype, 1);
    bson_append_int(&fields, cfp_bundleargs, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *recordList = NULL;
    Item *bundlesFound = NULL;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        char bundleName[CF_MAXVARSIZE], bundleType[CF_MAXVARSIZE];

        BsonStringWrite(bundleName, sizeof(bundleName), &(cursor->current), cfp_bundlename);
        BsonStringWrite(bundleType, sizeof(bundleType), &(cursor->current), cfp_bundletype);

        if (!ReturnItemInClass(bundlesFound, bundleName, bundleType))
        {
            Rlist *bundleArgs = BsonStringArrayAsRlist(&(cursor->current), cfp_bundleargs);

            PrependRlistAlienUnlocked(&recordList, NewHubPromiseBundle(bundleName, bundleType, bundleArgs));

            PrependItem(&bundlesFound, bundleName, bundleType);
        }
    }

    DeleteItemList(bundlesFound);

    mongo_cursor_destroy(cursor);

    return NewHubQuery(NULL, recordList);

}

/*****************************************************************************/

Rlist *CFDB_QueryBundleClasses(EnterpriseDB *conn, PromiseFilter *filter)
/*
 * Returns the set of classes used in the given bundle.
 * MEMORY NOTE: Caller must free returned value with DeleteRlist()
 */
{
    bson_iterator it1;
    Rlist *classList = { 0 }, *tmpList = { 0 };

    // query
    bson query;
    bson_init(&query);
    BsonAppendPromiseFilter(&query, filter);
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_classcontext, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_classcontext) == 0)
            {
                tmpList = SplitRegexAsRList((char *) bson_iterator_string(&it1), "[.!()|&]+", 100, false);
                IdempAppendRlist(&classList, tmpList, CF_LIST);
                DeleteRlist(tmpList);
            }
        }
    }

    DeleteScope("match");       // avoid leak from bad side-effect of SplitRegexAsRList()

    mongo_cursor_destroy(cursor);

    return classList;
}

/*****************************************************************************/

Item *CFDB_QueryBundlesUsing(EnterpriseDB *conn, PromiseFilter *promiseFilter, char *bNameReferenced)
/*
 * Returns the set of bundle names using the given bundle though
 * methods:usebundle, NULL if none.
 * Bundle types are implicitly agent (both referencing and referees).
 * MEMORY NOTE: Caller must free returned value (!= NULL) with DeleteItemList()
 */
{
    bson_iterator it1;
    Item *bNameReferees = { 0 };
    char queryConstr[CF_MAXVARSIZE];

    snprintf(queryConstr, sizeof(queryConstr), "usebundle => %s", bNameReferenced);

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_constraints, queryConstr);
    BsonAppendPromiseFilter(&query, promiseFilter);
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_bundlename, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
            {
                IdempPrependItem(&bNameReferees, (char *) bson_iterator_string(&it1), "agent");
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return bNameReferees;
}

/*****************************************************************************/

int CFDB_QueryBundleCount(EnterpriseDB *conn)
/**
 * Returns the number of bundles (may appear multiple times in the
 * promises DB).
 **/
{
    bson_iterator it1;
    Item *bundleNames = { 0 };
    int bundleCount = 0;

    // query all

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_bundlename, 1);
    bson_finish(&fields);

    bson query;

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_UNEXP, bson_empty(&query), &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_bundlename) == 0)
            {
                IdempPrependItem(&bundleNames, (char *) bson_iterator_string(&it1), NULL);
            }
        }
    }

    mongo_cursor_destroy(cursor);

    bundleCount = ListLen(bundleNames);

    DeleteItemList(bundleNames);

    return bundleCount;
}

/*****************************************************************************/

int CFDB_QueryPromiseCount(EnterpriseDB *conn)
{
    int promiseCount = 0;

    promiseCount = (int) mongo_count(conn, MONGO_BASE, MONGO_PROMISES_UNEXP_COLLECTION, NULL);

    return promiseCount;
}

/*****************************************************************************/

HubBody *CFDB_QueryBody(EnterpriseDB *conn, char *type, char *name)
/*
 * Returns all attribs of one body by its type and name.
 * MEMORY NOTE: Caller must use DeleteHubBody() on the reutrned val (!=NULL)
 */
{
    bson_iterator it1, it2, it3;
    char ba[CF_MAXVARSIZE] = { 0 }, cc[CF_MAXVARSIZE] = { 0 };
    char lval[CF_MAXVARSIZE] = { 0 }, rval[CF_MAXVARSIZE] = { 0 };
    HubBody *hb = { 0 };

    /* BEGIN query document */

    bson query;
    bson_init(&query);
    bson_append_string(&query, cfb_bodytype, type);
    bson_append_string(&query, cfb_bodyname, name);
    bson_finish(&query);

/* BEGIN SEARCH */
    mongo_cursor *cursor = mongo_find(conn, MONGO_BODIES, &query, NULL, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&query);

    if (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        hb = NewHubBody(type, name, NULL);

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfb_bodyargs) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                memset(ba, 0, sizeof(ba));

                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strlen(ba) + strlen(bson_iterator_string(&it2)) < sizeof(ba))
                    {
                        strcat(ba, bson_iterator_string(&it2));
                        strcat(ba, ",");
                    }
                    else
                    {
                        break;
                    }
                }

                if (ba[0] != '\0')
                {
                    ba[strlen(ba) - 1] = '\0';  // remove last comma
                    hb->bodyArgs = xstrdup(ba); // freed in DeleteHubBody()
                }

            }
            else if (strcmp(bson_iterator_key(&it1), cfb_classcontext) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    snprintf(cc, sizeof(cc), "%s", bson_iterator_key(&it2));

                    bson_iterator_subiterator(&it2, &it3);

                    while (bson_iterator_next(&it3))
                    {
                        snprintf(lval, sizeof(lval), "%s", bson_iterator_key(&it3));
                        snprintf(rval, sizeof(rval), "%s", bson_iterator_string(&it3));

                        NewHubBodyAttr(hb, lval, rval, cc);
                    }
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);

    return hb;
}

/*****************************************************************************/

Item *CFDB_QueryAllBodies(EnterpriseDB *conn, char *bTypeRegex, char *bNameRegex)
/*
 * Returns all attribs of one body by its type and name.
 * MEMORY NOTE: Caller must use DeleteHubBody() on the reutrned val (!=NULL)
 */
{
    int found;
    bson_iterator it1;
    char type[CF_MAXVARSIZE] = { 0 };
    char name[CF_MAXVARSIZE] = { 0 };
    Item *record_list = NULL;

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(bTypeRegex))
    {
        bson_append_regex(&query, cfb_bodytype, bTypeRegex, "");
    }

    if (!NULL_OR_EMPTY(bNameRegex))
    {
        bson_append_regex(&query, cfb_bodyname, bNameRegex, "");
    }

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfb_bodytype, 1);
    bson_append_int(&fields, cfb_bodyname, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_BODIES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));
        found = false;

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfb_bodyname) == 0)
            {
                strncpy(name, (char *) bson_iterator_string(&it1), CF_MAXVARSIZE - 1);
            }
            else if (strcmp(bson_iterator_key(&it1), cfb_bodytype) == 0)
            {
                strncpy(type, (char *) bson_iterator_string(&it1), CF_MAXVARSIZE - 1);
            }

            found = true;
        }

        if (found)
        {
            PrependItem(&record_list, name, type);
        }
    }

    mongo_cursor_destroy(cursor);

    return record_list;
}

/*****************************************************************************/
/*                     Content-Driven Policy queries                         */
/*****************************************************************************/

Item *CFDB_QueryCdpAcls(EnterpriseDB *conn, char *sep)
/*
 * Returns all CDP ACLs from expanded policy as
 * "handle sep path sep aces sep owner sep action sep ifvarclass".
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *retList = { 0 };
    char path[CF_SMALLBUF] = { 0 };
    char aces[CF_SMALLBUF] = { 0 };
    char action[CF_SMALLBUF] = { 0 };
    char handle[CF_SMALLBUF] = { 0 };
    char owner[CF_SMALLBUF] = { 0 };
    char ifvarclass[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_bundlename, cfp_cdp_bundle_acls);
    bson_append_string(&query, cfp_promisetype, "files");
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_promiser_exp, 1);
    bson_append_int(&fields, cfp_handle_exp, 1);
    bson_append_int(&fields, cfp_constraints_exp, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(handle, sizeof(handle), "(unknown)");
        snprintf(path, sizeof(path), "(unknown)");
        snprintf(aces, sizeof(aces), "(unknown)");
        snprintf(action, sizeof(action), "(unknown)");
        snprintf(owner, sizeof(owner), "(unknown)");
        snprintf(ifvarclass, sizeof(ifvarclass), "(unknown)");

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
            {
                EscapeJson((char *) bson_iterator_string(&it1), path, sizeof(path));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strncmp(bson_iterator_string(&it2), "aces =>", 7) == 0)
                    {
                        StripListSep((char *) bson_iterator_string(&it2) + 8, aces, sizeof(aces));
                    }
                    else if (strncmp(bson_iterator_string(&it2), "owners =>", 9) == 0)
                    {
                        GetStringListElement((char *) bson_iterator_string(&it2) + 10, 0, owner, sizeof(owner));
                    }
                    else if (strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                    {
                        snprintf(action, sizeof(action), "%s", bson_iterator_string(&it2) + 17);
                    }
                    else if (strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                    {
                        snprintf(ifvarclass, sizeof(ifvarclass), "%s", bson_iterator_string(&it2) + 14);
                    }

                }
            }
        }

        snprintf(buf, sizeof(buf), "%s%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"",
                 handle, sep, path, sep, aces, sep, owner, sep, action, sep, ifvarclass);
        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/

Item *CFDB_QueryCdpCommands(EnterpriseDB *conn, char *sep)
/*
 * Returns all CDP commands from expanded policy as
 * "handle sep command sep failclass sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *retList = { 0 };
    char handle[CF_SMALLBUF] = { 0 };
    char command[CF_MAXVARSIZE] = { 0 };
    char failClass[CF_SMALLBUF] = { 0 };
    char action[CF_SMALLBUF] = { 0 };
    char ifvarclass[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_bundlename, cfp_cdp_bundle_commands);
    bson_append_string(&query, cfp_promisetype, "commands");
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_promiser_exp, 1);
    bson_append_int(&fields, cfp_handle_exp, 1);
    bson_append_int(&fields, cfp_constraints_exp, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(handle, sizeof(handle), "(unknown)");
        snprintf(command, sizeof(command), "(unknown)");
        snprintf(failClass, sizeof(failClass), "(unknown)");
        snprintf(action, sizeof(action), "(unknown)");
        snprintf(ifvarclass, sizeof(ifvarclass), "(unknown)");

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
            {
                EscapeJson((char *) bson_iterator_string(&it1), command, sizeof(command));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strncmp(bson_iterator_string(&it2), "repair_failed =>", 16) == 0)
                    {
                        GetStringListElement((char *) bson_iterator_string(&it2) + 17, 0, failClass, sizeof(failClass));
                    }
                    else if (strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                    {
                        snprintf(action, sizeof(action), "%s", bson_iterator_string(&it2) + 17);
                    }
                    else if (strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                    {
                        snprintf(ifvarclass, sizeof(ifvarclass), "%s", bson_iterator_string(&it2) + 14);
                    }

                }
            }
        }

        snprintf(buf, sizeof(buf), "%s%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"",
                 handle, sep, command, sep, failClass, sep, action, sep, ifvarclass);
        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/

Item *CFDB_QueryCdpPromiser(EnterpriseDB *conn, char *sep, char *bundleName, char *promiseType)
/*
 * Returns all CDP promisers from expanded policy as
 * "handle sep promiser sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *retList = { 0 };
    char handle[CF_SMALLBUF] = { 0 };
    char path[CF_SMALLBUF] = { 0 };
    char ifvarclass[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_bundlename, bundleName);
    bson_append_string(&query, cfp_promisetype, promiseType);
    bson_append_regex(&query, cfp_handle_exp, "^cdp_.*", "");
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_promiser_exp, 1);
    bson_append_int(&fields, cfp_handle_exp, 1);
    bson_append_int(&fields, cfp_constraints_exp, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(handle, sizeof(handle), "(unknown)");
        snprintf(path, sizeof(path), "(unknown)");
        snprintf(ifvarclass, sizeof(ifvarclass), "(unknown)");

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
            {
                snprintf(path, sizeof(path), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                    {
                        snprintf(ifvarclass, sizeof(ifvarclass), "%s", bson_iterator_string(&it2) + 14);
                    }

                }
            }
        }

        snprintf(buf, sizeof(buf), "%s%s\"%s\"%s\"%s\"", handle, sep, path, sep, ifvarclass);
        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/

int CFDB_QueryLastFileChange(EnterpriseDB *conn, char *keyHash, char *reportType, char *fileName, char *outBuf,
                             int outBufSz)
/*
 * Queries a file change / file diff report for a given host and file,
 * and writes the last time this file has been changed there to
 * outBuf (of size outBufSz).
 * No memory is allocated.
 * Returns true if a date is found, false otherwise.
 */
{
    bson_iterator it1, it2, it3;
    char currFileName[CF_MAXVARSIZE];
    time_t currTime;
    time_t lastChange = 0;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, keyHash);
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, reportType, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // should be only one document
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), reportType) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))        // loops over report rows
                {

                    bson_iterator_subiterator(&it2, &it3);

                    currFileName[0] = '\0';
                    currTime = 0;

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfr_name) == 0)
                        {
                            snprintf(currFileName, sizeof(currFileName), "%s", bson_iterator_string(&it3));
                        }
                        else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                        {
                            currTime = bson_iterator_int(&it3);
                        }
                    }

                    if (strcmp(currFileName, fileName) == 0 && currTime > lastChange)
                    {
                        lastChange = currTime;
                    }
                }
            }
        }

    }

    mongo_cursor_destroy(cursor);

    if (lastChange == 0)
    {
        snprintf(outBuf, outBufSz, "%s", "(never changed)");
    }
    else
    {
        snprintf(outBuf, outBufSz, "%s", cf_ctime(&lastChange));

        // ctime makes newline for some reason...

        if (outBuf[strlen(outBuf) - 1] == '\n')
        {
            outBuf[strlen(outBuf) - 1] = '\0';
        }

    }

    return (lastChange != 0);
}

/*****************************************************************************/

Item *CFDB_QueryCdpRegistry(EnterpriseDB *conn, char *sep)
/*
 * Returns all CDP registry from expanded policy as
 * "handle sep key sep value sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *retList = { 0 };
    char handle[CF_SMALLBUF] = { 0 };
    char key[CF_SMALLBUF] = { 0 };
    char value[CF_SMALLBUF] = { 0 };
    char action[CF_SMALLBUF] = { 0 };
    char ifvarclass[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_bundlename, cfp_cdp_bundle_registry);
    bson_append_string(&query, cfp_promisetype, "databases");
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_promiser_exp, 1);
    bson_append_int(&fields, cfp_handle_exp, 1);
    bson_append_int(&fields, cfp_constraints_exp, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(handle, sizeof(handle), "(unknown)");
        snprintf(key, sizeof(key), "(unknown)");
        snprintf(value, sizeof(value), "(unknown)");
        snprintf(action, sizeof(action), "(unknown)");
        snprintf(ifvarclass, sizeof(ifvarclass), "(unknown)");

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
            {
                EscapeJson((char *) bson_iterator_string(&it1), key, sizeof(key));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strncmp(bson_iterator_string(&it2), "database_rows =>", 16) == 0)
                    {
                        GetStringListElement((char *) bson_iterator_string(&it2) + 17, 0, value, sizeof(value));
                    }
                    else if (strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                    {
                        snprintf(action, sizeof(action), "%s", bson_iterator_string(&it2) + 17);
                    }
                    else if (strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                    {
                        snprintf(ifvarclass, sizeof(ifvarclass), "%s", bson_iterator_string(&it2) + 14);
                    }

                }
            }
        }

        snprintf(buf, sizeof(buf), "%s%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"",
                 handle, sep, key, sep, value, sep, action, sep, ifvarclass);

        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/

Item *CFDB_QueryCdpServices(EnterpriseDB *conn, char *sep)
/*
 * Returns all CDP Services from expanded policy as
 * "handle sep servicename sep servicepolicy sep action sep ifvarclass"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2;
    Item *retList = { 0 };
    char handle[CF_SMALLBUF] = { 0 };
    char serviceName[CF_SMALLBUF] = { 0 };
    char servicePolicy[CF_SMALLBUF] = { 0 };
    char action[CF_SMALLBUF] = { 0 };
    char ifvarclass[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfp_bundlename, cfp_cdp_bundle_services);
    bson_append_string(&query, cfp_promisetype, "services");
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfp_promiser_exp, 1);
    bson_append_int(&fields, cfp_handle_exp, 1);
    bson_append_int(&fields, cfp_constraints_exp, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_PROMISES_EXP, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(handle, sizeof(handle), "(unknown)");
        snprintf(serviceName, sizeof(serviceName), "(unknown)");
        snprintf(servicePolicy, sizeof(servicePolicy), "(unknown)");
        snprintf(action, sizeof(action), "(unknown)");
        snprintf(ifvarclass, sizeof(ifvarclass), "(unknown)");

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfp_promiser_exp) == 0)
            {
                snprintf(serviceName, sizeof(serviceName), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_handle_exp) == 0)
            {
                snprintf(handle, sizeof(handle), "%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfp_constraints_exp) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strncmp(bson_iterator_string(&it2), "service_policy =>", 17) == 0)
                    {
                        snprintf(servicePolicy, sizeof(servicePolicy), "%s", bson_iterator_string(&it2) + 18);
                    }
                    else if (strncmp(bson_iterator_string(&it2), "action_policy =>", 16) == 0)
                    {
                        snprintf(action, sizeof(action), "%s", bson_iterator_string(&it2) + 17);
                    }
                    else if (strncmp(bson_iterator_string(&it2), "ifvarclass =>", 13) == 0)
                    {
                        snprintf(ifvarclass, sizeof(ifvarclass), "%s", bson_iterator_string(&it2) + 14);
                    }

                }
            }
        }

        snprintf(buf, sizeof(buf), "%s%s\"%s\"%s\"%s\"%s\"%s\"%s\"%s\"",
                 handle, sep, serviceName, sep, servicePolicy, sep, action, sep, ifvarclass);
        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/

Item *CFDB_QueryCdpCompliance(EnterpriseDB *conn, char *handle)
/*
 * Returns all CDP Compliance host entries as
 * "hostkeyhash;host;status;time_t"
 * MEMORY NOTE: Caller must free returned value with DeleteItemList()
 */
{
    bson_iterator it1, it2, it3;
    mongo_cursor *cursor;
    Item *retList = { 0 };
    time_t t;
    char host[CF_SMALLBUF] = { 0 };
    char hostKeyHash[CF_SMALLBUF] = { 0 };
    char status = { 0 };
    char time[CF_SMALLBUF] = { 0 };
    char buf[CF_MAXVARSIZE] = { 0 };

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_promisecompl_keys, handle);
    bson_finish(&query);

    // returned attribute
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_promisecompl, 1);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_finish(&fields);

    cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // iterate over docs
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        // make sure everything gets defined
        snprintf(host, sizeof(host), "(unknown)");
        snprintf(hostKeyHash, sizeof(hostKeyHash), "(unknown)");
        snprintf(time, sizeof(time), "(unknown)");
        status = '?';
        t = 0;

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_promisecompl) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (strcmp(bson_iterator_key(&it2), handle) == 0)
                    {
                        bson_iterator_subiterator(&it2, &it3);

                        while (bson_iterator_next(&it3))
                        {
                            if (strcmp(bson_iterator_key(&it3), cfr_promisestatus) == 0)
                            {
                                status = *bson_iterator_string(&it3);
                            }
                            else if (strcmp(bson_iterator_key(&it3), cfr_time) == 0)
                            {
                                t = bson_iterator_int(&it3);
                                snprintf(time, sizeof(time), "%lu", t);
                            }
                        }
                    }
                }
            }
            else if (strcmp(bson_iterator_key(&it1), cfr_host_array) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                if (bson_iterator_next(&it2))   // just pick first hostname
                {
                    snprintf(host, sizeof(host), "%s", bson_iterator_string(&it2));
                }
            }
            else if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
            {
                snprintf(hostKeyHash, sizeof(hostKeyHash), "%s", bson_iterator_string(&it1));
            }
        }

        snprintf(buf, sizeof(buf), "%s;%s;%c;%s", hostKeyHash, host, status, time);

        PrependItem(&retList, buf, NULL);
    }

    mongo_cursor_destroy(cursor);
    return retList;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static bool AppendHostKeys(EnterpriseDB *conn, bson *b, HostClassFilter *hostClassFilter)
/**
 * Appends to b the keyhash of hosts matching the class filter.
 * Useful for "joins".
 * Returns true if b is modified, false otherwise.
 **/
{
    bson_iterator it1;
    mongo_cursor *cursor;

    bson query;
    bson_init(&query);
    if (!BsonAppendHostClassFilter(&query, hostClassFilter))
    {
        bson_destroy(&query);
        return false;
    }
    bson_finish(&query);

    bson field;
    bson_init(&field);
    bson_append_int(&field, cfr_keyhash, 1);
    bson_finish(&field);

    cursor = mongo_find(conn, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&field);

    int i = 0;
    char iStr[64] = { 0 };
    bool bsonArrayStarted = false;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {
            if (strcmp(bson_iterator_key(&it1), cfr_keyhash) == 0)
            {
                if (!bsonArrayStarted)
                {
                    bson_append_start_array(b, "$or");
                    bsonArrayStarted = true;
                }

                snprintf(iStr, sizeof(iStr), "%d", i);

                {
                    bson_append_start_object(b, iStr);
                    bson_append_string(b, cfr_keyhash, bson_iterator_string(&it1));
                    bson_append_finish_object(b);
                }
                i++;
            }
        }
    }

    if (bsonArrayStarted)
    {
        bson_append_finish_object(b);
    }
    else
    {
        bson_append_string(b, cfr_keyhash, "");        // no match, indicate to caller
    }

    mongo_cursor_destroy(cursor);

    return true;
}



HubQuery *CFDB_QueryCachedTotalCompliance(EnterpriseDB *conn, char *policy, time_t minGenTime)
{
    bson_iterator it1, it2, it3;
    mongo_cursor *cursor;
    Rlist *record_list = NULL;
    char policyDB[CF_MAXVARSIZE];
    bson query, field;
    double kept, repaired, notkept;
    int slot, count;
    time_t genTime;

    bson_init(&query);
    bson_append_string(&query, cfc_cachetype, cfc_cachecompliance);
    bson_finish(&query);

    cursor = mongo_find(conn, MONGO_CACHE, &query, bson_empty(&field), 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);

    if (mongo_cursor_next(cursor) == MONGO_OK)      // loops over cache types (want just one)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (bson_iterator_next(&it1))
        {

            if (bson_iterator_type(&it1) != BSON_OBJECT)
            {
                continue;
            }

            snprintf(policyDB, sizeof(policyDB), "%s", bson_iterator_key(&it1));

            // if policy specified, retrieve only that one
            if (policy && strcmp(policyDB, policy) != 0)
            {
                continue;
            }

            bson_iterator_subiterator(&it1, &it2);

            while (bson_iterator_next(&it2))
            {
                slot = -1;
                sscanf(bson_iterator_key(&it2), "%d", &slot);

                bson_iterator_subiterator(&it2, &it3);

                kept = 0;
                repaired = 0;
                notkept = 0;
                count = 0;
                genTime = 0;

                while (bson_iterator_next(&it3))
                {
                    if (strcmp(bson_iterator_key(&it3), cfr_kept) == 0)
                    {
                        kept = bson_iterator_double(&it3);
                    }
                    else if (strcmp(bson_iterator_key(&it3), cfr_repaired) == 0)
                    {
                        repaired = bson_iterator_double(&it3);
                    }
                    else if (strcmp(bson_iterator_key(&it3), cfr_notkept) == 0)
                    {
                        notkept = bson_iterator_double(&it3);
                    }
                    else if (strcmp(bson_iterator_key(&it3), cfc_count) == 0)
                    {
                        count = bson_iterator_int(&it3);
                    }
                    else if (strcmp(bson_iterator_key(&it3), cfc_timegen) == 0)
                    {
                        genTime = bson_iterator_int(&it3);
                    }
                }
                if (genTime >= minGenTime)
                {
                    PrependRlistAlienUnlocked(&record_list,
                                      NewHubCacheTotalCompliance(policyDB, slot, count, -1, kept, repaired, notkept,
                                                                 genTime));
                }
            }

        }

    }

    return NewHubQuery(NULL, record_list);
}

/*****************************************************************************/

Rlist *CFDB_QueryNotes(EnterpriseDB *conn, char *keyhash, char *nid, Item *data)
{
    mongo_cursor *cursor;
    bson_iterator it1, it2, it3;
    HubNoteInfo *hci = NULL;
    HubNote *hc = NULL, *tail = NULL;
    HubHost *hh;
    Rlist *ret = NULL, *host_list = NULL;

    char kh[CF_MAXVARSIZE] = { 0 }, noteId[CF_MAXVARSIZE] = { 0 }, username[CF_MAXVARSIZE] = { 0 }, note[CF_BUFSIZE] = { 0 }, rptData[CF_BUFSIZE] = { 0 };
    char fusername[CF_SMALLBUF] = { 0 };
    time_t datetime = -1, from = -1, to = -1;
    bson_oid_t bsonid;
    bson_type t;
    int firstComment = false, specificQuery = false;
    int reportType = -1;

    if (BEGINSWITH(data->name, ","))
    {
        sscanf(data->name + 1, "%ld,%ld\n", &from, &to);
    }
    else
    {
        sscanf(data->name, "%255[^','],%ld,%ld\n", fusername, &from, &to);
    }

    bson query;
    bson_init(&query);

    if (!NULL_OR_EMPTY(nid))
    {
        bson_oid_from_string(&bsonid, nid);
        bson_append_oid(&query, "_id", &bsonid);
    }
    else
    {
        if (!NULL_OR_EMPTY(keyhash))
        {
            bson_append_string(&query, cfn_keyhash, keyhash);
        }

        if (!NULL_OR_EMPTY(fusername))
        {
            bson_append_string(&query, "n.u", fusername);
            specificQuery = true;
        }

        if (from >= 0 || to >= 0)
        {
            specificQuery = true;
        }
    }

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, "_id", 1);
    bson_append_int(&fields, cfn_keyhash, 1);
    bson_append_int(&fields, cfn_reportdata, 1);
    bson_append_int(&fields, cfn_note, 1);
    bson_append_int(&fields, cfn_reporttype, 1);
    bson_finish(&fields);

    cursor = mongo_find(conn, MONGO_NOTEBOOK, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);
    bson_destroy(&query);

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));
        kh[0] = '\0';
        username[0] = '\0';
        note[0] = '\0';
        noteId[0] = '\0';
        while (bson_iterator_next(&it1))
        {
            switch (bson_iterator_type(&it1))
            {
            case BSON_OID:
                if (strcmp(bson_iterator_key(&it1), "_id") == 0)
                {
                    bson_oid_to_string(bson_iterator_oid(&it1), noteId);
                }
                break;
            case BSON_STRING:
                if (strcmp(bson_iterator_key(&it1), cfn_keyhash) == 0)
                {
                    strncpy(kh, bson_iterator_string(&it1), CF_MAXVARSIZE - 1);
                }
                else if (strcmp(bson_iterator_key(&it1), cfn_reportdata) == 0)
                {
                    strncpy(rptData, bson_iterator_string(&it1), CF_BUFSIZE - 1);
                }
                break;

            case BSON_INT:
                if (strcmp(bson_iterator_key(&it1), cfn_reporttype) == 0)
                {
                    reportType = bson_iterator_int(&it1);
                }
                break;

            case BSON_OBJECT:
            case BSON_ARRAY:
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    while (bson_iterator_next(&it3))
                    {
                        t = bson_iterator_type(&it3);
                        if (t == 0)
                        {
                            break;
                        }
                        switch (t)
                        {
                        case BSON_STRING:
                            if (strcmp(bson_iterator_key(&it3), cfn_username) == 0)
                            {
                                strncpy(username, bson_iterator_string(&it3), CF_MAXVARSIZE - 1);
                            }
                            else if (strcmp(bson_iterator_key(&it3), cfn_message) == 0)
                            {
                                strncpy(note, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                            }
                            break;
                        case BSON_INT:
                            if (strcmp(bson_iterator_key(&it3), cfn_datetime) == 0)
                            {
                                datetime = (time_t) bson_iterator_int(&it3);
                                //Define brackets for the start and end time eg. (from,to] => including to
                            }
                            break;
                        default:
                            CfOut(cf_inform, "", " !! Unknown key \"%s\" in last seen", bson_iterator_key(&it3));
                            break;
                        }
                    }
                    /* apply filter: username then datetime */
                    if (specificQuery)
                    {
                        if (strcmp(username, fusername) != 0 && !NULL_OR_EMPTY(fusername))
                        {
                            continue;
                        }

                        if (!(datetime <= to) && (to >= 0))
                        {
                            continue;
                        }

                        if (!(datetime >= from) && (from >= 0))
                        {
                            continue;
                        }
                    }

                    if (hci == NULL)
                    {
                        hh = NewHubHost(NULL, kh, NULL, NULL);
                        PrependRlistAlienUnlocked(&host_list, hh);
                        QueryInsertHostInfo(conn, host_list);
                        hci = NewHubNoteInfo(hh, noteId, username, note, datetime, rptData, reportType);
                        firstComment = true;
                    }
                    else
                    {
                        hc = NewHubNote(username, note, datetime);
                        if (firstComment)
                        {
                            hci->note->next = hc;
                            firstComment = false;
                        }
                        else
                        {
                            tail->next = hc;
                        }
                        tail = hc;
                    }
                }
                if (hci)
                {
                    AppendRlistAlien(&ret, hci);
                }
                hci = NULL;
                tail = NULL;
                hc = NULL;
                break;
            default:
                break;
            }
        }
    }
    mongo_cursor_destroy(cursor);
    return ret;
}

/*****************************************************************************/

Rlist *CFDB_QueryNoteId(EnterpriseDB *conn, bson *query)
{
    bson_iterator it1;
    Rlist *host_list = NULL;
    char noteId[CF_MAXVARSIZE] = { 0 };
    char keyhash[CF_MAXVARSIZE] = { 0 };

/* BEGIN RESULT DOCUMENT */

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfn_keyhash, 1);
    bson_append_int(&fields, "_id", 1);
    bson_finish(&fields);

/* BEGIN SEARCH */

    mongo_cursor *cursor = mongo_find(conn, MONGO_NOTEBOOK, query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);

    if (!cursor)
    {
        return NULL;
    }
    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        noteId[0] = '\0';

        while (bson_iterator_next(&it1))
        {
            switch (bson_iterator_type(&it1))
            {
            case BSON_OID:
                bson_oid_to_string(bson_iterator_oid(&it1), noteId);
                break;
            case BSON_STRING:
                if (strcmp(bson_iterator_key(&it1), cfn_keyhash) == 0)
                {
                    strncpy(keyhash, bson_iterator_string(&it1), CF_MAXVARSIZE - 1);
                }
                break;
            default:
                break;
            }

        }
        if (strlen(noteId) > 15)
        {
            PrependRlistAlienUnlocked(&host_list, noteId);
        }
    }

    mongo_cursor_destroy(cursor);
    return host_list;
}

/*****************************************************************************/

Item *CFDB_QueryDistinctStr(EnterpriseDB *conn, char *database, char *collection, char *dKey, char *qKey,
                            char *qVal)
/**
 * Queries for distinct occurences of dKey in a query matching qKey = qVal
 */
{
    Item *retVal;

    bson query;
    bson_init(&query);
    bson_append_string(&query, qKey, qVal);
    bson_finish(&query);

    retVal = CFDB_QueryDistinct(conn, database, collection, dKey, &query);

    bson_destroy(&query);

    return retVal;
}

/******************************************************************/

Item *CFDB_QueryDistinct(EnterpriseDB *conn, char *database, char *collection, char *dKey, bson *queryBson)
{
    bson_iterator it1, values;
    Item *ret = NULL;

    bson cmd;
    bson_init(&cmd);
    bson_append_string(&cmd, "distinct", collection);
    bson_append_string(&cmd, "key", dKey);

    if (queryBson)
    {
        bson_append_bson(&cmd, "query", queryBson);
    }
    bson_finish(&cmd);

    bson result;

    if (mongo_run_command(conn, database, &cmd, &result) != MONGO_OK)
    {
        MongoCheckForError(conn, "CFDB_QueryDistinct()", "", NULL);
        bson_destroy(&cmd);
        return false;
    }

    bson_destroy(&cmd);

    if (!bson_find(&it1, &result, "values"))
    {
        CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryDistinct()");
        bson_destroy(&result);
        return false;
    }

    if (bson_iterator_type(&it1) != BSON_ARRAY)
    {
        CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryDistinct()");
        bson_destroy(&result);
        return false;
    }

    bson_iterator_subiterator(&it1, &values);

    while (bson_iterator_next(&values))
    {
        PrependItem(&ret, (char *) bson_iterator_string(&values), NULL);
    }

    bson_destroy(&result);

    return ret;
}

/******************************************************************/


/******************************************************************/



/*****************************************************************************/

HubQuery *CFDB_QueryClassesDistinctSorted(EnterpriseDB *conn, const char *class_rx,
                                          HostClassFilter *hostClassFilter, PageInfo *page)
{
    bson query;
    bson_init(&query);
    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    Item *classList = CFDB_QueryDistinct(conn, MONGO_BASE, MONGO_HOSTS_COLLECTION, cfr_class_keys, &query);

    bson_destroy(&query);

    Rlist *record_list = NULL;

    for (Item *ip = classList; ip != NULL; ip = ip->next)
    {
        if (!class_rx || StringMatch(class_rx, ip->name))
        {
            PrependRlistAlienUnlocked(&record_list, NewHubClass(NULL, ip->name, 0, 0, 0));
        }
    }

    DeleteItemList(classList);

    record_list = SortRlist(record_list, SortClasses);
    PageRecords(&record_list, page, DeleteHubClass);

    return NewHubQuery(NULL, record_list);
}

/*****************************************************************************/

int CFDB_QueryIsMaster(void)
{
    bson_iterator it1;
    int ret = false;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson cmd;
    bson_init(&cmd);
    bson_append_string(&cmd, "isMaster", MONGO_HOSTS_COLLECTION);
    bson_finish(&cmd);

    bson result;

    if (mongo_run_command(&conn, MONGO_BASE, &cmd, &result) == MONGO_OK)
    {
        if (bson_find(&it1, &result, "ismaster"))
        {
            if (bson_iterator_bool(&it1))
            {
                ret = true;
            }
        }
        else
        {
            CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryIsMaster()");
        }
    }
    else
    {
        MongoCheckForError(&conn, "CFDB_QueryIsMaster()", "", NULL);
    }
    bson_destroy(&cmd);
    bson_destroy(&result);
    CFDB_Close(&conn);

    if (!ret)
    {
        CfOut(cf_verbose, "", "We are part of report replica set, but not master \n");
    }

    return ret;
}

/*************************************************/

int CFDB_QueryMasterIP(char *buffer, int bufsize)
{
    bson_iterator it1;
    int ret = false;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    bson cmd;
    bson_init(&cmd);
    bson_append_string(&cmd, "isMaster", MONGO_HOSTS_COLLECTION);
    bson_finish(&cmd);

    bson result;

    if (mongo_run_command(&conn, MONGO_BASE, &cmd, &result) == MONGO_OK)
    {
        if (bson_find(&it1, &result, "primary"))
        {
            snprintf(buffer, bufsize, "%s", bson_iterator_string(&it1));
            ret = true;
        }
        else
        {
            CfOut(cf_verbose, "", " Malformed query result in CFDB_QueryIsMaster()");
        }
    }
    else
    {
        MongoCheckForError(&conn, "CFDB_QueryIsMaster()", "", NULL);
    }
    bson_destroy(&cmd);
    bson_destroy(&result);
    CFDB_Close(&conn);
    return ret;
}

/*************************************************/

int CFDB_QueryReplStatus(EnterpriseDB *conn, char *buffer, int bufsize)
{
    bson_iterator it1, it2, it3;
    int ret = false;
    char work[CF_MAXVARSIZE] = { 0 };
    int totalStatus = 1;

    StartJoin(buffer, "{", bufsize);

    bson cmd;
    bson_init(&cmd);
    bson_append_string(&cmd, "replSetGetStatus", MONGO_HOSTS_COLLECTION);
    bson_finish(&cmd);

    bson result;

    if (mongo_run_command(conn, "admin", &cmd, &result) == MONGO_OK)
    {

        bson_iterator_init(&it1, &result);

        if (bson_find(&it1, &result, "ok") && bson_iterator_int(&it1) == 1)
        {
            if (bson_find(&it1, &result, "set"))
            {
                snprintf(work, sizeof(work), "\"set\":\"%s\",", bson_iterator_string(&it1));
                Join(buffer, work, bufsize);
            }

            if (bson_find(&it1, &result, "members"))
            {
                bson_iterator_subiterator(&it1, &it2);

                Join(buffer, "\"members\":[", bufsize);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);
                    Join(buffer, "{", bufsize);

                    while (bson_iterator_next(&it3))
                    {
                        char *dbkey = (char *) bson_iterator_key(&it3);

                        work[0] = '\0';

                        if (strcmp(dbkey, "name") == 0)
                        {
                            snprintf(work, sizeof(work), "\"name\":\"%s\",", bson_iterator_string(&it3));
                        }
                        else if (strcmp(dbkey, "health") == 0)
                        {
                            int health = bson_iterator_int(&it3);

                            snprintf(work, sizeof(work), "\"health\":%d,", health);

                            if (health == 0)
                            {
                                totalStatus = 0;
                            }
                        }
                        else if (strcmp(dbkey, "state") == 0)
                        {
                            int state = bson_iterator_int(&it3);
                            char *stateStr;

                            if (state == 1)
                            {
                                stateStr = "Primary";
                            }
                            else if (state == 2)
                            {
                                stateStr = "Secondary";
                            }
                            else
                            {
                                totalStatus = 0;
                                stateStr = "Unknown";
                            }

                            snprintf(work, sizeof(work), "\"stateStr\":\"%s\",", stateStr);
                        }
                        else if (strcmp(dbkey, "uptime") == 0)
                        {
                            snprintf(work, sizeof(work), "\"uptime\":%d,", bson_iterator_int(&it3));
                        }
                        else if (strcmp(dbkey, "lastHeartbeat") == 0)
                        {
                            snprintf(work, sizeof(work), "\"lastHeartbeat\":%ld,", bson_iterator_time_t(&it3));
                        }
                        else if (strcmp(dbkey, "optimeDate") == 0)
                        {
                            snprintf(work, sizeof(work), "\"optime\":%ld,", bson_iterator_time_t(&it3));
                        }

                        if (!NULL_OR_EMPTY(work))
                        {
                            Join(buffer, work, bufsize);
                        }
                    }

                    ReplaceTrailingChar(buffer, ',', '\0');
                    Join(buffer, "},", bufsize);
                }

                ReplaceTrailingChar(buffer, ',', '\0');
                Join(buffer, "],", bufsize);

            }

            snprintf(work, sizeof(work), "\"status\":%d", totalStatus);
            Join(buffer, work, bufsize);
        }
    }
    else
    {
        MongoCheckForError(conn, "CFDB_QueryReplStatus()", "", NULL);
    }

    EndJoin(buffer, "}", bufsize);

    bson_destroy(&cmd);
    bson_destroy(&result);
    return ret;
}

/*****************************************************************************/

static bool BsonAppendPromiseFilter(bson *query, PromiseFilter *filter)
{
    if (filter == NULL)
    {
        return false;
    }

    bool modified = false;

    modified |= BsonAppendStringSafe(query, cfp_handle, filter->handleInclude);
    modified |= BsonAppendRegexSafe(query, cfp_handle, filter->handleRxInclude);

    modified |= BsonAppendStringSafe(query, cfp_promiser, filter->promiserInclude);
    modified |= BsonAppendRegexSafe(query, cfp_promiser, filter->promiserRxInclude);

    modified |= BsonAppendStringSafe(query, cfp_promisetype, filter->promiseTypeInclude);

    modified |= BsonAppendStringSafe(query, cfp_bundletype, filter->bundleTypeInclude);
    modified |= BsonAppendRegexSafe(query, cfp_bundletype, filter->bundleTypeRxInclude);

    modified |= BsonAppendIncludeList(query, cfp_bundlename, filter->bundleIncludes);
    modified |= BsonAppendIncludeRxList(query, cfp_bundlename, filter->bundleRxIncludes);

    modified |= BsonAppendExcludeList(query, cfp_bundlename, filter->bundleExcludes);
    modified |= BsonAppendExcludeRxList(query, cfp_bundlename, filter->bundleRxExcludes);

    return modified;
}

/*****************************************************************************/

Rlist *CFDB_QueryHostKeys(EnterpriseDB *conn, const char *hostname, const char *ip, time_t from, time_t to,
                          HostClassFilter *hostClassFilter)
{
    // query
    bson query;
    bson_init(&query);
    if (hostname)
    {
        bson_append_regex(&query, cfr_host_array, hostname, "");
    }
    if (ip)
    {
        bson_append_regex(&query, cfr_ip_array, ip, "");
    }
    BsonAppendHostClassFilter(&query, hostClassFilter);
    bson_finish(&query);

    // projection
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_day, 1);
    bson_finish(&fields);

    mongo_cursor *cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    Rlist *hostkeys = NULL;

    while (mongo_cursor_next(cursor) == MONGO_OK)
    {
        const char *hostkey = NULL;
        BsonStringGet(&cursor->current, cfr_keyhash, &hostkey);
        assert(hostkey);

        time_t timestamp = 0;
        BsonTimeGet(&cursor->current, cfr_day, &timestamp);

        if (timestamp < from || timestamp > to)
        {
            continue;
        }

        AppendRlist(&hostkeys, hostkey, CF_SCALAR);
    }

    mongo_cursor_destroy(cursor);

    return hostkeys;
}

/*****************************************************************************/

HubHost *CFDB_GetHostByKey(EnterpriseDB *conn, const char *hostkey)
{
// query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    bson_finish(&query);

// projection
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_finish(&fields);

    bson out;
    HubHost *host = NULL;

    if (mongo_find_one(conn, MONGO_DATABASE, &query, &fields, &out) == MONGO_OK)
    {
        Item *host_names = BsonGetStringArrayAsItemList(&out, cfr_host_array);
        Item *ip_addresses = BsonGetStringArrayAsItemList(&out, cfr_ip_array);

        const char *hostkey = NULL;
        BsonStringGet(&out, cfr_keyhash, &hostkey);
        assert(hostkey);

        host = NewHubHost(NULL, hostkey, ip_addresses->name, host_names->name);

        DeleteItemList(host_names);
        DeleteItemList(ip_addresses);
        bson_destroy(&out);
    }

    bson_destroy(&query);
    bson_destroy(&fields);

    return host;
}

/*****************************************************************************/

/* for blue host returned score is the score from last reachable recivied report */
/* returns unsorted data */
Item *CFDB_GetHostByColour(EnterpriseDB *conn, HostClassFilter *host_class_filter,
                           HostColourFilter *host_colour_filter)
{
    const HostRankMethod method = host_colour_filter ? HOST_RANK_METHOD_COMPLIANCE : host_colour_filter->method;

    bson query;
    bson_init(&query);
    {
        bson_append_start_object( &query, "$query");
        BsonAppendHostClassFilter(&query, host_class_filter);
        BsonAppendHostColourFilter(&query, host_colour_filter);
        bson_append_finish_object(&query);
    }

    bson_finish(&query);

    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_keyhash, 1);
    bson_append_int(&fields, cfr_ip_array, 1);
    bson_append_int(&fields, cfr_host_array, 1);
    bson_append_int(&fields, HostRankMethodToMongoCode(method), 1);
    bson_finish(&fields);

    mongo_cursor *cursor = NULL;
    cursor = mongo_find(conn, MONGO_DATABASE, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&fields);
    bson_destroy(&query);

    /* process db output */
    char keyhash[CF_MAXVARSIZE];
    char hostnames[CF_BUFSIZE];
    char addresses[CF_BUFSIZE];
    int score;

    keyhash[0] = '\0';
    hostnames[0] = '\0';
    addresses[0] = '\0';

    Item *list = NULL;
    bson_iterator it1;

    while (mongo_cursor_next(cursor) == MONGO_OK) // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';
        score = 0;

        while (bson_iterator_next(&it1))
        {
            /* Extract the common HubHost data */
            CFDB_ScanHubHost(&it1, keyhash, addresses, hostnames);

            if (strcmp(bson_iterator_key(&it1), HostRankMethodToMongoCode(method)) == 0)
            {
                score = (int) bson_iterator_int(&it1);
            }
        }

        PrependItem(&list, keyhash, hostnames);
        SetItemListCounter(list, keyhash, score);
    }

    mongo_cursor_destroy(cursor);

    return list;
}

/*****************************************************************************/

long CFDB_GetLastAgentExecution(EnterpriseDB *conn, const char *hostkey)
{
    long agent_last_exec = 0;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    bson_finish(&query);

    // projection
    bson field;
    bson_init(&field);
    bson_append_int(&field, cfr_last_execution, 1);
    bson_finish(&field);

    bson out;

    if (mongo_find_one(conn, MONGO_DATABASE, &query, &field, &out) == MONGO_OK)
    {
        agent_last_exec = BsonLongGet(&out, cfr_last_execution);
        bson_destroy(&out);
    }

    bson_destroy(&query);
    bson_destroy(&field);

    return agent_last_exec;
}

/*****************************************************************************/

long CFDB_GetDeltaAgentExecution(EnterpriseDB *conn, const char *hostkey)
{
    long delta = 0;

    // query
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    bson_finish(&query);

    // projection
    bson field;
    bson_init(&field);
    bson_append_int(&field, cfr_schedule, 1);
    bson_finish(&field);

    bson out;
    if (mongo_find_one(conn, MONGO_DATABASE, &query, &field, &out) == MONGO_OK)
    {
        delta = BsonLongGet(&out, cfr_schedule);
        bson_destroy(&out);
    }

    bson_destroy(&query);
    bson_destroy(&field);

    return delta;
}

bool CFDB_GetHostColour(char *lkeyhash, const HostRankMethod method, HostColour *result)
{
    if (lkeyhash == NULL)
    {
        return false;
    }

    unsigned long bluehost_threshold;
    if (!CFDB_GetBlueHostThreshold(&bluehost_threshold))
    {
        return false;
    }
    time_t now = time(NULL);

    EnterpriseDB conn;
    if (!CFDB_Open(&conn))
    {
        return false;
    }

    /* query */
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, lkeyhash);
    bson_finish(&query);

   /* result document */
    bson fields;
    bson_init(&fields);
    bson_append_int(&fields, cfr_day, 1);
    bson_append_int(&fields, HostRankMethodToMongoCode(method), 1);
    bson_append_int(&fields, cfr_is_black, 1);
    bson_finish(&fields);

    bson out;

    /* if no records are found it's seen are host with unknown state (blue) */
    if (mongo_find_one(&conn, MONGO_DATABASE, &query, &fields, &out) != MONGO_OK)
    {
        *result = HOST_COLOUR_BLUE;
    }
    else
    {
        time_t then = 0;
        BsonTimeGet(&out, cfr_day, &then);

        bool is_black;
        BsonBoolGet(&out, cfr_is_black, &is_black);

        int score = 0;
        if (BsonIntGet(&out, HostRankMethodToMongoCode(method), &score))
        {
            *result = HostColourFromScore(now, then, bluehost_threshold, score, is_black);
        }
        else
        {
            // special case: could not find the score, return blue
            *result = HOST_COLOUR_BLUE;
        }

        bson_destroy(&out);
    }

    bson_destroy(&query);
    bson_destroy(&fields);

    CFDB_Close(&conn);

    return 0;
}

