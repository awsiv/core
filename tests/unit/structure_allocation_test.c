#include "platform.h"
#include "test.h"
#include "json.h"
#include "install.h"

/*
 * Segfaults and leaks (valgrind) are the fault lines here.
 */

#ifdef HAVE_LIBMONGOC

static void test_json_header_table(void **state)
{
    JsonElement *header = JsonArrayCreate(5);
    JsonArrayAppendString(header, "First column");
    JsonArrayAppendString(header, "Second column");
    JsonArrayAppendString(header, "Third column");
    JsonArrayAppendString(header, "Fourth column");
    JsonArrayAppendString(header, "Fifth column");


    JsonElement *data = JsonArrayCreate(2);

    JsonElement *row1 = JsonArrayCreate(5);
    JsonArrayAppendString(row1, "11");
    JsonArrayAppendString(row1, "12");
    JsonArrayAppendString(row1, "13");
    JsonArrayAppendString(row1, "14");
    JsonArrayAppendString(row1, "15");
    JsonArrayAppendArray(data, row1);

    JsonElement *row2 = JsonArrayCreate(5);
    JsonArrayAppendString(row1, "21");
    JsonArrayAppendString(row1, "22");
    JsonArrayAppendString(row1, "23");
    JsonArrayAppendString(row1, "24");
    JsonArrayAppendString(row1, "25");
    JsonArrayAppendArray(data, row2);

    JsonHeaderTable *table = NewJsonHeaderTable("Some title", header, data);
    DeleteJsonHeaderTable(table);
}

#endif  /* HAVE_LIBMONGOC */

int main()
{
    const UnitTest tests[] =
    {
#ifdef HAVE_LIBMONGOC
        unit_test(test_json_header_table),
#endif
    };

    return run_tests(tests);
}
