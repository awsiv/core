/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "reporting_engine.h"

#include "db_common.h"
#include "db_query.h"
#include "web_rbac.h"
#include "install.h"
#include "conversion.h"
#include "assert.h"
#include "conf.h"
#include "log.h"
#include "cfstream.h"
#include "db_export_csv.h"
#include "csv_writer.h"
#include "files_interfaces.h"
#include "files_hashes.h"
#include "string_lib.h"
#include "db_diagnostics.h"
#include "hashes.h"
#include "buffer.h"

#define SQL_TABLE_COUNT 19

#define SQL_TABLE_HOSTS "Hosts"
#define SQL_TABLE_FILECHANGES "FileChanges"
#define SQL_TABLE_CONTEXTS "Contexts"
#define SQL_TABLE_VARIABLES "Variables"
#define SQL_TABLE_SOFTWARE "Software"
#define SQL_TABLE_PROMISESTATUS "PromiseStatusLast"
#define SQL_TABLE_PROMISEDEFINITIONS "PromiseDefinitions"
#define SQL_TABLE_PROMISELOG "PromiseLogs"
#define SQL_TABLE_PROMISE_SUMMARY "PromiseSummary"
#define SQL_TABLE_BUNDLESTATUS "BundleStatus"
#define SQL_TABLE_BENCHMARKS "Benchmarks"
#define SQL_TABLE_LASTSEEN "LastSeenHosts"
#define SQL_TABLE_TOTALCOMPLIANCE "PolicyStatus"
#define SQL_TABLE_PATCH "SoftwareUpdates"
#define SQL_TABLE_FILEDIFFS "FileDiffs"
#define SQL_TABLE_DIAGNOSTIC_SERVER_STATUS "DatabaseServerStatus"
#define SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS "DatabaseStatus"
#define SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS "DatabaseCollectionStatus"

#define CREATE_SQL_HOSTS "CREATE TABLE " SQL_TABLE_HOSTS "(" \
                         "HostKey VARCHAR(100) PRIMARY KEY, " \
                         "HostName VARCHAR(100), " \
                         "IPAddress VARCHAR(50), " \
                         "ReportTimeStamp BIGINT);"

#define CREATE_SQL_FILECHANGES "CREATE TABLE " SQL_TABLE_FILECHANGES "(" \
                               "HostKey VARCHAR(100), " \
                               "PromiseHandle VARCHAR(50), " \
                               "FileName VARCHAR(400), " \
                               "ChangeTimeStamp BIGINT, " \
                               "ChangeType VARCHAR(10), " \
                               "LineNumber INT, " \
                               "ChangeDetails VARCHAR(2047), " \
                               "FOREIGN key(hostkey) REFERENCES hosts(hostkey));"

#define CREATE_SQL_CONTEXTS "CREATE TABLE " SQL_TABLE_CONTEXTS "(" \
                            "HostKey VARCHAR(100), " \
                            "ContextName VARCHAR(50), " \
                            "DefineTimeStamp BIGINT, " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_VARIABLES "CREATE TABLE " SQL_TABLE_VARIABLES "(" \
                             "HostKey VARCHAR(100), " \
                             "NameSpace VARCHAR(50), " \
                             "Bundle VARCHAR(50), " \
                             "VariableName VARCHAR(50), " \
                             "VariableValue VARCHAR(100), " \
                             "VariableType VARCHAR(20), " \
                             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_SOFTWARE "CREATE TABLE " SQL_TABLE_SOFTWARE "(" \
                            "HostKey VARCHAR(100), " \
                            "SoftwareName VARCHAR(50), " \
                            "SoftwareVersion VARCHAR(50), " \
                            "SoftwareArchitecture VARCHAR(20), " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_PROMISESTATUS "CREATE TABLE " SQL_TABLE_PROMISESTATUS "(" \
                                 "HostKey VARCHAR(100), " \
                                 "PromiseHandle VARCHAR(50), " \
                                 "PromiseStatus VARCHAR(10), " \
                                 "CheckTimeStamp BIGINT, " \
                                 "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey), " \
                                 "FOREIGN KEY(PromiseHandle) REFERENCES PromiseDefinitions(PromiseHandle));"

#define CREATE_SQL_PROMISEDEFINITIONS "CREATE TABLE " SQL_TABLE_PROMISEDEFINITIONS "(" \
                                      "NameSpace VARCHAR(50), " \
                                      "PromiseHandle VARCHAR(50), " \
                                      "Promiser VARCHAR(50), " \
                                      "Bundle VARCHAR(50), " \
                                      "Promisee VARCHAR(100));"

#define CREATE_SQL_PROMISELOG "CREATE TABLE " SQL_TABLE_PROMISELOG "(" \
                                       "HostKey VARCHAR(100), " \
                                       "PromiseHandle VARCHAR(254), " \
                                       "PromiseStatus VARCHAR(10), " \
                                       "PromiseStatusReport VARCHAR(1024), " \
                                       "Time BIGINT, " \
                                       "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_PROMISE_SUMMARY "CREATE TABLE " SQL_TABLE_PROMISE_SUMMARY "(" \
                                       "PromiseHandle VARCHAR(254), " \
                                       "PromiseStatus VARCHAR(10), " \
                                       "PromiseStatusReport VARCHAR(1024), " \
                                       "Occurrences INT, " \
                                       "FOREIGN KEY(PromiseHandle) REFERENCES PromiseLog(PromiseHandle), " \
                                       "FOREIGN KEY(PromiseStatusReport) REFERENCES PromiseLog(PromiseStatusReport));"

#define CREATE_SQL_BUNDLESTATUS "CREATE TABLE " SQL_TABLE_BUNDLESTATUS "(" \
                                       "HostKey VARCHAR(100), " \
                                       "NameSpace VARCHAR(50), " \
                                       "Bundle VARCHAR(254), " \
                                       "PercentageCompliance REAL, " \
                                       "CheckTimeStamp BIGINT, " \
                                       "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_BENCHMARKS "CREATE TABLE " SQL_TABLE_BENCHMARKS "(" \
                                       "HostKey VARCHAR(100), " \
                                       "EventName VARCHAR(254), " \
                                       "TimeTaken REAL, " \
                                       "CheckTimeStamp BIGINT, " \
                                       "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_LASTSEEN "CREATE TABLE " SQL_TABLE_LASTSEEN "(" \
                                       "HostKey VARCHAR(100), " \
                                       "LastSeenDirection VARCHAR(10), " \
                                       "RemoteHostKey VARCHAR(100), " \
                                       "LastSeenAt BIGINT, " \
                                       "LastSeenInterval INT, " \
                                       "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_TOTALCOMPLIANCE "CREATE TABLE " SQL_TABLE_TOTALCOMPLIANCE "(" \
                                       "HostKey VARCHAR(100), " \
                                       "PolicyName VARCHAR(254), " \
                                       "TotalKept INT, " \
                                       "TotalRepaired INT, " \
                                       "TotalNotKept INT, " \
                                       "CheckTimeStamp BIGINT, " \
                                       "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_PATCH "CREATE TABLE " SQL_TABLE_PATCH "(" \
                            "HostKey VARCHAR(100), " \
                            "PatchReportType VARCHAR(8), " \
                            "PatchName VARCHAR(50), " \
                            "PatchVersion VARCHAR(50), " \
                            "PatchArchitecture VARCHAR(20), " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_FILEDIFFS "CREATE TABLE " SQL_TABLE_FILEDIFFS "(" \
                               "HostKey VARCHAR(100), " \
                               "PromiseHandle VARCHAR(50), " \
                               "FileName VARCHAR(400), " \
                               "ChangeTimeStamp BIGINT, " \
                               "ChangeType VARCHAR(10), " \
                               "LineNumber INT, " \
                               "ChangeDetails VARCHAR(2047), " \
                               "FOREIGN KEY(PromiseHandle) REFERENCES PromiseDefinitions(PromiseHandle), " \
                               "FOREIGN KEY(FileName) REFERENCES FileChanges(FileName), " \
                               "FOREIGN KEY(ChangeTimeStamp) REFERENCES FileChanges(ChangeTimeStamp), " \
                               "FOREIGN key(HostKey) REFERENCES hosts(HostKey));"

#define CREATE_SQL_DIAGNOSTIC_SERVER_STATUS "CREATE TABLE " SQL_TABLE_DIAGNOSTIC_SERVER_STATUS "(" \
                                              "SampleTime BIGINT, " \
                                              "Host VARCHAR(100), " \
                                              "Version VARCHAR(100), " \
                                              "Uptime REAL, " \
                                              "GlobalLockTotalTime REAL, " \
                                              "GlobalLockTime REAL, " \
                                              "GlobalLockQuereTotal INT, " \
                                              "GlobalLockQuereReaders INT, " \
                                              "GlobalLockQuereWriters INT, " \
                                              "MemoryResident INT, " \
                                              "MemoryVirtual INT, " \
                                              "MemoryMapped INT, " \
                                              "BackgroundFlushCount INT, " \
                                              "BackgroundFlushTotalTime INT, " \
                                              "BackgroundFlushAverageTime INT, " \
                                              "BackgroundFlushLastTime INT);"

#define CREATE_SQL_DIAGNOSTIC_DATABASE_STATUS "CREATE TABLE " SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS "(" \
                                                "SampleTime BIGINT, " \
                                                "Database VARCHAR(100), " \
                                                "AverageObjectSize REAL, " \
                                                "DataSize INT, " \
                                                "StorageSize INT, " \
                                                "FileSize INT, " \
                                                "FOREIGN KEY(SampleTime) REFERENCES ServiceStatus(SampleTime));"

#define CREATE_SQL_DIAGNOSTIC_COLLECTION_STATUS "CREATE TABLE " SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS "(" \
                                                "SampleTime BIGINT, " \
                                                "Database VARCHAR(100), " \
                                                "Collection VARCHAR(100), " \
                                                "ObjectCount INT, " \
                                                "DataSize INT, " \
                                                "AverageObjectSize REAL, " \
                                                "StorageSize INT, " \
                                                "IndexCount INT, " \
                                                "TotalIndexSize INT, " \
                                                "PaddingFactor REAL, " \
                                                "FOREIGN KEY(Database) REFERENCES DatabaseStatus(Database)," \
                                                "FOREIGN KEY(SampleTime) REFERENCES ServiceStatus(SampleTime));"

/******************************************************************/

static bool Sqlite3_BeginTransaction(sqlite3 *db);
static bool Sqlite3_CommitTransaction(sqlite3 *db);
static JsonElement *EnterpriseQueryPublicDataModel(sqlite3 *db, const char *select_op);
static JsonElement *GetColumnNamesJson(sqlite3 *db, const char *select_op);

/* Conversion functions */
static void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Variables(sqlite3 *db, HostClassFilter *filter);
static bool EnterpriseDBToSqlite3_Variables_Insert(sqlite3 *db, char *keyhash,
                                                   const char *ns, const char *bundle,
                                                   const char *lval, const char *rval,
                                                   char *dtype);
static void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseStatusLast(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PromiseDefinitions(sqlite3 *db, PromiseFilter *filter);
static bool EnterpriseDBToSqlite3_PromiseDefinitions_Insert(sqlite3 *db, const char *ns,
                                                            const char *handle,
                                                            const char *promiser,
                                                            const char *bundle_name,
                                                            const char *promisee);

static void EnterpriseDBToSqlite3_PromiseLog(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_NotKeptLog(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_RepairedLog(sqlite3 *db, HostClassFilter *filter);

static void EnterpriseDBToSqlite3_PromiseSummary(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_NotKeptLogsSummary(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_RepairedLogsSummary(sqlite3 *db, HostClassFilter *filter);

static void EnterpriseDBToSqlite3_BundleStatus(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Benchmarks(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_LastSeen(sqlite3 *db, HostClassFilter *filter);

static void EnterpriseDBToSqlite3_TotalCompliance(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_Patch(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PatchInstalled(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_PatchAvailable(sqlite3 *db, HostClassFilter *filter);
static void EnterpriseDBToSqlite3_FileDiffs(sqlite3 *db, HostClassFilter *filter);

static void EnterpriseDBToSqlite3_DiagnosticServiceStatus(sqlite3 *db);
static void EnterpriseDBToSqlite3_DiagnosticDatabaseStatus(sqlite3 *db);
static void EnterpriseDBToSqlite3_DiagnosticCollectionStatus(sqlite3 *db);

static bool CreateSQLTable(sqlite3 *db, char *create_sql);
bool GenerateAllTables(sqlite3 *db);

static void SetVirtualNameSpace(const char *key, const char *namespace,
                                char *buffer, size_t buffer_size);


bool EnterpriseQueryPrepare(sqlite3 *db, const char *username, const char *select_op_expanded);

/* Asynchronous query */
int ExportCSVOutput(void *out, int argc, char **argv, char **azColName);
void AsyncQueryExportResult(sqlite3 *db, const char *select_op, WebReportFileInfo *wr_info);

#define MD5_HEX_LENGTH 32
static void AsyncToken(const char *username, const char *query, char token_out[MD5_HEX_LENGTH + 1]);
static JsonElement *PackageAsyncQueryCreateResult(ReportingEngineAsyncError err_id, const char *query, const char *token);
static JsonElement *PackageAsyncQueryAbortResult(ReportingEngineAsyncError err_id, const char *token);
static JsonElement *PackageAsyncQueryStatusResult(ReportingEngineAsyncError err_id, const char *token, size_t percentage_complete, const char *href_static);

static bool IsExporterProcRunning(WebReportFileInfo *wr_info);
bool ReadExporterPid(WebReportFileInfo *wr_info);
int ReadExportStatus(WebReportFileInfo *wr_info);
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
    SQL_TABLE_PROMISELOG,
    SQL_TABLE_PROMISE_SUMMARY,
    SQL_TABLE_BUNDLESTATUS,
    SQL_TABLE_BENCHMARKS,
    SQL_TABLE_LASTSEEN,
    SQL_TABLE_TOTALCOMPLIANCE,
    SQL_TABLE_PATCH,
    SQL_TABLE_FILEDIFFS,
    SQL_TABLE_DIAGNOSTIC_SERVER_STATUS,
    SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS,
    SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS,
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
    EnterpriseDBToSqlite3_PromiseLog,
    EnterpriseDBToSqlite3_PromiseSummary,
    EnterpriseDBToSqlite3_BundleStatus,
    EnterpriseDBToSqlite3_Benchmarks,
    EnterpriseDBToSqlite3_LastSeen,
    EnterpriseDBToSqlite3_TotalCompliance,
    EnterpriseDBToSqlite3_Patch,
    EnterpriseDBToSqlite3_FileDiffs,
    EnterpriseDBToSqlite3_DiagnosticServiceStatus,
    EnterpriseDBToSqlite3_DiagnosticDatabaseStatus,
    EnterpriseDBToSqlite3_DiagnosticCollectionStatus,
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
    CREATE_SQL_PROMISELOG,
    CREATE_SQL_PROMISE_SUMMARY,
    CREATE_SQL_BUNDLESTATUS,
    CREATE_SQL_BENCHMARKS,
    CREATE_SQL_LASTSEEN,
    CREATE_SQL_TOTALCOMPLIANCE,
    CREATE_SQL_PATCH,
    CREATE_SQL_FILEDIFFS,
    CREATE_SQL_DIAGNOSTIC_SERVER_STATUS,
    CREATE_SQL_DIAGNOSTIC_DATABASE_STATUS,
    CREATE_SQL_DIAGNOSTIC_COLLECTION_STATUS,
    NULL
};

/******************************************************************/

static const char *LABEL_ADD = "add";
static const char *LABEL_REMOVE = "remove";
static const char *LABEL_UNKNOWN = "unknown";

/******************************************************************/

bool Sqlite3_DBOpen(sqlite3 **db)
{
    int rc = sqlite3_open(":memory:", db);

    if( rc != SQLITE_OK )
    {
        CfOut(OUTPUT_LEVEL_ERROR, "","Can't open temporary database: %s\n", sqlite3_errmsg(*db));
        return false;
    }

    return true;
}

/******************************************************************/

void Sqlite3_DBClose(sqlite3 *db)
{
    assert(db);
    sqlite3_close(db);
}

/******************************************************************/

bool Sqlite3_Execute(sqlite3 *db, const char *sql, void *BuildOutputCallback, void *arg_to_callback)
{
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, BuildOutputCallback, arg_to_callback, &err_msg);

    if( rc != SQLITE_OK )
    {        
        syslog(LOG_ERR, "Error executing query - message: %s, sql: \"%s\"",
               err_msg,
               sql);

        Sqlite3_FreeString(err_msg);
        return false;
    }

    return true;
}

/******************************************************************/

static JsonElement *PackageReportingEngineResult(const char *query,
                                                 JsonElement *columns,
                                                 JsonElement *rows)
{
    JsonElement *result = JsonObjectCreate(3);

    JsonObjectAppendString(result, "query", query);
    JsonObjectAppendArray(result, "header", columns);
    JsonObjectAppendInteger(result, "rowCount", JsonElementLength(rows));
    JsonObjectAppendArray(result, "rows", rows);

    return result;
}

/******************************************************************/
/* Callback function for iterating through the resultset          */
/* from sqlite                                                    */
/******************************************************************/

int BuildJsonOutput(void *out, int argc, char **argv, char **azColName)
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

static JsonElement *EnterpriseQueryPublicDataModel(sqlite3 *db, const char *select_op)
{
    /* Query sqlite and print table contents */

    JsonElement *rows = JsonArrayCreate(5);

    if (!Sqlite3_Execute(db, select_op, (void *) BuildJsonOutput, rows))
    {        
        JsonElementDestroy(rows);
        return PackageReportingEngineResult(select_op, JsonArrayCreate(0), JsonArrayCreate(0));
    }

    return PackageReportingEngineResult(select_op, GetColumnNamesJson(db, select_op), rows);
}

/******************************************************************/

static JsonElement *GetColumnNamesJson(sqlite3 *db, const char *select_op)
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

int WriteColumnNamesCsv(sqlite3 *db, const char *select_op, Writer *writer)
{
    assert(writer);

    sqlite3_stmt *statement;

    int rc = sqlite3_prepare_v2(db, select_op, -1, &statement, 0);

    if (rc != SQLITE_OK)
    {
        return -1;
    }

    int column_count = sqlite3_column_count(statement);

    if (column_count > 0)
    {
        CsvWriter *c = CsvWriterOpen(writer);

        for (int i = 0; i < column_count; i++)
        {
            CsvWriterField(c, sqlite3_column_name(statement, i));
        }

        CsvWriterClose(c);
    }

    sqlite3_finalize(statement);

    return column_count;
}

/******************************************************************/

cfapi_errid LoadSqlite3Tables(sqlite3 *db, Set *tables, const char *username)
{
    assert(tables);

    cfapi_errid errid;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC((char*)username);

    if(!RBACFilterSuccess(hqHostClassFilter->errid))
    {
        errid = hqHostClassFilter->errid;
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        return errid;
    }

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC((char*)username);

    if(!RBACFilterSuccess(hqPromiseFilter->errid))
    {
        errid = hqPromiseFilter->errid;
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
        return errid;
    }

    HostClassFilter *context_filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    PromiseFilter *promise_filter =  HubQueryGetFirstRecord(hqPromiseFilter);

    for (int i = 0; TABLES[i] != NULL; i++)
    {
        if (SetContains(tables, TABLES[i]))
        {
            assert(SQL_CONVERSION_HANDLERS[i]);

            void (*fnptr) () = SQL_CONVERSION_HANDLERS[i];


            if (!Sqlite3_BeginTransaction(db))
            {
                DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
                DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
                return ERRID_DB_OPERATION;
            }

            LogPerformanceTimer timer = LogPerformanceStart();
            if(strcmp(TABLES[i], SQL_TABLE_PROMISEDEFINITIONS) == 0)
            {
                (*fnptr) (db, promise_filter);
            }
            else if ((strcmp(TABLES[i], SQL_TABLE_DIAGNOSTIC_SERVER_STATUS) == 0) ||
                     (strcmp(TABLES[i], SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS) == 0) ||
                     (strcmp(TABLES[i], SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS) == 0))
            {
                (*fnptr) (db);

            }
            else
            {
                (*fnptr) (db, context_filter);
            }
            LogPerformanceStop(&timer, "Loaded table %s", TABLES[i]);

            if (!Sqlite3_CommitTransaction(db))
            {
                DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
                DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
                return ERRID_DB_OPERATION;
            }
        }
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    return ERRID_SUCCESS;
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Hosts(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryColour(&dbconn, HOST_RANK_METHOD_COMPLIANCE,
                                    filter, PROMISE_CONTEXT_MODE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        HubHost *hh = (HubHost *) rp->item;

        Buffer *insert_buf = BufferNew();

        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%ld');",
                     SQL_TABLE_HOSTS,
                     SkipHashType(hh->keyhash),
                     NULLStringToEmpty(hh->hostname),
                     NULLStringToEmpty(hh->ipaddr),
                     hh->last_report);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, free);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Contexts(sqlite3 *db, HostClassFilter *filter)
{    
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryClasses(&dbconn, NULL, NULL, 0, time(NULL),
                                     filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                     QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;

        Buffer *insert_buf = BufferNew();

        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s', '%ld');",
                     SQL_TABLE_CONTEXTS,
                     SkipHashType(hc->hh->keyhash),
                     NULLStringToEmpty(hc->class),
                     hc->t);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
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

    HubQuery *hq = CFDB_QueryVariables(&dbconn, NULL, NULL, NULL, NULL, NULL, NULL,
                                       0, time(NULL), filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                       QUERY_FLAG_DISABLE_ALL);
    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubVariable *hv = (HubVariable *) rp->item;

        if(hv->rval.type == RVAL_TYPE_SCALAR)
        {
            if (!EnterpriseDBToSqlite3_Variables_Insert(db, hv->hh->keyhash, hv->ns, hv->bundle,
                                                        hv->lval, hv->rval.item, hv->dtype))
            {
                DeleteHubQuery(hq, DeleteHubVariable);
                return;
            }
        }
        else
        {
            for (Rlist *rpv = (Rlist*)hv->rval.item; rpv != NULL; rpv = rpv->next)
            {
                if (!EnterpriseDBToSqlite3_Variables_Insert(db, hv->hh->keyhash, hv->ns,
                                                            hv->bundle, hv->lval, rpv->item,
                                                            hv->dtype))
                {
                    DeleteHubQuery(hq, DeleteHubVariable);
                    return;
                }
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubVariable);
}

static bool EnterpriseDBToSqlite3_Variables_Insert(sqlite3 *db, char *keyhash,
                                                   const char *ns, const char *bundle,
                                                   const char *lval, const char *rval,
                                                   char *dtype)
{
    char *rval_scalar_escaped = EscapeCharCopy((char*) rval, '\'', '\'');

    char v_namespace[CF_MAXVARSIZE] = { 0 };
    SetVirtualNameSpace(bundle, ns, v_namespace, CF_MAXVARSIZE);

    Buffer *insert_buf = BufferNew();
    BufferPrintf(insert_buf,
                 "INSERT INTO %s VALUES('%s','%s','%s','%s','%s','%s');",
                 SQL_TABLE_VARIABLES,
                 SkipHashType(keyhash),
                 v_namespace,
                 bundle,
                 lval,
                 rval_scalar_escaped,
                 DataTypeShortToType(dtype));

    free(rval_scalar_escaped);

    bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

    BufferDestroy(&insert_buf);
    insert_buf = NULL;

    return exec_ok;
}

/******************************************************************/

static bool FileChangesInsertSQL(sqlite3 *db, HubFileChanges *change_record)
{
    assert(change_record);

    Buffer *insert_buf = BufferNew();
    BufferPrintf(insert_buf,
                 "INSERT INTO %s VALUES('%s','%s','%s',%ld,NULL,NULL,NULL);",
                 SQL_TABLE_FILECHANGES,
                 SkipHashType(change_record->hh->keyhash),
                 NULLStringToEmpty(change_record->handle),
                 NULLStringToEmpty(change_record->path),
                 change_record->t);

    bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

    BufferDestroy(&insert_buf);
    insert_buf = NULL;

    return exec_ok;
}

static bool FileDiffsInsertSQL(sqlite3 *db, HubFileDiff *diff_record)
{
    assert(diff_record);

    char tline[CF_BUFSIZE] = { 0 };

    for (const char *sp = diff_record->diff; sp && *sp != '\0'; sp += strlen(tline) + 1)
    {
        int line = -1;
        char diff_type = -1;

        char diff[CF_BUFSIZE] = { 0 };
        sscanf(sp, "%c,%d,%2047[^\n]", &diff_type, &line, diff);
        sscanf(sp, "%2047[^\n]", tline);

        char diff_type_str[CF_SMALLBUF] = {0};
        switch (diff_type)
        {
        case '+':
            snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_ADD);
            break;
        case '-':
            snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_REMOVE);
            break;
        default:
            snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_UNKNOWN);
            break;
        }

        char *diff_escaped = EscapeCharCopy(diff, '\'', '\'');

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%ld,'%s',%d,'%s');",
                     SQL_TABLE_FILECHANGES,
                     SkipHashType(diff_record->hh->keyhash),
                     NULLStringToEmpty(diff_record->promise_handle),
                     NULLStringToEmpty(diff_record->path),
                     diff_record->t,
                     diff_type_str,
                     line,
                     diff_escaped);

        free(diff_escaped);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            return false;
        }
    }

    return true;
}


static bool FileRecordsInsertSQL(sqlite3 *db, HubFileChanges *change_record)
{
    assert(change_record);

    if (change_record->diff_record == NULL)
    {
        return FileChangesInsertSQL(db, change_record);
    }
    else
    {
        return FileDiffsInsertSQL(db, change_record->diff_record);
    }

    return true;
}

/******************************************************************/

static void EnterpriseDBToSqlite3_FileChanges(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq_fc = CFDB_QueryFileChanges(&dbconn, NULL, NULL, 0, time(NULL),
                                         filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                         QUERY_FLAG_DISABLE_ALL);

    HubQuery *hq_fd = CFDB_QueryFileDiff(&dbconn, NULL, NULL, NULL, 0, time(NULL),
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                      QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *changep = hq_fc->records; changep != NULL; changep = changep->next)
    {
        HubFileChanges *hC = (HubFileChanges *) changep->item;

        for (const Rlist *diffp = hq_fd->records; diffp != NULL; diffp = diffp->next)
        {
            HubFileDiff *hD = (HubFileDiff *) diffp->item;

            if (FileRecordsEqual(hC, hD))
            {
                hC->diff_record = hD;
                break;
            }
        }

        if (!FileRecordsInsertSQL(db, hC))
        {
            DeleteHubQuery(hq_fc, DeleteHubFileChanges);
            DeleteHubQuery(hq_fd, DeleteHubFileDiff);
            return;
        }
    }    

    DeleteHubQuery(hq_fc, DeleteHubFileChanges);
    DeleteHubQuery(hq_fd, DeleteHubFileDiff);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Software(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_software, NULL,NULL,NULL,
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%s');",
                     SQL_TABLE_SOFTWARE,
                     SkipHashType(hs->hh->keyhash),
                     NULLStringToEmpty(hs->name),
                     NULLStringToEmpty(hs->version),
                     Nova_LongArch(hs->arch));

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
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

    HubQuery *hq = CFDB_QueryPromiseCompliance(&dbconn, NULL, NULL, PROMISE_STATE_ANY,
                                               0, time(NULL), filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                               QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseCompliance *hc = (HubPromiseCompliance *) rp->item;

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%ld);",
                     SQL_TABLE_PROMISESTATUS,
                     SkipHashType(hc->hh->keyhash),
                     NULLStringToEmpty(hc->handle),
                     PromiseStateToString(hc->status),
                     hc->t);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
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
                if(!EnterpriseDBToSqlite3_PromiseDefinitions_Insert(db, hp->ns, hp->handle, hp->promiser, hp->bundleName, RlistScalarValue(promisees)))
                {
                    DeleteHubQuery(hq, DeleteHubPromise);
                    return;
                }
            }
        }
        else
        {
            if(!EnterpriseDBToSqlite3_PromiseDefinitions_Insert(db, hp->ns, hp->handle, hp->promiser, hp->bundleName, ""))
            {
                DeleteHubQuery(hq, DeleteHubPromise);
                return;
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubPromise);
}


static bool EnterpriseDBToSqlite3_PromiseDefinitions_Insert(sqlite3 *db, const char *ns, const char *handle, const char *promiser,
                                                            const char *bundle_name, const char *promisee)
{
    char *promiser_escaped = EscapeCharCopy(promiser, '\'', '\'');
    char *promisee_escaped = EscapeCharCopy(promisee, '\'', '\'');

    char v_namespace[CF_MAXVARSIZE] = { 0 };
    SetVirtualNameSpace(bundle_name, ns, v_namespace, CF_MAXVARSIZE);

    Buffer *insert_buf = BufferNew();

    BufferPrintf(insert_buf,
                 "INSERT INTO %s VALUES('%s','%s','%s','%s','%s');",
                 SQL_TABLE_PROMISEDEFINITIONS,
                 v_namespace,
                 handle,
                 promiser_escaped,
                 bundle_name,
                 promisee_escaped);

    free(promisee_escaped);
    free(promiser_escaped);

    bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

    BufferDestroy(&insert_buf);

    return exec_ok;
}

/******************************************************************/

static void EnterpriseDBToSqlite3_PromiseLog(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDBToSqlite3_NotKeptLog(db, filter);
    EnterpriseDBToSqlite3_RepairedLog(db, filter);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_NotKeptLog(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, NULL, PROMISE_LOG_STATE_NOTKEPT, NULL, false, NULL,
                                        0, time(NULL), false, filter, NULL, PROMISE_CONTEXT_MODE_ALL, NULL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseLog *hp = (HubPromiseLog *) rp->item;

        char *cause_escaped = EscapeCharCopy(NULLStringToEmpty(hp->cause), '\'', '\'');

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%s',%ld);",
                     SQL_TABLE_PROMISELOG,
                     SkipHashType(hp->hh->keyhash),
                     NULLStringToEmpty(hp->handle),
                     LABEL_STATE_NOTKEPT,
                     cause_escaped,
                     hp->t);

        free(cause_escaped);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseLog);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_RepairedLog(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLog(&dbconn, NULL, PROMISE_LOG_STATE_REPAIRED, NULL, false, NULL,
                                        0, time(NULL), false, filter, NULL, PROMISE_CONTEXT_MODE_ALL, NULL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubPromiseLog *hp = (HubPromiseLog *) rp->item;

        char *cause_escaped = EscapeCharCopy(NULLStringToEmpty(hp->cause), '\'', '\'');

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%s',%ld);",
                     SQL_TABLE_PROMISELOG,
                     SkipHashType(hp->hh->keyhash),
                     NULLStringToEmpty(hp->handle),
                     LABEL_STATE_REPAIRED,
                     cause_escaped,
                     hp->t);

        free(cause_escaped);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseLog);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_PromiseSummary(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDBToSqlite3_RepairedLogsSummary(db, filter);
    EnterpriseDBToSqlite3_NotKeptLogsSummary(db, filter);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_RepairedLogsSummary(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLogSummary(&dbconn, NULL, PROMISE_LOG_STATE_REPAIRED, NULL,
                                               false, NULL, 0, time(NULL), false,
                                               filter, PROMISE_CONTEXT_MODE_ALL, NULL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp; rp = rp->next)
    {
        const HubPromiseSum *record = (const HubPromiseSum *)rp->item;

        char *cause_escaped = EscapeCharCopy(NULLStringToEmpty(record->cause), '\'', '\'');

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%d);",
                     SQL_TABLE_PROMISE_SUMMARY,
                     NULLStringToEmpty(record->handle),
                     LABEL_STATE_REPAIRED,
                     cause_escaped,
                     record->occurences);

        free(cause_escaped);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseSum);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_NotKeptLogsSummary(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPromiseLogSummary(&dbconn, NULL, PROMISE_LOG_STATE_NOTKEPT, NULL,
                                               false, NULL, 0, time(NULL), false,
                                               filter, PROMISE_CONTEXT_MODE_ALL, NULL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp; rp = rp->next)
    {
        const HubPromiseSum *record = (const HubPromiseSum *)rp->item;

        char *cause_escaped = EscapeCharCopy(NULLStringToEmpty(record->cause), '\'', '\'');

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%d);",
                     SQL_TABLE_PROMISE_SUMMARY,
                     NULLStringToEmpty(record->handle),
                     LABEL_STATE_NOTKEPT,
                     cause_escaped,
                     record->occurences);

        free(cause_escaped);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseSum);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_BundleStatus(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryBundleSeen(&dbconn, NULL, NULL, NULL, filter,
                                   PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubBundleSeen *hb = (HubBundleSeen *) rp->item;

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%f,%ld);",
                     SQL_TABLE_BUNDLESTATUS,
                     SkipHashType(hb->hh->keyhash),
                     NULLStringToEmpty(hb->ns),
                     NULLStringToEmpty(hb->bundle),
                     (hb->bundlecomp * 100.0),
                     hb->t);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubBundleSeen);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_Benchmarks(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryPerformance(&dbconn, NULL, NULL, filter,
                                         PROMISE_CONTEXT_MODE_ALL, NULL,
                                         QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubPerformance *hP = (HubPerformance *) rp->item;

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s',%f,%ld);",
                     SQL_TABLE_BENCHMARKS,
                     SkipHashType(hP->hh->keyhash),
                     NULLStringToEmpty(hP->event),
                     hP->q,
                     hP->t);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubPerformance);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_LastSeen(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryLastSeen(&dbconn, NULL, NULL, NULL, NULL, -1.0, 0, time(NULL),
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubLastSeen *hl = (HubLastSeen *) rp->item;

        char inout[CF_SMALLBUF];
        switch (hl->direction)
        {
        case LAST_SEEN_DIRECTION_OUTGOING:
            snprintf(inout, CF_SMALLBUF, "out");
            break;

        case LAST_SEEN_DIRECTION_INCOMING:
            snprintf(inout, CF_SMALLBUF, "in");
            break;

        default:
            snprintf(inout, CF_SMALLBUF, "unknown");
            break;
        }

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s',%ld,%d);",
                     SQL_TABLE_LASTSEEN,
                     SkipHashType(hl->hh->keyhash),
                     inout,
                     SkipHashType(hl->rhost->keyhash),
                     hl->t,
                     (int)(hl->hrsago * SECONDS_PER_HOUR));

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubLastSeen);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_TotalCompliance(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryTotalCompliance(&dbconn, NULL, NULL, 0, time(NULL), -1, -1, -1,
                                             filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubTotalCompliance *ht = (HubTotalCompliance *) rp->item;

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s',%d,%d,%d,%ld);",
                     SQL_TABLE_TOTALCOMPLIANCE,
                     SkipHashType(ht->hh->keyhash),
                     NULLStringToEmpty(ht->version),
                     ht->kept,
                     ht->repaired,
                     ht->notkept,
                     ht->t);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubTotalCompliance);
}


/******************************************************************/
static const char *LABEL_PATCH_INSTALLED = "Installed";
static const char *LABEL_PATCH_AVAILABLE = "Available";

static void EnterpriseDBToSqlite3_Patch(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDBToSqlite3_PatchInstalled(db, filter);
    EnterpriseDBToSqlite3_PatchAvailable(db, filter);
}

/******************************************************************/
static void EnterpriseDBToSqlite3_PatchInstalled(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_patch_installed, NULL,NULL,NULL,
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        Buffer *insert_buf = BufferNew();

        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%s','%s');",
                     SQL_TABLE_PATCH,
                     SkipHashType(hs->hh->keyhash),
                     LABEL_PATCH_INSTALLED,
                     NULLStringToEmpty(hs->name),
                     NULLStringToEmpty(hs->version),
                     Nova_LongArch(hs->arch));

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubSoftware);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_PatchAvailable(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QuerySoftware(&dbconn, NULL, cfr_patch_avail, NULL,NULL,NULL,
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubSoftware *hs = (HubSoftware *) rp->item;

        Buffer *insert_buf = BufferNew();

        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%s','%s','%s','%s','%s');",
                     SQL_TABLE_PATCH,
                     SkipHashType(hs->hh->keyhash),
                     LABEL_PATCH_AVAILABLE,
                     NULLStringToEmpty(hs->name),
                     NULLStringToEmpty(hs->version),
                     Nova_LongArch(hs->arch));

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    DeleteHubQuery(hq, DeleteHubSoftware);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_FileDiffs(sqlite3 *db, HostClassFilter *filter)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    HubQuery *hq = CFDB_QueryFileDiff(&dbconn, NULL, NULL, NULL, 0, time(NULL),
                                      filter, PROMISE_CONTEXT_MODE_ALL, NULL,
                                      QUERY_FLAG_DISABLE_ALL);

    CFDB_Close(&dbconn);

    for (const Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubFileDiff *hd = (HubFileDiff *) rp->item;

        if (!hd->diff)
        {
            continue;
        }

        char tline[CF_BUFSIZE] = { 0 };
        for (const char *sp = hd->diff; sp && *sp != '\0'; sp += strlen(tline) + 1)
        {
            int line = -1;
            char diff_type = -1;

            char diff[CF_BUFSIZE] = { 0 };
            sscanf(sp, "%c,%d,%2047[^\n]", &diff_type, &line, diff);
            sscanf(sp, "%2047[^\n]", tline);

            char diff_type_str[CF_SMALLBUF] = {0};
            switch (diff_type)
            {
            case '+':
                snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_ADD);
                break;
            case '-':
                snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_REMOVE);
                break;
            default:
                snprintf(diff_type_str, CF_SMALLBUF, "%s", LABEL_UNKNOWN);
                break;
            }

            char *diff_escaped = EscapeCharCopy(diff, '\'', '\'');

            Buffer *insert_buf = BufferNew();
            BufferPrintf(insert_buf,
                         "INSERT INTO %s VALUES('%s','%s','%s',%ld,'%s',%d,'%s');",
                         SQL_TABLE_FILEDIFFS,
                         SkipHashType(hd->hh->keyhash),
                         NULLStringToEmpty(hd->promise_handle),
                         NULLStringToEmpty(hd->path),
                         hd->t,
                         diff_type_str,
                         line,
                         diff_escaped);

            free(diff_escaped);

            bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

            BufferDestroy(&insert_buf);
            insert_buf = NULL;

            if (!exec_ok)
            {
                DeleteHubQuery(hq, DeleteHubFileDiff);
                return;
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubFileDiff);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_DiagnosticServiceStatus(sqlite3 *db)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    Seq *diag_mongo_snapshot = DiagnosticsQueryMongoSnapshot(&dbconn);

    CFDB_Close(&dbconn);

    if (!diag_mongo_snapshot)
    {
        return;
    }

    for (int i = 0; i < SeqLength(diag_mongo_snapshot); i++)
    {
        DiagnosticMongoSnaphot *stats =
                (DiagnosticMongoSnaphot*) SeqAt(diag_mongo_snapshot, i);

        if (!stats)
        {
            continue;
        }

        if (!stats->status)
        {
            continue;
        }

        Buffer *insert_buf = BufferNew();
        BufferPrintf(insert_buf,
                     "INSERT INTO %s VALUES('%ld','%s','%s','%lf','%lf','%lf','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",
                     SQL_TABLE_DIAGNOSTIC_SERVER_STATUS,
                     stats->time,
                     NULLStringToEmpty(stats->status->host),
                     NULLStringToEmpty(stats->status->version),
                     stats->status->server_uptime,
                     stats->status->global_lock_total_time_us,
                     stats->status->global_lock_lock_time_us,
                     stats->status->global_lock_queue_total,
                     stats->status->global_lock_queue_readers,
                     stats->status->global_lock_queue_writers,
                     stats->status->mem_resident,
                     stats->status->mem_virtual,
                     stats->status->mem_mapped,
                     stats->status->bg_flush_count,
                     stats->status->bg_flush_total_ms,
                     stats->status->bg_flush_avg_ms,
                     stats->status->bg_flush_last_ms);

        bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

        BufferDestroy(&insert_buf);
        insert_buf = NULL;

        if (!exec_ok)
        {
            break;
        }
    }

    SeqDestroy(diag_mongo_snapshot);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_DiagnosticDatabaseStatus(sqlite3 *db)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    Seq *diag_mongo_snapshot = DiagnosticsQueryMongoSnapshot(&dbconn);

    CFDB_Close(&dbconn);

    if (!diag_mongo_snapshot)
    {
        return;
    }

    for (int i = 0; i < SeqLength(diag_mongo_snapshot); i++)
    {
        DiagnosticMongoSnaphot *stats =
                (DiagnosticMongoSnaphot*) SeqAt(diag_mongo_snapshot, i);

        if (!stats)
        {
            continue;
        }

        if (!stats->status)
        {
            continue;
        }

        if (!stats->status->db_list)
        {
            continue;
        }

        for (int j = 0; j < SeqLength(stats->status->db_list); j++)
        {

            DiagnosticDatabaseStatus *db_status =
                    (DiagnosticDatabaseStatus*) SeqAt(stats->status->db_list ,j);

            if (!db_status)
            {
                continue;
            }

            Buffer *insert_buf = BufferNew();
            BufferPrintf(insert_buf,
                         "INSERT INTO %s VALUES('%ld','%s','%lf','%d',%d,'%d');",
                         SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS,
                         stats->time,
                         NULLStringToEmpty(db_status->db_name),
                         db_status->avg_object_size,
                         db_status->data_size,
                         db_status->storage_size,
                         db_status->file_size);

            bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

            BufferDestroy(&insert_buf);
            insert_buf = NULL;

            if (!exec_ok)
            {
                SeqDestroy(diag_mongo_snapshot);
                return;
            }
        }
    }

    SeqDestroy(diag_mongo_snapshot);
}

/******************************************************************/

static void EnterpriseDBToSqlite3_DiagnosticCollectionStatus(sqlite3 *db)
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    Seq *diag_mongo_snapshot = DiagnosticsQueryMongoSnapshot(&dbconn);

    CFDB_Close(&dbconn);

    if (!diag_mongo_snapshot)
    {
        return;
    }

    for (int i = 0; i < SeqLength(diag_mongo_snapshot); i++)
    {
        DiagnosticMongoSnaphot *stats =
                (DiagnosticMongoSnaphot*) SeqAt(diag_mongo_snapshot, i);

        if (!stats)
        {
            continue;
        }

        if (!stats->status)
        {
            continue;
        }

        if (!stats->status->db_list)
        {
            continue;
        }

        for (int j = 0; j < SeqLength(stats->status->db_list); j++)
        {

            DiagnosticDatabaseStatus *db_status =
                    (DiagnosticDatabaseStatus*) SeqAt(stats->status->db_list, j);

            if (!db_status)
            {
                continue;
            }

            if (!db_status->collection_list)
            {
                continue;
            }

            for (int k = 0; k < SeqLength(db_status->collection_list); k++)
            {

                DiagnosticCollectionStatus *coll_status =
                        (DiagnosticCollectionStatus*) SeqAt(db_status->collection_list, k);

                if (!coll_status)
                {
                    continue;
                }

                Buffer *insert_buf = BufferNew();
                BufferPrintf(insert_buf,
                             "INSERT INTO %s VALUES('%ld','%s','%s','%d','%d',%lf,'%d','%d','%d','%lf');",
                             SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS,
                             stats->time,
                             NULLStringToEmpty(db_status->db_name),
                             NULLStringToEmpty(coll_status->name),
                             coll_status->object_count,
                             coll_status->data_size,
                             coll_status->avg_object_size,
                             coll_status->storage_size,
                             coll_status->index_count,
                             coll_status->total_index_size,
                             coll_status->padding_factor);

                bool exec_ok = Sqlite3_Execute(db, BufferData(insert_buf), (void *) BuildJsonOutput, 0);

                BufferDestroy(&insert_buf);
                insert_buf = NULL;

                if (!exec_ok)
                {
                    SeqDestroy(diag_mongo_snapshot);
                    return;
                }
            }
        }
    }

    SeqDestroy(diag_mongo_snapshot);
}

/******************************************************************/

bool GetTableNamesInQuery(const char *select_op, Set *tables_set)
{
    assert(tables_set);
    assert(select_op);

    char *select_low = SafeStringDuplicate(select_op);
    ToLowerStrInplace(select_low);

    bool found = false;

    for (int i = 0; TABLES[i] != NULL; i++)
    {
        char *table_name_low = xstrdup(TABLES[i]);
        ToLowerStrInplace(table_name_low);

        Buffer *table_regex = BufferNew();

        BufferPrintf(table_regex, "\\b%s\\b", table_name_low);

        if (StringMatch(BufferData(table_regex), select_low))
        {
            found = true;
            SetAdd(tables_set, xstrdup(TABLES[i]));
        }

        free(table_name_low);
        BufferDestroy(&table_regex);
    }

    free(select_low);
    return found;
}

/******************************************************************/

static bool CreateSQLTable(sqlite3 *db, char *create_sql)
{
    return Sqlite3_Execute(db, create_sql, (void *) BuildJsonOutput, 0);
}

/******************************************************************/

bool GenerateAllTables(sqlite3 *db)
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
    assert(db);

    return Sqlite3_Execute(db, "BEGIN TRANSACTION;", (void *) BuildJsonOutput, 0);
}

/******************************************************************/

static bool Sqlite3_CommitTransaction(sqlite3 *db)
{
    assert(db);

    return Sqlite3_Execute(db, "COMMIT;", (void *) BuildJsonOutput, 0);
}
/******************************************************************/

int BuildCSVOutput(void *out, int argc, char **argv, char **azColName)
{
    Writer *writer = (Writer *) out;

    CsvWriter *c = CsvWriterOpen(writer);

    for (int i = 0; i < argc; i++)
    {
        CsvWriterField(c, argv[i] ? argv[i] : "NULL");
    }
    CsvWriterNewRecord(c);
    CsvWriterClose(c);

    return 0;
}

/******************************************************************/

static void SetVirtualNameSpace(const char *key, const char *namespace,
                                char *buffer, size_t buffer_size)
{
    if (IsKeyWithinPromiseContext(namespace, key, PROMISE_CONTEXT_MODE_INTERNAL))
    {
        snprintf(buffer, buffer_size, "%s", CF_INTERNAL_NAMESPACE);
    }
    else
    {
        if (namespace)
        {
            snprintf(buffer, buffer_size, "%s", namespace);
        }
        else
        {
            snprintf(buffer, buffer_size, "%s", "");
        }
    }
}

/******************************************************************/

static ReportingEngineAsyncError ExportColumnNamesCSV(sqlite3 *db, const char *select_op, WebReportFileInfo *wr_info)
{
    assert(wr_info);
    assert(select_op);
    assert(db);

    FILE *fout = fopen(wr_info->csv_path, "w");
    if (!fout)
    {
        return REPORTING_ENGINE_ASYNC_ERROR_IO;
    }

    Writer *writer = FileWriter(fout);
    assert(writer);

    int column_count = WriteColumnNamesCsv(db, select_op, writer);

    WriterClose(writer);

    if (column_count < 0)
    {
        return REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_PREPARE;
    }
    else  if (column_count == 0)
    {
        // NOTE: since the rows count are > 0 before this method is called
        // this has to be an error
        return REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_QUERY;
    }

    return REPORTING_ENGINE_ASYNC_SUCCESS;
}

/******************************************************************/

int ExportCSVOutput(void *out, int argc, char **argv, char **azColName)
{
    assert(out);
    //assert( wr_info->total_lines >= 0 );

    WebReportFileInfo *wr_info = (WebReportFileInfo *) out;

    if( !wr_info->write_data )
    {
        wr_info->total_lines++;
        return 0;
    }

    Writer *writer = NULL;
    {
        FILE *stream = fopen(wr_info->csv_path, "a+");
        if (!stream)
        {
            syslog(LOG_ERR, "Opening csv report path for append-read (a+), path %s, errno: %d", wr_info->csv_path, errno);
            return -1;
        }
        writer = FileWriter(stream);
    }
    assert(writer);

    // TODO: handle sqlite3 db close
    ExportWebReportCheckAbort(wr_info, writer);

    assert( writer );
    assert( wr_info->lines_written >= 0 );
    assert( wr_info->lines_since_last_update >= 0 );

    if (wr_info->report_type & REPORT_FORMAT_CSV)
    {
        CsvWriter *c = CsvWriterOpen(writer);

        for(int i=0; i<argc; i++)
        {
            CsvWriterField(c, argv[i] ? argv[i] : "NULL");
        }
        CsvWriterNewRecord(c);
        CsvWriterClose(c);
    }
    else
    {
        // support for other formats later
        WriterClose(writer);
        return 0;
    }

    wr_info->lines_written++;

    if( wr_info->lines_since_last_update++ >= CHECKPOINT )
    {
        wr_info->lines_since_last_update = 0;

        if( !ExportWebReportUpdateStatus( wr_info ))
        {
            WriterClose(writer);
            return 0;
        }
    }

    WriterClose(writer);
    return 0;
}

/******************************************************************/

static const char *ReportingEngineAsyncErrorToString(ReportingEngineAsyncError err)
{
    assert(err < REPORTING_ENGINE_ASYNC_ERROR_MAX);

    switch (err)
    {
    case REPORTING_ENGINE_ASYNC_SUCCESS:
        return "Success";
    case REPORTING_ENGINE_ASYNC_ERROR_START_PROC:
        return "Unable to start process";
    case REPORTING_ENGINE_ASYNC_ERROR_ENTERPRISE_DB_CONNECT:
        return "Unable to connect to main database";
    case REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_CONNECT:
        return "Unable to connect to sqlite3";
    case REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_PREPARE:
        return "Unable to prepare statement for sqlite3";
    case REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_QUERY:
        return "Error in sqlite3 query";
    case REPORTING_ENGINE_ASYNC_ERROR_IO:
        return "I/O error";
    case REPORTING_ENGINE_ASYNC_ERROR_UNEXPECTED_CHILD_EXIT:
        return "Unexpected child process exit";
    case REPORTING_ENGINE_ASYNC_ERROR_DOCROOT_NOT_FOUND:
        return "Document root cannot be found";
    case REPORTING_ENGINE_ASYNC_ERROR_WRITING:
        return "Error writing report file";
    default:
        return "Unknown";
    }
}

JsonElement *PackageAsyncQueryStatusResult(ReportingEngineAsyncError err_id, const char *token,
                                           size_t percentage_complete, const char *href_static)
{
    assert(token);
    assert(href_static);

    JsonElement *payload = JsonObjectCreate(1);

    if (err_id == REPORTING_ENGINE_ASYNC_SUCCESS)
    {
        JsonObjectAppendString(payload, "id", token);

        JsonObjectAppendInteger(payload, "percentageComplete", percentage_complete);

        if (percentage_complete >= 100)
        {
            assert(percentage_complete == 100);

            char *href = StringConcatenate(3, href_static, token, ".csv");
            JsonObjectAppendString(payload, "href", href);
            free(href);
        }
    }
    else
    {
        JsonObjectAppendString(payload, "error", ReportingEngineAsyncErrorToString(err_id));
    }

    return payload;
}

JsonElement *PackageAsyncQueryCreateResult(ReportingEngineAsyncError err_id, const char *query, const char *token)
{
    JsonElement *payload = JsonObjectCreate(2);

    if (err_id == REPORTING_ENGINE_ASYNC_SUCCESS)
    {
        JsonObjectAppendString(payload, "id", token);
    }
    else
    {
        JsonObjectAppendString(payload, "error", ReportingEngineAsyncErrorToString(err_id));
    }
    JsonObjectAppendString(payload, "query", query);

    return payload;
}

JsonElement *PackageAsyncQueryAbortResult(ReportingEngineAsyncError err_id, const char *token)
{
    JsonElement *payload = JsonObjectCreate(1);

    if (err_id == REPORTING_ENGINE_ASYNC_SUCCESS)
    {
        JsonObjectAppendString(payload, "id", token);
    }
    else
    {
        JsonObjectAppendString(payload, "error", ReportingEngineAsyncErrorToString(err_id));
    }

    return payload;
}

/******************************************************************/

static void AsyncToken(const char *username, const char *query, char token_out[MD5_HEX_LENGTH + 1])
{
    char digest[EVP_MAX_MD_SIZE + 1] = { 0 };

    HashString(username, strlen(username), digest, cf_md5);
    HashString(query, strlen(query), digest, cf_md5);

    char time_str[CF_SMALLBUF] = {0};
    snprintf(time_str, CF_SMALLBUF - 1, "%ld", time(NULL));

    HashString(time_str, CF_SMALLBUF - 1, digest, cf_md5);

    char prefixed_hash[EVP_MAX_MD_SIZE * 4] = { 0 };
    HashPrintSafe(cf_md5, digest, prefixed_hash);

    strncpy(token_out, prefixed_hash + 4, MD5_HEX_LENGTH);
    token_out[32] = '\0';
}

/******************************************************************/

JsonElement *EnterpriseExecuteSQLAsync(const char *username, const char *select_op)
{
    assert( username && select_op );

    char token[MD5_HEX_LENGTH + 1] = { 0 };
    AsyncToken(username, select_op, token);
    pid_t pid = fork();

    if (pid == -1)
    {
        return PackageAsyncQueryCreateResult(REPORTING_ENGINE_ASYNC_ERROR_START_PROC, select_op, token);
    }    

    char docroot[CF_MAXVARSIZE] = {0};

    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        return PackageAsyncQueryCreateResult(REPORTING_ENGINE_ASYNC_ERROR_ENTERPRISE_DB_CONNECT, select_op, token);
    }

    if (!CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH))
    {
        return PackageAsyncQueryCreateResult(REPORTING_ENGINE_ASYNC_ERROR_DOCROOT_NOT_FOUND, select_op, token);
    }

    CFDB_Close(conn);

    char path_to_file[CF_MAXVARSIZE] = {0};
    snprintf(path_to_file, CF_MAXVARSIZE, "%s/api/static", docroot);

    WebReportFileInfo *wr_info = NULL;
    wr_info = NewWebReportFileInfo(REPORT_FORMAT_CSV, path_to_file, token, "");

    if (pid == 0)
    {
        ALARM_PID = -1;

        char *select_op_expanded = SqlVariableExpand(select_op);

        sqlite3 *db;
        if (!Sqlite3_DBOpen(&db))
        {
            free(select_op_expanded);
            syslog(LOG_ERR, "code %d, message: %s", REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_CONNECT, "Cannot connect to temporary DB");
            _exit(0);
        }

        if (!EnterpriseQueryPrepare(db, username, select_op_expanded))
        {
            Sqlite3_DBClose(db);
            free(select_op_expanded);

            syslog(LOG_ERR, "code %d, message: %s", REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_PREPARE, "Cannot load temporary DB");
            _exit(0);
        }        

        Writer *writer = NULL;
        struct stat s;
        if (stat(wr_info->csv_path, &s) == -1)
        {
            FILE *write_file = fopen(wr_info->csv_path, "w");
            if (!write_file)
            {
                Sqlite3_DBClose(db);
                free(select_op_expanded);
                syslog(LOG_ERR, "code %d, message: %s, os errno: %d, path: %s", REPORTING_ENGINE_ASYNC_ERROR_IO, "Could not open write file for writing", errno, wr_info->csv_path);
                _exit(0);
            }

            writer = FileWriter(write_file);
        }

        WriterClose(writer);

        AsyncQueryExportResult(db, select_op_expanded, wr_info);

        DeleteWebReportFileInfo(wr_info);

        Sqlite3_DBClose(db);
        free(select_op_expanded);
        _exit(0);
    }
    else
    {
        wr_info->child_pid = pid;
        if (!WebExportWriteChildPid(wr_info))
        {
            return PackageAsyncQueryCreateResult(REPORTING_ENGINE_ASYNC_ERROR_WRITING, select_op, token);
        }
    }

    DeleteWebReportFileInfo(wr_info);

    return PackageAsyncQueryCreateResult(REPORTING_ENGINE_ASYNC_SUCCESS, select_op, token);
}

/******************************************************************/

void AsyncQueryExportResult(sqlite3 *db, const char *select_op, WebReportFileInfo *wr_info)
{
    /* Query sqlite and print table contents */

    //count
    wr_info->total_lines = 0;
    wr_info->write_data = false;

    if (!Sqlite3_Execute(db, select_op, (void *) ExportCSVOutput, wr_info))
    {
        return;
    }

    if(wr_info->total_lines <= 0)
    {
        syslog(LOG_ERR, "Query returned empty results - code %d, sql \"%s\"",
               REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_QUERY,
               select_op);
        return;
    }

    // export
    wr_info->write_data = true;

    // write csv header
    int write_header_status = ExportColumnNamesCSV(db, select_op, wr_info);

    if (write_header_status != REPORTING_ENGINE_ASYNC_SUCCESS)
    {
        syslog(LOG_ERR, "Error writing CSV header - code %d, sql \"%s\"",
               write_header_status,
               select_op);

        return;
    }

    if( !ExportWebReportStatusInitialize( wr_info ) )
    {
        syslog(LOG_ERR, "Error initializing status - code %d, os errno: %d sql \"%s\"",
               REPORTING_ENGINE_ASYNC_ERROR_IO,
               errno,
               select_op);
        return;
    }

    /* Sqlite3_Execute writes to syslog on failure, no need to write it twice */
    if (!Sqlite3_Execute(db, select_op, (void *) ExportCSVOutput, wr_info))
    {
        return;
    }

    if (!ExportWebReportStatusFinalize(wr_info))
    {
        syslog(LOG_ERR, "Error finalizing status - code %d, os errno: %d, sql \"%s\"",
               REPORTING_ENGINE_ASYNC_ERROR_IO,
               errno,
               select_op);
        return;
    }
}

/******************************************************************/

bool EnterpriseQueryPrepare(sqlite3 *db, const char *username, const char *select_op_expanded)
{
    if (!GenerateAllTables(db))
    {
        return false;
    }

    Set *tables = SetNew((MapHashFn)OatHash, (MapKeyEqualFn)StringSafeEqual, free);

    if (!(GetTableNamesInQuery(select_op_expanded, tables)))
    {
        return false;
    }

    LoadSqlite3Tables(db, tables, username);

    SetDestroy(tables);

    return true;
}

/******************************************************************/

JsonElement *EnterpriseExecuteSQLSync(const char *username, const char *select_op)
{
    char *select_op_expanded = SqlVariableExpand(select_op);

    sqlite3 *db;

    if (!Sqlite3_DBOpen(&db))
    {
        Sqlite3_DBClose(db);
        JsonElement *result = PackageReportingEngineResult(select_op_expanded, JsonArrayCreate(0), JsonArrayCreate(0));
        free(select_op_expanded);
        return result;
    }

    if (!EnterpriseQueryPrepare(db, username, select_op_expanded))
    {
        Sqlite3_DBClose(db);
        JsonElement *result = PackageReportingEngineResult(select_op_expanded, JsonArrayCreate(0), JsonArrayCreate(0));
        free(select_op_expanded);
        return result;
    }

    LogPerformanceTimer timer = LogPerformanceStart();

    JsonElement *out = EnterpriseQueryPublicDataModel(db, select_op_expanded);
    Sqlite3_DBClose(db);

    assert(out);

    LogPerformanceStop(&timer, "Reporting Engine select operation time for %s", select_op_expanded);

    free(select_op_expanded);
    return out;
}

/******************************************************************/

JsonElement *AsyncQueryStatus(const char *token, int report_type, const char *static_files_uri)
{
    assert(token);

    ReportingEngineAsyncError err = REPORTING_ENGINE_ASYNC_SUCCESS;

    //TODO: directory must be configurable       
    char docroot[CF_MAXVARSIZE] = {0};
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        err = REPORTING_ENGINE_ASYNC_ERROR_ENTERPRISE_DB_CONNECT;

        return PackageAsyncQueryStatusResult(err, token, -1, static_files_uri);
    }

    if (!CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH))
    {
        err = REPORTING_ENGINE_ASYNC_ERROR_DOCROOT_NOT_FOUND;

        return PackageAsyncQueryStatusResult(err, token, -1, static_files_uri);
    }

    CFDB_Close(conn);

    char path_to_file[CF_MAXVARSIZE] = {0};
    snprintf(path_to_file, CF_MAXVARSIZE, "%s/api/static", docroot);

    WebReportFileInfo *wr_info = NULL;
    wr_info = NewWebReportFileInfo(report_type, path_to_file, token, "");

    if(!IsExporterProcRunning(wr_info))
    {
        return PackageAsyncQueryStatusResult(REPORTING_ENGINE_ASYNC_ERROR_UNEXPECTED_CHILD_EXIT,
                                             token, -1, static_files_uri);
    }

    int status = ReadExportStatus(wr_info);

    if(status < 0)
    {
        return PackageAsyncQueryStatusResult(REPORTING_ENGINE_ASYNC_ERROR_IO, token,
                                             status, static_files_uri);
    }

    return PackageAsyncQueryStatusResult(REPORTING_ENGINE_ASYNC_SUCCESS, token,
                                         status, static_files_uri);
}

/******************************************************************/

static bool IsExporterProcRunning(WebReportFileInfo *wr_info)
{
    assert(wr_info);

    if (wr_info->child_pid <= 0)
    {
        ReadExporterPid(wr_info);
    }

    assert(wr_info->child_pid > 0);

    if( kill(wr_info->child_pid, 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************/

bool ReadExporterPid(WebReportFileInfo *wr_info)
{
    assert(wr_info);

    char pid_file[CF_MAXVARSIZE] = {0};
    snprintf(pid_file, CF_MAXVARSIZE - 1, "%s.pid", wr_info->csv_path);

    FILE *fin = fopen(pid_file, "r");
    if (fin == NULL)
    {
        return false;
    }

    int pid = -1;
    fscanf(fin,"%d",&pid);

    fclose(fin);

    wr_info->child_pid = (pid_t) pid;

    return wr_info->child_pid > 0;
}

/******************************************************************/

int ReadExportStatus(WebReportFileInfo *wr_info)
{
    assert(wr_info);

    char status_file[CF_MAXVARSIZE] = {0};
    snprintf(status_file, CF_MAXVARSIZE - 1, "%s.status", wr_info->csv_path);

    FILE *fin = fopen(status_file, "r");
    if (fin == NULL)
    {
        return -1;
    }

    int status = -1;
    fscanf(fin,"%d",&status);
    fclose(fin);

    return status;
}

/******************************************************************/

JsonElement *AsyncQueryAbort(const char *token)
{
    assert(token);

    //TODO: directory must be configurable
    WebReportFileInfo *wr_info = NULL;
    char docroot[CF_MAXVARSIZE] = {0};
    EnterpriseDB conn[1];

    ReportingEngineAsyncError err = REPORTING_ENGINE_ASYNC_SUCCESS;

    if (!CFDB_Open(conn))
    {
        err = REPORTING_ENGINE_ASYNC_ERROR_ENTERPRISE_DB_CONNECT;

        return PackageAsyncQueryAbortResult(err, token);
    }

    if (!CFDB_HandleGetValue(cfr_mp_install_dir, docroot, CF_MAXVARSIZE - 1, NULL, conn, MONGO_SCRATCH))
    {
        err = REPORTING_ENGINE_ASYNC_ERROR_DOCROOT_NOT_FOUND;

        return PackageAsyncQueryAbortResult(err, token);
    }

    CFDB_Close(conn);

    char path_to_file[CF_MAXVARSIZE] = {0};
    snprintf(path_to_file, CF_MAXVARSIZE, "%s/api/static", docroot);

    wr_info = NewWebReportFileInfo(0, path_to_file, token, "");

    assert(wr_info);
    assert(wr_info->abort_file);

    FILE *fin = fopen(wr_info->abort_file, "w");
    if (fin == NULL)
    {
        return PackageAsyncQueryAbortResult(REPORTING_ENGINE_ASYNC_ERROR_IO, token);
    }

    fclose(fin);

    // TODO: check if the child has actually exit for robustness
    // status is not important here
    return PackageAsyncQueryAbortResult(err, token);
}

/******************************************************************/
