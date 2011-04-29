abs_srcdir=@abs_srcdir@

top_srcdir = $(abs_srcdir)/../../../core
top_builddir = $(abs_srcdir)/../../../core
top_build_prefix = $(abs_srcdir)/../../../core/

GUIdir = $(docdir)/GUI/images
GUI_DATA = *
EXTRA_DIST = *


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
