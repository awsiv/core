/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "reporting-engine.h"
#include "db_common.h"
#include "db_query.h"

#if defined(HAVE_LIBSQLITE3)
#include "sqlite3.h"
#endif

#if defined(HAVE_LIBSQLITE3)
/******************************************************************/
JsonElement *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op);

/* Conversion functions */
void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db);
void EnterpriseDBToSqlite3_PromiseLog_nk(sqlite3 *db);
void EnterpriseDBToSqlite3_Contexts(sqlite3 *db);
void EnterpriseDBToSqlite3_Variables(sqlite3 *db);
void EnterpriseDBToSqlite3_Software(sqlite3 *db);
#endif
/******************************************************************/

JsonElement *ReportingEngineQuery(JsonElement *query)
{
    JsonElement *result = JsonObjectCreate(5);

    JsonObjectAppendString(result, "hello", "world");

    return result;
}

/******************************************************************/

JsonElement *EnterpriseExecuteSQL(char *select_op)
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

    /* Query MongoDB and dump the result into Sqlite */

    EnterpriseDBToSqlite3_Contexts(db);
    EnterpriseDBToSqlite3_Variables(db);
    EnterpriseDBToSqlite3_FileChanges(db);
    EnterpriseDBToSqlite3_Software(db);

    /* Now query the in-memory database */
    JsonElement *out = EnterpriseQueryPublicDataModel(db, select_op);

    sqlite3_close(db);

    return out;
#else
    return NULL; /* TODO: Return error - not configured with sqlite3 */
#endif
}

/******************************************************************/
#if defined(HAVE_LIBSQLITE3)
/******************************************************************/
/* return Json result instead of printing into stdout             */
/* Callback function for iterating through the resultset          */
/* from sqlite                                                    */
/******************************************************************/
/* TODO: the returned JSON needs to have a better structure */
static int BuildOutput(void *out, int argc, char **argv, char **azColName)
{
    JsonElement *result = (JsonElement *) out;

    JsonElement *row = JsonObjectCreate(10);

    for(int i=0; i<argc; i++)
    {
        JsonObjectAppendString(row, azColName[i], argv[i] ? argv[i] : "NULL");
    }

    JsonArrayAppendObject(result, row);

    return 0;
}

/******************************************************************/

JsonElement *EnterpriseQueryPublicDataModel(sqlite3 *db, char *select_op)
{
    /* Query sqlite and print table contents */
    char *err = 0;

    JsonElement *result = JsonArrayCreate(5);

    int rc = sqlite3_exec(db, select_op, BuildOutput, (void *)result, &err);

    if( rc != SQLITE_OK )
    {
        CfOut(cf_error, "", "SQL error: %s\n", err);
        sqlite3_free(err);
        return NULL; /* TODO: Empty object ? */
    }

    return result;
}

/******************************************************************/

void EnterpriseDBToSqlite3_Contexts(sqlite3 *db)
{
    EnterpriseDB dbconn;

    CFDB_Open(&dbconn);

    /* TODO: Move host class filtering to the cfmod implementation */

    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);

    HubQuery *hq = CFDB_QueryClasses(&dbconn, NULL, NULL, false, 0, time(NULL),
                                     filter, false);

    CFDB_Close(&dbconn);

    /* Table schema in sqlite */
    /* TODO: define global constants for comumn sizes*/

    char table_schema[CF_BUFSIZE] = {0};

    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE contexts("
             "hostkey VARCHAR(100) PRIMARY KEY, "
             "name VARCHAR(50));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    /* Iterate through the HubQuery and dump data into in-memory sqlite tables */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO contexts VALUES('%s','%s');",
                 hc->hh->keyhash, hc->class);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        /* TODO: Error handling*/
    }

    DeleteHubQuery(hq, DeleteHubClass);
}

/******************************************************************/

void EnterpriseDBToSqlite3_Variables(sqlite3 *db)
{
    EnterpriseDB dbconn;

    CFDB_Open(&dbconn);
    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);

    HubQuery *hq = CFDB_QueryVariables(&dbconn, NULL, NULL, NULL, NULL, NULL,
                                       false, 0, time(NULL), filter);
    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE variables("
             "hostkey VARCHAR(100), "
             "scope VARCHAR(50), "
             "name VARCHAR(50), "
             "value VARCHAR(100), "
             "type VARCHAR(20), "
             "FOREIGN key(hostkey) REFERENCES contexts(hostkey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

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
                 "INSERT INTO variables VALUES('%s','%s','%s','%s','%s');",
                 hc->hh->hostname, hc->scope, hc->lval, rval_scalar, hc->dtype);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        /* TODO: Error Handling */
    }

    DeleteHubQuery(hq, DeleteHubVariable);
}

/******************************************************************/

void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db)
{
    EnterpriseDB dbconn;

    CFDB_Open(&dbconn);

    /* TODO: Move hostclass filtering into cfmod */
    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);

    HubQuery *hq = CFDB_QueryFileChanges(&dbconn, NULL, NULL, false, 0, time(NULL), false, filter);

    CFDB_Close(&dbconn);

    /* Table schema in sqlite */
    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE filechanges("
             "hostkey VARCHAR(100), "
             "filename VARCHAR(400), "
             "changetime BIGINT, "
             "FOREIGN key(hostkey) REFERENCES contexts(hostkey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubFileChanges *hC = (HubFileChanges *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO filechanges VALUES('%s','%s',%ld);",
                 hC->hh->keyhash, hC->path, hC->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        /* TODO: Error handling */
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
}

/******************************************************************/

void EnterpriseDBToSqlite3_Software(sqlite3 *db)
{
    EnterpriseDB dbconn;

    CFDB_Open(&dbconn);

    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_software, NULL,NULL,NULL, false, filter, false);

    CFDB_Close(&dbconn);

    /* Table schema */

    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE software("
             "hostkey VARCHAR(100), "
             "name VARCHAR(50), "
             "version VARCHAR(50), "
             "architecture VARCHAR(20), "
             "FOREIGN key(hostkey) REFERENCES contexts(hostkey));");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    /* Dump HubQuery into in-memory sqlite table */
    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO software VALUES('%s','%s','%s','%s');",
                 hs->hh->hostname, hs->name, hs->version, hs->arch);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        /* TODO: Error Handling */
    }

    DeleteHubQuery(hq, DeleteHubSoftware);
}

/******************************************************************/
/* CAUTION: THIS IS EXPERIMENTAL IMPLEMENTATION ONLY */

void EnterpriseDBToSqlite3_PromiseLog_nk(sqlite3 *db)
{
    EnterpriseDB dbconn;

    CFDB_Open(&dbconn);

    /* TODO: move hostclass filtering into cfmod */
    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);

    /* Table schema in sqlite */
    char table_schema[CF_BUFSIZE] = {0};
    snprintf(table_schema, sizeof(table_schema),
             "CREATE TABLE promisenk("
             "hostkey VARCHAR(100), "
             "handle VARCHAR(100), "
             "reason VARCHAR(500), "
             "time BIGINT);");

    char *err = 0;
    int rc = sqlite3_exec(db, table_schema, BuildOutput, 0, &err);

    /* TODO: return error */

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, NULL, PROMISE_LOG_STATE_REPAIRED/*PROMISE_LOG_STATE_NOTKEPT*/,
                                        NULL, false, NULL, 0, time(NULL), false, filter, NULL);
    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseLog *hp = (HubPromiseLog *) rp->item;

        char insert_op[CF_BUFSIZE] = {0};

        snprintf(insert_op, sizeof(insert_op),
                 "INSERT INTO promisenk VALUES('%s','%s', '%s',%ld);",
                 hp->hh->hostname, hp->handle, hp->cause, hp->t);

        rc = sqlite3_exec(db, insert_op, BuildOutput, 0, &err);

        /* TODO: Error Handling */
    }

    DeleteHubQuery(hq, DeleteHubPromiseLog);
}

/******************************************************************/

#endif
