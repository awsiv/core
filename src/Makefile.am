top_srcdir = $(abs_srcdir)/../../core
top_builddir = $(abs_srcdir)/../../core
top_build_prefix = $(abs_srcdir)/../../core/

AM_CFLAGS = $(GCC_CFLAG) @CFLAGS@  
AM_YFLAGS = -d
LEXLIB = @LEXLIB@
LDADD = $(BERKELEY_DB_LDFLAGS) $(BERKELEY_DB_LIB) $(OPENSSL_LIB_LDFLAGS) $(OPENSSL_LIB_LIB) $(srcdir)/../../core/src/libpromises.la
INCLUDES = $(BERKELEY_DB_CFLAGS) $(OPENSSL_LIB_CPPFLAGS)
AM_LDFLAGS = $(BERKELEY_DB_LDFLAGS) $(OPENSSL_LIB_LDFLAGS) @LDFLAGS@

AM_CPPFLAGS = -I$(srcdir)/../../core/src $(CONSTELLATION_CFLAGS)

noinst_LTLIBRARIES = libcfnova.la libcfhub.la

libcfnova_la_LDFLAGS = -no-undefined

# Separate out a library

libcfnova_la_SOURCES =						\
	cfnova.c						\
	logging.c						\
	acl.c							\
	acl_linux.c						\
	acl_nt.c                                                \
	database.c						\
	db_save.c                                               \
        db_query.c                                              \
        db_maintain.c                                           \
	sql.c							\
	ldap.c							\
	license.c						\
	registry.c						\
	monitoring.c						\
	server.c						\
	datapackaging.c						\
	dataunpack.c						\
	client_code.c						\
	outputs.c						\
	promises.c						\
	promise_db.c						\
	packages.c						\
	processes.c						\
	services.c						\
	services_win.c						\
	environments.c						\
	reporting.c						\
	request.c						\
	html.c							\
	knowledge.c						\
	syntax.c						\
	scorecards.c						\
	graphics_core.c						\
	copernicus.c						\
	histogram.c						\
	yearly.c						\
	weekly.c						\
	magnify.c						\
	coordinates.c						\
	topicmap.c                                              \
	install.c						\
	web_api.c						\
	win_api.c						\
        win_disphelper.c                                        \
	win_file.c						\
	win_log.c                                               \
	win_mon.c                                               \
	win_net.c 						\
	win_pipe.c						\
	win_proc.c						\
	win_ps.c                                                \
	win_service_exec.c                                      \
	win_sysinfo.c                                           \
	win_user.c                                              \
        win_wmi.c

libcfhub_la_SOURCES =	                                        \
	hub.c
#
# Pickup header files here so Automake knows about them
#

noinst_HEADERS =							\
        win_disphelper.h

include_HEADERS = cf.nova.h cf.nova.web_api.h

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

MAINTAINERCLEANFILES = Makefile.in stamp-h.in

