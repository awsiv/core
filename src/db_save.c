#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "files_names.h"
#include "db_save.h"
#include "granules.h"

#include <assert.h>

/*****************************************************************************/
/* Cache / scratch space                                                     */
/*****************************************************************************/

// WTF: manages its own connection
int CFDB_PutValue(char *lval, char *rval, char *db_name)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson setOp, empty;
    mongo_connection dbconn;

    if (!IsDefinedClass("am_policy_hub") && !AM_PHP_MODULE)
    {
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");
    bson_append_string(setObj, lval, rval);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(&dbconn, db_name, bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int CFDB_SaveLastseenCache(Item *lastseen)
{
    bson_buffer bb;
    bson_buffer *setObj, *sub, *arr;
    bson setOp, empty;
    mongo_connection dbconn;
    Item *ip;
    char arrIndex[CF_BUFSIZE] = { 0 };
    int i = 0;

    if (!IsDefinedClass("am_policy_hub"))
    {
        CfOut(cf_verbose, "", "Ignoring caching of lastseen hosts into database - we are not a policy server");
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");
    arr = bson_append_start_array(setObj, cfr_lastseen_hosts);

    for (ip = lastseen, i = 0; ip != NULL; ip = ip->next, i++)
    {
        snprintf(arrIndex, sizeof(arrIndex), "%d", i);
        sub = bson_append_start_object(arr, arrIndex);

        bson_append_string(sub, cfr_keyhash, ip->name);
        bson_append_string(sub, cfr_ipaddr, ip->classes);
        bson_append_int(sub, cfr_time, ip->time);

        bson_append_finish_object(sub);
    }

    bson_append_finish_object(arr);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);

    mongo_update(&dbconn, MONGO_SCRATCH, bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

void CFDB_SaveGoalsCache(char *goal_patterns)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson setOp, empty;
    mongo_connection dbconn;

    if (!IsDefinedClass("am_policy_hub"))
    {
        CfOut(cf_verbose, "", "Ignoring caching of lastseen hosts into database - we are not a policy server");
        return;
    }

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");
    bson_append_string(setObj, "goal_patterns", goal_patterns);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(&dbconn, MONGO_SCRATCH, bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);
    bson_destroy(&setOp);
    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_SaveHostID(mongo_connection *conn, char *database, char *keyField, char *keyhash, char *ipaddr,
                     char *hostname)
/**
 *  hostname is optional, reverse lookup if not specified
 **/
{
    bson_buffer bb;
    bson_buffer *setObj, *arr;
    bson host_key;              // host description
    bson setOp;
    char foundHostName[CF_MAXVARSIZE];

    if (!NULL_OR_EMPTY(hostname))
    {
        snprintf(foundHostName, sizeof(foundHostName), "%s", hostname);
    }
    else
    {
        snprintf(foundHostName, sizeof(foundHostName), "%s", IPString2Hostname(ipaddr));
    }

// locate right host key

    bson_buffer_init(&bb);
    bson_append_string(&bb, keyField, keyhash);
    bson_from_buffer(&host_key, &bb);

// ip address - replace array with one el (more later - aging..)

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");
    arr = bson_append_start_array(setObj, cfr_ip_array);
    bson_append_string(setObj, "0", ipaddr);
    bson_append_finish_object(arr);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, database, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);

// host name

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");
    arr = bson_append_start_array(setObj, cfr_host_array);
    bson_append_string(setObj, "0", foundHostName);
    bson_append_finish_object(arr);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, database, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSoftware(mongo_connection *conn, enum software_rep sw, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson_buffer *sub;
    bson host_key;              // host description
    bson setOp;
    int i;
    bson_buffer *arr;
    Item *ip;
    char *dbOp = { 0 };
    char packNumStr[CF_MAXVARSIZE];
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch, archStr[CF_MAXVARSIZE];

// find right host

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    switch (sw)
    {
    case sw_rep_installed:
        arr = bson_append_start_array(setObj, cfr_software);
        dbOp = "update installed software";
        break;
    case sw_rep_patch_installed:
        arr = bson_append_start_array(setObj, cfr_patch_installed);
        dbOp = "update patch installed software";
        break;
    case sw_rep_patch_avail:
        arr = bson_append_start_array(setObj, cfr_patch_avail);
        dbOp = "update patch available software";
        break;

    default:
        arr = bson_append_start_array(setObj, cfr_software);
        dbOp = "update installed software";
        break;
    }

/* Read and insert data */

    for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
    {
        sscanf(ip->name, "%250[^,],%250[^,],%c", name, version, &arch);
        snprintf(packNumStr, sizeof(packNumStr), "%d", i);
        snprintf(archStr, sizeof(archStr), "%c", arch);
        sub = bson_append_start_object(arr, packNumStr);

        bson_append_string(sub, cfr_name, name);
        bson_append_string(sub, cfr_version, version);
        bson_append_string(sub, cfr_arch, archStr);

        bson_append_finish_object(sub);
    }

    bson_append_finish_object(arr);
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, dbOp, keyhash, NULL);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/
/* Monitor data                                                              */
/*****************************************************************************/

void CFDB_SaveMonitorData2(mongo_connection *conn, char *keyHash, enum monitord_rep rep_type, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj, *arr;
    bson keys;
    bson setOp;
    bson insertOp;
    char monId[CF_SMALLBUF] = { 0 }, monDesc[CF_MAXVARSIZE] = { 0 }, monUnits[CF_SMALLBUF] = { 0 };
    char varName[CF_SMALLBUF], iStr[CF_SMALLBUF];
    bool didUpdate, haveAllMeta = false;
    char *db;
    char *dbOp;
    Item *ip, *slotStart;
    int monGlobal;
    double monExpMin, monExpMax;
    int i, slot, numSlots, iterations;
    double q, e, d;

    switch (rep_type)
    {
    case mon_rep_mag:
        db = MONGO_DATABASE_MON_MG;
        dbOp = "Update monitord mag";
        numSlots = CF_MAG_SLOTS;        // every five mins
        break;
    case mon_rep_week:
        db = MONGO_DATABASE_MON_WK;
        dbOp = "Update monitord week";
        numSlots = CF_TIMESERIESDATA;   // every hour
        break;
    case mon_rep_yr:
        db = MONGO_DATABASE_MON_YR;
        dbOp = "Update monitord year";
        numSlots = CF_YEAR_SLOTS;       // every week
        break;
    default:
        CfOut(cf_error, "", "!! Undefined monitord type in save (%d)", rep_type);
        FatalError("Software error");
    }

    iterations = 0;
    ip = data;
    while (ip != NULL)
    {
        if (strncmp(ip->name, "M:", 2) == 0)
        {
            monId[0] = '\0';
            monGlobal = 0;
            monExpMin = 0.0f;
            monExpMax = 0.0f;
            monUnits[0] = '\0';
            monDesc[0] = '\0';
            haveAllMeta = false;

            if (sscanf(ip->name + 2, "%64[^,],%d,%lf,%lf,%32[^,],%128[^\n]",
                       monId, &monGlobal, &monExpMin, &monExpMax, monUnits, monDesc) == 6)
            {
                haveAllMeta = true;
            }

            CfDebug("Found new monitoring probe \"%s\" \"%s\" \"%s\" %d %lf %lf\n", monId, monDesc, monUnits, monGlobal,
                    monExpMin, monExpMax);
            ip = ip->next;
            continue;
        }

        if (monId[0] == '\0')
        {
            CfOut(cf_verbose, "", "!! Missing monitoring probe id - skipping");
            ip = ip->next;
            continue;
        }

        // key - identify the record object
        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, keyHash);
        bson_append_string(&bb, cfm_id, monId);
        bson_from_buffer(&keys, &bb);

        // create object to insert
        bson_buffer_init(&bb);
        setObj = bson_append_start_object(&bb, "$set");

        if (haveAllMeta)        // full query only
        {
            bson_append_string(setObj, cfm_description, monDesc);
            bson_append_string(setObj, cfm_units, monUnits);
            bson_append_bool(setObj, cfm_global, monGlobal);
            bson_append_double(setObj, cfm_expmin, monExpMin);
            bson_append_double(setObj, cfm_expmax, monExpMax);
        }

        slotStart = ip;

        while (ip && (strncmp(ip->name, "M:", 2) != 0))
        {
            sscanf(ip->name, "%d %lf %lf %lf", &slot, &q, &e, &d);

            if (slot < 0 || slot >= numSlots)
            {
                CfOut(cf_error, "", "Slot %d out of range (max=%d) on %s - skipping", slot, numSlots, dbOp);
                ip = ip->next;
                continue;
            }

            snprintf(varName, sizeof(varName), "%s.%d", cfm_q_arr, slot);
            bson_append_double(setObj, varName, q);

            snprintf(varName, sizeof(varName), "%s.%d", cfm_expect_arr, slot);
            bson_append_double(setObj, varName, e);

            snprintf(varName, sizeof(varName), "%s.%d", cfm_deviance_arr, slot);
            bson_append_double(setObj, varName, d);

            ip = ip->next;
        }

        bson_append_finish_object(setObj);

        bson_from_buffer(&setOp, &bb);
        mongo_update(conn, db, &keys, &setOp, 0);       // no upsert

        bson_destroy(&setOp);

        if (!MongoCheckForError(conn, dbOp, keyHash, &didUpdate))
        {
            CfOut(cf_error, "", "!! Will not insert monitoring data");
            bson_destroy(&keys);
            return;
        }

        if (!didUpdate)
        {
            // no previously existing object with given key - insert
            // note that this happens only once per object (~0.000001% of the time)

            CfOut(cf_verbose, "", " -> Inserting new monitoring object for %s,%s", keyHash, monId);

            bson_buffer_init(&bb);

            bson_append_string(&bb, cfr_keyhash, keyHash);
            bson_append_string(&bb, cfm_id, monId);

            arr = bson_append_start_array(&bb, cfm_q_arr);
            for (i = 0; i < numSlots; i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_double(arr, iStr, 0.0);
            }
            bson_append_finish_object(arr);

            arr = bson_append_start_array(&bb, cfm_expect_arr);
            for (i = 0; i < numSlots; i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_double(arr, iStr, 0.0);
            }
            bson_append_finish_object(arr);

            arr = bson_append_start_array(&bb, cfm_deviance_arr);
            for (i = 0; i < numSlots; i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_double(arr, iStr, 0.0);
            }
            bson_append_finish_object(arr);

            bson_from_buffer(&insertOp, &bb);

            // upsert instead of insert avoids duplicates (if race conditions occur)
            mongo_update(conn, db, &keys, &insertOp, MONGO_UPDATE_UPSERT);

            bson_destroy(&insertOp);

            ip = slotStart;     // go back to update into empty object
        }

        bson_destroy(&keys);

        if (++iterations > 5000)
        {
            CfOut(cf_error, "",
                  "!! Anomaly: More than 5000 iterations when saving monitor data (%s) - check DB consistency", dbOp);
            break;
        }
    }

}

/*****************************************************************************/

void CFDB_SaveMonitorHistograms(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson_buffer *arr;
    bson host_key;              // host description
    bson setOp;
    char monId[128], kStr[32];
    Item *ip;
    int k;
    double currHist;
    char *sp;

    for (ip = data; ip != NULL; ip = ip->next)  // each line is a monitoring histogram array
    {

        if (sscanf(ip->name, "%127[^,]", monId) != 1)
        {
            CfOut(cf_error, "", "!! Could not find monitoring id when saving histograms");
            continue;
        }

        // find right host and monitoring id
        bson_buffer_init(&bb);
        bson_append_string(&bb, cfr_keyhash, keyhash);
        bson_append_string(&bb, cfm_id, monId);
        bson_from_buffer(&host_key, &bb);

        bson_buffer_init(&bb);
        setObj = bson_append_start_object(&bb, "$set");

        sp = ip->name;

        while (*(++sp) != ',')
        {
        }

        sp++;

        arr = bson_append_start_array(setObj, cfr_histo);

        for (k = 0; k < CF_GRAINS; k++)
        {
            sscanf(sp, "%lf", &currHist);

            while (*(++sp) != ',')
            {
            }

            if (currHist < 0)
            {
                currHist = 1;
            }

            sp++;
            snprintf(kStr, sizeof(kStr), "%d", k);
            bson_append_double(arr, kStr, currHist);
        }

        bson_append_finish_object(arr);
        bson_append_finish_object(setObj);
        bson_from_buffer(&setOp, &bb);

        mongo_update(conn, MONGO_DATABASE_MON_MG, &host_key, &setOp, MONGO_UPDATE_UPSERT);

        bson_destroy(&setOp);
        bson_destroy(&host_key);
    }

}

/*****************************************************************************/

static void CFDB_PutEnvironmentForHost(mongo_connection *conn, const char *keyhash, const char *environment)
{
    bson_buffer bb;
    bson host_key, op;

    /* { kH: $keyhash } */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    if (environment)
    {
        bson_buffer *env_sub;

        /* { $set: { env: $environment } } */
        bson_buffer_init(&bb);
        env_sub = bson_append_start_object(&bb, "$set");
        bson_append_string(env_sub, cfr_environment, environment);
        bson_append_finish_object(env_sub);
        bson_from_buffer(&op, &bb);
    }
    else
    {
        bson_buffer *env_sub;

        /* { $unset: { env: 1 } } */
        bson_buffer_init(&bb);
        env_sub = bson_append_start_object(&bb, "$unset");
        bson_append_string(env_sub, cfr_environment, "1");
        bson_append_finish_object(env_sub);
        bson_from_buffer(&op, &bb);
    }

    mongo_update(conn, MONGO_DATABASE, &host_key, &op, MONGO_UPDATE_UPSERT);

    bson_destroy(&op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

# define ENV_NAME_PREFIX "environment_"
# define ENV_NAME_LEN (sizeof(ENV_NAME_PREFIX) / sizeof(char) - 1)

static void CFDB_SaveEnvironment(mongo_connection *conn, const char *keyhash, const Item *data)
{
    const Item *i;
    char *environment = NULL;

    for (i = data; i; i = i->next)
    {
        char classname[CF_MAXVARSIZE] = "";

        sscanf(i->name, "%[^,]", classname);

        if (!strncmp(ENV_NAME_PREFIX, classname, ENV_NAME_LEN) && strlen(classname) > ENV_NAME_LEN)
        {
            if (environment)
            {
                /* FIXME: warn user */
                free(environment);
            }
            environment = xstrdup(classname + ENV_NAME_LEN);
        }
    }

    CFDB_PutEnvironmentForHost(conn, keyhash, environment);

    free(environment);
}

/*****************************************************************************/

void CFDB_SaveClasses(mongo_connection *conn, char *keyhash, Item *data)
/*
 *  Replacing existing class entry, but not deleting "old" entries (purging)
 */
{
    bson_buffer bb, *setObj, *clObj, *keyArr, *keyArrField;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char name[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    time_t t;
    double e = 0, dev = 0;
    char iStr[32];
    int i;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson classKeys;

    bson_buffer bbClassKeys;
    bson_buffer_init(&bbClassKeys);
    keyArrField = bson_append_start_object(&bbClassKeys, cfr_class_keys);
    keyArr = bson_append_start_array(keyArrField, "$each");

    for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
    {
        sscanf(ip->name, "%[^,],%ld,%lf,%lf\n", name, &t, &e, &dev);
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(keyArr, iStr, name);
    }

    bson_append_finish_object(keyArr);
    bson_append_finish_object(keyArrField);
    bson_from_buffer(&classKeys, &bbClassKeys);
    
    bson_buffer_init(&bb);
    bson_append_bson(&bb, "$addToSet", &classKeys);
    bson_from_buffer(&setOp, &bb);

    mongo_update(conn, MONGO_ARCHIVE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    bson_destroy(&setOp);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

// insert objects

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%[^,],%ld,%lf,%lf\n", name, (time_t *) &t, &e, &dev);
        snprintf(varName, sizeof(varName), "%s.%s", cfr_class, name);

        clObj = bson_append_start_object(setObj, varName);

        bson_append_double(clObj, cfr_obs_E, e);
        bson_append_double(clObj, cfr_obs_sigma, dev);
        bson_append_int(clObj, cfr_time, t);
        bson_append_finish_object(clObj);
    }

    bson_append_finish_object(setObj);

// insert keys into numbered key array - needed for efficient regexes
    bson_append_bson(&bb, "$addToSet", &classKeys);
    bson_destroy(&classKeys);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "SaveClasses", keyhash, NULL);

    bson_destroy(&setOp);
    bson_destroy(&host_key);

    CFDB_SaveEnvironment(conn, keyhash, data);
}

/*****************************************************************************/

void CFDB_SaveVariables(mongo_connection *conn, char *keyhash, Item *data)
/* Should be deprecated some time - was replaced after Nova 2.0.2 */
{
    bson_buffer bb;
    bson_buffer *unset, *setObj, *arr;
    bson host_key;              // host description
    bson setOp, unsetOp;
    Item *ip;
    int i;
    char iStr[32];
    Rlist *rp, *list;
    char type[CF_SMALLBUF], lval[CF_MAXVARSIZE], rval[CF_BUFSIZE], scope[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

// delete any old report first

    unset = bson_append_start_object(&bb, "$unset");
    bson_append_int(unset, cfr_vars, 1);
    bson_append_finish_object(unset);
    bson_from_buffer(&unsetOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &unsetOp, 0);
    bson_destroy(&unsetOp);

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        if (strncmp(ip->name, "S: ", 3) == 0)
        {
            scope[0] = '\0';
            sscanf(ip->name + 3, "%254[^\n]", scope);
            continue;
        }

        rval[0] = '\0';  // rval may be empty

        sscanf(ip->name, "%4[^,], %255[^,], %2040[^\n]", type, lval, rval);

        if (strchr(lval, '/'))
        {
            // Private data not meant to be seen
            continue;
        }

        snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_type);
        bson_append_string(setObj, varName, type);

        snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_rval);

        if (IsCfList(type))
        {
            arr = bson_append_start_array(setObj, varName);

            list = ParseShownRlist(rval);

            for (rp = list, i = 0; rp != NULL; rp = rp->next, i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(arr, iStr, rp->item);
            }

            DeleteRlist(list);

            bson_append_finish_object(arr);
        }
        else
        {
            bson_append_string(setObj, varName, rval);
        }
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "SaveVariables", keyhash, NULL);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveVariables2(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj, *arr;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    int i;
    char iStr[32];
    long tl;
    time_t t;
    Rlist *rp = NULL, *list = NULL;
    char type[CF_SMALLBUF], lval[CF_MAXVARSIZE], rval[CF_BUFSIZE], scope[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        if (strncmp(ip->name, "S:", 2) == 0)
        {
            scope[0] = '\0';
            sscanf(ip->name + 2, "%254[^\n]", scope);
            continue;
        }

        rval[0] = '\0';  // rval may be empty

        sscanf(ip->name, "%4[^,],%ld,%255[^,],%2040[^\n]", type, &tl, lval, rval);

        t = (time_t) tl;

        if (strchr(lval, '/'))
        {
            // Private data not meant to be seen
            continue;
        }

        char scope_canonified[CF_MAXVARSIZE] = {0};
        ReplaceChar(scope, scope_canonified, sizeof(scope_canonified), '.', '_');

        char lval_canonified[CF_MAXVARSIZE] = {0};
        ReplaceChar(lval, lval_canonified, sizeof(lval_canonified), '.', '_');

        snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope_canonified, lval_canonified, cfr_type);
        bson_append_string(setObj, varName, type);

        snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope_canonified, lval_canonified, cfr_time);
        bson_append_int(setObj, varName, t);

        snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope_canonified, lval_canonified, cfr_rval);

        if (IsCfList(type))
        {
            arr = bson_append_start_array(setObj, varName);

            list = ParseShownRlist(rval);

            for (rp = list, i = 0; rp != NULL; rp = rp->next, i++)
            {
                snprintf(iStr, sizeof(iStr), "%d", i);

                char rval_with_newline[CF_MAXTRANSSIZE] = "\0";
                ReplaceChar(rp->item, rval_with_newline, CF_MAXTRANSSIZE, CF_N_CODE, '\n');

                bson_append_string(setObj, iStr, rval_with_newline);
            }

            DeleteRlist(list);

            bson_append_finish_object(arr);
        }
        else
        {
            char rval_with_newline[CF_MAXTRANSSIZE] = "\0";
            ReplaceChar(rval, rval_with_newline, CF_MAXTRANSSIZE, CF_N_CODE, '\n');

            bson_append_string(setObj, varName, rval_with_newline);
        }
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "SaveVariables2", keyhash, NULL);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveTotalCompliance(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char version[CF_MAXVARSIZE];
    Item *keys = NULL, *addedKey = NULL;
    int kept, repaired, notrepaired;
    char varName[CF_MAXVARSIZE];
    long t;
    time_t then;
    bson_buffer *sub;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%ld,%127[^,],%d,%d,%d\n", &t, version, &kept, &repaired, &notrepaired);
        then = (time_t) t;

        snprintf(varName, sizeof(varName), "%s.%s", cfr_total_compliance, GenTimeKey(then));

        // check for duplicate keys
        addedKey = ReturnItemIn(keys, varName);
        if (addedKey)
        {
            CfDebug("!! Duplicate key \"%s\" in SaveTotalCompliance - ignoring second time=%s - stored=%s", varName,
                    cf_ctime(&then), addedKey->classes);
            continue;           // avoids DB update failure
        }
        else
        {
            PrependItem(&keys, varName, cf_ctime(&then));
        }

        sub = bson_append_start_object(setObj, varName);
        bson_append_int(sub, cfr_time, then);
        bson_append_string(sub, cfr_version, version);
        bson_append_int(sub, cfr_kept, kept);
        bson_append_int(sub, cfr_repaired, repaired);
        bson_append_int(sub, cfr_notkept, notrepaired);
        bson_append_finish_object(sub);
    }

    DeleteItemList(keys);

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

static void CreateUniquePromiseLogDBKey(char *handle,char *cause, char *buffer, int bufsize)
{
    char escapedCause[CF_BUFSIZE] = { 0 };
    EscapeJson(cause, escapedCause, sizeof(escapedCause));

    char nameNoDot[CF_BUFSIZE] = { 0 };
    ReplaceChar(escapedCause,nameNoDot,sizeof(nameNoDot),'.','_');

    snprintf(buffer, bufsize, "%s@%s",handle,nameNoDot);
}

/*****************************************************************************/
void CFDB_SavePromiseLog(mongo_connection *conn, char *keyhash, PromiseLogState state, Item *data)
{
    char *collName;

    switch (state)
    {
    case PROMISE_LOG_STATE_REPAIRED:
        collName = "logs_rep";
        break;
    case PROMISE_LOG_STATE_NOTKEPT:
        collName = "logs_nk";
        break;
    default:
        CfOut(cf_error, "", "!! Unknown promise log report type (%d)", state);
        return;
    }

    bson_buffer bb;
    bson_buffer_init(&bb);

    bson_buffer *setObj = bson_append_start_object(&bb, "$set");

    Item *uniquePromiseKeysList = NULL;
    Item *promiseKeysAndTimeList = NULL;

    for (Item *ip = data; ip != NULL; ip = ip->next)
    {
        long then = -1;
        char handle[CF_MAXVARSIZE] = "\0",
                reason[CF_BUFSIZE] = "\0";

        sscanf(ip->name, "%ld,%254[^,],%1024[^\n]", &then, handle, reason);

        if (NULL_OR_EMPTY(handle)
                || NULL_OR_EMPTY(reason)
                || (then < 0))
        {
            /* Data seems to be corrupted before/during the transfer
             * or parsed incorrectly by the hub,
             * skip silently as this might fill up the logs.

             * The debug log will show this information
             * if something needs to be diagnosed
             */
            continue;
        }

        time_t tthen = (time_t) then;

        char newKey[CF_BUFSIZE] = { 0 };
        CreateUniquePromiseLogDBKey(handle,reason,newKey,sizeof(newKey));

        if (!IsItemIn(uniquePromiseKeysList,newKey))
        {
            PrependItem(&uniquePromiseKeysList, newKey, NULL);

            char causeKey[CF_BUFSIZE] = { 0 };
            snprintf(causeKey, sizeof(causeKey), "%s.%s.%s",collName,newKey,cfr_cause);
            bson_append_string(setObj, causeKey,reason);

            char handleKey[CF_BUFSIZE] = { 0 };
            snprintf(handleKey, sizeof(handleKey), "%s.%s.%s",collName,newKey,cfr_promisehandle);
            bson_append_string(setObj, handleKey,handle);
        }

        PrependFullItem(&promiseKeysAndTimeList, newKey, NULL, 0, tthen);
    }

    bson_append_finish_object(setObj);

    // sort timestamp list
    Item *sortedList = NULL;
    sortedList = SortItemListNames(promiseKeysAndTimeList);

    for(Item *ip2 = uniquePromiseKeysList; ip2 != NULL; ip2 = ip2->next)
    {
        char timeKey[CF_BUFSIZE] = { 0 };
        snprintf(timeKey, sizeof(timeKey), "%s.%s.%s",collName,ip2->name,cfr_time);

        bson_buffer *addToSetObj = bson_append_start_object(&bb, "$addToSet");
        bson_buffer *timeArrayObj = bson_append_start_object(addToSetObj, timeKey);
        bson_buffer *eachObj = bson_append_start_array(timeArrayObj, "$each");

        int timestampArrayIdx = 0;
        bool firstEntry = true;

        for (Item *ip = sortedList; ip != NULL; ip = ip->next)
        {
            if(strcmp(ip->name, ip2->name) == 0)
            {
                char varName[CF_BUFSIZE] = { 0 };
                snprintf(varName, sizeof(varName), "%d", timestampArrayIdx++);
                bson_append_int(eachObj, varName, ip->time);

                firstEntry = false;
            }
            else if(!firstEntry)
            {
                break;
            }            
        }

        bson_append_finish_object(eachObj);
        bson_append_finish_object(timeArrayObj);
        bson_append_finish_object(addToSetObj);       
    }

    bson setOp;
    bson_from_buffer(&setOp, &bb);

    DeleteItemList(uniquePromiseKeysList);
    DeleteItemList(sortedList);

    // find right host
    bson host_key;
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "Update failed for : ", keyhash, NULL);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}


/*****************************************************************************/

void CFDB_SaveLastSeen(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj, *sub;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char inout, ipaddr[CF_MAXVARSIZE];
    char hostkey[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    double ago = 0, average = 0, dev = 0;
    long fthen = 0;
    time_t then = 0;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%c %625s %250s %ld %lf %lf %lf\n", &inout, hostkey, ipaddr, &fthen, &ago, &average, &dev);

        then = (time_t) fthen;

        snprintf(varName, sizeof(varName), "%s.%c%s", cfr_lastseen, inout, hostkey);

        sub = bson_append_start_object(setObj, varName);
        bson_append_string(sub, cfr_ipaddr, ipaddr);
        bson_append_double(sub, cfr_hrsago, ago);
        bson_append_double(sub, cfr_hrsavg, average);
        bson_append_double(sub, cfr_hrsdev, dev);
        bson_append_int(sub, cfr_time, then);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveMeter(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson_buffer *sub;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char varName[CF_MAXVARSIZE];
    char type;
    double kept, repaired;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%c: %lf %lf", &type, &kept, &repaired);
        snprintf(varName, sizeof(varName), "%s.%c", cfr_meter, type);

        sub = bson_append_start_object(setObj, varName);
        bson_append_double(sub, cfr_meterkept, kept);
        bson_append_double(sub, cfr_meterrepaired, repaired);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

/* HostRankMethod can be passed as miltiple option merge opt|opt|opt */
void CFDB_SaveScore(mongo_connection *conn, char *keyhash, Item *data, HostRankMethod method)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;  // host description
    bson setOp;

    /* find right host */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    /* build score object */
    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$set");

    double kept_full[meter_endmark] = {0};
    double repaired_full[meter_endmark] = {0};

    /* gather meters for score calculetion from meter report */
    for (Item *ip = data; ip != NULL; ip = ip->next)
    {
        char type;
        double kept;
        double repaired;

        sscanf(ip->name,"%c: %lf %lf",&type,&kept,&repaired);

        switch (type)
        {
            case cfmeter_hour:
                kept_full[meter_compliance_hour] = kept;
                repaired_full[meter_compliance_hour] = repaired;
                break;

            case cfmeter_day:
                kept_full[meter_compliance_day] = kept;
                repaired_full[meter_compliance_day] = repaired;
                break;

            case cfmeter_week:
                kept_full[meter_compliance_week] = kept;
                repaired_full[meter_compliance_week] = repaired;
                break;

            case cfmeter_perf:
                kept_full[meter_perf_day] = kept;
                repaired_full[meter_perf_day] = repaired;
                break;

            case cfmeter_anomaly:
                kept_full[meter_anomalies_day] = kept;
                repaired_full[meter_anomalies_day] = repaired;
                break;

            case cfmeter_comms:
                kept_full[meter_comms_hour] = kept;
                repaired_full[meter_comms_hour] = repaired;
                break;

            case cfmeter_other:
                kept_full[meter_other_day] = kept;
                repaired_full[meter_other_day] = repaired;
                break;
        }
    }

    /* calculate scores and build score db object */
    int score;

    if (method & HOST_RANK_METHOD_COMPLIANCE)
    {
        score = Nova_GetComplianceScore(HOST_RANK_METHOD_COMPLIANCE, kept_full, repaired_full);
        bson_append_int(setObj, cfr_score_comp, score);
    }

    if (method & HOST_RANK_METHOD_ANOMALY)
    {
        score = Nova_GetComplianceScore(HOST_RANK_METHOD_ANOMALY, kept_full, repaired_full);
        bson_append_int(setObj, cfr_score_anom, score);
    }

    if (method & HOST_RANK_METHOD_PERFORMANCE)
    {
        score = Nova_GetComplianceScore(HOST_RANK_METHOD_PERFORMANCE, kept_full, repaired_full);
        bson_append_int(setObj, cfr_score_perf, score);
    }

    if (method & HOST_RANK_METHOD_LASTSEEN)
    {
        score = Nova_GetComplianceScore(HOST_RANK_METHOD_LASTSEEN, kept_full, repaired_full);
        bson_append_int(setObj, cfr_score_lastseen, score);
    }

    if (method & HOST_RANK_METHOD_MIXED)
    {
        score = Nova_GetComplianceScore(HOST_RANK_METHOD_MIXED, kept_full, repaired_full);
        bson_append_int(setObj, cfr_score_mixed, score);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSoftwareDates(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char type;
    time_t t;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%c:%ld", &type, &t);

        switch (type)
        {
        case 'S':
            bson_append_int(setObj, cfr_software_t, t);
            break;
        }
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePerformance(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson_buffer *sub;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char varName[CF_MAXVARSIZE];
    long t;
    char eventname[CF_MAXVARSIZE], eventnameKey[CF_MAXVARSIZE];
    double measure = 0, average = 0, dev = 0;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        eventname[0] = '\0';
        sscanf(ip->name, "%ld,%lf,%lf,%lf,%255[^\n]\n", &t, &measure, &average, &dev, eventname);

        // Need to canonify the dots, as dot is not allowed in a mongo key
        ReplaceChar(eventname, eventnameKey, sizeof(eventnameKey), '.', '_');

        snprintf(varName, sizeof(varName), "%s.%s", cfr_performance, eventnameKey);

        sub = bson_append_start_object(setObj, varName);
        bson_append_string(sub, cfr_perf_event, eventname);
        bson_append_double(sub, cfr_obs_q, measure);
        bson_append_double(sub, cfr_obs_E, average);
        bson_append_double(sub, cfr_obs_sigma, dev);
        bson_append_int(sub, cfr_time, t);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSetUid(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *keyArr, *set;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char progName[CF_MAXVARSIZE];
    char iStr[32];
    int i;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

// old report is replaced

    bson_buffer_init(&bb);

// insert keys into key array

    set = bson_append_start_object(&bb, "$set");
    keyArr = bson_append_start_array(set, cfr_setuid);

    for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
    {
        sscanf(ip->name, "%255[^\n]", progName);
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(keyArr, iStr, progName);
    }

    bson_append_finish_object(keyArr);
    bson_append_finish_object(set);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePromiseCompliance(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson_buffer *sub;
    bson_buffer *keyArr, *keyAdd, *keyArrField;
    Item *keys = NULL, *addedKey = NULL;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char varName[CF_MAXVARSIZE];
    char handle[CF_MAXVARSIZE];
    char status, statusStr[16];
    time_t then;
    double av, dev;
    int i;
    char iStr[32];

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%ld,%255[^,],%c,%lf,%lf\n", &then, handle, &status, &av, &dev);

        snprintf(varName, sizeof(varName), "%s.%s", cfr_promisecompl, handle);
        snprintf(statusStr, sizeof(statusStr), "%c", status);

        addedKey = ReturnItemIn(keys, handle);
        if (addedKey)
        {
            CfDebug("!! Duplicate key \"%s\" in SavePromiseCompliance - ignoring second time=%s - stored=%s", varName,
                    cf_ctime(&then), addedKey->classes);
            continue;           // avoids DB update failure
        }
        else
        {
            PrependItem(&keys, handle, cf_ctime(&then));
        }

        sub = bson_append_start_object(setObj, varName);
        bson_append_string(sub, cfr_promisestatus, statusStr);
        bson_append_double(sub, cfr_obs_E, av);
        bson_append_double(sub, cfr_obs_sigma, dev);
        bson_append_int(sub, cfr_time, then);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

// insert keys into numbered key array

    keyAdd = bson_append_start_object(&bb, "$addToSet");
    keyArrField = bson_append_start_object(keyAdd, cfr_promisecompl_keys);
    keyArr = bson_append_start_array(keyAdd, "$each");

    for (ip = keys, i = 0; ip != NULL; ip = ip->next, i++)
    {
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(keyArr, iStr, ip->name);
    }

    bson_append_finish_object(keyArr);
    bson_append_finish_object(keyArrField);
    bson_append_finish_object(keyAdd);

    DeleteItemList(keys);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileChanges(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char name[CF_MAXVARSIZE] = { 0 }, nameNoDot[CF_MAXVARSIZE] = { 0 };
    char varName[128] = { 0 };
    bson_buffer *sub;
    time_t then;
    long date;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%ld,%255[^\n]", &date, name);
        then = (time_t) date;

        char escapedname[CF_BUFSIZE] = { 0 };
        EscapeJson(name, escapedname, sizeof(escapedname));
        ReplaceChar(escapedname, nameNoDot, sizeof(nameNoDot), '.', '_');

        snprintf(varName, sizeof(varName), "%s.%s@%ld", cfr_filechanges, nameNoDot, date);
        sub = bson_append_start_object(setObj, varName);
        bson_append_int(sub, cfr_time, then);
        bson_append_string(sub, cfr_name, name);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);

    mongo_update(conn, MONGO_ARCHIVE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileDiffs(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char name[CF_MAXVARSIZE], change[CF_BUFSIZE], nameNoDot[CF_MAXVARSIZE];
    char varName[128];
    bson_buffer *sub;
    time_t then;
    long t;
    char *sp;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        change[0] = '\0';
        sscanf(ip->name, "%ld|%255[^|]|%2047[^\n]", &t, name, change);

        for (sp = change; *sp != '\0'; sp++)
        {
            if (*sp == CF_N_CODE)
            {
                *sp = '\n';
            }
        }

        then = (time_t) t;

        ReplaceChar(name, nameNoDot, sizeof(nameNoDot), '.', '_');
        snprintf(varName, sizeof(varName), "%s.%s@%ld", cfr_filediffs, nameNoDot, then);

        sub = bson_append_start_object(setObj, varName);
        bson_append_int(sub, cfr_time, then);
        bson_append_string(sub, cfr_name, name);
        bson_append_string(sub, cfr_diff, change);
        bson_append_finish_object(sub);
    }

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);

    mongo_update(conn, MONGO_ARCHIVE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveBundles(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *setObj, *sub;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    char bundle[CF_MAXVARSIZE];
    char varName[CF_MAXVARSIZE];
    double compliance = 0, average = 0, dev = 0;
    long fthen = 0;
    time_t then = 0;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        sscanf(ip->name, "%250s %ld %lf %lf %lf\n", bundle, &fthen, &compliance, &average, &dev);

        then = (time_t) fthen;

        // ignore values with corrupted timestamps
        if(!cf_ctime(&then))
        {
            continue;
        }

        snprintf(varName, sizeof(varName), "%s.%s", cfr_bundles, bundle);

        sub = bson_append_start_object(setObj, varName);
        bson_append_double(sub, cfr_bundlecomp, compliance);
        bson_append_double(sub, cfr_bundleavg, average);
        bson_append_double(sub, cfr_bundledev, dev);
        bson_append_int(sub, cfr_time, then);
        bson_append_finish_object(sub);
    }
    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveValueReport(mongo_connection *conn, char *keyhash, Item *data)
{
    bson_buffer bb;
    bson_buffer *set;
    bson host_key;              // host description
    bson setOp;
    Item *ip;
    bson_buffer *sub1, *sub2;
    char datestr[CF_SMALLBUF];
    double kept, notkept, repaired;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    set = bson_append_start_object(&bb, "$set");

    sub1 = bson_append_start_object(set, cfr_valuereport);

    for (ip = data; ip != NULL; ip = ip->next)
    {

        sscanf(ip->name, "%100[^,],%lf,%lf,%lf\n", datestr, &kept, &repaired, &notkept);

        sub2 = bson_append_start_object(sub1, datestr);
        bson_append_string(sub2, cfr_day, datestr);
        bson_append_double(sub2, cfr_kept, kept);
        bson_append_double(sub2, cfr_repaired, repaired);
        bson_append_double(sub2, cfr_notkept, notkept);

        bson_append_finish_object(sub2);
    }

    bson_append_finish_object(sub1);
    bson_append_finish_object(set);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &setOp, MONGO_UPDATE_UPSERT);

    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveHostComplianceShift(mongo_connection *conn, const char *hostkey, int kept, int repaired,
                                  int notkept, int num_samples, time_t shift_start)
{
    bson_buffer bb;

    bson cond;
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, hostkey);
    bson_from_buffer(&cond, &bb);

    bson op;
    bson_buffer_init(&bb);
    bson_buffer *set = bson_append_start_object(&bb, "$set");
    {
        char slot_key[CF_MAXVARSIZE] = { 0 };
        int shift_slot = GetShiftSlot(shift_start);
        snprintf(slot_key, sizeof(slot_key), "%s.%d", cfr_compliance_shifts, shift_slot);

        bson_buffer *slot = bson_append_start_object(set, slot_key);
        {
            bson_append_int(&bb, cfr_kept, kept);
            bson_append_int(&bb, cfr_repaired, repaired);
            bson_append_int(&bb, cfr_notkept, notkept);
            bson_append_int(&bb, cfr_count, num_samples);
            bson_append_int(&bb, cfr_time, shift_start);
        }
        bson_append_finish_object(slot);
    }
    bson_append_finish_object(set);
    bson_from_buffer(&op, &bb);

    mongo_update(conn, MONGO_DATABASE, &cond, &op, MONGO_UPDATE_UPSERT);

    bson_destroy(&cond);
    bson_destroy(&op);
}

// TODO: deprecation candidate
void CFDB_SaveCachedTotalCompliance(mongo_connection *conn, char *policy, int slot, double kept, double repaired,
                                    double notkept, int count, time_t genTime)
{
    bson_buffer bb;
    bson_buffer *set;
    bson cacheType, setOp;
    bson_buffer *sub1;
    char varName[CF_SMALLBUF];

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfc_cachetype, cfc_cachecompliance);
    bson_from_buffer(&cacheType, &bb);

    bson_buffer_init(&bb);

    set = bson_append_start_object(&bb, "$set");

    snprintf(varName, sizeof(varName), "%s.%d", policy, slot);

    sub1 = bson_append_start_object(set, varName);

    bson_append_double(sub1, cfr_kept, kept);
    bson_append_double(sub1, cfr_repaired, repaired);
    bson_append_double(sub1, cfr_notkept, notkept);
    bson_append_int(sub1, cfc_count, count);
    bson_append_int(sub1, cfc_timegen, genTime);

    bson_append_finish_object(sub1);
    bson_append_finish_object(set);

    bson_from_buffer(&setOp, &bb);

    mongo_update(conn, MONGO_CACHE, &cacheType, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "SaveCachedTotalCompliance", policy, NULL);

    bson_destroy(&setOp);
    bson_destroy(&cacheType);
}

/*****************************************************************************/

void CFDB_SaveLastHostUpdate(mongo_connection *conn, char *hostKey)
{
    CFDB_SaveLastUpdate(conn, MONGO_DATABASE, cfr_keyhash, hostKey);
}

/*****************************************************************************/

void CFDB_SaveLastUpdate(mongo_connection *conn, char *database, char *keyField, char *keyhash)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson host_key;              // host description
    bson setOp;

// find right host
    bson_buffer_init(&bb);
    bson_append_string(&bb, keyField, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);

    setObj = bson_append_start_object(&bb, "$set");

    bson_append_int(setObj, cfr_day, (long) time(NULL));

    bson_append_finish_object(setObj);

    bson_from_buffer(&setOp, &bb);
    mongo_update(conn, database, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    bson_destroy(&setOp);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLastHostUpdateSize(mongo_connection *conn, char *hostkey, int update_size)
/*
 * NOTE: it may be more useful to record averages and store in vitals,
 *       we could then show trends and graphs too (instead of just last sample)
 **/
{
    bson_buffer bb;
    bson host_id;

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, hostkey);
    bson_from_buffer(&host_id, &bb);

    bson_buffer_init(&bb);

    bson_buffer *set = bson_append_start_object(&bb, "$set");
    bson_append_int(set, cfr_last_update_size, update_size);
    bson_append_finish_object(set);

    bson set_operation;
    bson_from_buffer(&set_operation, &bb);

    mongo_update(conn, MONGO_DATABASE, &host_id, &set_operation, MONGO_UPDATE_UPSERT);

    bson_destroy(&set_operation);
    bson_destroy(&host_id);
}

/*****************************************************************************/

int CFDB_AddNote(mongo_connection *conn, char *keyhash, int reportType, char *nid,
                 char *reportData, char *username, long datetime, char *msg)
{
    bson_buffer bb;
    bson host_key;
    bson_buffer *setObj, *sub;
    bson setOp;
    bson b_key;
    bson_buffer buf_key;

    int options = MONGO_INDEX_UNIQUE | MONGO_INDEX_DROP_DUPS;

    /* for getting object id */
    mongo_cursor *cursor;
    bson_iterator it1;
    bson field;
    char objectId[CF_MAXVARSIZE] = { 0 };
    bson_oid_t bsonid;
    int found = false, newnote = false, retval = true;

    // Add index, TODO: must do this while creating the database

    bson_buffer_init(&buf_key);
    bson_append_int(&buf_key, cfn_keyhash, 1);
    bson_append_int(&buf_key, cfn_reportdata, 1);
    bson_from_buffer(&b_key, &buf_key);
    mongo_create_index(conn, MONGO_NOTEBOOK, &b_key, options, NULL);
    bson_destroy(&b_key);

    // find right host
    bson_buffer_init(&bb);

    if (!NULL_OR_EMPTY(nid))
    {
        bson_oid_from_string(&bsonid, nid);
        bson_append_oid(&bb, "_id", &bsonid);
    }
    else
    {
        newnote = true;
        bson_append_new_oid(&bb, "_id");
        bson_append_string(&bb, cfn_keyhash, keyhash);

        if (!NULL_OR_EMPTY(reportData))
        {
            bson_append_string(&bb, cfn_reportdata, reportData);
        }
        bson_append_int(&bb, cfn_reporttype, reportType);
    }
    bson_from_buffer(&host_key, &bb);

    char msg_esc[CF_BUFSIZE] = { 0 };
    strncpy(msg_esc, msg, CF_BUFSIZE);
    EscapeChar(msg_esc, sizeof(msg_esc), '\\');
    EscapeChar(msg_esc, sizeof(msg_esc), '\"');

    bson_buffer_init(&bb);
    setObj = bson_append_start_object(&bb, "$addToSet");
    sub = bson_append_start_object(setObj, cfn_note);
    bson_append_string(sub, cfn_username, username);
    bson_append_string(sub, cfn_message, msg_esc);
    bson_append_int(sub, cfn_datetime, datetime);
    bson_append_finish_object(sub);
    bson_append_finish_object(setObj);
    bson_from_buffer(&setOp, &bb);

    mongo_update(conn, MONGO_NOTEBOOK, &host_key, &setOp, MONGO_UPDATE_UPSERT);
    MongoCheckForError(conn, "AddNote", keyhash, NULL);
    bson_destroy(&setOp);

    if (newnote)
    {
        // get the objectid
        bson_buffer_init(&bb);
        bson_append_int(&bb, "_id", 1);
        bson_from_buffer(&field, &bb);

        cursor = mongo_find(conn, MONGO_NOTEBOOK, &host_key, &field, 0, 0, CF_MONGO_SLAVE_OK);
        MongoCheckForError(conn, "GetNoteID", keyhash, NULL);
        bson_destroy(&field);

        while (mongo_cursor_next(cursor) && !found)
        {
            bson_iterator_init(&it1, cursor->current.data);
            objectId[0] = '\0';
            while (bson_iterator_next(&it1))
            {
                switch (bson_iterator_type(&it1))
                {
                case bson_oid:
                    if (strcmp(bson_iterator_key(&it1), "_id") == 0)
                    {
                        bson_oid_to_string(bson_iterator_oid(&it1), objectId);
                        found = true;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if (!found)
        {
            CfOut(cf_verbose, "", "!! Unable to add note");
            retval = false;
        }
        snprintf(nid, CF_MAXVARSIZE, "%s", objectId);
    }
    bson_destroy(&host_key);
    return retval;
}

/*****************************************************************************/

// WTF: the only db_save function name that does not start with Save. necessary?
int CFDB_MarkAsDeleted(mongo_connection *dbconn, char *keyHash)
{
    bson_buffer bb;
    bson_buffer *setObj;
    bson setOp, empty;

    if (!IsDefinedClass("am_policy_hub") && !AM_PHP_MODULE)
    {
        CfOut(cf_verbose, "", "Ignoring caching of deleted hosts - not called by php module");
        return false;
    }

    Rlist *hostKeyList = SplitStringAsRList(keyHash, ',');

    for (Rlist *rp = hostKeyList; rp != NULL; rp = rp->next)
    {
        bson_buffer_init(&bb);
        setObj = bson_append_start_object(&bb, "$addToSet");
        bson_append_string(setObj, cfr_deleted_hosts, ScalarValue(rp));
        bson_append_finish_object(setObj);
        bson_from_buffer(&setOp, &bb);

        mongo_update(dbconn, MONGO_SCRATCH, bson_empty(&empty), &setOp, MONGO_UPDATE_UPSERT);
        MongoCheckForError(dbconn, "MarkHostAsDeleted", ScalarValue(rp), NULL);
        bson_destroy(&setOp);
    }

    DeleteRlist(hostKeyList);

    return true;
}

/*****************************************************************************/

void CFDB_SaveExecutionStatus(mongo_connection *conn, char *keyhash, bool is_black)
{
    bson_buffer bb;
    bson host_key;  // host description
    bson set_op;

    /* find right host */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);
    bson_append_start_object(&bb, "$set");

    /* save report to mongo */
    bson_append_bool(&bb, cfr_is_black, is_black);
    bson_append_finish_object(&bb);

    bson_from_buffer(&set_op, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLastAgentExecution(mongo_connection *conn, char *keyhash, long last_agent_exec)
{
    bson_buffer bb;
    bson host_key;  // host description
    bson set_op;

    /* find right host */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);
    bson_append_start_object(&bb, "$set");

    /* save report to mongo */
    bson_append_long(&bb, cfr_last_execution, last_agent_exec);

    bson_append_finish_object(&bb);

    bson_from_buffer(&set_op, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveDeltaAgentExecution(mongo_connection *conn, char *keyhash, long delta_schedule)
{
    bson_buffer bb;
    bson host_key;  // host description
    bson set_op;

    /* find right host */
    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, keyhash);
    bson_from_buffer(&host_key, &bb);

    bson_buffer_init(&bb);
    bson_append_start_object(&bb, "$set");

    /* save report to mongo */
    bson_append_long(&bb, cfr_schedule, delta_schedule);
    bson_append_finish_object(&bb);

    bson_from_buffer(&set_op, &bb);
    mongo_update(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}
