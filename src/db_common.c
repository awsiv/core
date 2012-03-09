#include "cf.nova.h"

#if defined(HAVE_LIBMONGOC)
#include "db_common.h"
#endif

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

    int result;

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

/*****************************************************************************/

int CFDB_Close(mongo_connection *conn)
{
    if (mongo_destroy(conn) != 0)
    {
        CfOut(cf_error, "mongo_destroy", "!! Could not disconnect from mongo server");
        return false;
    }

    return true;
}
