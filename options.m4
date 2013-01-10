define([novarevision], esyscmd([sh -c "cd nova && (git rev-list -1 --abbrev-commit HEAD || echo unknown) | tr -d '\n'" 2>/dev/null]))dnl
define([novaversion], [3.1.0a1.novarevision])

dnl Define shell variable as well to make it easier for buildsystem to pick up
NOVA_VERSION=novaversion
AC_DEFINE_UNQUOTED(NOVA_VERSION, "novaversion", "Nova version")

AC_ARG_WITH(nova,
	AS_HELP_STRING(--without-nova, [disable Nova enterprise extensions (enabled by default)]),[],
	with_nova=yes)

if test "x$with_nova" != xno; then
   AC_MSG_CHECKING([presence of Nova directory])
   if test -d "${srcdir}/nova"; then
     AC_DEFINE([HAVE_NOVA],[1],[Define if Nova extensions are to be enabled])
     NOVA_SUBDIR=nova
     NOVA_LIB_SUBDIRS='nova/sqlite3 nova/libcfnova nova/libcfagent nova/libcfmonitord nova/libcfserverd nova/libcfreport'
     NOVA_BIN_SUBDIRS='nova/cf-know nova/cf-hub'
     NOVA_CFLAGS='-I$(abs_top_srcdir)/nova/libcfnova'
     NOVA_LDADD='$(abs_top_builddir)/nova/libcfnova/libcfnova.la'
     AC_MSG_RESULT(yes)
   else
     AC_MSG_RESULT(no)
   fi
fi
AC_SUBST([NOVA_SUBDIR])
AC_SUBST([NOVA_LIB_SUBDIRS])
AC_SUBST([NOVA_BIN_SUBDIRS])
AC_SUBST([NOVA_CFLAGS])
AC_SUBST([NOVA_LDADD])

AM_CONDITIONAL(NEW_MP, test -d "../mission-portal")

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
    AC_CHECK_LIB(bson, bson_init, [], [AC_MSG_ERROR(Cannot find bson_init)])
    AC_CHECK_HEADERS(mongo.h, [], [AC_MSG_ERROR(Cannot find mongo.h)])
    AC_CHECK_HEADERS(bson.h, [], [AC_MSG_ERROR(Cannot find bson.h)])
  ])
fi

AM_CONDITIONAL(MONGO, test "x$ac_cv_lib_mongoc_mongo_connect" = xyes)

if test "x$ac_cv_lib_mongoc_mongo_connect" = xyes; then
   if test "x$use_docs"; then
      NOVA_DOC_SUBDIR=nova/knowledge
   fi
fi
AC_SUBST([NOVA_DOC_SUBDIR])

AC_ARG_WITH([ldap],
    [AS_HELP_STRING([--with-ldap[[=PATH]]], [Enable LDAP functions])],
    [], [with_ldap=check])

if test "x$with_ldap" != xno; then
   CF3_WITH_LIBRARY(ldap, [
      AC_CHECK_HEADERS(ldap.h, [], [if test "x$with_ldap" != xcheck; then AC_MSG_ERROR(Cannot find OpenLDAP library headers); fi])
      AC_CHECK_LIB(ldap, ldap_get_values_len, [], [if test "x$with_ldap" != xcheck; then AC_MSG_ERROR(Cannot find OpenLDAP library); fi])
      AC_CHECK_LIB(lber, ber_free, [], [if test "x$with_ldap" != xcheck; then AC_MSG_ERROR(Cannot find OpenLDAP lber library); fi])
   ])
fi

AC_ARG_WITH([enterprise-api],
    [AS_HELP_STRING([--with-enterprise-api[[=PATH]]], [enable Enterprise API using path to locate php-config])],
    [], [with_enterprise_api=check])

if test "x$with_enterprise_api" != xno; then
   if test "x$with_enterprise_api" != xyes && test "x$with_enterprise_api" != xcheck; then
      PHP_CONFIG=$with_enterprise_api
   else
      PHP_CONFIG=php-config
   fi

   if test x`which $PHP_CONFIG` != x ; then
      CPPFLAGS="$CPPFLAGS `$PHP_CONFIG --includes`"
   else
      if test "x$with_enterprise_api" != xcheck; then
         AC_MSG_ERROR([Unable to find php-dev])
      fi
      with_enterprise_api=no
   fi

   ENTERPRISE_API_SUBDIR=nova/api
fi
AC_SUBST([ENTERPRISE_API_SUBDIR])



AC_ARG_WITH([cfmod],
    [AS_HELP_STRING([--with-cfmod[[=PATH]]], [enable cfmod using path to locate php-config])],
    [], [with_cfmod=check])

if test "x$with_cfmod" != xno; then
   if test "x$with_cfmod" != xyes && test "x$with_cfmod" != xcheck; then
      PHP_CONFIG=$with_cfmod
   else
      PHP_CONFIG=php-config
   fi

   if test x`which $PHP_CONFIG` != x ; then
      CPPFLAGS="$CPPFLAGS `$PHP_CONFIG --includes`"
   else
      if test "x$with_cfmod" != xcheck; then
         AC_MSG_ERROR([Unable to find php-dev])
      fi
      with_cfmod=no
   fi

   CFMOD_SUBDIR=nova/cfmod
fi
AC_SUBST([CFMOD_SUBDIR])


AC_SEARCH_LIBS(kstat_open,kstat,[
        AC_DEFINE(HAVE_KSTAT,1,[We have Solaris kstat library])
        LDFLAGS="-lkstat $LDFLAGS"])

#
# AC_SEARCH_LIBS does not work for Win32 platform, as it does not include any
# headers in test programs, and compiler needs function prototypes to generate
# apropriate mangling for __stdcall calling convention.
#
case "$target_os" in
   mingw*)
      # WSAStartup
      LIBS="$LIBS -lws2_32"
      # GetProcessMemoryInfo
      LIBS="$LIBS -lpsapi"
      # CoInitialize
      LIBS="$LIBS -lole32"
      # GetActiveObject
      LIBS="$LIBS -loleaut32"
      # GetAdaptersAddresses
      LIBS="$LIBS -liphlpapi"
      # IID_IDispatch
      LIBS="$LIBS -luuid"
      ;;
esac