
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: license.c                                                           */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include <assert.h>

#include "env_context.h"
#include "promises.h"
#include "lastseen.h"
#include "crypto.h"
#include "files_names.h"
#include "vars.h"
#include "conversion.h"
#include "files_lib.h"
#include "license.h"

#ifdef HAVE_LIBMONGOC
#include "db_save.h"
#include "db_query.h"
#endif

#define INTERNAL_EXPIRY_DAY "25"
#define INTERNAL_EXPIRY_MONTH "December"
#define INTERNAL_EXPIRY_YEAR "2222"
#define INTERNAL_EXPIRY_COMPANY "FREE ENTERPRISE – http://cfengine.com/25free for terms"

static time_t LAST_LICENSE_CHECK_TIMESTAMP;
static bool RecentlyCheckedLicense(void);
static char *LicenseFilePath(void);
static bool HubKeyPath(char path[MAX_FILENAME], char *hub_key_digest, char *hub_ip_address);
int Nova_HashKey(char *filename, char *buffer, const char *hash);
static void Nova_LogLicenseStatus(void);
static bool LicensePublicKeyPath(char path_public_key[MAX_FILENAME], char *path_license);


/*****************************************************************************/

int IsEnterprise(void)
{
    return true;
}

/*****************************************************************************/

int EnterpriseExpiry(void)
{
    char name[CF_MAXVARSIZE], policy_server[CF_MAXVARSIZE],
        installed_time_str[CF_MAXVARSIZE];
    char snumber[CF_SMALLBUF];
    int m_now, m_expire, d_now, d_expire;

#ifdef HAVE_LIBMONGOC
    bool am_policy_server = false;
#endif
    char u_day[16], u_month[16], u_year[16];

    if (THIS_AGENT_TYPE == cf_keygen)
    {
        return false;
    }

    if (RecentlyCheckedLicense())
    {
        return false;
    }

    strcpy(u_day, INTERNAL_EXPIRY_DAY);
    strcpy(u_month, INTERNAL_EXPIRY_MONTH);
    strcpy(u_year, INTERNAL_EXPIRY_YEAR);
    policy_server[0] = '\0';

// Verify first whether this host has been bootstrapped

    snprintf(name, sizeof(name), "%s%cpolicy_server.dat", CFWORKDIR, FILE_SEPARATOR);

    FILE *fp;

    if ((fp = fopen(name, "r")) != NULL)
    {
        fscanf(fp, "%s", policy_server);
        fclose(fp);
    }

    char *license_file_path = LicenseFilePath();
    EnterpriseLicense license;
    bool license_found = false;

    if(license_file_path)
    {
        license_found = LicenseFileParse(&license, license_file_path);
        free(license_file_path);
    }

    if (license_found)
    {
        // This is the simple password hash to obfuscate license fixing
        // Nothing top security here - this is a helper file to track licenses

        if (strlen(license.company_name) > 0)
        {
            snprintf(name, sizeof(name), "%s-%o.%d Nova %d %s", license.expiry_month, license.count, license.expiry_day, license.expiry_year, license.company_name);
        }
        else
        {
            snprintf(name, sizeof(name), "%s-%o.%d Nova %d", license.expiry_month, license.count, license.expiry_day, license.expiry_year);
        }

        char hub_key_path[MAX_FILENAME];

        if(!HubKeyPath(hub_key_path, license.public_key_digest, policy_server))
        {
            CfOut(cf_verbose, "", "Failed to verify license file for this host (%s) as we don't know the hub's public key", license.digest);
            LICENSES = 0;
            return false;
        }

        CfOut(cf_verbose, "", "Using public key %s for license verification", hub_key_path);

        if (Nova_HashKey(CFPUBKEYFILE, name, license.digest))
        {
            snprintf(u_day, sizeof(u_day), "%d", license.expiry_day);
            strcpy(u_month, license.expiry_month);
            snprintf(u_year, sizeof(u_year), "%d", license.expiry_year);
            LICENSES = license.count;
            CfOut(cf_verbose, "", " -> Verified license file %s - this is a policy server (%s)", license.digest, license.company_name);
#ifdef HAVE_LIBMONGOC
            am_policy_server = true;
#endif
            HardClass("am_policy_hub");
        }
        else if (Nova_HashKey(hub_key_path, name, license.digest))
        {
            snprintf(u_day, sizeof(u_day), "%d", license.expiry_day);
            strcpy(u_month, license.expiry_month);
            snprintf(u_year, sizeof(u_year), "%d", license.expiry_year);
            LICENSES = license.count;
            CfOut(cf_verbose, "", " -> Verified license %s (%s)", license.digest,
                  license.company_name);
        }
        else
        {
            CfOut(cf_verbose, "", "Failed to verify license file for this host (%s)\n", license.digest);
            LICENSES = 0;
            return false;       // Want to be able to bootstrap
        }
    }
    else
    {
        CfOut(cf_inform, "", " !! No commercial license file found - falling back on internal expiry\n");
        LICENSES = MAX_FREE_LICENSES;
        snprintf(license.company_name, MAX_COMPANY_NAME + 1, "%s", INTERNAL_EXPIRY_COMPANY);

        snprintf(name, sizeof(name), "%s/state/am_policy_hub", CFWORKDIR);
        MapName(name);

        struct stat sb;

        if (stat(name, &sb) != -1)
        {
            CfOut(cf_verbose, "", " -> This is a policy server %s of %s", POLICY_SERVER, license.company_name);
#ifdef HAVE_LIBMONGOC
            am_policy_server = true;
#endif
            HardClass("am_policy_hub");
        }
        else
        {
            CfOut(cf_verbose, "", " -> This system is a satellite");
#ifdef HAVE_LIBMONGOC
            am_policy_server = false;
#endif
        }
    }

    m_now = Month2Int(VMONTH);
    d_now = Str2Int(VDAY);

    m_expire = Month2Int(u_month);
    d_expire = Str2Int(u_day);

    CfDebug("Y. %s > %s\nM. %s > %s\nD: %s > %s\n", VYEAR, INTERNAL_EXPIRY_YEAR, VMONTH, INTERNAL_EXPIRY_MONTH, VDAY, INTERNAL_EXPIRY_DAY);
    CfDebug("Y. %s > %s\nM. %d > %d\nD: %d > %d\n", VYEAR, INTERNAL_EXPIRY_YEAR, m_now, m_expire, d_now, d_expire);

    snprintf(EXPIRY, sizeof(EXPIRY), "%s %s %s", u_day, u_month, u_year);
    strncpy(LICENSE_COMPANY, license.company_name, CF_SMALLBUF - 1);

    Nova_LogLicenseStatus();

    NewScalar("sys", "license_owner", license.company_name, cf_str);
    snprintf(snumber, CF_SMALLBUF, "%d", LICENSES);
    NewScalar("sys", "licenses_granted", snumber, cf_int);

    snprintf(installed_time_str, CF_MAXVARSIZE, "%ld", license.install_timestamp);
    NewScalar("sys", "licenses_installtime", installed_time_str, cf_str);

#ifdef HAVE_LIBMONGOC
    if (am_policy_server && THIS_AGENT_TYPE == cf_agent && CFDB_QueryIsMaster())
    {
        EnterpriseDB conn;
        if (CFDB_Open(&conn))
        {
            time_t expiry = 0;
            {
                struct tm t = { 0 };
                strptime(EXPIRY, "%d %b %Y", &t);
                expiry = mktime(&t);
            }

            CFDB_SaveLicense(&conn, expiry, license.install_timestamp, license.company_name, LICENSES);
            CFDB_Close(&conn);
        }
    }
#endif

    if ((strcmp(VYEAR, u_year) > 0) || ((strcmp(VYEAR, u_year) == 0) && (m_now > m_expire))
        || ((strcmp(VYEAR, u_year) == 0) && (m_now == m_expire) && (d_now > d_expire)))
    {
        if (!IsDefinedClass("bootstrap_mode", NULL))  // avoid cf-promises complaints while bootstrapping
        {
            CfOut(cf_error, "", " !! %d licenses expired on %s %s %s -- reverting to Community Edition", LICENSES,
                  u_day, u_month, u_year);
        }

        LICENSES = 0;
        return false;           // return true if we want to stop everything
    }
    else
    {
        CfOut(cf_verbose, "", " -> Found %d CFE Nova licenses, expiring on %s %s %s for %s", LICENSES, u_day,
              u_month, u_year, license.company_name);
        AM_NOVA = true;
        
        return false;
    }
}

/*****************************************************************************/

static bool RecentlyCheckedLicense(void)
{
#define SECONDS_BETWEEN_CHECKS (5 * SECONDS_PER_MINUTE)

    time_t now = time(NULL);

    if (LAST_LICENSE_CHECK_TIMESTAMP > now - SECONDS_BETWEEN_CHECKS)
    {
        return true;
    }

    LAST_LICENSE_CHECK_TIMESTAMP = now;

    return false;
}

/*****************************************************************************/

bool LicenseFileParse(EnterpriseLicense *license, char *license_file_path)
{
#define MAX_LICENSE_FILE_SIZE 256

    assert(SafeStringLength(license_file_path) > 0);

    license->company_name[0] = '\0';

    char *license_file_contents;
    FileReadMax(&license_file_contents, license_file_path, MAX_LICENSE_FILE_SIZE);

    if(!license_file_contents)
    {
        return false;
    }

    char format[256];

    snprintf(format, sizeof(format), "%%d %%x %%%ds %%d %%%ds %%%d[^\n]",
             MAX_MONTH_NAME + 1, MAX_DIGEST_HEX + 5, MAX_COMPANY_NAME + 1);

    sscanf(license_file_contents, format,
           &(license->expiry_day), &(license->count), license->expiry_month,
           &(license->expiry_year), license->digest, license->company_name);


    snprintf(format, sizeof(format), "%%*[^\n]%%%ds[^\n]", MAX_DIGEST_HEX + 1);

    sscanf(license_file_contents, format, license->public_key_digest);

    free(license_file_contents);

    struct stat sb;

    if(cfstat(license_file_path, &sb) != 0)
    {
        return false;
    }

    license->install_timestamp = sb.st_mtime;

    return true;
}

/*****************************************************************************/

static char *LicenseFilePath(void)
{
    struct stat sb;
    char filename[MAX_FILENAME];

    snprintf(filename, sizeof(filename), "%s/inputs/license.dat", CFWORKDIR);
    MapName(filename);

    if (cfstat(filename, &sb) == -1)
    {
        snprintf(filename, sizeof(filename), "%s/masterfiles/license.dat", CFWORKDIR);
        MapName(filename);
    }

    if (cfstat(filename, &sb) == -1)
    {
        return NULL;
    }

    return xstrdup(filename);
}

/*****************************************************************************/

static bool HubKeyPath(char path[MAX_FILENAME], char *hub_key_digest, char *hub_ip_address)
{
    snprintf(path, MAX_FILENAME, "%s/ppkeys/root-SHA=%s.pub", CFWORKDIR, hub_key_digest);
    MapName(path);

    struct stat sb;

    if(cfstat(path, &sb) == 0)
    {
        return true;
    }

    CfOut(cf_verbose, "", "No public key found at %s", path);

    if(strlen(hub_ip_address) == 0)
    {
        CfOut(cf_verbose, "", "Hub ip address is empty");
        return false;
    }

    // fallback to lookup of ip address -> hub key in lastseen

    /*
       DEPRECATED:
       license.dat's produced after September 10 2012 (commit b933e0bbf52f9ada224dfd998e68ba53cb14932a)
       will include the hub's public key digest and thus be independent of lastseen lookups.
       The code below can be removed when we don't want it to be backwards-compatible with old license files.
    */

    char hub_key_digest_lookup[CF_MAXVARSIZE] = {0};

    Address2Hostkey(hub_ip_address, hub_key_digest_lookup);

    snprintf(path, MAX_FILENAME, "%s/ppkeys/%s-%s.pub", CFWORKDIR, "root", hub_key_digest_lookup);
    MapName(path);

    RSA *serverrsa;

    if ((serverrsa = HavePublicKey("root", hub_ip_address, hub_key_digest_lookup)))
    {
        RSA_free(serverrsa);
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

/* Not a static as being tested by unit tests */
int Nova_HashKey(char *filename, char *buffer, const char *hash)
{
    EVP_MD_CTX context;
    const EVP_MD *md = NULL;
    FILE *fp;
    unsigned int md_len;
    bool result = false;
    char fbuf[CF_BUFSIZE];
    unsigned char digest[EVP_MAX_MD_SIZE + 1];

    md = EVP_get_digestbyname("sha256");

    if (md == NULL)
    {
        CfOut(cf_error, "", " !! Unable to compute a valid hash");
        return false;
    }

    EVP_DigestInit(&context, md);
    EVP_DigestUpdate(&context, (unsigned char *) buffer, strlen(buffer));

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return false;
    }
    else
    {
        fbuf[0] = '\0';

        while (!feof(fp))
        {
            fgets(fbuf, CF_BUFSIZE, fp);
            EVP_DigestUpdate(&context, (unsigned char *) fbuf, strlen(fbuf));
        }

        fclose(fp);

        EVP_DigestFinal(&context, digest, &md_len);

// Compare this to the assertions

        if (strcmp(HashPrint(cf_sha256, digest), hash) == 0)
        {
            return true;
        }

//   CfOut(cf_verbose,""," Key file %s was not authorized as policy server by sha256\n",filename);
        result = false;
    }

// Support legacy

    md = EVP_get_digestbyname("md5");

    if (md == NULL)
    {
        CfOut(cf_error, "", " !! Unable to compute a valid hash");
        return false;
    }

    EVP_DigestInit(&context, md);
    EVP_DigestUpdate(&context, (unsigned char *) buffer, strlen(buffer));

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return false;
    }

    fbuf[0] = '\0';

    while (!feof(fp))
    {
        fgets(fbuf, CF_BUFSIZE, fp);
        EVP_DigestUpdate(&context, (unsigned char *) fbuf, strlen(fbuf));
    }

    fclose(fp);

    EVP_DigestFinal(&context, digest, &md_len);

// Compare this to the assertions

    if (strcmp(HashPrint(cf_md5, digest), hash) == 0)
    {
        return true;
    }

//CfOut(cf_verbose,""," Key file %s was not authorized as policy server by md5\n",filename);

    return result;
}

/*****************************************************************************/

void CheckLicenses(void)
{
    int licenses = 0;
    Rval retval;

    if (GetVariable("control_common", CFG_CONTROLBODY[cfg_licenses].lval, &retval) != cf_notype)
    {
        licenses = Str2Int(retval.item);
        CfOut(cf_verbose, "", " -> %d paid licenses have been purchased (this is a promise by you)", licenses);
        NewScalar("sys", "licenses_promised", retval.item, cf_int);
#ifdef HAVE_LIBMONGOC
        if (THIS_AGENT_TYPE == cf_agent && CFDB_QueryIsMaster())
        {
            EnterpriseDB conn;
            if (CFDB_Open(&conn))
            {
                size_t promised = StringToLong((const char *)retval.item);
                CFDB_SaveLicenseNumberPromised(&conn, promised);
                CFDB_Close(&conn);
            }
        }
#endif
    }

    if (licenses == 0)
    {
        // using bootstrap_mode to avoid cf-promises complaining during bootstrap also (gets defined)
        if (!IsDefinedClass("bootstrap_mode", NULL) && getuid() == 0 && (THIS_AGENT_TYPE != cf_know)
            && (THIS_AGENT_TYPE != cf_keygen))
        {
            CfOut(cf_error, "", " !! Your configuration promises no host_licenses_paid in common control");
            CfOut(cf_error, "", " !! By doing this, you confirm that the terms of the contract are legally binding");
        }
    }
    else if (licenses > LICENSES && THIS_AGENT_TYPE != cf_know)
    {
        CfOut(cf_error, "",
              " !! You have promised that %d license(s) have been paid for, but CFEngine has only promised to honour %d in the agreement. ",
              licenses, LICENSES);
        CfOut(cf_inform, "", " !! You could be in violation of contract.");
    }
    else if (licenses < LICENSES)
    {
        CfOut(cf_inform, "",
              " -> According to you only %d license(s) have been paid for. CFEngine has promised to honour %d in the agreement.",
              licenses, LICENSES);
    }
}

/*****************************************************************************/

static void Nova_LogLicenseStatus(void)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    char datestr[CF_MAXVARSIZE], data[CF_MAXVARSIZE];
    Rval retval;
    Promise *pp = NewPromise("track_license", "License tracker");
    Attributes dummyattr = { {0} };
    CfLock thislock;
    int min = 9999999, max = -1, lic1, lic2, i = 0;
    time_t now = time(NULL), dt, then;
    double sum_t = 0, ex_t = 0, lic_t = 0;
    int ksize, vsize;
    char *key;
    void *value;
    long ltime;

    dummyattr.transaction.ifelapsed = 1440;     // 1 day
    dummyattr.transaction.expireafter = 1440;   // 1 day

    if (!IsDefinedClass("am_policy_hub", NULL))
    {
        return;
    }

    thislock = AcquireLock("license_track", VUQNAME, CFSTARTTIME, dummyattr, pp, false);

    if (thislock.lock == NULL)
    {
        DeletePromise(pp);
        return;
    }

    int licenses_policy = 0;

    if (GetVariable("control_common", CFG_CONTROLBODY[cfg_licenses].lval, &retval) != cf_notype)
    {
        licenses_policy = Str2Int(retval.item);
    }

    int lastseen_count = LastSeenHostKeyCount();

    if (lastseen_count == 0)
    {
        lastseen_count = 1;
    }

    CfOut(cf_verbose, "", " -> Detected current number of used licenses at approximately %d/%d\n", lastseen_count, LICENSES);

    if (!OpenDB(&dbp, dbid_license))
    {
        DeletePromise(pp);
        YieldCurrentLock(thislock);
        return;
    }

    snprintf(datestr, CF_MAXVARSIZE - 1, "%s", cf_ctime(&now));
    snprintf(data, CF_MAXVARSIZE - 1, "%d,%d,%d,%ld", lastseen_count, LICENSES, licenses_policy, (long) now);

    Chop(datestr);
    WriteDB(dbp, datestr, data, sizeof(data));

/* Cache the license report */

// Calculate utilization in each dt of the record

    then = time(NULL);          // Set this to now for first round
    int count;

    if (NewDBCursor(dbp, &dbcp))
    {
        while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
        {
            if (value == NULL)
            {
                continue;
            }

            count = lic1 = lic2 = 0;
            ltime = 0L;

            sscanf(value, "%d,%d,%d,%ld", &count, &lic1, &lic2, &ltime);

            i++;
            now = (time_t) ltime;

            if (count > max)
            {
                max = count;
            }

            if (count < min)
            {
                min = count;
            }

            dt = now - then;

            if (dt > 0)
            {
                ex_t += (double) dt *count;
                lic_t += (double) dt *lic1;

                sum_t += (double) dt;
            }

            if (then > 0)
            {
                then = now;
            }
            else
            {
                then = time(NULL);
            }
        }

        DeleteDBCursor(dbp, dbcp);
    }

    CloseDB(dbp);

#ifdef HAVE_LIBMONGOC
    if (CFDB_QueryIsMaster())
    {
        EnterpriseDB conn;
        if (CFDB_Open(&conn))
        {
            now = time(NULL);

            if (sum_t > 0)
            {
                double average = ex_t / sum_t;
                double granted = lic_t / sum_t;

                CFDB_SaveLicenseUsage(&conn, now, i, min, max, average, average / granted * 100.0, count);
            }
            else
            {
                CFDB_SaveLicenseUsage(&conn, now, i, min, max, -1.0, -1.0, count);
            }

            CFDB_Close(&conn);
        }
    }
#endif

    YieldCurrentLock(thislock);
    DeletePromise(pp);
}

/*****************************************************************************/

int Nova_CheckLicenseWin(char *pos)
{
    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! Invalid Enterprise license limits functionality (%s requires a license, agent %s)",
              pos, CF_AGENTTYPES[THIS_AGENT_TYPE]);
    }

    return true;
}


bool LicenseInstall(char *path_source)
{
    struct stat sb;

    if(cfstat(path_source, &sb) == -1)
    {
        CfOut(cf_error, "cfstat", "!! Can not stat input license file %s", path_source);
        return false;
    }

    char path_destination[MAX_FILENAME];
    snprintf(path_destination, sizeof(path_destination), "%s/inputs/license.dat", CFWORKDIR);
    MapName(path_destination);

    if(cfstat(path_destination, &sb) == 0)
    {
        CfOut(cf_error, "", "!! A license file is already installed in %s -- please move it out of the way and try again", path_destination);
        return false;
    }

    char path_public_key[MAX_FILENAME];

    if(!LicensePublicKeyPath(path_public_key, path_source))
    {
        CfOut(cf_error, "", "!! Could not find path to public key -- license parse error?");
    }

    if(cfstat(path_public_key, &sb) != 0)
    {
        CfOut(cf_error, "", "!! The licensed public key is not installed -- please copy it to %s and try again", path_public_key);
        return false;
    }


    bool success = CopyRegularFileDisk(path_source, path_destination, false);

    if(success)
    {
        CfOut(cf_inform, "", "Installed license at %s", path_destination);
    }
    else
    {
        CfOut(cf_error, "", "!! Failed copying license from %s to %s", path_source, path_destination);
    }

    return success;
}


static bool LicensePublicKeyPath(char path_public_key[MAX_FILENAME], char *path_license)
{
    EnterpriseLicense license;

    if(!LicenseFileParse(&license, path_license))
    {
        return false;
    }

    snprintf(path_public_key, MAX_FILENAME, "%s/ppkeys/root-SHA=%s.pub", CFWORKDIR, license.public_key_digest);
    MapName(path_public_key);

    return true;
}
