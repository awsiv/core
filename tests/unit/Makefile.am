abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

CMOCKERY_DIR = $(top_srcdir)/tests/unit

AM_CFLAGS = $(NOVA_CFLAGS) $(CONSTELLATION_CFLAGS) -I$(top_srcdir)/src -I$(CMOCKERY_DIR) -I$(top_srcdir)/nova/sqlite3

MOCKERY_SOURCES = $(CMOCKERY_DIR)/cmockery.c

check_PROGRAMS = \
        conversion_test \
        file_cache_test \
        license_test \
        structure_allocation_test

if MONGO
check_PROGRAMS += scorecards_test \
        reporting_engine_test \
        bson_lib_test \
        scheduled_reports_test \
        db_query_test \
        web_rbac_test \
        db_maintain_test

scorecards_test_SOURCES = $(MOCKERY_SOURCES) scorecards_test.c
scorecards_test_LDADD = $(top_builddir)/src/libpromises.la

bson_lib_test_SOURCES = $(MOCKERY_SOURCES) bson_lib_test.c
bson_lib_test_LDADD = $(top_builddir)/src/libpromises.la

reporting_engine_test_SOURCES = $(MOCKERY_SOURCES) reporting_engine_test.c
reporting_engine_test_LDADD = $(top_builddir)/src/libpromises.la

scheduled_reports_test_SOURCES = $(MOCKERY_SOURCES) scheduled_reports_test.c
scheduled_reports_test_LDADD = $(top_builddir)/src/libpromises.la

db_query_test_SOURCES = $(MOCKERY_SOURCES) db_query_test.c
db_query_test_LDADD = $(top_builddir)/src/libpromises.la

web_rbac_test_SOURCES = $(MOCKERY_SOURCES) web_rbac_test.c
web_rbac_test_LDADD = $(top_builddir)/src/libpromises.la

db_maintain_test_SOURCES = $(MOCKERY_SOURCES) db_maintain_test.c
db_maintain_test_LDADD = $(top_builddir)/src/libpromises.la

endif

TESTS = $(check_PROGRAMS)

conversion_test_SOURCES = $(MOCKERY_SOURCES) conversion_test.c
conversion_test_LDADD = $(top_builddir)/src/libpromises.la

license_test_SOURCES = $(MOCKERY_SOURCES) license_test.c
license_test_LDADD = $(top_builddir)/src/libpromises.la

structure_allocation_test_SOURCES = $(MOCKERY_SOURCES) structure_allocation_test.c
structure_allocation_test_LDADD = $(top_builddir)/src/libpromises.la

file_cache_test_SOURCES = $(MOCKERY_SOURCES) file_cache_test.c
file_cache_test_LDADD = $(top_builddir)/src/libpromises.la
