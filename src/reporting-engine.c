/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "reporting-engine.h"
#include "db_common.h"
#include "db_query.h"
#include "web_rbac.h"
#include "install.h"

#if defined(HAVE_LIBSQLITE3)
#include "sqlite3.h"
#endif

#if defined(HAVE_LIBSQLITE3)
/******************************************************************/

static bool Sqlite3_DBOpen(sqlite3 **db);
static void Sqlite3_DBClose(sqlite3 *db);
static bool Sqlite3_BeginTransaction(sqlite3 *db);
static bool Sqlite3_CommitTransaction(sqlite3 *db);
static bool Sqlite3_Execute(sqlite3 *db, const char *sql, void *fn_ptr, void *arg_to_callback, char *err_msg);
static JsonHeaderTable *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op);
static JsonElement *GetColumnNames(sqlite3 *db, char *select_op);


/* Conversion functions */
static void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter);

/******************************************************************/
static bool Sqlite3_DBOpen(sqlite3 **db)
{
    int rc = sqlite3_open(":memory:", db);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "","Can't open temporary database: %s\n", sqlite3_errmsg(*db));
        return false;
    }

    return true;
}

/******************************************************************/

static void Sqlite3_DBClose(sqlite3 *db)
{
    sqlite3_close(db);
}

/******************************************************************/

static bool Sqlite3_Execute(sqlite3 *db, const char *sql, void *fn_ptr, void *arg_to_callback, char *err_msg)
{
    int rc = sqlite3_exec(db, sql, fn_ptr, arg_to_callback, &err_msg);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err_msg);
        //sqlite3_free(err_msg);
        return false;
    }

    return true;
}

/******************************************************************/
#endif

/******************************************************************/

JsonHeaderTable *EnterpriseExecuteSQL(const char *username, const char *select_op,
                                  Rlist *context_include, Rlist *context_exclude)
{
#if defined(HAVE_LIBSQLITE3)
    sqlite3 *db;

    if (!Sqlite3_DBOpen(&db))
    {
        /* TODO: better error handling */
        Sqlite3_DBClose(db);
        return NewJsonHeaderTable(select_op, JsonArrayCreate(0), JsonArrayCreate(0));
    }

    /* Apply RBAC & Context filters */

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC((char*)username);
    HostClassFilter *context_filter =  (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddClassLists(context_filter, context_include, context_exclude);


    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC((char*)username);
    PromiseFilter *promise_filter = HubQueryGetFirstRecord(hqPromiseFilter);

    /* Query MongoDB and dump the result into Sqlite */
    EnterpriseDBToSqlite3_Hosts(db, context_filter);
    EnterpriseDBToSqlite3_Contexts(db, context_filter);
    EnterpriseDBToSqlite3_Variables(db, context_filter);
    EnterpriseDBToSqlite3_FileChanges(db, context_filter);
    EnterpriseDBToSqlite3_Software(db, context_filter);
    EnterpriseDBToSqlite3_PromiseStatusLast(db, context_filter);
    EnterpriseDBToSqlite3_PromiseDefinitions(db, promise_filter);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    /* Now query the in-memory database */
    JsonHeaderTable *out = EnterpriseQueryPublicDataModel(db, select_op);

    sqlite3_close(db);
    Sqlite3_DBClose(db);
    return out;
#else
    return NULL; /* TODO: Return error - not configured with sqlite3 */
#endif
}

/******************************************************************/

#if defined(HAVE_LIBSQLITE3)
/******************************************************************/
/* Callback function for iterating through the resultset          */
/* from sqlite                                                    */
/******************************************************************/

static int BuildOutput(void *out, int argc, char **argv, char **azColName)
{
    JsonElement *result = (JsonElement *) out;

    JsonElement *row = JsonArrayCreate(10);

    for(int i=0; i<argc; i++)
    {
        JsonArrayAppendString(row, argv[i] ? argv[i] : "NULL");
    }

    JsonArrayAppendArray(result, row);

    return 0;
}

/******************************************************************/

static JsonHeaderTable *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op)
{
    /* Query sqlite and print table contents */
    char *err_msg = 0;

    JsonHeaderTable *result = NewJsonHeaderTable(select_op, GetColumnNames(db, select_op), JsonArrayCreate(5));

    if (!Sqlite3_Execute(db, select_op, (void *) BuildOutput, (void *)result->rows, err_msg))
    {
        Sqlite3_FreeString(err_msg);
        return NewJsonHeaderTable(select_op, JsonArrayCreate(0), JsonArrayCreate(0));;
    }

    return result;
}

static JsonElement *GetColumnNames(sqlite3 *db, char *select_op)
{
    sqlite3_stmt *statement;
    JsonElement *columns = JsonArrayCreate(5);

    int rc = sqlite3_prepare_v2(db, select_op, -1, &statement, 0);

    if (rc == SQLITE_OK)
    {
        int column_count = sqlite3_column_count(statement);

        if (column_count > 0)
        {
            for (int i = 0; i < column_count; i++)
            {
                JsonArrayAppendString(columns, sqlite3_column_name(statement, i));
            }
        }

        sqlite3_finalize(statement);
    }

    return columns;
}

/******************************************************************/

void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter)

static void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryColour(&dbconn, HOST_RANK_METHOD_COMPLIANCE, filter);

    CFDB_Close(&dbconn);

    char *err = 0;
    for (Rlist *rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        HubHost *hh = (HubHost *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Hosts VALUES('%s','%s','%s','%ld','%s');",
                 SkipHashType(hh->keyhash), hh->hostname, hh->ipaddr, hh->last_report, Nova_HostColourToString(hh->colour));

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            break;
        }
    }

    DeleteHubQuery(hq, NULL);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter)
{    
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryClasses(&dbconn, NULL, NULL, false, 0, time(NULL),
                                     filter, false);

    CFDB_Close(&dbconn);

    char *err = 0;
    /* Iterate through the HubQuery and dump data into in-memory sqlite tables */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Contexts VALUES('%s','%s','%ld');",
                 SkipHashType(hc->hh->keyhash), hc->class, hc->t);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err); // add the error message to the return string
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubClass);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, NULL, NULL, NULL, NULL, NULL,
                                       false, 0, time(NULL), filter);
    CFDB_Close(&dbconn);

    char *err = 0;
    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubVariable *hc = (HubVariable *) rp->item;

        char rval_scalar[CF_MAXVARSIZE] = {0};

        if(hc->rval.rtype == CF_SCALAR)
        {
            strcpy(rval_scalar, (char *) hc->rval.item);
        }
        else
        {
            /* TODO: Split lists into separate rows */
            continue;
        }

        char insert_op[CF_BUFSIZE] = {0};

        char *rval_scalar_escaped = EscapeCharCopy(rval_scalar, '\'', '\'');

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Variables VALUES('%s','%s','%s','%s','%s');",
                 SkipHashType(hc->hh->keyhash), hc->scope, hc->lval, rval_scalar_escaped, hc->dtype);

        free(rval_scalar_escaped);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubVariable);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, NULL, NULL, false, 0, time(NULL), false, filter);

    CFDB_Close(&dbconn);

    char *err = 0;
    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubFileChanges *hC = (HubFileChanges *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO FileChanges VALUES('%s','%s',%ld);",
                 SkipHashType(hC->hh->keyhash), hC->path, hC->t);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_software, NULL,NULL,NULL, false, filter, false);

    CFDB_Close(&dbconn);

    char *err = 0;
    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Software VALUES('%s','%s','%s','%s');",
                 SkipHashType(hs->hh->keyhash), hs->name, hs->version, hs->arch);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubSoftware);
}

/******************************************************************/
/*TODO: Conversion functions and definitions are all over the place*/
/* Needs unification, this is taken from public-api.c             */
/******************************************************************/
static const char *LABEL_STATE_REPAIRED = "repaired";
static const char *LABEL_STATE_NOTKEPT = "notkept";
static const char *LABEL_STATE_KEPT = "kept";
static const char *LABEL_STATE_UNKNOWN = "unknown";

static const char *PromiseStateToString(PromiseState state)
{
    switch (state)
    {
    case PROMISE_STATE_REPAIRED:
        return LABEL_STATE_REPAIRED;
    case PROMISE_STATE_NOTKEPT:
        return LABEL_STATE_NOTKEPT;
    case PROMISE_STATE_KEPT:
        return LABEL_STATE_KEPT;
    default:
        return LABEL_STATE_UNKNOWN;
    }
}

/******************************************************************/

static void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseCompliance(&dbconn, NULL, NULL, PROMISE_STATE_ANY, false, 0, time(NULL), false, filter);

    CFDB_Close(&dbconn);

    char *err = 0;
    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *hc = (HubPromiseCompliance *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO PromiseStatusLast VALUES('%s','%s','%s',%ld);",
                 SkipHashType(hc->hh->keyhash), hc->handle, PromiseStateToString(hc->status), hc->t);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromises(&dbconn, filter);

    CFDB_Close(&dbconn);

    char *err = 0;
    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromise *hp = (HubPromise *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        char *promiser_escaped = EscapeCharCopy(hp->promiser, '\'', '\'');
        char *promisee_escaped = EscapeCharCopy(hp->promisee, '\'', '\'');

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO PromiseDefinitions VALUES('%s','%s','%s','%s');",
                 hp->handle, promiser_escaped, hp->bundleName, promisee_escaped);

        free(promiser_escaped);
        free(promisee_escaped);

        if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
        {
            Sqlite3_FreeString(err);
            return;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromise);
}

/******************************************************************/
/* CAUTION: THIS IS EXPERIMENTAL IMPLEMENTATION ONLY */

void EnterpriseDBToSqlite3_PromiseLog_nk(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }


    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, NULL, PROMISE_LOG_STATE_REPAIRED/*PROMISE_LOG_STATE_NOTKEPT*/,
                                        NULL, false, NULL, 0, time(NULL), false, filter, NULL);
    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseLog *hp = (HubPromiseLog *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO promisenk VALUES('%s','%s', '%s',%ld);",
                 SkipHashType(hp->hh->keyhash), hp->handle, hp->cause, hp->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
/******************************************************************/

static bool Sqlite3_BeginTransaction(sqlite3 *db)
{
    char *err = 0;
    if (!Sqlite3_Execute(db, "BEGIN TRANSACTION;", (void *) BuildOutput, 0, err))
    {
        /* TODO: return error string */

        Sqlite3_FreeString(err);
        return false;
    }

    return true;
}

/******************************************************************/

static bool Sqlite3_CommitTransaction(sqlite3 *db)
{
    char *err = 0;
    if (!Sqlite3_Execute(db, "COMMIT;", (void *) BuildOutput, 0, err))
    {
        /* TODO: return error string */

        Sqlite3_FreeString(err);
        return false;
    }

    return true;
}
/******************************************************************/
#endif
