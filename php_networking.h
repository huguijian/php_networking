/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_NETWORKING_H
#define PHP_NETWORKING_H

extern zend_module_entry networking_module_entry;
#define phpext_networking_ptr &networking_module_entry


#ifdef PHP_WIN32
#	define PHP_NETWORKING_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_NETWORKING_API __attribute__ ((visibility("default")))
#else
#	define PHP_NETWORKING_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php.h"

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(networking)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(networking)
*/

/* Always refer to the globals in your function as NETWORKING_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define NETWORKING_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(networking, v)

#if defined(ZTS) && defined(COMPILE_DL_NETWORKING)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_NETWORKING_H */

#define NETWORKING_VERSION "v1.0"


#define NETWORKING_STARTUP_FUNCTION(module)  ZEND_MINIT_FUNCTION(networking_##module)
#define NETWORKING_RINIT_FUNCTION(module)    ZEND_RINIT_FUNCTION(networking_##module)
#define NETWORKING_STARTUP(module)           ZEND_MODULE_STARTUP_N(networking_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define NETWORKING_SHUTDOWN_FUNCTION(module) ZEND_MSHUTDOWN_FUNCTION(networking_##module)
#define NETWORKING_SHUTDOWN(module)          ZEND_MODULE_SHUTDOWN_N(networking_##module)(INIT_FUNC_ARGS_PASSTHRU)


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

