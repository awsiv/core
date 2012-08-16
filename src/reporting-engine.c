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
static JsonHeaderTable *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op);
static JsonElement *GetColumnNames(sqlite3 *db, char *select_op);


/* Conversion functions */
void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_PromiseLog_nk(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter);
void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter);

char *SqliteEscapeSingleQuote(char *str, int strSz);


#endif

/******************************************************************/

JsonHeaderTable *EnterpriseExecuteSQL(const char *username, const char *select_op,
                                  Rlist *context_include, Rlist *context_exclude)
{
#if defined(HAVE_LIBSQLITE3)
    sqlite3 *db;

    /* Open an in-memory database */
    int rc = sqlite3_open(":memory:", &db);
    if( rc )
    {
        CfOut(cf_error, "","Can't open temporary database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL; /* TODO: return empty object? */
    }

    /* Apply RBAC & Context filters */
    HostClassFilter *context_filter = NULL;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC((char*)username);
    context_filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddClassLists(context_filter, context_include, context_exclude);

    PromiseFilter *promise_filter = NULL;
    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC((char*)username);
    promise_filter =  HubQueryGetFirstRecord(hqPromiseFilter);

    /* Query MongoDB and dump the result into Sqlite */
    EnterpriseDBToSqlite3_Hosts(db, context_filter);
    EnterpriseDBToSqlite3_Contexts(db, context_filter);
    EnterpriseDBToSqlite3_Variables(db, context_filter);
    EnterpriseDBToSqlite3_FileChanges(db, context_filter);
    EnterpriseDBToSqlite3_Software(db, context_filter);
    EnterpriseDBToSqlite3_PromiseStatusLast(db, context_filter);
    EnterpriseDBToSqlite3_PromiseDefinitions(db, promise_filter);

    DeleteHostClassFilter(context_filter);
    DeletePromiseFilter(promise_filter);

    /* Now query the in-memory database */
    JsonHeaderTable *out = EnterpriseQueryPublicDataModel(db, (char *)select_op);

    sqlite3_close(db);

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
    char *err = 0;

    JsonHeaderTable *result = NewJsonHeaderTable(select_op, JsonArrayCreate(5), JsonArrayCreate(5));

    int rc = sqlite3_exec(db, select_op, BuildOutput, (void *)result->rows, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return NULL; /* TODO: Empty object ? */
    }

    result->header = GetColumnNames(db, select_op);

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
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryColour(&dbconn, HOST_RANK_METHOD_COMPLIANCE, filter);

    CFDB_Close(&dbconn);

    /* Table schema in sqlite */
    /* TODO: define global constants for comumn sizes*/

    char table_schema[CF_BUFSIZE] = {0};

    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE Hosts("
             "HostKey VARCHAR(100) PRIMARY KEY, "
             "HostName VARCHAR(100), "
             "HostIPAddress VARCHAR(50), "
             "HostLastReport BIGINT, "
             "HostColour VARCHAR(20));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Iterate through the HubQuery and dump data into in-memory sqlite tables */
    for (Rlist *rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        HubHost *hh = (HubHost *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Hosts VALUES('%s','%s','%s','%ld','%s');",
                 SkipHashType(hh->keyhash), hh->hostname, hh->ipaddr, hh->last_report, Nova_HostColourToString(hh->colour));

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    DeleteHubQuery(hq, NULL);
}

/******************************************************************/

void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryClasses(&dbconn, NULL, NULL, false, 0, time(NULL),
                                     filter, false);

    CFDB_Close(&dbconn);

    /* Table schema in sqlite */
    /* TODO: define global constants for comumn sizes*/

    char table_schema[CF_BUFSIZE] = {0};

    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE Contexts("
             "HostKey VARCHAR(100), "
             "ContextName VARCHAR(50), "
             "ContextLastDefined BIGINT, "
             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Iterate through the HubQuery and dump data into in-memory sqlite tables */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Contexts VALUES('%s','%s','%ld');",
                 SkipHashType(hc->hh->keyhash), hc->class, hc->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    DeleteHubQuery(hq, DeleteHubClass);
}

/******************************************************************/

void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, NULL, NULL, NULL, NULL, NULL,
                                       false, 0, time(NULL), filter);
    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE Variables("
             "HostKey VARCHAR(100), "
             "VariableScope VARCHAR(50), "
             "VariableName VARCHAR(50), "
             "VariableValue VARCHAR(100), "
             "VariableType VARCHAR(20), "
             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

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
        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Variables VALUES('%s','%s','%s','%s','%s');",
                 SkipHashType(hc->hh->keyhash), hc->scope, hc->lval, SqliteEscapeSingleQuote(rval_scalar, strlen(rval_scalar)), hc->dtype);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    DeleteHubQuery(hq, DeleteHubVariable);
}

/******************************************************************/

void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, NULL, NULL, false, 0, time(NULL), false, filter);

    CFDB_Close(&dbconn);

    /* Table schema in sqlite */
    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE FileChanges("
             "HostKey VARCHAR(100), "
             "FileName VARCHAR(400), "
             "FileChangeTime BIGINT, "
             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubFileChanges *hC = (HubFileChanges *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO FileChanges VALUES('%s','%s',%ld);",
                 SkipHashType(hC->hh->keyhash), hC->path, hC->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
}

/******************************************************************/

void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_software, NULL,NULL,NULL, false, filter, false);

    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE Software("
             "HostKey VARCHAR(100), "
             "SoftwareName VARCHAR(50), "
             "SoftwareVersion VARCHAR(50), "
             "SoftwareArchitecture VARCHAR(20), "
             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO Software VALUES('%s','%s','%s','%s');",
                 SkipHashType(hs->hh->keyhash), hs->name, hs->version, hs->arch);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
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

void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseCompliance(&dbconn, NULL, NULL, PROMISE_STATE_ANY, false, 0, time(NULL), false, filter);

    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE PromiseStatusLast("
             "HostKey VARCHAR(100), "
             "PromiseHandle VARCHAR(50), "
             "PromiseStatus VARCHAR(10), "
             "PromiseLastRun BIGINT, "
             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey), "
             "FOREIGN KEY(PromiseHandle) REFERENCES PromiseDefinitions(PromiseHandle));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *hc = (HubPromiseCompliance *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO PromiseStatusLast VALUES('%s','%s','%s',%ld);",
                 SkipHashType(hc->hh->keyhash), hc->handle, PromiseStateToString(hc->status), hc->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);
}

/******************************************************************/

void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromises(&dbconn, filter);

    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE PromiseDefinitions("
             "PromiseHandle VARCHAR(50), "
             "Promiser VARCHAR(50), "
             "Bundle VARCHAR(50), "
             "Promisees VARCHAR(100));");


    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
    }

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromise *hp = (HubPromise *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO PromiseDefinitions VALUES('%s','%s','%s','%s');",
                 hp->handle, SqliteEscapeSingleQuote(hp->promiser, strlen(hp->promiser)), hp->bundleName, SqliteEscapeSingleQuote(hp->promisee, strlen(hp->promisee)));

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        if( rc != SQLITE_OK )
        {
            CfOut(cf_error, "", "SQL error: %s\n", err);
            sqlite3_free(err);
            return;
        }
    }

    rc = sqlite3_exec(db, "COMMIT;", BuildOutput, 0, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return;
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

    /* Table schema in sqlite */
    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE promisenk("
             "hostkey VARCHAR(100), "
             "handle VARCHAR(100), "
             "reason VARCHAR(500), "
             "time BIGINT, "
             "FOREIGN key(hostkey) REFERENCES hosts(hostkey));");

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
    }

    DeleteHubQuery(hq, DeleteHubPromiseLog);
}

/******************************************************************/

char *SqliteEscapeSingleQuote(char *str, int strSz)
/* Escapes characters esc in the string str of size strSz  */
{
    char strDup[CF_BUFSIZE];
    int strPos, strDupPos;

    if (sizeof(strDup) < strSz)
    {
        FatalError("Too large string passed to SqliteEscapeSingleQuote()\n");
    }

    snprintf(strDup, sizeof(strDup), "%s", str);
    memset(str, 0, strSz);

    for (strPos = 0, strDupPos = 0; strPos < strSz - 2; strPos++, strDupPos++)
    {
        if (strDup[strDupPos] == '\'')
        {
            str[strPos] = '\'';
            strPos++;
        }

        str[strPos] = strDup[strDupPos];
    }

    return str;
}

#endif
