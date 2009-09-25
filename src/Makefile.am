AM_CFLAGS = $(GCC_CFLAG) @CFLAGS@  
AM_YFLAGS = -d
LEXLIB = @LEXLIB@
LDADD = $(BERKELEY_DB_LDFLAGSD) $(BERKELEY_DB_LIB) $(OPENSSL_LIB_LDFLAGS) $(OPENSSL_LIB_LIB) libpromises.la 
INCLUDES = $(BERKELEY_DB_CFLAGS) $(OPENSSL_LIB_CPPFLAGS)
AM_LDFLAGS = $(BERKELEY_DB_LDFLAGS) $(OPENSSL_LIB_LDFLAGS) @LDFLAGS@

lib_LTLIBRARIES = libcfnova.la

libcfnova_la_LDFLAGS = -version-info 1:0:0 -no-undefined

# Separate out a library

libcfnova_la_SOURCES =						\
	cfnova.c						\
	acl.c							\
	acl_linux.c						\
	database.c						\
	sql.c							\
	ldap.c							\
	license.c						\
	registry.c						\
	monitoring.c						\
	aggregation.c						\
	server.c						\
	promises.c						\
	processes.c						\
	reporting.c						\
	html.c							\
	knowledge.c						\
	graphs.c						\
	copernicus.c						\
	histogram.c						\
	timeseries.c						\
	magnify.c						\
	coordinates.c						\
	cfcore.c                                                \
	win_api.c						\
	win_file.c						\
	win_proc.c						\
	win_ps.c


#
# Pickup header files here so Automake knows about them
#

noinst_HEADERS =							\
	cf.defs.h							\
	cf.extern.h							\
	prototypes3.h							\
	prototypes.h							\
	conf.h   							\
	cf3.defs.h							\
	cf3.extern.h							\
	snprintf.h

include_HEADERS = cf.nova.h

#
# Some basic clean ups
#

MOSTLYCLEANFILES = *~ *.orig *.rej

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 

CLEANFILES = cf3lex.c cf3parse.c cf3parse.h

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#

DISTCLEANFILES = cflex.c cfparse.c cfparse.h

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#

MAINTAINERCLEANFILES = Makefile.in conf.h.in stamp-h.in
