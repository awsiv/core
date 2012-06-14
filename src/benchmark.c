/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "benchmark.h"

#include "db_save.h"

int Nova_BenchmarkReportStorage(char *reportFilePath, int iterationsToRun)
/**
 * Take a sample of the report we are simulating (reportFilePath) with
 * cf-report -x full on a representative host. We import this report
 * iterationsToRun times. Check if mongod drops down to 0% CPU quickly
 * after this function completes.
 *
 * NOTE: The cryptographic functions needs to be initialised before calling
 *       this function. This can be done by running the agent initialisation
 *       functions.
 **/
{
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    FILE *fin = fopen(reportFilePath, "r");

    if (fin == NULL)
    {
        CfOut(cf_error, "fopen", "!! Cannot open import file %s", reportFilePath);
        CFDB_Close(&dbconn);
        return false;
    }

    int i;

    for (i = 0; i < iterationsToRun; i++)
    {
        // position fp after header -- we generate random digest in header
        fseek(fin, 0, SEEK_SET);
        char discardHeader[CF_MAXVARSIZE];

        CfReadLine(discardHeader, sizeof(discardHeader), fin);

        int randomNumber = rand();
        char randomString[EVP_MAX_MD_SIZE * 4] = { 0 };
        unsigned char randomDigest[EVP_MAX_MD_SIZE + 1] = { 0 };
        snprintf(randomString, sizeof(randomString), "%d", randomNumber);
        HashString(randomString, sizeof(randomString), randomDigest, cf_sha256);

        char header[CF_MAXVARSIZE];

        snprintf(header, sizeof(header), "NOVA_EXPORT full %s 10.10.10.1 generated-hostname-%d",
                 HashPrint(cf_sha256, randomDigest), randomNumber);

        if (!Nova_ImportHostReportsFromStream(&dbconn, header, fin))
        {
            CfOut(cf_error, "", "!! Failed import");
        }

        CfOut(cf_cmdout, "", " -> Benchmark-host number %d\n", i);
    }

    fclose(fin);
    CFDB_Close(&dbconn);

    return 0;
}
