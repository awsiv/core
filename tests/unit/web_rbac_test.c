#include "test.h"
#include "cf.nova.h"
#include "web_rbac.h"

#if defined(HAVE_LIBMONGOC)

static void test_user_is_admin_when_rbac(void **state)
{
    CFDB_UserIsAdminWhenRBAC("admin");
}

static void test_create_update_delete_role(void **state)
{
    CFDB_CreateRole("admin", "jersey", "test", ".*", NULL, NULL, NULL);
    CFDB_UpdateRole("admin", "jersey", "test2", ".*", NULL, NULL, NULL);
    CFDB_DeleteRole("admin", "jersey", true);
}

#endif


int main()
{
    const UnitTest tests[] =
    {
#if defined(HAVE_LIBMONGOC)
        unit_test(test_user_is_admin_when_rbac),
        unit_test(test_create_update_delete_role)
#endif
    };

    PRINT_TEST_BANNER();
    return run_tests(tests);
}
