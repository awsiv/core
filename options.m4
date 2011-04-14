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
	AS_HELP_STRING(--without-mongo, [disable MongoDB report aggregation database]),[],with_mongo=yes)

if test "x$with_mongo" = xyes; then
   AC_MSG_CHECKING(for libmongoc)
   WITH_MONGO=1
else
   WITH_MONGO=0
fi

if test $WITH_MONGO = 1; then
  AC_CHECK_HEADERS(mongo.h)
  AC_CHECK_HEADERS(mongo_except.h)
  AC_CHECK_HEADERS(bson.h)
  AC_CHECK_HEADERS(platform_hacks.h)
  AC_CHECK_LIB(mongoc,main)
fi

AC_ARG_WITH([ldap],
    [AS_HELP_STRING([--with-ldap], [support ldap functions])])
AS_IF([test "x$with_ldap" = "xyes"], [AC_CHECK_LIB([ldap], [ldap_get_values_len])])

AC_ARG_WITH([libvirt],
    [AS_HELP_STRING([--with-libvirt], [support virtual machine management])])
AS_IF([test "x$with_libvirt" = "xyes"], [WITH_VIRT=1], [WITH_VIRT=0])

if test $WITH_VIRT = 1; then
  AC_MSG_CHECKING(for libvirt)
  AC_CHECK_LIB(virt,main, [], [AC_MSG_ERROR(Cannot find libvirt)])
fi
