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
#include "item_lib.h"
#include "set.h"
#include "install.h"
#include "sort.h"

#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"

char **String2StringArray(char *str, char separator);
void FreeStringArray(char **strs);


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
static const char *LABEL_HOST_IDENTIFIER = "hostIdentifier";

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
    char *promise_context = NULL; int pc_len;
    int username_len = -1, hostkey_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &username_len,
                              &hostkey, &hostkey_len,
                              &promise_context, &pc_len) == FAILURE)
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

    HubQuery *result = CFDB_QueryVariables(&conn, hostkey, NULL, NULL, NULL, NULL,
                                           NULL, 0, time(NULL), NULL,
                                           PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

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

        if (var && strcmp(var->bundle, "sys") == 0)
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

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    HostColour colour = HOST_COLOUR_BLUE;
    CFDB_GetHostColour(host->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour, promise_context_mode);
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
    char *promise_context = NULL; int pc_len;
    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len,
                              &hostKey, &hk_len,
                              &promise_context, &pc_len) == FAILURE)
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

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    buffer[0] = '\0';
    Nova2PHP_meter(hostKey, buffer, sizeof(buffer), promise_context_mode);

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

static int CompareHubVitalsByLastValueAsc(const void *a, const void *b)
{
    const HubVital *a_hv = a;
    const HubVital *b_hv = b;
    const HubVitalPoint *a_point = HubVitalLastValue(a_hv);
    const HubVitalPoint *b_point = HubVitalLastValue(b_hv);
    if (!a_point || !b_point)
    {
        return 0;
    }

    if (a_point->value < b_point->value)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static int CompareHubVitalsByLastValueDesc(const void *a, const void *b)
{
    const HubVital *a_hv = a;
    const HubVital *b_hv = b;
    const HubVitalPoint *a_point = HubVitalLastValue(a_hv);
    const HubVitalPoint *b_point = HubVitalLastValue(b_hv);
    if (!a_point || !b_point)
    {
        return 0;
    }

    if (a_point->value > b_point->value)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool HubVitalIsInAllowedHostkeySet(void *_vital, void *_hostkeys)
{
    HubVital *vital = _vital;
    Set *hostkeys = _hostkeys;

    return SetContains(hostkeys, vital->hostkey);
}

PHP_FUNCTION(cfpr_hosts_sorted_by_last_vital_value)
{
    const char *username = NULL; int username_len = 0;
    const char *vital_id = NULL; int vital_id_len = 0;
    zval *context_includes = NULL,
         *context_excludes = NULL;
    bool desc = false;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaabll",
                              &username, &username_len,
                              &vital_id, &vital_id_len,
                              &context_includes,
                              &context_excludes,
                              &desc,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);
    ARGUMENT_CHECK_CONTENTS(vital_id_len);

    HostClassFilter *filter = NULL;
    {
        HubQuery *result = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(result, DeleteHostClassFilter);

        filter = (HostClassFilter *) HubQueryGetFirstRecord(result);
        HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);
        DeleteHubQuery(result, NULL);
    }
    assert(filter);

    EnterpriseDB conn[1];
    if (!CFDB_Open(conn))
    {
        RETURN_NULL();
    }

    HubQuery *result = CFDB_QueryVital(conn, NULL, vital_id, 0, time(NULL));

    Set *hostkeys = SetNew((MapHashFn)OatHash, (MapKeyEqualFn)StringSafeEqual, free);
    {
        Rlist *key_list = CFDB_QueryHostKeys(conn, NULL, NULL, 0, time(NULL), filter);
        for (const Rlist *rp = key_list; rp; rp = rp->next)
        {
            SetAdd(hostkeys, SafeStringDuplicate(ScalarValue(rp)));
        }
        DeleteRlist(key_list);
    }

    CFDB_Close(conn);
    DeleteHostClassFilter(filter);

    RlistFilter(&result->records, HubVitalIsInAllowedHostkeySet, hostkeys, (void (*)(void *))DeleteHubVital);
    SetDestroy(hostkeys);

    if (desc)
    {
        result->records = SortRlist(result->records, CompareHubVitalsByLastValueDesc);
    }
    else
    {
        result->records = SortRlist(result->records, CompareHubVitalsByLastValueAsc);
    }

    PageRecords(&result->records, &page, DeleteHubVital);

    JsonElement *payload = JsonArrayCreate(RlistLen(result->records));
    for (const Rlist *rp = result->records; rp; rp = rp->next)
    {
        const HubVital *vital = rp->item;

        JsonElement *entry = JsonObjectCreate(5);
        JsonObjectAppendString(entry, "hostkey", vital->hostkey);
        JsonObjectAppendString(entry, "vitalId", vital->id);
        {
            const HubVitalPoint *last_value = HubVitalLastValue(vital);
            if (last_value)
            {
                JsonObjectAppendInteger(entry, "lastUpdate", last_value->t);
                JsonObjectAppendReal(entry, "lastValue", last_value->value);
            }
        }
        JsonArrayAppendObject(payload, entry);
    }

    DeleteHubQuery(result, DeleteHubVital);

    RETURN_JSON(payload);
}

PHP_FUNCTION(cfpr_vitals_list)
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

    JsonElement *payload = Nova2PHP_vitals_list(hostKey);

    RETURN_JSON(payload);
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
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);    

    JsonElement *payload = NULL;

    if( report_file_info )
    {
        payload = WebExportSoftwareReport( fhostkey, fname, fversion, farch, (bool) regex,
                                                        cfr_software, filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex, cfr_software, filter, &page,
                                           promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportSoftwareReport( fhostkey, fname, fversion, farch, (bool) regex,
                                           cfr_patch_installed, filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex,
                                           cfr_patch_installed, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, n_len, v_len, a_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {        
        payload = WebExportSoftwareReport( fhostkey, fname, fversion, farch, (bool) regex,
                                           cfr_patch_avail, filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_software_report(fhostkey, fname, fversion, farch, (bool) regex,
                                           cfr_patch_avail, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_classes)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *name = NULL; int n_len;
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;

    zend_bool regex;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;
    time_t from, to;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbllll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &from, &to,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fname = (n_len == 0) ? NULL : name;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportClassesReport( fhostkey, fname, (bool) regex, filter, from, to,
                                          promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_classes_report(fhostkey, fname, (bool) regex, filter, &page, from,
                                          to, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
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

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &classes, &cl_len) == FAILURE)
    {
        RETURN_NULL();
    }

    fclasses = (cl_len == 0) ? NULL : classes;

    char **classArray = String2StringArray(fclasses, ',');

    JsonElement *payload = Nova2PHP_classes_summary(classArray);

    FreeStringArray(classArray);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_vars)
{
    char *userName, *hostkey, *scope, *lval, *rval, *type;
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, s_len, l_len, r_len, t_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &scope, &s_len,
                              &lval, &l_len,
                              &rval, &r_len,
                              &type, &t_len,
                              &regex,
                              &contextIncludes,
                              &contextExcludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fscope = (s_len == 0) ? NULL : scope;
    char *flval = (l_len == 0) ? NULL : lval;
    char *frval = (r_len == 0) ? NULL : rval;
    char *ftype = (t_len == 0) ? NULL : type;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportVariablesReport( fhostkey, fscope, flval, frval, ftype, (bool) regex,
                                            filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_vars_report(fhostkey, fscope, flval, frval, ftype, (bool) regex,
                             filter, &page, promise_context_mode);
    }
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *version = NULL; int v_len;
    char *promise_context = NULL; int pc_len;
    zval *contextIncludes = NULL,
            *contextExcludes = NULL,
            *report_file_info_array = NULL;
    long k, nk, r, from;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllllaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &version, &v_len,
                              &from,
                              &k,
                              &nk,
                              &r,
                              &contextIncludes,
                              &contextExcludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fversion = (v_len == 0) ? NULL : version;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, contextIncludes, contextExcludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportComplianceReport( fhostkey, fversion, (time_t) from, time(NULL),
                                          (int) k, (int) nk, (int) r, filter,
                                          promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_compliance_report(fhostkey, fversion, (time_t) from, time(NULL),
                                                      (int) k, (int) nk, (int) r, filter,
                                                      &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *status = NULL; int s_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fhandle = NULL;
    char *fstatus = NULL;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportPromiseComplianceReport( fhostkey, fhandle, fstatus, (bool) regex, filter, NULL,
                                                    false, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_compliance_promises(fhostkey, fhandle, fstatus, (bool) regex, filter, NULL,
                                               false, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastknown_compliance_promises)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *status = NULL; int s_len;
    char *hostcolour = NULL; int hc_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fhandle = NULL;
    char *fstatus = NULL;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    HostColourFilter *hostColourFilter = NULL;
    if (!NULL_OR_EMPTY(hostcolour))
    {
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                               HostColourFromString(hostcolour),
                                               promise_context_mode);
    }

    JsonElement *payload = NULL;

    if( report_file_info )
    {
        payload = WebExportPromiseComplianceReport( fhostkey, fhandle, fstatus, (bool) regex, filter,
                                                    hostColourFilter, true, promise_context_mode,
                                                    report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_compliance_promises(fhostkey, fhandle, fstatus, (bool) regex, filter,
                                               hostColourFilter, true, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_JSON(payload);
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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *status = NULL; int s_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fhandle = NULL;
    char *fstatus = NULL;
    zend_bool regex;
    zval *context_includes = NULL;
    zval *context_excludes = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaa",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
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

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *out = Nova2PHP_promise_compliance_summary(fhostkey, fhandle, fstatus,
                                                           (bool) regex, filter,
                                                           promise_context_mode);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_compliance_for_bundles)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *bundle = NULL; int b_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fbundle = NULL;
    zend_bool regex;
    zval *context_includes = NULL;
    zval *context_excludes = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaa",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
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

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *out = Nova2PHP_bundle_compliance_summary(fhostkey, fbundle,  (bool) regex,
                                                          filter, promise_context_mode);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(out);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_lastseen)
{
    char *userName, *hostkey, *host, *address, *hash;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, h_len, a_len, h2_len;
    long ago;
    time_t tago;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssslbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &hash, &h2_len,
                              &host, &h_len,
                              &address, &a_len,
                              &ago,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    tago = (time_t) ago;

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhash = (h2_len == 0) ? NULL : hash;
    char *fhost = (h_len == 0) ? NULL : host;
    char *faddress = (a_len == 0) ? NULL : address;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportLastseenReport( fhostkey, fhash, fhost, faddress, tago, (bool) regex,
                                          filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_lastseen_report(fhostkey, fhash, fhost, faddress, tago, (bool) regex,
                                           filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance)
{
    char *userName = NULL, *hostkey = NULL, *job = NULL;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, j_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &job, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fjob = (j_len == 0) ? NULL : job;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportPerformanceReport( fhostkey, fjob, (bool) regex, filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_performance_report(fhostkey, fjob, (bool) regex, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid)
{
    char *userName, *hostkey, *file;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, j_len;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportSetuidReport( fhostkey, ffile, (bool) regex, filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_setuid_report(fhostkey, ffile, (bool) regex, filter, &page, promise_context_mode);
    }
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges)
{
    char *userName, *hostkey, *file;
    char *fhostkey, *ffile;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, f_len;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbllaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &f_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (f_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportFileChangesReport( fhostkey, ffile, (bool) regex,
                                              from, to, filter,
                                              promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_filechanges_report(fhostkey, ffile, (bool) regex,
                                              from, to, filter, &page,
                                              promise_context_mode);
    }
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs)
{
    char *userName, *hostkey, *file, *diff;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, f_len, d_len;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbllaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &f_len,
                              &diff, &d_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *ffile = (f_len == 0) ? NULL : file;
    char *fdiff = (d_len == 0) ? NULL : diff;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportFileDiffsReport( fhostkey, ffile, fdiff, (bool) regex, from, to,
                                            filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_filediffs_report(fhostkey, ffile, fdiff, (bool) regex,
                                  from, to, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *bundle = NULL; int b_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &bundle, &b_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (b_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;

    if( report_file_info )
    {
        payload = WebExportBundleComplianceReport( hostkey, fbundle, (bool) regex, filter, NULL, false,
                                                   promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_bundle_report(fhostkey, fbundle, (bool) regex, filter, NULL, false, &page,
                                                  promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastknown_bundlesseen)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *bundle = NULL; int b_len;
    char *promise_context = NULL; int pc_len;
    char *hostcolour = NULL; int hc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &promise_context, &pc_len,
                              &bundle, &b_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (b_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    if (!NULL_OR_EMPTY(hostcolour))
    {
        // NOTE: colour filter is not used as mongo filter -- do not care about promise context here
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                               HostColourFromString(hostcolour),
                                               promise_context_mode);
    }

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportBundleComplianceReport( fhostkey, fbundle, (bool) regex, filter, hostColourFilter,
                                                   true, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_bundle_report(fhostkey, fbundle, (bool) regex, filter, hostColourFilter,
                                         true, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(hostColourFilter);

    RETURN_JSON(payload);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_report_value)
{
    char *userName = NULL, *hostkey = NULL, *day = NULL,
         *month = NULL, *year = NULL;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, d_len, m_len, y_len;
    PageInfo page = { 0 };
    char *sortColumnName;
    int sc_len;
    bool sortDescending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssaasbll|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &day, &d_len,
                              &month, &m_len,
                              &year, &y_len,
                              &context_includes,
                              &context_excludes,
                              &sortColumnName, &sc_len, &sortDescending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fday = (d_len == 0) ? NULL : day;
    char *fmonth = (m_len == 0) ? NULL : month;
    char *fyear = (y_len == 0) ? NULL : year;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportValueReport( fhostkey, fday, fmonth, fyear, filter,
                                        promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_value_report(fhostkey, fday, fmonth, fyear, filter,
                                        &page, promise_context_mode);
    }
    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int c_len;
    char *promise_context = NULL; int pc_len;
    char *sort_column_name; int sc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportPromiseLogReport( fhostkey, fhandle, fcause_rx,
                                          PROMISE_LOG_STATE_NOTKEPT, from, to,
                                          filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_promiselog(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_NOTKEPT,
                                      from, to, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_repaired)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int pc_len;
    char *promise_context = NULL; int c_len;
    char *sort_column_name = NULL; int sc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    bool sort_descending;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &sort_column_name, &sc_len, &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportPromiseLogReport( fhostkey, fhandle, fcause_rx,
                                             PROMISE_LOG_STATE_REPAIRED, from, to,
                                             filter, promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_promiselog(fhostkey, fhandle, fcause_rx, PROMISE_LOG_STATE_REPAIRED,
                                      from, to, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int c_len;
    zval *includes = NULL,
            *excludes = NULL,
            *report_file_info_array = NULL;
    char *sort_column_name; int sc_len;
    char *promise_context = NULL; int pc_len;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &includes,
                              &excludes,
                              &sort_column_name, &sc_len,
                              &sort_descending,
                              &(page.resultsPerPage),
                              &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;

    if( report_file_info )
    {
        payload = WebExportPromiseLogSummaryReport( hostkey, fhandle, fcause_rx,
                                                    PROMISE_LOG_STATE_NOTKEPT, from, to, filter,
                                                    promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_promiselog_summary(fhostkey, fhandle, fcause_rx,
                                             PROMISE_LOG_STATE_NOTKEPT, from,
                                             to, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_repaired)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int c_len;
    char *promise_context = NULL; int pc_len;
    zval *includes = NULL,
            *excludes = NULL,
            *report_file_info_array = NULL;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };
    char *sort_column_name; int sc_len;
    bool sort_descending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaasbll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &includes,
                              &excludes,
                              &sort_column_name, &sc_len,
                              &sort_descending,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *payload = NULL;
    if( report_file_info )
    {
        payload = WebExportPromiseLogSummaryReport( hostkey, fhandle, fcause_rx,
                                                    PROMISE_LOG_STATE_REPAIRED, from, to, filter,
                                                    promise_context_mode, report_file_info );

        DeleteWebReportFileInfo( report_file_info );
    }
    else
    {
        payload = Nova2PHP_promiselog_summary(fhostkey, fhandle, fcause_rx,
                                              PROMISE_LOG_STATE_REPAIRED, from,
                                              to, filter, &page, promise_context_mode);
    }

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_JSON(payload);
}

/******************************************************************************/
/* Returning hosts                                                            */
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_software_in)
{
    char *userName, *hostkey, *name, *version, *arch;
    char *fhostkey, *fname, *fversion, *farch;
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, n_len, v_len, a_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch,
                                                    (bool) regex, cfr_software, filter,
                                                    &page, promise_context_mode, report_file_info);

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
    char *userName = NULL, *hostkey = NULL, *day = NULL,
         *month = NULL, *year = NULL;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    int user_len, hk_len, d_len, m_len, y_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &day, &d_len,
                              &month, &m_len,
                              &year, &y_len,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fday = (d_len == 0) ? NULL : day;
    char *fmonth = (m_len == 0) ? NULL : month;
    char *fyear = (y_len == 0) ? NULL : year;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_value_hosts(fhostkey, fday, fmonth, fyear, filter, &page,
                                                 promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage),&(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fname = (n_len == 0) ? NULL : name;
    char *fversion = (v_len == 0) ? NULL : version;
    char *farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch,
                                       (bool) regex, cfr_patch_installed, filter,
                                       &page, promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, n_len, v_len, a_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &version, &v_len,
                              &arch, &a_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage),&(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fname = (n_len == 0) ? NULL : name;
    fversion = (v_len == 0) ? NULL : version;
    farch = (a_len == 0) ? NULL : arch;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_software_hosts(fhostkey, fname, fversion, farch,
                                                    (bool) regex, cfr_patch_avail, filter,
                                                    &page, promise_context_mode, report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *name = NULL; int n_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };
    time_t from, to;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaallll|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &name, &n_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &from, &to,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fname = (n_len == 0) ? NULL : name;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_classes_hosts(fhostkey, fname, (bool) regex, filter,
                                                   &page, from, to, promise_context_mode,
                                                   report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int c_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promiselog_hosts(fhostkey, fhandle, fcause_rx,
                                         PROMISE_LOG_STATE_REPAIRED, from, to,
                                         filter, &page, promise_context_mode, report_file_info);

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
    char *userName = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *cause_rx = NULL; int c_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    time_t from = 0, to = 0;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssllaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &cause_rx, &c_len,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhandle = (h_len == 0) ? NULL : handle;
    char *fcause_rx = (c_len == 0) ? NULL : cause_rx;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_promiselog_hosts(fhostkey, fhandle, fcause_rx,
                                         PROMISE_LOG_STATE_NOTKEPT, from, to,
                                         filter, &page, promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, s_len, l_len, r_len, t_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &scope, &s_len,
                              &lval, &l_len,
                              &rval, &r_len,
                              &type, &t_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fscope = (s_len == 0) ? NULL : scope;
    char *flval = (l_len == 0) ? NULL : lval;
    char *frval = (r_len == 0) ? NULL : rval;
    char *ftype = (t_len == 0) ? NULL : type;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_vars_hosts(fhostkey, fscope, flval, frval, ftype, (bool) regex,
                                                filter, &page, promise_context_mode, report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *version = NULL; int v_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    long k, nk, r, t;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssllllaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &version, &v_len,
                              &t,
                              &k, &nk, &r,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fversion = (v_len == 0) ? NULL : version;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_compliance_hosts(fhostkey, fversion, (int) t, time(NULL),
                                                      (int) k, (int) nk, (int) r, filter, &page,
                                                      promise_context_mode, report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *status = NULL; int s_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fhandle = NULL;
    char *fstatus = NULL;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_promise_hosts(fhostkey, fhandle, fstatus, (bool) regex,
                                                   filter, NULL, false, &page, promise_context_mode,
                                                   report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *handle = NULL; int h_len;
    char *status = NULL; int s_len;
    char *hostcolour = NULL; int hc_len;
    char *promise_context = NULL; int pc_len;
    char *fhostkey = NULL;
    char *fhandle = NULL;
    char *fstatus = NULL;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssssbaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &hostcolour, &hc_len,
                              &handle, &h_len,
                              &status, &s_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    fhandle = (h_len == 0) ? NULL : handle;
    fstatus = (s_len == 0) ? NULL : status;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    HostColourFilter *hostColourFilter = NULL;
    if(!NULL_OR_EMPTY(hostcolour))
    {
        // filter is used not on mongo so promise_context_mode value is not used
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                               HostColourFromString(hostcolour),
                                               promise_context_mode);
    }

    JsonElement *json_out = Nova2PHP_promise_hosts(fhostkey, fhandle, fstatus, (bool) regex,
                                                   filter, hostColourFilter, true, &page,
                                                   promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, h_len, a_len, h2_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    long ago;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssslbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &hash, &h2_len,
                              &host, &h_len,
                              &address, &a_len,
                              &ago,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fhash = (h2_len == 0) ? NULL : hash;
    char *fhost = (h_len == 0) ? NULL : host;
    char *faddress = (a_len == 0) ? NULL : address;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = Nova2PHP_lastseen_hosts(fhostkey, fhash, fhost, faddress, ago,
                                                    (bool) regex, filter, &page, promise_context_mode,
                                                    report_file_info);

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
    char *userName = NULL, *hostkey = NULL, *job = NULL;
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &job, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fjob = (j_len == 0) ? NULL : job;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_performance_hosts(fhostkey, fjob, (bool) regex, filter, &page,
                                          promise_context_mode, report_file_info);
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
    char *userName = NULL, *hostkey = NULL, *file = NULL;
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &j_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_setuid_hosts(fhostkey, ffile, (bool) regex, filter, &page,
                                     promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, j_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbllaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &j_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    fhostkey = (hk_len == 0) ? NULL : hostkey;
    ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_filechanges_hosts(fhostkey, ffile, (bool) regex, from,
                                          to, filter, &page, promise_context_mode, report_file_info);

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
    char *promise_context = NULL; int pc_len;
    int user_len, hk_len, j_len, d_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    time_t from, to;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbllaall|a",
                              &userName, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &file, &j_len,
                              &diff, &d_len,
                              &regex,
                              &from,
                              &to,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *ffile = (j_len == 0) ? NULL : file;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

        PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_filediffs_hosts(fhostkey, ffile, diff, (bool) regex,
                                        from, to, filter, &page,
                                        promise_context_mode,
                                        report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *bundle = NULL; int b_len;
    char *promise_context = NULL; int pc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssssbaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &pc_len,
                              &bundle, &b_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (b_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_bundle_hosts(fhostkey, fbundle, (bool) regex, filter,
                                     NULL, false, &page, promise_context_mode, report_file_info);

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
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *bundle = NULL; int b_len;
    char *promise_context = NULL; int pc_len;
    char *hostcolour = NULL; int hc_len;
    zval *context_includes = NULL,
            *context_excludes = NULL,
            *report_file_info_array = NULL;
    zend_bool regex;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssssbaall|a",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &hostcolour, &hc_len,
                              &promise_context, &pc_len,
                              &bundle, &b_len,
                              &regex,
                              &context_includes,
                              &context_excludes,
                              &(page.resultsPerPage), &(page.pageNum),
                              &report_file_info_array) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);
    WebReportFileInfo *report_file_info = NULL;
    PHP_ARRAY_GET_WEBREPORT_INFO( report_file_info_array, report_file_info );

    char *fhostkey = (hk_len == 0) ? NULL : hostkey;
    char *fbundle = (b_len == 0) ? NULL : bundle;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    HostClassFilterAddIncludeExcludeLists(filter, context_includes, context_excludes);

    HostColourFilter *hostColourFilter = NULL;

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    if (!NULL_OR_EMPTY(hostcolour))
    {
        // NOTE: colour filter is not used on mongo -- promise context is not used
        hostColourFilter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                               HostColourFromString(hostcolour),
                                               promise_context_mode);
    }

    JsonElement *json_out = NULL;
    json_out = Nova2PHP_bundle_hosts(fhostkey, fbundle, (bool) regex, filter,
                                     hostColourFilter, true, &page, promise_context_mode, report_file_info);

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
    Item *results, *ip;
    JsonElement *out = NULL;

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

    //Deprecated
    //JsonElement *out = NULL;
    //out = Nova2PHP_search_topics(fsearch, (bool) regex);
    
    results =  Nova2PHP_search(fsearch, true, username);

    if (results)
       {
       out = JsonArrayCreate(100);
       
       for (ip = results; ip != NULL; ip = ip->next)
          {
          JsonElement *json_obj = JsonObjectCreate(4);
          JsonObjectAppendString(json_obj, "url", ip->classes);
          JsonObjectAppendString(json_obj, "text", ip->name);

          switch (ip->counter)
             {
             case CF_CATEGORY_REPORTS:
                 JsonObjectAppendString(json_obj, "category", "Tools, Reports and Apps");
                 JsonObjectAppendInteger(json_obj, "rank", 1);
                 break;             
             case CF_CATEGORY_HOSTS:
                 JsonObjectAppendString(json_obj, "category", "Possible matches in hosts");
                 JsonObjectAppendInteger(json_obj, "rank", 2);                 
                 break;                 
             case CF_CATEGORY_VARS:
                 JsonObjectAppendString(json_obj, "category", "Possible matches in hosts");
                 JsonObjectAppendInteger(json_obj, "rank", 2);                 
                 break;                 
             case CF_CATEGORY_CLASSES:
                 JsonObjectAppendString(json_obj, "category", "Possible matches in hosts");
                 JsonObjectAppendInteger(json_obj, "rank", 2);                 
                 break;
             case CF_CATEGORY_TOPIC:
             default:
                 JsonObjectAppendString(json_obj, "category", "Topics in the knowledge index");
                 JsonObjectAppendInteger(json_obj, "rank", 3);                 
             }
          
          JsonArrayAppendObject(out, json_obj);
          }
       }
    else
       {
       out = JsonArrayCreate(0);
       }

    DeleteItemList(results);
    
    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_topics_for_bundle)
{
    char *name;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &name, &len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(len);

    JsonElement *out = Nova2PHP_list_topics_for_bundle(name);
    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_knowledge_bundles)
{
    JsonElement *out = NULL;
    out = Nova2PHP_list_knowledge_bundles();

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

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic((int) id, username);

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

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_leads((int) id, username);

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

    JsonElement *out = NULL;
    out = Nova2PHP_show_all_context_leads(unqualified_topic, username);

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

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_hits((int) id, username);

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

    JsonElement *out = NULL;
    out = Nova2PHP_show_topic_category((int) id);

    RETURN_JSON(out);
}

/******************************************************************************/
/* GRAPHS                                                                     */
/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_list_all)
{
    char *username = NULL; int user_len;
    char *promise_context = NULL; int promise_context_len;
    char buffer[CF_WEBBUFFER];
    PageInfo page = { 0 };
    zval *includes, *excludes;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaall",
                              &username, &user_len,
                              &promise_context, &promise_context_len,
                              &includes,
                              &excludes,
                              &(page.resultsPerPage),
                              &(page.pageNum)) == FAILURE)
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

    buffer[0] = '\0';

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    Nova2PHP_host_compliance_list_all(&conn, filter, &page, buffer, sizeof(buffer),
                                      promise_context_mode);

    DATABASE_CLOSE(&conn);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_count)
{
    char *username = NULL, *colour = NULL, *promise_context = NULL;
    int user_len, colour_len, promise_context_len;
    zval *includes, *excludes;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssaa",
                              &username, &user_len,
                              &colour, &colour_len,
                              &promise_context, &promise_context_len,
                              &includes, &excludes) == FAILURE)
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
    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    if (!NULL_OR_EMPTY(colour))
    {
        host_colour_filter = NewHostColourFilter(HOST_RANK_METHOD_COMPLIANCE,
                                                 HostColourFromString(colour),
                                                 promise_context_mode);
    }

    count = CFDB_CountHosts(&conn, filter, host_colour_filter, promise_context_mode);

    DATABASE_CLOSE(&conn);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
    free(host_colour_filter);

    RETURN_LONG(count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_compliance_timeseries)
{
    char *username = NULL;
    char *promise_context_mode = NULL;
    zval *contextIncludes = NULL;
    zval *contextExcludes = NULL;
    long username_len = -1;
    long promise_context_mode_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssaa",
                              &username, &username_len,
                              &promise_context_mode, &promise_context_mode_len,
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

        PromiseContextMode mode = PromiseContextModeFromString(promise_context_mode);

        result = CFDB_QueryTotalCompliance(&conn, NULL, NULL, from, to, -1, -1, -1,
                                           filter, mode, NULL, QUERY_FLAG_DISABLE_ALL);

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
    char *promise_context_mode = NULL;
    zval *contextIncludes = NULL,
         *contextExcludes = NULL;
    long username_len = -1;
    long promise_context_mode_len = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssaa",
                              &username, &username_len,
                              &promise_context_mode, &promise_context_mode_len,
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

        PromiseContextMode mode = PromiseContextModeFromString(promise_context_mode);

        records = CFDB_QueryHostComplianceShifts(&conn, filter, mode);

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
    char *promise_context = NULL;
    int promise_context_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssaall",
                              &userName, &user_len,
                              &colour, &colour_len,
                              &promise_context, &promise_context_len,
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

        PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

        Nova2PHP_host_compliance_list(&conn, colour, filter, &page, buffer,
                                      sizeof(buffer), promise_context_mode);

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
    char *promise_context = NULL; int pc_len;
    char buffer[128];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &userName, &user_len,
                              &hostKey, &hk_len,
                              &promise_context, &pc_len) == FAILURE)
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

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    buffer[0] = '\0';
    Nova2PHP_get_host_colour(hostKey, buffer, sizeof(buffer), promise_context_mode);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_handle_rx)
{
    char *userName = NULL, *handle = NULL;
    int user_len, h_len;
    char *promise_context = NULL; int pc_len;
    PageInfo page = { 0 };

    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sssll",
                              &userName, &user_len,
                              &promise_context, &pc_len,
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

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, promise_context_mode);

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

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, PROMISE_CONTEXT_MODE_ALL);

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

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, PROMISE_CONTEXT_MODE_ALL);

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

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, PROMISE_CONTEXT_MODE_ALL);

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
    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, PROMISE_CONTEXT_MODE_ALL);

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

    Nova2PHP_promise_list(filter, buffer, sizeof(buffer), &page, PROMISE_CONTEXT_MODE_ALL);

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

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter, PROMISE_CONTEXT_MODE_ALL);

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

    JsonElement *payload = Nova2PHP_bundle_classes_used(filter);

    DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

    RETURN_JSON(payload);
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

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter, PROMISE_CONTEXT_MODE_ALL);

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
    char *userName = NULL; int user_len;
    char *promise_context = NULL; int pc_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss",
                              &userName, &user_len,
                              &promise_context, &pc_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    HubQuery *hqPromiseFilter = CFDB_PromiseFilterFromUserRBAC(userName);

    ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

    PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    EnterpriseDB conn;

    DATABASE_OPEN(&conn);

    HubQuery *hqBundles = CFDB_QueryPromiseBundles(&conn, filter, promise_context_mode);

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

    HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter, PROMISE_CONTEXT_MODE_ALL);

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

PHP_FUNCTION(cfpr_list_services)
{
 JsonElement *out = Nova2PHP_list_services();

 if (!out)
 {
     out = JsonObjectCreate(0);
 }

 RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_service_ports)
{
 JsonElement *out = Nova2PHP_list_service_ports();

 if (!out)
 {
     out = JsonObjectCreate(0);
 }

 RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(list_promises_with_promisee)
{
    char *name;
    int n_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &name, &n_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = Nova2PHP_list_promises_with_promisee(name);

    if (!out)
    {
        out = JsonObjectCreate(0);
    }

    RETURN_JSON(out);
}
/******************************************************************************/

PHP_FUNCTION(agent_bundle_type_histogram)
{
    char *name;
    int n_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &name, &n_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    JsonElement *out = Nova2PHP_list_types_in_bundle(name);

    if (!out)
    {
       out = JsonObjectCreate(0);
    }

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_goal_progress)
{
 long hlen;
 char *handle;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &handle, &hlen) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }
 
 JsonElement *out = Nova2PHP_get_goal_progress(handle);

 if (!out)
 {
     out = JsonObjectCreate(0);
 }

 RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_for_topic)
{
 long id;
 char buffer[1000];
 
 if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "l", &id) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

 Nova2PHP_bundle_for_topic((int) id, buffer, sizeof(buffer));
 RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_service_histogram)
{
    JsonElement *out = Nova2PHP_get_service_histogram();

    if (!out)
    {
        out = JsonObjectCreate(0);
    }

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_service_level_histogram)
{
    char *srv;
    int s_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &srv, &s_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }
 
    JsonElement *out = Nova2PHP_get_service_level_histogram(srv);

    if (!out)
    {
        out = JsonObjectCreate(0);
    }

    RETURN_JSON(out);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_class_frequency)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *promise_context = NULL; int promise_context_len;
    char *pattern = NULL; int p_len;

    char buffer[CF_WEBBUFFER];

   if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssss",
                             &username, &user_len,
                             &hostkey, &hk_len,
                             &promise_context, &promise_context_len,
                             &pattern, &p_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    char *fpattern = (p_len == 0) ? NULL : pattern;
    char *fhostkey = (hk_len == 0) ? NULL : hostkey;

    buffer[0] = '\0';

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    int count = Nova2PHP_countclasses(fhostkey, fpattern, 1, filter, buffer, sizeof(buffer),
                                      promise_context_mode);

    DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

    RETURN_LONG((long) count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_class_frequency)
{
    char *username = NULL; int user_len;
    char *hostkey = NULL; int hk_len;
    char *promise_context = NULL; int promise_context_len;
    char *pattern = NULL; int p_len;

    char buffer[CF_WEBBUFFER];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssss",
                              &username, &user_len,
                              &hostkey, &hk_len,
                              &promise_context, &promise_context_len,
                              &pattern, &p_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    buffer[0] = '\0';

    char *fpattern = (p_len == 0) ? NULL : pattern;
    char *fhostkey = (hk_len == 0) ? NULL : hostkey;

    HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);

    ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

    HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

    PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

    Nova2PHP_countclasses(fhostkey, fpattern, 1, filter, buffer, sizeof(buffer),
                          promise_context_mode);

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
    out = Nova2PHP_get_knowledge_view(pid, view, username);

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

    switch (CFDB_UserAuthenticate(username, password, NULL))
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

PHP_FUNCTION(cfpr_update_host_identifier)
{
    char *username, *scope, *variable;
    int user_len, s_len, v_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sss",
                              &username, &user_len,
                              &scope, &s_len,
                              &variable, &v_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len && s_len && v_len);

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_BOOL(false);
    }

    EnterpriseDB conn[1];
    DATABASE_OPEN(conn);

    char full_var_name[CF_MAXVARSIZE] = {0};
    snprintf(full_var_name, CF_MAXVARSIZE, "%s.%s", scope, variable);

    if (!CFDB_PutValue(conn, cfr_host_identifier, full_var_name, MONGO_SCRATCH))
    {
        DATABASE_CLOSE(conn);
        zend_throw_exception(cfmod_exception_db, "Unable to update host identifier", 0 TSRMLS_CC);
        RETURN_BOOL(false);
    }

    DATABASE_CLOSE(conn);
    RETURN_BOOL(true);
}

/******************************************************************************/

PHP_FUNCTION( cfpr_reset_host_identifier )
{
    char *username;
    int user_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s",
                              &username, &user_len ) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS( user_len );

    if (CFDB_UserIsAdminWhenRBAC(username) != ERRID_SUCCESS)
    {
        zend_throw_exception(cfmod_exception_rbac, LABEL_ERROR_RBAC_NOT_ADMIN, 0 TSRMLS_CC);
        RETURN_BOOL(false);
    }

    EnterpriseDB conn[1];
    DATABASE_OPEN(conn);

    if (!CFDB_PutValue(conn, cfr_host_identifier, "", MONGO_SCRATCH))
    {
        DATABASE_CLOSE(conn);
        zend_throw_exception(cfmod_exception_db, "Unable to reset host identifier", 0 TSRMLS_CC);
        RETURN_BOOL(false);
    }

    DATABASE_CLOSE(conn);
    RETURN_BOOL(true);
}

/**************************************************/

PHP_FUNCTION(cfpr_get_host_identifier)
{
    const char *user = NULL;
    int user_len;

    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC,
                                "s",
                                &user, &user_len
                                ) == FAILURE)
    {
        zend_throw_exception( cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC );
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(user_len);

    EnterpriseDB conn[1];
    DATABASE_OPEN( conn );

    char host_identifier[CF_MAXVARSIZE] = {0};
    CFDB_HandleGetValue( cfr_host_identifier, host_identifier, CF_MAXVARSIZE - 1, "", conn, MONGO_SCRATCH );

    DATABASE_CLOSE( conn );

    JsonElement *output = JsonObjectCreate(1);
    JsonObjectAppendString(output, LABEL_HOST_IDENTIFIER, host_identifier);

    RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION( cfpr_get_reverse_ip_lookup_name )
{
    char *user, *hostkey;
    int user_len, hk_len;

    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC,
                                "ss",
                                &user, &user_len,
                                &hostkey, &hk_len) == FAILURE)
    {
        zend_throw_exception( cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC );
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS( user_len && hk_len );

    cfapi_errid err = CFDB_HasHostAccessFromUserRBAC( user, hostkey );

    if ( err != ERRID_SUCCESS )
    {
        zend_throw_exception( cfmod_exception_rbac, (char *) GetErrorDescription( err ), 0 TSRMLS_CC );
        RETURN_NULL();
    }

    EnterpriseDB conn[1];
    DATABASE_OPEN( conn );

    char reverse_lookup_name[CF_BUFSIZE] = "\0";

    CFDB_QueryValueFromHostKeyStr( conn, hostkey, cfr_reverse_lookup_name, reverse_lookup_name, CF_BUFSIZE - 1 );

    DATABASE_CLOSE( conn );

    RETURN_STRING( reverse_lookup_name, 1 );
}

/******************************************************************************/
/* Mission Tree-Control (Astrolabe)                                           */
/******************************************************************************/

PHP_FUNCTION(cfpr_astrolabe_host_list)
{
    char *username = NULL; int username_len;
    char *promise_context = NULL; int promise_context_len;
    zval *includes, *excludes;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssaa",
                              &username, &username_len,
                              &promise_context, &promise_context_len,
                              &includes, &excludes) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(username_len);
    ARGUMENT_CHECK_CONTENTS(promise_context_len);

    HubQuery *result = NULL;
    {
        HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
        ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

        HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);
        HostClassFilterAddIncludeExcludeLists(filter, includes, excludes);

        EnterpriseDB conn;
        DATABASE_OPEN(&conn);

        PromiseContextMode promise_context_mode = PromiseContextModeFromString(promise_context);

        result = CFDB_QueryColour(&conn, HOST_RANK_METHOD_COMPLIANCE, filter, promise_context_mode);

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
