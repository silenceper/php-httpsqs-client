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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_httpsqs.h"

#define HTTPSQS_PUT_OK 			0
#define HTTPSQS_PUT_ERROR 		1
#define HTTPSQS_PUT_END 		2
#define HTTPSQS_GET_END 		3
#define HTTPSQS_RESET_OK 		4
#define HTTPSQS_RESET_ERROR 	5
#define HTTPSQS_MAXQUEUE_OK		6
#define HTTPSQS_MAXQUEUE_CANCEL 7
#define HTTPSQS_SYNCTIME_OK		8
#define HTTPSQS_SYNCTIME_CANCEL 9
#define HTTPSQS_ERROR			10

ZEND_DECLARE_MODULE_GLOBALS(httpsqs)

/* True global resources - no need for thread safety here */
static int le_httpsqs;
static zend_class_entry *httpsqs_class_entry_ptr;

/* {{{ httpsqs_functions[]
 *
 * Every user visible function must have an entry in httpsqs_functions[].
 */
zend_function_entry httpsqs_functions[] = {
	PHP_FE(httpsqs_connect, NULL)
	PHP_FE(httpsqs_get, NULL)
	PHP_FE(httpsqs_put, NULL)
	PHP_FE(httpsqs_status, NULL)
	PHP_FE(httpsqs_view, NULL)
	PHP_FE(httpsqs_reset, NULL)
	PHP_FE(httpsqs_maxqueue, NULL)
	PHP_FE(httpsqs_synctime, NULL)

	{NULL, NULL, NULL}	/* Must be the last line in httpsqs_functions[] */
};
/* }}} */

static zend_function_entry httpsqs_class_functions[] = {
	PHP_ME(Httpsqs, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FALIAS(get, httpsqs_get, NULL)
	PHP_FALIAS(put, httpsqs_put, NULL)
	PHP_FALIAS(status, httpsqs_status, NULL)
	PHP_FALIAS(view, httpsqs_view, NULL)
	PHP_FALIAS(reset, httpsqs_reset, NULL)
	PHP_FALIAS(maxqueue, httpsqs_maxqueue, NULL)
	PHP_FALIAS(synctime, httpsqs_synctime, NULL)
	{NULL, NULL, NULL}
};

/* {{{ httpsqs_module_entry
 */
zend_module_entry httpsqs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"httpsqs",
	httpsqs_functions,
	PHP_MINIT(httpsqs),
	PHP_MSHUTDOWN(httpsqs),
	PHP_RINIT(httpsqs),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(httpsqs),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(httpsqs),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HTTPSQS
ZEND_GET_MODULE(httpsqs)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("httpsqs.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_httpsqs_globals, httpsqs_globals)
    STD_PHP_INI_ENTRY("httpsqs.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_httpsqs_globals, httpsqs_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_httpsqs_init_globals
 */
static void php_httpsqs_init_globals(zend_httpsqs_globals *httpsqs_globals_p TSRMLS_DC)
{
	HTTPSQS_G(default_port) = 1218;
	HTTPSQS_G(default_charset) = "utf-8";
	HTTPSQS_G(default_host) = "127.0.0.1";
	HTTPSQS_G(timeout) = 20;
}
/* }}} */

static void php_httpsqs_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	httpsqs_t *httpsqs = (httpsqs_t *)rsrc->ptr;
	efree(httpsqs->response_body);
	efree(httpsqs->hostname);
	efree(httpsqs);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(httpsqs)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry httpsqs_class_entry;
	INIT_CLASS_ENTRY(httpsqs_class_entry, "Httpsqs", httpsqs_class_functions);
	httpsqs_class_entry_ptr = zend_register_internal_class(&httpsqs_class_entry TSRMLS_CC);

#ifdef ZTS
	ts_allocate_id(&httpsqs_globals_id, sizeof(zend_httpsqs_globals), (ts_allocate_ctor) php_httpsqs_init_globals, NULL);
#else
	php_httpsqs_init_globals(&httpsqs_globals TSRMLS_CC);
#endif

	le_httpsqs = zend_register_list_destructors_ex(php_httpsqs_dtor, NULL, HTTPSQS_RES_NAME, module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(httpsqs)
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
PHP_RINIT_FUNCTION(httpsqs)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(httpsqs)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(httpsqs)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "httpsqs support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

static void httpsqs_connect(INTERNAL_FUNCTION_PARAMETERS) {
	zval *object = getThis();
	httpsqs_t *httpsqs;
	char *hostname, *host = HTTPSQS_G(default_host);
	long port = HTTPSQS_G(default_port);
	int hostname_len, host_len, list_id;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &host_len, &port) == FAILURE) {
		RETURN_NULL();
	}

	hostname_len = spprintf(&hostname, 0, "%s:%d", host, port);

	httpsqs = emalloc(sizeof(httpsqs_t));
	httpsqs->hostname = estrndup(hostname, hostname_len);
	httpsqs->hostname_len = hostname_len;
	efree(hostname);

	if (!object) {
		ZEND_REGISTER_RESOURCE(return_value, httpsqs, le_httpsqs);
	} else {
		list_id = zend_list_insert(httpsqs, le_httpsqs);
		add_property_resource(object, "connection", list_id);
	}

}

PHP_METHOD(Httpsqs, __construct) {
	httpsqs_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(httpsqs_connect) {
	httpsqs_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(httpsqs_get) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	zend_bool return_array = 0;
	char *queuename, *charset = HTTPSQS_G(default_charset), *query;
	int queuename_len, charset_len, query_len;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bs", &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
			RETURN_NULL();
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"),(void **) &connection) == FAILURE) {
			RETURN_NULL();
		}
		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|bs", &hr, &queuename, &queuename_len, &return_array, &charset, &charset_len) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=get&charset=%s HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, charset);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);
	
	if (httpsqs->response_code == HTTPSQS_GET_END || httpsqs->response_code == HTTPSQS_ERROR) {
		RETURN_FALSE;
	} else {
		if (!return_array) {
			RETURN_STRING(httpsqs->response_body, 1);
		} else {
			zval *pos, *data;
			MAKE_STD_ZVAL(pos);
			MAKE_STD_ZVAL(data);

			ZVAL_LONG(pos, httpsqs->response_pos);
			ZVAL_STRING(data, httpsqs->response_body, 1);
		
			array_init(return_value);

			zend_hash_update(HASH_OF(return_value), "pos", sizeof("pos"), &pos, sizeof(zval*), NULL);
			zend_hash_update(HASH_OF(return_value), "data", sizeof("data"), &data, sizeof(zval*), NULL);
		}
	}
}

PHP_FUNCTION(httpsqs_put) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *data, *charset =HTTPSQS_G(default_charset), *query;
	int queuename_len, data_len, charset_len, query_len;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &queuename, &queuename_len, &data, &data_len, &charset, &charset_len) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|s", &hr, &queuename, &queuename_len, &data, &data_len, &charset, &charset_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "POST /?name=%s&opt=put&charset=%s HTTP/1.1\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length:%d\r\nConnection:close\r\n\r\n%s", queuename, charset, data_len, data);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);
	
	if (httpsqs->response_code == HTTPSQS_PUT_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}	
}

PHP_FUNCTION(httpsqs_status) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *charset = HTTPSQS_G(default_charset), *query;
	int queuename_len, charset_len, query_len;
	zend_bool return_json = 0;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bs", &queuename, &queuename_len, &return_json, &charset, &charset_len) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|bs", &hr, &queuename, &queuename_len, &return_json, &charset, &charset_len) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=%s&charset=%s HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, (return_json ? "status_json" : "status"), charset);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);

	if (httpsqs->response_code == HTTPSQS_ERROR) {
		RETURN_NULL();
	} else {
		RETURN_STRING(httpsqs->response_body, 1);
	}
}

PHP_FUNCTION(httpsqs_view) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *query, *charset = HTTPSQS_G(default_charset);
	int queuename_len, query_len, charset_len, pos;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|s", &queuename, &queuename_len, &pos, &charset, &charset_len) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|s", &hr, &queuename, &queuename_len, &pos, &charset, &charset_len) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=view&pos=%d&charset=%s HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, pos, charset);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);

	if (httpsqs->response_code == HTTPSQS_ERROR) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(httpsqs->response_body, 1);
	}
}

PHP_FUNCTION(httpsqs_reset) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *query;
	int queuename_len, query_len;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &queuename, &queuename_len) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &hr, &queuename, &queuename_len) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=reset HTTP/1.1\r\nConnection:close\r\n\r\n", queuename);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);


	if (httpsqs->response_code == HTTPSQS_RESET_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_FUNCTION(httpsqs_maxqueue) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *query;
	int queuename_len, query_len, maxqueue;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &queuename, &queuename_len, &maxqueue) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &hr, &queuename, &queuename_len, &maxqueue) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=maxqueue&num=%d HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, maxqueue);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);	
	
	if (httpsqs->response_code == HTTPSQS_MAXQUEUE_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}

}

PHP_FUNCTION(httpsqs_synctime) {
	httpsqs_t *httpsqs;
	zval *hr, **connection, *object = getThis();
	char *queuename, *query;
	int queuename_len, query_len, synctime;

	if (object) { 
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &queuename, &queuename_len, &synctime) == FAILURE) {
			RETURN_FALSE;
		}
		if (zend_hash_find(Z_OBJPROP_P(object), "connection", sizeof("connection"), (void **) &connection) == FAILURE) {
			RETURN_FALSE;
		}

		MAKE_STD_ZVAL(hr);
		ZVAL_RESOURCE(hr, Z_LVAL_PP(connection));
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &hr, &queuename, &queuename_len, &synctime) == FAILURE) {
			RETURN_NULL();
		}
	}

	ZEND_FETCH_RESOURCE(httpsqs, httpsqs_t*, &hr, -1, HTTPSQS_RES_NAME, le_httpsqs);

	query_len = spprintf(&query, 0, "GET /?name=%s&opt=synctime&num=%d HTTP/1.1\r\nConnection:close\r\n\r\n", queuename, synctime);
	if (!get_response(httpsqs, query, query_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	efree(query);

	if (httpsqs->response_code == HTTPSQS_SYNCTIME_OK) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

int get_response(httpsqs_t *httpsqs, char *query, int query_len TSRMLS_DC) {
	php_stream *stream;
	char *errstr = NULL;
	int errcode;
	struct timeval tv;

	tv.tv_sec = HTTPSQS_G(timeout);
	
	stream = php_stream_xport_create(httpsqs->hostname, httpsqs->hostname_len, ENFORCE_SAFE_MODE | REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, &tv, NULL, &errstr, &errcode); 
	
	if (errstr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d]%s", errcode, errstr);
		efree(errstr);
		return 0;
	}

	php_stream_write(stream, query, query_len);

	char buf[4096], *response, *eptr;
	int response_len, pos = 0;
	int bodyflag = 0, posflag = 0;

	while (!stream->eof) {
		response = php_stream_get_line(stream, buf, 1024, &response_len);
		
		// 将响应中第一次出现的Pos:作为队列的位置 
		if (strspn(response, "Pos:") && posflag == 0) {
			int j = 0, k = 0;
			char num[10];
			while (response[j] != '\0') {
				if (isdigit(response[j])) {
					num[k] = response[j];
					k++;
				}
				j++;
			} 
			num[k] = '\0';
			pos = strtol(num, &eptr, 10);
			posflag++;
		}
		
		// 说明这一行是body
		if (bodyflag == 1) {
			httpsqs->response_body = estrndup(response, response_len);
		}

		// \r\n是响应head和body的分割符
		if (h_strcmp(response, "\r\n")) {
			bodyflag++;
		}

	}

    php_stream_close(stream);
	httpsqs->response_pos = pos;
	httpsqs->response_code = get_response_code(httpsqs->response_body);
	
	return 1;
}


int h_strcmp(const char *a, const char *b) {
	if (strcmp(a, b) == 0) {
		return 1;
	}
	return 0;
}

int get_response_code(char *response) {
	if (h_strcmp(response, "HTTPSQS_ERROR")) {
		return HTTPSQS_ERROR;
	} else if (h_strcmp(response, "HTTPSQS_PUT_OK")) {
		return HTTPSQS_PUT_OK;
	} else if (h_strcmp(response, "HTTPSQS_PUT_END")) {
		return HTTPSQS_PUT_END;
	} else if (h_strcmp(response, "HTTPSQS_PUT_ERROR")) {
		return HTTPSQS_PUT_ERROR;
	} else if (h_strcmp(response, "HTTPSQS_GET_END")) {
		return HTTPSQS_GET_END;
	} else if (h_strcmp(response, "HTTPSQS_RESET_OK")) {
		return HTTPSQS_RESET_OK;
	} else if (h_strcmp(response, "HTTPSQS_RESET_ERROR")) {
		return HTTPSQS_RESET_ERROR;
	} else if (h_strcmp(response, "HTTPSQS_MAXQUEUE_OK")) {
		return HTTPSQS_MAXQUEUE_OK;
	} else if (h_strcmp(response, "HTTPSQS_MAXQUEUE_CANCEL")) {
		return HTTPSQS_MAXQUEUE_CANCEL;
	} else if (h_strcmp(response, "HTTPSQS_SYNCTIME_OK")) {
		return HTTPSQS_SYNCTIME_OK;
	} else if (h_strcmp(response, "HTTPSQS_SYNCTIME_CANCEL")) {
		return HTTPSQS_SYNCTIME_CANCEL;
	} else {
		return -1;
	}
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
