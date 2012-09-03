abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

AM_CFLAGS = $(GCC_CFLAG) -I$(top_srcdir)/src -I$(top_srcdir)/nova/src $(CONSTELLATION_CFLAGS) @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

if MONGO

phplibdir = @libdir@/php
phplib_LTLIBRARIES = cfengine-enterprise-api.la

cfengine_enterprise_api_la_SOURCES = api-php.c api-php.h \
        api.c api.h \
	api-host.c \
	api-role.c \
	api-settings.c \
	api-user.c \
        db-serialize.c db-serialize.h \
        utils.c utils.h

cfengine_enterprise_api_la_LDFLAGS = -module -avoid-version -R$(workdir)/lib
cfengine_enterprise_api_la_CFLAGS = $(AM_CFLAGS)
cfengine_enterprise_api_la_LIBADD = $(abs_top_builddir)/src/libpromises.la

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(phplibdir)
	echo 'extension=cfengine-enterprise-api.so' > $(DESTDIR)$(phplibdir)/cfengine-enterprise-api.ini

endif
