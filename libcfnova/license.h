#ifndef CFENGINE_LICENSE_H
#define CFENGINE_LICENSE_H

#define MAX_COMPANY_NAME 512

typedef enum
{
    ENTERPRISE_LICENSE_TYPE_FULL,
    ENTERPRISE_LICENSE_TYPE_FREE
}EnterpriseLicenseType;

typedef struct
{
    int expiry_day;
    char expiry_month[MAX_MONTH_NAME + 1];
    int expiry_year;
    int count;
    char company_name[MAX_COMPANY_NAME + 1];
    char digest[MAX_DIGEST_HEX + 5];     /* has "SHA=" prefix */
    char public_key_digest[MAX_DIGEST_HEX + 1];
    EnterpriseLicenseType license_type;
    time_t install_timestamp;
}EnterpriseLicense;

bool LicenseFileParse(EnterpriseLicense *license, char *license_file_path);
bool LicenseInstall(char *path_source);

#endif  /* CFENGINE_LICENSE_H */
