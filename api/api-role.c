#include "api.h"

#include "web_rbac.h"
#include "utils.h"
#include "db-serialize.h"
#include "db_query.h"


PHP_FUNCTION(cfapi_role_list)
{
    syslog(LOG_DEBUG, "Requesting GET /api/role");

    const char *username = NULL; int username_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &username_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");

    HubQuery *result = CFDB_ListRoles(username);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error listing roles");
    }

    JsonElement *data = JsonArrayCreate(500);
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        JsonArrayAppendObject(data, HubRoleToJson((HubRole *)rp->item));
    }
    DeleteHubQuery(result, DeleteHubRole);

    RETURN_JSON(PackageResult(data, 1, JsonElementLength(data)));
}

PHP_FUNCTION(cfapi_role_get)
{
    syslog(LOG_DEBUG, "Requesting GET /api/role/:id");

    const char *username = NULL; int username_len = 0;
    const char *role = NULL; int role_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &role, &role_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(role_len, "role");

    HubQuery *result = CFDB_GetRoleByNameAuth(username, role);
    if (result->errid != ERRID_SUCCESS)
    {
        THROW_GENERIC(result->errid, "Error looking up role");
    }
    if (RlistLen(result->records) < 1)
    {
        RETURN_NULL();
    }

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, HubRoleToJson((HubRole *)result->records->item));

    DeleteHubQuery(result, DeleteHubRole);

    RETURN_JSON(PackageResult(data, 1, 1));
}

PHP_FUNCTION(cfapi_role_put)
{
    syslog(LOG_DEBUG, "Requesting PUT /api/role/:id");

    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;
    const char *description = NULL; int description_len = 0;
    const char *include_context_rx = NULL; int include_context_rx_len = 0;
    const char *exclude_context_rx = NULL; int exclude_context_rx_len = 0;
    const char *include_bundle_rx = NULL; int include_bundle_rx_len = 0;
    const char *exclude_bundle_rx = NULL; int exclude_bundle_rx_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &username, &username_len,
                              &name, &name_len,
                              &description, &description_len,
                              &include_context_rx, &include_context_rx_len,
                              &exclude_context_rx, &exclude_context_rx_len,
                              &include_bundle_rx, &include_bundle_rx_len,
                              &exclude_bundle_rx, &exclude_bundle_rx_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    if (!CFDB_UserIsAdminWhenRBAC(username))
    {
        THROW_GENERIC(ERRID_RBAC_ACCESS_DENIED, "Must be admin to add roles");
    }

    cfapi_errid err = CFDB_DeleteRole(username, name, true);
    if (err != ERRID_SUCCESS && err != ERRID_ITEM_NONEXISTING)
    {
        THROW_GENERIC(err, "Error trying to delete existing role");
    }

    if ((err = CFDB_CreateRole(username, name, description,
                               include_context_rx, exclude_context_rx,
                               include_bundle_rx, exclude_bundle_rx)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to create role");
    }

    RETURN_BOOL(true);
}


PHP_FUNCTION(cfapi_role_post)
{
    syslog(LOG_DEBUG, "Requesting POST /api/role/:id");

    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;
    const char *description = NULL; int description_len = 0;
    const char *include_context_rx = NULL; int include_context_rx_len = 0;
    const char *exclude_context_rx = NULL; int exclude_context_rx_len = 0;
    const char *include_bundle_rx = NULL; int include_bundle_rx_len = 0;
    const char *exclude_bundle_rx = NULL; int exclude_bundle_rx_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &username, &username_len,
                              &name, &name_len,
                              &description, &description_len,
                              &include_context_rx, &include_context_rx_len,
                              &exclude_context_rx, &exclude_context_rx_len,
                              &include_bundle_rx, &include_bundle_rx_len,
                              &exclude_bundle_rx, &exclude_bundle_rx_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    if (!CFDB_UserIsAdminWhenRBAC(username))
    {
        THROW_GENERIC(ERRID_RBAC_ACCESS_DENIED, "Must be admin to edit roles");
    }

    cfapi_errid err = ERRID_UNKNOWN;
    if ((err = CFDB_UpdateRole(username, name, description,
                              include_context_rx, exclude_context_rx,
                              include_bundle_rx, exclude_bundle_rx)) != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to update role");
    }

    RETURN_BOOL(true);
}


PHP_FUNCTION(cfapi_role_delete)
{
    syslog(LOG_DEBUG, "Requesting DELETE /api/role/:id");

    const char *username = NULL; int username_len = 0;
    const char *name = NULL; int name_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &name, &name_len) == FAILURE)
    {
        THROW_ARGS_MISSING();
    }

    ARGUMENT_CHECK_CONTENTS(username_len, "username");
    ARGUMENT_CHECK_CONTENTS(name_len, "name");

    cfapi_errid err = CFDB_DeleteRole(username, name, true);
    if (err != ERRID_SUCCESS)
    {
        THROW_GENERIC(err, "Unable to delete role");
    }

    RETURN_BOOL(true);
}
