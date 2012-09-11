#include "cf3.defs.h"
#include "cf3.extern.h"
#include "license.h"
#include "test.h"

#include <setjmp.h>
#include <cmockery.h>

int Nova_HashKey(char *filename, char *buffer, const char *hash);

char *pubkey_filename;
char *license_filename;


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

static const char *LICENSE =
    "15 32 December 2012 SHA=9aeaf908d4d72da59103052cd6e7dd1e5321ae498563f79a49cf8b3be652406c Tester 123\n"
    "265e5eb7abf2bb45a4e82b1753a95d047815f8ad2966d2cc8d205a027c50ba7d";

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

void test_license_file_parse(void **state)
{
    EnterpriseLicense license;

    bool result = LicenseFileParse(&license, license_filename);
    assert_int_equal(result, true);

    assert_int_equal(license.expiry_day, 15);
    assert_string_equal(license.expiry_month, "December");
    assert_int_equal(license.expiry_year, 2012);
    assert_int_equal(license.count, 50);
    assert_string_equal(license.company_name, "Tester 123");
    assert_string_equal(license.digest, "SHA=9aeaf908d4d72da59103052cd6e7dd1e5321ae498563f79a49cf8b3be652406c");
    assert_string_equal(license.public_key_digest, "265e5eb7abf2bb45a4e82b1753a95d047815f8ad2966d2cc8d205a027c50ba7d");

    time_t now = time(NULL);
    assert_true(now - 10 < license.install_timestamp);
    assert_true(now >= license.install_timestamp);
}

static void setup(void)
{
    OpenSSL_add_all_digests();

    pubkey_filename = tempnam(NULL, NULL);
    license_filename = tempnam(NULL, NULL);

    FILE *fh = fopen(pubkey_filename, "w");
    fwrite(PUBLIC_KEY, strlen(PUBLIC_KEY), 1, fh);
    fclose(fh);

    FILE *fh_license = fopen(license_filename, "w");
    fwrite(LICENSE, strlen(LICENSE), 1, fh_license);
    fclose(fh_license);
}

static void teardown(void)
{
    unlink(pubkey_filename);
    free(pubkey_filename);

    unlink(license_filename);
    free(license_filename);
}

int main()
{
    setup();

    const UnitTest tests[] =
    {
        unit_test(test_hashing),
        unit_test(test_wrong_hashing),
        unit_test(test_license_file_parse)
    };

    PRINT_TEST_BANNER();
    int ret = run_tests(tests);

    teardown();

    return ret;
}
