#include "test.h"
#include "cf.nova.h"
#include "hub-worker.h"
#include "item_lib.h"
#include "lastseen.h"

#include <assert.h>

char CFWORKDIR[CF_BUFSIZE];

void UpdateLastSawHost(const char *hostkey, const char *address,
                       bool incoming, time_t timestamp);

static void tests_setup(void)
{
    snprintf(CFWORKDIR, CF_BUFSIZE, "/tmp/lastseen_test.XXXXXX");
    mkdtemp(CFWORKDIR);
}

static void setup(void)
{
    char cmd[CF_BUFSIZE];
    snprintf(cmd, CF_BUFSIZE, "rm -rf '%s'/*", CFWORKDIR);
    system(cmd);
}

static void tests_teardown(void)
{
    char cmd[CF_BUFSIZE];
    snprintf(cmd, CF_BUFSIZE, "rm -rf '%s'", CFWORKDIR);
    system(cmd);
}

static void test_scan_clients_tcdb(void **state)
{
    setup();

    LICENSES = 25;

    char keyhash[CF_MAXVARSIZE] = {0};
    char ip[CF_MAXVARSIZE] = {0};

    for (int i = 0; i < LICENSES; i++)
    {
        snprintf(keyhash, CF_MAXVARSIZE, "SHA-testhost%d", i + 1);
        snprintf(ip, CF_MAXVARSIZE, "127.0.0.%d", i + 1);

        UpdateLastSawHost(keyhash, ip, true, 666);
    }

    Item *list = Nova_ScanClients();

    int count = ListLen(list);

    assert(count == LICENSES);

    DeleteItemList(list);
}

int main()
{
    tests_setup();

    const UnitTest tests[] =
    {
        unit_test(test_scan_clients_tcdb)
    };

    PRINT_TEST_BANNER();
    int ret = run_tests(tests);

    tests_teardown();

    return ret;
}

// stubs

void Nova_HubLog(const char *s, ...)
{
}
