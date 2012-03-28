#include "cf.nova.h"

#include "db_common.h"
#include "bson_lib.h"

static const char *MongoHostname()
{
    const char *hostname = getenv("CFENGINE_TEST_OVERRIDE_MONGO_HOSTNAME");

    return hostname ? hostname : "127.0.0.1";
}

static int MongoPort()
{
    const char *port = getenv("CFENGINE_TEST_OVERRIDE_MONGO_PORT");

    if (port)
    {
        return (int) StringToLong(port);
    }
    else
    {
        return 27017;
    }
}

int CFDB_Open(mongo_connection *conn)
{
    int result = 0;

# ifdef MONGO_OLD_CONNECT
    mongo_connection_options connOpts;

    snprintf(connOpts.host, sizeof(connOpts.host), "%s", MongoHostname());
    connOpts.port = MongoPort();

    result = mongo_connect(conn, &connOpts);

# else

    result = mongo_connect(conn, MongoHostname(), MongoPort());

# endif

    if (result != 0)
    {
        mongo_destroy(conn);
        CfOut(cf_verbose, "mongo_connect", "!! Could not connect to mongo server (got %d)", result);
        return false;
    }

    return true;
}


int CFDB_Close(mongo_connection *conn)
{
    if (mongo_destroy(conn) != 0)
    {
        CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from mongo server");
        return false;
    }

    return true;
}


bool MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra, bool *checkUpdate)
/**
 * NOTE: This has performance penalties, and should not be widely used.
 *       It has the side-effect of guaranteeing that the previous operation finishes before returning.
 */
{
    char dbErr[CF_MAXVARSIZE];
    bson b;

    if (!extra)
    {
        extra = "";
    }

    bson_empty(&b);

    if (mongo_cmd_get_last_error(conn, MONGO_BASE, &b))
    {
        BsonToString(dbErr, sizeof(dbErr), b.data);
        CfOut(cf_error, "", "!! Database error on %s (%s): %s", operation, extra, dbErr);
        bson_destroy(&b);
        return false;
    }

    if (checkUpdate)
    {
        if (!BsonBoolGet(&b, "updatedExisting", checkUpdate))
        {
            CfOut(cf_error, "", "!! Unable to determine if update happened on %s (%s)", operation, extra);
            bson_destroy(&b);
            return false;
        }
    }

    bson_destroy(&b);

    return true;
}
