abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

# Make sure these get distributed with everything else.

SUFFIXES = html pdf dvi

%.8 : ../src/%
	../src/$< -M > `echo $<.8 | sed s/src/docs/`

%.html : %.texinfo
	${MAKEINFO} -I $(srcdir) --html --no-split $<

%.dvi : %.texinfo
	${TEXI2DVI} $<

%.pdf : %.texinfo
	${TEXI2PDF} $<

#
# Some basic clean ups
#
MOSTLYCLEANFILES = *~ *.orig

#
# Get everything removed down to where rebuilding requires:
# "make; make install"
# 
CLEANFILES = $(htmlfiles)

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
