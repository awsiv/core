#include "private-api.h"

#include "env_context.h"
#include "common.h"
#include "rlist.h"
#include "cf.nova.web_api.h"
#include "web_rbac.h"
#include "map.h"
#include "scorecards.h"
#include "granules.h"
#include "misc_lib.h"

#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"

char **String2StringArray(char *str, char separator);
void FreeStringArray(char **strs);
static JsonElement *ParseRolesToJson(HubQuery *hq);


/******************************************************************************/
/* Common response payload keys                                               */
/******************************************************************************/
static const char *LABEL_HOST_KEY = "hostkey";
static const char *LABEL_HOST_NAME = "hostname";
static const char *LABEL_IP = "ip";
static const char *LABEL_COLOUR = "colour";
static const char *LABEL_OS_TYPE = "osType";
static const char *LABEL_FLAVOUR = "flavour";
static const char *LABEL_RELEASE = "release";
static const char *LABEL_LAST_REPORT_UPDATE = "lastReportUpdate";
static const char *LABEL_LAST_POLICY_UPDATE = "lastPolicyUpdate";
static const char *LABEL_POSITION = "position";
static const char *LABEL_TIMESTAMP = "timestamp";
static const char *LABEL_RESOLUTION = "resolution";
static const char *LABEL_COUNT = "count";
static const char *LABEL_FROM = "from";
static const char *LABEL_DATA = "data";
static const char *LABEL_GREEN = "green";
static const char *LABEL_RED = "red";
static const char *LABEL_YELLOW = "yellow";

/******************************************************************************/
/* API                                                                        */
/******************************************************************************/

/******************************************************************************/

PHP_FUNCTION(cfpr_get_story_by_id)
{
    const int bufsize = 1000000;
    char buffer[bufsize];
    long id;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "l", &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Con2PHP_get_story_by_id((int) id, buffer, bufsize);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_story_by_name)
{
    const int bufsize = 1000000;
    char buffer[bufsize];
    char *search;
    int s_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &search, &s_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(s_len);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hub_key)
{
    const int bufsize = 4096;
    char buffer[bufsize];

    buffer[0] = '\0';
    if (Nova2PHP_GetHubKey(buffer, bufsize))
    {
        RETURN_STRING(buffer, 1);
    }

    RETURN_STRING("Cannot find Hub hostkey!", 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_hub_master)
{
    const int bufsize = 4096;
    char buffer[bufsize];

    buffer[0] = '\0';

    if (Nova2PHP_GetHubMaster(buffer, bufsize))
    {
        RETURN_STRING(buffer, 1);
    }

    RETURN_STRING("Unknown Hub Master!", 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_by_hostkey)
{
    char *userName, *hostKey;
    int user_len, hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hq = CFDB_QueryHostByHostKey(&conn, hostKey);

    DATABASE_CLOSE(&conn);

    if (!(hq->hosts && hq->hosts->item))
    {
        DeleteHubQuery(hq, NULL);
        zend_throw_exception(cfmod_exception_generic, "No host matches the given hostkey", 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubHost *hh = hq->hosts->item;

    JsonElement *hostinfo = JsonArrayCreate(2);

    JsonArrayAppendString(hostinfo, hh->hostname);
    JsonArrayAppendString(hostinfo, hh->ipaddr);

    DeleteHubQuery(hq, NULL);

    RETURN_JSON(hostinfo);
}

PHP_FUNCTION(cfpr_host_info)
{
    char *username = NULL, *hostkey = NULL;
    int username_len = -1, hostkey_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len,
                              &hostkey, &hostkey_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len && hostkey_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(username, hostkey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    EnterpriseDB conn;
    DATABASE_OPEN(&conn);

    HubHost *host = CFDB_GetHostByKey(&conn, hostkey);
    if (!host) {
        RETURN_NULL();
    }

    HubQuery *result = CFDB_QueryVariables(&conn, hostkey, NULL, NULL, NULL, NULL, false, 0, time(NULL), NULL);

    time_t last_report_update = -1;
    int last_update_size = 0;

    CFDB_QueryLastUpdate(&conn, MONGO_DATABASE, cfr_keyhash, hostkey, &last_report_update, &last_update_size);

    DATABASE_CLOSE(&conn);

    JsonElement *infoObject = JsonObjectCreate(10);

    JsonObjectAppendString(infoObject, LABEL_HOST_KEY, host->keyhash);
    JsonObjectAppendString(infoObject, LABEL_HOST_NAME, host->hostname);
    JsonObjectAppendString(infoObject, LABEL_IP, host->ipaddr);
    JsonObjectAppendInteger(infoObject, LABEL_LAST_REPORT_UPDATE, last_report_update);

    for (Rlist *rp = result->records; rp; rp = rp->next)
    {
        HubVariable *var = (HubVariable *) rp->item;

        if (var && strcmp(var->scope, "sys") == 0)
        {
            if (strcmp(var->lval, "ostype") == 0)
            {
                JsonObjectAppendString(infoObject, LABEL_OS_TYPE, ScalarRvalValue(var->rval));
            }
            else if (strcmp(var->lval, "flavour") == 0)
            {
                JsonObjectAppendString(infoObject, LABEL_FLAVOUR, ScalarRvalValue(var->rval));
            }
            else if (strcmp(var->lval, "release") == 0)
            {
                JsonObjectAppendString(infoObject, LABEL_RELEASE, ScalarRvalValue(var->rval));
            }
            else if (strcmp(var->lval, "last_policy_update") == 0)
            {
                JsonObjectAppendString(infoObject, LABEL_LAST_POLICY_UPDATE, ScalarRvalValue(var->rval));
            }
        }
    }

    HostColour colour = HOST_COLOUR_BLUE;
    CFDB_GetHostColour(host->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour);
    JsonObjectAppendString(infoObject, LABEL_COLOUR, Nova_HostColourToString(colour));

    DeleteHubHost(host);
    DeleteHubQuery(result, DeleteHubVariable);

    RETURN_JSON(infoObject);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summary_meter)
{
    const int bufsize = 512 * 1024;
    char buffer[bufsize];

    buffer[0] = '\0';
    Nova2PHP_summary_meter(buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_meter)
{
    char *userName, *hostKey;
    int user_len, hk_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_meter(hostKey, buffer, sizeof(buffer));
    RETURN_STRING(buffer, 1);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

PHP_FUNCTION(cfpr_vitals_list)
{
    char *userName, *hostKey;
    int user_len, hk_len;
    char buffer[4096];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_list(hostKey, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_magnified)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[1024 * 1024];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_view_magnified(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_week)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[1024 * 1024];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_view_week(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_year)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[1024 * 1024];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_view_year(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_histogram)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[1024 * 1024];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_view_histogram(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_magnified)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[4096];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_analyse_magnified(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_week)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[4096];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_analyse_week(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_year)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[4096];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_analyse_year(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_histogram)
{
    char *userName, *hostKey, *vitalId;
    int user_len, hk_len, vi_len;
    char buffer[4096];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hostKey, &hk_len, &vitalId, &vi_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len && vi_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_vitals_analyse_histogram(hostKey, vitalId, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/
/* Returning tabular html data                                                */
/******************************************************************************/

PHP_FUNCTION(cfpr_report_software_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    zval *contextIncludes = NULL, *contextExcludes = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex, cfr_software, filter, &page, buffer,
                             sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    zval *contextIncludes = NULL, *contextExcludes = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex, cfr_patch_installed, filter, &page, buffer,
                             sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    zval *contextIncludes = NULL, *contextExcludes = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex, cfr_patch_avail, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_classes)
{
    char *userName, *hostkey, *name;
    char *fname, *fhostkey;
    zval *contextIncludes = NULL, *contextExcludes = NULL;

    int user_len, hk_len, n_len;
    zend_bool regex;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_classes_report(fhostkey, fname, (bool) regex, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_list_distinct_by_name_rx)
{
    char *userName = NULL,
         *class_rx = NULL;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len = 0,
        class_rx_len = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaall",
                              &userName, &user_len,
                              &class_rx, &class_rx_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    HubQuery *hqClasses = CFDB_QueryClassesDistinctSorted(&conn, class_rx, filter, &page);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    DATABASE_CLOSE(&conn);

    JsonElement *data = JsonArrayCreate(5000);

    for (Rlist *rp = hqClasses->records; rp != NULL; rp = rp->next)
    {
        HubClass *hubClass = rp->item;

        JsonArrayAppendString(data, hubClass->class);
    }
    DeleteHubQuery(hqClasses, DeleteHubClass);

    JsonElement *output = JsonObjectWrapper(data, page.totalResultCount);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_list_time_distinct_by_name_rx)
{
    char *userName = NULL,
         *class_rx = NULL;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len = 0,
        class_rx_len = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaall",
                              &userName, &user_len,
                              &class_rx, &class_rx_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    HubQuery *hqClasses = CFDB_QueryClassesDistinctSorted(&conn, class_rx, filter, &page);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    DATABASE_CLOSE(&conn);

    JsonElement *data = JsonArrayCreate(5000);

    for (Rlist *rp = hqClasses->records; rp != NULL; rp = rp->next)
    {
        HubClass *hubClass = rp->item;

        if (IsTimeClass(hubClass->class))
        {
            JsonArrayAppendString(data, hubClass->class);
        }
        else
        {
            page.totalResultCount--;
        }
    }
    DeleteHubQuery(hqClasses, DeleteHubClass);

    JsonElement *output = JsonObjectWrapper(data, page.totalResultCount);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_list_soft_distinct_by_name_rx)
{
    char *userName = NULL,
         *class_rx = NULL;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len = 0,
        class_rx_len = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaall",
                              &userName, &user_len,
                              &class_rx, &class_rx_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    HubQuery *hqClasses = CFDB_QueryClassesDistinctSorted(&conn, class_rx, filter, &page);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    DATABASE_CLOSE(&conn);

    JsonElement *data = JsonArrayCreate(5000);

    for (Rlist *rp = hqClasses->records; rp != NULL; rp = rp->next)
    {
        HubClass *hubClass = rp->item;

        if (IsSoftClass(hubClass->class))
        {
            JsonArrayAppendString(data, hubClass->class);
        }
        else
        {
            page.totalResultCount--;
        }
    }
    DeleteHubQuery(hqClasses, DeleteHubClass);

    JsonElement *output = JsonObjectWrapper(data, page.totalResultCount);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_cloud)
{
    char *classes;
    char *fclasses;
    int cl_len;
    const int bufsize = 512 * 1024;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &classes, &cl_len) == FAILURE)
    {
        RETURN_NULL();
    }

    fclasses = (cl_len == 0) ? NULL : classes;

    char **classArray = String2StringArray(fclasses, ',');

    buffer[0] = '\0';
    Nova2PHP_classes_summary(classArray, buffer, sizeof(buffer));

    FreeStringArray(classArray);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_vars)
{
    char *userName, *hostkey, *scope, *lval, *rval, *type;
    char *fhostkey, *fscope, *flval, *frval, *ftype;
    zval *contextIncludes = NULL, *contextExcludes = NULL;
    int user_len, hk_len, s_len, l_len, r_len, t_len;
    zend_bool regex;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &scope, &s_len,
                              &lval, &l_len,
                              &rval, &r_len,
                              &type, &t_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fscope = (s_len == 0) ? NULL : scope;
    flval = (l_len == 0) ? NULL : lval;
    frval = (r_len == 0) ? NULL : rval;
    ftype = (t_len == 0) ? NULL : type;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_vars_report(fhostkey, fscope, flval, frval, ftype, (bool) regex, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_compliance_summary_graph)
{
    const int bufsize = 512 * 1024;
    char buffer[bufsize];
    char *policy, *fpolicy;
    int pol_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &policy, &pol_len) == FAILURE)
    {
        RETURN_NULL();
    }

// "any" is keyword for all policies

    fpolicy = (pol_len == 0) ? "any" : policy;

    buffer[0] = '\0';
    Nova2PHP_ComplianceSummaryGraph(fpolicy, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_summary)
{
    char *userName, *hostkey, *version;
    char *fhostkey, *fversion;
    zval *contextIncludes = NULL, *contextExcludes = NULL;
    int user_len, hk_len, v_len;
    long k, nk, r, from;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssllllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &version, &v_len,
                              &from,
                              &k,
                              &nk,
                              &r,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fversion = (v_len == 0) ? NULL : version;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    Nova2PHP_compliance_report(fhostkey, fversion, (time_t) from, time(NULL), (int) k, (int) nk, (int) r, filter, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises)
{
    char *userName, *hostkey, *handle, *status;
    char *fhostkey, *fhandle, *fstatus;
    zval *context_includes = NULL, *context_excludes = NULL;
    int hk_len, h_len, s_len, user_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_compliance_promises(fhostkey, fhandle, fstatus, (bool) regex, filter, NULL, false, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastknown_compliance_promises)
{
    char *userName, *hostkey, *handle, *status, *hostcolour;
    char *fhostkey, *fhandle, *fstatus;
    zval *context_includes = NULL, *context_excludes = NULL;
    int hk_len, h_len, s_len, hc_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    int user_len;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;
    if (!NULL_OR_EMPTY(hostcolour))
    {
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, HostColourFromString(hostcolour));
    }

    Nova2PHP_compliance_promises(fhostkey, fhandle, fstatus, (bool) regex, filter, hostColourFilter, true, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_STRING(buffer, 1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_report_overall_summary)
{
    char *userName, *hostkey, *handle, *status, *classreg;
    char *fhostkey, *fhandle, *fstatus, *fclassreg;
    int user_len, hk_len, h_len, s_len, cr_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbs",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len, &status, &s_len, &regex, &classreg, &cr_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;
    fclassreg = (s_len == 0) ? NULL : classreg;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddClasses(filter, fclassreg, NULL);

    buffer[0] = '\0';    
    Nova2PHP_summary_report(fhostkey, fhandle, fstatus, (bool) regex, fclassreg, filter, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_compliance_for_promises)
{
    char *userName, *hostkey, *handle, *status;
    char *fhostkey, *fhandle, *fstatus;
    int user_len, hk_len, h_len, s_len;
    zend_bool regex;
    zval *context_includes = NULL, *context_excludes = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaa",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *out = Nova2PHP_promise_compliance_summary(fhostkey, fhandle, fstatus, (bool) regex, filter);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_compliance_for_bundles)
{
    char *userName, *hostkey, *bundle;
    char *fhostkey, *fbundle;
    int user_len, hk_len, b_len;
    zend_bool regex;
    zval *context_includes = NULL, *context_excludes = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaa",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &bundle, &b_len,
                              &regex,
                              &context_includes,
                              &context_excludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fbundle = (b_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *out = Nova2PHP_bundle_compliance_summary(fhostkey, fbundle, (bool) regex, filter);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(out);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_lastseen)
{
    char *userName, *hostkey, *host, *address, *hash;
    char *fhostkey, *fhost, *faddress, *fhash;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, h_len, a_len, h2_len;
    char buffer[CF_WEBBUFFER];
    long ago;
    time_t tago;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;
    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssslbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hash, &h2_len,
                              &host, &h_len,
                              &address, &a_len,
                              &ago,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    tago = (time_t) ago;

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhash = (h2_len == 0) ? NULL : hash;
    fhost = (h_len == 0) ? NULL : host;
    faddress = (a_len == 0) ? NULL : address;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_lastseen_report(fhostkey, fhash, fhost, faddress, tago, (bool) regex, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance)
{
    char *userName, *hostkey, *job;
    char *fhostkey, *fjob;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &job, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fjob = (j_len == 0) ? NULL : job;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_performance_report(fhostkey, fjob, (bool) regex, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid)
{
    char *userName, *hostkey, *file;
    char *fhostkey, *ffile;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (j_len == 0) ? NULL : file;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_setuid_report(fhostkey, ffile, (bool) regex, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges)
{
    char *userName, *hostkey, *file;
    char *fhostkey, *ffile;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, f_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &f_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (f_len == 0) ? NULL : file;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_filechanges_report(fhostkey, ffile, (bool) regex, from, to, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs)
{
    char *userName, *hostkey, *file, *diff;
    char *fhostkey, *ffile, *fdiff;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, f_len, d_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &f_len,
                              &diff, &d_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (f_len == 0) ? NULL : file;
    fdiff = (d_len == 0) ? NULL : diff;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_filediffs_report(fhostkey, ffile, fdiff, (bool) regex, from, to, filter, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_details)
{
    char *userName, *handle;
    int user_len, handle_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &handle, &handle_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && handle_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseBody(filter, handle, NULL);

    buffer[0] = '\0';
    Nova2PHP_promise_details(filter, buffer, sizeof(buffer));

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_bundlesseen)
{
    char *userName, *hostkey, *bundle;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &bundle, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (j_len == 0) ? NULL : bundle;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_bundle_report(fhostkey, fbundle, (bool) regex, filter, NULL, false, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastknown_bundlesseen)
{
    char *userName, *hostkey, *bundle, *hostcolour;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len, hc_len;
    char buffer[CF_WEBBUFFER];
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &bundle, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (j_len == 0) ? NULL : bundle;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;

    if (!NULL_OR_EMPTY(hostcolour))
    {
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, HostColourFromString(hostcolour));
    }
    Nova2PHP_bundle_report(fhostkey, fbundle, (bool) regex, filter, hostColourFilter, true, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_STRING(buffer, 1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_report_value)
{
    char *userName, *hostkey, *day, *month, *year;
    char *fhostkey, *fmonth, *fday, *fyear;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, d_len, m_len, y_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &day, &d_len,
                              &month, &m_len,
                              &year, &y_len,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fday = (d_len == 0) ? NULL : day;
    fmonth = (m_len == 0) ? NULL : month;
    fyear = (y_len == 0) ? NULL : year;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_value_report(fhostkey, fday, fmonth, fyear, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_value_graph)
{
    char buffer[10000];

    if (ZEND_NUM_ARGS() != 0)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_get_value_graph(NULL, NULL, NULL, NULL, NULL, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_notkept)
{
    char *userName, *hostkey, *handle, *cause_rx;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, h_len, c_len;
    char buffer[CF_WEBBUFFER];
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);    

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_promiselog(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_NOTKEPT, from, to, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_repaired)
{
    char *userName, *hostkey, *handle, *cause_rx;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, h_len, c_len;
    char buffer[CF_WEBBUFFER];
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    Nova2PHP_promiselog(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_REPAIRED, from, to, filter, &page, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept)
{
    char *userName, *hostkey, *handle, *cause_rx;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len, hk_len, h_len, c_len;

    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &includes,
                              &excludes,
                              &sortColumnName, &sc_len,
                              &sortDescending,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    JsonElement *output = Nova2PHP_promiselog_summary(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_NOTKEPT, from, to, filter, &page);
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_repaired)
{
    char *userName, *hostkey, *handle, *cause_rx;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len, hk_len, h_len, c_len;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaasbll",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &includes,
                              &excludes,
                              &sortColumnName, &sc_len,
                              &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    JsonElement *output = Nova2PHP_promiselog_summary(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_REPAIRED, from, to, filter, &page);
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(output);
}

/******************************************************************************/
/* Returning hosts                                                            */
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_software_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    int user_len, hk_len, n_len, v_len, a_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, (bool) regex,
                                       cfr_software, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_value)
{
    char *userName, *hostkey, *day, *month, *year;
    char *fhostkey, *fmonth, *fday, *fyear;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, d_len, m_len, y_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &day, &d_len,
                              &month, &m_len,
                              &year, &y_len,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fday = (d_len == 0) ? NULL : day;
    fmonth = (m_len == 0) ? NULL : month;
    fyear = (y_len == 0) ? NULL : year;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_value_hosts(fhostkey, fday, fmonth, fyear, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    int user_len, hk_len, n_len, v_len, a_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fname = (n_len == 0) ? NULL : name;
    char *fversion = (v_len == 0) ? NULL : version;
    char *farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, (bool) regex,
                                       cfr_patch_installed, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_avail)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    int user_len, hk_len, n_len, v_len, a_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, (bool) regex,
                                       cfr_patch_avail, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_classes)
{
    char *userName, *hostkey, *name;
    int user_len, hk_len, n_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &name, &n_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fname = (n_len == 0) ? NULL : name;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_classes_hosts(fhostkey, fname, (bool) regex, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_repaired)
//$ret = cfpr_hosts_with_repaired($hostkey,$name,$from,$to,$class_regex);
{
    char *userName, *hostkey, *handle, *cause_rx;
    int user_len, hk_len, h_len, c_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promiselog_hosts(fhostkey, fhandle, fcause_rx,
                                         PROMISE_LOG_STATE_REPAIRED, from, to,
                                         filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_notkept)
{
    char *userName, *hostkey, *handle, *cause_rx;
    int user_len, hk_len, h_len, c_len;
    zval *context_includes = NULL, *context_excludes = NULL;

    time_t from = 0, to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promiselog_hosts(fhostkey, fhandle, fcause_rx,
                                         PROMISE_LOG_STATE_NOTKEPT, from, to,
                                         filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_vars)
{
    char *userName, *hostkey, *scope, *lval, *rval, *type;
    char *fhostkey, *fscope, *flval, *frval, *ftype;
    int user_len, hk_len, s_len, l_len, r_len, t_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &scope, &s_len,
                              &lval, &l_len,
                              &rval, &r_len,
                              &type, &t_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fscope = (s_len == 0) ? NULL : scope;
    flval = (l_len == 0) ? NULL : lval;
    frval = (r_len == 0) ? NULL : rval;
    ftype = (t_len == 0) ? NULL : type;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_vars_hosts(fhostkey, fscope, flval, frval,
                                   ftype, (bool) regex, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_summary)
{
    char *userName, *hostkey, *version;
    int user_len, hk_len, v_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    long k, nk, r, t;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssllllaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &version, &v_len,
                              &t,
                              &k, &nk, &r,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fversion = (v_len == 0) ? NULL : version;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_compliance_hosts(fhostkey, fversion, (int) t, time(NULL),
                                         (int) k, (int) nk, (int) r, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_promises)
{
    char *userName, *hostkey, *handle, *status;
    char *fhostkey, *fhandle, *fstatus;
    int user_len, hk_len, h_len, s_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promise_hosts(fhostkey, fhandle, fstatus, (bool) regex,
                                      filter, NULL, false, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_hosts_with_lastknown_compliance_promises)
{
    char *userName, *hostkey, *handle, *status, *hostcolour;
    char *fhostkey, *fhandle, *fstatus;
    int user_len, hk_len, h_len, s_len, hc_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;
    if(!NULL_OR_EMPTY(hostcolour))
    {
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, HostColourFromString(hostcolour));
    }

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promise_hosts(fhostkey, fhandle, fstatus, (bool) regex,
                                      filter, hostColourFilter, true, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_lastseen)
{
    char *userName, *hostkey, *host, *address, *hash;
    char *fhostkey, *fhost, *faddress, *fhash;
    int user_len, hk_len, h_len, a_len, h2_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    long ago;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssslbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hash, &h2_len,
                              &host, &h_len,
                              &address, &a_len,
                              &ago,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhash = (h2_len == 0) ? NULL : hash;
    fhost = (h_len == 0) ? NULL : host;
    faddress = (a_len == 0) ? NULL : address;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_lastseen_hosts(fhostkey, fhash, fhost, faddress, ago,
                                       (bool) regex, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_performance)
{
    char *userName, *hostkey, *job;
    char *fhostkey, *fjob;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &job, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fjob = (j_len == 0) ? NULL : job;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_performance_hosts(fhostkey, fjob, (bool) regex, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_setuid)
{
    char *userName, *hostkey, *file;
    char *fhostkey, *ffile;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), & (page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_setuid_hosts(fhostkey, ffile, (bool) regex, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filechanges)
{
    char *userName, *hostkey, *file;
    char *fhostkey, *ffile;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbllaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &j_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_filechanges_hosts(fhostkey, ffile, (bool) regex, from,
                                          to, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filediffs)
{
    char *userName, *hostkey, *file, *diff;
    char *fhostkey, *ffile;
    int user_len, hk_len, j_len, d_len;
    zval *context_includes = NULL, *context_excludes = NULL;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbllaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &file, &j_len,
                              &diff, &d_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_filediffs_hosts(fhostkey, ffile, diff, (bool) regex,
                                        from, to, filter, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_bundlesseen)
{
    char *userName, *hostkey, *bundle;
    char *fhostkey, *fbundle;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &bundle, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fbundle = (j_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_bundle_hosts(fhostkey, fbundle, (bool) regex, filter,
                                     NULL, false, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_lastknown_bundlesseen)
{
    char *userName, *hostkey, *bundle, *hostcolour;
    char *fhostkey, *fbundle;
    zval *context_includes = NULL, *context_excludes = NULL;
    int user_len, hk_len, j_len, hc_len;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaall",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &bundle, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fbundle = (j_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;

    if (!NULL_OR_EMPTY(hostcolour))
    {
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                               HostColourFromString(hostcolour));
    }

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_bundle_hosts(fhostkey, fbundle, (bool) regex, filter,
                                     hostColourFilter, true, &page);

    if (!json_out)
    {
        json_out = JsonObjectCreate(0);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_JSON(json_out);
}

/******************************************************************************/
/* Topic Map                                                                  */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_pid_for_topic)
{
    char *username, *type, *topic, typed_topic[1024];
    int user_len, o_len, y_len;
    long pid;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &user_len,
                              &type, &y_len,
                              &topic, &o_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    if (y_len > 0)
    {
        snprintf(typed_topic, 1023, "%s::%s", type, topic);
        pid = (long) Nova_GetTopicIdForTopic(typed_topic);
    }
    else
    {
        pid = (long) Nova_GetTopicIdForTopic(topic);
    }

    RETURN_LONG(pid);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_search_topics)
{
    char *username, *search;
    char *fsearch;
    int user_len, s_len;
    zend_bool regex;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssb",
                              &username, &user_len,
                              &search, &s_len,
                              &regex) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    fsearch = (s_len == 0) ? NULL : search;

    JsonElement *out = NULL;
    out = Nova2PHP_search_topics(fsearch, (bool) regex);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic)
{
    long id;
    char *username;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sl",
                              &username, &user_len,
                              &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic((int) id);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_leads)
{
    long id;
    char *username;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sl",
                              &username, &user_len,
                              &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_leads((int) id);

    RETURN_JSON(out);
}


/******************************************************************************/

PHP_FUNCTION(cfpr_show_all_context_leads)
{
    char *username;
    char *unqualified_topic;
    int user_len, topic_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &user_len,
                              &unqualified_topic, &topic_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    ARGUMENT_CHECK_CONTENTS(topic_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_show_all_context_leads(unqualified_topic);

    RETURN_JSON(out);
}


/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_hits)
{
    long id;
    char *username;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sl",
                              &username, &user_len,
                              &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_hits((int) id);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_category)
{
    long id;
    char *username;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sl",
                              &username, &user_len,
                              &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_category((int) id);

    RETURN_JSON(out);
}

/******************************************************************************/
/* GRAPHS                                                                     */
/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_list_all)
{
    char *userName;
    int user_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    zval *includes, *excludes;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "saall",
                              &userName, &user_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    buffer[0] = '\0';
    Nova2PHP_host_compliance_list_all(&conn, filter, &page, buffer, sizeof(buffer));

    DATABASE_CLOSE(&conn);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_count)
{
    char *username = NULL, *colour = NULL;
    int user_len, colour_len;
    zval *includes, *excludes;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaa",
                              &username, &user_len, &colour, &colour_len, &includes, &excludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    int count = 0;

    HostColourFilter *host_colour_filter = NULL;
    if (!NULL_OR_EMPTY(colour))
    {
        host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE, HostColourFromString(colour));
    }

    count = CFDB_CountHosts(&conn, filter, host_colour_filter);

    DATABASE_CLOSE(&conn);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(host_colour_filter);

    RETURN_LONG(count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_timeseries)
{
    char *username = NULL;
    zval *contextIncludes = NULL,
         *contextExcludes = NULL;
    long username_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saa",
                              &username, &username_len,
                              &contextIncludes,
                              &contextExcludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);

    static const time_t horizon = SECONDS_PER_HOUR * 6;
    static const time_t resolution = CF_MEASURE_INTERVAL;
    const time_t from = MeasurementSlotStart(time(NULL) - horizon);
    const time_t to = from + horizon;

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryTotalCompliance(&conn, NULL, NULL, from, to, -1, -1, -1, false, filter);

        DATABASE_CLOSE(&conn);
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }

    const size_t num_slots = horizon / resolution;
    assert(num_slots > 0);

    size_t green[num_slots]; memset(green, 0, sizeof(size_t) * num_slots);
    size_t red[num_slots]; memset(red, 0, sizeof(size_t) * num_slots);
    size_t yellow[num_slots]; memset(yellow, 0, sizeof(size_t) * num_slots);

    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubTotalCompliance *record = (const HubTotalCompliance *)rp->item;
        size_t slot = (record->t - from) / resolution;

        if (slot >= num_slots)
        {
            // TODO: log shit
            continue;
        }

        int score = HostComplianceScore(record->kept, record->repaired);
        switch (HostColourFromScoreForConnectedHost(score))
        {
        case HOST_COLOUR_GREEN:
            green[slot]++;
            break;

        case HOST_COLOUR_RED:
            red[slot]++;
            break;

        case HOST_COLOUR_YELLOW:
            yellow[slot]++;
            break;

        default:
            // TODO: log shit
            continue;
        }
    }

    DeleteHubQuery(result, DeleteHubTotalCompliance);

    JsonElement *data = JsonArrayCreate(num_slots);
    for (size_t slot = 0; slot < num_slots; slot++)
    {
        JsonElement *entry = JsonObjectCreate(10);

        JsonObjectAppendInteger(entry, LABEL_POSITION, slot);
        JsonObjectAppendInteger(entry, LABEL_TIMESTAMP, from + (resolution * slot));
        JsonObjectAppendInteger(entry, LABEL_GREEN, green[slot]);
        JsonObjectAppendInteger(entry, LABEL_RED, red[slot]);
        JsonObjectAppendInteger(entry, LABEL_YELLOW, yellow[slot]);

        JsonArrayAppendObject(data, entry);
    }

    JsonElement *output = JsonObjectCreate(4);
    JsonObjectAppendInteger(output, LABEL_FROM, (int)from);
    JsonObjectAppendInteger(output, LABEL_RESOLUTION, resolution);
    JsonObjectAppendInteger(output, LABEL_COUNT, num_slots);
    JsonObjectAppendArray(output, LABEL_DATA, data);

    RETURN_JSON(output);
}

PHP_FUNCTION(cfpr_host_compliance_timeseries_shifts)
{
    char *username = NULL;
    zval *contextIncludes = NULL,
         *contextExcludes = NULL;
    long username_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saa",
                              &username, &username_len,
                              &contextIncludes,
                              &contextExcludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);

    Sequence *records = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        records = CFDB_QueryHostComplianceShifts(&conn, filter);

        DATABASE_CLOSE(&conn);
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }

    size_t green[SHIFTS_PER_WEEK] = { 0 };
    size_t red[SHIFTS_PER_WEEK] = { 0 };
    size_t yellow[SHIFTS_PER_WEEK] = { 0 };

    for (size_t host_index = 0; host_index < records->length; host_index++)
    {
        const HubHostComplianceShifts *record = (const HubHostComplianceShifts *)records->data[host_index];

        for (size_t shift_index = 0; shift_index < SHIFTS_PER_WEEK; shift_index++)
        {
            if (record->num_samples[shift_index] <= 0)
            {
                continue;
            }

            int score = HostComplianceScore(record->kept[shift_index], record->repaired[shift_index]);
            switch (HostColourFromScoreForConnectedHost(score))
            {
            case HOST_COLOUR_GREEN:
                green[shift_index]++;
                break;

            case HOST_COLOUR_RED:
                red[shift_index]++;
                break;

            case HOST_COLOUR_YELLOW:
                yellow[shift_index]++;
                break;

            default:
                // TODO: log shit
                continue;
            }
        }
    }

    SequenceDestroy(records);

    time_t to = GetShiftSlotStart(time(NULL));
    time_t from = to - (SHIFTS_PER_WEEK * SECONDS_PER_SHIFT);
    int most_recent_shift = GetShiftSlot(to);

    JsonElement *data = JsonArrayCreate(SHIFTS_PER_WEEK);
    for (int pos = 0; pos < SHIFTS_PER_WEEK; pos++)
    {
        JsonElement *entry = JsonObjectCreate(10);

        int shift_index = UnsignedModulus(most_recent_shift + pos + 1, SHIFTS_PER_WEEK);

        JsonObjectAppendInteger(entry, LABEL_POSITION, pos);
        JsonObjectAppendInteger(entry, LABEL_GREEN, green[shift_index]);
        JsonObjectAppendInteger(entry, LABEL_RED, red[shift_index]);
        JsonObjectAppendInteger(entry, LABEL_YELLOW, yellow[shift_index]);

        JsonArrayAppendObject(data, entry);
    }

    JsonElement *output = JsonObjectCreate(4);
    JsonObjectAppendInteger(output, LABEL_FROM, (int)from);
    JsonObjectAppendInteger(output, LABEL_RESOLUTION, SECONDS_PER_SHIFT);
    JsonObjectAppendInteger(output, LABEL_COUNT, SHIFTS_PER_WEEK);
    JsonObjectAppendArray(output, LABEL_DATA, data);

    RETURN_JSON(output);
}



/******************************************************************************/

PHP_FUNCTION(cfpr_select_reports)
{
    const int bufsize = 1000000;
    char buffer[bufsize];
    char *report_id;
    int r_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &report_id, &r_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_select_reports(report_id, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_list)
{
    char *userName;
    zval *includes = NULL,
         *excludes = NULL;
    int user_len;
    char buffer[CF_WEBBUFFER];
    char *colour = NULL;
    int colour_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaall",
                              &userName, &user_len,
                              &colour, &colour_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    buffer[0] = '\0';

    if (!NULL_OR_EMPTY(colour))
    {
        EnterpriseDB conn;

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

        DATABASE_OPEN(&conn);

        Nova2PHP_host_compliance_list(&conn, colour, filter, &page, buffer, sizeof(buffer));

        DATABASE_CLOSE(&conn);
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }


    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_list_by_ip_rx)
{
    char buffer[CF_WEBBUFFER];
    char *userName, *ipRegex, *fipRegex;
    int user_len, ip_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &ipRegex, &ip_len, &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fipRegex = (ip_len == 0) ? NULL : ipRegex;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    buffer[0] = '\0';
    Nova2PHP_show_hosts(NULL, fipRegex, filter, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_list_by_name_rx)
{
    char buffer[CF_WEBBUFFER];
    char *userName, *hostNameRegex, *fhostNameRegex;
    int user_len, hname_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &hostNameRegex, &hname_len, &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostNameRegex = (hname_len == 0) ? NULL : hostNameRegex;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    buffer[0] = '\0';
    Nova2PHP_show_hosts(fhostNameRegex, NULL, filter, &page, buffer, sizeof(buffer));

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlastupdate)
{
    char *userName, *hostKey;
    int user_len, hk_len;
    char buffer[100];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_getlastupdate(hostKey, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_colour)
{
    char *userName, *hostKey;
    int user_len, hk_len;
    char buffer[128];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Nova2PHP_get_host_colour(hostKey, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_handle_rx)
{
    char *userName, *handle;
    int user_len, h_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &handle, &h_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fhandle = (h_len == 0) ? NULL : handle;

    buffer[0] = '\0';

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseBodyRx(filter, fhandle, NULL);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promiser)
{
    char *userName, *promiser;
    int user_len, pr_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &promiser, &pr_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fpromiser = (pr_len == 0) ? NULL : promiser;

    buffer[0] = '\0';

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseBody(filter, NULL, fpromiser);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promiser_rx)
{
    char *userName, *promiser;
    int user_len, pr_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &promiser, &pr_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fpromiser = (pr_len == 0) ? NULL : promiser;

    buffer[0] = '\0';

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseBodyRx(filter, NULL, fpromiser);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promise_type)
{
    char *userName, *promiseType;
    int user_len, pt_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll",
                              &userName, &user_len,
                              &promiseType, &pt_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && pt_len);

    buffer[0] = '\0';

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseType(filter, promiseType);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_bundle)
{
    char *userName, *bundleType, *bundleName;
    int user_len, btype_len, bname_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssll",
                              &userName, &user_len,
                              &bundleType, &btype_len,
                              &bundleName, &bname_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddBundleType(filter, bundleType);
    PromiseFilterAddBundles(filter, bundleName, NULL);

    buffer[0] = '\0';
    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_bundle_rx)
{
    char *userName, *bundleNameRx, *bundleTypeRx;
    int user_len, b_len, btype_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssll",
                              &userName , &user_len,
                              &bundleTypeRx, &btype_len,
                              &bundleNameRx, &b_len,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fBundleNameRx = (b_len == 0) ? NULL : bundleNameRx;
    char *fBundleTypeRx = (btype_len == 0) ? NULL : bundleTypeRx;

    buffer[0] = '\0';

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddBundleTypeRx(filter, fBundleTypeRx);
    PromiseFilterAddBundlesRx(filter, fBundleNameRx, NULL);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_by_promise_handle)
{
    char *userName;
    char *promiseHandle;
    int user_len, ph_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &promiseHandle, &ph_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && ph_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddPromiseBody(filter, promiseHandle, NULL);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    DATABASE_CLOSE(&conn);

    HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);

    char *returnBuf = NULL;

    if (bundle && bundle->bundleName)
    {
        returnBuf = estrdup(bundle->bundleName);
    }

    DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

    if (returnBuf)
    {
        RETURN_STRING(returnBuf, 0);
    }
    else
    {
        RETURN_NULL();
    }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_classes_used)
{
    char *userName, *bundleName, *bundleType;
    int user_len, btype_len, bname_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &bundleType, &btype_len, &bundleName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddBundleType(filter, bundleType);
    PromiseFilterAddBundles(filter, bundleName, NULL);

    buffer[0] = '\0';
    Nova2PHP_bundle_classes_used(filter, buffer, sizeof(buffer));

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_arguments)
{
    char *userName;
    char *bundleName;
    char *bundleType;
    int user_len, bname_len, btype_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &bundleType, &btype_len, &bundleName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddBundleType(filter, bundleType);
    PromiseFilterAddBundles(filter, bundleName, NULL);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    DATABASE_CLOSE(&conn);

    JsonElement *output = JsonArrayCreate(100);

    HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);

    if (bundle)
    {
        for (Rlist *rp = bundle->bundleArgs; rp != NULL; rp = rp->next)
        {
            JsonArrayAppendString(output, ScalarValue(rp));
        }
    }

    DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_list_all)
{
    char *userName;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &userName, &user_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hqBundles = CFDB_QueryPromiseBundles(&conn, filter);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    DATABASE_CLOSE(&conn);

    HubQuerySortPromiseBundles(hqBundles);

    JsonElement *output = JsonArrayCreate(100);

    for (Rlist *rp = hqBundles->records; rp != NULL; rp = rp->next)
    {
        HubPromiseBundle *bundle = rp->item;

        JsonElement *bundleArray = JsonArrayCreate(2);

        JsonArrayAppendString(bundleArray, bundle->bundleType);
        JsonArrayAppendString(bundleArray, bundle->bundleName);

        JsonArrayAppendArray(output, bundleArray);
    }

    DeleteHubQuery(hqBundles, DeleteHubPromiseBundle);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_agent_goals)
{
    char buffer[CF_WEBBUFFER];
    char *userName;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &userName, &user_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseFilterAddBundleType(filter, "agent");

    buffer[0] = '\0';
    Nova2PHP_bundle_agent_goals(filter, buffer, sizeof(buffer));

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_bundle_type)
// FIXME: this function inherently wrong - a bundle is not unique by its name -
//        e.g. there may be a "bundle monitor test" and a "bundle agent test"
{
    char *bundleName;
    int bname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &bundleName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(bname_len);

    PromiseFilter *filter = NewPromiseFilter();

    PromiseFilterAddBundles(filter, bundleName, NULL);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);

    DeletePromiseFilter(filter);
    DATABASE_CLOSE(&conn);

    HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);

    char *returnBuf = NULL;

    if (bundle && bundle->bundleType)
    {
        returnBuf = estrdup(bundle->bundleType);
    }

    DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

    if (returnBuf)
    {
        RETURN_STRING(returnBuf, 0);
    }
    else
    {
        RETURN_NULL();
    }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_list_by_bundle_usage)
{
    char *userName, *bundleName;
    int user_len, bname_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &bundleName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && bname_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    buffer[0] = '\0';
    Nova2PHP_bundle_list_by_bundle_usage(filter, bundleName, buffer, sizeof(buffer));

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_business_goals)
{
    const int bufsize = 1000000;
    char buffer[bufsize];

    buffer[0] = '\0';

    Nova2PHP_list_all_goals(buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_class_frequency)
{
    char *userName, *hkey, *fhkey, *pattern, *fpattern;
    int user_len, p_len, h_len, count;
    char buffer[1000000];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hkey, &h_len, &pattern, &p_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fpattern = (p_len == 0) ? NULL : pattern;
    fhkey = (h_len == 0) ? NULL : hkey;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    count = Nova2PHP_countclasses(fhkey, fpattern, 1, filter, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_LONG((long) count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_class_frequency)
{
    char *userName, *hkey, *fhkey, *pattern, *fpattern;
    int user_len, p_len, h_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len, &hkey, &h_len, &pattern, &p_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    buffer[0] = '\0';

    fpattern = (p_len == 0) ? NULL : pattern;
    fhkey = (h_len == 0) ? NULL : hkey;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    Nova2PHP_countclasses(fhkey, fpattern, 1, filter, buffer, sizeof(buffer));
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_body_details)
{
    char *bodyType, *bodyName;
    int btype_len, bname_len;
    char buffer[1000000];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &bodyType, &btype_len, &bodyName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(btype_len && bname_len);

    buffer[0] = '\0';
    Nova2PHP_GetPromiseBody(bodyName, bodyType, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_body_list)
{
    char *bodyType, *bodyName;
    int bname_len, btype_len;
    char buffer[1000000];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &bodyType, &btype_len, &bodyName, &bname_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';

    Nova2PHP_body_list(bodyName, bodyType, buffer, sizeof(buffer));
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_network_speed)
{
    char *userName, *hostKey;
    int user_len, hostkey_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &userName, &user_len,
                              &hostKey, &hostkey_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hostkey_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = Nova2PHP_network_speed(hostKey);

    RETURN_JSON(out);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_description)
{
    char *reportName;
    int reportNameSz;
    const int bufSize = 5 * 1024;
    char buf[bufSize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &reportName, &reportNameSz) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buf[0] = '\0';
    Nova2PHP_report_description(reportName, buf, sizeof(buf));

    RETURN_STRING(buf, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_validate_policy)
{
    char *file;
    int f_len, ret;
    const int bufsize = 100000;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &file, &f_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    ret = Nova2PHP_validate_policy(file, buffer, bufsize);
    switch (ret)
    {
    case 0:
        RETURN_STRING("SUCCESS", 1);
        break;
    case -1:                   // error
    case 1:
    default:
        break;
    }
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_delete_host)
{
    char *userName, *hostKey;
    int user_len, hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hk_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    Nova2PHP_delete_host(hostKey);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environment_list)
{
    char *userName;
    int user_len;
    EnvironmentsList *el, *i;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &userName, &user_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    if (!Nova2PHP_environment_list(&el, filter))
    {
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        zend_throw_exception(cfmod_exception_generic, "Unable to query list of environments", 0 TSRMLS_CC);
        RETURN_NULL();
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    array_init(return_value);

    for (i = el; i != NULL; i = i->next)
    {
        add_next_index_string(return_value, i->name, 1);
    }

    FreeEnvironmentsList(el);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_list_by_environment)
{
    HostsList *hl, *i;
    char *userName, *envName;
    int user_len, env_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &envName, &env_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && env_len);

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    if (!Nova2PHP_host_list_by_environment(&hl, envName, filter))
    {
        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
        zend_throw_exception(cfmod_exception_generic, "Unable to query environment", 0 TSRMLS_CC);
        RETURN_NULL();
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    array_init(return_value);

    for (i = hl; i != NULL; i = i->next)
    {
        add_next_index_string(return_value, i->keyhash, 1);
    }

    FreeHostsList(hl);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environment_by_hostkey)
{
    char *userName, *hostKey;
    int user_len, hostkey_len;
    char *environment;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &user_len, &hostKey, &hostkey_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && hostkey_len);

    cfapi_errid erridAccess = CFDB_HasHostAccessFromUserRBAC(userName, hostKey);

    if (erridAccess != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, (char *) GetErrorDescription(erridAccess), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    environment = Nova2PHP_environment_by_hostkey(hostKey);

    if (!environment)
    {
        RETURN_NULL();
    }
    else
    {
        char *envcopy = estrdup(environment);

        free(environment);
        RETURN_STRING(envcopy, 0);
    }
}

/******************************************************************************/
/* COPERNICUS KNOWLEDGE MAP                                                   */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_knowledge_view)
{
    char *username, *view;
    int user_len, v_len;
    long pid;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sls",
                              &username, &user_len,
                              &pid,
                              &view, &v_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = NULL;
    out = Nova2PHP_get_knowledge_view(pid, view);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_add_note)
{
    char *user, *note, *nid, *hostkey;
    char *fuser, *fnote;
    int u_len, n_len, nid_len, hk_len;
    time_t datetime;
    const int bufsize = 1000;
    char returnval[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssls",
                              &nid, &nid_len,
                              &hostkey, &hk_len,
                              &user, &u_len,
                              &datetime,
                              &note, &n_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    fuser = (u_len == 0) ? NULL : user;
    fnote = (n_len == 0) ? NULL : note;

    returnval[0] = '\0';

    Nova2PHP_add_note(nid, hostkey, fuser, datetime, fnote, returnval, sizeof(returnval));

    RETURN_STRING(returnval, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_query_note)
{
    char *hostkey = { 0 }, *user, *nid, *filter_user;
    char *fhostkey, *fuser, *fnid;
    int hk_len, u_len, nid_len, fus_len;
    const int bufsize = 1000000;
    long from, to;
    PageInfo page = { 0 };

    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllll",
                              &hostkey, &hk_len,
                              &nid, &nid_len,
                              &user, &u_len,
                              &filter_user, &fus_len,
                              &from,
                              &to,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fuser = (fus_len == 0) ? NULL : filter_user;
    fnid = (nid_len == 0) ? NULL : nid;

    buffer[0] = '\0';
    Nova2PHP_get_notes(fhostkey, fnid, user, fuser, from, to, &page, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_host_noteid)
{
    char s1[4096];
    char *hostkey;
    int hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hostkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    s1[0] = '\0';
    Nova2PHP_get_host_noteid(hostkey, s1, sizeof(s1));

    RETURN_STRING(s1, 1);
}

/******************************************************************************/

// TODO: get policiy names, get compliance by policy name

PHP_FUNCTION(cfpr_replica_status)
/* MongoDB hub replication */
{
    const int bufsize = 4096;
    char buf[bufsize];

    buf[0] = '\0';
    Nova2PHP_replica_status(buf, sizeof(buf));
    RETURN_STRING(buf, 1);
}

/******************************************************************************/
/* Library                                                                    */
/******************************************************************************/

PHP_FUNCTION(cfpr_list_documents)
{
    const int bufsize = 1000000;
    char buffer[bufsize];
    char *filePath, *fFilePath;
    int fpLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &filePath, &fpLen) == FAILURE)
    {
        RETURN_NULL();
    }

    fFilePath = (fpLen == 0) ? NULL : filePath;

    buffer[0] = '\0';
    Nova2PHP_GetLibraryDocuments(fFilePath, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/
/* RBAC                                                                       */
/******************************************************************************/

PHP_FUNCTION(cfpr_user_authenticate)
{
    char *username = NULL, *password = NULL;
    int username_len = -1, password_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &username, &username_len, &password, &password_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    switch (CFDB_UserAuthenticate(username, password, password_len, NULL))
    {
    case ERRID_DBCONNECT:
        zend_throw_exception(cfmod_exception_db, "Error opening database", 0 TSRMLS_CC);
        RETURN_NULL();

    case ERRID_DBCLOSE:
        zend_throw_exception(cfmod_exception_db, "Error closing database", 0 TSRMLS_CC);
        RETURN_NULL();

    case ERRID_SUCCESS:
        RETURN_BOOL(true);

    default:
        RETURN_BOOL(false);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_create)
{
    char *creatingUserName, *roleName, *description, *includeClassRx, *excludeClassRx, *includeBundleRx,
        *excludeBundleRx;
    int creatingUserNameLen, roleNameLen, descLen, icrxLen, ecrxLen, ibrxLen, ebrxLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &creatingUserName, &creatingUserNameLen,
                              &roleName, &roleNameLen,
                              &description, &descLen,
                              &includeClassRx, &icrxLen,
                              &excludeClassRx, &ecrxLen,
                              &includeBundleRx, &ibrxLen, &excludeBundleRx, &ebrxLen) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(creatingUserNameLen && roleNameLen && descLen);

    char *fIncludeClassRx = (icrxLen == 0) ? NULL : includeClassRx;
    char *fExcludeClassRx = (ecrxLen == 0) ? NULL : excludeClassRx;
    char *fIncludeBundleRx = (ibrxLen == 0) ? NULL : includeBundleRx;
    char *fExcludeBundleRx = (ebrxLen == 0) ? NULL : excludeBundleRx;

    cfapi_errid errid = CFDB_CreateRole(creatingUserName, roleName, description,
                                        fIncludeClassRx, fExcludeClassRx, fIncludeBundleRx, fExcludeBundleRx);

    if (errid != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_generic, (char *) GetErrorDescription(errid), 0 TSRMLS_CC);
    }

    RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_delete)
{
    char *userName, *roleName;
    int userNameLen, roleNameLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &userNameLen, &roleName, &roleNameLen) ==
        FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(userNameLen && roleNameLen);

    cfapi_errid errid = CFDB_DeleteRole(userName, roleName, true);

    if (errid != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_generic, (char *) GetErrorDescription(errid), 0 TSRMLS_CC);
    }

    RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_update)
{
    char *updatingUserName, *roleName, *description, *includeClassRx, *excludeClassRx, *includeBundleRx,
        *excludeBundleRx;
    int updatingUserNameLen, roleNameLen, descLen, icrxLen, ecrxLen, ibrxLen, ebrxLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssss",
                              &updatingUserName, &updatingUserNameLen,
                              &roleName, &roleNameLen,
                              &description, &descLen,
                              &includeClassRx, &icrxLen,
                              &excludeClassRx, &ecrxLen,
                              &includeBundleRx, &ibrxLen, &excludeBundleRx, &ebrxLen) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(updatingUserNameLen && roleNameLen && descLen);

    char *fIncludeClassRx = (icrxLen == 0) ? NULL : includeClassRx;
    char *fExcludeClassRx = (ecrxLen == 0) ? NULL : excludeClassRx;
    char *fIncludeBundleRx = (ibrxLen == 0) ? NULL : includeBundleRx;
    char *fExcludeBundleRx = (ebrxLen == 0) ? NULL : excludeBundleRx;

    cfapi_errid errid = CFDB_UpdateRole(updatingUserName, roleName, description,
                                        fIncludeClassRx, fExcludeClassRx, fIncludeBundleRx, fExcludeBundleRx);

    if (errid != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_generic, (char *) GetErrorDescription(errid), 0 TSRMLS_CC);
    }

    RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_all)
{
    char *userName;
    int userNameLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &userName, &userNameLen) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(userNameLen);

    HubQuery *hq = CFDB_ListRoles(userName);

    if (hq->errid != ERRID_SUCCESS)
    {
        DeleteHubQuery(hq, DeleteHubRole);
        zend_throw_exception(cfmod_exception_generic, (char *) GetErrorDescription(hq->errid), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *roles = ParseRolesToJson(hq);

    DeleteHubQuery(hq, DeleteHubRole);

    RETURN_JSON(roles);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_by_name)
{

    char *userName, *roleName;
    int userNameLen, roleNameLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userName, &userNameLen, &roleName, &roleNameLen) ==
        FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(userNameLen && roleNameLen);

    HubQuery *hq = CFDB_GetRoleByNameAuth(userName, roleName);

    if (hq->errid != ERRID_SUCCESS)
    {
        DeleteHubQuery(hq, DeleteHubRole);
        zend_throw_exception(cfmod_exception_generic, (char *) GetErrorDescription(hq->errid), 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *roles = ParseRolesToJson(hq);

    DeleteHubQuery(hq, DeleteHubRole);

    RETURN_JSON(roles);
}

/******************************************************************************/

static JsonElement *ParseRolesToJson(HubQuery *hq)
{
#define LABEL_ROLE_NAME "name"
#define LABEL_ROLE_DESCRIPTION "description"
#define LABEL_ROLE_CLASSRX_INCLUDE "classrxinclude"
#define LABEL_ROLE_CLASSRX_EXCLUDE "classrxexclude"
#define LABEL_ROLE_BUNDLERX_INCLUDE "bundlerxinlcude"
#define LABEL_ROLE_BUNDLERX_EXCLUDE "bundlerxexclude"

    JsonElement *roles = JsonArrayCreate(100);

    for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubRole *record = (HubRole *) rp->item;
        JsonElement *role_entry = JsonObjectCreate(10);

        JsonObjectAppendString(role_entry, LABEL_ROLE_NAME, record->name);
        JsonObjectAppendString(role_entry, LABEL_ROLE_DESCRIPTION, NULLStringToEmpty(record->description));
        JsonObjectAppendString(role_entry, LABEL_ROLE_CLASSRX_INCLUDE, NULLStringToEmpty(record->classRxInclude));
        JsonObjectAppendString(role_entry, LABEL_ROLE_CLASSRX_EXCLUDE, NULLStringToEmpty(record->classRxExclude));
        JsonObjectAppendString(role_entry, LABEL_ROLE_BUNDLERX_INCLUDE, NULLStringToEmpty(record->bundleRxInclude));
        JsonObjectAppendString(role_entry, LABEL_ROLE_BUNDLERX_EXCLUDE, NULLStringToEmpty(record->bundleRxExclude));

        JsonArrayAppendObject(roles, role_entry);
    }

    return roles;
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_bluehost_threshold)
{
    int buffsize = 12;
    char buffer[buffsize];

    if (!Nova2PHP_get_bluehost_threshold(buffer, buffsize))
    {
        zend_throw_exception(cfmod_exception_db, "Error opening database", 0 TSRMLS_CC);
        RETURN_NULL();
    }

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/
/* Mission Tree-Control (Astrolabe)                                           */
/******************************************************************************/

PHP_FUNCTION(cfpr_astrolabe_host_list)
{
    char *username = NULL;
    zval *includes, *excludes;
    int len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "saa", &username, &len, &includes, &excludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        result = CFDB_QueryColour(&conn, HOST_RANK_METHOD_COMPLIANCE, filter);

        DATABASE_CLOSE(&conn);

        DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    }
    assert(result);

    JsonElement *output = JsonArrayCreate(1000);

    for (Rlist *rp = result->hosts; rp; rp = rp->next)
    {
        HubHost *host = (HubHost *) rp->item;

        JsonElement *entry = JsonObjectCreate(3);

        JsonObjectAppendString(entry, LABEL_HOST_KEY, host->keyhash);
        JsonObjectAppendString(entry, LABEL_HOST_NAME, NULLStringToEmpty(host->hostname));
        JsonObjectAppendString(entry, LABEL_COLOUR, Nova_HostColourToString(host->colour));

        JsonArrayAppendObject(output, entry);
    }

    DeleteHubQuery(result, NULL);

    RETURN_JSON(output)
}
