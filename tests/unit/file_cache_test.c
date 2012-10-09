#include "test.h"
#include "cf.nova.h"
#include "file-cache.h"

#include <utime.h>

static void test_new_destroy(void **state)
{
    FileCache *c = FileCacheNew("test-cache", "/tmp");
    FileCacheDestroy(c);
}

static void test_put_get(void **state)
{
    FileCache *c = FileCacheNew("test-cache", "/tmp");

    FileCachePut(c, "jersey", "abc", sizeof("abc"));

    time_t stamp = time(NULL);
    FileCacheEntry entry = FileCacheGet(c, "jersey");
    assert_in_range(entry.last_modified, stamp, time(NULL));
    assert_int_equal(sizeof("abc"), entry.value_size);
    assert_string_equal("abc", entry.value);
    FileCacheEntryDestroy(&entry);

    FileCacheDestroy(c);
}

static void test_remove_expired(void **state)
{
    FileCache *c = FileCacheNew("test-cache", "/tmp");

    FileCachePut(c, "jersey", "abc", sizeof("abc"));
    {
        struct utimbuf buf = { 0 };
        buf.modtime = 1;
        utime("/tmp/test-cache_jersey", &buf);
    }

    FileCachePut(c, "jersey2", "abc", sizeof("abc"));
    FileCacheRemoveExpired(c, 2);

    FileCacheEntry jersey = FileCacheGet(c, "jersey");
    assert_true(jersey.value == NULL);

    FileCacheEntry jersey2 = FileCacheGet(c, "jersey2");
    assert_true(jersey2.value != NULL);

    FileCacheEntryDestroy(&jersey2);

    FileCacheDestroy(c);
}


int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBMONGOC)
        unit_test(test_new_destroy),
        unit_test(test_put_get),
        unit_test(test_remove_expired),
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
