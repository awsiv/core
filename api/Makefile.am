abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

AM_CFLAGS = -I$(top_srcdir)/libpromises -I$(top_srcdir)/nova/src -I$(top_srcdir)/nova/sqlite3 $(CONSTELLATION_CFLAGS) @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

if MONGO

phplibdir = @libdir@/php
phplib_LTLIBRARIES = cfengine-enterprise-api.la

cfengine_enterprise_api_la_SOURCES = api-php.c api-php.h \
        api.c api.h \
        api-query.c \
        api-host.c \
        api-promise.c \
        api-role.c \
        api-settings.c \
        api-user.c \
        db-serialize.c db-serialize.h \
        utils.c utils.h

cfengine_enterprise_api_la_LDFLAGS = -module -avoid-version -R$(workdir)/lib
cfengine_enterprise_api_la_CFLAGS = $(AM_CFLAGS)
cfengine_enterprise_api_la_LIBADD = $(abs_top_builddir)/libpromises/libpromises.la

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(phplibdir)
	echo 'extension=cfengine-enterprise-api.so' > $(DESTDIR)$(phplibdir)/cfengine-enterprise-api.ini

endif
