#include "platform.h"
#include "test.h"
#include "scorecards.h"
#include "matching.h"

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

static void test_check_key_in_context_all(void **state)
{
    assert_true(IsKeyWithinPromiseContext(NULL, "handle_123", PROMISE_CONTEXT_MODE_ALL));
    assert_true(IsKeyWithinPromiseContext("cfe_system", "handle_123", PROMISE_CONTEXT_MODE_ALL));
    assert_true(IsKeyWithinPromiseContext("cfe_sysem", "cfe_internal_handle", PROMISE_CONTEXT_MODE_ALL));
    assert_true(IsKeyWithinPromiseContext(NULL, "cfe_internal_handle", PROMISE_CONTEXT_MODE_ALL));
    assert_true(IsKeyWithinPromiseContext("ns1", "cfe_internal_handle", PROMISE_CONTEXT_MODE_ALL));
}

static void test_check_key_in_context_user(void **state)
{
    assert_true(IsKeyWithinPromiseContext(NULL, "handle_123", PROMISE_CONTEXT_MODE_USER));
    assert_false(IsKeyWithinPromiseContext("cfe_system", "handle_123", PROMISE_CONTEXT_MODE_USER));
    assert_false(IsKeyWithinPromiseContext("cfe_system", "cfe_internal_handle", PROMISE_CONTEXT_MODE_USER));
    assert_false(IsKeyWithinPromiseContext(NULL, "cfe_internal_handle", PROMISE_CONTEXT_MODE_USER));
    assert_true(IsKeyWithinPromiseContext("ns1", "cfe_internal_handle", PROMISE_CONTEXT_MODE_USER));
}

static void test_check_key_in_context_internal(void **state)
{
    assert_false(IsKeyWithinPromiseContext(NULL, "handle_123", PROMISE_CONTEXT_MODE_INTERNAL));
    assert_true(IsKeyWithinPromiseContext("cfe_system", "handle_123", PROMISE_CONTEXT_MODE_INTERNAL));
    assert_true(IsKeyWithinPromiseContext("cfe_system", "cfe_internal_handle", PROMISE_CONTEXT_MODE_INTERNAL));
    assert_true(IsKeyWithinPromiseContext(NULL, "cfe_internal_handle", PROMISE_CONTEXT_MODE_INTERNAL));
    assert_false(IsKeyWithinPromiseContext("ns1", "cfe_internal_handle", PROMISE_CONTEXT_MODE_INTERNAL));
}

static void test_check_handle_in_context_all(void **state)
{
    assert_true(IsHandleWithinPromiseContext("handle_123", PROMISE_CONTEXT_MODE_ALL));
    assert_true(IsHandleWithinPromiseContext("cfe_internal_handle", PROMISE_CONTEXT_MODE_ALL));
}

static void test_check_handle_in_context_user(void **state)
{
    assert_true(IsHandleWithinPromiseContext("handle_123", PROMISE_CONTEXT_MODE_USER));
    assert_false(IsHandleWithinPromiseContext("cfe_internal_handle", PROMISE_CONTEXT_MODE_USER));
}

static void test_check_handle_in_context_internal(void **state)
{
    assert_false(IsHandleWithinPromiseContext("handle_123", PROMISE_CONTEXT_MODE_INTERNAL));
    assert_true(IsHandleWithinPromiseContext("cfe_internal_handle", PROMISE_CONTEXT_MODE_INTERNAL));
}

#endif

int main()
{
    const UnitTest tests[] =
    {
#ifdef HAVE_LIBMONGOC
        unit_test(test_host_compliance_score),
        unit_test(test_colour_from_score),
        unit_test(test_colour_from_score_for_connected_host),
        unit_test(test_check_key_in_context_all),
        unit_test(test_check_key_in_context_internal),
        unit_test(test_check_key_in_context_user),
        unit_test(test_check_handle_in_context_all),
        unit_test(test_check_handle_in_context_user),
        unit_test(test_check_handle_in_context_internal)
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}

