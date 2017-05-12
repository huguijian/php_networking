#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_networking.h"

PHP_MINIT_FUNCTION(networking)
{
    NETWORKING_STARTUP(server);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(networking)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(networking)
{

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(networking)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(networking)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "networking support", "enabled");
    php_info_print_table_row(2, "Version", NETWORKING_VERSION);
    php_info_print_table_end();
    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */



/* {{{ networking_module_entry
 */
zend_module_entry networking_module_entry = {
    STANDARD_MODULE_HEADER,
    "networking",
    NULL,
    PHP_MINIT(networking),
    PHP_MSHUTDOWN(networking),
    PHP_RINIT(networking),		/* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(networking),	/* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(networking),
    NULL,
    STANDARD_MODULE_PROPERTIES
};

/* }}} */

#ifdef COMPILE_DL_NETWORKING
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(networking)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

