#include "test.h"
#include "conversion.h"

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


int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_bundle_qualified_name_split)
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}


