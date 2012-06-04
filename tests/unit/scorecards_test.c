#include "platform.h"
#include "test.h"
#include "scorecards.h"

#ifdef HAVE_LIBMONGOC
static void test_host_compliance_score(void **state)
{
    int score = HostComplianceScore(100, 0);
    assert_int_equal(HostColourFromScoreForConnectedHost(score), HOST_COLOUR_GREEN);

    score = HostComplianceScore(81, 19);
    assert_int_equal(HostColourFromScoreForConnectedHost(score), HOST_COLOUR_GREEN);

    score = HostComplianceScore(79, 21);
    assert_int_equal(HostColourFromScoreForConnectedHost(score), HOST_COLOUR_YELLOW);

    score = HostComplianceScore(60, 19);
    assert_int_equal(HostColourFromScoreForConnectedHost(score), HOST_COLOUR_RED);

    score = HostComplianceScore(79, 0);
    assert_int_equal(HostColourFromScoreForConnectedHost(score), HOST_COLOUR_RED);
}

static void test_colour_from_score(void **state)
{
    assert_int_equal(HOST_COLOUR_BLUE, HostColourFromScore(10, 7, 2, 500, false));
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
#endif

int main()
{
    const UnitTest tests[] =
    {
#ifdef HAVE_LIBMONGOC
        unit_test(test_host_compliance_score),
        unit_test(test_colour_from_score),
        unit_test(test_colour_from_score_for_connected_host)
#endif
    };

    return run_tests(tests);
}

