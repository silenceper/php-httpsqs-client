dnl $Id$
dnl config.m4 for extension httpsqs

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(httpsqs, for httpsqs support,
dnl Make sure that the comment is aligned:
dnl [  --with-httpsqs             Include httpsqs support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(httpsqs, whether to enable httpsqs support,
dnl Make sure that the comment is aligned:
[  --enable-httpsqs           Enable httpsqs support])

if test "$PHP_HTTPSQS" = "yes"; then
  dnl Write more examples of tests here...

  dnl # --with-httpsqs -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/httpsqs.h"  # you most likely want to change this
  dnl if test -r $PHP_HTTPSQS/$SEARCH_FOR; then # path given as parameter
  dnl   HTTPSQS_DIR=$PHP_HTTPSQS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for httpsqs files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       HTTPSQS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$HTTPSQS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the httpsqs distribution])
  dnl fi

  dnl # --with-httpsqs -> add include path
  dnl PHP_ADD_INCLUDE($HTTPSQS_DIR/include)

  dnl # --with-httpsqs -> check for lib and symbol presence
  dnl LIBNAME=httpsqs # you may want to change this
  dnl LIBSYMBOL=httpsqs # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HTTPSQS_DIR/lib, HTTPSQS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_HTTPSQSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong httpsqs lib version or lib not found])
  dnl ],[
  dnl   -L$HTTPSQS_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(HTTPSQS_SHARED_LIBADD)
  AC_DEFINE(HAVE_HTTPSQS, 1, [Whether you have httpsqs support])
  PHP_NEW_EXTENSION(httpsqs, httpsqs.c, $ext_shared)
fi
