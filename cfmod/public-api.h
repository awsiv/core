#ifndef CFENGINE_CFMOD_PUBLIC_API_H
#define CFENGINE_CFMOD_PUBLIC_API_H

#include "php_cfmod.h"

PHP_FUNCTION(cfmod_resource_host);
PHP_FUNCTION(cfmod_resource_host_id);
PHP_FUNCTION(cfmod_resource_host_id_seen);
PHP_FUNCTION(cfmod_resource_host_id_seen_by);

PHP_FUNCTION(cfmod_resource_promise_compliance);
PHP_FUNCTION(cfmod_resource_promise_log_repaired);
PHP_FUNCTION(cfmod_resource_promise_log_repaired_summary);
PHP_FUNCTION(cfmod_resource_promise_log_notkept);
PHP_FUNCTION(cfmod_resource_promise_log_notkept_summary);

PHP_FUNCTION(cfmod_resource_variable);

PHP_FUNCTION(cfmod_resource_context);

PHP_FUNCTION(cfmod_resource_software);

PHP_FUNCTION(cfmod_resource_setuid);

PHP_FUNCTION(cfmod_resource_report_bundle_profile);
PHP_FUNCTION(cfmod_resource_report_software_installed);


#endif
