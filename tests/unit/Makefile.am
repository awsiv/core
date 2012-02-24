abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

CMOCKERY_DIR = $(top_srcdir)/tests/unit

AM_CFLAGS = $(NOVA_CFLAGS) $(CONSTELLATION_CFLAGS) -I$(top_srcdir)/src -I$(CMOCKERY_DIR)

MOCKERY_SOURCES = $(CMOCKERY_DIR)/cmockery.c

check_PROGRAMS = \
        logging_test

TESTS = $(check_PROGRAMS)

logging_test_SOURCES = $(MOCKERY_SOURCES) logging_test.c
logging_test_LDADD = $(top_builddir)/src/libpromises.la

