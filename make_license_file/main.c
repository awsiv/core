/*

 This file is (C) Cfengine AS. Internal use ONLY!

*/

// gcc -o make_license_file make_license_file.c -lcrypto

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#define CF_SHA256_LEN 32
#define CF_MAXVARSIZE 1024

#define CF_LICENSE_FILENAME "license.dat"

static const struct option OPTIONS[] =
{
    {"help", no_argument, 0, 'h'},
    {"license-version", required_argument, 0, 'L'},
    {"organization-name", required_argument, 0, 'n'},
    {"expire", required_argument, 0, 'd'},
    {"license-count", required_argument, 0, 'c'},
    {"public-key", required_argument, 0, 'k'},
    {NULL, 0, 0, '\0'}
};

static const char *HINTS[] =
{
    "Print option list",
    "Specify license product nova/constellation (required)",
    "Unique company name (example \"CFEngine AS\")(required)",
    "License expire date: (example \"1 January 2012\") (required)",
    "Number of licenses (required)",
    "Filepath for public key (required)",
    NULL
};

enum cf_product_id
{
    cf_constellation,
    cf_nova,
    cf_none
};

typedef struct
{
    enum cf_product_id product_id;
    char *company_uid;
    char *expire_day;
    char *expire_month;
    char *expire_year;
    int licence_count;
    char *public_key_filepath;
} LicenceSettings;

/*******************************************************************/

/* license generation */
static char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE + 1]);
static void ThisHashString(char *fna, char *buffer, int len, unsigned char digest[EVP_MAX_MD_SIZE + 1]);
static char *PublicKeyDigestString(char *public_key_digest_str, const char *public_key_file);
static void MakeLicense(const char *public_key_file, enum cf_product_id product_id, const char *day_str,
                        const char *month_str, const char *year_str, const char *company_str, int license_count);
static void ThisHashPubKey(unsigned char digest[EVP_MAX_MD_SIZE + 1], RSA *key);
static RSA *ReadPublicKey(const char *filename);

/* input validation */
static int IsLeapYear(int year);
static int Month2Int(const char *month);
static int ValidateMonth(int day, int month, int year);
static int ValidateDay(int day, int month, int year);
static void ValidateDate(const char *day_str, const char *month_str, const char *year_str);
static int Str2Int(const char *str);
static void Chop(char *str);

static LicenceSettings GetSettingsFromUser(void);
static LicenceSettings GetSettingsFromCmd(int argc, char **argv);
static void ValidateLicenseSettings(const LicenceSettings * settings);
static void PrintSettings(const LicenceSettings * settings);
static void FreeSettings(const LicenceSettings * settings);
static void Syntax(const char *component, const struct option options[], const char *hints[]);

/*******************************************************************/

int main(int argc, char **argv)
{
    LicenceSettings settings;

    if (argc != 1)
    {
        settings = GetSettingsFromCmd(argc, argv);
    }
    else
    {
        settings = GetSettingsFromUser();
    }

    ValidateLicenseSettings(&settings);
    PrintSettings(&settings);

    MakeLicense(settings.public_key_filepath, settings.product_id, settings.expire_day,
                settings.expire_month, settings.expire_year, settings.company_uid, settings.licence_count);

    FreeSettings(&settings);

    return 0;
}

/*******************************************************************/

static char *xgetline(void)
{
    char *retval = NULL;
    char buff[CF_MAXVARSIZE];

    fflush(stdin);
    if (fgets(buff, CF_MAXVARSIZE, stdin))
    {
        char *newline = strchr(buff, '\n');

        if (newline)
        {
            *newline = '\0';
        }
        Chop(buff);
        retval = strdup(buff);
    }

    return retval;
}

static LicenceSettings GetSettingsFromUser(void)
{
    char *buff;
    LicenceSettings settings;

    settings.product_id = cf_none;
    settings.company_uid = NULL;
    settings.expire_day = NULL;
    settings.expire_month = NULL;
    settings.expire_year = NULL;
    settings.licence_count = 0;
    settings.public_key_filepath = NULL;

/* product id */
    printf("Enter edition of CFEngine: (c)onstellation or (n)ova: ");
    if ((buff = xgetline()) != NULL)
    {
        if (strcmp(buff, "n") == 0)
        {
            settings.product_id = cf_nova;
        }
        else if (strcmp(buff, "c") == 0)
        {
            settings.product_id = cf_constellation;
        }
        free(buff);
    }

/* company uid */
    printf("Enter unique company string: ");
    if ((buff = xgetline()) != NULL)
    {
        settings.company_uid = buff;
    }

/* public-key */
    printf("Enter the filename of the client\'s public key: ");
    if ((buff = xgetline()) != NULL)
    {
        settings.public_key_filepath = buff;
    }

    printf("Enter license expiry date:\n");
    printf("Enter day (e.g. 28): ");
    if ((buff = xgetline()) != NULL)
    {
        settings.expire_day = buff;
    }

    printf("Enter month (e.g. July): ");
    if ((buff = xgetline()) != NULL)
    {
        settings.expire_month = buff;
    }

    printf("Enter year (e.g. 2023): ");
    if ((buff = xgetline()) != NULL)
    {
        settings.expire_year = buff;
    }

    printf("Enter number of licenses: ");
    if ((buff = xgetline()) != NULL)
    {
        sscanf(buff, "%d", &settings.licence_count);
    }

    return settings;
}

/*******************************************************************/

static LicenceSettings GetSettingsFromCmd(int argc, char **argv)
{
    LicenceSettings settings;

    settings.product_id = cf_none;
    settings.company_uid = NULL;
    settings.expire_day = NULL;
    settings.expire_month = NULL;
    settings.expire_year = NULL;
    settings.licence_count = 0;
    settings.public_key_filepath = NULL;

    int c;
    int optindex = 0;
    extern char *optarg;
    char day[CF_MAXVARSIZE];
    char month[CF_MAXVARSIZE];
    char year[CF_MAXVARSIZE];
    char junk[CF_MAXVARSIZE];
    char buff[CF_MAXVARSIZE];
    int cnt;

    while ((c = getopt_long(argc, argv, "hL:n:d:c:k:", OPTIONS, &optindex)) != EOF)
    {
        switch ((char) c)
        {
        case 'h':
            Syntax("CFEngine License Generator", OPTIONS, HINTS);
            exit(0);

        case 'L':
            if (strcmp(optarg, "nova") == 0)
            {
                settings.product_id = cf_nova;
            }
            else if (strcmp(optarg, "constellation") == 0)
            {
                settings.product_id = cf_constellation;
            }
            break;

        case 'n':
            settings.company_uid = strdup(optarg);
            break;

        case 'd':
            if (sscanf(optarg, "%s %s %s %s", day, month, year, junk) == 3)
            {
                settings.expire_day = strdup(day);
                settings.expire_month = strdup(month);
                settings.expire_year = strdup(year);
            }
            break;

        case 'c':
            if (sscanf(optarg, "%d", &cnt) > 0)
            {
                settings.licence_count = cnt;
            }
            break;

        case 'k':
            settings.public_key_filepath = strdup(optarg);
            break;

        default:
            Syntax("CFEngine License Generator", OPTIONS, HINTS);
            exit(1);
        }
    }

    return settings;
}

/*******************************************************************/

static void ValidateLicenseSettings(const LicenceSettings * settings)
{
/* product */
    switch (settings->product_id)
    {
    case cf_constellation:
    case cf_nova:
        break;

    default:
        printf("Error: Invalid CFEngine Edition\n");
        exit(1);
    }

/* company uid */
    if (settings->company_uid == NULL || strlen(settings->company_uid) == 0)
    {
        printf("Error: Invalid unique company name\n");
        exit(1);
    }

/* public key */
    struct stat sb;

    if ((stat(settings->public_key_filepath, &sb) == -1) && (settings->public_key_filepath != NULL))
    {
        printf("Public key file %s not found\n", settings->public_key_filepath);
        exit(1);
    }

/* date */
    ValidateDate(settings->expire_day, settings->expire_month, settings->expire_year);

/* licence number */
    if (settings->licence_count <= 0)
    {
        printf("Error: Invalid number of licenses: %d\n", settings->licence_count);
        exit(1);
    }
}

/*******************************************************************/

static void PrintSettings(const LicenceSettings * settings)
{
    printf("\nLICENSE SUMMARY:\n\n");
    printf("CFEngine product: \t");
    switch (settings->product_id)
    {
    case cf_nova:
        printf("NOVA\n");
        break;

    case cf_constellation:
        printf("CONSTELLATION\n");
        break;

    default:
        printf("\n");
    }

    printf("Company unique nane: \t%s\n", settings->company_uid);
    printf("License expire date: \t%s %s %s\n", settings->expire_day, settings->expire_month, settings->expire_year);
    printf("Number of licenses: \t%d\n", settings->licence_count);
    printf("\n");
}

/*******************************************************************/

static void MakeLicense(const char *public_key_file, enum cf_product_id product_id, const char *day_str,
                        const char *month_str, const char *year_str, const char *company_str, int license_count)
{
    char buffer[CF_MAXVARSIZE];
    unsigned char digest[EVP_MAX_MD_SIZE + 1];
    FILE *fp = NULL;

    if ((fp = fopen(CF_LICENSE_FILENAME, "w")) == NULL)
    {
        printf("Error: Could not create %s file\n", CF_LICENSE_FILENAME);
        exit(1);
    }

    switch (product_id)         // constellation
    {
    case cf_constellation:
        snprintf(buffer, CF_MAXVARSIZE - 1, "%s-%o.%s Constellation %s %s",
                 month_str, license_count, day_str, year_str, company_str);
        ThisHashString((char *) public_key_file, buffer, strlen(buffer), digest);
        fprintf(fp, "%2s %x %2s %4s %s %s\n", day_str, license_count, month_str, year_str,
                ThisHashPrint(digest), company_str);
        fprintf(fp, "CN");
        break;

    case cf_nova:              // nova
        snprintf(buffer, CF_MAXVARSIZE - 1, "%s-%o.%s Nova %s %s",
                 month_str, license_count, day_str, year_str, company_str);
        ThisHashString((char *) public_key_file, buffer, strlen(buffer), digest);
        fprintf(fp, "%2s %x %2s %4s %s %s", day_str, license_count, month_str, year_str,
                ThisHashPrint(digest), company_str);
        break;

    default:
        printf("Error: Unknown product version\n");
        exit(1);
    }

    char public_key_digest_str[512];
    PublicKeyDigestString(public_key_digest_str, public_key_file);
    fprintf(fp, "\n%s", public_key_digest_str);

    fclose(fp);

    printf("Info: Wrote %s - install this in WORKDIR/masterfiles on the policy server\n", CF_LICENSE_FILENAME);
}

/*******************************************************************/

static void ThisHashString(char *filename, char *buffer, int len, unsigned char digest[EVP_MAX_MD_SIZE + 1])
{
    EVP_MD_CTX context;
    const EVP_MD *md = NULL;
    FILE *fp;
    int md_len;
    char fbuf[2048];

    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
    ERR_load_crypto_strings();

    md = EVP_get_digestbyname("sha256");

    if (md == NULL)
    {
        printf(" !! Digest type not supported by OpenSSL library\n");
    }

    EVP_DigestInit(&context, md);
    EVP_DigestUpdate(&context, (unsigned char *) buffer, len);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("No public key file could be read");
        exit(1);
    }

    fbuf[0] = '\0';
    while (!feof(fp))
    {
        fgets(fbuf, 2048, fp);
        EVP_DigestUpdate(&context, (unsigned char *) fbuf, strlen(fbuf));
    }
    fclose(fp);

    EVP_DigestFinal(&context, digest, &md_len);
}

/*********************************************************************/

static char *PublicKeyDigestString(char *public_key_digest_str, const char *public_key_file)
{
    unsigned char public_key_digest[EVP_MAX_MD_SIZE + 1];

    RSA *public_key = ReadPublicKey(public_key_file);

    if(public_key == NULL)
    {
        printf("Could not read public key");
        exit(1);
    }

    ThisHashPubKey(public_key_digest, public_key);
    RSA_free(public_key);

    strcpy(public_key_digest_str, ThisHashPrint(public_key_digest) + 4);

    return public_key_digest_str;
}

/*********************************************************************/

static void ThisHashPubKey(unsigned char digest[EVP_MAX_MD_SIZE + 1], RSA *key)
{
    EVP_MD_CTX context;
    const EVP_MD *md = NULL;
    int md_len, i, buf_len, actlen;
    unsigned char *buffer;

    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
    ERR_load_crypto_strings();

    if (key->n)
    {
        buf_len = (size_t) BN_num_bytes(key->n);
    }
    else
    {
        buf_len = 0;
    }

    if (key->e)
    {
        if (buf_len < (i = (size_t) BN_num_bytes(key->e)))
        {
            buf_len = i;
        }
    }

    buffer = malloc(buf_len + 10);

    md = EVP_get_digestbyname("sha256");

    if (md == NULL)
    {
        printf("!! Digest type sha256 not supported by OpenSSL library");
        exit(1);
    }

    EVP_DigestInit(&context, md);

    actlen = BN_bn2bin(key->n, buffer);
    EVP_DigestUpdate(&context, buffer, actlen);
    actlen = BN_bn2bin(key->e, buffer);
    EVP_DigestUpdate(&context, buffer, actlen);
    EVP_DigestFinal(&context, digest, &md_len);

    free(buffer);
}

/*********************************************************************/

static RSA *ReadPublicKey(const char *filename)
{
    char *passphrase = "Cfengine passphrase";

    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("!! Could not read public key %s\n", filename);
        return NULL;
    }

    RSA *key = PEM_read_RSAPublicKey(fp, NULL, NULL, passphrase);
    unsigned long err;

    if (key == NULL)
    {
        err = ERR_get_error();
        printf("Error reading public key: %s\n", ERR_reason_error_string(err));
    }

    fclose(fp);
    return key;
}

/*********************************************************************/

static char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE + 1])
{
    unsigned int i;
    static char buffer[EVP_MAX_MD_SIZE * 4];

    sprintf(buffer, "SHA=  ");

    for (i = 0; i < CF_SHA256_LEN; i++)
    {
        sprintf((char *) (buffer + 4 + 2 * i), "%02x", digest[i]);
    }

    return buffer;
}

/*********************************************************************/

static void Chop(char *str)     /* remove trailing spaces */
{
    int i;

    if ((str == NULL) || (strlen(str) == 0))
    {
        return;
    }

    for (i = strlen(str) - 1; isspace((int) str[i]); i--)
    {
        str[i] = '\0';
    }
}

/*********************************************************************/

static int IsLeapYear(int year)
{
    return (int) ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

/*********************************************************************/

static int Month2Int(const char *month)
{
    int i;

    const char *month_tab[] =
{
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    for (i = 0; i < 12; ++i)
    {
        if (strcmp(month, month_tab[i]) == 0)
        {
            return i + 1;
        }
    }

    return -1;
}

/*********************************************************************/

static int ValidateDay(int day, int month, int year)
{
    int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month == 2 && IsLeapYear(year))
    {
        days_per_month[1] = 29; /* february in leapyear have +1 day */
    }

    if (day > 0 && day <= days_per_month[month - 1])
    {
        return 1;
    }

    return 0;
}

/*********************************************************************/

static int Str2Int(const char *str)
{
    int isNum = 1;
    int i;

    for (i = 0; i < strlen(str); ++i)
    {
        if (!isdigit(str[i]))
        {
            isNum = 0;
        }
    }

    if (!isNum)
    {
        printf("Error: Invalid value: %s\n", str);
        exit(1);
    }

    return atoi(str);
}

/*********************************************************************/

static void ValidateDate(const char *day_str, const char *month_str, const char *year_str)
{
    if (day_str == NULL || month_str == NULL || year_str == NULL)
    {
        printf("Error: Invalid date format\n");
        exit(1);
    }

    int year = Str2Int(year_str);
    int month = Month2Int(month_str);

    if (month == -1)
    {
        printf("Error: Invalid month: %s\n", month_str);
        exit(1);
    }

    int day = Str2Int(day_str);

    if (!ValidateDay(day, month, year))
    {
        printf("Error: Invalid day: %s\n", day_str);
        exit(1);
    }

/* checking if the expire date is in the past */

    time_t current_time, expire_time;

    current_time = time(NULL);

    struct tm current_tm;

    current_tm = *localtime(&current_time);

    struct tm expire_tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    expire_tm.tm_year = year - 1900;
    expire_tm.tm_mon = month - 1;
    expire_tm.tm_mday = day;
    expire_tm.tm_hour = current_tm.tm_hour;
    expire_tm.tm_min = current_tm.tm_min;
    expire_tm.tm_sec = current_tm.tm_sec;
    expire_time = mktime(&expire_tm);

    if (expire_time == -1)
    {
        printf("Error: Invalid expire date: %s %s %s\n", year_str, month_str, day_str);
        exit(1);
    }

    if (current_time > expire_time)
    {
        printf("Error: Expire date is in the past\n");
        exit(1);
    }
}

/*********************************************************************/

static void FreeSettings(const LicenceSettings * settings)
{
    free(settings->company_uid);
    free(settings->expire_day);
    free(settings->expire_month);
    free(settings->expire_year);
    free(settings->public_key_filepath);
}

/*********************************************************************/

static void Syntax(const char *component, const struct option options[], const char *hints[])
{
    int i;

    printf("\n%s\n\n", component);

    printf("SYNOPSIS:\n\n   program [options]\n\nDESCRIPTION:\n");
    printf("Command line options:\n\n");

    for (i = 0; options[i].name != NULL; i++)
    {
        if (options[i].has_arg)
        {
            printf("--%-19s, -%c value - %s\n", options[i].name, (char) options[i].val, hints[i]);
        }
        else
        {
            printf("--%-19s, -%-7c - %s\n", options[i].name, (char) options[i].val, hints[i]);
        }
    }
    printf("\n");
}
