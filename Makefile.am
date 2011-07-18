abs_srcdir=@abs_srcdir@

top_srcdir = $(abs_srcdir)/../core
top_builddir = $(abs_srcdir)/../core
top_build_prefix = $(abs_srcdir)/../core/

SUBDIRS = src docs GUI2

EXTRA_DIST = acconfig.h COPYING ChangeLog LICENSE

docdir = $(pkgdatadir)

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

initdir= $(sysconfdir)/init.d
init_SCRIPTS = misc/init.d/cfengine3

sysconfigdir = $(sysconfdir)/sysconfig
sysconfig_DATA = misc/sysconfig/cfengine3

shprofiledir = $(sysconfdir)/profile.d
shprofile_SCRIPTS = misc/profile.d/cfengine.sh

install-data-hook:
	mkdir -p $(DESTDIR)$(prefix)/inputs
	mkdir -p $(DESTDIR)$(prefix)/master_software_updates
	mkdir -p $(DESTDIR)$(prefix)/masterfiles
	mkdir -p $(DESTDIR)$(prefix)/modules
	mkdir -p $(DESTDIR)$(prefix)/outputs
	mkdir -p $(DESTDIR)$(prefix)/ppkeys
	cp -R misc/software_updates $(DESTDIR)$(prefix)
