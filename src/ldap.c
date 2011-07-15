
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: ldap.c                                                              */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBLDAP

/* Prototypes */

int CfLDAPAuthenticate(char *uri,char *basedn,char *passwd)
LDAP *NovaQueryLDAP(char *uri,char *basedn,char *sec,char *pwd);
void *CfLDAPValue(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec);
void *CfLDAPList(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec);
void *CfLDAPArray(char *array,char *uri,char *basedn,char *filter,char *scopes,char *sec);
void *CfRegLDAP(char *uri,char *basedn,char *filter,char *name,char *scopes,char *regex,char *sec);

int NovaStr2Scope(char *scope);

#endif

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

int CfLDAPAuthenticate(char *uri,char *basedn,char *passwd)

{ LDAP *ld;
  
if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");
   return false;
   }

if ((ld = NovaQueryLDAP(uri,basedn,"sasl",passwd)) == NULL)
   {
   return false;
   }

return true;
}

/*****************************************************************************/

void *CfLDAPValue(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec)

{ LDAP *ld;
  LDAPMessage *res, *msg;
  LDAPControl **serverctrls;
  BerElement *ber;
  struct berval **vals;
  char **referrals;
  int version,i,ret,parse_ret,msgtype,num_entries = 0,num_refs = 0;
  char *a, *dn, *matched_msg = NULL, *error_msg = NULL,*return_value = NULL;
  int scope = NovaStr2Scope(scopes);
  

if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");
   return NULL;
   }

if ((ld = NovaQueryLDAP(uri,basedn,sec,NULL)) == NULL)
   {
   return NULL;
   }

if ((ret = ldap_search_ext_s(ld,basedn,scope,filter,NULL,0,NULL,NULL,NULL,LDAP_NO_LIMIT,&res)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
    ldap_unbind(ld);
   return NULL;
   }

num_entries = ldap_count_entries(ld,res);
num_refs = ldap_count_references(ld,res);

for (msg = ldap_first_message(ld,res); msg != NULL; msg = ldap_next_message(ld,msg))
   {
   msgtype = ldap_msgtype(msg);
   
   switch(msgtype)
      {
      case LDAP_RES_SEARCH_ENTRY:
          
          if ((dn = ldap_get_dn(ld,msg)) != NULL)
             {
             CfOut(cf_verbose,""," -> LDAP query found dn: %s\n",dn);
             }
          else
             {
             CfOut(cf_verbose,""," !! No LDAP query result found\n");
             break;
             }          
          
          /* Iterate through each attribute in the entry. */
          
          for (a = ldap_first_attribute(ld,msg,&ber); a != NULL; a = ldap_next_attribute(ld,msg,ber))
             {
             /* Get and print all values for each attribute. */

             if ((vals = ldap_get_values_len(ld,msg,a)) != NULL)
                {                
                for (i = 0; vals[i] != NULL; i++)
                   {
                   if (cf_strcmp(a,name) == 0)
                      {
                      CfOut(cf_verbose,"","Located LDAP value %s => %s\n", a,vals[i]);
                      return_value = strdup((char *)vals[i]->bv_val);
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
             ber_free(ber,0);             
             }

          ldap_memfree(dn);
          break;
          
      case LDAP_RES_SEARCH_REFERENCE:
          
       /* The server sent a search reference encountered during the search operation. */
       /* Parse the result and print the search references.
          Ideally, rather than print them out, you would follow the references....what does this mean? */
          
          parse_ret = ldap_parse_reference(ld,msg,&referrals,NULL,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {
             CfOut(cf_error,"","Unable to parse LDAP references: %s\n",ldap_err2string(parse_ret));
             ldap_unbind(ld);
             return NULL;
             }

          if (referrals != (char **)NULL)
             {
             for (i = 0; referrals[i] != NULL; i++)
                {                
                CfOut(cf_verbose,"","Search reference: %s\n\n",referrals[i]);
                }
             
             ldap_value_free(referrals);
             }
          
          break;
          
      case LDAP_RES_SEARCH_RESULT:

          /* At the end, a result status is sent */

          CfOut(cf_verbose,""," -> LDAP Query result received\n");

          parse_ret = ldap_parse_result(ld,msg,&ret,&matched_msg,&error_msg,NULL,&serverctrls,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP Error parsed: %s\n",ldap_err2string(parse_ret));             
             ldap_unbind(ld);             
             return NULL;
             }

          /* Then check the results of the LDAP search operation. */

          if (ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
             
             if (error_msg != NULL & *error_msg != '\0')
                {                
                CfOut(cf_error,"","%s", error_msg);                
                }
             
             if (matched_msg != NULL && *matched_msg != '\0')
                {                
                CfOut(cf_verbose,"","Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
             }
          else
             {          
             CfOut(cf_verbose,""," -> LDAP search successful, %d entries, %d references",num_entries,num_refs);
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
void *CfLDAPValue(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec)
{
CfOut(cf_error, "", "LDAP support is disabled");
return NULL;
}
#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP
void *CfLDAPList(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec)

{ LDAP *ld;
  LDAPMessage *res, *msg;
  LDAPControl **serverctrls;
  BerElement *ber;
  struct berval **vals;
  char **referrals;
  int version,i,ret,parse_ret,num_entries = 0,num_refs = 0;
  char *a, *dn, *matched_msg = NULL, *error_msg = NULL;
  int scope = NovaStr2Scope(scopes);
  struct Rlist *return_value = NULL;
  
if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");
   return NULL;
   }

if ((ld = NovaQueryLDAP(uri,basedn,sec,NULL)) == NULL)
   {
   return NULL;
   }

if ((ret = ldap_search_ext_s(ld,basedn,scope,filter,NULL,0,NULL,NULL,NULL,LDAP_NO_LIMIT,&res)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
   ldap_unbind(ld);
   return NULL;
   }

num_entries = ldap_count_entries(ld,res);
num_refs = ldap_count_references(ld,res);

for (msg = ldap_first_message(ld,res); msg != NULL; msg = ldap_next_message(ld,msg))
   {
   switch(ldap_msgtype(msg))
      {
      case LDAP_RES_SEARCH_ENTRY:
          
          if ((dn = ldap_get_dn(ld,msg)) != NULL)
             {
             CfOut(cf_verbose,""," -> LDAP query found dn: %s\n",dn);
             }
          else
             {
             CfOut(cf_verbose,""," !! No LDAP query found\n");
             }
          
          /* Iterate through each attribute in the entry. */
          
          for (a = ldap_first_attribute(ld,res,&ber); a != NULL; a = ldap_next_attribute(ld,res,ber))
             {
             /* Get and print all values for each attribute. */

             Debug(" ->   LDAP query found attribute %s",a);
             
             if ((vals = ldap_get_values_len(ld,msg,a)) != NULL)
                {                
                for (i = 0; vals[i] != NULL; i++)
                   {
                   if (cf_strcmp(a,name) == 0)
                      {
                      CfOut(cf_verbose,"","Located LDAP value %s => %s\n", a,vals[i]);
                      AppendRScalar(&return_value,(char *)vals[i]->bv_val,CF_SCALAR);
                      }
                   }
                
                ldap_value_free_len(vals);
                }
             
             ldap_memfree(a);
             }
          
          if (ber != NULL)
             {             
             ber_free(ber,0);             
             }

          ldap_memfree(dn);
          break;
          
      case LDAP_RES_SEARCH_REFERENCE:
          
       /* The server sent a search reference encountered during the search operation. */
       /* Parse the result and print the search references.
          Ideally, rather than print them out, you would follow the references....what does this mean? */
          
          parse_ret = ldap_parse_reference(ld,msg,&referrals,NULL,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {
             CfOut(cf_error,"","Unable to parse LDAP references: %s\n",ldap_err2string(parse_ret));
             ldap_unbind(ld);
             return NULL;
             }

          if (referrals != (char **)NULL)
             {
             for (i = 0; referrals[i] != NULL; i++)
                {                
                CfOut(cf_verbose,"","Search reference: %s\n\n",referrals[i]);
                }
             
             ldap_value_free(referrals);
             }
          
          break;
          
      case LDAP_RES_SEARCH_RESULT:

          /* At the end, a result status is sent */

          CfOut(cf_verbose,""," -> LDAP Query result received\n");

          parse_ret = ldap_parse_result(ld,msg,&ret,&matched_msg,&error_msg,NULL,&serverctrls,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP Error parsed: %s\n",ldap_err2string(parse_ret));             
             ldap_unbind(ld);             
             return NULL;
             }

          /* Then check the results of the LDAP search operation. */

          if (ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
             
             if (error_msg != NULL & *error_msg != '\0')
                {                
                CfOut(cf_error,"","%s", error_msg);                
                }
             
             if (matched_msg != NULL && *matched_msg != '\0')
                {                
                CfOut(cf_verbose,"","Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
             }
          else
             {          
             CfOut(cf_verbose,""," -> LDAP search was successful, %d entries, %d references",num_entries,num_refs);
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

void *CfLDAPList(char *uri,char *basedn,char *filter,char *name,char *scopes,char *sec)
{
CfOut(cf_error, "", "LDAP support is disabled");
return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

void *CfLDAPArray(char *array,char *uri,char *basedn,char *filter,char *scopes,char *sec)

{ LDAP *ld;
  LDAPMessage *res, *msg;
  LDAPControl **serverctrls;
  BerElement *ber;
  struct berval **vals;
  char **referrals;
  int version,i,ret,parse_ret,msgtype,num_entries = 0,num_refs = 0;
  char *a, *dn,*matched_msg = NULL, *error_msg = NULL;
  int scope = NovaStr2Scope(scopes);
  char name[CF_MAXVARSIZE],*return_value = NULL;
  
if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");
   return NULL;
   }

if ((ld = NovaQueryLDAP(uri,basedn,sec,NULL)) == NULL)
   {
   return NULL;
   }

if ((ret = ldap_search_ext_s(ld,basedn,scope,filter,NULL,0,NULL,NULL,NULL,LDAP_NO_LIMIT,&res)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
   ldap_unbind(ld);
   return NULL;
   }

num_entries = ldap_count_entries(ld,res);
num_refs = ldap_count_references(ld,res);

for (msg = ldap_first_message(ld,res); msg != NULL; msg = ldap_next_message(ld,msg))
   {
   msgtype = ldap_msgtype(msg);
   
   switch(msgtype)
      {
      case LDAP_RES_SEARCH_ENTRY:
          
          if ((dn = ldap_get_dn(ld,msg)) != NULL)
             {
             CfOut(cf_verbose,""," -> LDAP query found dn: %s\n",dn);
             ldap_memfree(dn);
             }
          
          /* Iterate through each attribute in the entry. */
          
          for (a = ldap_first_attribute(ld,msg,&ber); a != NULL; a = ldap_next_attribute(ld,msg,ber))
             {
             /* Get and print all values for each attribute. */

             if ((vals = ldap_get_values_len(ld,msg,a)) != NULL)
                {                
                for (i = 0; vals[i] != NULL; i++)
                   {
                   if (cf_strcmp(a,"objectClass") != 0) // This is non-unique
                      {
                      snprintf(name,CF_MAXVARSIZE-1,"%s[%s]",array,a);
                      NewScalar(THIS_BUNDLE,name,(char *)vals[i]->bv_val,cf_str);
                      CfOut(cf_verbose,""," -> Setting %s => %s, in context %s/%s\n",name,(char *)vals[i]->bv_val,THIS_BUNDLE,CONTEXTID);
                      return_value = strdup("any");

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
             ber_free(ber,0);             
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
          
          parse_ret = ldap_parse_reference(ld,msg,&referrals,NULL,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {
             CfOut(cf_error,"","Unable to parse LDAP references: %s\n",ldap_err2string(parse_ret));
             ldap_unbind(ld);
             return NULL;
             }

          if (referrals != (char **)NULL)
             {
             for (i = 0; referrals[i] != NULL; i++)
                {                
                CfOut(cf_verbose,"","Search reference: %s\n\n",referrals[i]);
                }
             
             ldap_value_free(referrals);
             }
          
          break;
          
      case LDAP_RES_SEARCH_RESULT:

          /* At the end, a result status is sent */

          CfOut(cf_verbose,""," -> LDAP Query result received\n");

          parse_ret = ldap_parse_result(ld,msg,&ret,&matched_msg,&error_msg,NULL,&serverctrls,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP Error parsed: %s\n",ldap_err2string(parse_ret));             
             ldap_unbind(ld);             
             return NULL;
             }

          /* Then check the results of the LDAP search operation. */

          if (ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
             
             if (error_msg != NULL & *error_msg != '\0')
                {                
                CfOut(cf_error,"","%s", error_msg);                
                }
             
             if (matched_msg != NULL && *matched_msg != '\0')
                {                
                CfOut(cf_verbose,"","Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
             }
          else
             {          
             CfOut(cf_verbose,""," -> LDAP search successful, %d entries, %d references",num_entries,num_refs);
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
   return_value = strdup("!any");
   }

ldap_unbind(ld);
return return_value;
}

#else /* HAVE_LIBLDAP */

void *CfLDAPArray(char *array,char *uri,char *basedn,char *filter,char *scopes,char *sec)
{
CfOut(cf_error, "", "LDAP support is disabled");
return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/

#ifdef HAVE_LIBLDAP

void *CfRegLDAP(char *uri,char *basedn,char *filter,char *name,char *scopes,char *regex,char *sec)

{ LDAP *ld;
  LDAPMessage *res, *msg;
  LDAPControl **serverctrls;
  BerElement *ber;
  struct berval **vals;
  char **referrals;
  int version,i,ret,parse_ret,num_entries = 0,num_refs = 0;
  char *a, *dn, *matched_msg = NULL, *error_msg = NULL,*return_value = NULL;
  int scope = NovaStr2Scope(scopes);
  
if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");
   return NULL;
   }

if ((ld = NovaQueryLDAP(uri,basedn,sec,NULL)) == NULL)
   {
   return NULL;
   }

if ((ret = ldap_search_ext_s(ld,basedn,scope,filter,NULL,0,NULL,NULL,NULL,LDAP_NO_LIMIT,&res)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
   ldap_unbind(ld);
   return NULL;
   }

num_entries = ldap_count_entries(ld,res);
num_refs = ldap_count_references(ld,res);

for (msg = ldap_first_message(ld,res); msg != NULL; msg = ldap_next_message(ld,msg))
   {
   switch(ldap_msgtype(msg))
      {
      case LDAP_RES_SEARCH_ENTRY:
          
          if ((dn = ldap_get_dn(ld,msg)) != NULL)
             {
             CfOut(cf_verbose,""," -> LDAP query found dn: %s\n",dn);
             ldap_memfree(dn);
             }
          
          /* Iterate through each attribute in the entry. */
          
          for (a = ldap_first_attribute(ld,msg,&ber); a != NULL; a = ldap_next_attribute(ld,msg,ber))
             {
             /* Get and print all values for each attribute. */

             if ((vals = ldap_get_values_len(ld,msg,a)) != NULL)
                {                
                for (i = 0; vals[i] != NULL; i++)
                   {
                   if (cf_strcmp(a,name) == 0 && FullTextMatch(regex,(char *)vals[i]->bv_val))
                      {
                      CfOut(cf_verbose,""," -> Located regex matching LDAP value %s => %s\n", a,(char *)vals[i]->bv_val);
                      return_value = strdup("any");
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
             ber_free(ber,0);             
             }

          break;
          
      case LDAP_RES_SEARCH_REFERENCE:
          
       /* The server sent a search reference encountered during the search operation. */
       /* Parse the result and print the search references.
          Ideally, rather than print them out, you would follow the references....what does this mean? */
          
          parse_ret = ldap_parse_reference(ld,msg,&referrals,NULL,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {
             CfOut(cf_error,"","Unable to parse LDAP references: %s\n",ldap_err2string(parse_ret));
             ldap_unbind(ld);
             return NULL;
             }

          if (referrals != (char **)NULL)
             {
             for (i = 0; referrals[i] != NULL; i++)
                {                
                CfOut(cf_verbose,"","Search reference: %s\n\n",referrals[i]);
                }
             
             ldap_value_free(referrals);
             }
          
          break;
          
      case LDAP_RES_SEARCH_RESULT:

          /* At the end, a result status is sent */

          CfOut(cf_verbose,""," -> LDAP Query result received\n");

          parse_ret = ldap_parse_result(ld,msg,&ret,&matched_msg,&error_msg,NULL,&serverctrls,0);
          
          if (parse_ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP Error parsed: %s\n",ldap_err2string(parse_ret));             
             ldap_unbind(ld);             
             return NULL;
             }

          /* Then check the results of the LDAP search operation. */

          if (ret != LDAP_SUCCESS)
             {             
             CfOut(cf_error,""," !! LDAP search failed: %s\n",ldap_err2string(ret));
             
             if (error_msg != NULL & *error_msg != '\0')
                {                
                CfOut(cf_error,"","%s", error_msg);                
                }
             
             if (matched_msg != NULL && *matched_msg != '\0')
                {                
                CfOut(cf_verbose,"","Part of the DN that matches an existing entry: %s\n", matched_msg);
                }
             }
          else
             {          
             CfOut(cf_verbose,""," -> LDAP search successful, %d entries, %d references",num_entries,num_refs);
             }          
          break;
          
      default:
          CfOut(cf_verbose,"","Unknown message received\n");
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
   return_value = strdup("!any");
   }

return return_value;
}

#else /* HAVE_LIBLDAP */

void *CfRegLDAP(char *uri,char *basedn,char *filter,char *name,char *scopes,char *regex,char *sec)
{
CfOut(cf_error, "", "LDAP support is disabled");
return NULL;
}

#endif /* HAVE_LIBLDAP */

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

#ifdef HAVE_LIBLDAP

LDAP *NovaQueryLDAP(char *uri,char *basedn,char *sec,char *password)

{ LDAP *ld;
  char *matched_msg = NULL, *error_msg = NULL;
  int ret,version;
  struct berval passwd = { strlen(password), password };

/* Get a handle to an LDAP connection. */

if (LICENSES == 0)
   {
   CfOut(cf_error,""," !! The commercial license has expired, this function is not available");   
   return NULL;
   }

if ((ret = ldap_initialize(&ld,uri)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,"","LDAP connection failed: %s",ldap_err2string(ret));   
   return NULL;
   }

version = LDAP_VERSION3;

if ((ret = ldap_set_option(ld,LDAP_OPT_PROTOCOL_VERSION,&version)) != LDAP_SUCCESS)
   {
   CfOut(cf_error,"","Trouble setting LDAP option %s",ldap_err2string(ret));   
   ldap_unbind(ld);
   return NULL;
   }

/* Bind to the server anonymously. */

if (cf_strcmp(sec,"sasl") == 0)
   {
   int err;
   ret = ldap_sasl_bind_s(ld,basedn,LDAP_SASL_SIMPLE,&passwd,NULL,NULL,&err);

   if (err == -1)
      {
      CfLog(cf_verbose,"ldap_sasl_bind"," !! Unable to authenticate with given credentials");
      }
   }
else
   {
   ret = ldap_simple_bind_s(ld,NULL,NULL);
   }

if (ret != LDAP_SUCCESS)
   {
   CfOut(cf_inform,"","Trouble binding to LDAP: %s",ldap_err2string(ret));   
   ldap_unbind(ld);
   return NULL;
   }

return ld;
}

/*****************************************************************************/

int NovaStr2Scope(char *scope)

{
if (cf_strcmp(scope,"subtree") == 0)
   {
   return LDAP_SCOPE_SUBTREE;
   }

if (cf_strcmp(scope,"base") == 0)
   {
   return LDAP_SCOPE_BASE;
   }

if (cf_strcmp(scope,"onelevel") == 0)
   {
   return LDAP_SCOPE_ONELEVEL;
   }

return LDAP_SCOPE_SUBTREE;
}

#endif
