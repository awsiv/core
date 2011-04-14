# -*- mode: autoconf -*-

if test "x$NOVA_SUBDIR" != "x"; then
  AC_MSG_RESULT([ Nova: enabled])

  if test $WITH_MONGO = 1; then
    AC_MSG_RESULT([ -> MongoDB: enabled])
  else
    AC_MSG_RESULT([ -> MongoDB: disabled])
  fi

  if test "x$ac_cv_lib_ldap_ldap_get_values_len" = "xyes"; then
    AC_MSG_RESULT([ -> LDAP: enabled])
  else
    AC_MSG_RESULT([ -> LDAP: disabled])
  fi

  if test $WITH_VIRT = 1; then
    AC_MSG_RESULT([ -> libvirt: enabled])
  else
    AC_MSG_RESULT([ -> libvirt: disabled])
  fi
else
  AC_MSG_RESULT([ Nova: disabled])
fi
