#include "platform.h"
#include "test.h"
#include "reporting-engine.h"


static void test_count_all_hosts(void **state)
{

}

int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_count_all_hosts)
    };

    return run_tests(tests);
}
