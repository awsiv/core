/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"
#include "files_names.h"

Item *BsonGetStringArrayAsItemList(const bson *b, const char *key)
// TODO: Deprecate in favour of BsonStringArrayAsRlist()
{
    const char *array = BsonGetArrayValue(b, key);

    if (!array)
    {
        return NULL;
    }

    bson_iterator it;

    bson_iterator_init(&it, array);

    Item *values = NULL;

    while (bson_iterator_next(&it))
    {
        PrependItem(&values, bson_iterator_string(&it), NULL);
    }

    return values;
}

/*****************************************************************************/

Rlist *BsonStringArrayAsRlist(const bson *b, const char *key)
{
    const char *array = BsonGetArrayValue(b, key);

    if (!array)
    {
        return NULL;
    }

    bson_iterator it;

    bson_iterator_init(&it, array);

    Rlist *values = NULL;

    while (bson_iterator_next(&it))
    {
        if (bson_iterator_type(&it) == bson_string)
        {
            // NOTE: preserve ordering (don't prepend)
            AppendRScalar(&values, (char *) bson_iterator_string(&it), CF_SCALAR);
        }
        else
        {
            CfOut(cf_error, "BsonStringArrayAsRlist: a value in array \"%s\" is not a string, but type=%d",
                  key, bson_iterator_type(&it));
        }
    }

    return values;
}

/*****************************************************************************/

bool BsonIntGet(const bson *b, const char *key, int *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_int)
    {
        *out = bson_iterator_int(&it);
        return true;
    }

    CfOut(cf_verbose, "", "BsonFindInt: No match for \"%s\"", key);
    return false;
}

bool BsonTimeGet(const bson *b, const char *key, time_t *out)
{
    int value = 0;
    if (BsonIntGet(b, key, &value))
    {
        *out = (time_t)value;
        return true;
    }
    else
    {
        return false;
    }
}

bool BsonArrayGet(const bson *b, const char *key, const char **out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_array)
    {
        *out = bson_iterator_value(&it);
        return true;
    }
    else
    {
        return false;
    }
}

bool BsonObjectGet(const bson *b, const char *key, const char **out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_object)
    {
        *out = bson_iterator_value(&it);
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/
bool BsonIsArrayNonExistentOrEmpty(const bson *b, const char *key)
{
    const char *array = BsonGetArrayValue(b, key);

    if (!array)
    {
        return true;
    }

    bson_iterator it;

    bson_iterator_init(&it, array);

    while (bson_iterator_next(&it))
    {
        return false;
    }

    return true;
}
/*****************************************************************************/

long BsonLongGet(const bson *b, const char *key)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_long)
    {
        return bson_iterator_long(&it);
    }

    CfOut(cf_verbose, "", "BsonLongGet: No match for \"%s\"", key);

    return 0;
}

/*****************************************************************************/

bool BsonStringGet(const bson *b, const char *key, const char **out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_string)
    {
        *out = bson_iterator_string(&it);
        return true;
    }

    CfOut(cf_verbose, "", "BsonStringGet: No match for \"%s\"", key);

    return false;
}
/*****************************************************************************/

bool BsonBoolGet(const bson *b, const char *key, bool *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_bool)
    {
        *out = (int)bson_iterator_bool(&it);
        return true;
    }

    CfOut(cf_verbose, "", "BsonBoolGet: No match for \"%s\"", key);

    return false;
}

/*****************************************************************************/

void BsonStringWrite(char *dest, int destSz, const bson *b, const char *key)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_string)
    {
        snprintf(dest, destSz, "%s", bson_iterator_string(&it));
    }
    else
    {
        dest[0] = '\0';
        CfOut(cf_verbose, "", "BsonStringWrite: No match for \"%s\"", key);
    }
}

/*****************************************************************************/

const char *BsonGetArrayValue(const bson *b, const char *key)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == bson_array)
    {
        return bson_iterator_value(&it);
    }

    CfOut(cf_verbose, "", "BsonGetArrayValue: No match for \"%s\"", key);

    return NULL;
}

/*****************************************************************************/

bool BsonAppendStringSafe(bson_buffer *bb, char *key, char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return false;
    }

    bson_append_string(bb, key, value);

    return true;
}

/*****************************************************************************/

bool BsonAppendRegexSafe(bson_buffer *bb, char *key, char *rxValue)
{
    if (rxValue == NULL || rxValue[0] == '\0')
    {
        return false;
    }

    char anchoredRx[CF_MAXVARSIZE];

    AnchorRegex(rxValue, anchoredRx, sizeof(anchoredRx));

    bson_append_regex(bb, key, anchoredRx, "");

    return true;
}

/*****************************************************************************/

void BsonAppendStringArray(bson_buffer *bb, char *arrayName, Item *arrayValues)
{
    bson_buffer *arr = bson_append_start_array(bb, arrayName);
    int i = 0;
    char iStr[32];

    Item *ip;

    for (ip = arrayValues; ip != NULL; ip = ip->next, i++)
    {
        snprintf(iStr, sizeof(iStr), "%d", i);
        bson_append_string(bb, iStr, ip->name);
    }

    bson_append_finish_object(arr);
}

/*****************************************************************************/

bool BsonAppendHostClassFilter(bson_buffer *queryBuffer, HostClassFilter *filter)
{
    if (filter == NULL)
    {
        return false;
    }

    bool modified = false;

    modified |= BsonAppendIncludeRxList(queryBuffer, cfr_class_keys, filter->classRxIncludes);
    modified |= BsonAppendExcludeRxList(queryBuffer, cfr_class_keys, filter->classRxExcludes);

    return modified;
}

/*****************************************************************************/

bool BsonAppendIncludeList(bson_buffer *queryBuffer, char *includeKey, Rlist *includeValues)
{
    if (!includeValues)
    {
        return false;
    }

    for (Rlist *rp = includeValues; rp; rp = rp->next)
    {
        bson_append_string(queryBuffer, includeKey, ScalarValue(rp));
    }

    return true;
}

/*****************************************************************************/

void BsonAppendArrayRx(bson_buffer *buffer, const char *key, Rlist *rx_values)
{
    bson_buffer *array = bson_append_start_array(buffer, key);

    int i = 0;
    for (Rlist *rp = rx_values; rp != NULL; rp = rp->next, i++)
    {
        char anchoredRx[CF_BUFSIZE] = { 0 };
        AnchorRegex(ScalarValue(rp), anchoredRx, sizeof(anchoredRx));

        char index_str[32] = { 0 };
        snprintf(index_str, sizeof(index_str), "%d", i);
        bson_append_regex(array, index_str, anchoredRx, "");
    }

    bson_append_finish_object(array);
}

bool BsonAppendIncludeRxList(bson_buffer *query_buffer, char *include_key, Rlist *include_rx_values)
{
    if (!include_rx_values)
    {
        return false;
    }

    bson_buffer *include_class_query = bson_append_start_object(query_buffer, include_key);
    BsonAppendArrayRx(include_class_query, "$all", include_rx_values);
    bson_append_finish_object(include_class_query);

    return true;
}

bool BsonAppendExcludeRxList(bson_buffer *query_buffer, char *exclude_key, Rlist *exclude_rx_values)
{
    if (!exclude_rx_values)
    {
        return false;
    }

    bson_buffer *exclude_class_buffer = bson_append_start_object(query_buffer, exclude_key);
    {
        bson_buffer *not_buffer = bson_append_start_object(exclude_class_buffer, "$not");
        BsonAppendArrayRx(not_buffer, "$all", exclude_rx_values);
        bson_append_finish_object(not_buffer);
    }
    bson_append_finish_object(exclude_class_buffer);

    return true;
}

/*****************************************************************************/

bool BsonAppendExcludeList(bson_buffer *queryBuffer, char *excludeKey, Rlist *excludeValues)
{
    if (!excludeValues)
    {
        return false;
    }

    bson_buffer *excludeClassBuffer = bson_append_start_object(queryBuffer, excludeKey);
    bson_buffer *excludeClassArray = bson_append_start_array(excludeClassBuffer, "$not");

    for (Rlist *rp = excludeValues; rp != NULL; rp = rp->next)
    {
        bson_append_string(excludeClassArray, excludeKey, ScalarValue(rp));
    }

    bson_append_finish_object(excludeClassArray);
    bson_append_finish_object(excludeClassBuffer);

    return true;
}

/*****************************************************************************/

void BsonAppendAddToSetString(bson_buffer *bb, char *key, char *value)
{
    bson_append_start_object(bb, "$addToSet");
    bson_append_string(bb, key, value);
    bson_append_finish_object(bb);
}

/*****************************************************************************/

void BsonAppendRecentQuery(bson_buffer *querybuf, int maxAgeInSeconds)
{
    time_t currentTimeStamp = time(NULL);
    time_t minTimeStamp = currentTimeStamp - maxAgeInSeconds;

    bson_buffer *sub = bson_append_start_object(querybuf, cfr_time);

    bson_append_int(sub, "$gte", minTimeStamp);
    bson_append_finish_object(sub);
}

/*****************************************************************************/

void BsonAppendAgedQuery(bson_buffer *querybuf, int maxAgeInSeconds)
/*
 * timestamp not there or older than maxAgeInSeconds
 */
{
    time_t currentTimeStamp = time(NULL);
    time_t minTimeStamp = currentTimeStamp - maxAgeInSeconds;

    bson_buffer *or = bson_append_start_array(querybuf, "$or");

    bson_buffer *sub1, *sub2;

    sub1 = bson_append_start_object(or, "0");
    bson_append_null(sub1, cfr_time);
    bson_append_finish_object(sub1);

    sub1 = bson_append_start_object(or, "1");
    sub2 = bson_append_start_object(sub1, cfr_time);
    bson_append_int(sub2, "$lt", minTimeStamp);
    bson_append_finish_object(sub2);
    bson_append_finish_object(sub1);

    bson_append_finish_object(or);
}

/*****************************************************************************/

void BsonToString(char *retBuf, int retBufSz, char *data)
/* data = (bson *)b->data*/
{
    bson_iterator i;
    const char *key;
    char oidhex[25];
    char buf[CF_BUFSIZE];

    memset(retBuf, 0, retBufSz);
    bson_iterator_init(&i, data);

    while (bson_iterator_next(&i))
    {

        bson_type t = bson_iterator_type(&i);

        if (t == 0)
        {
            break;
        }

        key = bson_iterator_key(&i);

        snprintf(buf, sizeof(buf), "%s : ", key);
        Join(retBuf, buf, retBufSz);

        switch (t)
        {
        case bson_int:
            snprintf(buf, sizeof(buf), "%d", bson_iterator_int(&i));
            break;

        case bson_double:
            snprintf(buf, sizeof(buf), "%f", bson_iterator_double(&i));
            break;

        case bson_bool:
            snprintf(buf, sizeof(buf), "%s", bson_iterator_bool(&i) ? "true" : "false");
            break;

        case bson_string:
            snprintf(buf, sizeof(buf), "%s", bson_iterator_string(&i));
            break;

        case bson_regex:
            snprintf(buf, sizeof(buf), "/%s/", bson_iterator_string(&i));
            break;

        case bson_null:
            snprintf(buf, sizeof(buf), "null");
            break;

        case bson_oid:
            bson_oid_to_string(bson_iterator_oid(&i), oidhex);
            snprintf(buf, sizeof(buf), "%s", oidhex);
            break;

        case bson_object:
            buf[0] = '{';
            BsonToString(buf + 1, sizeof(buf - 1), (char *) bson_iterator_value(&i));
            EndJoin(buf, "}", sizeof(buf));
            break;

        case bson_array:

            buf[0] = '[';
            BsonToString(buf + 1, sizeof(buf - 1), (char *) bson_iterator_value(&i));
            EndJoin(buf, "]", sizeof(buf));
            break;

        default:
            snprintf(buf, sizeof(buf), "can't print type : %d\n", t);
        }

        Join(retBuf, buf, retBufSz);
        Join(retBuf, ", ", retBufSz);
    }

    retBuf[strlen(retBuf) - 2] = 0;     // clear last comma
}

/*****************************************************************************/

void BsonAppendHostColourFilter(bson_buffer *query_buffer, HostColourFilter *filter)
{
    if (filter == NULL)
    {
        return;
    }

    char *score_method = NULL;
    switch (filter->method)
    {
        case HOST_RANK_METHOD_COMPLIANCE:
            xasprintf(&score_method, "%s", cfr_score_comp);
            break;

        case HOST_RANK_METHOD_ANOMALY:
            xasprintf(&score_method, "%s", cfr_score_anom);
            break;

        case HOST_RANK_METHOD_PERFORMANCE:
            xasprintf(&score_method, "%s", cfr_score_perf);
            break;

        case HOST_RANK_METHOD_LASTSEEN:
            xasprintf(&score_method, "%s", cfr_score_lastseen);
            break;

        case HOST_RANK_METHOD_MIXED:
            xasprintf(&score_method, "%s", cfr_score_mixed);
            break;
    }

    if (filter->colour == HOST_COLOUR_BLUE) // blue overwrites black status
    {
        bson_buffer *arr = bson_append_start_array(query_buffer, "$or");

        bson_append_start_object(arr, "0");
        bson_append_start_object(arr, cfr_day);
        bson_append_long(arr, "$lt", filter->blue_time_horizon);
        bson_append_finish_object(arr);
        bson_append_finish_object(arr);

        bson_append_start_object(arr, "1");
        bson_append_start_object(arr, cfr_day);
        bson_append_bool(arr, "$exists", false);
        bson_append_finish_object(arr);
        bson_append_finish_object(arr);

        bson_append_start_object(arr, "2");
        bson_append_start_object(arr, score_method);
        bson_append_bool(arr, "$exists", false);
        bson_append_finish_object(arr);
        bson_append_finish_object(arr);

        bson_append_start_object(arr, "3");
        bson_append_start_object(arr, cfr_is_black);
        bson_append_bool(arr, "$exists", false);
        bson_append_finish_object(arr);
        bson_append_finish_object(arr);

        bson_append_finish_object(query_buffer);
    }

    if (filter->colour == HOST_COLOUR_GREEN)
    {
        bson_append_start_object(query_buffer, score_method);
        bson_append_int(query_buffer, "$lt", CF_AMBER_THRESHOLD);
        bson_append_finish_object(query_buffer);

        bson_append_start_object (query_buffer, cfr_day);
        bson_append_long(query_buffer, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query_buffer);

        bson_append_bool(query_buffer, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_YELLOW)
    {
        bson_append_start_object(query_buffer, score_method);
        bson_append_int(query_buffer, "$gte", CF_AMBER_THRESHOLD);
        bson_append_int(query_buffer, "$lt", CF_RED_THRESHOLD);
        bson_append_finish_object(query_buffer);

        bson_append_start_object (query_buffer, cfr_day);
        bson_append_long(query_buffer, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query_buffer);

        bson_append_bool(query_buffer, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_RED)
    {
        bson_append_start_object(query_buffer, score_method);
        bson_append_int(query_buffer, "$gte", CF_RED_THRESHOLD);
        bson_append_finish_object(query_buffer);

        bson_append_start_object (query_buffer, cfr_day);
        bson_append_long(query_buffer, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query_buffer);

        bson_append_bool(query_buffer, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_GREEN_YELLOW_RED) // !BLUE
    {
        bson_append_start_object(query_buffer, cfr_day);
        bson_append_long(query_buffer, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query_buffer);

        bson_append_bool(query_buffer, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_BLACK)
    {
        bson_append_start_object (query_buffer, cfr_day);
        bson_append_long(query_buffer, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query_buffer);

        bson_append_bool(query_buffer, cfr_is_black, true);
    }

    free(score_method);
}

/*****************************************************************************/

void BsonAppendSortField(bson_buffer *bb, char *sortField)
/* 
 * usage:
 * bson_buffer_init(&bb);
 * ... append query fields ...
 * bson_append_string(&bb, somekey, someconstraint);
 * ...
 * BsonAppendSortField(&bb, fieldtosorton);
 * bson_from_buffer(&query, &bb);
 *
 * Supports sorting ascending on one field currently.
 * Works with libmongoc v. 0.2, may need adjustment for newer versions.
 */
{
    bson query;
    bson_from_buffer(&query, bb);

    bson_buffer_init(bb);
    bson_append_bson(bb, "$query", &query);
    bson_destroy(&query);
  
    bson_buffer *sort = bson_append_start_object(bb, "$orderby");
    bson_append_int(sort, sortField, -1);
    bson_append_finish_object(sort);
}

/*****************************************************************************/
bool BsonIsKeyCorrupt(const char *key)
{
    if(NULL_OR_EMPTY(key))
    {
        return true;
    }

    // fieldname cannot start with a '$'
    if(key[0] == '$')
    {
        return true;
    }

    const char *sp;
    // fieldname must not contain '.'
    for (sp = key; *sp != '\0'; sp++)
    {
        if (*sp == '.')
        {
            return true;
        }
    }

    return false;
}

/*****************************************************************************/
bool BsonIsEmpty(const bson *object)
{
    if (bson_size(object) > 5)
    {
        return false;
    }
    return true;
}
