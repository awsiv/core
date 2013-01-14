# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfmonitord.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDLAGS@

libcfmonitord_la_LIBADD = $(srcdir)/../../core/libpromises/libpromises.la

libcfmonitord_la_SOURCES = \
        probes.c \
        mon_cumulative.c

if LINUX
libcfmonitord_la_SOURCES += mon_io_linux.c
libcfmonitord_la_SOURCES += mon_mem_linux.c
endif

if SOLARIS
libcfmonitord_la_SOURCES += mon_mem_solaris.c
libcfmonitord_la_SOURCES += shared_kstat.c
endif

if !LINUX
libcfmonitord_la_SOURCES += mon_io_stub.c
if !SOLARIS
libcfmonitord_la_SOURCES += mon_mem_stub.c
endif
endif

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
