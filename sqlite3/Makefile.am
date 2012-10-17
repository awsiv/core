abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

AM_CFLAGS = $(GCC_CFLAG)  @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

if MONGO

noinst_LTLIBRARIES = libsqlite3.la

libsqlite3_la_SOURCES = \
	sqlite3.c

EXTRA_DIST = \
	sqlite3.h

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES = *.gcno *.gcda

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#
DISTCLEANFILES = 

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#
MAINTAINERCLEANFILES = Makefile.in

endif