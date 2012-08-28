#include "cf.nova.h"

#include "db_common.h"
#include "bson_lib.h"

#include <assert.h>

/********************************************************************/

const char *DBHostname()
{
    const char *hostname = getenv("CFENGINE_TEST_OVERRIDE_MONGO_HOSTNAME");

    return hostname ? hostname : "127.0.0.1";
}

/********************************************************************/

int DBPort()
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

/********************************************************************/

bool CFDB_Open(EnterpriseDB *conn)
{
    int result = 0;

# ifdef MONGO_OLD_CONNECT
    mongo_connection_options connOpts;

    snprintf(connOpts.host, sizeof(connOpts.host), "%s", DBHostname());
    connOpts.port = MongoPort();

    result = mongo_connect(conn, &connOpts);

# else

    result = mongo_connect(conn, DBHostname(), DBPort());

# endif

    if (result != MONGO_OK)
    {
        mongo_destroy(conn);
        CfOut(cf_verbose, "mongo_connect", "!! Could not connect to mongo server (got %d)", result);
        return false;
    }

    return true;
}

/********************************************************************/

bool CFDB_Close(EnterpriseDB *conn)
{
    mongo_destroy(conn);
    /*
    {
        CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from mongo server");
        return false;
    }
*/
    return true;
}

/********************************************************************/

bool MongoCheckForError(EnterpriseDB *conn, const char *operation, const char *extra, bool *checkUpdate)
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
        BsonToString(dbErr, sizeof(dbErr), &b);
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

/********************************************************************/

mongo_cursor *MongoFind( EnterpriseDB *conn, const char *ns, const bson *query,
                          const bson *fields, int limit, int skip, int options )
{
    assert( conn && ns);
    assert( query && query->finished);
    assert( fields && fields->finished );

    return mongo_find( conn, ns, query, fields, limit, skip, options );
}

/********************************************************************/

int MongoFindOne( EnterpriseDB *conn, const char *ns, const bson *query,
                    const bson *fields, bson *out )
{
    assert( conn && ns);
    assert( query && query->finished);
    assert( fields && fields->finished );

    assert( out && !out->finished );

    return mongo_find_one( conn, ns, query, fields, out );
}

/********************************************************************/

int MongoUpdate( EnterpriseDB *conn, const char *ns, const bson *cond,
    const bson *op, int flags, mongo_write_concern *custom_write_concern )
{
    assert( conn && ns );
    assert( cond && cond->finished );
    assert( op && op->finished );

    return mongo_update( conn, ns, cond, op, flags, custom_write_concern );
}

/********************************************************************/

int MongoInsert( EnterpriseDB *conn, const char *ns,
                 const bson *bson, mongo_write_concern *custom_write_concern )
{
    assert( conn && ns );
    assert( bson && bson->finished );

    return mongo_insert( conn, ns, bson, custom_write_concern );
}

/********************************************************************/
