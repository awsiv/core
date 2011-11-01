/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "benchmark.h"

#ifdef HAVE_LIBMONGOC


int Nova_BenchmarkReportStorage(char *reportFilePath, int iterationsToRun)
/**
 * Take a sample of the report we are simulating (reportFilePath) with
 * cf-report -x full on a representative host. We import this report
 * iterationsToRun times. Check if mongod drops down to 0% CPU quickly
 * after this function completes.
 *
 * NOTE: Since we are not seeding the random number generator with srand(),
 *       it will generate the same sequence of hostkeys on every run. This is
 *       realistic, because hosts are seldomly added in real-world scenarios
 *       (compared to all the 5-minute intervals).
 **/
{
 mongo_connection dbconn;

 if(!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
    {
    return false;
    }

 FILE *fin = fopen(reportFilePath, "r");
 
 if (fin == NULL)
   {
   CfOut(cf_error,"fopen","!! Cannot open import file %s", reportFilePath);
   CFDB_Close(&dbconn);
   return false;
   }

 int i;
 for(i = 0; i < iterationsToRun; i++)
    {
    // position fp after header -- we generate random digest in header
    fseek(fin, 0, SEEK_SET);
    char discardHeader[CF_MAXVARSIZE];
    CfReadLine(discardHeader, sizeof(discardHeader), fin);
    
    char randomString[EVP_MAX_MD_SIZE*4] = {0};
    unsigned char randomDigest[EVP_MAX_MD_SIZE+1] = {0};
    snprintf(randomString, sizeof(randomString), "%d", rand());
    HashString(randomString, sizeof(randomString), randomDigest, cf_sha256);
    
    char header[CF_MAXVARSIZE];
    snprintf(header,sizeof(header),"NOVA_EXPORT full %s 10.10.10.1 generated-hostname",
             HashPrint(cf_sha256, randomDigest));
    
    if(!Nova_ImportHostReportsFromStream(&dbconn, header, fin))
       {
       CfOut(cf_error, "", "!! Failed import");
       }
    
    CfOut(cf_cmdout, "", " -> Benchmark-host number %d\n", i);
    }

 fclose(fin);
 CFDB_Close(&dbconn);

 return 0;
}


#endif  /* HAVE_LIBMONGOC */

