#include "platform.h"
#include "test.h"
#include "reporting-engine.c"
#include "cf.nova.h"

static void test_get_table_names(void **state)
{
    char *select_op_list[] = {
                                "SELECT * FROM Hosts",
                                "SELECT * FROM Hosts, Variables",
                                "SELECT * FROM Hosts, Variables, Software",
                                "",
                                "SELECT * FROM Hosts h, Variables v",
                                "SELECT * FROM PromiseStatusLast s, PromiseDefinitions d, Contexts c",
                                "SELECT Name FROM FileChanges f, Contexts s",
                                "SELECT f.FileName, COUNT(*) FROM FileChanges f, Contexts s WHERE f.HostKey=s.HostKey AND f.ChangeTimeStamp>0 AND s.Name='linux' GROUP BY f.FileName ORDER BY f.FileName",
                                NULL
                             };

    char *table_list[] = {
                            "{'Hosts'}",
                            "{'Variables','Hosts'}",
                            "{'Software','Variables','Hosts'}",
                            "{}",
                            "{'Variables','Hosts'}",
                            "{'Contexts','PromiseDefinitions','PromiseStatusLast'}",
                            "{'Contexts','FileChanges'}",
                            "{'Contexts','FileChanges'}",
                            NULL
                         };

    for (int i = 0; select_op_list[i] != NULL; i++)
    {
        Rlist *tables = NULL;
        tables = GetTableNamesInQuery(select_op_list[i]);

        char buff[CF_MAXVARSIZE] = {0};
        PrintRlist(buff, CF_MAXVARSIZE, tables);
printf("%s\n", buff);
        assert_string_equal(buff, table_list[i]);

        DeleteRlist(tables);
    }

    assert_int_equal(1,1);
}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_get_table_names)
    };

    return run_tests(tests);
}
