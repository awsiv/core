/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_PRIVATE_API_CONSTELLATION_H
#define CFENGINE_CFMOD_PRIVATE_API_CONSTELLATION_H

#include "php_cfmod.h"

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
PHP_FUNCTION(cfcon_delete_hub);

#endif /* HAVE_CONSTELLATION */

#endif
