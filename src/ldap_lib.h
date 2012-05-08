#ifndef CFENGINE_NOVA_LDAP_H
#define CFENGINE_NOVA_LDAP_H

#include "cf3.defs.h"
#include "rlist.h"
#include "vars.h"

bool CfLDAPAuthenticate(const char *uri, const char *basedn, const char *passwd, bool starttls, const char **const errstr);

int CfLDAP_JSON_GetSeveralAttributes(char *uri, char *user, char *basedn, char *filter, Rlist *names, char *scopes,
                                     char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                     int bufsize, const char **const errstr);
int CfLDAP_JSON_GetSingleAttributeList(char *uri, char *user, char *basedn, char *filter, char *name, char *scopes,
                                       char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                       int bufsize, const char **const errstr);

#endif
