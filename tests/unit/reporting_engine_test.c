#include "platform.h"
#include "test.h"
#include "reporting_engine.c"
#include "cf.nova.h"

#include "matching.h"

static void test_get_table_names(void **state)
{
    const char *select_op_list[] = {
                                "SELECT * FROM Hosts",
                                "SELECT * FROM Hosts, Variables",
                                "SELECT * FROM Hosts, Variables, Software",
                                "",
                                "SELECT * FROM Hosts h, Variables v",
                                "SELECT * FROM PromiseStatusLast s, PromiseDefinitions d, Contexts c",
                                "SELECT ContextName FROM FileChanges f, Contexts s",
                                "SELECT f.FileName, COUNT(*) FROM FileChanges f, Contexts s WHERE f.HostKey=s.HostKey AND f.ChangeTimeStamp>0 AND s.ContextName='linux' GROUP BY f.FileName ORDER BY f.FileName",
                                "SELECT * FROM Hosts, VARIABLES",
                                "SELECT * FROM PromiseLog;",
                                "SELECT * FROM PromiseSummary;",
                                "SELECT * FROM BundleStatus;",
                                "SELECT * FROM Benchmarks;",
                                "SELECT * FROM LastSeen;",
                                "SELECT * FROM TotalCompliance;",
                                "SELECT * FROM Patch;",
                                "SELECT * FROM FileDiffs;",
                                NULL
                             };

    // Table names require to be in reverse order as in TABLES global list
    const char *table_list[] = {
                            "{'Hosts'}",
                            "{'Variables','Hosts'}",
                            "{'Software','Variables','Hosts'}",
                            "{}",
                            "{'Variables','Hosts'}",
                            "{'PromiseDefinitions','PromiseStatusLast','Contexts'}",
                            "{'FileChanges','Contexts'}",
                            "{'FileChanges','Contexts'}",
                            "{'Variables','Hosts'}",
                            "{'PromiseLog'}",
                            "{'PromiseSummary'}",
                            "{'BundleStatus'}",
                            "{'Benchmarks'}",
                            "{'LastSeen'}",
                            "{'TotalCompliance'}",
                            "{'Patch'}",
                            "{'FileDiffs'}",
                            NULL
                         };

    for (int i = 0; select_op_list[i] != NULL; i++)
    {
        Rlist *tables = NULL;
        tables = GetTableNamesInQuery(select_op_list[i]);

        char buff[CF_MAXVARSIZE] = {0};
        PrintRlist(buff, CF_MAXVARSIZE, tables);

        assert_string_equal(buff, table_list[i]);

        DeleteRlist(tables);
    }

    assert_int_equal(1,1);
}

static void test_get_column_count(void **state)
{
    const char *tables[] = {
                            SQL_TABLE_HOSTS,
                            SQL_TABLE_FILECHANGES,
                            SQL_TABLE_CONTEXTS,
                            SQL_TABLE_VARIABLES,
                            SQL_TABLE_SOFTWARE,
                            SQL_TABLE_PROMISESTATUS,
                            SQL_TABLE_PROMISEDEFINITIONS,
                            SQL_TABLE_PROMISELOGS,
                            SQL_TABLE_PROMISE_SUMMARY,
                            SQL_TABLE_BUNDLESTATUS,
                            SQL_TABLE_BENCHMARKS,
                            SQL_TABLE_LASTSEEN,
                            SQL_TABLE_TOTALCOMPLIANCE,
                            SQL_TABLE_PATCH,
                            SQL_TABLE_FILEDIFFS,
                            NULL
                         };

    int column_count[] = {
                            4,  // hosts
                            3,  // filechanges
                            3,  // contexts
                            6,  // variables
                            4,  // software
                            4,  // promisestatuslast
                            5,  // promisedefinitions
                            5,  //promiselog
                            4,  // promise summary
                            4,  // bundle status
                            4,  // benchmarks
                            5,  // lastseen
                            6,  // totalcompliance
                            5,  // patch
                            7,  // filediffs
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

    const char *tables[] = {
                            SQL_TABLE_HOSTS,
                            SQL_TABLE_FILECHANGES,
                            SQL_TABLE_CONTEXTS,
                            SQL_TABLE_VARIABLES,
                            SQL_TABLE_SOFTWARE,
                            SQL_TABLE_PROMISESTATUS,
                            SQL_TABLE_PROMISEDEFINITIONS,
                            SQL_TABLE_PROMISELOGS,
                            SQL_TABLE_PROMISE_SUMMARY,
                            SQL_TABLE_BUNDLESTATUS,
                            SQL_TABLE_BENCHMARKS,
                            SQL_TABLE_LASTSEEN,
                            SQL_TABLE_TOTALCOMPLIANCE,
                            SQL_TABLE_PATCH,
                            SQL_TABLE_FILEDIFFS,
                            NULL
                         };

    const char *column_names[][7] = {
        {"HostKey", "HostName", "IPAddress", "ReportTimeStamp", NULL, NULL, NULL},        // hosts
        {"HostKey", "FileName", "ChangeTimeStamp", NULL, NULL, NULL, NULL},               // filechanges
        {"HostKey", "ContextName", "DefineTimeStamp", NULL, NULL, NULL, NULL},                   // contexts
        {"HostKey", "NameSpace", "Bundle", "VariableName", "VariableValue", "VariableType", NULL},                        // variables
        {"HostKey", "SoftwareName", "SoftwareVersion", "SoftwareArchitecture", NULL, NULL, NULL},                   // software
        {"HostKey", "PromiseHandle", "PromiseStatus", "CheckTimeStamp", NULL, NULL, NULL},  // promisestatuslast
        {"NameSpace", "PromiseHandle", "Promiser", "Bundle", "Promisee", NULL, NULL},             // promisedefinitions
        {"HostKey", "PromiseHandle", "PromiseLogType", "PromiseLogReport", "Time", NULL, NULL},
        {"PromiseHandle", "PromiseLogType", "PromiseLogReport", "Occurrences", NULL, NULL, NULL},
        {"HostKey", "Bundle", "PercentageCompliance", "CheckTimeStamp", NULL, NULL, NULL},
        {"HostKey", "EventName", "TimeTaken", "CheckTimeStamp", NULL, NULL, NULL},
        {"HostKey", "LastSeenDirection", "RemoteHostKey", "LastSeenAt", "LastSeenInterval", NULL, NULL},
        {"HostKey", "PolicyVersion", "TotalKept", "TotalRepaired", "TotalNotKept", "CheckTimeStamp", NULL},
        {"HostKey", "PatchReportType", "PatchName", "PatchVersion", "PatchArchitecture", NULL, NULL},
        {"HostKey", "PromiseHandle", "FileName", "ChangeTimeStamp", "ChangeType", "LineNumber", "ChangeDetails"},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL}
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
        unit_test(test_get_table_names),
        unit_test(test_get_column_count),
        unit_test(test_validate_column_names)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
