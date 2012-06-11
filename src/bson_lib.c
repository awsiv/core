/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"
#include "files_names.h"
#include "item_lib.h"

Item *BsonGetStringArrayAsItemList(const bson *b, const char *key)
// TODO: Deprecate in favour of BsonStringArrayAsRlist()
{
    bson array;

    if(!BsonGetArrayValue(b, key, &array))
    {
        return NULL;
    }

    bson_iterator it;

    bson_iterator_init(&it, &array);

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
    bson array;

    if(!BsonGetArrayValue(b, key, &array))
    {
        return NULL;
    }

    bson_iterator it;

    bson_iterator_init(&it, &array);

    Rlist *values = NULL;

    while (bson_iterator_next(&it))
    {
        if (bson_iterator_type(&it) == BSON_STRING)
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

    if (bson_find(&it, b, key) == BSON_INT)
    {
        *out = bson_iterator_int(&it);
        return true;
    }

    CfOut(cf_verbose, "", "BsonIntGet: No match for \"%s\"", key);
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

    if (bson_find(&it, b, key) == BSON_ARRAY)
    {
        *out = bson_iterator_value(&it);
        return true;
    }
    else
    {
        return false;
    }
}

bool BsonObjectGet(const bson *b, const char *key, bson **out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_OBJECT)
    {
        bson_iterator_subobject(&it, *out);
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
    bson array;

    if(!BsonGetArrayValue(b, key, &array))
    {
        return true;
    }

    bson_iterator it;

    bson_iterator_init(&it, &array);

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

    if (bson_find(&it, b, key) == BSON_LONG)
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

    if (bson_find(&it, b, key) == BSON_STRING)
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

    if (bson_find(&it, b, key) == BSON_BOOL)
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

    if (bson_find(&it, b, key) == BSON_STRING)
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

bool BsonGetArrayValue(const bson *b, const char *key, bson *sub)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_ARRAY)
    {
        bson_iterator_subobject(&it, sub);
        return true;
    }

    CfOut(cf_verbose, "", "BsonGetArrayValue: No match for \"%s\"", key);

    return false;
}

/*****************************************************************************/

bool BsonAppendStringSafe(bson *b, const char *key, char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return false;
    }

    if(bson_append_string(b, key, value) == BSON_OK)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

bool BsonAppendRegexSafe(bson *bb, const char *key, char *rxValue)
{
    if (rxValue == NULL || rxValue[0] == '\0')
    {
        return false;
    }

    char anchoredRx[CF_MAXVARSIZE];

    AnchorRegex(rxValue, anchoredRx, sizeof(anchoredRx));

    if (bson_append_regex(bb, key, anchoredRx, "") == BSON_OK)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

void BsonAppendStringArray(bson *b, char *arrayName, Item *arrayValues)
{    
    int i = 0;
    char iStr[32];

    Item *ip;

    {
        bson_append_start_array(b, arrayName);

        for (ip = arrayValues; ip != NULL; ip = ip->next, i++)
        {
            snprintf(iStr, sizeof(iStr), "%d", i);
            bson_append_string(b, iStr, ip->name);
        }

        bson_append_finish_object(b);
    }
}

/*****************************************************************************/

bool BsonAppendHostClassFilter(bson *queryBuffer, HostClassFilter *filter)
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

bool BsonAppendIncludeList(bson *queryBuffer, char *includeKey, Rlist *includeValues)
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

void BsonAppendArrayRx(bson *buffer, const char *key, Rlist *rx_values)
{    
    int i = 0;

    {
        bson_append_start_array(buffer, key);
        for (Rlist *rp = rx_values; rp != NULL; rp = rp->next, i++)
        {
            char anchoredRx[CF_BUFSIZE] = { 0 };
            AnchorRegex(ScalarValue(rp), anchoredRx, sizeof(anchoredRx));

            char index_str[32] = { 0 };
            snprintf(index_str, sizeof(index_str), "%d", i);
            bson_append_regex(buffer, index_str, anchoredRx, "");
        }

        bson_append_finish_object(buffer);
    }
}

bool BsonAppendIncludeRxList(bson *query, char *include_key, Rlist *include_rx_values)
{
    if (!include_rx_values)
    {
        return false;
    }

    {
        bson_append_start_object(query, include_key);
        BsonAppendArrayRx(query, "$all", include_rx_values);
        bson_append_finish_object(query);
    }
    return true;
}

bool BsonAppendExcludeRxList(bson *query, char *exclude_key, Rlist *exclude_rx_values)
{
    if (!exclude_rx_values)
    {
        return false;
    }

    {
        bson_append_start_object(query, exclude_key);
        {
            bson_append_start_object(query, "$not");

            BsonAppendArrayRx(query, "$all", exclude_rx_values);

            bson_append_finish_object(query);
        }
        bson_append_finish_object(query);
    }
    return true;
}

/*****************************************************************************/

bool BsonAppendExcludeList(bson *query, char *excludeKey, Rlist *excludeValues)
{
    if (!excludeValues)
    {
        return false;
    }

    {
        bson_append_start_object(query, excludeKey);
        {
            bson_append_start_array(query, "$not");

            for (Rlist *rp = excludeValues; rp != NULL; rp = rp->next)
            {
                bson_append_string(query, excludeKey, ScalarValue(rp));
            }

            bson_append_finish_object(query);
        }
        bson_append_finish_object(query);
    }
    return true;
}

/*****************************************************************************/

void BsonAppendAddToSetString(bson *b, char *key, char *value)
{
    bson_append_start_object(b, "$addToSet");
    bson_append_string(b, key, value);
    bson_append_finish_object(b);
}

/*****************************************************************************/

void BsonAppendRecentQuery(bson *query, int maxAgeInSeconds)
{
    time_t currentTimeStamp = time(NULL);
    time_t minTimeStamp = currentTimeStamp - maxAgeInSeconds;

    {
        bson_append_start_object(query, cfr_time);

        bson_append_int(query, "$gte", minTimeStamp);

        bson_append_finish_object(query);
    }
}

/*****************************************************************************/

void BsonAppendAgedQuery(bson *query, int maxAgeInSeconds)
/*
 * timestamp not there or older than maxAgeInSeconds
 */
{
    time_t currentTimeStamp = time(NULL);
    time_t minTimeStamp = currentTimeStamp - maxAgeInSeconds;

    bson_append_start_array(query, "$or");
    {
        {
            bson_append_start_object(query, "0");
            bson_append_null(query, cfr_time);
            bson_append_finish_object(query);
        }

        {
            bson_append_start_object(query, "1");
            {
                bson_append_start_object(query, cfr_time);
                bson_append_int(query, "$lt", minTimeStamp);
                bson_append_finish_object(query);
            }
            bson_append_finish_object(query);
        }
    }
    bson_append_finish_object(query);
}

/*****************************************************************************/

void BsonToString(char *retBuf, int retBufSz, bson *data)
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

        bson sub;

        switch (t)
        {
        case BSON_INT:
            snprintf(buf, sizeof(buf), "%d", bson_iterator_int(&i));
            break;

        case BSON_DOUBLE:
            snprintf(buf, sizeof(buf), "%f", bson_iterator_double(&i));
            break;

        case BSON_BOOL:
            snprintf(buf, sizeof(buf), "%s", bson_iterator_bool(&i) ? "true" : "false");
            break;

        case BSON_STRING:
            snprintf(buf, sizeof(buf), "%s", bson_iterator_string(&i));
            break;

        case BSON_REGEX:
            snprintf(buf, sizeof(buf), "/%s/", bson_iterator_string(&i));
            break;

        case BSON_NULL:
            snprintf(buf, sizeof(buf), "null");
            break;

        case BSON_OID:
            bson_oid_to_string(bson_iterator_oid(&i), oidhex);
            snprintf(buf, sizeof(buf), "%s", oidhex);
            break;

        case BSON_OBJECT:
            buf[0] = '{';

            bson_iterator_subobject(&i, &sub);

            BsonToString(buf + 1, sizeof(buf - 1), &sub);
            EndJoin(buf, "}", sizeof(buf));
            break;

        case BSON_ARRAY:

            buf[0] = '[';
            bson_iterator_subobject(&i, &sub);

            BsonToString(buf + 1, sizeof(buf - 1), &sub);
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

void BsonAppendHostColourFilter(bson *query, HostColourFilter *filter)
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
        {
            bson_append_start_array(query, "$or");
            {
                bson_append_start_object(query, "0");
                {
                    bson_append_start_object(query, cfr_day);
                    bson_append_long(query, "$lt", filter->blue_time_horizon);
                    bson_append_finish_object(query);
                }
                bson_append_finish_object(query);
            }

            {
                bson_append_start_object(query, "1");
                {
                    bson_append_start_object(query, cfr_day);
                    bson_append_bool(query, "$exists", false);
                    bson_append_finish_object(query);
                }
                bson_append_finish_object(query);
            }

            {
                bson_append_start_object(query, "2");
                {
                    bson_append_start_object(query, score_method);
                    bson_append_bool(query, "$exists", false);
                    bson_append_finish_object(query);
                }
                bson_append_finish_object(query);
            }

            {
                bson_append_start_object(query, "3");
                {
                    bson_append_start_object(query, cfr_is_black);
                    bson_append_bool(query, "$exists", false);
                    bson_append_finish_object(query);
                }
                bson_append_finish_object(query);
            }
            bson_append_finish_object(query);
        }
    }

    if (filter->colour == HOST_COLOUR_GREEN)
    {
        bson_append_start_object(query, score_method);
        bson_append_int(query, "$lt", CF_AMBER_THRESHOLD);
        bson_append_finish_object(query);

        bson_append_start_object (query, cfr_day);
        bson_append_long(query, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query);

        bson_append_bool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_YELLOW)
    {
        bson_append_start_object(query, score_method);
        bson_append_int(query, "$gte", CF_AMBER_THRESHOLD);
        bson_append_int(query, "$lt", CF_RED_THRESHOLD);
        bson_append_finish_object(query);

        bson_append_start_object (query, cfr_day);
        bson_append_long(query, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query);

        bson_append_bool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_RED)
    {
        bson_append_start_object(query, score_method);
        bson_append_int(query, "$gte", CF_RED_THRESHOLD);
        bson_append_finish_object(query);

        bson_append_start_object (query, cfr_day);
        bson_append_long(query, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query);

        bson_append_bool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_GREEN_YELLOW_RED) // !BLUE
    {
        bson_append_start_object(query, cfr_day);
        bson_append_long(query, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query);

        bson_append_bool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_BLACK)
    {
        bson_append_start_object (query, cfr_day);
        bson_append_long(query, "$gte", filter->blue_time_horizon);
        bson_append_finish_object(query);

        bson_append_bool(query, cfr_is_black, true);
    }

    free(score_method);
}

/*****************************************************************************/

void BsonAppendSortField(bson *bb, char *sortField)
/* 
 * usage:
 * bson_init(&bb);
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
    bson_append_start_object(bb, "$query");
    bson_append_finish_object(bb);

    bson_append_start_object(bb, "$orderby");
    bson_append_int(bb, sortField, -1);
    bson_append_finish_object(bb);
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
