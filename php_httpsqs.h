/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
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

/* $Id: header 226204 2007-01-01 19:32:10Z iliaa $ */

#ifndef PHP_HTTPSQS_H
#define PHP_HTTPSQS_H

extern zend_module_entry httpsqs_module_entry;
#define phpext_httpsqs_ptr &httpsqs_module_entry

#ifdef PHP_WIN32
#define PHP_HTTPSQS_API __declspec(dllexport)
#else
#define PHP_HTTPSQS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

typedef struct httpsqs {
	char *hostname;
	int hostname_len;
	char *query;
	int query_len;
	int response_code;
	int response_pos;
	char *response_body;
} httpsqs_t;

#define HTTPSQS_RES_NAME "Httpsqs Connection"

int get_response(httpsqs_t *httpsqs, char *query, int query_len TSRMLS_DC);
int h_strcmp(const char *a, const char *b);
int get_response_code(char *response_body);

PHP_MINIT_FUNCTION(httpsqs);
PHP_MSHUTDOWN_FUNCTION(httpsqs);
PHP_RINIT_FUNCTION(httpsqs);
PHP_RSHUTDOWN_FUNCTION(httpsqs);
PHP_MINFO_FUNCTION(httpsqs);

PHP_FUNCTION(httpsqs_connect);
PHP_FUNCTION(httpsqs_get);
PHP_FUNCTION(httpsqs_put);
PHP_FUNCTION(httpsqs_status);
PHP_FUNCTION(httpsqs_view);
PHP_FUNCTION(httpsqs_reset);
PHP_FUNCTION(httpsqs_maxqueue);
PHP_FUNCTION(httpsqs_synctime);

ZEND_BEGIN_MODULE_GLOBALS(httpsqs)
	long default_port;
	char *default_charset;
	char *default_host;
	long timeout;
ZEND_END_MODULE_GLOBALS(httpsqs)

/* In every utility function you add that needs to use variables 
   in php_httpsqs_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as HTTPSQS_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define HTTPSQS_G(v) TSRMG(httpsqs_globals_id, zend_httpsqs_globals *, v)
#else
#define HTTPSQS_G(v) (httpsqs_globals.v)
#endif

#endif	/* PHP_HTTPSQS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
