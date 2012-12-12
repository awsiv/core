#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"

#include "string_lib.h"
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

int GetTotalCount(int endIndex)
{
    const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");

    return ((total_env == NULL) ? endIndex : atoi(total_env));
}

/*****************************************************************************/