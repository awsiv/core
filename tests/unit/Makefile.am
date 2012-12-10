abs_srcdir=@abs_srcdir@
MKDIR_P=@mkdir_p@

top_srcdir = $(abs_top_srcdir)
top_builddir = $(abs_top_builddir)
top_build_prefix = $(abs_top_builddir)/

CMOCKERY_DIR = $(top_srcdir)/tests/unit

AM_CFLAGS = $(NOVA_CFLAGS) $(CONSTELLATION_CFLAGS) -I$(top_srcdir)/src -I$(CMOCKERY_DIR)

MOCKERY_SOURCES = $(CMOCKERY_DIR)/cmockery.c

check_PROGRAMS = \
        logging_test \
        map_test \
        license_test

if MONGO
check_PROGRAMS += scorecards_test

scorecards_test_SOURCES = $(MOCKERY_SOURCES) scorecards_test.c
scorecards_test_LDADD = $(top_builddir)/src/libpromises.la

check_PROGRAMS += db_data_unpack_test
db_data_unpack_test_SOURCES = $(MOCKERY_SOURCES) db_data_unpack_test.c
db_data_unpack_test_LDADD = $(top_builddir)/src/libpromises.la

check_PROGRAMS += db_maintain_test
db_maintain_test_SOURCES = $(MOCKERY_SOURCES) db_maintain_test.c
db_maintain_test_LDADD = $(top_builddir)/src/libpromises.la

endif

TESTS = $(check_PROGRAMS)

logging_test_SOURCES = $(MOCKERY_SOURCES) logging_test.c
logging_test_LDADD = $(top_builddir)/src/libpromises.la

map_test_SOURCES = $(MOCKERY_SOURCES) map_test.c
map_test_LDADD = $(top_builddir)/src/libpromises.la

license_test_SOURCES = $(MOCKERY_SOURCES) license_test.c
license_test_LDADD = $(top_builddir)/src/libpromises.la
