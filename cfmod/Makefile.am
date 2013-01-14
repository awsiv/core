# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

if MONGO

phplibdir = @libdir@/php
phplib_LTLIBRARIES = cfmod.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(top_srcdir)/libpromises -I$(top_srcdir)/nova/libcfnova 

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = -module -avoid-version -R$(workdir)/lib

cfmod_la_LIBADD = $(abs_top_builddir)/libpromises/libpromises.la

cfmod_la_SOURCES = cfmod.c \
        common.c common.h \
        private-api.c private-api.h \
        public-api.c public-api.h

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(phplibdir)
	echo 'extension=cfmod.so' > $(DESTDIR)$(phplibdir)/cfmod.ini

endif

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
