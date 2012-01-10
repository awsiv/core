#include "php_cfmod.h"

/* If you declare any globals in php_cfmod.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(cfmod)
*/

/* True global resources - no need for thread safety here */

static function_entry cfmod_functions[] =
    {
    // public API
    PHP_FE(cfmod_resource_report_list, NULL)
    PHP_FE(cfmod_resource_report_software_installed, NULL)

    // private API
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

    PHP_FE(cfpr_show_topic,NULL)
    PHP_FE(cfpr_search_topics,NULL)
    PHP_FE(cfpr_show_topic_leads,NULL)
    PHP_FE(cfpr_show_topic_hits,NULL)
    PHP_FE(cfpr_show_topic_category,NULL)
    PHP_FE(cfpr_get_pid_for_topic,NULL)

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
    
//    PHP_FE(cfcon_subscribe_software, NULL)
    PHP_FE(cfcon_local_subscribe_virtualbundle, NULL)
    PHP_FE(cfcon_local_delete_subscription_virtualbundle, NULL)

    
    
//    PHP_FE(cfcon_subscribe_repairlog, NULL)
//    PHP_FE(cfcon_subscribe_notkeptlog, NULL)
    
    
    
    PHP_FE(cfcon_report_software, NULL)
    PHP_FE(cfcon_local_report_virtualbundle, NULL)
    PHP_FE(cfcon_local_report_virtualbundle_promises, NULL)
    
    
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
    PHP_FE(cfcon_hub_details,NULL)
    PHP_FE(cfcon_getlastupdate,NULL)
    PHP_FE(cfcon_hubname,NULL)
    PHP_FE(cfcon_ipaddr,NULL)
    PHP_FE(cfcon_delete_hub,NULL)

    PHP_FE(cfcon_get_story_by_name,NULL)
    PHP_FE(cfcon_get_story_by_id,NULL)
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

extern int AM_PHP_MODULE;  // TODO: Defined in cf.nova.web.api.h, but apparently need to redifine here??

zend_class_entry *cfmod_ldap_exception_ce;
zend_class_entry *cfmod_exception_db;
zend_class_entry *cfmod_exception_args;

PHP_MINIT_FUNCTION(cfmod)
{
zend_class_entry *exception_class_entry = zend_exception_get_default(TSRMLS_CC);

zend_class_entry cfmod_ldap_exception_class_entry;
INIT_CLASS_ENTRY(cfmod_ldap_exception_class_entry, "CfmodLdapException", NULL);

cfmod_ldap_exception_ce = zend_register_internal_class_ex(
    &cfmod_ldap_exception_class_entry,
    exception_class_entry,
    NULL TSRMLS_CC);

zend_class_entry cfmod_exception_db_class_entry;
INIT_CLASS_ENTRY(cfmod_exception_db_class_entry, "CFModExceptionDB", NULL);

cfmod_exception_db = zend_register_internal_class_ex(
    &cfmod_exception_db_class_entry,
    exception_class_entry,
    NULL TSRMLS_CC);

zend_class_entry cfmod_exception_args_class_entry;
INIT_CLASS_ENTRY(cfmod_exception_args_class_entry, "CFModExceptionArgs", NULL);
cfmod_exception_args = zend_register_internal_class_ex(
    &cfmod_exception_args_class_entry,
    exception_class_entry,
    NULL TSRMLS_CC);

AM_PHP_MODULE = 1;

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

