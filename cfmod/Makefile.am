abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_srcdir)/../../core
top_builddir = $(abs_srcdir)/../../core
top_build_prefix = $(abs_srcdir)/../../core/

AM_CFLAGS = $(GCC_CFLAG) -I$(top_srcdir)/src -I$(top_srcdir)/nova/src $(CONSTELLATION_CFLAGS) @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

cfmod_la_SOURCES = cfmod.c

lib_LTLIBRARIES = cfmod.la
cfmod_la_LDFLAGS = -module -avoid-version -R$(workdir)/lib
cfmod_la_CFLAGS = -DCOMPILE_DL_CFMOD=1 $(AM_CFLAGS)

cfmod_la_LIBADD = $(top_srcdir)/src/libpromises.la
