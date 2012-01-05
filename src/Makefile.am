abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

AM_CFLAGS = $(GCC_CFLAG) @CFLAGS@  
AM_YFLAGS = -d
LEXLIB = @LEXLIB@
LDADD = $(BERKELEY_DB_LDFLAGS) $(BERKELEY_DB_LIB) $(OPENSSL_LIB_LDFLAGS) $(OPENSSL_LIB_LIB) $(srcdir)/../../core/src/libpromises.la
INCLUDES = $(BERKELEY_DB_CFLAGS) $(OPENSSL_LIB_CPPFLAGS)
AM_LDFLAGS = $(BERKELEY_DB_LDFLAGS) $(OPENSSL_LIB_LDFLAGS) @LDFLAGS@

AM_CPPFLAGS = -I$(srcdir)/../../core/src $(CONSTELLATION_CFLAGS)

noinst_LTLIBRARIES = libcfnova.la libcfmonitord.la

if MONGO
noinst_LTLIBRARIES += libcfhub.la
endif

libcfnova_la_LDFLAGS = -no-undefined

# Separate out a library

libcfnova_la_SOURCES =						\
	benchmark.c						\
	cfnova.c						\
	conversion.c						\
	logging.c logging.h					\
	acl.c							\
	acl_linux.c						\
	acl_nt.c                                                \
	bson_lib.c						\
	database.c						\
	db_save.c                                               \
        db_query.c db_query                                     \
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
	pscalar.c						\
	runagent.c						\
	services.c						\
	services_win.c						\
	environments.c						\
	reporting.c						\
	html.c							\
	knowledge.c						\
	syntax.c						\
	scorecards.c						\
	copernicus.c						\
	histogram.c						\
	yearly.c						\
	weekly.c						\
	magnify.c						\
	topicmap.c                                              \
	install.c						\
	cmd_api.c						\
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

if MONGO
libcfnova_la_SOURCES +=						\
	web_api.c
endif

libcfmonitord_la_LDFLAGS = -no-undefined

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

if MONGO
libcfhub_la_SOURCES =	                                        \
	hub.c
endif

#
# Pickup header files here so Automake knows about them
#

noinst_HEADERS =							\
        win_disphelper.h cf.nova.h cf.nova.web_api.h cf.nova.cmd_api.h bson_lib.h benchmark.h

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

