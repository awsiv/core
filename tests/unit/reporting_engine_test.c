#include "platform.h"
#include "test.h"
#include "reporting_engine.c"
#include "cf.nova.h"

#include "matching.h"
#include "set.h"

// TODO: add tests to check multiple table names in a query

static void test_get_table_names_single(void **state)
{
    const char *select_op_list[] = {
                                "SELECT * FROM Hosts",
                                "SELECT * FROM FileChanges",
                                "SELECT * FROM Contexts",
                                "SELECT * FROM Variables",
                                "SELECT * FROM Software",
                                "SELECT * FROM PromiseStatusLast",
                                "SELECT * FROM PromiseDefinitions",
                                "SELECT * FROM PromiseLogs;",
                                "SELECT * FROM BundleStatus;",
                                "SELECT * FROM Benchmarks;",
                                "SELECT * FROM LastSeenHosts;",
                                "SELECT * FROM PolicyStatus;",
                                "SELECT * FROM SoftwareUpdates;",
                                "SELECT * FROM DatabaseServerStatus;",
                                "SELECT * FROM DatabaseStatus;",
                                "SELECT * FROM DatabaseCollectionStatus;",
                                NULL
                             };

    // Table names require to be in reverse order as in TABLES global list
    const char *table_list[] = {
        "Hosts",
        "FileChanges",
        "Contexts",
        "Variables",
        "Software",
        "PromiseStatusLast",
        "PromiseDefinitions",
        "PromiseLogs",
        "BundleStatus",
        "Benchmarks",
        "LastSeenHosts",
        "PolicyStatus",
        "SoftwareUpdates",
        "DatabaseServerStatus",
        "DatabaseStatus",
        "DatabaseCollectionStatus",
        NULL
    };

    for (int i = 0; select_op_list[i] != NULL; i++)
    {
        Set *tables = SetNew((MapHashFn)OatHash, (MapKeyEqualFn)StringSafeEqual, free);

        assert_true((GetTableNamesInQuery(select_op_list[i], tables)));

        assert_true(SetContains(tables, table_list[i]));

        // Also make sure that the Set contains only one element
        int count = 0;
        SetIterator it = SetIteratorInit(tables);
        while (SetIteratorNext(&it))
        {
            count++;
        }

        SetDestroy(tables);

        assert_int_equal(count, 1);
        tables = NULL;
    }    
}

static void test_get_column_count(void **state)
{
    const char *tables[SQL_TABLE_COUNT] =
    {
        SQL_TABLE_HOSTS,
        SQL_TABLE_CONTEXTS,
        SQL_TABLE_FILECHANGES,
        SQL_TABLE_VARIABLES,
        SQL_TABLE_SOFTWARE,
        SQL_TABLE_PROMISESTATUS_LAST,
        SQL_TABLE_PROMISEDEFINITIONS,
        SQL_TABLE_PROMISELOGS,
        SQL_TABLE_BUNDLESTATUS,
        SQL_TABLE_BENCHMARKS,
        SQL_TABLE_LASTSEEN_HOSTS,
        SQL_TABLE_POLICYSTATUS,
        SQL_TABLE_SOFTWARE_UPDATES,
        SQL_TABLE_DIAGNOSTIC_SERVER_STATUS,
        SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS,
        SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS,
        NULL,
        NULL,
        NULL
    };

    int column_count[] = {
                            4,  // hosts
                            3,  // contexts
                            7,  // filechanges
                            6,  // variables
                            4,  // software
                            4,  // promisestatuslast
                            5,  // promisedefinitions
                            5,  //promiselog
                            5,  // bundle status
                            4,  // benchmarks
                            5,  // lastseen
                            6,  // totalcompliance
                            5,  // patch
                            16, // serverstatus
                            6,  // databasestatus
                            10, // collectionstatus
                            0
                        };

    sqlite3 *db = NULL;

    assert(Sqlite3_DBOpen(&db));

    assert(GenerateAllTables(db));

    for (int i = 0; tables[i] != NULL; i++)
    {
        sqlite3_stmt *statement;
        char select_op[CF_BUFSIZE] = {0};

        snprintf(select_op, CF_BUFSIZE - 1, "SELECT * from %s;", tables[i]);

        assert( sqlite3_prepare_v2(db, select_op, -1, &statement, 0) == SQLITE_OK);
        
        assert((sqlite3_column_count(statement) == column_count[i]) && "This means that the table columns have been altered from what it was originally");

        assert(sqlite3_finalize(statement) == SQLITE_OK);
    }

    Sqlite3_DBClose(db);
}

static void test_validate_column_names(void **state)
{
    EnterpriseDB conn[1];

    if (!CFDB_Open(conn))
    {
        printf("Connection to mongod failed - couldn't proceed with testing\n");
        return;
    }

    CFDB_Close(conn);

    const char *tables[SQL_TABLE_COUNT] =
    {
        SQL_TABLE_HOSTS,
        SQL_TABLE_FILECHANGES,
        SQL_TABLE_CONTEXTS,
        SQL_TABLE_VARIABLES,
        SQL_TABLE_SOFTWARE,
        SQL_TABLE_PROMISESTATUS_LAST,
        SQL_TABLE_PROMISEDEFINITIONS,
        SQL_TABLE_PROMISELOGS,
        SQL_TABLE_BUNDLESTATUS,
        SQL_TABLE_BENCHMARKS,
        SQL_TABLE_LASTSEEN_HOSTS,
        SQL_TABLE_POLICYSTATUS,
        SQL_TABLE_SOFTWARE_UPDATES,
        SQL_TABLE_DIAGNOSTIC_SERVER_STATUS,
        SQL_TABLE_DIAGNOSTIC_DATABASE_STATUS,
        SQL_TABLE_DIAGNOSTIC_COLLECTION_STATUS,
        NULL
    };

    const char *column_names[][16] = {
        {"HostKey", "HostName", "IPAddress", "ReportTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},        // hosts
        {"HostKey", "PromiseHandle", "FileName", "ChangeTimeStamp", "ChangeType", "LineNumber", "ChangeDetails", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},               // filechanges
        {"HostKey", "ContextName", "DefineTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},                   // contexts
        {"HostKey", "NameSpace", "Bundle", "VariableName", "VariableValue", "VariableType", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},                        // variables
        {"HostKey", "SoftwareName", "SoftwareVersion", "SoftwareArchitecture", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},                   // software
        {"HostKey", "PromiseHandle", "PromiseStatus", "CheckTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},  // promisestatuslast
        {"NameSpace", "PromiseHandle", "Promiser", "Bundle", "Promisee", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},             // promisedefinitions
        {"HostKey", "PromiseHandle", "PromiseStatus", "PromiseStatusReport", "Time", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"HostKey", "NameSpace", "Bundle", "PercentageCompliance", "CheckTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, //Bundlestatus
        {"HostKey", "EventName", "TimeTaken", "CheckTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"HostKey", "LastSeenDirection", "RemoteHostKey", "LastSeenAt", "LastSeenInterval", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"HostKey", "PolicyName", "TotalKept", "TotalRepaired", "TotalNotKept", "CheckTimeStamp", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"HostKey", "PatchReportType", "PatchName", "PatchVersion", "PatchArchitecture", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"SampleTime", "Host", "Version", "Uptime", "GlobalLockTotalTime", "GlobalLockTime", "GlobalLockQuereTotal", "GlobalLockQuereReaders", "GlobalLockQuereWriters", "MemoryResident", "MemoryVirtual", "MemoryMapped", "BackgroundFlushCount", "BackgroundFlushTotalTime", "BackgroundFlushAverageTime", "BackgroundFlushLastTime"},
        {"SampleTime", "Database", "AverageObjectSize", "DataSize", "StorageSize", "FileSize", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"SampleTime", "Database", "Collection", "ObjectCount", "DataSize", "AverageObjectSize", "StorageSize", "IndexCount", "TotalIndexSize", "PaddingFactor", NULL, NULL, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    };

    sqlite3 *db;

    assert(Sqlite3_DBOpen(&db));

    assert(GenerateAllTables(db));

    for (int i = 0; tables[i] != NULL; i++)
    {
        char select_op[CF_BUFSIZE] = {0};
        snprintf(select_op, CF_BUFSIZE - 1, "SELECT * from %s;", tables[i]);

        sqlite3_stmt *statement;

        assert(sqlite3_prepare_v2(db, select_op, -1, &statement, 0) == SQLITE_OK);

        int column_count = sqlite3_column_count(statement);
        assert(column_count > 0);

        for (int j = 0; j < column_count; j++)
        {
            // This means that the column names have been altered from what it was originally
            assert_string_equal(column_names[i][j], sqlite3_column_name(statement, j));
        }

        assert(sqlite3_finalize(statement) == SQLITE_OK);
    }

    Sqlite3_DBClose(db);
}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_get_table_names_single),
        unit_test(test_get_column_count),
        unit_test(test_validate_column_names)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
