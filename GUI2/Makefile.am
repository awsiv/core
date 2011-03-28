top_srcdir = $(abs_srcdir)/../../core
top_builddir = $(abs_srcdir)/../../core
top_build_prefix = $(abs_srcdir)/../../core/

SUBDIRS = images
GUIdir = $(docdir)/GUI
GUI_DATA = *.php
EXTRA_DIST = *.php


#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES = $(htmlfiles) $(psfiles)

#
# Get everything removed down to where rebuilding requires:
# "configure; make; make install"
#
DISTCLEANFILES = *.log *.aux *.mbs *.mb *.info*

#
# Get everything removed down to where rebuilding requires:
# "aclocal; autoconf; autoheader; automake --add-missing"
# "configure; make; make install"
#
MAINTAINERCLEANFILES = Makefile.in mdate-sh
