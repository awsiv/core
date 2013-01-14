# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfserverd.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova \
	-I$(srcdir)/../../core/cf-serverd

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

libcfserverd_la_LIBADD = $(srcdir)/../../core/libpromises/libpromises.la

libcfserverd_la_SOURCES = server.c

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
