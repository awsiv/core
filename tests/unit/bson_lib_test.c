#include "platform.h"
#include "test.h"
#include "bson_lib.h"
#include <bson.h>
#include "json.h"
#include "writer.h"

#ifdef HAVE_LIBMONGOC

static void test_conversion_between_json_and_bson_simple(void **state)
{
    JsonElement *json_in = JsonObjectCreate(1);
    JsonObjectAppendString(json_in, "string", "my_string");
    JsonObjectAppendBool(json_in, "bool", true);
    JsonObjectAppendInteger(json_in, "int", 1234);

    Writer *writer_in = StringWriter();
    JsonElementPrint(writer_in, json_in, 1);

    bson buffer;
    if (!BsonInitFromJsonStringF(&buffer, "{'string': '%s', 'bool': %s, 'int': %d}",
                                 "my_string", "true", 1234))
    {
        assert_false(true);
    }

    JsonElement *json_out = NULL;
    json_out = BsonContainerToJsonContainer(&buffer, BSON_OBJECT, false);
    if (json_out == NULL)
    {
        assert_false(true);
    }

    Writer *writer_out = StringWriter();
    JsonElementPrint(writer_out, json_out, 1);

    assert_string_equal(StringWriterData(writer_out), StringWriterData(writer_in));

    StringWriterClose(writer_out);
    StringWriterClose(writer_in);

    JsonElementDestroy(json_in);
    JsonElementDestroy(json_out);

    bson_destroy(&buffer);
}

static void test_conversion_between_json_and_bson_object(void **state)
{
    JsonElement *json_subobj2 = JsonObjectCreate(3);
    JsonObjectAppendString(json_subobj2, "string2", "my_string2");
    JsonObjectAppendBool(json_subobj2, "bool2", true);
    JsonObjectAppendInteger(json_subobj2, "int2", 1234);

    JsonElement *json_subobj = JsonObjectCreate(4);
    JsonObjectAppendString(json_subobj, "string", "my_string");
    JsonObjectAppendBool(json_subobj, "bool", true);
    JsonObjectAppendInteger(json_subobj, "int", 1234);
    JsonObjectAppendObject(json_subobj, "subobj2", json_subobj2);

    JsonElement *json_in = JsonObjectCreate(2);
    JsonObjectAppendString(json_in, "string", "my_string");
    JsonObjectAppendObject(json_in, "subobj", json_subobj);

    Writer *writer_in = StringWriter();
    JsonElementPrint(writer_in, json_in, 1);

    bson buffer = {0};
    if (!BsonInitFromJsonStringF(&buffer, StringWriterData(writer_in)))
    {
        assert_false(true);
    }

    JsonElement *json_out = NULL;
    json_out = BsonContainerToJsonContainer(&buffer, BSON_OBJECT, false);
    if (json_out == NULL)
    {
        assert_false(true);
    }

    Writer *writer_out = StringWriter();
    JsonElementPrint(writer_out, json_out, 1);

    assert_string_equal(StringWriterData(writer_in), StringWriterData(writer_out));

    StringWriterClose(writer_out);
    StringWriterClose(writer_in);

    JsonElementDestroy(json_in);
    JsonElementDestroy(json_out);

    bson_destroy(&buffer);
}

static void test_conversion_between_json_and_bson_array_complex(void **state)
{
    JsonElement *json_in = JsonObjectCreate(1);
    JsonObjectAppendString(json_in, "string", "my_string");

    JsonElement *json_subobj1 = JsonObjectCreate(3);
    JsonObjectAppendString(json_subobj1, "string2", "my_string2");
    JsonObjectAppendBool(json_subobj1, "bool2", true);
    JsonObjectAppendInteger(json_subobj1, "int2", 1234);

    JsonElement *json_subobj2 = JsonObjectCreate(3);
    JsonObjectAppendString(json_subobj2, "string2", "my_string2");
    JsonObjectAppendBool(json_subobj2, "bool2", true);
    JsonObjectAppendInteger(json_subobj2, "int2", 1234);

    JsonElement *json_subarr = JsonArrayCreate(2);
    JsonArrayAppendObject(json_subarr, json_subobj1);
    JsonArrayAppendObject(json_subarr, json_subobj2);

    JsonObjectAppendArray(json_in, "array", json_subarr);
    Writer *writer_in = StringWriter();
    JsonElementPrint(writer_in, json_in, 1);

    bson buffer = {0};
    if (!BsonInitFromJsonStringF(&buffer, StringWriterData(writer_in)))
    {
        assert_false(true);
    }

    JsonElement *json_out = NULL;
    json_out = BsonContainerToJsonContainer(&buffer, BSON_OBJECT, false);
    if (json_out == NULL)
    {
        assert_false(true);
    }

    Writer *writer_out = StringWriter();
    JsonElementPrint(writer_out, json_out, 1);

    assert_string_equal(StringWriterData(writer_in), StringWriterData(writer_out));

    StringWriterClose(writer_in);
    StringWriterClose(writer_out);

    JsonElementDestroy(json_in);
    JsonElementDestroy(json_out);

    bson_destroy(&buffer);
}

static void test_conversion_between_json_and_bson_array_simple(void **state)
{
    JsonElement *json_in = JsonObjectCreate(1);
    JsonObjectAppendString(json_in, "string", "my_string");

    JsonElement *json_subarr = JsonArrayCreate(2);
    JsonArrayAppendString(json_subarr, "lalalala_1");
    JsonArrayAppendString(json_subarr, "lalalala_2");

    JsonObjectAppendArray(json_in, "array", json_subarr);
    Writer *writer_in = StringWriter();
    JsonElementPrint(writer_in, json_in, 1);

    bson buffer = {0};
    if (!BsonInitFromJsonStringF(&buffer, StringWriterData(writer_in)))
    {
        assert_false(true);
    }

    JsonElement *json_out = NULL;
    json_out = BsonContainerToJsonContainer(&buffer, BSON_OBJECT, false);
    if (json_out == NULL)
    {
        assert_false(true);
    }

    Writer *writer_out = StringWriter();
    JsonElementPrint(writer_out, json_out, 1);

    assert_string_equal(StringWriterData(writer_in), StringWriterData(writer_out));

    StringWriterClose(writer_in);
    StringWriterClose(writer_out);

    JsonElementDestroy(json_in);
    JsonElementDestroy(json_out);

    bson_destroy(&buffer);
}

#endif

int main()
{
    const UnitTest tests[] =
    {
#ifdef HAVE_LIBMONGOC
        unit_test(test_conversion_between_json_and_bson_simple),
        unit_test(test_conversion_between_json_and_bson_object),
        unit_test(test_conversion_between_json_and_bson_array_complex),
        unit_test(test_conversion_between_json_and_bson_array_simple),
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}

