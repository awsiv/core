# -*- mode: autoconf -*-

if test "x$NOVA_SUBDIR" != "x"; then
  AC_MSG_RESULT([ Nova: enabled])

  if test $WITH_MONGO = 1; then
    AC_MSG_RESULT([ -> MongoDB: $MONGO_PATH])
  else
    AC_MSG_RESULT([ -> MongoDB: disabled])
  fi

  if test "x$ac_cv_lib_ldap_ldap_get_values_len" = "xyes"; then
    AC_MSG_RESULT([ -> LDAP: $LDAP_PATH])
  else
    AC_MSG_RESULT([ -> LDAP: disabled])
  fi

  if test "x$ac_cv_lib_virt_virConnectOpen" = xyes; then
    AC_MSG_RESULT([ -> libvirt: $LIBVIRT_PATH])
  else
    AC_MSG_RESULT([ -> libvirt: disabled])
  fi
else
  AC_MSG_RESULT([ Nova: disabled])
fi
