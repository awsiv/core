abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

AM_CFLAGS = @CFLAGS@
LDADD = $(OPENSSL_LIB_LDFLAGS) $(OPENSSL_LIB_LIB) $(abs_top_srcdir)/libpromises/libpromises.la
INCLUDES = $(OPENSSL_LIB_CPPFLAGS)
AM_LDFLAGS = $(OPENSSL_LIB_LDFLAGS) @LDFLAGS@

AM_CPPFLAGS = -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova -I$(srcdir)/../sqlite3

if MONGO

sbin_PROGRAMS = cf-hub

cf_hub_SOURCES = \
        cf-hub.c \
        cf-hub-maintenance.c \
        cf-hub-worker.c \
        cf-hub-scheduled-reports.c cf-hub-scheduled-reports.h
endif

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

