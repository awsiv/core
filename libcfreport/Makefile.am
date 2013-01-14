# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfreport.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

libcfreport_la_LIBADD = $(srcdir)/../../core/libpromises/libpromises.la

libcfreport_la_SOURCES = syntax.c

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
