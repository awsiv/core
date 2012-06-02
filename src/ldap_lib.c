
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "ldap_lib.h"

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "files_names.h"
#include "item_lib.h"

#ifdef HAVE_LIBLDAP

void *CfLDAPValue(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec);
void *CfLDAPList(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec);
void *CfLDAPArray(char *array, char *uri, char *basedn, char *filter, char *scopes, char *sec);
void *CfRegLDAP(char *uri, char *basedn, char *filter, char *name, char *scopes, char *regex, char *sec);

static LDAP *NovaLDAPConnect(const char *uri, bool starttls, const char **errstr);
static int NovaLDAPAuthenticate(LDAP *ldap, const char *basedn, const char *sec, const char *pwd);
static int NovaStr2Scope(char *scope);
static Rlist *LDAPKeyInRlist(Rlist *list, char *key);

#endif

/*****************************************************************************/

#ifdef HAVE_LIBLDAP
void *CfLDAPValue(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, msgtype, num_entries = 0, num_refs = 0;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL, *return_value = NULL;
    int scope = NovaStr2Scope(scopes);

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The commercial license has expired, this function is not available");
        return NULL;
    }

    if ((ld = NovaLDAPConnect(uri, false, NULL)) == NULL)
    {
        return NULL;
    }

    if (NovaLDAPAuthenticate(ld, basedn, sec, NULL) != LDAP_SUCCESS)
    {
        return NULL;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        return NULL;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        msgtype = ldap_msgtype(msg);

        switch (msgtype)
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
            }
            else
            {
                CfOut(cf_verbose, "", " !! No LDAP query result found\n");
                break;
            }

            /* Iterate through each attribute in the entry. */

            for (a = ldap_first_attribute(ld, msg, &ber); a != NULL; a = ldap_next_attribute(ld, msg, ber))
            {
                /* Get and print all values for each attribute. */

                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        if (strcmp(a, name) == 0)
                        {
                            CfOut(cf_verbose, "", "Located LDAP value %s => %s\n", a, vals[i]->bv_val);
                            return_value = xstrdup((char *) vals[i]->bv_val);
                            break;
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);

                if (return_value)
                {
                    break;
                }
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            ldap_memfree(dn);
            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search successful, %d entries, %d references", num_entries, num_refs);
            }
            break;

        default:
            break;

        }

        if (return_value)
        {
            break;
        }
    }

/* Disconnect when done. */

    ldap_unbind(ld);
    return return_value;
}
#else /* HAVE_LIBLDAP */
void *CfLDAPValue(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    CfOut(cf_error, "", "LDAP support is disabled");
    return NULL;
}
#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP
void *CfLDAPList(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, num_entries = 0, num_refs = 0;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL;
    int scope = NovaStr2Scope(scopes);
    Rlist *return_value = NULL;

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The commercial license has expired, this function is not available");
        return NULL;
    }

    if ((ld = NovaLDAPConnect(uri, false, NULL)) == NULL)
    {
        return NULL;
    }

    if (NovaLDAPAuthenticate(ld, basedn, sec, NULL) != LDAP_SUCCESS)
    {
        return NULL;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        return NULL;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        switch (ldap_msgtype(msg))
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
            }
            else
            {
                CfOut(cf_verbose, "", " !! No LDAP query found\n");
            }

            /* Iterate through each attribute in the entry. */

            for (a = ldap_first_attribute(ld, res, &ber); a != NULL; a = ldap_next_attribute(ld, res, ber))
            {
                /* Get and print all values for each attribute. */

                CfDebug(" ->   LDAP query found attribute %s", a);

                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        if (strcmp(a, name) == 0)
                        {
                            CfOut(cf_verbose, "", "Located LDAP value %s => %s\n", a, vals[i]->bv_val);
                            AppendRScalar(&return_value, (char *) vals[i]->bv_val, CF_SCALAR);
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            ldap_memfree(dn);
            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search was successful, %d entries, %d references", num_entries,
                      num_refs);
            }
            break;

        default:
            break;

        }
    }

/* Disconnect when done. */

    ldap_unbind(ld);
    return return_value;
}

#else /* HAVE_LIBLDAP */

void *CfLDAPList(char *uri, char *basedn, char *filter, char *name, char *scopes, char *sec)
{
    CfOut(cf_error, "", "LDAP support is disabled");
    return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

void *CfLDAPArray(char *array, char *uri, char *basedn, char *filter, char *scopes, char *sec)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, msgtype, num_entries = 0, num_refs = 0;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL;
    int scope = NovaStr2Scope(scopes);
    char name[CF_MAXVARSIZE], *return_value = NULL;

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The commercial license has expired, this function is not available");
        return NULL;
    }

    if ((ld = NovaLDAPConnect(uri, false, NULL)) == NULL)
    {
        return NULL;
    }

    if (NovaLDAPAuthenticate(ld, basedn, sec, NULL) != LDAP_SUCCESS)
    {
        return NULL;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        return NULL;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        msgtype = ldap_msgtype(msg);

        switch (msgtype)
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
                ldap_memfree(dn);
            }

            /* Iterate through each attribute in the entry. */

            for (a = ldap_first_attribute(ld, msg, &ber); a != NULL; a = ldap_next_attribute(ld, msg, ber))
            {
                /* Get and print all values for each attribute. */

                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        if (strcmp(a, "objectClass") != 0)      // This is non-unique
                        {
                            snprintf(name, CF_MAXVARSIZE - 1, "%s[%s]", array, a);
                            NewScalar(THIS_BUNDLE, name, (char *) vals[i]->bv_val, cf_str);
                            CfOut(cf_verbose, "", " -> Setting %s => %s, in context %s/%s\n", name,
                                  (char *) vals[i]->bv_val, THIS_BUNDLE, CONTEXTID);
                            return_value = xstrdup("any");

                            if (return_value)
                            {
                                break;
                            }
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            if (return_value)
            {
                break;
            }

            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search successful, %d entries, %d references", num_entries, num_refs);
            }
            break;

        default:
            break;

        }

        if (return_value)
        {
            break;
        }
    }

/* Disconnect when done. */

    if (!return_value)
    {
        return_value = xstrdup("!any");
    }

    ldap_unbind(ld);
    return return_value;
}

#else /* HAVE_LIBLDAP */

void *CfLDAPArray(char *array, char *uri, char *basedn, char *filter, char *scopes, char *sec)
{
    CfOut(cf_error, "", "LDAP support is disabled");
    return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

void *CfRegLDAP(char *uri, char *basedn, char *filter, char *name, char *scopes, char *regex, char *sec)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, num_entries = 0, num_refs = 0;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL, *return_value = NULL;
    int scope = NovaStr2Scope(scopes);

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The commercial license has expired, this function is not available");
        return NULL;
    }

    if ((ld = NovaLDAPConnect(uri, false, NULL)) == NULL)
    {
        return NULL;
    }

    if (NovaLDAPAuthenticate(ld, basedn, sec, NULL) != LDAP_SUCCESS)
    {
        return NULL;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        return NULL;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        switch (ldap_msgtype(msg))
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
                ldap_memfree(dn);
            }

            /* Iterate through each attribute in the entry. */

            for (a = ldap_first_attribute(ld, msg, &ber); a != NULL; a = ldap_next_attribute(ld, msg, ber))
            {
                /* Get and print all values for each attribute. */

                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        if (strcmp(a, name) == 0 && FullTextMatch(regex, (char *) vals[i]->bv_val))
                        {
                            CfOut(cf_verbose, "", " -> Located regex matching LDAP value %s => %s\n", a,
                                  (char *) vals[i]->bv_val);
                            return_value = xstrdup("any");
                            break;
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);

                if (return_value)
                {
                    break;
                }
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                return NULL;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search successful, %d entries, %d references", num_entries, num_refs);
            }
            break;

        default:
            CfOut(cf_verbose, "", "Unknown message received\n");
            break;

        }

        if (return_value)
        {
            break;
        }
    }

/* Disconnect when done. */

    ldap_unbind(ld);

    if (!return_value)
    {
        return_value = xstrdup("!any");
    }

    return return_value;
}

#else /* HAVE_LIBLDAP */

void *CfRegLDAP(char *uri, char *basedn, char *filter, char *name, char *scopes, char *regex, char *sec)
{
    CfOut(cf_error, "", "LDAP support is disabled");
    return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

bool CfLDAPAuthenticate(const char *uri, const char *basedn, const char *passwd, bool starttls, const char **const errstr)
{
    LDAP *ld = NovaLDAPConnect(uri, starttls, errstr);

    if (ld == NULL)
    {
        return false;
    }

    int ret = NovaLDAPAuthenticate(ld, basedn, "sasl", passwd);

    ldap_unbind(ld);

    if (ret == LDAP_SUCCESS)
    {
        return true;
    }

    if (errstr)
    {
        *errstr = (ret == LDAP_INVALID_CREDENTIALS) ? NULL : ldap_err2string(ret);
    }

    return false;
}

/*****************************************************************************/
/* JSON functions                                                            */
/*****************************************************************************/

//CODE EXAMPLE

/*
void test(void)
{
       if (Nova2PHP_LDAPAuthenticate("ldap://10.0.0.100","uid=sudhir,cn=users,dc=cf022osx,dc=cfengine,dc=com","q1w2e3r4t5"))
             {
             char buffer[1000000] = {0};

             printf("Authenticated\n");

             Rlist *names = SplitStringAsRList("uid,mail,sn,altSecurityIdentities",',');

             buffer[0] = '\0';
             if (CfLDAP_JSON_GetSeveralAttributes(
                       "ldap://10.0.0.152",
                       "uid=sudhir","ou=people,dc=cfengine,dc=com",
                     "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))" ,
                     names,
                     "subtree",
                     "sasl",
                       "password",
                     1,
                     100,
                     buffer,1000000) == 0)
                {
                printf("JSON2: %s\n",buffer);
                }

             }
          else
             {
             printf("NOT Authenticated\n");
             }
}

*/

/*****************************************************************************/

int CfLDAP_JSON_GetSeveralAttributes(char *uri, char *authdn, char *basedn, char *filter, Rlist *names, char *scopes,
                                     char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                     int bufsize, const char **const errstr)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, num_entries = 0, num_refs = 0, notdone = false;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL;
    int scope = NovaStr2Scope(scopes), count;
    Rlist *master = NULL, *rp, *dn_rp;
    Item *ip;
    CfAssoc *ap;
    char work[CF_BUFSIZE];

    if (page == 0)
    {
        page = 1;
    }

    if (linesperpage == 0)
    {
        linesperpage = 1000;
    }

    if ((ld = NovaLDAPConnect(uri, starttls, errstr)) == NULL)
    {
        return -1;
    }

    if ((ret = NovaLDAPAuthenticate(ld, authdn, sec, passwd)) != LDAP_SUCCESS)
    {
        *errstr = ldap_err2string(ret);
        return -1;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        *errstr = ldap_err2string(ret);
        return -1;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);
    count = 0;

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        count++;

        if (count > linesperpage * page)
        {
            break;
        }

        if (count < linesperpage * (page - 1))
        {
            continue;
        }

        switch (ldap_msgtype(msg))
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
            }
            else
            {
                CfOut(cf_verbose, "", " !! No LDAP query found\n");
            }

            /* Iterate through each attribute in the entry. */
            for (a = ldap_first_attribute(ld, res, &ber); a != NULL; a = ldap_next_attribute(ld, res, ber))
            {
                /* Get and print all values for each attribute. */
                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        for (rp = names; rp != NULL; rp = rp->next)
                        {
                            if (strcmp(a, rp->item) == 0)
                            {
                                Item **list;

                                if ((dn_rp = LDAPKeyInRlist(master, a)) == NULL)
                                {
                                    dn_rp = PrependRlistAlien(&master, NewAssoc(a, (Rval) {"dummy", CF_SCALAR}, cf_slist));
                                    ap = (CfAssoc *) dn_rp->item;
                                    free(ap->rval.item);
                                    ap->rval.item = NULL;
                                }
                                else
                                {
                                    ap = (CfAssoc *) dn_rp->item;
                                }

                                list = (Item **) &(ap->rval.item);

                                CfOut(cf_verbose, "", "Located LDAP value %s => %s\n", a, vals[i]->bv_val);
                                PrependItem(list, (char *) vals[i]->bv_val, rp->item);
                            }
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            ldap_memfree(dn);
            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                *errstr = ldap_err2string(parse_ret);
                return -1;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                *errstr = ldap_err2string(parse_ret);
                return -1;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search was successful, %d entries, %d references", num_entries,
                      num_refs);
            }
            break;

        default:
            break;

        }
    }

/* Disconnect when done. */

    ldap_unbind(ld);

/* Now format the data in JSON */
    strcpy(buffer, "{");

    snprintf(work, CF_BUFSIZE, "\"keys\" : {");
    Join(buffer, work, bufsize);
    count = 0;

    for (rp = master; rp != NULL; rp = rp->next, count++)
    {
        ap = rp->item;
        rp->state_ptr = ap->rval.item;

        if (rp->next)
        {
            snprintf(work, CF_BUFSIZE, "\"%s\" : %d,", (char *) ap->lval, count);
        }
        else
        {
            snprintf(work, CF_BUFSIZE, "\"%s\" : %d", (char *) ap->lval, count);
        }
        Join(buffer, work, bufsize);

        notdone = true;
    }
    snprintf(work, CF_BUFSIZE, "}, \"data\" : [");
    Join(buffer, work, bufsize);

    while (notdone)
    {
        Join(buffer, "[", bufsize);

        for (rp = master; rp != NULL; rp = rp->next)
        {
            ap = rp->item;

            ip = (Item *) rp->state_ptr;

            snprintf(work, CF_BUFSIZE, "\"%s\"", ip->name);
            Join(buffer, work, bufsize);

            if (rp->next)
            {
                Join(buffer, ",", bufsize);
            }

            ap = (CfAssoc *) rp->item;
            DeleteAssoc(ap);
            rp->item = NULL;

            if (ip->next)
            {
                rp->state_ptr = (void *) ip->next;      // This is a harmless hack
            }
            else
            {
                notdone = false;
            }
        }

        if (notdone)
        {
            Join(buffer, "],", bufsize);
        }
        else
        {
            Join(buffer, "]", bufsize);
        }
    }

    DeleteRlist(master);

    Join(buffer, "]}", bufsize);

    *errstr = NULL;
    return 0;
}

#else /* HAVE_LIBLDAP */

int CfLDAP_JSON_GetSeveralAttributes(char *uri, char *user, char *basedn, char *filter, Rlist *names, char *scopes,
                                     char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                     int bufsize, const char **const errstr)
{
    *errstr = "LDAP support is disabled";
    CfOut(cf_error, "", "LDAP support is disabled");
    return -1;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

int CfLDAP_JSON_GetSingleAttributeList(char *uri, char *authdn, char *basedn, char *filter, char *name, char *scopes,
                                       char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                       int bufsize, const char **const errstr)
{
    LDAP *ld;
    LDAPMessage *res, *msg;
    LDAPControl **serverctrls;
    BerElement *ber;
    struct berval **vals;
    char **referrals;
    int i, ret, parse_ret, num_entries = 0, num_refs = 0;
    char *a, *dn, *matched_msg = NULL, *error_msg = NULL;
    int scope = NovaStr2Scope(scopes), count = 0;
    Rlist *return_value = NULL, *rp;
    char work[CF_BUFSIZE];

    if (page == 0)
    {
        page = 1;
    }

    if (linesperpage == 0)
    {
        linesperpage = 1000;
    }

    if ((ld = NovaLDAPConnect(uri, starttls, errstr)) == NULL)
    {
        return -1;
    }

    if (NovaLDAPAuthenticate(ld, authdn, sec, passwd) != LDAP_SUCCESS)
    {
        return -1;
    }

    if ((ret =
         ldap_search_ext_s(ld, basedn, scope, filter, NULL, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &res)) != LDAP_SUCCESS)
    {
        CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));
        ldap_unbind(ld);
        *errstr = ldap_err2string(ret);
        return -1;
    }

    num_entries = ldap_count_entries(ld, res);
    num_refs = ldap_count_references(ld, res);

    for (msg = ldap_first_message(ld, res); msg != NULL; msg = ldap_next_message(ld, msg))
    {
        count++;

        if (count > page * linesperpage)
        {
            break;
        }

        if (count < linesperpage * (page - 1))
        {
            continue;
        }

        switch (ldap_msgtype(msg))
        {
        case LDAP_RES_SEARCH_ENTRY:

            if ((dn = ldap_get_dn(ld, msg)) != NULL)
            {
                CfOut(cf_verbose, "", " -> LDAP query found dn: %s\n", dn);
            }
            else
            {
                CfOut(cf_verbose, "", " !! No LDAP query found\n");
            }

            /* Iterate through each attribute in the entry. */

            for (a = ldap_first_attribute(ld, res, &ber); a != NULL; a = ldap_next_attribute(ld, res, ber))
            {
                /* Get and print all values for each attribute. */

                CfDebug(" ->   LDAP query found attribute %s\n", a);

                if ((vals = ldap_get_values_len(ld, msg, a)) != NULL)
                {
                    for (i = 0; vals[i] != NULL; i++)
                    {
                        if (strcmp(a, name) == 0)
                        {
                            CfOut(cf_verbose, "", "Located LDAP value %s => %s\n", a, vals[i]->bv_val);
                            AppendRScalar(&return_value, (char *) vals[i]->bv_val, CF_SCALAR);
                        }
                    }

                    ldap_value_free_len(vals);
                }

                ldap_memfree(a);
            }

            if (ber != NULL)
            {
                ber_free(ber, 0);
            }

            ldap_memfree(dn);
            break;

        case LDAP_RES_SEARCH_REFERENCE:

            /* The server sent a search reference encountered during the search operation. */
            /* Parse the result and print the search references.
               Ideally, rather than print them out, you would follow the references....what does this mean? */

            parse_ret = ldap_parse_reference(ld, msg, &referrals, NULL, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", "Unable to parse LDAP references: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                *errstr = ldap_err2string(parse_ret);
                return -1;
            }

            if (referrals != (char **) NULL)
            {
                for (i = 0; referrals[i] != NULL; i++)
                {
                    CfOut(cf_verbose, "", "Search reference: %s\n\n", referrals[i]);
                }

                ldap_value_free(referrals);
            }

            break;

        case LDAP_RES_SEARCH_RESULT:

            /* At the end, a result status is sent */

            CfOut(cf_verbose, "", " -> LDAP Query result received\n");

            parse_ret = ldap_parse_result(ld, msg, &ret, &matched_msg, &error_msg, NULL, &serverctrls, 0);

            if (parse_ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP Error parsed: %s\n", ldap_err2string(parse_ret));
                ldap_unbind(ld);
                *errstr = ldap_err2string(parse_ret);
                return -1;
            }

            /* Then check the results of the LDAP search operation. */

            if (ret != LDAP_SUCCESS)
            {
                CfOut(cf_error, "", " !! LDAP search failed: %s\n", ldap_err2string(ret));

                if (error_msg != NULL && *error_msg != '\0')
                {
                    CfOut(cf_error, "", "%s", error_msg);
                }

                if (matched_msg != NULL && *matched_msg != '\0')
                {
                    CfOut(cf_verbose, "", "Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
            }
            else
            {
                CfOut(cf_verbose, "", " -> LDAP search was successful, %d entries, %d references", num_entries,
                      num_refs);
            }
            break;

        default:
            break;

        }
    }

/* Format JSON */

    strcpy(buffer, "[");

    for (rp = return_value; rp != NULL; rp = rp->next)
    {
        if (rp->next)
        {
            snprintf(work, CF_BUFSIZE, "\"%s\",", (char *) rp->item);
        }
        else
        {
            snprintf(work, CF_BUFSIZE, "\"%s\",", (char *) rp->item);
        }
        Join(buffer, work, bufsize);
    }

    buffer[strlen(buffer) - 1] = ']';

/* Disconnect when done. */
    ldap_unbind(ld);
    *errstr = NULL;
    return 0;
}

#else /* HAVE_LIBLDAP */

int CfLDAP_JSON_GetSingleAttributeList(char *uri, char *user, char *basedn, char *filter, char *name, char *scopes,
                                       char *sec, char *passwd, bool starttls, int page, int linesperpage, char *buffer,
                                       int bufsize, const char **const errstr)
{
    CfOut(cf_error, "", "LDAP support is disabled");
    *errstr = "LDAP support is disabled";
    return 0;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

#ifdef HAVE_LIBLDAP

static LDAP *NovaLDAPConnect(const char *uri, bool starttls, const char **const errstr)
{
    LDAP *ld;
    int ret, version;

/* TLS options need to be set up before opening a connection */

/* Do not check supplied certificate -- we don't have CA certificate storage */

    int never = LDAP_OPT_X_TLS_NEVER;

    if ((ret = ldap_set_option(NULL, LDAP_OPT_X_TLS_REQUIRE_CERT, &never)) != LDAP_SUCCESS)
    {
        if (errstr)
        {
            *errstr = ldap_err2string(ret);
        }
        CfOut(cf_error, "", "Unable to set global LDAP TLS options: %s", ldap_err2string(ret));
        return NULL;
    }

/* Do not chase referrals, it is unlikely that they all use the same
 * credentials */

    if ((ret = ldap_set_option(NULL, LDAP_OPT_REFERRALS, LDAP_OPT_OFF)) != LDAP_SUCCESS)
    {
        if (errstr)
        {
            *errstr = ldap_err2string(ret);
        }
        CfOut(cf_error, "", "Unable to disable LDAP referrals chasing: %s", ldap_err2string(ret));
        return NULL;
    }

/* Get a handle to an LDAP connection. */

    if ((ret = ldap_initialize(&ld, uri)) != LDAP_SUCCESS)
    {
        if (errstr)
        {
            *errstr = ldap_err2string(ret);
        }
        CfOut(cf_error, "", "LDAP connection failed: %s", ldap_err2string(ret));
        return NULL;
    }

    version = LDAP_VERSION3;

    if ((ret = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version)) != LDAP_SUCCESS)
    {
        if (errstr)
        {
            *errstr = ldap_err2string(ret);
        }
        CfOut(cf_error, "", "Trouble setting LDAP option %s", ldap_err2string(ret));
        return NULL;
    }

    if (starttls)
    {
        if ((ret = ldap_start_tls_s(ld, NULL, NULL)) != LDAP_SUCCESS)
        {
            if (errstr)
            {
                if (ret == LDAP_CONNECT_ERROR)
                {
                    char *msg = NULL;

                    ldap_get_option(ld, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void *) &msg);
                    //tool_perror( "ldap_start_tls", rc, NULL, NULL, msg, NULL );
                    *errstr = xstrdup(msg);
                    ldap_memfree(msg);
                }
                else
                {
                    *errstr = ldap_err2string(ret);
                }
            }

            CfOut(cf_error, "", "Unable to negotiate TLS encryption with LDAP server: %s", ldap_err2string(ret));
            return NULL;
        }
    }

    return ld;
}

/*****************************************************************************/

static int NovaLDAPAuthenticate(LDAP *ld, const char *basedn, const char *sec, const char *password)
{
    int pwdLen = 0;
    int ret;

    if (password)
    {
        pwdLen = strlen(password);
    }

/* ldap_sasl_bind_s accepts non-constant string as a parameter */
    struct berval passwd = { pwdLen, (char *) password };

/* Bind to the server anonymously. */

    if (strcmp(sec, "sasl") == 0)
    {
        struct berval *servcred = NULL;

        ret = ldap_sasl_bind_s(ld, basedn, LDAP_SASL_SIMPLE, &passwd, NULL, NULL, &servcred);
    }
    else
    {
        ret = ldap_simple_bind_s(ld, NULL, NULL);
    }

    return ret;
}

/*****************************************************************************/

static int NovaStr2Scope(char *scope)
{
    if (strcmp(scope, "subtree") == 0)
    {
        return LDAP_SCOPE_SUBTREE;
    }

    if (strcmp(scope, "base") == 0)
    {
        return LDAP_SCOPE_BASE;
    }

    if (strcmp(scope, "onelevel") == 0)
    {
        return LDAP_SCOPE_ONELEVEL;
    }

    return LDAP_SCOPE_SUBTREE;
}

/*******************************************************************/

static Rlist *LDAPKeyInRlist(Rlist *list, char *key)
{
    Rlist *rp;
    CfAssoc *ap;

    for (rp = list; rp != NULL; rp = rp->next)
    {
        if (rp->type != CF_SCALAR)
        {
            continue;
        }

        ap = (CfAssoc *) rp->item;

        if (strcmp((char *) ap->lval, key) == 0)
        {
            return rp;
        }
    }

    return NULL;
}

#endif
