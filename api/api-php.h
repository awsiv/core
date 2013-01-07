/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_API_PHP_H
#define CFENGINE_API_PHP_H

#ifdef HAVE_CONFIG_H
# include "conf.h"
#endif

/* PHP has HAVE_LCHOWN in php_config.h */
#ifdef HAVE_LCHOWN
# undef HAVE_LCHOWN
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

extern zend_module_entry cfapi_module_entry;

#define phpext_cfapi_ptr &cfapi_module_entry

extern zend_class_entry *cfapi_exception_db;
extern zend_class_entry *cfapi_exception_args;
extern zend_class_entry *cfapi_exception_forbidden;
extern zend_class_entry *cfapi_exception_bad_request;
extern zend_class_entry *cfapi_exception;

#ifdef PHP_WIN32
# define CFAPI_PHP __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define CFAPI_PHP __attribute__ ((visibility("default")))
#else
# define CFAPI_API_PHP
#endif

#ifdef ZTS
# include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(cfapi);
PHP_MSHUTDOWN_FUNCTION(cfapi);
PHP_RINIT_FUNCTION(cfapi);
PHP_RSHUTDOWN_FUNCTION(cfapi);
PHP_MINFO_FUNCTION(cfapi);

#include "file_cache.h"

ZEND_BEGIN_MODULE_GLOBALS(cfapi)
    FileCache *query_cache;
ZEND_END_MODULE_GLOBALS(cfapi)


/* In every utility function you add that needs to use variables
   in cfapi_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CFAPI_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
# define CFAPI_G(v) TSRMG(cfapi_globals_id, zend_cfapi_globals *, v)
#else
# define CFAPI_G(v) (cfapi_globals.v)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 *
 */

#endif
