#ifndef CFENGINE_NOVA_LDAP_H
#define CFENGINE_NOVA_LDAP_H

#include "cf3.defs.h"
#include "rlist.h"

bool CfLDAPAuthenticate(const char *uri, const char *basedn, const char *passwd, const char *authentication_method, bool starttls,
                        time_t timeout_seconds, const char **const errstr);

Rlist *CfLDAP_GetSingleAttributeList(const char *username, const char *password, const char *uri, const char *authdn, const char *basedn, const char *filter,
                                     const char *attribute_name, const char *scopes, const char *security, bool start_tls,
                                     size_t page, size_t results_per_page, const char **const errstr);

void *CfLDAPValue(char *uri, char *dn, char *filter, char *name, char *scope, char *sec);

#endif
