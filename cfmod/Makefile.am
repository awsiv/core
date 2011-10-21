abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_srcdir)/../../core
top_builddir = $(abs_srcdir)/../../core
top_build_prefix = $(abs_srcdir)/../../core/

AM_CFLAGS = $(GCC_CFLAG) @CFLAGS@  
AM_CPPFLAGS = -I$(srcdir)/../../core/src $(CONSTELLATION_CFLAGS)

cfmod_la_SOURCES = cfmod.c

lib_LTLIBRARIES = cfmod.la
cfmod_la_LDFLAGS = -module -avoid-version

cfmod_la_LIBADD = $(top_srcdir)/src/libpromises.la
