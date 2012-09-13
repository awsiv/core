#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "env_context.h"
#include "files_names.h"
#include "db_save.h"
#include "granules.h"
#include "item_lib.h"
#include "sort.h"
#include "conversion.h"
#include "bson_lib.h"

#include <assert.h>

/*****************************************************************************/
/* Cache / scratch space                                                     */
/*****************************************************************************/

bool CFDB_PutValue(EnterpriseDB *conn, const char *lval, const char *rval, const char *db_name)
{
    if (!IsDefinedClass("am_policy_hub") && !AM_PHP_MODULE)
    {
        return false;
    }

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_string(&set_op, lval, rval);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    if (MongoUpdate(conn, db_name, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL) != MONGO_OK)
    {
        bson_destroy(&set_op);
        return false;
    }

    bson_destroy(&set_op);

    return true;
}

/*****************************************************************************/

int CFDB_SaveLastseenCache(Item *lastseen)
{
    mongo dbconn;
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

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_array(&set_op, cfr_lastseen_hosts);

            for (ip = lastseen, i = 0; ip != NULL; ip = ip->next, i++)
            {
                snprintf(arrIndex, sizeof(arrIndex), "%d", i);
                {
                    bson_append_start_object(&set_op, arrIndex);

                    bson_append_string(&set_op, cfr_keyhash, ip->name);
                    bson_append_string(&set_op, cfr_ipaddr, ip->classes);
                    bson_append_int(&set_op, cfr_time, ip->time);

                    bson_append_finish_object(&set_op);
                }
            }
            bson_append_finish_object(&set_op);
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    MongoUpdate(&dbconn, MONGO_SCRATCH, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

void CFDB_SaveGoalsCache(char *goal_patterns)
{
    mongo dbconn;

    if (!IsDefinedClass("am_policy_hub"))
    {
        CfOut(cf_verbose, "", "Ignoring caching of lastseen hosts into database - we are not a policy server");
        return;
    }

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_string(&set_op, "goal_patterns", goal_patterns);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    MongoUpdate(&dbconn, MONGO_SCRATCH, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_SaveHostID(EnterpriseDB *conn, char *database, char *keyField, char *keyhash, char *ipaddr,
                     char *hostname)
/**
 *  hostname is optional, reverse lookup if not specified
 **/
{
// locate right host key

    bson host_key;              // host description

    bson_init(&host_key);
    bson_append_string(&host_key, keyField, keyhash);
    BsonFinish(&host_key);

// ip address - replace array with one el (more later - aging..)
    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_array(&set_op, cfr_ip_array);
            bson_append_string(&set_op, "0", ipaddr);
            bson_append_finish_object(&set_op);
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, database, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);

    // host_key destroyed after updating host names

    // host name
    /* If the host identifier is configured from the mission portal,
     * this will be the variable value (eg. sys.fqhost)
     * By default, this holds the reverse IP lookup
     */

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_array(&set_op, cfr_host_array);
            bson_append_string(&set_op, "0", hostname);
            bson_append_finish_object(&set_op);
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, database, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);
    bson_destroy( &set_op );

    /* reverse IP lookup name
     * Needed for cases when user has
     * configured the custom host identifier
     * This value is used in the assets page
     */

    bson_init( &set_op );
    {
        bson_append_start_object( &set_op, "$set" );
        {
            BsonAppendStringSafe( &set_op, cfr_reverse_lookup_name, IPString2Hostname( ipaddr ) );
        }
        bson_append_finish_object( &set_op );
    }
    BsonFinish( &set_op );

    MongoUpdate(conn, database, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSoftware(EnterpriseDB *conn, enum software_rep sw, char *keyhash, Item *data)
{
    int i;
    Item *ip;
    char *dbOp = { 0 };
    char packNumStr[CF_MAXVARSIZE];
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch, archStr[CF_MAXVARSIZE];

// find right host
    bson host_key;              // host description

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            switch (sw)
            {
            case sw_rep_installed:
                bson_append_start_array(&set_op, cfr_software);
                dbOp = "update installed software";
                break;
            case sw_rep_patch_installed:
                bson_append_start_array(&set_op, cfr_patch_installed);
                dbOp = "update patch installed software";
                break;
            case sw_rep_patch_avail:
                bson_append_start_array(&set_op, cfr_patch_avail);
                dbOp = "update patch available software";
                break;

            default:
                bson_append_start_array(&set_op, cfr_software);
                dbOp = "update installed software";
                break;
            }

            /* Read and insert data */

            for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
            {
                sscanf(ip->name, "%250[^,],%250[^,],%c", name, version, &arch);
                snprintf(packNumStr, sizeof(packNumStr), "%d", i);
                snprintf(archStr, sizeof(archStr), "%c", arch);
                {
                    bson_append_start_object(&set_op, packNumStr);

                    bson_append_string(&set_op, cfr_name, name);
                    bson_append_string(&set_op, cfr_version, version);
                    bson_append_string(&set_op, cfr_arch, archStr);

                    bson_append_finish_object(&set_op);
                }
            }

            bson_append_finish_object(&set_op);
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);

    MongoCheckForError(conn, dbOp, keyhash, NULL);
}

/*****************************************************************************/
/* Monitor data                                                              */
/*****************************************************************************/

void CFDB_SaveMonitorData2(EnterpriseDB *conn, char *keyHash, enum monitord_rep rep_type, Item *data)
{
    char monId[CF_SMALLBUF] = { 0 }, monDesc[CF_MAXVARSIZE] = { 0 }, monUnits[CF_SMALLBUF] = { 0 };
    char varName[CF_SMALLBUF], iStr[CF_SMALLBUF];
    bool didUpdate, haveAllMeta = false;
    char *db;
    char *dbOp;
    Item *ip, *slotStart;
    int monGlobal;
    double monExpMin, monExpMax;
    int i, slot, numSlots, iterations;
    double q, e, d, g;

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
        bson keys;

        bson_init(&keys);
        bson_append_string(&keys, cfr_keyhash, keyHash);
        bson_append_string(&keys, cfm_id, monId);
        BsonFinish(&keys);

        // create object to insert
        bson set_op;

        bson_init(&set_op);
        {
            bson_append_start_object(&set_op, "$set");

            if (haveAllMeta)        // full query only
            {
                bson_append_string(&set_op, cfm_description, monDesc);
                bson_append_string(&set_op, cfm_units, monUnits);
                bson_append_bool(&set_op, cfm_global, monGlobal);
                bson_append_double(&set_op, cfm_expmin, monExpMin);
                bson_append_double(&set_op, cfm_expmax, monExpMax);
            }

            slotStart = ip;

            while (ip && (strncmp(ip->name, "M:", 2) != 0))
            {
   	        sscanf(ip->name, "%d %lf %lf %lf %lf", &slot, &q, &e, &d, &g);
                
                if (slot < 0 || slot >= numSlots)
                {
                    CfOut(cf_error, "", "Slot %d out of range (max=%d) on %s - skipping", slot, numSlots, dbOp);
                    ip = ip->next;
                    continue;
                }

                snprintf(varName, sizeof(varName), "%s.%d", cfm_q_arr, slot);
                bson_append_double(&set_op, varName, q);

                snprintf(varName, sizeof(varName), "%s.%d", cfm_expect_arr, slot);
                bson_append_double(&set_op, varName, e);

		snprintf(varName, sizeof(varName), "%s.%d", cfm_grad_arr, slot);
		bson_append_double(&set_op, varName, g);

                snprintf(varName, sizeof(varName), "%s.%d", cfm_deviance_arr, slot);
                bson_append_double(&set_op, varName, d);

                ip = ip->next;
            }
            bson_append_finish_object(&set_op);
        }
        BsonFinish(&set_op);

        MongoUpdate(conn, db, &keys, &set_op, 0, NULL);       // no upsert

        bson_destroy(&set_op);

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

            bson insert_op;

            bson_init(&insert_op);
            {
                bson_append_string(&insert_op, cfr_keyhash, keyHash);
                bson_append_string(&insert_op, cfm_id, monId);
                {
                    bson_append_start_array(&insert_op, cfm_q_arr);
                    for (i = 0; i < numSlots; i++)
                    {
                        snprintf(iStr, sizeof(iStr), "%d", i);
                        bson_append_double(&insert_op, iStr, 0.0);
                    }
                    bson_append_finish_object(&insert_op);
                }

                {
                    bson_append_start_array(&insert_op, cfm_expect_arr);
                    for (i = 0; i < numSlots; i++)
                    {
                        snprintf(iStr, sizeof(iStr), "%d", i);
                        bson_append_double(&insert_op, iStr, 0.0);
                    }
                    bson_append_finish_object(&insert_op);
                }

                {
                    bson_append_start_array(&insert_op, cfm_deviance_arr);
                    for (i = 0; i < numSlots; i++)
                    {
                        snprintf(iStr, sizeof(iStr), "%d", i);
                        bson_append_double(&insert_op, iStr, 0.0);
                    }
                    bson_append_finish_object(&insert_op);
                }
                BsonFinish(&insert_op);

                // upsert instead of insert avoids duplicates (if race conditions occur)
                MongoUpdate(conn, db, &keys, &insert_op, MONGO_UPDATE_UPSERT, NULL);

                bson_destroy(&insert_op);

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
}

/*****************************************************************************/

void CFDB_SaveMonitorHistograms(EnterpriseDB *conn, char *keyhash, Item *data)
{
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
        bson host_key;

        bson_init(&host_key);
        bson_append_string(&host_key, cfr_keyhash, keyhash);
        bson_append_string(&host_key, cfm_id, monId);
        BsonFinish(&host_key);

        bson set_op;

        bson_init(&set_op);
        {
            bson_append_start_object(&set_op, "$set");

            sp = ip->name;

            while (*(++sp) != ',')
            {
            }

            sp++;
            {
                bson_append_start_array(&set_op, cfr_histo);

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
                    bson_append_double(&set_op, kStr, currHist);
                }

                bson_append_finish_object(&set_op);
            }
            bson_append_finish_object(&set_op);
        }
        BsonFinish(&set_op);

        MongoUpdate(conn, MONGO_DATABASE_MON_MG, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

        bson_destroy(&set_op);
        bson_destroy(&host_key);
    }

}

/*****************************************************************************/

static void CFDB_PutEnvironmentForHost(EnterpriseDB *conn, const char *keyhash, const char *environment)
{
    /* { kH: $keyhash } */
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson op;
    bson_init(&op);

    if (environment)
    {
        /* { $set: { env: $environment } } */
        bson_append_start_object(&op, "$set");
        bson_append_string(&op, cfr_environment, environment);
        bson_append_finish_object(&op);
    }
    else
    {        
        /* { $unset: { env: 1 } } */
        bson_append_start_object(&op, "$unset");
        bson_append_string(&op, cfr_environment, "1");
        bson_append_finish_object(&op);
    }

    BsonFinish(&op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

# define ENV_NAME_PREFIX "environment_"
# define ENV_NAME_LEN (sizeof(ENV_NAME_PREFIX) / sizeof(char) - 1)

static void CFDB_SaveEnvironment(EnterpriseDB *conn, const char *keyhash, const Item *data)
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

void CFDB_SaveClasses(EnterpriseDB *conn, char *keyhash, Item *data)
/*
 *  Replacing existing class entry, but not deleting "old" entries (purging)
 */
{
    Item *ip;
    char name[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    time_t t;
    double e = 0, dev = 0;
    char iStr[32];
    int i;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson class_keys;

    bson_init(&class_keys);
    {
        bson_append_start_object(&class_keys, cfr_class_keys);
        {
            bson_append_start_array(&class_keys, "$each");

            for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
            {
                sscanf(ip->name, "%[^,],%ld,%lf,%lf\n", name, &t, &e, &dev);
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(&class_keys, iStr, name);
            }

            bson_append_finish_object(&class_keys);
        }
        bson_append_finish_object(&class_keys);
    }
    BsonFinish(&class_keys);
    
    bson set_op;

    bson_init(&set_op);
    bson_append_bson(&set_op, "$addToSet", &class_keys);
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_ARCHIVE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);
    bson_destroy(&set_op);
    // host_keys and class_keys freed after adding to classes report

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        // insert objects

        for (ip = data; ip != NULL; ip = ip->next)
        {
            sscanf(ip->name, "%[^,],%ld,%lf,%lf\n", name, (time_t *) &t, &e, &dev);
            snprintf(varName, sizeof(varName), "%s.%s", cfr_class, name);

            {
                bson_append_start_object(&set_op, varName);

                bson_append_double(&set_op, cfr_obs_E, e);
                bson_append_double(&set_op, cfr_obs_sigma, dev);
                bson_append_int(&set_op, cfr_time, t);

                bson_append_finish_object(&set_op);
            }
        }
        bson_append_finish_object(&set_op);
    }
// insert keys into numbered key array - needed for efficient regexes
    bson_append_bson(&set_op, "$addToSet", &class_keys);
    bson_destroy(&class_keys);

    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);

    MongoCheckForError(conn, "SaveClasses", keyhash, NULL);

    CFDB_SaveEnvironment(conn, keyhash, data);
}

/*****************************************************************************/

void CFDB_SaveVariables(EnterpriseDB *conn, char *keyhash, Item *data)
/* Should be deprecated some time - was replaced after Nova 2.0.2 */
{
    Item *ip;
    int i;
    char iStr[32];
    Rlist *rp, *list;
    char type[CF_SMALLBUF], lval[CF_MAXVARSIZE], rval[CF_BUFSIZE], scope[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson unset_op;

    bson_init(&unset_op);
    {
        // delete any old report first

        bson_append_start_object(&unset_op, "$unset");
        bson_append_int(&unset_op, cfr_vars, 1);
        bson_append_finish_object(&unset_op);
    }
    BsonFinish(&unset_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &unset_op, 0, NULL);

    bson_destroy(&unset_op);

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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
            bson_append_string(&set_op, varName, type);

            snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_rval);

            if (IsCfList(type))
            {
                bson_append_start_array(&set_op, varName);

                list = ParseShownRlist(rval);

                for (rp = list, i = 0; rp != NULL; rp = rp->next, i++)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);
                    bson_append_string(&set_op, iStr, rp->item);
                }

                DeleteRlist(list);

                bson_append_finish_object(&set_op);
            }
            else
            {
                bson_append_string(&set_op, varName, rval);
            }
        }

        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);

    MongoCheckForError(conn, "SaveVariables", keyhash, NULL);
}

/*****************************************************************************/

void CFDB_SaveVariables2(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    int i;
    char iStr[32];
    long tl;
    time_t t;
    Rlist *rp, *list;
    char type[CF_SMALLBUF], lval[CF_MAXVARSIZE], rval[CF_BUFSIZE], scope[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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
            sscanf(ip->name, "%4[^,],%ld,%255[^,],%2040[^\n]", type, &tl, lval, rval);

            t = (time_t) tl;

            if (strchr(lval, '/'))
            {
                // Private data not meant to be seen
                continue;
            }

            snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_type);
            bson_append_string(&set_op, varName, type);

            snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_time);
            bson_append_int(&set_op, varName, t);

            snprintf(varName, sizeof(varName), "%s.%s.%s.%s", cfr_vars, scope, lval, cfr_rval);

            if (IsCfList(type))
            {
                bson_append_start_array(&set_op, varName);

                list = ParseShownRlist(rval);

                for (rp = list, i = 0; rp != NULL; rp = rp->next, i++)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);
                    bson_append_string(&set_op, iStr, rp->item);
                }

                DeleteRlist(list);

                bson_append_finish_object(&set_op);
            }
            else
            {
                bson_append_string(&set_op, varName, rval);
            }
        }

        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);

    MongoCheckForError(conn, "SaveVariables2", keyhash, NULL);
}

/*****************************************************************************/

void CFDB_SaveTotalCompliance(mongo *conn, char *keyhash, Item *data)
{    
    Item *ip;
    char version[CF_MAXVARSIZE];
    Item *keys = NULL, *addedKey = NULL;
    int kept, repaired, notrepaired;
    char varName[CF_MAXVARSIZE];
    long t;
    time_t then;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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
            {
                bson_append_start_object(&set_op, varName);
                bson_append_int(&set_op, cfr_time, then);
                bson_append_string(&set_op, cfr_version, version);
                bson_append_int(&set_op, cfr_kept, kept);
                bson_append_int(&set_op, cfr_repaired, repaired);
                bson_append_int(&set_op, cfr_notkept, notrepaired);
                bson_append_finish_object(&set_op);
            }
        }

        DeleteItemList(keys);

        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
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
void CFDB_SavePromiseLog(EnterpriseDB *conn, char *keyhash, PromiseLogState state, Item *data)
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

    Item *uniquePromiseKeysList = NULL;
    Item *promiseKeysAndTimeList = NULL;

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (Item *ip = data; ip != NULL; ip = ip->next)
        {
            long then;
            char handle[CF_MAXVARSIZE], reason[CF_BUFSIZE];

            sscanf(ip->name, "%ld,%254[^,],%1024[^\n]", &then, handle, reason);
            time_t tthen = (time_t) then;

            char newKey[CF_BUFSIZE] = { 0 };
            CreateUniquePromiseLogDBKey(handle,reason,newKey,sizeof(newKey));

            if (!IsItemIn(uniquePromiseKeysList,newKey))
            {
                PrependItem(&uniquePromiseKeysList, newKey, NULL);

                char causeKey[CF_BUFSIZE] = { 0 };
                snprintf(causeKey, sizeof(causeKey), "%s.%s.%s",collName,newKey,cfr_cause);
                bson_append_string(&set_op, causeKey,reason);

                char handleKey[CF_BUFSIZE] = { 0 };
                snprintf(handleKey, sizeof(handleKey), "%s.%s.%s",collName,newKey,cfr_promisehandle);
                bson_append_string(&set_op, handleKey,handle);
            }

            PrependFullItem(&promiseKeysAndTimeList, newKey, NULL, 0, tthen);
        }

        bson_append_finish_object(&set_op);
    }

    // sort timestamp list
    Item *sortedList = NULL;
    sortedList = SortItemListNames(promiseKeysAndTimeList);

    for(Item *ip2 = uniquePromiseKeysList; ip2 != NULL; ip2 = ip2->next)
    {
        char timeKey[CF_BUFSIZE] = { 0 };
        snprintf(timeKey, sizeof(timeKey), "%s.%s.%s",collName,ip2->name,cfr_time);
        {
            bson_append_start_object(&set_op, "$addToSet");
            {
                bson_append_start_object(&set_op, timeKey);
                {
                    bson_append_start_array(&set_op, "$each");

                    int timestampArrayIdx = 0;
                    bool firstEntry = true;

                    for (Item *ip = sortedList; ip != NULL; ip = ip->next)
                    {
                        if(strcmp(ip->name, ip2->name) == 0)
                        {
                            char varName[CF_BUFSIZE] = { 0 };
                            snprintf(varName, sizeof(varName), "%d", timestampArrayIdx++);
                            bson_append_int(&set_op, varName, ip->time);

                            firstEntry = false;
                        }
                        else if(!firstEntry)
                        {
                            break;
                        }
                    }

                    bson_append_finish_object(&set_op); // $each
                }
                bson_append_finish_object(&set_op); // timeKey
            }
            bson_append_finish_object(&set_op); // $addToSet
        }
    }
    BsonFinish(&set_op);

    DeleteItemList(uniquePromiseKeysList);
    DeleteItemList(sortedList);

    // find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);

    MongoCheckForError(conn, "SavePromiseLog", keyhash, NULL);
}


/*****************************************************************************/

void CFDB_SaveLastSeen(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char inout, ipaddr[CF_MAXVARSIZE];
    char hostkey[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    double ago = 0, average = 0, dev = 0;
    long fthen = 0;
    time_t then = 0;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            sscanf(ip->name, "%c %625s %250s %ld %lf %lf %lf\n", &inout, hostkey, ipaddr, &fthen, &ago, &average, &dev);

            then = (time_t) fthen;

            snprintf(varName, sizeof(varName), "%s.%c%s", cfr_lastseen, inout, hostkey);

            {
                bson_append_start_object(&set_op, varName);
                bson_append_string(&set_op, cfr_ipaddr, ipaddr);
                bson_append_double(&set_op, cfr_hrsago, ago);
                bson_append_double(&set_op, cfr_hrsavg, average);
                bson_append_double(&set_op, cfr_hrsdev, dev);
                bson_append_int(&set_op, cfr_time, then);
                bson_append_finish_object(&set_op);
            }
        }

        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveMeter(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char varName[CF_MAXVARSIZE];
    char type;
    double kept, repaired;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            sscanf(ip->name, "%c: %lf %lf", &type, &kept, &repaired);
            snprintf(varName, sizeof(varName), "%s.%c", cfr_meter, type);

            {
                bson_append_start_object(&set_op, varName);

                bson_append_double(&set_op, cfr_meterkept, kept);
                bson_append_double(&set_op, cfr_meterrepaired, repaired);

                bson_append_finish_object(&set_op);
            }
        }

        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

/* HostRankMethod can be passed as miltiple option merge opt|opt|opt */
void CFDB_SaveScore(EnterpriseDB *conn, char *keyhash, Item *data, HostRankMethod method)
{
    /* find right host */
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    /* build score object */
    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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
            bson_append_int(&set_op, cfr_score_comp, score);
        }

        if (method & HOST_RANK_METHOD_ANOMALY)
        {
            score = Nova_GetComplianceScore(HOST_RANK_METHOD_ANOMALY, kept_full, repaired_full);
            bson_append_int(&set_op, cfr_score_anom, score);
        }

        if (method & HOST_RANK_METHOD_PERFORMANCE)
        {
            score = Nova_GetComplianceScore(HOST_RANK_METHOD_PERFORMANCE, kept_full, repaired_full);
            bson_append_int(&set_op, cfr_score_perf, score);
        }

        if (method & HOST_RANK_METHOD_LASTSEEN)
        {
            score = Nova_GetComplianceScore(HOST_RANK_METHOD_LASTSEEN, kept_full, repaired_full);
            bson_append_int(&set_op, cfr_score_lastseen, score);
        }

        if (method & HOST_RANK_METHOD_MIXED)
        {
            score = Nova_GetComplianceScore(HOST_RANK_METHOD_MIXED, kept_full, repaired_full);
            bson_append_int(&set_op, cfr_score_mixed, score);
        }

        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSoftwareDates(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char type;
    time_t t;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            sscanf(ip->name, "%c:%ld", &type, &t);

            switch (type)
            {
            case 'S':
                bson_append_int(&set_op, cfr_software_t, t);
                break;
            }
        }

        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePerformance(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char varName[CF_MAXVARSIZE];
    long t;
    char eventname[CF_MAXVARSIZE], eventnameKey[CF_MAXVARSIZE];
    double measure = 0, average = 0, dev = 0;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            eventname[0] = '\0';
            sscanf(ip->name, "%ld,%lf,%lf,%lf,%255[^\n]\n", &t, &measure, &average, &dev, eventname);

            // Need to canonify the dots, as dot is not allowed in a mongo key
            ReplaceChar(eventname, eventnameKey, sizeof(eventnameKey), '.', '_');

            snprintf(varName, sizeof(varName), "%s.%s", cfr_performance, eventnameKey);

            {
                bson_append_start_object(&set_op, varName);

                bson_append_string(&set_op, cfr_perf_event, eventname);
                bson_append_double(&set_op, cfr_obs_q, measure);
                bson_append_double(&set_op, cfr_obs_E, average);
                bson_append_double(&set_op, cfr_obs_sigma, dev);
                bson_append_int(&set_op, cfr_time, t);

                bson_append_finish_object(&set_op);
            }
        }

        bson_append_finish_object(&set_op); //$set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveSetUid(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char progName[CF_MAXVARSIZE];
    char iStr[32];
    int i;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

// insert keys into key array
// old report is replaced
    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_array(&set_op, cfr_setuid);

            for (ip = data, i = 0; ip != NULL; ip = ip->next, i++)
            {
                sscanf(ip->name, "%255[^\n]", progName);
                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(&set_op, iStr, progName);
            }

            bson_append_finish_object(&set_op); // cfr_setuid
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SavePromiseCompliance(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *keys = NULL, *addedKey = NULL;  
    Item *ip;
    char varName[CF_MAXVARSIZE];
    char handle[CF_MAXVARSIZE];
    char status, statusStr[16];
    time_t then;
    double av, dev;
    int i;
    char iStr[32];

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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

            {
                bson_append_start_object(&set_op, varName);
                bson_append_string(&set_op, cfr_promisestatus, statusStr);
                bson_append_double(&set_op, cfr_obs_E, av);
                bson_append_double(&set_op, cfr_obs_sigma, dev);
                bson_append_int(&set_op, cfr_time, then);
                bson_append_finish_object(&set_op);
            }
        }
        bson_append_finish_object(&set_op); // $set
    }

    // insert keys into numbered key array
    {
        bson_append_start_object(&set_op, "$addToSet");
        {
            bson_append_start_object(&set_op, cfr_promisecompl_keys);
            {
                bson_append_start_array(&set_op, "$each");

                for (ip = keys, i = 0; ip != NULL; ip = ip->next, i++)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);
                    bson_append_string(&set_op, iStr, ip->name);
                }

                bson_append_finish_object(&set_op); // $each
            }
            bson_append_finish_object(&set_op); // cfr_promisecompl_keys
        }
        bson_append_finish_object(&set_op); // $addToSet
    }
    BsonFinish(&set_op);

    DeleteItemList(keys);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/
// Deprecate in favour of CFDB_SaveFileChanges
void CFDB_SaveFileChangesOld(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char name[CF_MAXVARSIZE] = { 0 }, nameNoDot[CF_MAXVARSIZE] = { 0 };
    char varName[128] = { 0 };
    time_t then;
    long date;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            sscanf(ip->name, "%ld,%255[^\n]", &date, name);
            then = (time_t) date;

            char escapedname[CF_BUFSIZE] = { 0 };
            EscapeJson(name, escapedname, sizeof(escapedname));
            ReplaceChar(escapedname, nameNoDot, sizeof(nameNoDot), '.', '_');

            snprintf(varName, sizeof(varName), "%s.%s@%ld", cfr_filechanges, nameNoDot, date);
            {
                bson_append_start_object(&set_op, varName);

                bson_append_int(&set_op, cfr_time, then);
                bson_append_string(&set_op, cfr_name, name);

                bson_append_finish_object(&set_op); // varName
            }
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_ARCHIVE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileChanges(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char name[CF_MAXVARSIZE] = { 0 }, nameNoDot[CF_MAXVARSIZE] = { 0 };
    char varName[128] = { 0 };
    time_t then;
    long date;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (ip = data; ip != NULL; ip = ip->next)
        {
            char change[2] = {0};
            char msg[CF_MAXVARSIZE] = {0},
                 temp_name[CF_MAXVARSIZE] = {0};

            change[0] = '\0';
            sscanf(ip->name, "%ld,%255[^,],%1[^,],%255[^\n]", &date, temp_name, change, msg);
            then = (time_t) date;

            if(change[0] != '\0')
            {
                // data from new client 2.3 and above
                snprintf(name, sizeof(name), "%s", temp_name);
            }
            else
            {
                continue;
            }

            char escapedname[CF_BUFSIZE] = { 0 };
            EscapeJson(name, escapedname, sizeof(escapedname));
            ReplaceChar(escapedname, nameNoDot, sizeof(nameNoDot), '.', '_');

            snprintf(varName, sizeof(varName), "%s.%s@%ld", cfr_filechanges, nameNoDot, date);
            {
                bson_append_start_object(&set_op, varName);

                bson_append_int(&set_op, cfr_time, then);
                bson_append_string(&set_op, cfr_name, name);

                bson_append_string(&set_op, cfr_filechangetype, change);
                bson_append_string(&set_op, cfr_filechangemsg, msg);

                bson_append_finish_object(&set_op); // varName
            }
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_ARCHIVE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveFileDiffs(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char name[CF_MAXVARSIZE], change[CF_BUFSIZE], nameNoDot[CF_MAXVARSIZE];
    char varName[128];
    time_t then;
    long t;
    char *sp;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

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

            {
                bson_append_start_object(&set_op, varName);
                bson_append_int(&set_op, cfr_time, then);
                bson_append_string(&set_op, cfr_name, name);
                bson_append_string(&set_op, cfr_diff, change);
                bson_append_finish_object(&set_op); // varName
            }
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_ARCHIVE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveBundles(EnterpriseDB *conn, char *keyhash, const Item *data)
{
// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        for (const Item *ip = data; ip != NULL; ip = ip->next)
        {
            char bundle_name[CF_MAXVARSIZE] = { 0 };
            char bundle_namespace[CF_MAXVARSIZE] = { 0 };
            char varName[CF_MAXVARSIZE];
            double compliance = 0, average = 0, dev = 0;
            long fthen = 0;
            time_t then = 0;

            sscanf(ip->name, "%250s %ld %lf %lf %lf %250s\n",
                   bundle_name, &fthen, &compliance, &average, &dev, bundle_namespace);

            then = (time_t) fthen;

            // ignore values with corrupted timestamps
            if(!cf_ctime(&then))
            {
                continue;
            }

            snprintf(varName, sizeof(varName), "%s.%s", cfr_bundles, bundle_name);

            {
                bson_append_start_object(&set_op, varName);

                bson_append_double(&set_op, cfr_bundlecomp, compliance);
                bson_append_double(&set_op, cfr_bundleavg, average);
                bson_append_double(&set_op, cfr_bundledev, dev);

                if (!NULL_OR_EMPTY(bundle_namespace))
                {
                    bson_append_string_n(&set_op, cfr_bundle_namespace, bundle_namespace, 255);
                }
                bson_append_int(&set_op, cfr_time, then);

                bson_append_finish_object(&set_op); // varName
            }
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveValueReport(EnterpriseDB *conn, char *keyhash, Item *data)
{
    Item *ip;
    char datestr[CF_SMALLBUF];
    double kept, notkept, repaired;

// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_object(&set_op, cfr_valuereport);

            for (ip = data; ip != NULL; ip = ip->next)
            {

                sscanf(ip->name, "%100[^,],%lf,%lf,%lf\n", datestr, &kept, &repaired, &notkept);
                {
                    bson_append_start_object(&set_op, datestr);
                    bson_append_string(&set_op, cfr_day, datestr);
                    bson_append_double(&set_op, cfr_kept, kept);
                    bson_append_double(&set_op, cfr_repaired, repaired);
                    bson_append_double(&set_op, cfr_notkept, notkept);

                    bson_append_finish_object(&set_op);
                }
            }
            bson_append_finish_object(&set_op); // cfr_valuereport
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveHostComplianceShift(EnterpriseDB *conn, const char *hostkey, int kept, int repaired,
                                  int notkept, int num_samples, time_t shift_start)
{
    bson cond;
    bson_init(&cond);
    bson_append_string(&cond, cfr_keyhash, hostkey);
    BsonFinish(&cond);

    bson op;
    bson_init(&op);
    {
        bson_append_start_object(&op, "$set");

        char slot_key[CF_MAXVARSIZE] = { 0 };
        int shift_slot = GetShiftSlot(shift_start);
        snprintf(slot_key, sizeof(slot_key), "%s.%d", cfr_compliance_shifts, shift_slot);

        {
            bson_append_start_object(&op, slot_key);

            bson_append_int(&op, cfr_kept, kept);
            bson_append_int(&op, cfr_repaired, repaired);
            bson_append_int(&op, cfr_notkept, notkept);
            bson_append_int(&op, cfr_count, num_samples);
            bson_append_int(&op, cfr_time, shift_start);

            bson_append_finish_object(&op); // slot_key
        }

        bson_append_finish_object(&op); // $set
    }
    BsonFinish(&op);

    MongoUpdate(conn, MONGO_DATABASE, &cond, &op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&cond);
    bson_destroy(&op);
}

// TODO: deprecation candidate
void CFDB_SaveCachedTotalCompliance(EnterpriseDB *conn, char *policy, int slot, double kept, double repaired,
                                    double notkept, int count, time_t genTime)
{
    char varName[CF_SMALLBUF];

    bson cacheType;

    bson_init(&cacheType);
    bson_append_string(&cacheType, cfc_cachetype, cfc_cachecompliance);
    BsonFinish(&cacheType);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        snprintf(varName, sizeof(varName), "%s.%d", policy, slot);

        {
            bson_append_start_object(&set_op, varName);

            bson_append_double(&set_op, cfr_kept, kept);
            bson_append_double(&set_op, cfr_repaired, repaired);
            bson_append_double(&set_op, cfr_notkept, notkept);
            bson_append_int(&set_op, cfc_count, count);
            bson_append_int(&set_op, cfc_timegen, genTime);

            bson_append_finish_object(&set_op); // varName
        }
        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_CACHE, &cacheType, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&cacheType);

    MongoCheckForError(conn, "SaveCachedTotalCompliance", policy, NULL);
}

/*****************************************************************************/

void CFDB_SaveLastHostUpdate(EnterpriseDB *conn, char *hostKey)
{
    CFDB_SaveLastUpdate(conn, MONGO_DATABASE, cfr_keyhash, hostKey);
}

/*****************************************************************************/

void CFDB_SaveLastUpdate(EnterpriseDB *conn, char *database, char *keyField, char *keyhash)
{
// find right host
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, keyField, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_int(&set_op, cfr_day, (long) time(NULL));
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, database, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLastHostUpdateSize(EnterpriseDB *conn, char *hostkey, int update_size)
/*
 * NOTE: it may be more useful to record averages and store in vitals,
 *       we could then show trends and graphs too (instead of just last sample)
 **/
{
    bson host_id;

    bson_init(&host_id);
    bson_append_string(&host_id, cfr_keyhash, hostkey);
    BsonFinish(&host_id);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_int(&set_op, cfr_last_update_size, update_size);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_id, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_id);
}

/*****************************************************************************/

int CFDB_AddNote(EnterpriseDB *conn, char *keyhash, int reportType, char *nid,
                 char *reportData, char *username, long datetime, char *msg)
{
    int options = MONGO_INDEX_UNIQUE | MONGO_INDEX_DROP_DUPS;

    /* for getting object id */
    bson_iterator it1;
    char objectId[CF_MAXVARSIZE] = { 0 };
    bson_oid_t bsonid;
    int found = false, newnote = false, retval = true;

    // Add index, TODO: must do this while creating the database

    bson b_key;

    bson_init(&b_key);
    bson_append_int(&b_key, cfn_keyhash, 1);
    bson_append_int(&b_key, cfn_reportdata, 1);
    BsonFinish(&b_key);

    mongo_create_index(conn, MONGO_NOTEBOOK, &b_key, options, NULL);

    bson_destroy(&b_key);

    // find right host
    bson host_key;

    bson_init(&host_key);

    if (!NULL_OR_EMPTY(nid))
    {
        bson_oid_from_string(&bsonid, nid);
        bson_append_oid(&host_key, "_id", &bsonid);
    }
    else
    {
        newnote = true;
        bson_append_new_oid(&host_key, "_id");
        bson_append_string(&host_key, cfn_keyhash, keyhash);

        if (!NULL_OR_EMPTY(reportData))
        {
            bson_append_string(&host_key, cfn_reportdata, reportData);
        }
        bson_append_int(&host_key, cfn_reporttype, reportType);
    }
    BsonFinish(&host_key);

    char msg_esc[CF_BUFSIZE] = { 0 };
    strncpy(msg_esc, msg, CF_BUFSIZE);
    EscapeChar(msg_esc, sizeof(msg_esc), '\\');
    EscapeChar(msg_esc, sizeof(msg_esc), '\"');

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$addToSet");
        {
            bson_append_start_object(&set_op, cfn_note);

            bson_append_string(&set_op, cfn_username, username);
            bson_append_string(&set_op, cfn_message, msg_esc);
            bson_append_int(&set_op, cfn_datetime, datetime);

            bson_append_finish_object(&set_op); // cfn_note
        }
        bson_append_finish_object(&set_op); // $addToSet
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_NOTEBOOK, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);

    MongoCheckForError(conn, "AddNote", keyhash, NULL);    

    if (newnote)
    {
        // get the objectid
        bson field;

        bson_init(&field);
        bson_append_int(&field, "_id", 1);
        BsonFinish(&field);

        mongo_cursor *cursor = MongoFind(conn, MONGO_NOTEBOOK, &host_key, &field, 0, 0, CF_MONGO_SLAVE_OK);

        bson_destroy(&field);

        MongoCheckForError(conn, "GetNoteID", keyhash, NULL);

        while (mongo_cursor_next(cursor) == MONGO_OK && !found)
        {
            bson_iterator_init(&it1, mongo_cursor_bson(cursor));
            objectId[0] = '\0';
            while (bson_iterator_next(&it1) == BSON_OID)
            {
                if (strcmp(bson_iterator_key(&it1), "_id") == 0)
                {
                    bson_oid_to_string(bson_iterator_oid(&it1), objectId);
                    found = true;
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

int CFDB_MarkAsDeleted(mongo *dbconn, const char *keyHash)
{    
    if (!IsDefinedClass("am_policy_hub") && !AM_PHP_MODULE)
    {
        CfOut(cf_verbose, "", "Ignoring caching of deleted hosts - not called by php module");
        return false;
    }

    Rlist *hostKeyList = SplitStringAsRList(keyHash, ',');

    for (Rlist *rp = hostKeyList; rp != NULL; rp = rp->next)
    {
        bson set_op;

        bson_init(&set_op);
        {
            bson_append_start_object(&set_op, "$addToSet");
            bson_append_string(&set_op, cfr_deleted_hosts, ScalarValue(rp));
            bson_append_finish_object(&set_op);
        }
        BsonFinish(&set_op);

        bson empty;

        MongoUpdate(dbconn, MONGO_SCRATCH, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL); // Note: Upsert Necessary?

        bson_destroy(&set_op);

        MongoCheckForError(dbconn, "MarkHostAsDeleted", ScalarValue(rp), NULL);
    }

    DeleteRlist(hostKeyList);

    return true;
}

/*****************************************************************************/

void CFDB_SaveExecutionStatus(EnterpriseDB *conn, char *keyhash, bool is_black)
{
    /* find right host */
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_bool(&set_op, cfr_is_black, is_black);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);
    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLastAgentExecution(EnterpriseDB *conn, char *keyhash, long last_agent_exec)
{
    /* find right host */
    bson host_key;  // host description

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_long(&set_op, cfr_last_execution, last_agent_exec);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveDeltaAgentExecution(EnterpriseDB *conn, char *keyhash, long delta_schedule)
{
    /* find right host */
    bson host_key;

    bson_init(&host_key);
    bson_append_string(&host_key, cfr_keyhash, keyhash);
    BsonFinish(&host_key);

    bson set_op;

    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_long(&set_op, cfr_schedule, delta_schedule);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    MongoUpdate(conn, MONGO_DATABASE, &host_key, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
    bson_destroy(&host_key);
}

/*****************************************************************************/

void CFDB_SaveLicense(EnterpriseDB *conn, time_t expires, time_t install_time, const char *owner, size_t num_granted)
{
    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        bson_append_start_object(&set_op, cfr_license);
        {
            bson_append_int(&set_op, cfr_license_expires, expires);
            bson_append_int(&set_op, cfr_license_install_time, install_time);
            bson_append_string(&set_op, cfr_license_owner, owner);
            bson_append_int(&set_op, cfr_license_granted, num_granted);
            bson_append_int(&set_op, cfr_day, time(NULL));
        }
        bson_append_finish_object(&set_op);
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    MongoUpdate(conn, MONGO_SCRATCH, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
}

/*****************************************************************************/

void CFDB_SaveLicenseNumberPromised(EnterpriseDB *conn, size_t num_promised)
{
    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");

        bson_append_long(&set_op, cfr_license_promised, num_promised);
        bson_append_int(&set_op, cfr_day, time(NULL));

        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    MongoUpdate(conn, MONGO_SCRATCH "." cfr_license, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
}

/*****************************************************************************/

void CFDB_SaveLicenseUsage(EnterpriseDB *conn, time_t last_measured, size_t num_samples, size_t min_observed_level,
                           size_t max_observed_level, double mean_usage, double mean_utilization_cumulative,
                           size_t num_used_today)
{
    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            bson_append_start_object(&set_op, cfr_license_usage);
            {
                bson_append_int(&set_op, cfr_license_usage_last_measured, last_measured);
                bson_append_int(&set_op, cfr_license_usage_samples, num_samples);
                bson_append_int(&set_op, cfr_license_usage_min_observed_level, min_observed_level);
                bson_append_int(&set_op, cfr_license_usage_max_observed_level, max_observed_level);
                bson_append_double(&set_op, cfr_license_usage_mean_usage, mean_usage);
                bson_append_double(&set_op, cfr_license_usage_mean_utilization_cumulative, mean_utilization_cumulative);
                bson_append_int(&set_op, cfr_license_usage_used_today, num_used_today);
                bson_append_int(&set_op, cfr_day, time(NULL));
            }
            bson_append_finish_object(&set_op);
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    MongoUpdate(conn, MONGO_SCRATCH, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&set_op);
}

/*****************************************************************************/

void CFDB_SaveScheduledReport(EnterpriseDB *conn, const char *user, const char *email,
                              const char *scheduled_query_id, const char *scheduled_query,
                              const char *schedule, const bool enabled, const int report_output_type )
{
    assert( conn );
    assert( user );
    assert( email );
    assert( scheduled_query_id );
    assert( scheduled_query );
    assert( schedule );
    assert( report_output_type && "Must have at least one report type enabled" );

    bson query[1];
    bson_init( query );
    bson_append_string( query, cfr_user_id, user );
    bson_append_string( query, cfr_query_id, scheduled_query_id );
    BsonFinish( query );

    bson set_op[1];
    bson_init( set_op );
    bson_append_string( set_op, cfr_user_id, user );
    bson_append_string( set_op, cfr_user_email, email );
    bson_append_string( set_op, cfr_query, scheduled_query );
    bson_append_string( set_op, cfr_query_id, scheduled_query_id );
    bson_append_string( set_op, cfr_run_classes, schedule );
    bson_append_int( set_op, cfr_last_run, ( int ) time( NULL ) );
    BsonAppendBool( set_op, cfr_already_run, false );
    BsonAppendBool( set_op, cfr_enabled, enabled );
    bson_append_int( set_op, cfr_report_output_type, report_output_type );
    BsonFinish( set_op );

    MongoUpdate( conn, MONGO_SCHEDULED_REPORTS, query, set_op, MONGO_UPDATE_UPSERT, NULL );

    bson_destroy( query );
    bson_destroy( set_op );

    MongoCheckForError( conn, "Save scheduled report", scheduled_query_id, NULL );
}

/*****************************************************************************/

void CFDB_RemoveScheduledReport(EnterpriseDB *conn, const char *user, const char *scheduled_query_id)
{
    /* Important: NULL value is  allowed for scheduled_query_id  */
    /* This will remove all data scheduled for that user (eg. when the user is deleted) */

    assert( conn );
    assert( user );

    bson query[1];
    bson_init( query );
    bson_append_string( query, cfr_user_id, user );
    BsonAppendStringSafe( query, cfr_query_id, scheduled_query_id);
    BsonFinish( query );

    MongoRemove( conn, MONGO_SCHEDULED_REPORTS, query, NULL );

    bson_destroy( query );

    MongoCheckForError( conn, "Remove scheduled report", scheduled_query_id, NULL );
}

cfapi_errid CFDB_SaveSettings(EnterpriseDB *conn, const HubSettings *settings)
{
    assert(conn);
    assert(settings);

    bson set_op;
    bson_init(&set_op);
    {
        bson_append_start_object(&set_op, "$set");
        {
            if (settings->rbac_enabled != TRINARY_UNDEFINED)
            {
                bson_append_bool(&set_op, "rbacEnabled", settings->rbac_enabled == TRINARY_TRUE);
            }

            if (settings->ldap.enabled != TRINARY_UNDEFINED)
            {
                bson_append_bool(&set_op, "ldapEnabled", settings->ldap.enabled == TRINARY_TRUE);
            }

            if (settings->ldap.mode)
            {
                bson_append_string(&set_op, "ldapMode", HubSettingsLDAPModeToString(settings->ldap.mode));
            }

            if (settings->ldap.username)
            {
                bson_append_string(&set_op, "ldapUsername", settings->ldap.username);
            }

            if (settings->ldap.password)
            {
                bson_append_string(&set_op, "ldapPassword", settings->ldap.password);
            }

            if (settings->ldap.encryption)
            {
                bson_append_string(&set_op, "ldapEncryption", settings->ldap.encryption);
            }

            if (settings->ldap.base_dn)
            {
                bson_append_string(&set_op, "ldapBaseDN", settings->ldap.base_dn);
            }

            if (settings->ldap.users_directory)
            {
                bson_append_string(&set_op, "ldapUsersDirectory", settings->ldap.users_directory);
            }

            if (settings->ldap.host)
            {
                bson_append_string(&set_op, "ldapHost", settings->ldap.host);
            }

            if (settings->ldap.port > 0)
            {
                bson_append_int(&set_op, "ldapPort", settings->ldap.port);
            }

            if (settings->ldap.port_ssl > 0)
            {
                bson_append_int(&set_op, "ldapPortSSL", settings->ldap.port_ssl);
            }

            if (settings->ldap.ad_domain)
            {
                bson_append_string(&set_op, "ldapActiveDirectoryDomain", settings->ldap.ad_domain);
            }

            if (settings->bluehost_horizon > 0)
            {
                bson_append_int(&set_op, "blueHostHorizon", settings->bluehost_horizon);
            }
        }
        bson_append_finish_object(&set_op);
    }
    BsonFinish(&set_op);

    bson empty;
    if (MongoUpdate(conn, MONGO_SETTINGS, bson_empty(&empty), &set_op, MONGO_UPDATE_UPSERT, NULL) != MONGO_OK)
    {
        bson_destroy(&set_op);
        return ERRID_DB_OPERATION;
    }

    bson_destroy(&set_op);
    return ERRID_SUCCESS;
}
