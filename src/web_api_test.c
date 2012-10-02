#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"
#include "files_names.h"

#define WORD_LEN  10
#define HOSTNAME_LEN  30
#define KEYHASH_LEN 70
#define HANDLE_LEN 20
#define REPORT_LEN 70

#define CHAR_POOL "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz _2134l kahsdlkfhjsad 982374 lkjdflands 8234987 adslf 567890 asdf"

/***************************************************************************************************************/
static int GetTotalCount(int endIndex);
static char *strcatUnsafe(char *dest, char *src);
static const char *RandomizeString(int len, char *buffer, int buflen);

/***************************************************************************************************************/
/* 
 * Using Join/strcat is too slow
 * Buffer limit for destination must be handled by the calling function to avoid segfault
*/
/***************************************************************************************************************/
char *strcatUnsafe(char *dest, char *src)
{
    while (*dest)
    {
        dest++;
    }

    while (*src)
    {
        *dest++ = *src++;
    }

    return --dest;
}

/***************************************************************************************************************/
const char *RandomizeString(int len, char *buffer, int buflen)
{
    int max = strlen(CHAR_POOL);

    buffer[0] = '\0';

    for (int i = 0; i < len; i++)
    {
        int index = rand() % max;

        buffer[i] = CHAR_POOL[index];
        buffer[i + 1] = '\0';
    }

    return buffer;
}

/***************************************************************************************************************
 * cfpr_report_classes (sssbssbll) 
 **************************************************************************************************************/
int Nova2PHP_classes_report_test(char *hostkey, char *name, bool regex, HostClassFilter *hostClassFilter, PageInfo *page,
                                 char *returnval, int bufsize)
{
    char work[CF_BUFSIZE] = { 0 };
    char header[CF_BUFSIZE] = { 0 };
    char context[CF_MAXVARSIZE];
    char *p = returnval;

    int truncated = false;
    int totalLen = 0;
    int recordLen = 0;
    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;

    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\": {\"Host\":0,\"Class Context\":1,\"Occurs with Probability\":2,\"Uncertainty\":3,\"Last seen\":4"
             "}", total);

    StartJoin(returnval, "{\"data\":[", bufsize);
    totalLen = strlen(header) + strlen(CF_NOTICE_TRUNCATED) + strlen(returnval) + 1000;

    for (int i = startIndex; i < endIndex; i++)
    {
        if (!recordLen)
        {
            recordLen = strlen(work);
        }
        totalLen += recordLen;

        if (totalLen >= bufsize - 1)
        {
            truncated = true;
            break;
        }

        RandomizeString(HANDLE_LEN, context, sizeof(context));

        snprintf(work, sizeof(work), "[\"%s\",\"%s\",%lf,%lf,%ld],", "myhost", context, 1.8, 1.5, time(NULL));

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return true;
}

/**************************************************************************************************************/
/*
 * cfpr_promise_list_by_handle_rx(ss)
*/
int Nova2PHP_promise_list_test(PromiseFilter *promiseFilter, char *returnval, int bufsize)
{
    int total = GetTotalCount(0);

    if (total > 80000)
    {
        total = 80000;
    }

    StartJoin(returnval, "[", bufsize);

    char *p = returnval;

    for (int i = 0; i < total; i++)
    {
        char work[CF_MAXVARSIZE] = { 0 };
        char handle[CF_MAXVARSIZE] = { 0 };
        char promiseType[CF_MAXVARSIZE] = { 0 };
        char bundleName[CF_MAXVARSIZE] = { 0 };
        char bundleType[CF_MAXVARSIZE] = { 0 };
        char promiser[CF_MAXVARSIZE] = { 0 };

        RandomizeString(WORD_LEN, promiseType, sizeof(promiseType));
        RandomizeString(WORD_LEN, bundleType, sizeof(bundleType));
        RandomizeString(WORD_LEN, promiser, sizeof(promiser));
        RandomizeString(HANDLE_LEN, handle, sizeof(handle));
        RandomizeString(HANDLE_LEN, bundleName, sizeof(bundleName));

        snprintf(work, sizeof(work), "[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"],",
                 handle, promiseType, bundleName, bundleType, promiser);

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    return true;
}

/*****************************************************************************/
/*
 * cfpr_class_cloud(s) 
 * buffer limit ~5Kb
*/
int Nova2PHP_classes_summary_test(char **classes, char *returnval, int bufsize)
{
    int total = GetTotalCount(0);

    if (total > 9000)
    {
        total = 9000;
    }

    char *p = returnval;

    StartJoin(returnval, "{\"hosts\":[", bufsize);

    for (int i = 0; i < (total / 5); i++)
    {
        char work[CF_MAXVARSIZE] = { 0 };
        char hostname[CF_MAXVARSIZE] = { 0 };
        char keyhash[CF_MAXVARSIZE] = { 0 };

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(KEYHASH_LEN, keyhash, sizeof(keyhash));

        snprintf(work, sizeof(work), "[\"%s\",\"%s\"]\n,", hostname, keyhash);

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Join(returnval, ",\n\"classes\":[", bufsize - 10);

    for (int i = 0; i < (total * 4) / 5; i++)
    {
        char work[CF_MAXVARSIZE] = { 0 };
        char context[CF_MAXVARSIZE] = { 0 };

        RandomizeString(HANDLE_LEN, context, sizeof(context));

        snprintf(work, sizeof(work), "[\"%s\",%d]\n,", context, i);

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]}", bufsize);

    return true;
}

/*****************************************************************************/
/*
 * cfpr_show_hosts_name(ssll)
 * cfpr_show_hosts_ip(ssll)
*/
int Nova2PHP_show_hosts_test(char *hostNameRegex, char *ipRegex, char *classRegex, PageInfo *page, char *returnval,
                             int bufsize)
{
    char work[CF_MAXVARSIZE] = { 0 };

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;

    int total = GetTotalCount(endIndex);

// Max limit to avoid segfault
    if (total > 60000)
    {
        total = 60000;
    }

    if (endIndex > total)
    {
        endIndex = total;
    }

    char *p = returnval;

    snprintf(work, sizeof(work),
             "{\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\": {\"Key Hash\":0,\"Host name\":1,\"IP address\":2" "}},\n\"data\":[", total);

    StartJoin(returnval, work, bufsize);

    for (int i = startIndex; i < endIndex; i++)
    {
        char hostname[CF_MAXVARSIZE] = { 0 };
        char keyhash[CF_MAXVARSIZE] = { 0 };

        RandomizeString(WORD_LEN * 3, hostname, sizeof(hostname));
        RandomizeString(WORD_LEN * 3, keyhash, sizeof(keyhash));

        snprintf(work, sizeof(work), "[\"%s\", \"%s\", \"%s\"]\n,", hostname, "10.0.0.100", keyhash);

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]}", bufsize);

    return true;
}

/*****************************************************************************/
/*
 * cfpr_summarize_notkept ( sssllssbll )
 * cfpr_summarize_repaired ( sssllssbll )
*/
JsonElement *Nova2PHP_promiselog_summary_test(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                                              HostClassFilter *hostClassFilter, PageInfo *page)
{
    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    JsonElement *meta = JsonObjectCreate(2);
    {
        JsonObjectAppendInteger(meta, "count", total);
        JsonObjectAppendInteger(meta, "related", 1);

        JsonElement *header = JsonObjectCreate(3);

        JsonObjectAppendInteger(header, "Promise Handle", 0);
        JsonObjectAppendInteger(header, "Report", 1);
        JsonObjectAppendInteger(header, "Occurrences", 2);

        JsonObjectAppendObject(meta, "header", header);
    }

    JsonElement *data = JsonArrayCreate(1000);

    for (int i = startIndex; i < endIndex; i++)
    {
        char handle[CF_MAXVARSIZE];
        char report[CF_MAXVARSIZE];

        RandomizeString(REPORT_LEN, report, sizeof(report));
        RandomizeString(HANDLE_LEN, handle, sizeof(handle));

        JsonElement *entry = JsonArrayCreate(3);

        JsonArrayAppendString(entry, handle);
        JsonArrayAppendString(entry, report);
        JsonArrayAppendInteger(entry, total - i);
    }

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data);

    return output;
}

/*****************************************************************************/
/*
 * cfpr_report_value ( sssssssbll ) 
*/
int Nova2PHP_value_report_test(char *hostkey, char *day, char *month, char *year, HostClassFilter *hostClassFilter,
                               PageInfo *page, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE] = { 0 };
    char header[CF_BUFSIZE] = { 0 };
    //  int margin = 0;
    int headerLen = 0;
    int noticeLen = 0;
    int truncated = false;

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    snprintf(header, sizeof(header), "\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\":{\"Host\":0,\"Day\":1,\"Kept\":2,\"Repaired\":3,\"Not Kept\":4,"
             "\"Note\":{\"index\":5,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}}",
             total);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    char *p = returnval;

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (int i = startIndex; i < endIndex; i++)
    {
        char hostname[CF_MAXVARSIZE];
        char day[CF_MAXVARSIZE];
        char keyhash[CF_MAXVARSIZE];
        char handle[CF_MAXVARSIZE];

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(WORD_LEN, day, sizeof(day));
        RandomizeString(KEYHASH_LEN, keyhash, sizeof(keyhash));
        RandomizeString(HANDLE_LEN, handle, sizeof(handle));

        snprintf(work, sizeof(work),
                 "[\"%s\",\"%s\",%.1lf,%.1lf,%.1lf,"
                 "[\"add\",\"%s\",%d,\"%s\",\"\"]],",
                 hostname,
                 day,
                 (double) (rand() % 100) / 100,
                 (double) (rand() % 100) / 100, (double) (rand() % 100) / 100, keyhash, CFREPORT_VALUE, handle);

        //   margin = headerLen + noticeLen + strlen(work);
        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return true;
}

/*****************************************************************************/
/*
 * cfpr_report_software_in ( sssssbssbll )
 * cfpr_report_patch_in ( sssssbssbll )
 * cfpr_report_patch_avail ( sssssbssbll ) 
*/
int Nova2PHP_software_report_test(char *hostkey, char *name, char *value, char *arch, bool regex, char *type,
                                  HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE] = { 0 };
    char header[CF_BUFSIZE] = { 0 };
    //  int margin = 0;
    int headerLen = 0;
    int noticeLen = 0;
    int truncated = false;

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\": {\"Host\":0,\"Name\":1,\"Version\":2,\"Architecture\":3,\"Last seen\":4" "}", total);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    char *p = returnval;

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (int i = startIndex; i < endIndex; i++)
    {
        char hostname[CF_MAXVARSIZE];
        char name[CF_MAXVARSIZE];
        char version[CF_MAXVARSIZE];

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(HANDLE_LEN, name, sizeof(name));
        RandomizeString(WORD_LEN, version, sizeof(version));

        snprintf(work, sizeof(work), "[\"%s\",\"%s\",\"%s\",\"%s\",%ld],", hostname, name, version, "a", time(NULL));

        //  margin = headerLen + noticeLen + strlen(work);
        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);
    return true;
}

/*****************************************************************************/
/*
 * cfpr_report_vars ( ssssssbssbll )
*/
int Nova2PHP_vars_report_test(const char *hostkey, const char *scope, const char *lval, const char *rval, const char *type, bool regex,
                              const HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE];
    char header[CF_BUFSIZE] = { 0 };
    char lscope[CF_MAXVARSIZE];

    bool first = true;
    bool countadded = false;
    int scope_record_count = 0;
    int noticeLen = 0;
    int headerLen = 0;
    int truncated = false;

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    total = (total > 65000) ? 65000 : total;

    if (endIndex > total)
    {
        endIndex = total;
    }

    lscope[0] = '\0';

    snprintf(header, sizeof(header), "\"meta\":{\"count\":%d,  \"related\" : 1", total);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    char *p = returnval;

    StartJoin(returnval, "{", bufsize);

    int values_per_scope = 0;

    for (int i = startIndex; i < endIndex; i++)
    {
        if (scope_record_count >= values_per_scope)
        {
            if (strlen(work) > 0)
            {
                returnval[strlen(returnval) - 1] = '\0';
                snprintf(work, CF_BUFSIZE, "],\"count\":%d},", scope_record_count);
                Join(returnval, work, bufsize); // end scope      
                scope_record_count = 0;

                int max_values_per_scope = ((endIndex / 4) >= 1) ? (endIndex / 4) : 1;

                values_per_scope = rand() % max_values_per_scope;       // random variables per scope
                first = false;
            }

            char scope[CF_MAXVARSIZE];

            RandomizeString(HANDLE_LEN, scope, sizeof(scope));

            snprintf(work, CF_BUFSIZE, "\"%s\":{"
                     "\"header\":{\"Host\":0,\"Type\":1,\"Name\":2,\"Value\":3,\"Last seen\":4}," "\"data\":[", scope);
            Join(returnval, work, bufsize);
        }

        char hostname[CF_MAXVARSIZE];
        char lval[CF_MAXVARSIZE];
        char rval[CF_MAXVARSIZE];

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(HANDLE_LEN, lval, sizeof(lval));
        RandomizeString(WORD_LEN * 3, rval, sizeof(rval));

        snprintf(work, CF_BUFSIZE, "[\"%s\",\"%s\",\"%s\",\"%s\",%ld],", hostname, "string", lval, rval, time(NULL));

        //  margin = headerLen + noticeLen + strlen(work);

        p = strcatUnsafe(p, work);

        scope_record_count++;
    }

    if (!countadded)
    {
        returnval[strlen(returnval) - 1] = '\0';
        snprintf(work, CF_BUFSIZE, "],\"count\":%d}", scope_record_count);
        Join(returnval, work, bufsize); // end scope
    }
    else if (first)
    {
        returnval[strlen(returnval) - 1] = ']';
    }
    else
    {
        returnval[strlen(returnval) - 1] = '\0';
    }

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_report_test(char *hostkey, char *version, time_t t, int k, int nk, int rep,
                                    HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE];

    char header[CF_BUFSIZE] = { 0 };
    //  int margin = 0;
    int headerLen = 0;
    int noticeLen = 0;
    int truncated = false;

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\": {\"Host\":0,\"Policy\":1,\"Kept\":2,\"Repaired\":3,\"Not Kept\":4,\"Last seen\":5"
             "}", total);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    char *p = returnval;

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (int i = startIndex; i < endIndex; i++)
    {
        char hostname[CF_MAXVARSIZE];
        char version[CF_MAXVARSIZE];

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(WORD_LEN * 4, version, sizeof(version));

        snprintf(work, sizeof(work),
                 "[\"%s\",\"%s\",%d,%d,%d,%ld],",
                 hostname, version, (rand() % 100) / 100, (rand() % 100) / 100, (rand() % 100) / 100, time(NULL));

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return true;
}

/*****************************************************************************/

int Nova2PHP_compliance_promises_test(char *hostkey, char *handle, char *status, bool regex,
                                      HostClassFilter *hostClassFilter, PageInfo *page, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE];
    char header[CF_BUFSIZE] = { 0 };
    //  int margin = 0;
    int headerLen = 0;
    int noticeLen = 0;
    int truncated = false;

    int startIndex = page->resultsPerPage * (page->pageNum - 1);
    int endIndex = page->resultsPerPage * page->pageNum;
    int total = GetTotalCount(endIndex);

    if (endIndex > total)
    {
        endIndex = total;
    }

    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d, \"related\" : 1, "
             "\"header\": {\"Host\":0,\"Promise Handle\":1,\"Last Known State\":2,\"Probability Kept\":3,\"Uncertainty\":4,\"Last seen\":5"
             "}", total);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);

    char *p = returnval;

    StartJoin(returnval, "{\"data\":[", bufsize);

    for (int i = startIndex; i < endIndex; i++)
    {
        char hostname[CF_MAXVARSIZE];
        char handle[CF_MAXVARSIZE];

        RandomizeString(HOSTNAME_LEN, hostname, sizeof(hostname));
        RandomizeString(HANDLE_LEN, handle, sizeof(handle));

        snprintf(work, sizeof(work),
                 "[\"%s\",\"%s\",\"%s\",%.2lf,%.2lf,%ld],",
                 hostname,
                 handle,
                 "Compliant",
                 (double) (rand() % 100) / 100, (double) (rand() % 100) / 100, (long) (rand() % 100) / 100);

        p = strcatUnsafe(p, work);
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, work, sizeof(work) - 1);

    Join(returnval, work, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    return true;
}

/*****************************************************************************/

int GetTotalCount(int endIndex)
{
    const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");

    return ((total_env == NULL) ? endIndex : atoi(total_env));
}

/*****************************************************************************/
