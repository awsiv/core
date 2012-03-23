#include "cf.defs.h"
#include "test.h"
#include "scorecards.h"

static void test_colour_from_score(void **state)
{
    assert_int_equal(HOST_COLOUR_BLUE, HostColourFromScore(10, 7, 2, 500, false));
    assert_int_equal(HOST_COLOUR_BLUE, HostColourFromScore(10, 7, 5, -1, false));
    assert_int_equal(HOST_COLOUR_BLUE, HostColourFromScore(10, 7, 5, 0, false));
    assert_int_equal(HOST_COLOUR_BLUE, HostColourFromScore(10, 7, 2, 300, true));

    assert_int_equal(HOST_COLOUR_GREEN, HostColourFromScore(10, 7, 5, 50, false));
    assert_int_equal(HOST_COLOUR_YELLOW, HostColourFromScore(10, 7, 5, 200, false));
    assert_int_equal(HOST_COLOUR_RED, HostColourFromScore(10, 7, 5, 1100, false));

    assert_int_equal(HOST_COLOUR_BLACK, HostColourFromScore(10, 7, 5, 50, true));
}

static void test_colour_from_score_for_connected_host(void **state)
{
    assert_int_equal(HOST_COLOUR_RED, HostColourFromScoreForConnectedHost(1000));
    assert_int_equal(HOST_COLOUR_YELLOW, HostColourFromScoreForConnectedHost(100));
    assert_int_equal(HOST_COLOUR_GREEN, HostColourFromScoreForConnectedHost(10));
}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_colour_from_score),
        unit_test(test_colour_from_score_for_connected_host)
    };

    return run_tests(tests);
}

