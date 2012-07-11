# -*- mode: autoconf -*-

if test "x$NOVA_SUBDIR" != "x"; then
  AC_MSG_RESULT([ Nova: enabled])

  if test $WITH_MONGO = 1; then
    AC_MSG_RESULT([ -> MongoDB: $MONGO_PATH])
  else
    AC_MSG_RESULT([ -> MongoDB: disabled])
  fi

  if test $WITH_SQLITE3 = 1; then
    AC_MSG_RESULT([ -> Sqlite3: $SQLITE3_PATH])
  else
    AC_MSG_RESULT([ -> Sqlite3: disabled])
  fi

  if test "x$ac_cv_lib_ldap_ldap_get_values_len" = "xyes"; then
    AC_MSG_RESULT([ -> LDAP: $LDAP_PATH])
  else
    AC_MSG_RESULT([ -> LDAP: disabled])
  fi

  if test "x$with_enterprise_api" != xno; then
    AC_MSG_RESULT([ -> Enterprise API: enabled, using $PHP_CONFIG])
  else
    AC_MSG_RESULT([ -> Enterprise API: disabled])
  fi

  if test "x$with_cfmod" != xno; then
    AC_MSG_RESULT([ -> cfmod: enabled, using $PHP_CONFIG])
  else
    AC_MSG_RESULT([ -> cfmod: disabled])
  fi

else
  AC_MSG_RESULT([ Nova: disabled])
fi
