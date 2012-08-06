#include "private-api-constellation.h"

#include "common.h"
#include "files_names.h"

#ifdef HAVE_CONSTELLATION

static char *cfcon_aggr_promiselog(int argc, PromiseLogState state);
static char *cfcon_count_promiselog(int argc, PromiseLogState state);
static char *cfcon_reasons_promiselog(int argc, PromiseLogState state);


PHP_FUNCTION(cfcon_compliance_summary_graph)
{
    const int bufsize = 512 * 1024;
    char buffer[bufsize];
    char *hubKeyHash, *fhubKeyHash, *policy, *fpolicy;
    int pol_len, hkh_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &hubKeyHash, &hkh_len, &policy, &pol_len) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

// "any" is keyword for all policies

    fpolicy = (pol_len == 0) ? "any" : policy;
    fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

    buffer[0] = '\0';
    Con2PHP_ComplianceSummaryGraph(fhubKeyHash, fpolicy, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_compliance_virtualbundle_graph)
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *hubHostKey, *bundleName;
    char *fHubHostKey, *fBundleName;
    int hkLen, vbLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &hubHostKey, &hkLen, &bundleName, &vbLen) == FAILURE)
    {
        RETURN_NULL();
    }

    fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
    fBundleName = (vbLen == 0) ? NULL : bundleName;

    ARGUMENT_CHECK_CONTENTS(vbLen);

    buffer[0] = '\0';
    Con2PHP_compliance_virtualbundle_graph(fHubHostKey, fBundleName, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_hubs)
{
    const int bufsize = 64;
    char buffer[bufsize];
    char *classification, *fclassification;     // red/green, class regex?...
    int cl_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &classification, &cl_len) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fclassification = (cl_len == 0) ? NULL : classification;

    buffer[0] = '\0';
    Con2PHP_count_hubs(fclassification, buffer, sizeof(buffer));
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_show_hubs)
{
    const int bufsize = 16384;
    char buffer[bufsize];
    char *classification, *fclassification;     // red/green, class regex?...
    int cl_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &classification, &cl_len) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fclassification = (cl_len == 0) ? NULL : classification;

    buffer[0] = '\0';
    Con2PHP_show_hubs(fclassification, buffer, sizeof(buffer));
    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_repaired)
{
    char *buffer;

    buffer = cfcon_aggr_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_REPAIRED);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_notkept)
{
    char *buffer;

    buffer = cfcon_aggr_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_NOTKEPT);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

static char *cfcon_aggr_promiselog(int argc, PromiseLogState state)
/**
 * Helper for repaired and notkept logs.
 **/
{
    int hkh_len, ph_len;
    static char buffer[CF_WEBBUFFER];
    char *hubKeyHash, *fhubKeyHash;
    char *promiseHandle, *fPromiseHandle;
    PageInfo page = { 0 };

    if (zend_parse_parameters(argc, "ssll", &hubKeyHash, &hkh_len, &promiseHandle, &ph_len,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        return NULL;
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        return buffer;
    }

    fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;
    fPromiseHandle = (ph_len == 0) ? NULL : promiseHandle;

    buffer[0] = '\0';
    Con2PHP_aggr_promiselog(fhubKeyHash, fPromiseHandle, state, &page, buffer, sizeof(buffer));

    return buffer;
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_repaired)
{
    char *buffer;

    buffer = cfcon_count_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_REPAIRED);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_count_notkept)
{
    char *buffer;

    buffer = cfcon_count_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_NOTKEPT);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

static char *cfcon_count_promiselog(int argc, PromiseLogState state)
/**
 * Helper for repaired and notkept logs.
 **/
{
# define CFCON_CPL_BUFSIZE 64

    char *promiseHandle, *fpromiseHandle;
    int ph_len, hkh_len;
    static char buffer[CFCON_CPL_BUFSIZE];
    char *hubKeyHash, *fhubKeyHash;

    if (zend_parse_parameters(argc, "ss", &hubKeyHash, &hkh_len, &promiseHandle, &ph_len) == FAILURE)
    {
        return NULL;
    }

    fpromiseHandle = (ph_len == 0) ? NULL : promiseHandle;
    fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

    buffer[0] = '\0';
    Con2PHP_count_promiselog(fhubKeyHash, fpromiseHandle, state, buffer, sizeof(buffer));

    return buffer;
}

/******************************************************************************/

PHP_FUNCTION(cfcon_reasons_repaired)
{
    char *buffer;

    buffer = cfcon_reasons_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_REPAIRED);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

PHP_FUNCTION(cfcon_reasons_notkept)
{
    char *buffer;

    buffer = cfcon_reasons_promiselog(ZEND_NUM_ARGS()TSRMLS_CC, PROMISE_LOG_STATE_NOTKEPT);

    if (!buffer)
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_STRING(buffer, 1);
    }
}

/******************************************************************************/

static char *cfcon_reasons_promiselog(int argc, PromiseLogState state)
/**
 * Helper for repaired and notkept logs.
 **/
{
# define CFCON_RPL_BUFSIZE 16384

    char *promiseHandle, *fpromiseHandle, *hubKeyHash, *fhubKeyHash;
    int ph_len, hkh_len;
    static char buffer[CFCON_RPL_BUFSIZE];

    if (zend_parse_parameters(argc, "ss", &hubKeyHash, &hkh_len, &promiseHandle, &ph_len) == FAILURE)
    {
        return NULL;
    }

    fpromiseHandle = (ph_len == 0) ? NULL : promiseHandle;
    fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

    buffer[0] = '\0';
    Con2PHP_reasons_promiselog(fhubKeyHash, fpromiseHandle, state, buffer, sizeof(buffer));

    return buffer;
}

/******************************************************************************/

PHP_FUNCTION(cfcon_environments_list)
{
    char *hubKeyHash, *fhubKeyHash;
    int hkh_len;
    const int bufsize = 1024;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubKeyHash, &hkh_len) == FAILURE)
    {
        RETURN_NULL();
    }

    fhubKeyHash = (hkh_len == 0) ? NULL : hubKeyHash;

    buffer[0] = '\0';
    Con2PHP_environments_list(fhubKeyHash, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_promise_popularity)
{
    char *promiseHandle;
    int prh_len;
    const int bufsize = 256;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &promiseHandle, &prh_len) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(prh_len);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    Con2PHP_promise_popularity(promiseHandle, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_rank_promise_popularity)
{
    const int bufsize = CF_WEBBUFFER;;
    char buffer[bufsize];
    zend_bool sortAscending;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "b", &sortAscending) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    Con2PHP_rank_promise_popularity(sortAscending, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_filechange)
{
    const int bufsize = CF_WEBBUFFER;
    char buffer[bufsize];
    char *hubHostKey, *filePath;
    char *fHubHostKey, *fFilePath;
    int hkLen, fpLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &hubHostKey, &hkLen, &filePath, &fpLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
    fFilePath = (fpLen == 0) ? NULL : filePath;

    buffer[0] = '\0';
    Con2PHP_aggr_filechange(fHubHostKey, fFilePath, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_software)
{
    const int bufsize = CF_WEBBUFFER;
    char buffer[bufsize];
    char *hubHostKey, *pkgname;
    char *fHubHostKey, *fPackageName;
    int hkLen, pnLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &hubHostKey, &hkLen, &pkgname, &pnLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
    fPackageName = (pnLen == 0) ? NULL : pkgname;

    buffer[0] = '\0';
    Con2PHP_aggr_software(fHubHostKey, fPackageName, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_aggr_classes)
{
    const int bufsize = CF_WEBBUFFER;
    char buffer[bufsize];
    char *hubHostKey, *classname;
    char *fHubHostKey, *fClassName;
    int hkLen, cnLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &hubHostKey, &hkLen, &classname, &cnLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fHubHostKey = (hkLen == 0) ? NULL : hubHostKey;
    fClassName = (cnLen == 0) ? NULL : classname;

    buffer[0] = '\0';
    Con2PHP_aggr_classes(fHubHostKey, fClassName, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/
/* Constellation subscribe/report                                             */
/******************************************************************************/

PHP_FUNCTION(cfcon_list_subscriptions)
/**
 * Generic "show all subscritions" function
 **/
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *user, *subHandleRx;
    int usLen, shLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &user, &usLen, &subHandleRx, &shLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char *fUser = (usLen == 0) ? NULL : user;
    char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

    buffer[0] = '\0';
    Con2PHP_list_subscriptions(fUser, NULL, fSubHandleRx, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_show_subscription_virtualbundle)
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *user, *subHandleRx;
    int usLen, shLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &user, &usLen, &subHandleRx, &shLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char *fUser = (usLen == 0) ? NULL : user;
    char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

    buffer[0] = '\0';
    Con2PHP_list_subscriptions(fUser, cfl_subtype_local_virtualbundle, fSubHandleRx, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_subscribe_software)
// FIXME: Take description as input, in addition to handle
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *user, *hubClassRegex, *subHandle, *pkgName, *hostClassRegex;
    zend_bool pnRegex;
    int usLen, hcLen, shLen, pnLen, ocLen;

    if (zend_parse_parameters
        (ZEND_NUM_ARGS()TSRMLS_CC, "ssssbs", &user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen, &pkgName,
         &pnLen, &pnRegex, &hostClassRegex, &ocLen) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(usLen && shLen && pnLen && ocLen);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    if (hcLen == 0)
    {
        hubClassRegex = "any";
    }

    buffer[0] = '\0';
    Con2PHP_subscribe_software(user, subHandle, hubClassRegex, pkgName, pnRegex, hostClassRegex, buffer,
                               sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_report_software)
{
    char buffer[CF_WEBBUFFER];
    char *user, *subHandle, *hubClassRegex;
    int usLen, shLen, hcLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters
        (ZEND_NUM_ARGS()TSRMLS_CC, "sssll", &user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen,
         &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(usLen && shLen);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    if (hcLen == 0)
    {
        hubClassRegex = "any";
    }

    buffer[0] = '\0';
    Con2PHP_report_software(user, subHandle, hubClassRegex, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);

}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_report_virtualbundle)
{
    char buffer[CF_WEBBUFFER];
    char *user, *subHandleRx;
    int usLen, shLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &user, &usLen, &subHandleRx, &shLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char *fUser = (usLen == 0) ? NULL : user;
    char *fSubHandleRx = (shLen == 0) ? NULL : subHandleRx;

    buffer[0] = '\0';
    Con2PHP_local_report_virtualbundle(fUser, fSubHandleRx, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);

}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_report_virtualbundle_promises)
{
    char buffer[CF_WEBBUFFER];
    char *user, *subHandle;
    int usLen, shLen;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ssll", &user, &usLen, &subHandle, &shLen,
                              &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(shLen);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char *fUser = (usLen == 0) ? NULL : user;

    buffer[0] = '\0';
    Con2PHP_local_report_virtualbundle_promises(fUser, subHandle, &page, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);

}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_subscribe_virtualbundle)
// NOTE: Available to Constellation Mission Portal, not Mission Observatory (hence cfcon_local)
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *user, *description, *subHandle, *promiseListStr, *hostClassRegex;
    int usLen, descLen, shLen, plLen, ocLen;

    if (zend_parse_parameters
        (ZEND_NUM_ARGS()TSRMLS_CC, "sssss", &user, &usLen, &subHandle, &shLen, &description, &descLen, &promiseListStr,
         &plLen, &hostClassRegex, &ocLen) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(usLen && shLen && descLen && plLen);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char **promises = String2StringArray(promiseListStr, ',');

    char *fHostClassRx = (ocLen == 0) ? NULL : hostClassRegex;

    buffer[0] = '\0';
    Con2PHP_local_subscribe_virtualbundle(user, subHandle, description, promises, fHostClassRx, buffer, sizeof(buffer));

    FreeStringArray(promises);

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_local_delete_subscription_virtualbundle)
{
    char buffer[CF_WEBBUFFER] = { 0 };
    char *user, *subHandle;
    int usLen, shLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &user, &usLen, &subHandle, &shLen) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(shLen);

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    char *fUser = (usLen == 0) ? NULL : user;

    Con2PHP_delete_subscription(fUser, cfl_subtype_local_virtualbundle, subHandle, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/******************************************************************************/

PHP_FUNCTION(cfcon_subscribe_repairlog)
{
    const int bufsize = 4096;
    char buffer[bufsize];
    char *user, *hubClassRegex, *subHandle, *promiseHandleRegex, *reportRegex, *hostClassRegex;
    int usLen, hcLen, shLen, phLen, rxLen, ocLen;

    if (zend_parse_parameters
        (ZEND_NUM_ARGS()TSRMLS_CC, "ssssss", &user, &usLen, &subHandle, &shLen, &hubClassRegex, &hcLen,
         &promiseHandleRegex, &phLen, &reportRegex, &rxLen, &hostClassRegex, &ocLen) == FAILURE)
    {
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(usLen && shLen && phLen && ocLen);

    if (rxLen == 0)
    {
        reportRegex = NULL;
    }

    if (hcLen == 0)
    {
        hubClassRegex = "any";
    }

    buffer[0] = '\0';
    Con2PHP_subscribe_promiselog(user, subHandle, hubClassRegex, promiseHandleRegex, reportRegex, hostClassRegex,
                                 cfl_subtype_repairlog, buffer, sizeof(buffer));

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_hub_meter
 * Returns Meter data for a given hub key
 * @param string (hub keyhash)
 * @return NULL if argument mismatch
 *         JSON {meta:{},data:[],error:{}}
 * @throws cfmod_db_exception_ce (Exception)
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/
PHP_FUNCTION(cfcon_hub_meter)
{
    char *hostkey, *fhostkey;
    int hk_len;
    const int bufsize = 512 * 1024;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hostkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fhostkey = (hk_len == 0) ? NULL : hostkey;

    buffer[0] = '\0';
    Con2PHP_meter(fhostkey, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_count_hubs_colour
 * Count the number of hubs by colour code (status)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted
 *      3. JSON {meta:{},data:[],error:{}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_count_hubs_colour)
{
    const int bufsize = 1024;
    char buffer[bufsize];
    char *colour;
    int c_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &colour, &c_len) == FAILURE)
    {
        RETURN_NULL();
    }

    if (*colour != 'R' && *colour != 'G' && *colour != 'B' && *colour != 'Y' && *colour != 'A')
    {
        FormatSingletonErrorJson(buffer, sizeof(buffer), ERRID_ARGUMENT_WRONG);
        RETURN_STRING(buffer, 1);
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    colour[1] = '\0';
    Con2PHP_count_hubs_colour(colour, buffer, bufsize);

// TODO: throw exception
    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_get_hub_colour
 * Get the hub colour
 * @param: string (hubkey)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON {meta:{},data:[],error:{}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_get_hub_colour)
{
    char *hostkey;
    int hk_len;
    const int bufsize = 10000;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hostkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    Con2PHP_get_hub_colour(hostkey, buffer, bufsize);

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_count_hub_hosts
 * Get the count of hub hosts according to given colour
 * @param string (hubkey)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted
 *      2. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_count_hub_hosts)
{
    char *hostkey, *fhostkey;
    int hk_len;
    const int bufsize = 10000;
    char buffer[bufsize];
    char *colour;
    int c_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &hostkey, &hk_len, &colour, &c_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    if (*colour != 'R' && *colour != 'G' && *colour != 'B' && *colour != 'Y' && *colour != 'A')
    {
        RETURN_STRING("{\"error\":{\"errid\":\"-1\",\"msg\":\"Unrecognized parameters supplied\"}}", 1);
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    fhostkey = (hk_len == 0) ? NULL : hostkey;

    buffer[0] = '\0';
    colour[1] = '\0';
    Con2PHP_count_hub_hosts(fhostkey, colour, buffer, bufsize);

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_list_hubs
 * Return hubs(keyhash,name, IP)  according to given colour
 * @param string (hubkey)
 * @param string ("A"=all,"B"=blue,"R"=red,"G"=green,"Y"=yellow)
 * @return
 *      1. NULL if argument mismatch
 *      2. JSON error string if parameters are not accepted
 *      2. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_list_hub_colour)
{
    const int bufsize = 100000;
    char buffer[bufsize];
    char *colour;
    int c_len;
    PageInfo page = { 0 };

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sll", &colour, &c_len, &(page.resultsPerPage), &(page.pageNum))
        == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    if (*colour != 'R' && *colour != 'G' && *colour != 'B' && *colour != 'Y' && *colour != 'A')
    {
        RETURN_STRING("{\"error\":{\"errid\":\"-1\",\"msg\":\"Unrecognized parameters supplied\"}}", 1);
    }

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    colour[1] = '\0';
    Con2PHP_list_hub_colour(colour, &page, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_list_hubs
 * Return averages for kept, repaired and notkept data for Business value pie chart
 * @return
 *      1. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_value_graph)
{
    const int bufsize = 100000;
    char buffer[bufsize];

    if (!Con2PHP_CheckLicenseAndFormatError(buffer, sizeof(buffer)))
    {
        RETURN_STRING(buffer, 1);
    }

    buffer[0] = '\0';
    Con2PHP_get_value_graph(buffer, bufsize);

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_hub_details
 * Returns OS Class, Flavour, Release, CFE enterprise version and CFE community version for a hub
 * @return
 *      1. JSON {meta:{count=<count>},data:[],error:{...}} on success
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_hub_details)
{
    const int bufsize = 100000;
    char buffer[bufsize];
    char *hubkeyhash;
    int h_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubkeyhash, &h_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Con2PHP_GetHubCFEAndOSDetails(hubkeyhash, buffer, bufsize);

    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_getlastupdate
 * Returns last time the Mission Portal hubs reported to the Mission Observaroty (cfpr_getlastupdate)
 * @return
 *      1. string (timestamp)
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/
PHP_FUNCTION(cfcon_getlastupdate)
{
    char *hubkey;
    int hk_len;
    const int bufsize = 100;
    char buffer[bufsize];

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    buffer[0] = '\0';
    Con2PHP_getlastupdate(hubkey, buffer, bufsize);
    RETURN_STRING(buffer, 1);
}

/*****************************************************************************
 * Name: cfcon_hubname
 * Returns hubname corresponding to a keyhash
 * @return
 *      1. string (hub name)
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_hubname)
{
    char s1[4096], s2[4096];
    char *hubkey;
    int hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    s1[0] = '\0';
    s2[0] = '\0';
    Con2PHP_hubinfo(hubkey, s1, s2, sizeof(s1));

    RETURN_STRING(s1, 1);
}

/*****************************************************************************
 * Name: cfcon_ipaddr
 * Returns ipaddress corresponding to a keyhash
 * @return
 *      1. string (ipaddresses )
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_ipaddr)
{
    char s1[4096], s2[4096];
    char *hubkey;
    int hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    s1[0] = '\0';
    s2[0] = '\0';
    Con2PHP_hubinfo(hubkey, s1, s2, sizeof(s1));
    RETURN_STRING(s2, 1);
}

/*****************************************************************************
 * Name: cfcon_delete_host
 * Delete the record of a certain host from web-supporting DB
 * @return
 *      1. true or false
 *
 * @throws cfmod_db_exception_ce
 * @lastmodifiedby bishwa.shrestha@cfengine.com
 * @todo throw exception
 *****************************************************************************/

PHP_FUNCTION(cfcon_delete_hub)
{
    char *hubkey;
    int hk_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s", &hubkey, &hk_len) == FAILURE)
    {
        zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
        RETURN_NULL();
    }

    ARGUMENT_CHECK_CONTENTS(hk_len);

    Con2PHP_delete_hub(hubkey);
}

#endif /* HAVE_CONSTELLATION */
