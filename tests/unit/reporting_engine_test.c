#include "platform.h"
#include "test.h"
#include "reporting-engine.c"
#include "cf.nova.h"

#if defined(HAVE_LIBSQLITE3)

static void test_get_table_names(void **state)
{
    const char *select_op_list[] = {
                                "SELECT * FROM Hosts",
                                "SELECT * FROM Hosts, Variables",
                                "SELECT * FROM Hosts, Variables, Software",
                                "",
                                "SELECT * FROM Hosts h, Variables v",
                                "SELECT * FROM PromiseStatusLast s, PromiseDefinitions d, Contexts c",
                                "SELECT Name FROM FileChanges f, Contexts s",
                                "SELECT f.FileName, COUNT(*) FROM FileChanges f, Contexts s WHERE f.HostKey=s.HostKey AND f.ChangeTimeStamp>0 AND s.Name='linux' GROUP BY f.FileName ORDER BY f.FileName",
                                "SELECT * FROM Hosts, VARIABLES",
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
                            NULL
                         };

    int column_count[] = {
                            5,  // hosts
                            3,  // filechanges
                            3,  // contexts
                            5,  // variables
                            4,  // software
                            4,  // promisestatuslast
                            4,  // promisedefinitions
                            0
                        };

    sqlite3 *db;

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
    const char *tables[] = {
                            SQL_TABLE_HOSTS,
                            SQL_TABLE_FILECHANGES,
                            SQL_TABLE_CONTEXTS,
                            SQL_TABLE_VARIABLES,
                            SQL_TABLE_SOFTWARE,
                            SQL_TABLE_PROMISESTATUS,
                            SQL_TABLE_PROMISEDEFINITIONS,
                            NULL
                         };

    const char *column_names[][5] = {
      {"HostKey", "Name", "IPAddress", "ReportTimeStamp", "Colour"},        // hosts
      {"HostKey", "FileName", "ChangeTimeStamp", NULL, NULL},               // filechanges
      {"HostKey", "Name", "DefineTimeStamp", NULL, NULL},                   // contexts
      {"HostKey", "Scope", "Name", "Value", "Type"},                        // variables
      {"HostKey", "Name", "Version", "Architecture", NULL},                   // software
      {"HostKey", "PromiseHandle", "PromiseStatus", "CheckTimeStamp", NULL},  // promisestatuslast
      {"PromiseHandle", "Promiser", "Bundle", "Promisee", NULL },             // promisedefinitions
      {NULL, NULL, NULL, NULL,NULL}
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

#endif

int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBSQLITE3)
        unit_test(test_get_table_names),
        unit_test(test_get_column_count)
        /* sqlite3_column_name segfaults in RHEL 5 builds,
           will comment this to allow for normal packaging to proceed
           Redmine #1025
        */
//        unit_test(test_validate_column_names)
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
