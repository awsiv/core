abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

AM_CFLAGS = $(GCC_CFLAG) -I$(top_srcdir)/src -I$(top_srcdir)/nova/src $(CONSTELLATION_CFLAGS) @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

if MONGO

cfmod_la_SOURCES = cfmod.c \
	private-api.c private-api.h \
	public-api.c public-api.h

lib_LTLIBRARIES = cfmod.la
cfmod_la_LDFLAGS = -module -avoid-version -R$(workdir)/lib
cfmod_la_CFLAGS = -DCOMPILE_DL_CFMOD=1 $(AM_CFLAGS)

cfmod_la_LIBADD = $(abs_top_builddir)/src/libpromises.la

endif
