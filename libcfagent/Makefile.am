abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

AM_CFLAGS = @CFLAGS@
LDADD = $(BERKELEY_DB_LDFLAGS) $(BERKELEY_DB_LIB) $(OPENSSL_LIB_LDFLAGS) $(OPENSSL_LIB_LIB) $(srcdir)/../../core/libpromises/libpromises.la
INCLUDES = $(BERKELEY_DB_CFLAGS) $(OPENSSL_LIB_CPPFLAGS)
AM_LDFLAGS = $(BERKELEY_DB_LDFLAGS) $(OPENSSL_LIB_LDFLAGS) @LDFLAGS@

AM_CPPFLAGS = -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova -I$(srcdir)/../../core/cf-serverd -I$(srcdir)/../sqlite3

noinst_LTLIBRARIES = libcfagent.la

libcfagent_la_SOURCES = \
        processes.c

if NT
libcfagent_la_SOURCES += \
	win_wmi.c \
	win_services.c
else
libcfagent_la_SOURCES += \
	services.c
endif

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#

MAINTAINERCLEANFILES = Makefile.in stamp-h.in

