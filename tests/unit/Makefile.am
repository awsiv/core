abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

CMOCKERY_DIR = $(top_srcdir)/tests/unit

AM_CFLAGS = $(NOVA_CFLAGS) $(CONSTELLATION_CFLAGS) -I$(top_srcdir)/src -I$(CMOCKERY_DIR)

MOCKERY_SOURCES = $(CMOCKERY_DIR)/cmockery.c

check_PROGRAMS = \
        conversion_test \
        logging_test \
        license_test \
        reporting_engine_test \
        structure_allocation_test

if MONGO
check_PROGRAMS += scorecards_test \
                  bson_lib_test

scorecards_test_SOURCES = $(MOCKERY_SOURCES) scorecards_test.c
scorecards_test_LDADD = $(top_builddir)/src/libpromises.la

bson_lib_test_SOURCES = $(MOCKERY_SOURCES) bson_lib_test.c
bson_lib_test_LDADD = $(top_builddir)/src/libpromises.la

endif

TESTS = $(check_PROGRAMS)

conversion_test_SOURCES = $(MOCKERY_SOURCES) conversion_test.c
conversion_test_LDADD = $(top_builddir)/src/libpromises.la

logging_test_SOURCES = $(MOCKERY_SOURCES) logging_test.c
logging_test_LDADD = $(top_builddir)/src/libpromises.la

license_test_SOURCES = $(MOCKERY_SOURCES) license_test.c
license_test_LDADD = $(top_builddir)/src/libpromises.la

reporting_engine_test_SOURCES = $(MOCKERY_SOURCES) reporting_engine_test.c
reporting_engine_test_LDADD = $(top_builddir)/src/libpromises.la

structure_allocation_test_SOURCES = $(MOCKERY_SOURCES) structure_allocation_test.c
structure_allocation_test_LDADD = $(top_builddir)/src/libpromises.la

