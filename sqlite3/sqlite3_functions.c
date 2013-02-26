#include <config.h>
#include "sqlite3_functions.h"
#include "bool.h"
#include "string_lib.h"

void RegExp(sqlite3_context *ctx, int argc, sqlite3_value **argv)
{
    if (argc != 2)
    {
        sqlite3_result_error(ctx, "Incorrect argument count for SQL regexp()", -1);
        return;
    }

    const unsigned char *regex = sqlite3_value_text(argv[0]);

    if (regex == NULL)
    {
        sqlite3_result_error(ctx, "Please provide a regular expression for SQL regexp()", -1);
        return;
    }

    const unsigned char *text = sqlite3_value_text(argv[1]);
    if (text == NULL)
    {
        return;
    }

    sqlite3_result_int(ctx, StringMatchFull(regex, text));
    return;
}
