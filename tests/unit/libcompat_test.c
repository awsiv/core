#include <test.h>
#include <cmockery.h>
#include <string.h>

/* Include the tested functions directly from libcompat! */
#include <memmem.c>


/* TODO TEST MORE OF OUR libcompat REPLACEMENTS! */


static void test_memmem()
{
    char *result, *needle, *haystack;

    /* Can't find anything in nothing. */
    needle = "whatever";
    result = memmem("EMPTY", 0, needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* Even nothing is not in nothing. */
    result = memmem("EMPTY", 0, "EMPTY", 0);
    assert_int_equal(result, NULL);

    /* Nothing can be found in anything. */
    haystack = "blah";
    result = memmem(haystack, strlen(haystack), "EMPTY", 0);
    assert_int_equal(result, &haystack[0]);

    /* Find something when exact match. */
    haystack = "blah";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[0]);

    /* Find something when at the beginning. */
    haystack = "blah123";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[0]);

    /* Find something when in the middle. */
    haystack = "123blah123";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[3]);

    /* Find something when in the end. */
    haystack = "12345blah";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[5]);

    /* Partial match is not a match, part 1. */
    haystack = "12345bla";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* Partial match is not a match, part 2. */
    haystack = "bla";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* Partial match is not a match, part 3. */
    haystack = "bla123";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* All the right letters, in the right order, not contiguous. */
    haystack = "bleach";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* Should not be affected by case. */
    haystack = "BLAH";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, NULL);

    /* Don't jump forward too much on incomplete match. */
    haystack = "bblblablah";
    needle   = "blah";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[6]);

    /* Don't jump forward too much, part 2. */
    haystack = "abcabcabcd";
    needle   = "abcabcd";
    result = memmem(haystack, strlen(haystack), needle, strlen(needle));
    assert_int_equal(result, &haystack[3]);
}


int main()
{
    PRINT_TEST_BANNER();
    const UnitTest tests[] =
    {
        unit_test(test_memmem)
    };

    int ret = run_tests(tests);

    return ret;
}
