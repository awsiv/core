/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "reporting-engine.h"
#include "db_common.h"
#include "db_query.h"
#include "web_rbac.h"
#include "install.h"
#include "assert.h"
#include "conf.h"

#if defined(HAVE_LIBSQLITE3)
#include "sqlite3.h"

/******************************************************************/

static bool Sqlite3_DBOpen(sqlite3 **db);
static void Sqlite3_DBClose(sqlite3 *db);
static bool Sqlite3_BeginTransaction(sqlite3 *db);
static bool Sqlite3_CommitTransaction(sqlite3 *db);
static bool Sqlite3_Execute(sqlite3 *db, const char *sql, void *fn_ptr, void *arg_to_callback, char *err_msg);
static JsonHeaderTable *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op);
static JsonElement *GetColumnNames(sqlite3 *db, char *select_op);

/* Conversion functions */
static void LoadSqlite3Tables(sqlite3 *db, Rlist *tables, const char *username, Rlist *context_include, Rlist *context_exclude);
static void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter);
static bool EnterpriseDBToSqlite3_PromiseDefinitions_Insert(sqlite3 *db, char *handle, char *promiser, char *bundle_name, char *promisee);

/* Output generation */
static int BuildOutput(void *out, int argc, char **argv, char **azColName);

bool ValidateSQL (char *sql);
static bool CreateSQLTable(sqlite3 *db, char *create_sql);
static bool GenerateAllTables(sqlite3 *db);
static Rlist *GetTableNamesInQuery(const char *select_op);

void Sqlite3_FreeString(char *str);

/******************************************************************/

char *TABLES[SQL_TABLE_COUNT] =
{
    SQL_TABLE_HOSTS,
    SQL_TABLE_CONTEXTS,
    SQL_TABLE_FILECHANGES,
    SQL_TABLE_VARIABLES,
    SQL_TABLE_SOFTWARE,
    SQL_TABLE_PROMISESTATUS,
    SQL_TABLE_PROMISEDEFINITIONS,
    NULL
};

void *SQL_CONVERSION_HANDLERS[SQL_TABLE_COUNT] =
{
    EnterpriseDBToSqlite3_Hosts,
    EnterpriseDBToSqlite3_Contexts,
    EnterpriseDBToSqlite3_FileChanges,
    EnterpriseDBToSqlite3_Variables,
    EnterpriseDBToSqlite3_Software,
    EnterpriseDBToSqlite3_PromiseStatusLast,
    EnterpriseDBToSqlite3_PromiseDefinitions,
    NULL
};

char *SQL_CREATE_TABLE_STATEMENTS[SQL_TABLE_COUNT] =
{
    CREATE_SQL_HOSTS,
    CREATE_SQL_CONTEXTS,
    CREATE_SQL_FILECHANGES,
    CREATE_SQL_VARIABLES,
    CREATE_SQL_SOFTWARE,
    CREATE_SQL_PROMISESTATUS,
    CREATE_SQL_PROMISEDEFINITIONS,
    NULL
};

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

JsonHeaderTable *EnterpriseExecuteSQL(const char *username, char *select_op,
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

    if (!GenerateAllTables(db))
    {
        Sqlite3_DBClose(db);
        return NewJsonHeaderTable(select_op, JsonArrayCreate(0), JsonArrayCreate(0));
    }

    Rlist *tables = GetTableNamesInQuery(select_op);
    if(!tables)
    {
        Sqlite3_DBClose(db);
        return NewJsonHeaderTable(select_op, JsonArrayCreate(0), JsonArrayCreate(0));
    }

    LoadSqlite3Tables(db, tables, username, context_include, context_exclude);

    DeleteRlist(tables);

    JsonHeaderTable *out = EnterpriseQueryPublicDataModel(db, select_op);

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

static void LoadSqlite3Tables(sqlite3 *db, Rlist *tables, const char *username,
                              Rlist *context_include, Rlist *context_exclude)
{
    /* Apply RBAC & Context filters */
    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC((char*)username);
    HostClassFilter *context_filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddClassLists(context_filter, context_include, context_exclude);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC((char*)username);
    PromiseFilter *promise_filter =  HubQueryGetFirstRecord(hqPromiseFilter);

    for(Rlist *rp = tables; rp != NULL; rp = rp->next)
    {
        for (int i = 0; TABLES[i] != NULL; i++)
        {
            if(strcmp(rp->item, TABLES[i]) == 0)
            {
                assert(SQL_CONVERSION_HANDLERS[i]);

                void (*fnptr) () = SQL_CONVERSION_HANDLERS[i];

                if (!Sqlite3_BeginTransaction(db))
                {
                    return;
                }

                if(strcmp(rp->item, SQL_TABLE_PROMISEDEFINITIONS) == 0)
                {
                    (*fnptr) (db, promise_filter);
                }
                else
                {
                    (*fnptr) (db, context_filter);
                }

                if (!Sqlite3_CommitTransaction(db))
                {
                    return;
                }
            }
        }
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
}

/******************************************************************/

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
                 "INSERT INTO %s VALUES('%s','%s','%s', '%ld','%s');", SQL_TABLE_HOSTS,
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
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO %s VALUES('%s','%s', '%ld');", SQL_TABLE_CONTEXTS,
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
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubVariable *hc = (HubVariable *) rp->item;

        if(hc->rval.rtype == CF_SCALAR)
        {
            char *rval_scalar_escaped = EscapeCharCopy((char*) hc->rval.item, '\'', '\'');

            char insert_op[CF_BUFSIZE] = {0};

            snprintf(insert_op, sizeof(insert_op),
                     "INSERT INTO %s VALUES('%s','%s','%s','%s','%s');", SQL_TABLE_VARIABLES,
                     SkipHashType(hc->hh->keyhash), hc->scope, hc->lval, rval_scalar_escaped, hc->dtype);

            if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
            {
                Sqlite3_FreeString(err);
                return;
            }

            free(rval_scalar_escaped);
        }
        else
        {
            for (Rlist *rpv = (Rlist*)hc->rval.item; rpv != NULL; rpv = rpv->next)
            {
                char *rval_scalar_escaped = EscapeCharCopy((char *) rpv->item, '\'', '\'');

                char insert_op[CF_BUFSIZE] = {0};

                snprintf(insert_op, sizeof(insert_op),
                         "INSERT INTO %s VALUES('%s','%s','%s','%s','%s');", SQL_TABLE_VARIABLES,
                         SkipHashType(hc->hh->keyhash), hc->scope, hc->lval, rval_scalar_escaped, hc->dtype);

                if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
                {
                    Sqlite3_FreeString(err);
                    return;
                }

                free(rval_scalar_escaped);
            }
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
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubFileChanges *hC = (HubFileChanges *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO %s VALUES('%s','%s',%ld);", SQL_TABLE_FILECHANGES,
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
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO %s VALUES('%s','%s','%s','%s');", SQL_TABLE_SOFTWARE,
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
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *hc = (HubPromiseCompliance *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO %s VALUES('%s','%s','%s',%ld);", SQL_TABLE_PROMISESTATUS,
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

    HubQuery *hq = CFDB_QueryPromisesExpanded(&dbconn, filter);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromise *hp = (HubPromise *) rp->item;

        if(hp->promisees)
        {
            for(Rlist *promisees = hp->promisees; promisees != NULL; promisees = promisees->next)
            {
                if(!EnterpriseDBToSqlite3_PromiseDefinitions_Insert(db, hp->handle, hp->promiser, hp->bundleName, ScalarValue(promisees)))
                {
                    DeleteHubQuery(hq, DeleteHubPromise);
                    return;
                }
            }
        }
        else
        {
            if(!EnterpriseDBToSqlite3_PromiseDefinitions_Insert(db, hp->handle, hp->promiser, hp->bundleName, ""))
            {
                DeleteHubQuery(hq, DeleteHubPromise);
                return;
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubPromise);
}


static bool EnterpriseDBToSqlite3_PromiseDefinitions_Insert(sqlite3 *db, char *handle, char *promiser, char *bundle_name, char *promisee)
{
    char insert_op[CF_BUFSIZE] = {0};

    char *promiser_escaped = EscapeCharCopy(promiser, '\'', '\'');
    char *promisee_escaped = EscapeCharCopy(promisee, '\'', '\'');

    snprintf(insert_op, sizeof(insert_op),
             "INSERT INTO %s VALUES('%s','%s','%s','%s');", SQL_TABLE_PROMISEDEFINITIONS,
             handle, promiser_escaped, bundle_name, promisee_escaped);

    free(promisee_escaped);
    free(promiser_escaped);

    char *err = 0;

    if (!Sqlite3_Execute(db, insert_op, (void *) BuildOutput, 0, err))
    {
        Sqlite3_FreeString(err);
        return false;
    }

    return true;
}

/******************************************************************/

static Rlist *GetTableNamesInQuery(const char *select_op)
{
    Rlist *tables = NULL;

    char *select_low = SafeStringDuplicate(select_op);
    ToLowerStrInplace(select_low);

    for (int i = 0; TABLES[i] != NULL; i++)
    {
        char table_name[CF_BUFSIZE] = { 0 };
        strcpy(table_name, TABLES[i]);
        ToLowerStrInplace(table_name);

        if (StringMatch(table_name, select_low))
        {
            IdempPrependRScalar(&tables, TABLES[i], CF_SCALAR);
        }
    }

    free(select_low);

    return tables;
}

/******************************************************************/

// unused currently
bool ValidateSQL (char *sql)
{
    sqlite3 *db;

    if (!Sqlite3_DBOpen(&db))
    {
        return false;
    }

    if(!GenerateAllTables(db))
    {
        return false;
    }

    // TODO: Add error messaging
    JsonHeaderTable *out = EnterpriseQueryPublicDataModel(db, sql);

    Sqlite3_DBClose(db);

    return true;
}

/******************************************************************/

static bool CreateSQLTable(sqlite3 *db, char *create_sql)
{
    char *err = 0;
    if (!Sqlite3_Execute(db, create_sql, (void *) BuildOutput, 0, err))
    {
        //Sqlite3_FreeString(err);
        return false;
    }

    return true;
}

/******************************************************************/

static bool GenerateAllTables(sqlite3 *db)
{
    for (int i = 0; SQL_CREATE_TABLE_STATEMENTS[i] != NULL; i++)
    {
        assert(SQL_CREATE_TABLE_STATEMENTS[i]);
        assert(TABLES[i]);

        if (!CreateSQLTable(db, SQL_CREATE_TABLE_STATEMENTS[i]))
        {
            return false;
        }
    }

    return true;
}

/******************************************************************/

void Sqlite3_FreeString(char *str)
{
    if(str)
    {
        sqlite3_free(str);
    }
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
