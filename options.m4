define([novasvnversion], [r]esyscmd([sh -c "cd nova && svnversion --no-newline || echo exported | tr -d '\n'" 2>/dev/null]))dnl

AC_DEFINE(NOVA_VERSION, "2.1.0a1.novasvnversion", "Nova version")

AC_ARG_WITH(nova,
	AS_HELP_STRING(--without-nova, [disable Nova enterprise extensions (enabled by default)]),[],
	with_nova=yes)

if test "x$with_nova" != xno; then
   AC_MSG_CHECKING([presence of Nova directory])
   if test -d "${srcdir}/nova"; then
     AC_DEFINE([HAVE_NOVA],[1],[Define if Nova extensions are to be enabled])
     NOVA_SUBDIR=nova
     NOVA_CFLAGS='-I$(top_srcdir)/nova/src'
     NOVA_LDADD='$(top_srcdir)/nova/src/libcfnova.la'
     AC_MSG_RESULT(yes)
   else
     AC_MSG_RESULT(no)
   fi
fi
AC_SUBST([NOVA_SUBDIR])
AC_SUBST([NOVA_CFLAGS])
AC_SUBST([NOVA_LDADD])

#
# Report database driver - MongoDB C interface
#

AC_ARG_WITH(mongo,
	AS_HELP_STRING(--with-mongo[[=PATH]], [Use MongoDB as report aggregation database]), [], [with_mongo=yes])
AS_IF([test -n "$with_mongo" && test "x$with_mongo" != "xno"], [WITH_MONGO=1], [WITH_MONGO=0])

if test $WITH_MONGO = 1; then
  CF3_WITH_LIBRARY(mongo, [
    AC_DEFINE([MONGO_HAVE_STDINT], [], [Inform mongo header about stdint support])

    AC_CHECK_LIB(mongoc, mongo_connect, [], [AC_MSG_ERROR(Cannot find mongo_connect)])
    AC_CHECK_HEADERS(mongo.h, [], [AC_MSG_ERROR(Cannot find mongo.h)])
    AC_CHECK_HEADERS(mongo_except.h, [], [AC_MSG_ERROR(Cannot find mongo_except.h)])
    AC_CHECK_HEADERS(bson.h, [], [AC_MSG_ERROR(Cannot find bson.h)])
    AC_CHECK_HEADERS(platform_hacks.h, [], [AC_MSG_ERROR(Cannot find platform_hacks.h)])

    AC_CHECK_MEMBER([mongo_connection_options.host], AC_DEFINE([MONGO_OLD_CONNECT], [], [Use deprecated MongoDB client API]),[],[[#include <mongo.h>]])
  ])
fi

AC_ARG_WITH([ldap],
    [AS_HELP_STRING([--with-ldap[[=PATH]]], [Enable LDAP functions])],
    [], [with_ldap=check])

if test "x$with_ldap" != xno; then
   CF3_WITH_LIBRARY(ldap, [
      AC_CHECK_HEADERS(ldap.h, [], [if test "x$with_ldap" != xcheck; then AC_MSG_ERROR(Cannot find OpenLDAP library headers); fi])
      AC_CHECK_LIB(ldap, ldap_get_values_len, [], [if test "x$with_ldap" != xcheck; then AC_MSG_ERROR(Cannot find OpenLDAP library); fi])
   ])
fi

AC_ARG_WITH([libvirt],
    [AS_HELP_STRING([--with-libvirt], [support virtual machine management])],
    [], [with_libvirt=check])

if test "x$with_libvirt" != xno; then
   CF3_WITH_LIBRARY(libvirt, [
      AC_CHECK_LIB(virt, virConnectOpen, [], [if test "x$with_libvirt" != xcheck; then AC_MSG_ERROR(Cannot find libvirt library); fi])
      AC_CHECK_HEADERS(libvirt/libvirt.h, [], [if test "x$with_libvirt" != xcheck; then AC_MSG_ERROR(Cannot find libvirt library headers); fi])
   ])
fi

AC_SEARCH_LIBS(kstat_open,kstat,[
        AC_DEFINE(HAVE_KSTAT,1,[We have Solaris kstat library])
        LDFLAGS="-lkstat $LDFLAGS"])
