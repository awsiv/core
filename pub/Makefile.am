
AM_CFLAGS = @CFLAGS@ @GCC_CFLAG@
INCLUDES = @BERKELEY_DB_CFLAGS@ @OPENSSL_LIB_CPPFLAGS@

EXTRA_DIST =								\
	getopt.h
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
MAINTAINERCLEANFILES = Makefile.in
