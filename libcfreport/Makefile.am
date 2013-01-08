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

noinst_LTLIBRARIES = libcfreport.la

libcfreport_la_SOURCES = syntax.c

#
# Some basic clean ups
#

MOSTLYCLEANFILES = *~ *.orig *.rej

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#

MAINTAINERCLEANFILES = Makefile.in stamp-h.in

