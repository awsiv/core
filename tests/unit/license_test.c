#include "cf3.defs.h"
#include "cf3.extern.h"

#include <setjmp.h>
#include <cmockery.h>

int Nova_HashKey(char *filename, char *buffer, const char *hash);

char *pubkey_filename;

static const char *PUBLIC_KEY =
    "-----BEGIN RSA PUBLIC KEY-----\n"
    "MIIBCAKCAQEAvs74JCAjx12suPrPoHYX+4G29mtoKwCQ7ond1zoRDVPAfA5YJDwu\n"
    "Z7tqYEHesCuwM2Pzn19zwL1dtHlnN32si8JdRS/zBDztGaXD0cZwGaxol7Sa72bb\n"
    "t7ywi694ar5zJKOOrAWzca5q7v8R8AP31U2wMsFnfLTYiFfwBK0M6y8ZaFBVNMaQ\n"
    "Zkp+6kUYVqpIDoka8hJiuSlzaHDcn2qmjL49VFriiWbnOQ3mrbQikxfQA87+bFjc\n"
    "2HMEcp/BqxIbguAobdtq1zImnjUKSdcM/n4EORXbJygr/braKP9+o2bHo9QF34OA\n"
    "zefgEOBKjrW28Xcxc/U4YGvjX1/yF6EeQwIBIw==\n"
    "-----END RSA PUBLIC KEY-----\n";

static const char *EXPECTED_HASH =
    "SHA=20dbb20720e507ff521e84e12969ed99e2cffcf07213f4645e520d0574b4cbba";

/* FIXME: avoid creating files in this unit test */

void test_hashing(void **state)
{
    /* Expiration month, number of licenses (octal), expiration day, product,
     * expiration year, company */

    char header[] = "January-1777.1 Nova 3001 Test Company";

    assert_int_equal(Nova_HashKey(pubkey_filename, header, EXPECTED_HASH), 1);
}

void test_wrong_hashing(void **state)
{
    char header[] = "January-1777777.1 Nova 10001 Hackerz Co";

    unsigned char digest[EVP_MAX_MD_SIZE + 1];

    assert_int_equal(Nova_HashKey(pubkey_filename, header, EXPECTED_HASH), 0);
}

static void setup(void)
{
    OpenSSL_add_all_digests();

    pubkey_filename = tempnam(NULL, NULL);

    FILE *fh = fopen(pubkey_filename, "w");
    fwrite(PUBLIC_KEY, strlen(PUBLIC_KEY), 1, fh);
    fclose(fh);
}

static void teardown(void)
{
    unlink(pubkey_filename);
    free(pubkey_filename);
}

int main()
{
    setup();

    const UnitTest tests[] =
    {
        unit_test(test_hashing),
        unit_test(test_wrong_hashing),
    };

    int ret = run_tests(tests);

    teardown();

    return ret;
}
