# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfexecd.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova \
	-I$(srcdir)/../../core/cf-execd

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

libcfexecd_la_LIBADD = $(abs_top_srcdir)/libpromises/libpromises.la

libcfexecd_la_SOURCES =

if NT
libcfexecd_la_SOURCES += \
        win_service_exec.c
endif

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
