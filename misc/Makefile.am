abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

install-data-local:
	mkdir -p $(DESTDIR)$(prefix)/share
	cp -R $(abs_srcdir)/solaris_admin_files $(DESTDIR)$(prefix)/share
	find $(DESTDIR)$(prefix)/share/solaris_admin_files -name .svn | xargs rm -rf
