# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

sbin_PROGRAMS = cf-know

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises -I$(srcdir)/../libcfnova

AM_CFLAGS = @CFLAGS@

AM_LDFLAGS = @LDFLAGS@

LDADD = $(abs_top_srcdir)/libpromises/libpromises.la

cf_know_SOURCES = \
        cf-know.c \
        knowledge.c knowledge.h


CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
