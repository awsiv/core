/*
 * PHP macros use standard memory allocation functions. Do not override those
 * with CFEngine macros.
 */
#define ALLOC_IMPL

#include "php_cfmod.h"
#include "public-api.h"
#include "private-api.h"
#include "crypto.h"

/* If you declare any globals in php_cfmod.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(cfmod)
*/

/* True global resources - no need for thread safety here */

static zend_function_entry cfmod_functions[] =
{
    // public API
    PHP_FE(cfmod_resource, NULL)
    PHP_FE(cfmod_resource_host, NULL)
    PHP_FE(cfmod_resource_host_id, NULL)
    PHP_FE(cfmod_resource_host_id_seen, NULL)
    PHP_FE(cfmod_resource_host_id_seenby, NULL)
    PHP_FE(cfmod_resource_promise_compliance, NULL)
    PHP_FE(cfmod_resource_promise_log_repaired, NULL)
    PHP_FE(cfmod_resource_promise_log_repaired_summary, NULL)
    PHP_FE(cfmod_resource_promise_log_notkept, NULL)
    PHP_FE(cfmod_resource_promise_log_notkept_summary, NULL)
    PHP_FE(cfmod_resource_variable, NULL)
    PHP_FE(cfmod_resource_context, NULL)
    PHP_FE(cfmod_resource_software, NULL)
    PHP_FE(cfmod_resource_setuid, NULL)
    PHP_FE(cfmod_resource_file, NULL)


    // private API
    PHP_FE(cfpr_get_class_frequency, NULL)
    PHP_FE(cfpr_report_class_frequency, NULL)
    PHP_FE(cfpr_network_speed, NULL)
    PHP_FE(cfpr_summary_meter, NULL)
    PHP_FE(cfpr_host_meter, NULL)
    PHP_FE(cfpr_report_overall_summary, NULL)
    PHP_FE(cfpr_hosts_compliance_for_bundles, NULL)
    PHP_FE(cfpr_hosts_compliance_for_promises, NULL)
    PHP_FE(cfpr_vitals_list, NULL)
    PHP_FE(cfpr_vitals_view_magnified, NULL)
    PHP_FE(cfpr_vitals_view_week, NULL)
    PHP_FE(cfpr_vitals_view_year, NULL)
    PHP_FE(cfpr_vitals_view_histogram, NULL)
    PHP_FE(cfpr_vitals_analyse_magnified, NULL)
    PHP_FE(cfpr_vitals_analyse_week, NULL)
    PHP_FE(cfpr_vitals_analyse_year, NULL)
    PHP_FE(cfpr_vitals_analyse_histogram, NULL)

    PHP_FE(agent_bundle_type_histogram, NULL)
    PHP_FE(list_promises_with_promisee, NULL)
    
    PHP_FE(cfpr_port_histogram, NULL)
    PHP_FE(cfpr_service_histogram, NULL)
    PHP_FE(cfpr_service_level_histogram, NULL)
    
        /* POLICY */
        // RBAC-IFIED
    PHP_FE(cfpr_promise_list_by_handle_rx, NULL)
    PHP_FE(cfpr_promise_list_by_bundle, NULL)
    PHP_FE(cfpr_promise_list_by_bundle_rx, NULL)
    PHP_FE(cfpr_promise_list_by_promiser, NULL)
    PHP_FE(cfpr_promise_list_by_promiser_rx, NULL)
    PHP_FE(cfpr_promise_list_by_promise_type, NULL)
    PHP_FE(cfpr_promise_details, NULL)
    PHP_FE(cfpr_bundle_list_all, NULL)
    PHP_FE(cfpr_bundle_agent_goals, NULL)
    PHP_FE(cfpr_bundle_for_topic, NULL)
    PHP_FE(cfpr_goal_progress, NULL)
    PHP_FE(cfpr_bundle_arguments, NULL)
    PHP_FE(cfpr_bundle_by_promise_handle, NULL)
    PHP_FE(cfpr_bundle_classes_used, NULL)
    PHP_FE(cfpr_bundle_list_by_bundle_usage, NULL)
        // PromiseFilter-IFIED
    PHP_FE(cfpr_get_bundle_type, NULL)      // TODO: DEPRECATE??
        // Not part of RBAC
    PHP_FE(cfpr_body_list, NULL)
    PHP_FE(cfpr_body_details, NULL)
        /* END POLICY */
    PHP_FE(cfpr_getlastupdate, NULL)
    PHP_FE(cfpr_list_business_goals, NULL)
    PHP_FE(cfpr_list_services, NULL)
    PHP_FE(cfpr_list_service_ports, NULL)
    PHP_FE(cfpr_list_knowledge_bundles, NULL)
    PHP_FE(cfpr_list_topics_for_bundle, NULL)

    PHP_FE(cfpr_report_software_in, NULL)
    PHP_FE(cfpr_report_patch_in, NULL)
    PHP_FE(cfpr_report_patch_avail, NULL)
    PHP_FE(cfpr_report_classes, NULL)
    PHP_FE(cfpr_class_cloud, NULL)
    PHP_FE(cfpr_report_vars, NULL)
    PHP_FE(cfpr_report_compliance_summary, NULL)
    PHP_FE(cfpr_compliance_summary_graph, NULL)
    PHP_FE(cfpr_report_compliance_promises, NULL)
    PHP_FE(cfpr_report_lastknown_compliance_promises, NULL)
    PHP_FE(cfpr_report_lastseen, NULL)
    PHP_FE(cfpr_report_performance, NULL)
    PHP_FE(cfpr_report_setuid, NULL)
    PHP_FE(cfpr_report_filechanges, NULL)
    PHP_FE(cfpr_report_filediffs, NULL)
    PHP_FE(cfpr_report_bundlesseen, NULL)
    PHP_FE(cfpr_report_lastknown_bundlesseen, NULL)
    PHP_FE(cfpr_class_list_distinct_by_name_rx, NULL)
    PHP_FE(cfpr_class_list_time_distinct_by_name_rx, NULL)
    PHP_FE(cfpr_class_list_soft_distinct_by_name_rx, NULL)
    PHP_FE(cfpr_get_value_graph, NULL)
    PHP_FE(cfpr_report_value, NULL)
    PHP_FE(cfpr_report_notkept, NULL)
    PHP_FE(cfpr_report_repaired, NULL)
    PHP_FE(cfpr_summarize_notkept, NULL)
    PHP_FE(cfpr_summarize_repaired, NULL)
    PHP_FE(cfpr_get_knowledge_view, NULL)
    PHP_FE(cfpr_host_compliance_list_all, NULL)
    PHP_FE(cfpr_select_reports, NULL)
    PHP_FE(cfpr_host_compliance_list, NULL)
    PHP_FE(cfpr_host_list_by_ip_rx, NULL)
    PHP_FE(cfpr_host_list_by_name_rx, NULL)
    PHP_FE(cfpr_host_compliance_colour, NULL)
    PHP_FE(cfpr_host_compliance_timeseries, NULL)
    PHP_FE(cfpr_host_compliance_timeseries_shifts, NULL)
    PHP_FE(cfpr_host_count, NULL)
    PHP_FE(cfpr_hosts_with_software_in, NULL)
    PHP_FE(cfpr_hosts_with_patch_in, NULL)
    PHP_FE(cfpr_hosts_with_patch_avail, NULL)
    PHP_FE(cfpr_hosts_with_classes, NULL)
    PHP_FE(cfpr_hosts_with_vars, NULL)
    PHP_FE(cfpr_hosts_with_compliance_summary, NULL)
    PHP_FE(cfpr_hosts_with_compliance_promises, NULL)
    PHP_FE(cfpr_hosts_with_lastknown_compliance_promises, NULL)
    PHP_FE(cfpr_hosts_with_lastseen, NULL)
    PHP_FE(cfpr_hosts_with_performance, NULL)
    PHP_FE(cfpr_hosts_with_setuid, NULL)
    PHP_FE(cfpr_hosts_with_filechanges, NULL)
    PHP_FE(cfpr_hosts_with_filediffs, NULL)
    PHP_FE(cfpr_hosts_with_repaired, NULL)
    PHP_FE(cfpr_hosts_with_notkept, NULL)
    PHP_FE(cfpr_hosts_with_bundlesseen, NULL)
    PHP_FE(cfpr_hosts_with_lastknown_bundlesseen, NULL)
    PHP_FE(cfpr_hosts_with_value, NULL)
    PHP_FE(cfpr_host_by_hostkey, NULL)
    PHP_FE(cfpr_host_info, NULL)
    PHP_FE(cfpr_show_topic, NULL)
    PHP_FE(cfpr_search_topics, NULL)
    PHP_FE(cfpr_show_topic_leads, NULL)
    PHP_FE(cfpr_show_all_context_leads, NULL)
    PHP_FE(cfpr_show_topic_hits, NULL)
    PHP_FE(cfpr_show_topic_category, NULL)
    PHP_FE(cfpr_get_pid_for_topic, NULL)
    PHP_FE(cfpr_list_documents, NULL)
    PHP_FE(cfpr_get_story_by_id, NULL)
    PHP_FE(cfpr_get_story_by_name, NULL)
    PHP_FE(cfpr_get_reverse_ip_lookup_name, NULL)

        /*
         * SVN helper
         */
    PHP_FE(cfpr_validate_policy, NULL)
        /*
         * Commenting
         */
    PHP_FE(cfpr_add_note, NULL)
    PHP_FE(cfpr_query_note, NULL)
    PHP_FE(cfpr_get_host_noteid, NULL)

    /*
     *  DB write functions
     */
    PHP_FE(cfpr_delete_host, NULL)
    PHP_FE(cfpr_update_host_identifier, NULL)
        /*
         * Replica sets
         */
    PHP_FE(cfpr_replica_status, NULL)
        /*
         * Environments
         */
    PHP_FE(cfpr_environment_list, NULL)

        /*
         * Hub replication
         */
    PHP_FE(cfpr_hub_key, NULL)
    PHP_FE(cfpr_get_hub_master, NULL)
        /*
         * Role-Based Access Control
         */
    PHP_FE(cfpr_user_authenticate, NULL)
        /*
         * Mission Tree-Control (Astrolabe)
         */
    PHP_FE(cfpr_astrolabe_host_list, NULL)

    {NULL, NULL, NULL}          /* Must be the last line in cfmod_functions[] */
};

zend_module_entry cfmod_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "cfmod",
    cfmod_functions,
    PHP_MINIT(cfmod),
    PHP_MSHUTDOWN(cfmod),
    PHP_RINIT(cfmod),           /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(cfmod),       /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(cfmod),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1",                      /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(cfmod)
     extern int AM_PHP_MODULE;  // TODO: Defined in cf.nova.web.api.h, but apparently need to redifine here??

     zend_class_entry *cfmod_exception_db;
     zend_class_entry *cfmod_exception_args;
     zend_class_entry *cfmod_exception_rbac;
     zend_class_entry *cfmod_exception_generic;

PHP_MINIT_FUNCTION(cfmod)
{
    zend_class_entry *exception_class_entry = zend_exception_get_default(TSRMLS_CC);

    zend_class_entry cfmod_exception_db_class_entry;

    INIT_CLASS_ENTRY(cfmod_exception_db_class_entry, "CFModExceptionDB", NULL);

    cfmod_exception_db = zend_register_internal_class_ex(&cfmod_exception_db_class_entry,
                                                         exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfmod_exception_args_class_entry;

    INIT_CLASS_ENTRY(cfmod_exception_args_class_entry, "CFModExceptionArgs", NULL);
    cfmod_exception_args = zend_register_internal_class_ex(&cfmod_exception_args_class_entry,
                                                           exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfmod_exception_rbac_class_entry;

    INIT_CLASS_ENTRY(cfmod_exception_rbac_class_entry, "CFModExceptionRBAC", NULL);
    cfmod_exception_rbac = zend_register_internal_class_ex(&cfmod_exception_rbac_class_entry,
                                                           exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfmod_exception_generic_class_entry;

    INIT_CLASS_ENTRY(cfmod_exception_generic_class_entry, "CFModExceptionGeneric", NULL);
    cfmod_exception_generic = zend_register_internal_class_ex(&cfmod_exception_generic_class_entry,
                                                              exception_class_entry, NULL TSRMLS_CC);

    AM_PHP_MODULE = 1;

    CryptoInitialize();

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
