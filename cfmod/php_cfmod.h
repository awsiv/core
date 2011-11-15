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

#ifndef PHP_CFMOD_H
#define PHP_CFMOD_H

extern zend_module_entry cfmod_module_entry;
#define phpext_cfmod_ptr &cfmod_module_entry

#ifdef PHP_WIN32
#	define PHP_CFMOD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_CFMOD_API __attribute__ ((visibility("default")))
#else
#	define PHP_CFMOD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(cfmod);
PHP_MSHUTDOWN_FUNCTION(cfmod);
PHP_RINIT_FUNCTION(cfmod);
PHP_RSHUTDOWN_FUNCTION(cfmod);
PHP_MINFO_FUNCTION(cfmod);

PHP_FUNCTION(cfpr_enterprise_version);
PHP_FUNCTION(cfpr_community_version);
PHP_FUNCTION(cfpr_getlicense_summary);
PHP_FUNCTION(cfpr_getlicense_expiry);
PHP_FUNCTION(cfpr_getlicense_owner);
PHP_FUNCTION(cfpr_getlicenses_promised);
PHP_FUNCTION(cfpr_getlicenses_granted);
PHP_FUNCTION(cfpr_getlicense_installtime);
PHP_FUNCTION(cfpr_docroot);
PHP_FUNCTION(cfpr_get_class_frequency);
PHP_FUNCTION(cfpr_report_class_frequency);
PHP_FUNCTION(cfpr_get_network_rate);

PHP_FUNCTION(cfpr_summary_meter);
PHP_FUNCTION(cfpr_host_meter);
PHP_FUNCTION(cfpr_report_overall_summary);

PHP_FUNCTION(cfpr_vitals_list);
PHP_FUNCTION(cfpr_vitals_view_magnified);
PHP_FUNCTION(cfpr_vitals_view_week);
PHP_FUNCTION(cfpr_vitals_view_year);
PHP_FUNCTION(cfpr_vitals_view_histogram);
PHP_FUNCTION(cfpr_vitals_analyse_magnified);
PHP_FUNCTION(cfpr_vitals_analyse_week);
PHP_FUNCTION(cfpr_vitals_analyse_year);
PHP_FUNCTION(cfpr_vitals_analyse_histogram);

PHP_FUNCTION(cfpr_ldap_authenticate);
PHP_FUNCTION(cfpr_ldap_get_single_attribute_list);
PHP_FUNCTION(cfpr_ldap_get_several_attributes);

PHP_FUNCTION(cfpr_getlastupdate);

PHP_FUNCTION(cfpr_get_promise_body);
PHP_FUNCTION(cfpr_get_promise_comment);
PHP_FUNCTION(cfpr_get_help_text);
PHP_FUNCTION(cfpr_get_promise_bundle);
PHP_FUNCTION(cfpr_get_promise_type);
PHP_FUNCTION(cfpr_get_promiser);
PHP_FUNCTION(cfpr_get_variable);
PHP_FUNCTION(cfpr_get_classes_for_bundle);
PHP_FUNCTION(cfpr_get_args_for_bundle);
PHP_FUNCTION(cfpr_list_business_goals);

PHP_FUNCTION(cfpr_report_software_in);
PHP_FUNCTION(cfpr_report_patch_in);
PHP_FUNCTION(cfpr_report_patch_avail);
PHP_FUNCTION(cfpr_report_classes);
PHP_FUNCTION(cfpr_class_cloud);
PHP_FUNCTION(cfpr_report_vars);
PHP_FUNCTION(cfpr_report_compliance_summary);
PHP_FUNCTION(cfpr_compliance_summary_graph);
PHP_FUNCTION(cfpr_report_compliance_promises);
PHP_FUNCTION(cfpr_report_lastseen);
PHP_FUNCTION(cfpr_report_performance);
PHP_FUNCTION(cfpr_report_setuid);
PHP_FUNCTION(cfpr_report_filechanges);
PHP_FUNCTION(cfpr_report_filediffs);
PHP_FUNCTION(cfpr_report_bundlesseen);

PHP_FUNCTION(cfpr_report_filechanges_longterm);
PHP_FUNCTION(cfpr_report_filediffs_longterm);
PHP_FUNCTION(cfpr_list_documents);

PHP_FUNCTION(cfpr_list_time_classes);
PHP_FUNCTION(cfpr_list_soft_classes);
PHP_FUNCTION(cfpr_list_ip_classes);
PHP_FUNCTION(cfpr_list_all_classes);
PHP_FUNCTION(cfpr_list_host_classes);

PHP_FUNCTION(cfpr_hosts_with_software_in);
PHP_FUNCTION(cfpr_hosts_with_patch_in);
PHP_FUNCTION(cfpr_hosts_with_patch_avail);
PHP_FUNCTION(cfpr_hosts_with_classes);
PHP_FUNCTION(cfpr_hosts_with_vars);
PHP_FUNCTION(cfpr_hosts_with_compliance_summary);
PHP_FUNCTION(cfpr_hosts_with_compliance_promises);
PHP_FUNCTION(cfpr_hosts_with_lastseen);
PHP_FUNCTION(cfpr_hosts_with_performance);
PHP_FUNCTION(cfpr_hosts_with_setuid);
PHP_FUNCTION(cfpr_hosts_with_filechanges);
PHP_FUNCTION(cfpr_hosts_with_filediffs);
PHP_FUNCTION(cfpr_hosts_with_repaired);
PHP_FUNCTION(cfpr_hosts_with_notkept);
PHP_FUNCTION(cfpr_hosts_with_bundlesseen);
PHP_FUNCTION(cfpr_hosts_with_value);
PHP_FUNCTION(cfpr_report_value);
PHP_FUNCTION(cfpr_get_value_graph);
PHP_FUNCTION(cfpr_report_notkept);
PHP_FUNCTION(cfpr_report_repaired);
PHP_FUNCTION(cfpr_summarize_notkept);
PHP_FUNCTION(cfpr_summarize_repaired);
PHP_FUNCTION(cfpr_report_description);

PHP_FUNCTION(cfpr_hostname);
PHP_FUNCTION(cfpr_ipaddr);
PHP_FUNCTION(cfpr_hub_key);
PHP_FUNCTION(cfpr_get_hub_master);

PHP_FUNCTION(cfcon_get_story_by_name);
PHP_FUNCTION(cfcon_get_story_by_id);

PHP_FUNCTION(cfpr_get_pid_for_topic);
PHP_FUNCTION(cfpr_show_topic);
PHP_FUNCTION(cfpr_search_topics);
PHP_FUNCTION(cfpr_show_topic_leads);
PHP_FUNCTION(cfpr_show_topic_hits);
PHP_FUNCTION(cfpr_show_topic_category);

PHP_FUNCTION(cfpr_get_knowledge_view);

PHP_FUNCTION(cfpr_top_n_hosts);
PHP_FUNCTION(cfpr_select_hosts);
PHP_FUNCTION(cfpr_select_reports);

PHP_FUNCTION(cfpr_show_red_hosts);
PHP_FUNCTION(cfpr_show_yellow_hosts);
PHP_FUNCTION(cfpr_show_green_hosts);
PHP_FUNCTION(cfpr_show_blue_hosts);
PHP_FUNCTION(cfpr_show_hosts_ip);
PHP_FUNCTION(cfpr_show_hosts_name);
PHP_FUNCTION(cfpr_get_host_colour);

PHP_FUNCTION(cfpr_count_all_hosts);
PHP_FUNCTION(cfpr_count_red_hosts);
PHP_FUNCTION(cfpr_count_yellow_hosts);
PHP_FUNCTION(cfpr_count_green_hosts);
PHP_FUNCTION(cfpr_count_blue_hosts);
PHP_FUNCTION(cfpr_summarize_promise);
PHP_FUNCTION(cfpr_list_handles);
PHP_FUNCTION(cfpr_list_all_bundles);
PHP_FUNCTION(cfpr_list_bundles_using);
PHP_FUNCTION(cfpr_get_bundle_count);
PHP_FUNCTION(cfpr_get_bundle_type);
PHP_FUNCTION(cfpr_get_promise_count);
PHP_FUNCTION(cfpr_list_handles_for_bundle);
PHP_FUNCTION(cfpr_get_handles_for_bundle_with_comments);
PHP_FUNCTION(cfpr_list_bodies);

PHP_FUNCTION(cfpr_policy_finder_by_handle);
PHP_FUNCTION(cfpr_policy_finder_by_bundle);
PHP_FUNCTION(cfpr_policy_finder_by_promiser);

PHP_FUNCTION(cfpr_header);
PHP_FUNCTION(cfpr_footer);
PHP_FUNCTION(cfpr_menu);

PHP_FUNCTION(cfpr_delete_host);

PHP_FUNCTION(cfpr_environments_list);
PHP_FUNCTION(cfpr_environment_contents);
PHP_FUNCTION(cfpr_get_host_environment);

PHP_FUNCTION(cfpr_replica_status);

/*
 * For pdf generation
 */

PHP_FUNCTION(cfpr_report_compliance_summary_pdf);
PHP_FUNCTION(cfpr_report_bundlesseen_pdf);
PHP_FUNCTION(cfpr_report_value_pdf);
PHP_FUNCTION(cfpr_report_classes_pdf);
PHP_FUNCTION(cfpr_report_compliance_promises_pdf);
PHP_FUNCTION(cfpr_report_filechanges_pdf);
PHP_FUNCTION(cfpr_report_lastseen_pdf);
PHP_FUNCTION(cfpr_report_patch_avail_pdf);
PHP_FUNCTION(cfpr_report_patch_in_pdf);
PHP_FUNCTION(cfpr_report_software_in_pdf);
PHP_FUNCTION(cfpr_report_performance_pdf);
PHP_FUNCTION(cfpr_report_notkept_pdf);
PHP_FUNCTION(cfpr_report_repaired_pdf);
PHP_FUNCTION(cfpr_summarize_repaired_pdf);
PHP_FUNCTION(cfpr_summarize_notkept_pdf);
PHP_FUNCTION(cfpr_report_vars_pdf);
PHP_FUNCTION(cfpr_report_filediffs_pdf);
PHP_FUNCTION(cfpr_report_setuid_pdf);

/*
 * SVN helper
 */
PHP_FUNCTION(cfpr_validate_policy);

/*
 *  CDP reports
 */
PHP_FUNCTION(cfpr_cdp_reportnames);
PHP_FUNCTION(cfpr_cdp_report);
/*
 * Commenting
 */
PHP_FUNCTION(cfpr_add_note);
PHP_FUNCTION(cfpr_new_note);
PHP_FUNCTION(cfpr_query_note);
PHP_FUNCTION(cfpr_delete_host);
PHP_FUNCTION(cfpr_get_host_noteid);

/*
 * Constellation
 */

#ifdef HAVE_CONSTELLATION

PHP_FUNCTION(cfcon_compliance_summary_graph);
PHP_FUNCTION(cfcon_compliance_virtualbundle_graph);
PHP_FUNCTION(cfcon_count_repaired);
PHP_FUNCTION(cfcon_count_notkept);
PHP_FUNCTION(cfcon_reasons_repaired);
PHP_FUNCTION(cfcon_reasons_notkept);

PHP_FUNCTION(cfcon_count_hubs);
PHP_FUNCTION(cfcon_show_hubs);

PHP_FUNCTION(cfcon_environments_list);

PHP_FUNCTION(cfcon_promise_popularity);
PHP_FUNCTION(cfcon_rank_promise_popularity);


PHP_FUNCTION(cfcon_list_subscriptions);
PHP_FUNCTION(cfcon_local_show_subscription_virtualbundle);

PHP_FUNCTION(cfcon_subscribe_software);
PHP_FUNCTION(cfcon_local_subscribe_virtualbundle);
PHP_FUNCTION(cfcon_local_delete_subscription_virtualbundle);


PHP_FUNCTION(cfcon_subscribe_repairlog);


PHP_FUNCTION(cfcon_report_software);
PHP_FUNCTION(cfcon_local_report_virtualbundle);
PHP_FUNCTION(cfcon_local_report_virtualbundle_promises);

PHP_FUNCTION(cfcon_aggr_filechange);
PHP_FUNCTION(cfcon_aggr_software);
PHP_FUNCTION(cfcon_aggr_classes);
PHP_FUNCTION(cfcon_aggr_repaired);
PHP_FUNCTION(cfcon_aggr_notkept);

// meter and hub colour for observatory 
PHP_FUNCTION(cfcon_hub_meter);
PHP_FUNCTION(cfcon_get_hub_colour);
PHP_FUNCTION(cfcon_count_hubs_colour);
PHP_FUNCTION(cfcon_count_hub_hosts);

PHP_FUNCTION(cfcon_list_hub_colour);
PHP_FUNCTION(cfcon_value_graph);
PHP_FUNCTION(cfcon_hub_details);
PHP_FUNCTION(cfcon_getlastupdate);
PHP_FUNCTION(cfcon_hubname);
PHP_FUNCTION(cfcon_ipaddr);
#endif  /* HAVE_CONSTELLATION */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(cfmod)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(cfmod)
*/

/* In every utility function you add that needs to use variables 
   in php_cfmod_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CFMOD_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CFMOD_G(v) TSRMG(cfmod_globals_id, zend_cfmod_globals *, v)
#else
#define CFMOD_G(v) (cfmod_globals.v)
#endif

#endif	/* PHP_CFMOD_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 *
 */
