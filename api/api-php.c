/*
 * PHP macros use standard memory allocation functions. Do not override those
 * with CFEngine macros.
 */
#define ALLOC_IMPL

#include "api-php.h"
#include "api.h"
#include "crypto.h"

/* If you declare any globals in cfengine-api-php.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(cfapi)
*/

/* True global resources - no need for thread safety here */

static zend_function_entry cfapi_functions[] =
{
    PHP_FE(cfapi, NULL)
    PHP_FE(cfapi_auth, NULL)

    PHP_FE(cfapi_role_list, NULL)
    PHP_FE(cfapi_role_get, NULL)
    PHP_FE(cfapi_role_put, NULL)
    PHP_FE(cfapi_role_post, NULL)
    PHP_FE(cfapi_role_delete, NULL)
    PHP_FE(cfapi_user_list, NULL)
    PHP_FE(cfapi_user_get, NULL)
    PHP_FE(cfapi_user_put, NULL)
    PHP_FE(cfapi_user_post, NULL)
    PHP_FE(cfapi_user_delete, NULL)
    PHP_FE(cfapi_settings_get, NULL)
    PHP_FE(cfapi_settings_post, NULL)

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

    CryptoInitialize();

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cfapi)
{
    /* uncomment this line if you have INI entries
       UNREGISTER_INI_ENTRIES();
     */
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
