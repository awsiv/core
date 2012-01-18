abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

SUBDIRS = src docs GUI2 tests/unit

EXTRA_DIST = acconfig.h COPYING ChangeLog LICENSE

doc_DATA = ChangeLog

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

install-data-hook:
	mkdir -p $(DESTDIR)$(prefix)/inputs
	mkdir -p $(DESTDIR)$(prefix)/master_software_updates
	mkdir -p $(DESTDIR)$(prefix)/masterfiles
	mkdir -p $(DESTDIR)$(prefix)/modules
	mkdir -p $(DESTDIR)$(prefix)/outputs
	mkdir -p $(DESTDIR)$(prefix)/ppkeys
	cp -R $(abs_srcdir)/misc/software_updates $(DESTDIR)$(prefix)
	find $(DESTDIR)$(prefix)/software_updates -name .svn | xargs rm -rf
