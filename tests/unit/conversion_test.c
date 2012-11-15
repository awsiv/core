#include "test.h"
#include "conversion.h"
#include "matching.h"

char *SqlVariableExpandNumeric(const char *query, const char *key, long value);

static void test_bundle_qualified_name_split(void **state)
{
    char name[CF_MAXVARSIZE] = { 0 };
    char ns[CF_MAXVARSIZE] = { 0 };

    assert_false(BundleQualifiedNameSplit(NULL, ns, name));
    assert_false(BundleQualifiedNameSplit("", ns, name));

    assert_true(BundleQualifiedNameSplit("foo", ns, name));
    assert_string_equal("", ns);
    assert_string_equal("foo", name);

    assert_true(BundleQualifiedNameSplit("bar.foo", ns, name));
    assert_string_equal("bar", ns);
    assert_string_equal("foo", name);
}


static void test_sql_variable_expand(void **state)
{
    char *actual;
    char expected[1024];

    actual = SqlVariableExpand("SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < 60");
    assert_string_equal(actual, "SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < 60");
    free(actual);

    actual = SqlVariableExpand("SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < TIMESTAMP_UNIX() * 60");
    assert_string_not_equal(actual, "SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < TIMESTAMP_UNIX() * 60");
    free(actual);

    actual = SqlVariableExpand("SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < TIMESTAMP_UNIX_DAYS() * 60");
    snprintf(expected, sizeof(expected), "SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < %ld * 60", time(NULL) / SECONDS_PER_DAY);
    assert_string_equal(actual, expected);
    free(actual);

    actual = SqlVariableExpandNumeric("SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < 60", "TIMESTAMP_UNIX()", 123);
    assert_string_equal(actual, "SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < 60");
    free(actual);

    actual = SqlVariableExpandNumeric("SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < TIMESTAMP_UNIX()", "TIMESTAMP_UNIX()", 123);
    assert_string_equal(actual, "SELECT HostName,ReportTimeStamp FROM Hosts WHERE ReportTimeStamp < 123");
    free(actual);

    actual = SqlVariableExpandNumeric("SELECT FileName, COUNT(1) FROM FileChanges WHERE ChangeTimeStamp > (TIMESTAMP_UNIX() - 60*60*24 + TIMESTAMP_UNIX()) GROUP BY FileName", "TIMESTAMP_UNIX()", 1348870622);
    assert_string_equal(actual, "SELECT FileName, COUNT(1) FROM FileChanges WHERE ChangeTimeStamp > (1348870622 - 60*60*24 + 1348870622) GROUP BY FileName");
    free(actual);
}


int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_bundle_qualified_name_split),
        unit_test(test_sql_variable_expand)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}


