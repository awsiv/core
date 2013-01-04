abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

AM_CFLAGS = $(GCC_CFLAG) -I$(top_srcdir)/libpromises -I$(top_srcdir)/nova/src @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

if MONGO

phplibdir = @libdir@/php
phplib_LTLIBRARIES = cfmod.la

cfmod_la_SOURCES = cfmod.c \
        common.c common.h \
        private-api.c private-api.h \
        public-api.c public-api.h

cfmod_la_LDFLAGS = -module -avoid-version -R$(workdir)/lib
cfmod_la_CFLAGS = $(AM_CFLAGS)
cfmod_la_LIBADD = $(abs_top_builddir)/libpromises/libpromises.la

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(phplibdir)
	echo 'extension=cfmod.so' > $(DESTDIR)$(phplibdir)/cfmod.ini

endif
