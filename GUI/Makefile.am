
SUBDIRS = img
GUIdir = $(docdir)/GUI
GUI_DATA = index.php *.php
EXTRA_DIST = index.php *.php


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
