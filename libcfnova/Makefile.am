# Working around Automake mishandling of symlinks
abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_build_builddir)/

noinst_LTLIBRARIES = libcfnova.la

AM_CPPFLAGS = @CPPFLAGS@ -I$(srcdir)/../../core/libpromises \
	$(LDAP_CPPFLAGS)

if MONGO
AM_CPPFLAGS += -I$(srcdir)/../sqlite3
endif

AM_CFLAGS = @CFLAGS@ \
	$(LDAP_CFLAGS)

AM_LDFLAGS = @LDFLAGS@ -no-undefined \
	$(LDAP_LDFLAGS)

libcfnova_la_LIBADD = \
	$(LDAP_LIBS)

if MONGO
libcfnova_la_LIBADD += ../sqlite3/libsqlite3.la
endif

libcfnova_la_SOURCES = \
        cfnova.c \
        ldap_lib.h \
        license.c license.h \
        log.c log.h \
        monitoring.c \
        ontology.c ontology.h \
        datapackaging.c \
        promises.c \
        nova_reporting.c nova_reporting.h \
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
        dataunpack.c datapack.h \
        db_common.c db_common.h \
        db_maintain.c db_maintain.h \
        db_query.c db_query.h \
        db_save.c db_save.h \
        file_cache.c file_cache.h \
        db_export_csv.c db_export_csv.h \
        histogram.c \
        install.c install.h \
        magnify.c \
        promise_db.c \
        runagent.c \
        reporting_engine.c reporting_engine.h \
        scorecards.c scorecards.h \
        stories.c \
        topicmap.c topicmap.h \
        web_report_export.c web_report_export.h \
        web_api.c \
        web_rbac.c \
        weekly.c \
        yearly.c
endif

if !MONGO
libcfnova_la_SOURCES += \
	stubs.c
endif

noinst_HEADERS = \
        win_disphelper.h \
	cf.nova.h \
	cf.nova.web_api.h

CLEANFILES = *.gcno *.gcda

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig *.rej
