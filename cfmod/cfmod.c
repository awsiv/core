/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 252479 2008-02-07 19:39:50Z iliaa $ */

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cfmod.h"
#include "zend_exceptions.h"


/* If you declare any globals in php_cfmod.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(cfmod)
*/

/* True global resources - no need for thread safety here */
static int le_cfmod;

static zend_class_entry *cfmod_ldap_exception_ce;

static function_entry cfmod_functions[] =
    {
    PHP_FE(cfpr_enterprise_version,NULL)
    PHP_FE(cfpr_community_version,NULL)
    PHP_FE(cfpr_getlicense_summary,NULL)
    PHP_FE(cfpr_getlicense_expiry,NULL)
    PHP_FE(cfpr_getlicense_owner,NULL)
    PHP_FE(cfpr_getlicenses_promised,NULL)
    PHP_FE(cfpr_getlicenses_granted,NULL)
    PHP_FE(cfpr_getlicense_installtime,NULL)
    PHP_FE(cfpr_docroot,NULL)
    PHP_FE(cfpr_get_class_frequency,NULL)
    PHP_FE(cfpr_report_class_frequency,NULL)
    PHP_FE(cfpr_get_network_rate,NULL)
   
    PHP_FE(cfpr_summary_meter,NULL)
    PHP_FE(cfpr_host_meter,NULL)
    PHP_FE(cfpr_report_overall_summary,NULL)

    PHP_FE(cfpr_ldap_authenticate,NULL)
    PHP_FE(cfpr_ldap_get_single_attribute_list,NULL)
    PHP_FE(cfpr_ldap_get_several_attributes,NULL)

    PHP_FE(cfpr_vitals_list,NULL)
    PHP_FE(cfpr_vitals_view_magnified,NULL)
    PHP_FE(cfpr_vitals_view_week,NULL)
    PHP_FE(cfpr_vitals_view_year,NULL)
    PHP_FE(cfpr_vitals_view_histogram,NULL)
    PHP_FE(cfpr_vitals_analyse_magnified,NULL)
    PHP_FE(cfpr_vitals_analyse_week,NULL)
    PHP_FE(cfpr_vitals_analyse_year,NULL)
    PHP_FE(cfpr_vitals_analyse_histogram,NULL)
    
    PHP_FE(cfpr_getlastupdate,NULL)
    PHP_FE(cfpr_list_all_bundles,NULL)
    PHP_FE(cfpr_get_bundle_type,NULL)
    PHP_FE(cfpr_list_bundles_using,NULL)
    PHP_FE(cfpr_get_bundle_count,NULL)
    PHP_FE(cfpr_get_promise_count,NULL)
    PHP_FE(cfpr_list_bodies,NULL)
    PHP_FE(cfpr_list_business_goals,NULL)

    PHP_FE(cfpr_report_software_in,NULL)
    PHP_FE(cfpr_report_patch_in,NULL)
    PHP_FE(cfpr_report_patch_avail,NULL)
    PHP_FE(cfpr_report_classes,NULL)
    PHP_FE(cfpr_class_cloud,NULL)    
    PHP_FE(cfpr_report_vars,NULL)
    PHP_FE(cfpr_report_compliance_summary,NULL)
    PHP_FE(cfpr_compliance_summary_graph,NULL)
    PHP_FE(cfpr_report_compliance_promises,NULL)
    PHP_FE(cfpr_report_lastseen,NULL)
    PHP_FE(cfpr_report_performance,NULL)
    PHP_FE(cfpr_report_setuid,NULL)
    PHP_FE(cfpr_report_filechanges,NULL)
    PHP_FE(cfpr_report_filediffs,NULL)
    PHP_FE(cfpr_report_bundlesseen,NULL)

    PHP_FE(cfpr_report_filechanges_longterm,NULL)
    PHP_FE(cfpr_report_filediffs_longterm,NULL)

    PHP_FE(cfpr_list_all_classes,NULL)
    PHP_FE(cfpr_list_time_classes,NULL)
    PHP_FE(cfpr_list_soft_classes,NULL)
    PHP_FE(cfpr_list_ip_classes,NULL)
    PHP_FE(cfpr_list_host_classes,NULL)

    PHP_FE(cfpr_summarize_promise,NULL)
    PHP_FE(cfpr_list_handles,NULL)
    PHP_FE(cfpr_list_handles_for_bundle,NULL)
    PHP_FE(cfpr_get_handles_for_bundle_with_comments,NULL)
    PHP_FE(cfpr_get_promise_comment,NULL)
    PHP_FE(cfpr_get_help_text,NULL)
    PHP_FE(cfpr_get_promise_bundle,NULL)
    PHP_FE(cfpr_get_promise_type,NULL)
    PHP_FE(cfpr_get_promiser,NULL)
    PHP_FE(cfpr_get_variable,NULL)
    PHP_FE(cfpr_get_classes_for_bundle,NULL)
    PHP_FE(cfpr_get_args_for_bundle,NULL)
    PHP_FE(cfpr_get_promise_body,NULL)
    PHP_FE(cfpr_get_value_graph,NULL)

    PHP_FE(cfpr_policy_finder_by_handle,NULL)
    PHP_FE(cfpr_policy_finder_by_bundle,NULL)
    PHP_FE(cfpr_policy_finder_by_promiser,NULL)

    PHP_FE(cfpr_report_value,NULL)
    PHP_FE(cfpr_report_notkept,NULL)
    PHP_FE(cfpr_report_repaired,NULL)
    PHP_FE(cfpr_summarize_notkept,NULL)
    PHP_FE(cfpr_summarize_repaired,NULL)
   
    PHP_FE(cfpr_get_knowledge_view,NULL)

    PHP_FE(cfpr_top_n_hosts,NULL)

    PHP_FE(cfpr_select_hosts,NULL)
    PHP_FE(cfpr_select_reports,NULL)
    PHP_FE(cfpr_show_red_hosts,NULL)
    PHP_FE(cfpr_show_yellow_hosts,NULL)
    PHP_FE(cfpr_show_green_hosts,NULL)
    PHP_FE(cfpr_show_blue_hosts,NULL)

    PHP_FE(cfpr_show_hosts_ip,NULL)
    PHP_FE(cfpr_show_hosts_name,NULL)

    PHP_FE(cfpr_get_host_colour,NULL)

    PHP_FE(cfpr_count_all_hosts,NULL)
    PHP_FE(cfpr_count_red_hosts,NULL)
    PHP_FE(cfpr_count_yellow_hosts,NULL)
    PHP_FE(cfpr_count_green_hosts,NULL)
    PHP_FE(cfpr_count_blue_hosts,NULL)
   
    PHP_FE(cfpr_hosts_with_software_in,NULL)
    PHP_FE(cfpr_hosts_with_patch_in,NULL)
    PHP_FE(cfpr_hosts_with_patch_avail,NULL)
    PHP_FE(cfpr_hosts_with_classes,NULL)
    PHP_FE(cfpr_hosts_with_vars,NULL)
    PHP_FE(cfpr_hosts_with_compliance_summary,NULL)
    PHP_FE(cfpr_hosts_with_compliance_promises,NULL)
    PHP_FE(cfpr_hosts_with_lastseen,NULL)
    PHP_FE(cfpr_hosts_with_performance,NULL)
    PHP_FE(cfpr_hosts_with_setuid,NULL)
    PHP_FE(cfpr_hosts_with_filechanges,NULL)
    PHP_FE(cfpr_hosts_with_filediffs,NULL)
    PHP_FE(cfpr_hosts_with_repaired,NULL)
    PHP_FE(cfpr_hosts_with_notkept,NULL)
    PHP_FE(cfpr_hosts_with_bundlesseen,NULL)
    PHP_FE(cfpr_hosts_with_value,NULL)
   
    PHP_FE(cfpr_hostname,NULL)
    PHP_FE(cfpr_ipaddr,NULL)

    PHP_FE(cfpr_get_story_by_name,NULL)
    PHP_FE(cfpr_get_story_by_id,NULL)

    PHP_FE(cfpr_show_topic,NULL)
    PHP_FE(cfpr_search_topics,NULL)
    PHP_FE(cfpr_show_topic_leads,NULL)
    PHP_FE(cfpr_show_topic_hits,NULL)
    PHP_FE(cfpr_show_topic_category,NULL)
    PHP_FE(cfpr_get_pid_for_topic,NULL)
   
    PHP_FE(cfpr_header,NULL)
    PHP_FE(cfpr_footer,NULL)
    PHP_FE(cfpr_menu,NULL)

    /*
     * For pdf generation
     */
    PHP_FE(cfpr_report_compliance_summary_pdf,NULL)
    PHP_FE(cfpr_report_notkept_pdf,NULL)
    PHP_FE(cfpr_report_bundlesseen_pdf,NULL)
    PHP_FE(cfpr_report_value_pdf,NULL)
    PHP_FE(cfpr_report_classes_pdf,NULL)	
    PHP_FE(cfpr_report_compliance_promises_pdf,NULL)
    PHP_FE(cfpr_report_filechanges_pdf,NULL)
    PHP_FE(cfpr_report_lastseen_pdf,NULL)
    PHP_FE(cfpr_report_patch_avail_pdf,NULL)
    PHP_FE(cfpr_report_patch_in_pdf,NULL)
    PHP_FE(cfpr_report_software_in_pdf,NULL)
    PHP_FE(cfpr_report_performance_pdf,NULL)
    PHP_FE(cfpr_report_repaired_pdf,NULL)
    PHP_FE(cfpr_summarize_repaired_pdf,NULL)
    PHP_FE(cfpr_summarize_notkept_pdf,NULL)
    PHP_FE(cfpr_report_vars_pdf,NULL)   
    PHP_FE(cfpr_report_filediffs_pdf,NULL)																											    
    PHP_FE(cfpr_report_setuid_pdf,NULL)
    PHP_FE(cfpr_report_description,NULL)

    PHP_FE(cfpr_list_documents,NULL)

    /*
     * SVN helper
     */	
    PHP_FE(cfpr_validate_policy,NULL)
    /*
     *  CDP reports
     */
    PHP_FE(cfpr_cdp_reportnames,NULL)
    PHP_FE(cfpr_cdp_report,NULL)

    /*
     * Commenting
     */
    PHP_FE(cfpr_add_note,NULL)  
    PHP_FE(cfpr_new_note,NULL)  
    PHP_FE(cfpr_query_note,NULL)      
    PHP_FE(cfpr_get_host_noteid,NULL)      

    PHP_FE(cfpr_delete_host,NULL)

    /*
     * Replica sets
     */
    
    PHP_FE(cfpr_replica_status,NULL)

    /*
     * Environments
     */
    PHP_FE(cfpr_environments_list, NULL)
    PHP_FE(cfpr_environment_contents, NULL)
    PHP_FE(cfpr_get_host_environment, NULL)

    PHP_FE(cfpr_hub_key,NULL)
    PHP_FE(cfpr_get_hub_master,NULL)
    /*
     * Constellation
     */

    
#ifdef HAVE_CONSTELLATION    
    
    PHP_FE(cfcon_compliance_summary_graph,NULL)
    PHP_FE(cfcon_compliance_virtualbundle_graph,NULL)
    PHP_FE(cfcon_count_repaired,NULL)
    PHP_FE(cfcon_count_notkept,NULL)
    PHP_FE(cfcon_reasons_repaired,NULL)
    PHP_FE(cfcon_reasons_notkept,NULL)
   
    PHP_FE(cfcon_count_hubs,NULL)
    PHP_FE(cfcon_show_hubs,NULL)

    PHP_FE(cfcon_environments_list, NULL)

    PHP_FE(cfcon_promise_popularity, NULL)
    PHP_FE(cfcon_rank_promise_popularity, NULL)

    PHP_FE(cfcon_list_subscriptions, NULL)
    PHP_FE(cfcon_local_show_subscription_virtualbundle, NULL)
    
    PHP_FE(cfcon_subscribe_software, NULL)
    PHP_FE(cfcon_local_subscribe_virtualbundle, NULL)
    PHP_FE(cfcon_local_delete_subscription_virtualbundle, NULL)

    
    
    PHP_FE(cfcon_subscribe_repairlog, NULL)
//    PHP_FE(cfcon_subscribe_notkeptlog, NULL)
    
    
    
    PHP_FE(cfcon_report_software, NULL)
    PHP_FE(cfcon_local_report_virtualbundle, NULL)
    
    
    PHP_FE(cfcon_aggr_filechange, NULL)
    PHP_FE(cfcon_aggr_software, NULL)
    PHP_FE(cfcon_aggr_classes, NULL)
    PHP_FE(cfcon_aggr_repaired,NULL)
    PHP_FE(cfcon_aggr_notkept,NULL)
    PHP_FE(cfcon_hub_meter,NULL)
    PHP_FE(cfcon_get_hub_colour,NULL)
    PHP_FE(cfcon_count_hubs_colour,NULL)
    PHP_FE(cfcon_count_hub_hosts,NULL)
    PHP_FE(cfcon_list_hub_colour,NULL)
    PHP_FE(cfcon_value_graph,NULL)    
    
#endif  /* HAVE_CONSTELLATION */

    
    {NULL, NULL, NULL}	/* Must be the last line in cfmod_functions[] */
    };

zend_module_entry cfmod_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "cfmod",
    cfmod_functions,
    PHP_MINIT(cfmod),
    PHP_MSHUTDOWN(cfmod),
    PHP_RINIT(cfmod),		/* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(cfmod),	/* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(cfmod),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CFMOD
ZEND_GET_MODULE(cfmod)
#endif

PHP_MINIT_FUNCTION(cfmod)
{
zend_class_entry cfmod_ldap_exception_class_entry;
zend_class_entry *exception_class_entry = zend_exception_get_default(TSRMLS_CC);

INIT_CLASS_ENTRY(cfmod_ldap_exception_class_entry, "CfmodLdapException", NULL);

cfmod_ldap_exception_ce = zend_register_internal_class_ex(
    &cfmod_ldap_exception_class_entry,
    exception_class_entry,
    NULL TSRMLS_CC);

return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(cfmod)
{
 /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
 */
 return SUCCESS;
}

PHP_RINIT_FUNCTION(cfmod)
{
 return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */

PHP_RSHUTDOWN_FUNCTION(cfmod)
{
 return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cfmod)
{
 php_info_print_table_start();
 php_info_print_table_header(2, "cfmod support", "enabled");
 php_info_print_table_end();
 
 /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
 */
}


/******************************************************************************/
/* BEGIN                                                                      */
/******************************************************************************/

/* type key - single control string with varargs
   
l _ long
d _ double
s _ string (with possible null bytes) and its length
b _ boolean

char *s;
int s_len;
zval *param;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
"lsz", &l, &s, &s_len, &param) == FAILURE)
{
return;
}
*/

/******************************************************************************/


struct Rlist
   {
   void *item;
   char type;
   struct Rlist *state_ptr; /* Points to "current" state/element of sub-list */
   struct Rlist *next;
   };

enum promiselog_rep
   {
   plog_repaired,
   plog_notkept
   };

typedef struct PageInfo
{
  int resultsPerPage;
  int pageNum;
  int totalResultCount;
}PageInfo_t;

#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"
#define CF_WEBBUFFER 7*1024*1024

typedef enum cdp_report
   {
   cdp_acls,
   cdp_commands,
   cdp_filechanges,
   cdp_filediffs,
   cdp_registry,
   cdp_services,
   cdp_unknown
   }
  cdp_t;

struct Rlist *SplitStringAsRList(char *string,char sep);


#include "../nova/src/cf.nova.web_api.h"

#ifdef HAVE_CONSTELLATION
# include "../constellation/src/cf.constellation.web_api.h"
#endif

static time_t DeltaHrsConvert(long hrsAgo);
char **String2StringArray(char *str, char separator);
void FreeStringArray(char **strs);
static bool ParseVitalsArgs(int argc, char **retHostKey, char **retVitalId);
static char *cfcon_aggr_promiselog(int argc, enum promiselog_rep log_type);
static char *cfcon_count_promiselog(int argc, enum promiselog_rep log_type);
static char *cfcon_reasons_promiselog(int argc, enum promiselog_rep log_type);

/******************************************************************************/
/* API                                                                        */
/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_authenticate)
{
char *host,*dn,*passwd;
int h_len, d_len, p_len;
const char *errmsg;
zend_bool starttls = false;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|b",&host,&h_len,&dn,&d_len,&passwd,&p_len,&starttls) == FAILURE)
   {
   php_printf("Error is cfpr_ldap_authenticate args");
   RETURN_NULL();
   }

if (Nova2PHP_LDAPAuthenticate(host, dn, passwd, starttls, &errmsg) == true)
   {
   RETURN_LONG(true);
   }
else
   {
   if (errmsg == NULL)
      {
      /* Authentication failed */
      RETURN_LONG(false);
      }
   else
      {
      zend_throw_exception(cfmod_ldap_exception_ce, (char *)errmsg, 0 TSRMLS_CC);
      }
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_get_single_attribute_list)


/* cfpr_ldap_get_single_attribute_list(
                "ldap://10.0.0.100",
                "uid=sudhir",
                "cn=users,dc=cf022osx,dc=cfengine,dc=com",
                "(uid=sudhir)",
                "givenName",
                "subtree",
                "sasl",
                "q1w2e3r4t5",
                FALSE,
                1,
                100) == 0)
                {
                printf("JSON: %s\n",buffer);
                }
*/

{
char *uri,*host,*basedn,*filter,*name,*scopes,*sec,*passwd,*user;
int u_len,h_len, d_len, f_len,n_len,s_len,ss_len,p_len,page,linesperpage,uu_len;
char buffer[1000000];
const char *errmsg;
zend_bool starttls = false;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssssll|b",
                          &uri,&u_len,
                          &user,&uu_len,                          
                          &basedn,&d_len,
                          &filter,&f_len,
                          &name,&n_len,
                          &scopes,&s_len,
                          &sec,&ss_len,
                          &passwd,&p_len,
                          &page,
                          &linesperpage,
                          &starttls
                          ) == FAILURE)
   {
   php_printf("Error is cfpr_ldap_get_single_attribute_list args");
   RETURN_NULL();
   }

buffer[0]='\0';

if (Nova2PHP_LDAPGetSingleAttributeList(uri,user,basedn,filter,name,scopes,sec,passwd,starttls,page,linesperpage,buffer,1000000,&errmsg) == 0)
   {
   RETURN_STRING(buffer,1);
   }
else
   {
   zend_throw_exception(cfmod_ldap_exception_ce, (char *)errmsg, 0 TSRMLS_CC);
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_get_several_attributes)

/*

cfpr_ldap_get_several_attributes(
               "ldap://10.0.0.152",
               "uid=sudhir",
               "ou=people,dc=cfengine,dc=com",
               "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))" ,
               "uid,mail,sn,altSecurityIdentities", // search ids
               "subtree",
               "sasl",
               "password")

*/
    
{
struct Rlist *names = NULL;
char *uri,*host,*basedn,*filter,*name,*scopes,*sec,*passwd,*user;
int ret,u_len,h_len, d_len, f_len,n_len,s_len,ss_len,p_len,page,linesperpage,uu_len;
char buffer[1000000];
const char *errstr;
zend_bool starttls;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssssll|b",
                          &uri,&u_len,
                          &user,&uu_len,                          
                          &basedn,&d_len,
                          &filter,&f_len,
                          &name,&n_len,
                          &scopes,&s_len,
                          &sec,&ss_len,
                          &passwd,&p_len,
                          &page,
                          &linesperpage,
                          &starttls
                          ) == FAILURE)
   {
   php_printf("Error is cfpr_ldap_get_several_attributes args");
   RETURN_NULL();
   }

names = SplitStringAsRList(name,',');

buffer[0]='\0';
ret = Nova2PHP_LDAPGetSeveralAttributes(uri,user,basedn,filter,names,scopes,sec,passwd,starttls,page,linesperpage,buffer,1000000, &errstr);

DeleteRlist(names);

if (ret == 0)
   {
   RETURN_STRING(buffer,1);
   }
else
   {
   zend_throw_exception(cfmod_ldap_exception_ce, (char *)errstr, 0 TSRMLS_CC);
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_enterprise_version)

{ const int bufsize = 256;
 char buf[bufsize];

 buf[0] = '\0';

 Nova2PHP_enterprise_version(buf,bufsize);

 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_community_version)

{
 const int bufsize = 256;
 char buf[bufsize];

 buf[0] = '\0';
 Nova2PHP_community_version(buf,bufsize);
 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_header)

{ char *title,*device;
  int t_len, d_len;
  const int bufsize = 512*1024; 
  char buffer[bufsize];

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&title,&t_len,&device,&d_len) == FAILURE)
   {
   php_printf("Error is cfpr_header args");
   RETURN_NULL();
   }

Nova_Header(title,device,buffer,bufsize);
//php_printf("%s\n",buffer);
//Nova_TabMenu(buffer,bufsize);
php_printf("%s\n",buffer);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_menu)

{ char *title,*device;
 int t_len, d_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&title,&t_len) == FAILURE)
    {
    php_printf("Error is cfpr_menu args");
    RETURN_NULL();
    }

 Nova_TabMenu(title,buffer,bufsize);
 php_printf("%s\n",buffer);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_footer)

{ const int bufsize = 512*1024; 
 char buffer[bufsize];

 Nova_Footer(buffer,bufsize);
 php_printf("%s\n",buffer);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_summary)

{ char s[4096] = {0};

 s[0] = '\0';

 NewClass("am_policy_hub");
 CFDB_GetValue("license_report",s,sizeof(s));

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"{\"error\":\"Too early to retrieve license details, please try again later\"}");
    }

 RETURN_STRING(s,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_hub_key)

{ const int bufsize = 4096; 
 char buffer[bufsize];

 buffer[0] = '\0';
 NewClass("am_policy_hub");
 if( Nova2PHP_GetHubKey(buffer,bufsize))
    {
      RETURN_STRING(buffer,1);
    }

 RETURN_STRING("Cannot find Hub hostkey!",1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_hub_master)

{ const int bufsize = 4096; 
 char buffer[bufsize];

 buffer[0] = '\0';
 
 if( Nova2PHP_GetHubMaster(buffer,bufsize))
    {
      RETURN_STRING(buffer,1);
    }

 RETURN_STRING("Unknown Hub Master!",1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_expiry)

{ char s[4096];

 s[0] = '\0';

 NewClass("am_policy_hub");
 CFDB_GetValue("license_expires",s,sizeof(s));

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"no license");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_installtime)

{ char s[4096];

 s[0] = '\0';

 NewClass("am_policy_hub");
 CFDB_GetValue("license_installtime",s,sizeof(s));

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"no license");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_owner)

{ char s[4096];

 s[0] = '\0';
 NewClass("am_policy_hub");
 CFDB_GetValue("license_owner",s,sizeof(s));

 if (strlen(s) < 3)
    {
    snprintf(s,sizeof(s),"unknown licensee");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicenses_promised)

{ char s[4096];
 int i = -1;
 
 s[0] = '\0';
 NewClass("am_policy_hub");
 CFDB_GetValue("licenses_promised",s,sizeof(s));

 sscanf(s,"%d",&i);
 if (i == -1)
    {
    snprintf(s,sizeof(s),"No licenses promised");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicenses_granted)

{ char s[4096];
 int i = -1;

 NewClass("am_policy_hub");
 CFDB_GetValue("licenses_granted",s,sizeof(s));
 sscanf(s,"%d",&i);
 if (i == -1)
    {
    snprintf(s,sizeof(s),"No license granted!");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hostname)

{ char s1[4096],s2[4096];
 char *hostkey,*fhostkey;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error is cfpr_hostname function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 s1[0]='\0';
 s2[0]='\0';
 Nova2PHP_hostinfo(hostkey,s1,s2,sizeof(s1));

 RETURN_STRING(s1,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ipaddr)

{ char s1[4096],s2[4096];
 char *hostkey,*fhostkey;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error is cfpr_ipaddr function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 s1[0]='\0';
 s2[0]='\0';
 Nova2PHP_hostinfo(hostkey,s1,s2,sizeof(s1));
 RETURN_STRING(s2,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summary_meter)

{ const int bufsize = 512*1024; 
 char buffer[bufsize];

 buffer[0]='\0';
 Nova2PHP_summary_meter(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_meter)

{ char *hostkey,*fhostkey,*name;
 int hk_len, n_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_host_meter function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 buffer[0]='\0';
 Nova2PHP_meter(fhostkey,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

PHP_FUNCTION(cfpr_vitals_list)

{ char *hostkey,*fhostkey;
 int hk_len;
 const int bufsize = 4096;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    RETURN_NULL();
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 if(!fhostkey)
    {
    php_printf("cfpr_vitals_list: error: hostkey is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_list(fhostkey, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_magnified)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_magnified: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_magnified: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_magnified(fhostkey, fvitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_week)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_week: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_week: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_week(fhostkey, fvitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_year)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_year: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_year: error: vitalid is undefined\n");
    RETURN_NULL();
    }
  
 buffer[0]='\0';
 Nova2PHP_vitals_view_year(fhostkey, fvitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_histogram)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_histogram: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_histogram: error: vitalid is undefined\n");
    RETURN_NULL();
    }
  
 buffer[0]='\0';
 Nova2PHP_vitals_view_histogram(fhostkey, fvitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_magnified)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_magnified: Error while parsing arguments\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_magnified(hostkey, vitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_week)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_week: Error while parsing arguments\n");
    RETURN_NULL();
    }
 
 buffer[0]='\0';
 Nova2PHP_vitals_analyse_week(hostkey, vitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_year)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_year: Error while parsing arguments\n");
    RETURN_NULL();
    }
 
 buffer[0]='\0';
 Nova2PHP_vitals_analyse_year(hostkey, vitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_histogram)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_histogram: Error while parsing arguments\n");
    RETURN_NULL();
    }
 
 buffer[0]='\0';
 Nova2PHP_vitals_analyse_histogram(hostkey, vitalId, buffer, bufsize);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

static bool ParseVitalsArgs(int argc, char **retHostKey, char **retVitalId)
{
 char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 
 if (zend_parse_parameters(argc, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    return false;
    }
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("ParseVitalsParams: error: hostkey is undefined\n");
    return false;
    }

  if(!fvitalId)
    {
    php_printf("ParseVitalsParams: error: vitalid is undefined\n");
    return false;
    }

  *retHostKey = fhostkey;
  *retVitalId = fvitalId;

  return true;
}

/******************************************************************************/
/* Returning tabular html data                                                */
/******************************************************************************/

PHP_FUNCTION(cfpr_report_software_in)

//$ret = cfpr_report_software_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 int use_reg;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_software_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 
 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_software,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in)

// $ret = cfpr_report_patch_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg= (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_patch_installed,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail)

// $ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_avail function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 
 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_patch_avail,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_classes_in function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_classes_report(fhostkey,fname,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_time_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_time_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_time(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_host_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_host_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_host(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_all_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_all_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_all(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_soft_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_soft_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_soft(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_ip_classes)
//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_ip_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_ip(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_cloud)

{ char*classes;
 char *fclasses;
 int cl_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&classes,&cl_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fclasses =  (cl_len == 0) ? NULL : classes;

 char **classArray = String2StringArray(fclasses, ',');

 buffer[0]='\0';
 Nova2PHP_classes_summary(classArray, buffer, sizeof(buffer));

 FreeStringArray(classArray);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_vars)

//$ret = cfpr_report_vars($hostkey,$scope,$lval,$rval,$type,$regex);

{ char *hostkey,*scope,*lval,*rval,*type,*returnval,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbsll",
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_vars function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_vars_report(fhostkey,fscope,flval,frval,ftype,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_compliance_summary_graph)

{ const int bufsize = 512*1024; 
 char buffer[bufsize];
 char *policy, *fpolicy;
 int pol_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&policy,&pol_len) == FAILURE)
    {
    RETURN_NULL();
    }

// "any" is keyword for all policies

 fpolicy =  (pol_len == 0) ? "any" : policy;

 buffer[0]='\0';
 Nova2PHP_ComplianceSummaryGraph(fpolicy,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_summary)

//$ret = cfpr_report_compliance_summary($hostkey,$version,$time,$kept,$notkept,$repaired,">");

{ char *hostkey,*version,*cmp,*returnval,*classreg;
 char *fhostkey,*fversion,*fclassreg;
 int hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllllssll",
                           &hostkey,&hk_len,&version,&v_len,&t,&k,&nk,&r,&cmp,&cmp_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fversion = (v_len == 0) ? NULL : version;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_compliance_report(fhostkey,fversion,(time_t)t,(int)k,(int)nk,(int)r,cmp,classreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises)

//$ret = cfpr_report_compliance_promises($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*returnval,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbsll",
                           &hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance_promises function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_compliance_promises(fhostkey,fhandle,fstatus,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_report_overall_summary)
//$ret = cfpr_report_overall_summary($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*returnval,*classreg;
  char *fhostkey,*fhandle,*fstatus,*fclassreg;
  int hk_len,h_len,s_len,cr_len;

 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_overall_summary function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_summary_report(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,bufsize); 

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastseen)


{ char *hostkey,*host,*address,*hash,*returnval,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int hk_len,h_len,a_len,h2_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long ago;
 time_t tago;
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslbsll",
                           &hostkey,&hk_len,&hash,&h2_len,&host,&h_len,&address,&a_len,&ago,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_lastseem function args");
    RETURN_NULL();
    }

 tago = (time_t)ago;
 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_lastseen_report(fhostkey,fhash,fhost,faddress,tago,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance)

//$ret = cfpr_report_performance($hostkey,$job,$regex);

{ char *hostkey,*job,*returnval,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&job,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_performance function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_performance_report(fhostkey,fjob,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid)

//$ret = cfpr_report_setuid($hostkey,$file,$regex);

{ char *hostkey,*file,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&file,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_setuid function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_setuid_report(fhostkey,ffile,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges)

//$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,f_len,c_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblssll",
                           &hostkey,&hk_len,&file,&f_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filechanges function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filechanges_report(fhostkey,ffile,regex,then,cmp,fclassreg,&page,false,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs)

//$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int hk_len,f_len,c_len,d_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblssll",
                           &hostkey,&hk_len,&file,&f_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filediffs function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filediffs_report(fhostkey,ffile,fdiff,use_reg,then,cmp,fclassreg,&page,false,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges_longterm)

//$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,f_len,c_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblssll",
                           &hostkey,&hk_len,&file,&f_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filechanges function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filechanges_report(fhostkey,ffile,regex,then,cmp,fclassreg,&page,true,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs_longterm)

//$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int hk_len,f_len,c_len,d_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblssll",
                           &hostkey,&hk_len,&file,&f_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filediffs function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filediffs_report(fhostkey,ffile,fdiff,use_reg,then,cmp,fclassreg,&page,true,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_promise)

//$ret = cfpr_summarize_promise($handle);

{ char *returnval;
 char *handle;
 int handleLen;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&handleLen) == FAILURE)
    {
    php_printf("Error is cfpr_summarize_promise function args");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_summarize_promise(handle, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_bundlesseen)

//$ret = cfpr_report_bundlesseen($hostkey,$name,$isNameRegex,$classRegexStr);

{ char *hostkey,*bundle,*returnval,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&bundle,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_bundlesseen function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_bundle_report(fhostkey,fbundle,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_value)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssll",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_value function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_value_report(fhostkey,fday,fmonth,fyear,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_value_graph)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = 10000; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 struct PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_value_graph function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_get_value_graph(fhostkey,fday,fmonth,fyear,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_notkept)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",
                           &hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_notkept function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0]='\0';
 Nova2PHP_promiselog(fhostkey,fhandle,plog_notkept,from,to,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_repaired)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",
                           &hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_repaired function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0]='\0';
 Nova2PHP_promiselog(fhostkey,fhandle,plog_repaired,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 time_t from,to;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len, &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_summarize_notkept function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_promiselog_summary(fhostkey,fhandle,plog_notkept,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_repaired)

//$ret = cfpr_summarize_notkept(hostkey,handle,from,to,classRegex);
    
{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 time_t from,to;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len, &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_summarize_repaired function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_promiselog_summary(fhostkey,fhandle,plog_repaired,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}


/******************************************************************************/
/* Returning hosts                                                            */
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_software_in)

//$ret = cfpr_hosts_with_software_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_software_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_hosts(fhostkey,fname,fversion,farch,use_reg,cfr_software,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_value)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = 512*1024;  
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_hosts_with_value function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_value_hosts(fhostkey,fday,fmonth,fyear,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_in)

// $ret = cfpr_hosts_with_patch_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_patch_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;   

 buffer[0] = '\0';
 Nova2PHP_software_hosts(hostkey,name,version,arch,regex,cfr_patch_installed,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_avail)

// $ret = cfpr_hosts_with_patch_avail($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_patch_avail function args");
    RETURN_NULL();
    }


 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_hosts(fhostkey,fname,fversion,farch,use_reg,cfr_patch_avail,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_classes)

//$ret = cfpr_hosts_with_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_classes_in function args");
    RETURN_NULL();
    }
 
 buffer[0] = '\0';
 Nova2PHP_classes_hosts(hostkey,name,regex,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_repaired)

//$ret = cfpr_hosts_with_repaired($hostkey,$name,$hours_deltafrom,$hours_deltato,$class_regex);

{ char *hostkey,*handle,*classreg;
 int hk_len, h_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_repaired function args");
    RETURN_NULL();
    }

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);
 
 buffer[0] = '\0';
 Nova2PHP_promiselog_hosts(hostkey,handle,plog_repaired,from,to,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_notkept)

//$ret = cfpr_hosts_with_notkept($hostkey,$name,$hours_deltafrom,$hours_deltato,$class_regex);

{ char *hostkey,*handle,*classreg;
 int hk_len, h_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;


 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_notkept function args");
    RETURN_NULL();
    }

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);
 
 buffer[0] = '\0';
 Nova2PHP_promiselog_hosts(hostkey,handle,plog_notkept,from,to,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}


/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_vars)

//$ret = cfpr_hosts_with_vars($hostkey,$scope,$lval,$rval,$type,$regex);

{ char *hostkey,*scope,*lval,*rval,*type,*returnval,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_vars function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_vars_hosts(fhostkey,fscope,flval,frval,ftype,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_summary)

//$ret = cfpr_hosts_with_compliance_summary($hostkey,$version,$time,$kept,$notkept,$repaired,">");

{ char *hostkey,*version,*cmp,*returnval,*classreg;
 int hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 time_t then;
 const int bufsize = 512*1024; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllllss",&hostkey,&hk_len,&version,&v_len,&t,&k,&nk,&r,&cmp,&cmp_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_compliance function args");
    RETURN_NULL();
    }

 then = (time_t)t;

 buffer[0] = '\0';
 Nova2PHP_compliance_hosts(hostkey,version,(int)t,(int)k,(int)nk,(int)r,cmp,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_promises)

//$ret = cfpr_hosts_with_compliance_promises($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*returnval,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 const int bufsize = 512 * 1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",&hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_compliance_promises function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_promise_hosts(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_lastseen)

//$ret = cfpr_hosts_with_lastseen($hostkey,$host,$addr,$tago,$regex);

{ char *hostkey,*host,*address,*hash,*returnval,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int hk_len,h_len,a_len,h2_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 long ago;
 time_t tago;
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslbs",&hostkey,&hk_len,&hash,&h2_len,&host,&h_len,&address,&a_len,&ago,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_lastseen function args");
    RETURN_NULL();
    }

 tago = (time_t)ago;
 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_lastseen_hosts(fhostkey,fhash,fhost,faddress,ago,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_performance)

//$ret = cfpr_hosts_with_performance($hostkey,$job,$regex);

{ char *hostkey,*job,*returnval,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&job,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_performance function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : job;

 buffer[0] = '\0';
 Nova2PHP_performance_hosts(fhostkey,fjob,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_setuid)

//$ret = cfpr_hosts_with_setuid($hostkey,$file,$regex);

{ char *hostkey,*file,*returnval, *classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&file,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_setuid function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_setuid_hosts(fhostkey,ffile,regex,fhostkey,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filechanges)

//$ret = cfpr_hosts_with_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*returnval,*classreg;
 char *fhostkey,*ffile,*fcmp,*fclassreg;
 int hk_len,j_len,c_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblss",&hostkey,&hk_len,&file,&j_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_filechanges function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_filechanges_hosts(fhostkey,ffile,regex,then,fcmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filediffs)

//$ret = cfpr_hosts_with_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*returnval;
 char *fhostkey,*ffile,*fcmp,*fdiff,*fclassreg,*classreg;
 int hk_len,j_len,c_len,d_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblss",&hostkey,&hk_len,&file,&j_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_filediffs function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_filediffs_hosts(fhostkey,ffile,diff,regex,then,fcmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_bundlesseen)

//$ret = cfpr_hosts_with_bundlesseen($hostkey,$name,$isNameRegex,$classreg);

{ char *hostkey,*bundle,*returnval,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&bundle,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_bundlesseen function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_bundle_hosts(fhostkey,fbundle,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
/* Topic Map                                                                  */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_pid_for_topic)

{ char *type,*topic,typed_topic[1024];
 int o_len,y_len;
 long pid;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&type,&y_len,&topic,&o_len) == FAILURE)
    {
    php_printf("Error is cfpr_get_pid_for_topic function args");
    RETURN_NULL();
    }

 if (y_len > 0)
    {
    snprintf(typed_topic,1023,"%s::%s",type,topic);
    pid = (long) Nova_GetTopicIdForTopic(typed_topic);
    }
 else
    {
    pid = (long) Nova_GetTopicIdForTopic(topic);   
    }

 RETURN_LONG(pid);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_search_topics)

//$ret = cfpr_search_topics("leads","string",regex);

{ char *search;
 char *fsearch;
 int s_len;
 const int bufsize = 100000; 
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&search,&s_len,&regex) == FAILURE)
    {
    php_printf("Error is cfpr_search_topics function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fsearch =  (s_len == 0) ? NULL : search;

 buffer[0] = '\0';
 Nova2PHP_search_topics(fsearch,regex,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic)

//$ret = cfpr_search_topics("leads","string",regex);

{ const int bufsize = 100000; 
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    php_printf("Error is cfpr_show_topic function args");
    RETURN_NULL();
    }
 
 buffer[0] = '\0';
 Nova2PHP_show_topic((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_leads)

{ const int bufsize = 100000; 
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    php_printf("Error is cfpr_show_topic_leads function args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic_leads((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_hits)

{ const int bufsize = 100000; 
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    php_printf("Error is cfpr_show_topic_hits function args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic_hits((int)id,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_category)

{ const int bufsize = 10000; 
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    php_printf("Error is cfpr_show_topic_category function args");
    RETURN_NULL();
    }
 
 buffer[0] = '\0';
 Nova2PHP_show_topic_category((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_story_by_id)

{ const int bufsize = 100000; 
 char buffer[bufsize];
 long id;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
   {
   php_printf("Error is cfpr_get_story_by_name function args");
   RETURN_NULL();
   }

buffer[0] = '\0';
Con2PHP_get_story_by_id((int)id,buffer,bufsize);

RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_story_by_name)

{ const int bufsize = 100000; 
  char buffer[bufsize];
  char *search;
  int s_len;
  
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&search,&s_len) == FAILURE)
   {
   php_printf("Error is cfpr_get_story_by_name function args");
   RETURN_NULL();
   }

if (strlen(search) == 0)
   {
   snprintf(buffer,1000,"Can't tell any stories about an empty topic");
   }
else
   {
   buffer[0] = '\0';
   Con2PHP_get_story_by_name(search,buffer,bufsize);
   }

RETURN_STRING(buffer,1);
}

/******************************************************************************/
/* GRAPHS                                                                     */
/******************************************************************************/

PHP_FUNCTION(cfpr_top_n_hosts)

{ char *policy;
 int hk_len;
 long n;
 const int bufsize = 100000; 
 char buffer[bufsize];
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slll",&policy,&hk_len,&n,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_top_n_hosts needs a policy");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topN(policy,(int)n,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_all_hosts)

{
 long val = Nova2PHP_count_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_red_hosts)
{
 long val = Nova2PHP_count_red_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_yellow_hosts)
{
 long val = Nova2PHP_count_yellow_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_green_hosts)
{
 long val = Nova2PHP_count_green_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_blue_hosts)
{
 long val = Nova2PHP_count_blue_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_select_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *policy,*select;
 int hk_len,s_len;
 long n;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl",&select,&s_len,&policy,&hk_len,&n) == FAILURE)
    {
    php_printf("Error in cfpr_select_hosts args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_select_hosts(policy,select,(int)n,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_select_reports)

{  const int bufsize = 100000; 
 char buffer[bufsize];
 char *policy;

 buffer[0] = '\0';
 Nova2PHP_select_reports(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_red_hosts)

{ const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_red_hosts args");
    RETURN_NULL();
    }
 
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("red",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_yellow_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_yellow_hosts args");
    RETURN_NULL();
    }
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("yellow",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_green_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_green_hosts args");
    RETURN_NULL();
    }
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("green",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_blue_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 struct PageInfo page = {0};
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_blue_hosts args");
    RETURN_NULL();
    }
  
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("blue",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_ip)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *ipRegex, *fipRegex, *classRegex, *fclassRegex;
 int ip_len,cl_len;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&ipRegex,&ip_len,&classRegex,&cl_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 fipRegex = (ip_len == 0) ? NULL : ipRegex;
 fclassRegex = (cl_len == 0) ? NULL : classRegex;

 buffer[0] = '\0';
 Nova2PHP_show_hosts(NULL,fipRegex,fclassRegex,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_name)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *hostNameRegex, *fhostNameRegex, *classRegex, *fclassRegex;
 int ip_len,cl_len;
 struct PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&hostNameRegex,&ip_len,&classRegex,&cl_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostNameRegex = (ip_len == 0) ? NULL : hostNameRegex;
 fclassRegex = (cl_len == 0) ? NULL : classRegex;

 buffer[0] = '\0';
 Nova2PHP_show_hosts(fhostNameRegex,NULL,fclassRegex,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlastupdate)

{ char *hostkey;
 int hk_len;
 const int bufsize = 100; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_getlastupdate needs a host key");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_getlastupdate(hostkey,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_host_colour)

{ char *hostkey;
 int hk_len;
 const int bufsize = 100; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_host_colour needs a host key");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_host_colour(hostkey,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_handles)

{ char *promiser;
 char *ptype;
 int pr_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssb",&promiser,&pr_len,&ptype,&p_len,&regex) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_promise_handles(promiser,ptype,"","",(int)regex,buffer,100000);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_policy_finder_by_handle)

{ char *handle;
 int h_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&handle,&h_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_handle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder(handle,"","",escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_policy_finder_by_promiser)

{ char *promiser;
 int pr_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&promiser,&pr_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_promiser args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder("",promiser,"",escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_policy_finder_by_bundle)

{ char *bundle;
 char *btype;
 int r_len, b_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&bundle,&r_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_bundle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder("","",bundle,escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_get_handles_for_bundle_with_comments)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_handles_for_bundle_with_comments args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 Nova2PHP_list_promise_handles_with_comments(bundle,btype,buffer,100000);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_handles_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssb",&bundle,&r_len,&btype,&p_len,&regex) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_promise_handles("","",bundle,btype,(int)regex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_comment)

{ char *handle;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_comment args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseComment(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_help_text)

{ char *key;
  int hk_len;
  
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&key,&hk_len) == FAILURE)
   {
   php_printf("Error in cfpr_get_promise_comment args");
   RETURN_NULL();
   }

RETURN_STRING(Nova2PHP_get_help_text(key),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_bundle)

{ char *handle;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_bundle args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseBundle(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_type)

{ char *handle;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_type args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseType(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promiser)

{ char *handle;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promiser args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiser(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_variable)

{ char *key,*scope,*lval;
 int hk_len,s_len,l_len;
 char buffer[2048];
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",&key,&hk_len,&scope,&s_len,&lval,&l_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_variable args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_variable(key,scope,lval,buffer,2048);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_classes_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_classes_for_bundle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_classes_for_bundle(bundle,btype,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_args_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_args_for_bundle(bundle,btype,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_all_bundles)

{ char *bundle;
 char *btype,*fbtype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_all_bundles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbtype =  (p_len == 0) ? NULL : btype;

 Nova2PHP_list_all_bundles(fbtype,buffer,bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_bundle_type)

{ char *bundle;
 char *bname,*fbname;
 int n_len;
 const int bufsize = 10000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&bname,&n_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_bundle_type args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbname =  (n_len == 0) ? NULL : bname;

 Nova2PHP_get_bundle_type(fbname,buffer,bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_bundles_using)

{ char *bundle,*fbundle;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&bundle,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_bundles_using args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbundle =  (p_len == 0) ? NULL : bundle;

 Nova2PHP_list_bundles_using(fbundle,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_bundle_count)

{ char *bundle,*fbundle;
 int r_len, p_len;
 const int bufsize = 32; 
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_get_bundle_count(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_count)

{ char *bundle,*fbundle;
 int r_len, p_len;
 const int bufsize = 32; 
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_get_promise_count(buffer,bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_business_goals)

{ char *bundle;
 char *btype,*fbtype;
 int r_len, p_len;
 const int bufsize = 1000000; 
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_list_all_goals(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_class_frequency)

{ char *hkey,*fhkey,*pattern,*fpattern;
 int p_len, h_len,count;
 const int bufsize = 1000000; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hkey,&h_len,&pattern,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_count_classes args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 count = Nova2PHP_countclasses(fhkey,fpattern,1,buffer,bufsize);
 RETURN_LONG((long)count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_class_frequency)

{ char *hkey,*fhkey,*pattern,*fpattern;
 int p_len, h_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hkey,&h_len,&pattern,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_count_classes args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 Nova2PHP_countclasses(fhkey,fpattern,1,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_docroot)

{ const int bufsize = 1000000; 
 char buffer[bufsize];

 buffer[0] = '\0';
 Nova2PHP_docroot(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_body)

{ char *type,*ftype,*name,*fname;
 int p_len, t_len;
 const int bufsize = 1000000; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&name,&p_len,&type,&t_len) == FAILURE)
    {
    php_printf("Error in cfpr_show-body args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 fname = (p_len == 0) ? NULL : name;
 ftype = (t_len == 0) ? NULL : type;

 Nova2PHP_GetPromiseBody(fname,ftype,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_bodies)

{ char *name;
 char *btype;
 int pr_len, p_len;
 const int bufsize = 1000000; 
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&name,&pr_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_bodies args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 Nova2PHP_list_bodies(name,btype,buffer,100000);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_network_rate)

{ char *name;
 int n_len;
 const int bufsize = 1000000; 
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&name,&n_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_network_rate args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_network_speed(name,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/* for pdf generation  */
/******************************************************************************/
PHP_FUNCTION(cfpr_report_compliance_summary_pdf)

//$ret = cfpr_report_compliance_summary($hostkey,$version,$time,$kept,$notkept,$repaired,">");

{ char *hostkey,*version,*cmp,*returnval,*classreg;
 char *fhostkey,*fversion, *fclassreg;
 int hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllllss",&hostkey,&hk_len,&version,&v_len,&t,&k,&nk,&r,&cmp,&cmp_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance_summary_pdf function args");
    RETURN_NULL();
    }
   
 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fversion = (v_len == 0) ? NULL : version;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 buffer[0] = '\0';
 Nova2PHP_compliance_report_pdf(fhostkey,fversion,(time_t)t,(int)k,(int)nk,(int)r,cmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_notkept_pdf)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;


 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_report_notkept_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;   

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0] = '\0';
 Nova2PHP_promiselog_pdf(fhostkey,fhandle,plog_notkept,from,to,fclassreg,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_bundlesseen_pdf)

//$ret = cfpr_report_bundlesseen($hostkey,$name,$isNameRegex,$classRegexStr);

{ char *hostkey,*bundle,*returnval,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&bundle,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_bundlesseen_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;   

 buffer[0] = '\0';
 Nova2PHP_bundle_report_pdf(fhostkey,fbundle,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_value_pdf)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",&hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_report_value_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_value_report_pdf(fhostkey,fday,fmonth,fyear,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_classes_pdf)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*classreg;
 char *fclassreg,*fname,*fhostkey;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_classes_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_classes_report_pdf(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises_pdf)

//$ret = cfpr_report_compliance_promises($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*returnval,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",&hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance_promises_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_compliance_promises_pdf(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges_pdf)

//$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,f_len,c_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblss",&hostkey,&hk_len,&file,&f_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_filechanges_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0] = '\0';
 Nova2PHP_filechanges_report_pdf(fhostkey,ffile,regex,then,cmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastseen_pdf)

//$ret = cfpr_report_lastseen($hostkey,$rhash,rhost,$addr,$tago,$regex);

{ char *hostkey,*host,*address,*hash,*returnval,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int hk_len,h_len,a_len,h2_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long ago;
 time_t tago;
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslbs",&hostkey,&hk_len,&hash,&h2_len,&host,&h_len,&address,&a_len,&ago,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_lastseen_pdf function args");
    RETURN_NULL();
    }

 tago = (time_t)ago;
 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_lastseen_report_pdf(fhostkey,fhash,fhost,faddress,ago,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);

}
/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail_pdf)

// $ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_avail_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_report_pdf(fhostkey,fname,fversion,farch,use_reg,cfr_patch_avail,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in_pdf)

// $ret = cfpr_report_patch_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_in_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_report_pdf(fhostkey,fname,fversion,farch,use_reg,cfr_patch_installed,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_software_in_pdf)

//$ret = cfpr_report_software_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*n,*v,*a,*returnval,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 int use_reg;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_software_in_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_report_pdf(fhostkey,fname,fversion,farch,use_reg,cfr_software,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance_pdf)

//$ret = cfpr_report_performance($hostkey,$job,$regex);

{ char *hostkey,*job,*returnval,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&job,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_performance_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_performance_report_pdf(fhostkey,fjob,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_repaired_pdf)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;


 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_report_repaired_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0] = '\0';
 Nova2PHP_promiselog_pdf(fhostkey,fhandle,plog_repaired,from,to,fclassreg,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_summarize_repaired_pdf)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 time_t from, to;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_report_repaired_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : handle;

 buffer[0] = '\0';
 Nova2PHP_promiselog_summary_pdf(fhostkey,fhandle,plog_repaired,from,to,fclassreg,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept_pdf)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER; 
 char buffer[bufsize];
 time_t from,to;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_report_notkept_pdf function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_promiselog_summary_pdf(fhostkey,fhandle,plog_notkept,from,to,fclassreg,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_vars_pdf)

//$ret = cfpr_report_vars($hostkey,$scope,$lval,$rval,$type,$regex);

{ char *hostkey,*scope,*lval,*rval,*type,*returnval,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_vars_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
  
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_vars_report_pdf(fhostkey,fscope,flval,frval,ftype,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs_pdf)

//$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int hk_len,f_len,c_len,d_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblss",&hostkey,&hk_len,&file,&f_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_filediffs_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0] = '\0';
 Nova2PHP_filediffs_report_pdf(fhostkey,ffile,fdiff,use_reg,then,cmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid_pdf)

//$ret = cfpr_report_setuid($hostkey,$file,$regex);

{ char *hostkey,*file,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&file,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_setuid_pdf function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_setuid_report_pdf(fhostkey,ffile,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/* end pdf */

/******************************************************************************/
PHP_FUNCTION(cfpr_report_description)

{ char *reportName;
 int reportNameSz;
 const int bufSize = 5*1024;
 char buf[bufSize];
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&reportName,&reportNameSz) == FAILURE)
    {
    php_printf("Error is cfpr_report_description function args");
    RETURN_NULL();
    }
 
 buf[0] = '\0';
 Nova2PHP_report_description(reportName,buf,sizeof(buf));

 RETURN_STRING(buf,1);
}



/******************************************************************************/
/*                    Content-Driven Policy Reports                           */
/******************************************************************************/


PHP_FUNCTION(cfpr_cdp_reportnames)

{ char buf[1024]={0};
 char *hostkey;
 int hk_len;

 buf[0]='\0';
 Nova2PHP_cdp_reportnames(buf,sizeof(buf));

 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_cdp_report)

{const int bufsize = CF_WEBBUFFER;
 char buf[bufsize];  // one row is ~300 bytes
 char *hostkey,*reportName;
 int hk_len,rn_len;
 struct PageInfo page = {0};
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&hostkey,&hk_len,&reportName,&rn_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_cdp_report_acl function args");
    RETURN_NULL();
    }

 buf[0]='\0';
 Nova2PHP_cdp_report(hostkey,reportName,&page,buf,bufsize);

 RETURN_STRING(buf,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_validate_policy)

{ char *file;
 int f_len, ret;
 const int bufsize = 100000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&file,&f_len) == FAILURE)
    {
    php_printf("Error in cfpr_validate_policy args");
    RETURN_NULL();
    }

 buffer[0]='\0';
 ret = Nova2PHP_validate_policy(file,buffer,bufsize);
 switch(ret)
    {
    case 0:
	RETURN_STRING("SUCCESS",1);
	break;
    case -1:  // error 
    case 1:
    default:
	break;
    }
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_delete_host)
/* Delete the record of a certain host from web-supporting DB  */
{ char *hostkey;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error is cfpr_delete_host function args");
    RETURN_NULL();
    }

 if(hk_len == 0)
    {
    return;
    }
 NewClass("am_policy_hub");
 Nova2PHP_delete_host(hostkey);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environments_list)
{
 struct EnvironmentsList *el, *i;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE)
    {
    php_printf("Wrong number of arguments in cfpr_environments_list");
    RETURN_NULL();
    }

 if (!Nova2PHP_environments_list(&el))
    {
    php_printf("Unable to query list of environments in cfpr_environments_list");
    RETURN_NULL();
    }

 array_init(return_value);

 for (i = el; i != NULL; i = i->next)
    {
    add_next_index_string(return_value, i->name, 1);
    }

 FreeEnvironmentsList(el);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environment_contents)
{
 struct HostsList *hl, *i;
 char *name;
 int name_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
    {
    php_printf("Wrong arguments in cfpr_environment_contents");
    RETURN_NULL();
    }

 if (!Nova2PHP_environment_contents(name, &hl))
    {
    php_printf("Unable to query environment in cfpr_environment_contents");
    RETURN_NULL();
    }

 array_init(return_value);

 for (i = hl; i != NULL; i = i->next)
    {
    add_next_index_string(return_value, i->keyhash, 1);
    }

 FreeHostsList(hl);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_host_environment)
{
 char *hostkey;
 int hostkey_len;
 char *environment;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostkey, &hostkey_len) == FAILURE)
    {
    php_printf("Wrong arguments in cfpr_get_host_contents");
    RETURN_NULL();
    }

 environment = Nova2PHP_get_host_environment(hostkey);

 if (!environment)
    {
    RETURN_NULL();
    }
 else
    {
    char *envcopy = estrdup(environment);
    free(environment);
    RETURN_STRING(envcopy, 0);
    }
}


/******************************************************************************/
/* COPERNICUS KNOWLEDGE MAP                                                   */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_knowledge_view)

{ char *view;
 int v_len;
 const int bufsize = 1000000; 
 char buffer[bufsize];
 long pid;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls",&pid,&view,&v_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_knowledge_view args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_knowledge_view(pid,view,buffer,100000);

 RETURN_STRING(buffer,1);
}



/***** Local helpers ******/

static time_t DeltaHrsConvert(long hrsAgo)
{
 time_t now, absTime;

 if(hrsAgo == 0)
    {
    return 0;
    }

 now = time(NULL);

 absTime = now - (hrsAgo * 3600);
  
 return absTime;

}



/******************************************************************************/
/*
 * commenting
 */
/******************************************************************************/
PHP_FUNCTION(cfpr_add_note)

{ char *user,*note,*nid;
 char *fuser,*fnote,*fnid;
 int u_len,n_len, nid_len;
 int report_type;
 time_t datetime;
 int ret = 0;
 const int bufsize = 1000;
 char returnval[bufsize];
   
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssls",&nid,&nid_len,&user,&u_len,&datetime,&note,&n_len) == FAILURE)
    {
    php_printf("Error in cfpr_add_note function args");
    RETURN_NULL();
    }
   
 if(nid_len == 0)
    {
    php_printf("Note ID mut be given!!\n");
    RETURN_NULL();
    }
  
 fnid = nid; 
 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;

 returnval[0]='\0';
 ret = Nova2PHP_add_note(fnid,fuser,datetime,fnote,returnval,bufsize);
 RETURN_STRING(returnval,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_new_note)

{ char *hostkey,*user,*note, *repid;
 char *fhostkey,*fuser,*fnote, *frepid;
 int hk_len,u_len,n_len,rid_len;
 int report_type;
 time_t datetime;
 int ret = 0;
 const int bufsize = 1000;
 char returnval[bufsize];
   
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslsls",&hostkey,&hk_len,&repid,&rid_len,&report_type,&user,&u_len,&datetime,&note,&n_len) == FAILURE)
    {
    php_printf("Error is cfpr_add_note function args");
    RETURN_NULL();
    }
   
 /* none of these fields can be null (due to unique key constraint)!!*/
 if(hk_len == 0 && rid_len == 0)
    {
    php_printf("Host Key and report id must be given!!\n");
    RETURN_NULL();
    }

 fhostkey = hostkey;
 frepid = repid;

 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;
 
 returnval[0]='\0';  
 ret = Nova2PHP_add_new_note(fhostkey,frepid,report_type,fuser,datetime,fnote,returnval,bufsize);  
 RETURN_STRING(returnval,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_query_note)

{ char *hostkey = {0},*user, *nid;
 char *fhostkey,*fuser, *fnid;
 int hk_len,sk_len,h_len,u_len,nid_len;
 const int bufsize = 1000000;
 long from,to;
 struct PageInfo page = {0};

 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllll",&hostkey,&hk_len,&nid,&nid_len,&user, &u_len, &from, &to,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_query_note function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fuser =  (u_len == 0) ? NULL : user;
 fnid =  (nid_len == 0) ? NULL : nid;

 buffer[0] = '\0';
 Nova2PHP_get_notes(fhostkey,fnid,fuser, from, to, &page, buffer, bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_get_host_noteid)

{ char s1[4096];
 char *hostkey,*fhostkey;
 int hk_len;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error is cfpr_hostname function args");
    RETURN_NULL();
    }

 fhostkey = (hk_len == 0) ? NULL : hostkey;

 s1[0]='\0';
 Nova2PHP_get_host_noteid(hostkey,s1,sizeof(s1));

 RETURN_STRING(s1,1);
}

/******************************************************************************/

// TODO: get policiy names, get compliance by policy name

PHP_FUNCTION(cfpr_replica_status)
/* MongoDB hub replication */
{
 const int bufsize = 4096;
 char buf[bufsize];

 buf[0] = '\0';
 Nova2PHP_replica_status(buf, sizeof(buf));
 RETURN_STRING(buf,1);
}

/******************************************************************************/
/* Library                                                                    */
/******************************************************************************/

PHP_FUNCTION(cfpr_list_documents)

{ const int bufsize = 1000000; 
  char buffer[bufsize];
  char *filePath,*fFilePath;
  int fpLen;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&filePath, &fpLen) == FAILURE)
   {
   RETURN_NULL();
   }

fFilePath = (fpLen == 0) ? NULL : filePath;

buffer[0]='\0';
Nova2PHP_GetLibraryDocuments(fFilePath,buffer,bufsize);
RETURN_STRING(buffer,1);
}


/******************************************************************************/
/*                              Constellation                                 */
/******************************************************************************/

#ifdef HAVE_CONSTELLATION

PHP_FUNCTION(cfcon_compliance_summary_graph)

{ const int bufsize = 512*1024; 
 char buffer[bufsize];
 char *hubKeyHash, *fhubKeyHash, *policy, *fpolicy;
 int pol_len,hkh_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hubKeyHash,&hkh_len,&policy,&pol_len) == FAILURE)
    {
    RETURN_NULL();
    }
  
// "any" is keyword for all policies

 fpolicy =  (pol_len == 0) ? "any" : policy;
 fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

 buffer[0]='\0';
 Con2PHP_ComplianceSummaryGraph(fhubKeyHash,fpolicy,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_compliance_virtualbundle_graph)

{ const int bufsize = 4096;
 char buffer[bufsize];
 char *hubHostKey, *bundleName;
 char *fHubHostKey, *fBundleName;
 int hkLen, vbLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hubHostKey, &hkLen, &bundleName, &vbLen) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
 fBundleName = (vbLen == 0) ? NULL : bundleName;

 if(fBundleName == NULL)
    {
    php_printf("Error: Virtual bundle name is undefined in cfcon_virtualbundle_graph()\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Con2PHP_compliance_virtualbundle_graph(fHubHostKey, fBundleName, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_hubs)

{ const int bufsize = 64;
 char buffer[bufsize];
 char *classification, *fclassification;  // red/green, class regex?...
 int cl_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&classification,&cl_len) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fclassification = (cl_len == 0) ? NULL : classification;

 buffer[0]='\0';
 Con2PHP_count_hubs(fclassification,buffer,sizeof(buffer));
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_show_hubs)

{ const int bufsize = 16384;
 char buffer[bufsize];
 char *classification, *fclassification;  // red/green, class regex?...
 int cl_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&classification,&cl_len) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fclassification = (cl_len == 0) ? NULL : classification;

 buffer[0]='\0';
 Con2PHP_show_hubs(fclassification,buffer,sizeof(buffer));
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_repaired)

{
 char *buffer;

 buffer = cfcon_aggr_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_repaired);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_notkept)

{
 char *buffer;

 buffer = cfcon_aggr_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_notkept);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

static char *cfcon_aggr_promiselog(int argc, enum promiselog_rep log_type)
/**
 * Helper for repaired and notkept logs.
 **/
{
#define CFCON_SPL_BUFSIZE 16384

 int hkh_len,ph_len;
 static char buffer[CFCON_SPL_BUFSIZE];
 char *hubKeyHash, *fhubKeyHash;
 char *promiseHandle, *fPromiseHandle;

 if (zend_parse_parameters(argc, "ss",&hubKeyHash,&hkh_len,&promiseHandle,&ph_len) == FAILURE)
    {
    return NULL;
    }

 fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash; 
 fPromiseHandle = (ph_len == 0) ? NULL : promiseHandle;
     
 buffer[0]='\0';
 Con2PHP_aggr_promiselog(fhubKeyHash,fPromiseHandle,log_type,buffer,sizeof(buffer));

 return buffer;
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_repaired)

{
 char *buffer;

 buffer = cfcon_count_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_repaired);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_notkept)

{
 char *buffer;

 buffer = cfcon_count_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_notkept);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

static char *cfcon_count_promiselog(int argc, enum promiselog_rep log_type)
/**
 * Helper for repaired and notkept logs.
 **/
{
#define CFCON_CPL_BUFSIZE 64
 
 char *promiseHandle, *fpromiseHandle;
 int ph_len, hkh_len;
 static char buffer[CFCON_CPL_BUFSIZE];
 char *hubKeyHash, *fhubKeyHash;

 if (zend_parse_parameters(argc, "ss",&hubKeyHash,&hkh_len,&promiseHandle,&ph_len) == FAILURE)
    {
    return NULL;
    }

 fpromiseHandle =  (ph_len == 0) ? NULL : promiseHandle;
 fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

 buffer[0]='\0';
 Con2PHP_count_promiselog(fhubKeyHash,fpromiseHandle,log_type,buffer,sizeof(buffer));

 return buffer;
}

/******************************************************************************/

PHP_FUNCTION(cfcon_reasons_repaired)

{
 char *buffer;

 buffer = cfcon_reasons_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_repaired);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_reasons_notkept)

{
 char *buffer;

 buffer = cfcon_reasons_promiselog(ZEND_NUM_ARGS() TSRMLS_CC,plog_notkept);
 
 if(!buffer)
    {
    RETURN_NULL();
    }
 else
    {
    RETURN_STRING(buffer,1);
    }
}

/******************************************************************************/

static char *cfcon_reasons_promiselog(int argc, enum promiselog_rep log_type)
/**
 * Helper for repaired and notkept logs.
 **/
{
#define CFCON_RPL_BUFSIZE 16384
 
 char *promiseHandle, *fpromiseHandle, *time, *hubKeyHash, *fhubKeyHash;
 int ph_len,t_len,hkh_len;
 static char buffer[CFCON_RPL_BUFSIZE];

 if (zend_parse_parameters(argc, "ss",&hubKeyHash,&hkh_len,&promiseHandle,&ph_len) == FAILURE)
    {
    return NULL;
    }

 fpromiseHandle =  (ph_len == 0) ? NULL : promiseHandle;
 fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

 buffer[0]='\0';
 Con2PHP_reasons_promiselog(fhubKeyHash,fpromiseHandle,log_type,buffer,sizeof(buffer));

 return buffer;
}


/******************************************************************************/

PHP_FUNCTION(cfcon_environments_list)
{
 struct EnvironmentsList *el, *i;
 char *hubKeyHash, *fhubKeyHash;
 int hkh_len;
 const int bufsize = 1024;
 char buffer[bufsize];
    
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hubKeyHash, &hkh_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

 buffer[0]='\0';
 Con2PHP_environments_list(fhubKeyHash,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_promise_popularity)
{
 char *promiseHandle;
 int prh_len;
 const int bufsize = 256;
 char buffer[bufsize];
    
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &promiseHandle, &prh_len) == FAILURE)
    {
    RETURN_NULL();
    }

 if(prh_len == 0)
    {
    php_printf("Error: Promise handle is undefined in cfcon_promise_popularity()\n");
    RETURN_NULL();
    }
 
 buffer[0]='\0';
 Con2PHP_promise_popularity(promiseHandle,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_rank_promise_popularity)
{
 char *promiseHandle;
 int prh_len;
 const int bufsize = CF_WEBBUFFER;;
 char buffer[bufsize];
 zend_bool sortAscending;
 

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &sortAscending) == FAILURE)
    {
    RETURN_NULL();
    }

 buffer[0]='\0';
 Con2PHP_rank_promise_popularity(sortAscending,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_filechange)

{ const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *hubHostKey, *filePath;
 char *fHubHostKey, *fFilePath;
 int hkLen, fpLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hubHostKey, &hkLen, &filePath, &fpLen) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
 fFilePath = (fpLen == 0) ? NULL : filePath;

 buffer[0]='\0';
 Con2PHP_aggr_filechange(fHubHostKey, fFilePath, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_software)

{ const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *hubHostKey, *pkgname;
 char *fHubHostKey, *fPackageName;
 int hkLen, pnLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hubHostKey, &hkLen, &pkgname, &pnLen) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
 fPackageName = (pnLen == 0) ? NULL : pkgname;

 buffer[0]='\0';
 Con2PHP_aggr_software(fHubHostKey, fPackageName, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_classes)

{ const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *hubHostKey, *classname;
 char *fHubHostKey, *fClassName;
 int hkLen, cnLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hubHostKey, &hkLen, &classname, &cnLen) == FAILURE)
    {
    RETURN_NULL();
    }
  
 fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
 fClassName = (cnLen == 0) ? NULL : classname;

 buffer[0]='\0';
 Con2PHP_aggr_classes(fHubHostKey, fClassName, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/
/* Constellation subscribe/report                                             */
/******************************************************************************/

PHP_FUNCTION(cfcon_list_subscriptions)
/**
 * Generic "show all subscritions" function
 **/
{ const int bufsize = 4096;
 char buffer[bufsize];
 char *user, *subHandleRx;
 int usLen, shLen;
 PageInfo_t page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&user, &usLen, &subHandleRx, &shLen,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 char *fUser = (usLen == 0) ? NULL : user;
 char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

 buffer[0]='\0';
 Con2PHP_list_subscriptions(fUser, NULL, fSubHandleRx, &page, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_show_subscription_virtualbundle)

{ const int bufsize = 4096;
 char buffer[bufsize];
 char *user, *subHandleRx;
 int usLen, shLen;
 PageInfo_t page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&user, &usLen, &subHandleRx, &shLen,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 char *fUser = (usLen == 0) ? NULL : user;
 char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

 buffer[0]='\0';
 Con2PHP_list_subscriptions(fUser, cfl_subtype_local_virtualbundle, fSubHandleRx, &page, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_subscribe_software)
// FIXME: Take description as input, in addition to handle
{ const int bufsize = 4096;
 char buffer[bufsize];
 char *user, *hubClassRegex, *subHandle, *pkgName, *hostClassRegex;
 zend_bool pnRegex;
 int usLen, hcLen, shLen, pnLen, ocLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen,
                           &pkgName, &pnLen, &pnRegex, &hostClassRegex, &ocLen) == FAILURE)
    {
    RETURN_NULL();
    }

 if(usLen == 0)
    {
    php_printf("cfcon_subscribe_software: Parameter error: user must be specified\n");
    RETURN_NULL();
    }
 
 if(shLen == 0)
    {
    php_printf("cfcon_subscribe_software: Parameter error: handle must be specified\n");
    RETURN_NULL();
    }

  if(pnLen == 0)
    {
    php_printf("cfcon_subscribe_software: Parameter error: package name must be specified\n");
    RETURN_NULL();
    }
  
  if(ocLen == 0)
    {
    php_printf("cfcon_subscribe_software: Parameter error: host class regex must be specified\n");
    RETURN_NULL();
    }

  if(hcLen == 0)
     {
     hubClassRegex = "any";
     }

 buffer[0]='\0';
 Con2PHP_subscribe_software(user, subHandle, hubClassRegex, pkgName, pnRegex, hostClassRegex, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_report_software)
{
 char buffer[CF_WEBBUFFER];
 char *user, *subHandle, *hubClassRegex;
 int usLen, shLen, hcLen;
 struct PageInfo page = {0};
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssll", &user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

  if(usLen == 0)
    {
    php_printf("cfcon_report_software: Parameter error: user must be specified\n");
    RETURN_NULL();
    }
 
 if(shLen == 0)
    {
    php_printf("cfcon_report_software: Parameter error: handle must be specified\n");
    RETURN_NULL();
    }


 if(hcLen == 0)
    {
    hubClassRegex = "any";
    }

 buffer[0]='\0';
 Con2PHP_report_software(user, subHandle, hubClassRegex, &page, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_report_virtualbundle)
{
 char buffer[CF_WEBBUFFER];
 char *user, *subHandleRx;
 int usLen, shLen;
 struct PageInfo page = {0};
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &user, &usLen, &subHandleRx, &shLen,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 char *fUser = (usLen == 0) ? NULL : user;
 char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

 buffer[0]='\0';
 Con2PHP_local_report_virtualbundle(fUser, fSubHandleRx, &page, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);

}

/******************************************************************************/


PHP_FUNCTION(cfcon_local_subscribe_virtualbundle)
// NOTE: Available to Constellation Mission Portal, not Mission Observatory (hence cfcon_local)
{ const int bufsize = 4096;
 char buffer[bufsize];
 char *user, *description, *subHandle, *promiseListStr, *hostClassRegex;
 int usLen, descLen, shLen, plLen, ocLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss", &user, &usLen, &subHandle, &shLen, &description, &descLen,
                           &promiseListStr, &plLen, &hostClassRegex, &ocLen) == FAILURE)
    {
    RETURN_NULL();
    }

 if(usLen == 0)
    {
    php_printf("cfcon_local_subscribe_virtualbundle: Parameter error: user must be specified\n");
    RETURN_NULL();
    }
 
 if(shLen == 0)
    {
    php_printf("cfcon_local_subscribe_virtualbundle: Parameter error: handle must be specified\n");
    RETURN_NULL();
    }

  if(descLen == 0)
    {
    php_printf("cfcon_local_subscribe_virtualbundle: Parameter error: description must be specified\n");
    RETURN_NULL();
    }
  
  if(plLen == 0)
    {
    php_printf("cfcon_local_subscribe_virtualbundle: Parameter error: promise handle list must be specified\n");
    RETURN_NULL();
    }

  char **promises = String2StringArray(promiseListStr, ',');

  char *fHostClassRx = (ocLen == 0) ? NULL : hostClassRegex;

  buffer[0]='\0';
  Con2PHP_local_subscribe_virtualbundle(user, subHandle, description, promises, fHostClassRx, buffer, sizeof(buffer));

  FreeStringArray(promises);
  
  RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_delete_subscription_virtualbundle)
{
 char buffer[CF_WEBBUFFER] = {0};
 char *user, *subHandle;
 int usLen, shLen;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &user, &usLen, &subHandle, &shLen) == FAILURE)
    {
    RETURN_NULL();
    }

 if(shLen == 0)
    {
    php_printf("cfcon_local_delete_subscription_virtualbundle: Parameter error: subscription handle must be specified\n");
    RETURN_NULL();
    }
 
 char *fUser = (usLen == 0) ? NULL : user;

 Con2PHP_delete_subscription(fUser, cfl_subtype_local_virtualbundle, subHandle, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_subscribe_repairlog)

{ const int bufsize = 4096;
 char buffer[bufsize];
 char *user, *hubClassRegex, *subHandle, *promiseHandleRegex, *reportRegex, *hostClassRegex;
 int pnRegex;
 int usLen, hcLen, shLen, phLen, rxLen, ocLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssss",&user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen,
                           &promiseHandleRegex, &phLen, &reportRegex, &rxLen, &hostClassRegex, &ocLen) == FAILURE)
    {
    RETURN_NULL();
    }

 if(usLen == 0)
    {
    php_printf("cfcon_subscribe_repairlog: Parameter error: user must be specified\n");
    RETURN_NULL();
    }
 
 if(shLen == 0)
    {
    php_printf("cfcon_subscribe_repairlog: Parameter error: handle must be specified\n");
    RETURN_NULL();
    }

  if(phLen == 0)
    {
    php_printf("cfcon_subscribe_repairlog: Parameter error: package name must be specified\n");
    RETURN_NULL();
    }
  
  if(ocLen == 0)
    {
    php_printf("cfcon_subscribe_repairlog: Parameter error: host class regex must be specified\n");
    RETURN_NULL();
    }

  if(rxLen == 0)
     {
     reportRegex = NULL;
     }

  if(hcLen == 0)
     {
     hubClassRegex = "any";
     }

 buffer[0]='\0';
 Con2PHP_subscribe_promiselog(user, subHandle, hubClassRegex, promiseHandleRegex, reportRegex, hostClassRegex, cfl_subtype_repairlog, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}
/*****************************************************************************
 * Name: cfcon_hub_meter
 * Returns Meter data for a given hub key
 * @param string (hub keyhash)
 * @return NULL if argument mismatch
 *         JSON {meta:{},data:[],error:{}}
 * @throws cfmod_db_exception_ce (Exception)
 * @lastmodifiedby bishwa.shrestha@cfengine.com 
 * @todo throw exception
 *****************************************************************************/
PHP_FUNCTION(cfcon_hub_meter)

{ char *hostkey,*fhostkey,*name;
 int hk_len, n_len;
 long regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error in cfcon_hub_meter function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 buffer[0]='\0';
 Con2PHP_meter(fhostkey,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/*****************************************************************************
 * Name: cfcon_count_hubs_colour
 * Count the number of hubs by colour code (status)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted  
 *      3. JSON {meta:{},data:[],error:{}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_count_hubs_colour)

{ const int bufsize = 1024;
 char buffer[bufsize];
 char *colour;
 int c_len;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&colour,&c_len) == FAILURE)
   {
   RETURN_NULL();
   }

if(*colour!='R' && *colour!='G' && *colour!='B' && *colour!='Y' && *colour!='A')
   {
   RETURN_STRING("{\"error\":{\"errid\":\"-1\",\"msg\":\"Unrecognized parameters supplied\"}}",1);
   }

buffer[0]='\0';
colour[1]='\0';
Con2PHP_count_hubs_colour(colour,buffer,bufsize);

// TODO: throw exception
RETURN_STRING(buffer,1);
}

/*****************************************************************************
 * Name: cfcon_get_hub_colour
 * Get the hub colour
 * @param: string (hubkey)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON {meta:{},data:[],error:{}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_get_hub_colour)

{ char *hostkey;
 int hk_len;
 const int bufsize = 10000;
 char buffer[bufsize];

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
   {
   php_printf("Error in cfcon_get_hub_colour needs a host key");
   RETURN_NULL();
   }

buffer[0] = '\0';
Con2PHP_get_hub_colour(hostkey,buffer,bufsize);

RETURN_STRING(buffer,1);
}

/*****************************************************************************
 * Name: cfcon_count_hub_hosts
 * Get the count of hub hosts according to given colour
 * @param string (hubkey)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted  
 *      2. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_count_hub_hosts)

{ char *hostkey, *fhostkey;
 int hk_len;
 const int bufsize = 10000;
 char buffer[bufsize];
 char *colour;
 int c_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&colour,&c_len) == FAILURE)
   {
   php_printf("Error in cfcon_get_hub_colour needs a host key");
   RETURN_NULL();
   }

if(*colour!='R' && *colour!='G' && *colour!='B' && *colour!='Y' && *colour!='A')
   {
   RETURN_STRING("{\"error\":{\"errid\":\"-1\",\"msg\":\"Unrecognized parameters supplied\"}}",1);
   }

fhostkey =  (hk_len == 0) ? NULL : hostkey;

buffer[0] = '\0';
colour[1]='\0';
Con2PHP_count_hub_hosts(fhostkey,colour,buffer,bufsize);

RETURN_STRING(buffer,1);
}

/*****************************************************************************
 * Name: cfcon_list_hubs
 * Return hubs(keyhash,name, IP)  according to given colour
 * @param string (hubkey)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted  
 *      2. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_list_hub_colour)

{ const int bufsize = 100000;
 char buffer[bufsize];
 char *colour;
 int c_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&colour,&c_len) == FAILURE)
   {
   php_printf("Error in cfcon_list_hub_colour needs a host key");
   RETURN_NULL();
   }

if(*colour!='R' && *colour!='G' && *colour!='B' && *colour!='Y' && *colour!='A')
   {
   RETURN_STRING("{\"error\":{\"errid\":\"-1\",\"msg\":\"Unrecognized parameters supplied\"}}",1);
   }

buffer[0] = '\0';
colour[1]='\0';
Con2PHP_list_hub_colour(colour,buffer,bufsize);
RETURN_STRING(buffer,1);
}


/*****************************************************************************
 * Name: cfcon_list_hubs
 * Return averages for kept, repaired and notkept data for Business value pie chart
 * @return
 *      1. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_value_graph)

{ const int bufsize = 100000;
 char buffer[bufsize];

buffer[0] = '\0'; 
Con2PHP_get_value_graph(buffer,bufsize);


RETURN_STRING(buffer,1);
}

/******************************************************************************/
#endif  /* HAVE_CONSTELLATION */

