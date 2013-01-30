# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

if MONGO

sbin_PROGRAMS = cf-hub

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova \
	-I$(srcdir)/../sqlite3

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

LDADD = $(abs_top_srcdir)/libpromises/libpromises.la

cf_hub_SOURCES = \
        cf-hub.c cf-hub.h \
        cf-hub-maintenance.c cf-hub-maintenance.h \
        cf-hub-worker.c cf-hub-worker.h \
        cf-hub-scheduled-reports.c cf-hub-scheduled-reports.h
endif

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
