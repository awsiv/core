abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@
INSTALL=$(abs_top_srcdir)/install-sh

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

SUBDIRS = sqlite3 libcfnova libcfagent libcfmonitord libcfreport libcfserverd cf-know cf-hub tests/unit

EXTRA_DIST = acconfig.h COPYING ChangeLog LICENSE

docsdir = $(projdocdir)
docs_DATA = ChangeLog.Nova

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES =

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#
DISTCLEANFILES = 

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.sub \
        configure install-sh missing mkinstalldirs depcomp ylwrap \
        ltmain.sh mdate-sh

#
# Pass proper flags to aclocal to pick up Libtool macros
#
ACLOCAL_AMFLAGS = -I m4

initdir = $(sysconfdir)/init.d
init_SCRIPTS = $(abs_srcdir)/misc/init.d/cfengine3

sysconfigdir = $(sysconfdir)/sysconfig
sysconfig_DATA = $(abs_srcdir)/misc/sysconfig/cfengine3

shprofiledir = $(sysconfdir)/profile.d
shprofile_SCRIPTS = $(abs_srcdir)/misc/profile.d/cfengine.sh

install-data-local:
if MONGO
	mkdir -p $(DESTDIR)$(prefix)/master_software_updates
	rm -rf GUI2
	mkdir -p GUI2
if NEW_MP
	cp -R ../mission-portal/* GUI2/
	mkdir GUI2/api/
	cp -R api/http/* GUI2/api/
	cp api/mongo-initialize.js GUI2/api/
	cp api/http/.htaccess GUI2/api/
endif

endif
