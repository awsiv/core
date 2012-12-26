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

AM_CPPFLAGS = -I$(srcdir)/../../core/src -I$(srcdir)/../sqlite3

noinst_LTLIBRARIES = libcfnova.la libcfmonitord.la libcfreport.la libcfserverd.la libcfagent.la

libcfnova_la_LDFLAGS = -no-undefined

libcfnova_la_SOURCES = \
        cfnova.c \
        ldap_lib.c ldap_lib.h \
        license.c license.h \
        log.c log.h \
        monitoring.c \
        ontology.c \
        datapackaging.c \
        promises.c \
        nova-reporting.c nova-reporting.h \
        conversion.c

if NT
libcfnova_la_SOURCES += \
        acl_nt.c \
        registry.c \
        win_api.c \
        win_disphelper.c \
        win_file.c \
        win_log.c \
        win_mon.c \
        win_net.c \
        win_pipe.c \
        win_proc.c \
        win_ps.c \
        win_service_exec.c \
        win_sysinfo.c \
        win_user.c
endif

if MONGO
libcfnova_la_SOURCES += \
        bson_hub_conversion.c bson_hub_conversion.h \
        bson_lib.c bson_lib.h \
        client_code.c \
        cmd_api.c cf.nova.cmd_api.h \
        copernicus.c \
        dataunpack.c datapack.h \
        db_common.c db_common.h \
        db_maintain.c db_maintain.h \
        db_query.c db_query.h \
        db_save.c db_save.h \
        file-cache.c file-cache.h \
        db-export-csv.c db-export-csv.h \
        histogram.c \
        install.c install.h \
        magnify.c \
        promise_db.c \
        runagent.c \
        reporting-engine.c reporting-engine.h \
        scorecards.c scorecards.h \
        stories.c \
        topicmap.c \
        web-report-export.c web-report-export.h \
        web_api.c \
        web_rbac.c \
        weekly.c \
        yearly.c

libcfnova_la_LIBADD = ../sqlite3/libsqlite3.la
else # !MONGO
libcfnova_la_SOURCES += \
	stubs.c
endif

libcfagent_la_SOURCES = \
        processes.c

if NT
libcfagent_la_SOURCES += \
	win_wmi.c \
	win_services.c
else
libcfagent_la_SOURCES += \
	services.c
endif

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

libcfreport_la_SOURCES = syntax.c

libcfserverd_la_SOURCES = server.c

if MONGO
noinst_LTLIBRARIES += libcfhub.la

libcfhub_la_SOURCES = \
        hub.c \
        hub-maintenance.c \
        hub-worker.c \
        hub-scheduled-reports.c hub-scheduled-reports.h
endif

noinst_LTLIBRARIES += libcfknow.la

libcfknow_la_SOURCES = \
        cf-know.c \
        knowledge.c

#
# Pickup header files here so Automake knows about them
#

noinst_HEADERS = \
        win_disphelper.h cf.nova.h cf.nova.web_api.h

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

