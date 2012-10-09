/*
 * PHP macros use standard memory allocation functions. Do not override those
 * with CFEngine macros.
 */
#define ALLOC_IMPL

#include "api-php.h"
#include "api.h"
#include "crypto.h"
#include "log.h"
#include "utils.h"
#include "db_query.h"
#include "cf.nova.h"

ZEND_DECLARE_MODULE_GLOBALS(cfapi)

/* True global resources - no need for thread safety here */

static zend_function_entry cfapi_functions[] =
{
    PHP_FE(cfapi, NULL)
    PHP_FE(cfapi_auth, NULL)

    // user
    PHP_FE(cfapi_user_list, NULL)
    PHP_FE(cfapi_user_get, NULL)
    PHP_FE(cfapi_user_put, NULL)
    PHP_FE(cfapi_user_post, NULL)
    PHP_FE(cfapi_user_delete, NULL)
    PHP_FE(cfapi_user_subscription_query_list, NULL)
    PHP_FE(cfapi_user_subscription_query_get, NULL)
    PHP_FE(cfapi_user_subscription_query_put, NULL)
    PHP_FE(cfapi_user_subscription_query_delete, NULL)

    // role
    PHP_FE(cfapi_role_list, NULL)
    PHP_FE(cfapi_role_get, NULL)
    PHP_FE(cfapi_role_put, NULL)
    PHP_FE(cfapi_role_post, NULL)
    PHP_FE(cfapi_role_delete, NULL)

    // settings
    PHP_FE(cfapi_settings_get, NULL)
    PHP_FE(cfapi_settings_post, NULL)

    // host
    PHP_FE(cfapi_host_list, NULL)
    PHP_FE(cfapi_host_get, NULL)
    PHP_FE(cfapi_host_delete, NULL)
    PHP_FE(cfapi_host_context_list, NULL)
    PHP_FE(cfapi_host_vital_list, NULL)
    PHP_FE(cfapi_host_vital_get, NULL)

    // promise
    PHP_FE(cfapi_promise_list, NULL)
    PHP_FE(cfapi_promise_get, NULL)

    // query
    PHP_FE(cfapi_query_post, NULL)

    {NULL, NULL, NULL}          /* Must be the last line in cfapi_functions[] */
};

zend_module_entry cfapi_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "cfengine-api",
    cfapi_functions,
    PHP_MINIT(cfapi),
    PHP_MSHUTDOWN(cfapi),
    PHP_RINIT(cfapi),           /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(cfapi),       /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(cfapi),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1",                      /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};

static void cfapi_init_globals(zend_cfapi_globals *globals)
{
    globals->query_cache = FileCacheNew("cfapi-query", "/tmp");
    if (globals->query_cache)
    {
        syslog(LOG_NOTICE, "Initialized file cache for queries");
    }
    else
    {
        syslog(LOG_ERR, "Error initializing file cache for queries");
    }
}

ZEND_GET_MODULE(cfapi)
     extern int AM_PHP_MODULE; // FIX: remove

     zend_class_entry *cfapi_exception_db;
     zend_class_entry *cfapi_exception_args;
     zend_class_entry *cfapi_exception_forbidden;
     zend_class_entry *cfapi_exception_bad_request;
     zend_class_entry *cfapi_exception;

PHP_MINIT_FUNCTION(cfapi)
{
    zend_class_entry *exception_class_entry = zend_exception_get_default(TSRMLS_CC);

    zend_class_entry cfapi_exception_db_class_entry;
    INIT_CLASS_ENTRY(cfapi_exception_db_class_entry, "CFAPIExceptionDB", NULL);
    cfapi_exception_db = zend_register_internal_class_ex(&cfapi_exception_db_class_entry,
                                                         exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfapi_exception_args_class_entry;
    INIT_CLASS_ENTRY(cfapi_exception_args_class_entry, "CFAPIExceptionArgs", NULL);
    cfapi_exception_args = zend_register_internal_class_ex(&cfapi_exception_args_class_entry,
                                                           exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfapi_exception_forbidden_class_entry;
    INIT_CLASS_ENTRY(cfapi_exception_forbidden_class_entry, "CFAPIExceptionForbidden", NULL);
    cfapi_exception_forbidden = zend_register_internal_class_ex(&cfapi_exception_forbidden_class_entry,
                                                             exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfapi_exception_bad_request_class_entry;
    INIT_CLASS_ENTRY(cfapi_exception_bad_request_class_entry, "CFAPIExceptionBadRequest", NULL);
    cfapi_exception_bad_request = zend_register_internal_class_ex(&cfapi_exception_bad_request_class_entry,
                                                                  exception_class_entry, NULL TSRMLS_CC);

    zend_class_entry cfapi_exception_class_entry;
    INIT_CLASS_ENTRY(cfapi_exception_class_entry, "CFAPIException", NULL);
    cfapi_exception = zend_register_internal_class_ex(&cfapi_exception_class_entry,
                                                      exception_class_entry, NULL TSRMLS_CC);

    AM_PHP_MODULE = 1; // FIX: remove

    {
        openlog("cfengine-enterprise-api", LOG_PERROR | LOG_NDELAY, LOG_USER);
        static const int log_fallback_level = LOG_DEBUG;

        EnterpriseDB *conn = EnterpriseDBAcquire();
        if (conn)
        {
            HubSettings *settings = NULL;
            if (CFDB_QuerySettings(conn, &settings) == ERRID_SUCCESS)
            {
                assert(settings);
                setlogmask(LOG_UPTO(settings->log_level));
                syslog(LOG_NOTICE, "Initialized log-level: %s", LogLevelToString(settings->log_level));
            }
            else
            {
                setlogmask(LOG_UPTO(log_fallback_level));
                syslog(LOG_ERR, "Unable to load settings from database, log-level set to: %s", LogLevelToString(log_fallback_level));
            }

            DeleteHubSettings(settings);
            EnterpriseDBRelease(conn);
        }
        else
        {
            setlogmask(LOG_UPTO(log_fallback_level));
            syslog(LOG_ERR, "Unable to connect to database, log-level set to: %s", LogLevelToString(log_fallback_level));
        }
    }

    CryptoInitialize();
    syslog(LOG_NOTICE, "Initialized crypto");

    ZEND_INIT_MODULE_GLOBALS(cfapi, cfapi_init_globals, NULL);

    syslog(LOG_NOTICE, "CFEngine Enterprise API module initialized");
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cfapi)
{
    syslog(LOG_NOTICE, "CFEngine Enterprise API module shutting down");

    FileCacheDestroy(CFAPI_G(query_cache));
    closelog();

    return SUCCESS;
}

PHP_RINIT_FUNCTION(cfapi)
{
    return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */

PHP_RSHUTDOWN_FUNCTION(cfapi)
{
    return SUCCESS;
}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cfapi)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "cfengine-api support", "enabled");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
       DISPLAY_INI_ENTRIES();
     */
}
