/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"

#include "ldap_lib.h"

void *CfLDAPValue(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    CfOut(OUTPUT_LEVEL_ERROR, "", "LDAP support is disabled");
    return NULL;
}

void *CfLDAPList(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    CfOut(OUTPUT_LEVEL_ERROR, "", "LDAP support is disabled");
    return NULL;
}

void *CfLDAPArray(char *array, char *uri, char *basedn, char *filter, char *scopes, char *sec)
{
    CfOut(OUTPUT_LEVEL_ERROR, "", "LDAP support is disabled");
    return NULL;
}

void *CfRegLDAP(char *uri, char *basedn, char *filter, char *name, char *scopes, char *regex, char *sec)
{
    CfOut(OUTPUT_LEVEL_ERROR, "", "LDAP support is disabled");
    return NULL;
}

Rlist *CfLDAP_GetSingleAttributeList(const char *username, const char *password, const char *uri, const char *authdn, const char *basedn, const char *filter,
                                     const char *attribute_name, const char *scopes, const char *security, bool start_tls,
                                     size_t page, size_t count, const char **const errstr)
{
    CfOut(OUTPUT_LEVEL_ERROR, "", "LDAP support is disabled");
    *errstr = "LDAP support is disabled";
    return 0;
}
