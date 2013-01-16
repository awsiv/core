# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfagent.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova \
	-I$(srcdir)/../../core/cf-agent -I$(srcdir)/../disphelper

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

libcfagent_la_LIBADD = $(srcdir)/../../core/libpromises/libpromises.la

if NT
libcfagent_la_LIBADD += ../disphelper/libdisphelper.la
endif

libcfagent_la_SOURCES = \
        processes.c

if NT
libcfagent_la_SOURCES += \
        acl_nt.c \
	win_wmi.c \
	win_services.c
else
libcfagent_la_SOURCES += \
	services.c
endif

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
