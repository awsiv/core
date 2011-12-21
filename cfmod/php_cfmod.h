/*
   Copyright (C) Cfengine AS

   This file is part of Cfengine 3 - written and maintained by Cfengine AS.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

  To the extent this program is licensed as part of the Enterprise
  versions of Cfengine, the applicable Commerical Open Source License
  (COSL) may apply to this file if you as a licensee so wish it. See
  included file COSL.txt.
*/

#ifndef PHP_CFMOD_H
#define PHP_CFMOD_H

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

/* PHP has HAVE_LCHOWN in php_config.h */
#ifdef HAVE_LCHOWN
# undef HAVE_LCHOWN
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

extern zend_module_entry cfmod_module_entry;
#define phpext_cfmod_ptr &cfmod_module_entry

extern zend_class_entry *cfmod_ldap_exception_ce;

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

#include "private-api.h"

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
