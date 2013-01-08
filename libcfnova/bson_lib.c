/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"

#include "files_names.h"
#include "item_lib.h"
#include "json.h"
#include "matching.h"
#include "cfstream.h"
#include "string_lib.h"

#include <assert.h>

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

bool BsonDoubleGet(const bson *b, const char *key, double *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_DOUBLE)
    {
        *out = bson_iterator_double(&it);
        return true;
    }

    return false;
}

bool BsonArrayGet(const bson *b, const char *key, bson *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_ARRAY)
    {
        bson_iterator_subobject(&it, out);
        return true;
    }
    else
    {
        return false;
    }
}

bool BsonObjectGet(const bson *b, const char *key, bson *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_OBJECT)
    {
        bson_iterator_subobject(&it, out);
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

bool BsonBoolGetCheckExists(const bson *b, const char *key, bool *out)
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_BOOL)
    {
        *out = (bool)bson_iterator_bool(&it);
        return true;
    }

    CfOut(cf_verbose, "", "BsonBoolGet: No match for \"%s\"", key);

    return false;
}

/*****************************************************************************/

bool BsonBoolGet( const bson *b, const char *key )
{
    bson_iterator it;

    if (bson_find(&it, b, key) == BSON_BOOL)
    {
        return (bool) bson_iterator_bool(&it);
    }

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

bool BsonAppendNull( bson *b, const char *key )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int retval = bson_append_null( b, key );

    assert( retval == BSON_OK );

    return ( retval == BSON_OK );
}

/*****************************************************************************/


bool BsonAppendBool( bson *b, const char *key, const bool value )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int retval = bson_append_bool( b, key, value );

    assert( retval == BSON_OK );

    return ( retval == BSON_OK );
}

/*****************************************************************************/

bool BsonAppendInt( bson *b, const char *key, int value )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int retval = bson_append_int( b, key, value );

    assert( retval == BSON_OK );

    return ( retval == BSON_OK );
}

/*****************************************************************************/

bool BsonAppendDouble( bson *b, const char *key, double value )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int retval = bson_append_double( b, key, value );

    assert( retval == BSON_OK );

    return ( retval == BSON_OK );
}

/*****************************************************************************/

bool BsonAppendLong( bson *b, const char *key, long value )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int retval = bson_append_long( b, key, value );

    assert( retval == BSON_OK );

    return ( retval == BSON_OK );
}

/*****************************************************************************/

bool BsonAppendStringAt( bson *b, int pos, const char *value )
{
    assert( b );
    assert( !b->finished );
    assert( pos >= 0 );
    assert( value );

    char index_str[32];
    snprintf( index_str, sizeof(index_str), "%d", pos );

    return BsonAppendString( b, index_str, value );
}

/*****************************************************************************/

bool BsonAppendStringNonEmpty(bson *b, const char *key, const char *value)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    if (NULL_OR_EMPTY(value))
    {
        return false;
    }

    return BsonAppendString(b, key, value);
}

/*****************************************************************************/

bool BsonAppendString(bson *b, const char *key, const char *value)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    if( !value )
    {
        return false;
    }

    int ret = bson_append_string(b, key, value);

    assert( ret == BSON_OK);

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendRegex(bson *b, const char *key, const char *pattern, const char *opts)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    if( !pattern || !opts )
    {
        return false;
    }

    int ret = bson_append_regex( b, key, pattern, opts );

    assert( ret == BSON_OK);

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendStartObject( bson *b, const char *key )
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int ret = bson_append_start_object( b, key );

    assert( ret == BSON_OK );

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendStartArray( bson *b, const char *key)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    int ret = bson_append_start_array( b, key );

    assert( ret == BSON_OK );

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendFinishObject( bson *b )
{
    assert( b );
    assert( !b->finished );

    int ret = bson_append_finish_object( b );

    assert( ret == BSON_OK );

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendFinishArray( bson *b )
{
    assert( b );
    assert( !b->finished );

    int ret = bson_append_finish_array( b );

    assert( ret == BSON_OK );

    return ret == BSON_OK;
}

/*****************************************************************************/

bool BsonAppendRegexNonEmpty(bson *b, const char *key, char *rx_value)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    if (NULL_OR_EMPTY(rx_value))
    {
        return false;
    }

    char anchored_rx[CF_MAXVARSIZE];

    AnchorRegex(rx_value, anchored_rx, sizeof(anchored_rx));

    return BsonAppendRegex(b, key, anchored_rx, "");
}

/*****************************************************************************/

void BsonAppendStringArray(bson *b, char *array_name, Item *array_values)
{
    assert( b );
    assert( !b->finished );
    assert( array_name );

    {
        BsonAppendStartArray( b, array_name );

        int i = 0;
        char index_str[32];

        for (Item *ip = array_values; ip != NULL; ip = ip->next, i++)
        {
            snprintf(index_str, sizeof(index_str), "%d", i);
            BsonAppendString( b, index_str, ip->name );
        }

        BsonAppendFinishArray( b );
    }
}

/*****************************************************************************/

void BsonAppendStringArrayRlist(bson *b, const char *key, const Rlist *string_rlist)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    {
        BsonAppendStartArray( b, key );

        int i = 0;
        char index_str[32];

        for (const Rlist *rp = string_rlist; rp; rp = rp->next, i++)
        {
            snprintf(index_str, sizeof(index_str), "%d", i);
            BsonAppendString( b, index_str, ScalarValue(rp) );
        }

        BsonAppendFinishArray( b );
    }
}

/*****************************************************************************/

void BsonAppendStringArraySequence(bson *b, const char *key, const Sequence *string_seq)
{
    assert( b );
    assert( !b->finished );
    assert( key );
    assert( string_seq );

    {
        BsonAppendStartArray( b, key );

        for (int i = 0; i < string_seq->length; i++)
        {
            char index_str[32] = { 0 };
            snprintf(index_str, sizeof(index_str), "%d", i);
            BsonAppendString( b, index_str, string_seq->data[i] );
        }

        BsonAppendFinishArray( b );
    }
}

/*****************************************************************************/

bool BsonAppendHostClassFilter(bson *query, const HostClassFilter *filter)
{
    assert( query );
    assert( !query->finished );

    if (filter == NULL)
    {
        return false;
    }

    bool modified = false;

    modified |= BsonAppendIncludeRegexList(query, cfr_class_keys, filter->classRxIncludes);
    modified |= BsonAppendExcludeRegexList(query, cfr_class_keys, filter->classRxExcludes);

    return modified;
}

/*****************************************************************************/

bool BsonAppendIncludeList(bson *b, char *key, Rlist *include_values)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    if (!include_values)
    {
        return false;
    }

    for (Rlist *rp = include_values; rp; rp = rp->next)
    {
        if( !BsonAppendString( b, key, ScalarValue(rp) ))
        {
            return false;
        }
    }

    return true;
}

/*****************************************************************************/

void BsonAppendArrayRegex(bson *b, const char *key, Rlist *rx_values)
{
    assert( b );
    assert( !b->finished );
    assert( key );

    {
        BsonAppendStartArray( b, key );

        int i = 0;
        for (Rlist *rp = rx_values; rp != NULL; rp = rp->next, i++)
        {
            char anchored_rx[CF_BUFSIZE] = { 0 };
            AnchorRegex(ScalarValue(rp), anchored_rx, sizeof(anchored_rx));

            char index_str[32] = { 0 };
            snprintf(index_str, sizeof(index_str), "%d", i);
            BsonAppendRegex(b, index_str, anchored_rx, "");
        }

        BsonAppendFinishArray( b );
    }
}

/*****************************************************************************/

bool BsonAppendIncludeRegexList(bson *b, char *include_key, Rlist *include_rx_values)
{
    assert( b );
    assert( !b->finished );
    assert( include_key );

    if (!include_rx_values)
    {
        return false;
    }

    {
        BsonAppendStartObject( b, include_key );
        BsonAppendArrayRegex( b, "$all", include_rx_values );
        return BsonAppendFinishObject( b );
    }
}

/*****************************************************************************/

bool BsonAppendExcludeRegexList(bson *b, char *exclude_key, Rlist *exclude_rx_values)
{
    assert( b );
    assert( !b->finished );
    assert( exclude_key );

    if (!exclude_rx_values)
    {
        return false;
    }

    {
        BsonAppendStartObject( b, exclude_key );
        {
            BsonAppendStartObject( b, "$not" );

            BsonAppendArrayRegex( b, "$all", exclude_rx_values );

            BsonAppendFinishObject( b );
        }
        return BsonAppendFinishObject( b );
    }
}

/*****************************************************************************/

bool BsonAppendExcludeList(bson *b, char *exclude_key, Rlist *exclude_values)
{
    assert( b );
    assert( !b->finished );
    assert( exclude_key );

    if (!exclude_values)
    {
        return false;
    }

    bool ret = false;
    {
        BsonAppendStartObject(b, exclude_key);
        {
            BsonAppendStartArray(b, "$not");

            for (Rlist *rp = exclude_values; rp != NULL; rp = rp->next)
            {
                BsonAppendString(b, exclude_key, ScalarValue(rp));
            }

            BsonAppendFinishArray(b);
        }
        ret = BsonAppendFinishObject(b);
    }

    return ret;
}

/*****************************************************************************/

void BsonAppendAddToSetString(bson *b, char *key, char *value)
{
    assert( b );
    assert( !b->finished );
    assert( key );
    assert( value );

    {
        BsonAppendStartObject(b, "$addToSet");
        BsonAppendString(b, key, value);
        BsonAppendFinishObject(b);
    }
}

/*****************************************************************************/

void BsonAppendRecentQuery(bson *b, int max_age_seconds)
{
    assert( b );
    assert( !b->finished );
    assert( max_age_seconds >= 0 );

    time_t now = time(NULL);
    time_t min_timestamp = now - max_age_seconds;

    {
        BsonAppendStartObject(b, cfr_time);

        BsonAppendInt(b, "$gte", min_timestamp);

        BsonAppendFinishObject(b);
    }
}

/*****************************************************************************/

void BsonAppendAgedQuery(bson *b, int max_age_seconds)
/*
 * timestamp not there or older than maxAgeInSeconds
 */
{
    assert( b );
    assert( !b->finished );
    assert( max_age_seconds >= 0 );

    time_t now = time(NULL);
    time_t min_timestamp = now - max_age_seconds;

    {
        BsonAppendStartArray(b, "$or");

        {
            BsonAppendStartObject(b, "0");
            BsonAppendNull(b, cfr_time);
            BsonAppendFinishObject(b);
        }

        {
            BsonAppendStartObject(b, "1");
            {
                BsonAppendStartObject(b, cfr_time);
                BsonAppendInt(b, "$lt", min_timestamp);
                BsonAppendFinishObject(b);
            }
            BsonAppendFinishObject(b);
        }

        BsonAppendFinishArray(b);
    }
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

/* NOTE: Clients older than Enterprise 3.0 do not support
   promise_context and have to be filter out */
void BsonAppendClassFilterFromPromiseContext(bson *b, PromiseContextMode promise_context)
{
    assert( b );
    assert( !b->finished );

    if (promise_context != PROMISE_CONTEXT_MODE_ALL)
    {
        Rlist *old_ent_versions = NULL;
        PrependRScalar(&old_ent_versions, (void *) "enterprise_3.*", CF_SCALAR);

        {
            BsonAppendStartObject(b, cfr_class_keys);
            BsonAppendArrayRegex(b, "$in", old_ent_versions);
            BsonAppendFinishObject(b);
        }

        DeleteRlist(old_ent_versions);
    }
}

/*****************************************************************************/

void BsonAppendHostColourFilter(bson *query, HostColourFilter *filter)
{
    assert( query );
    assert( !query->finished );

    if (filter == NULL)
    {
        return;
    }

    char *score_method = NULL;
    char score_key[10] = { 0 };
    switch (filter->method)
    {
        case HOST_RANK_METHOD_COMPLIANCE:
            switch (filter->promise_context)
            {
                case PROMISE_CONTEXT_MODE_ALL:
                    strcpy(score_key, cfr_score_comp);
                    break;
                case PROMISE_CONTEXT_MODE_USER:
                    strcpy(score_key, cfr_score_comp_user);
                    break;
                case PROMISE_CONTEXT_MODE_INTERNAL:
                    strcpy(score_key, cfr_score_comp_internal);
                    break;
            }

            xasprintf(&score_method, "%s", score_key);
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
            BsonAppendStartArray(query, "$or");
            {
                BsonAppendStartObject(query, "0");
                {
                    BsonAppendStartObject(query, cfr_day);
                    BsonAppendLong(query, "$lt", filter->blue_time_horizon);
                    BsonAppendFinishObject(query);
                }
                BsonAppendFinishObject(query);
            }

            {
                BsonAppendStartObject(query, "1");
                {
                    BsonAppendStartObject(query, cfr_day);
                    BsonAppendBool(query, "$exists", false);
                    BsonAppendFinishObject(query);
                }
                BsonAppendFinishObject(query);
            }

            {
                BsonAppendStartObject(query, "2");
                {
                    BsonAppendStartObject(query, cfr_is_black);
                    BsonAppendBool(query, "$exists", false);
                    BsonAppendFinishObject(query);
                }
                BsonAppendFinishObject(query);
            }

            {
                BsonAppendStartObject(query, "3");
                {
                    BsonAppendStartObject(query, score_method);
                    BsonAppendBool(query, "$exists", false);
                    BsonAppendFinishObject(query);
                }
                BsonAppendFinishObject(query);
            }

            BsonAppendFinishArray(query);
        }
    }

    if (filter->colour == HOST_COLOUR_GREEN)
    {
        BsonAppendStartObject(query, score_method);
        BsonAppendInt(query, "$lt", CF_AMBER_THRESHOLD);
        BsonAppendFinishObject(query);

        BsonAppendStartObject (query, cfr_day);
        BsonAppendLong(query, "$gte", filter->blue_time_horizon);
        BsonAppendFinishObject(query);

        BsonAppendBool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_YELLOW)
    {
        BsonAppendStartObject(query, score_method);
        BsonAppendInt(query, "$gte", CF_AMBER_THRESHOLD);
        BsonAppendInt(query, "$lt", CF_RED_THRESHOLD);
        BsonAppendFinishObject(query);

        BsonAppendStartObject (query, cfr_day);
        BsonAppendLong(query, "$gte", filter->blue_time_horizon);
        BsonAppendFinishObject(query);

        BsonAppendBool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_RED)
    {
        BsonAppendStartObject(query, score_method);
        BsonAppendInt(query, "$gte", CF_RED_THRESHOLD);
        BsonAppendFinishObject(query);

        BsonAppendStartObject(query, cfr_day);
        BsonAppendLong(query, "$gte", filter->blue_time_horizon);
        BsonAppendFinishObject(query);

        BsonAppendBool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_GREEN_YELLOW_RED) // !BLUE
    {
        BsonAppendStartObject(query, cfr_day);
        BsonAppendLong(query, "$gte", filter->blue_time_horizon);
        BsonAppendFinishObject(query);

        BsonAppendBool(query, cfr_is_black, false);
    }

    if (filter->colour == HOST_COLOUR_BLACK)
    {
        BsonAppendStartObject(query, cfr_day);
        BsonAppendLong(query, "$gte", filter->blue_time_horizon);
        BsonAppendFinishObject(query);

        BsonAppendBool(query, cfr_is_black, true);
    }

    BsonAppendClassFilterFromPromiseContext(query, filter->promise_context);

    free(score_method);
}

/*****************************************************************************/

void BsonAppendSortField(bson *b, char *sort_field)
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
 */
{    
    assert( b );
    assert( !b->finished );
    assert( !NULL_OR_EMPTY(sort_field));

    BsonAppendStartObject(b, "$query");
    BsonAppendFinishObject(b);

    BsonAppendStartObject(b, "$orderby");
    BsonAppendInt(b, sort_field, -1);
    BsonAppendFinishObject(b);
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

/*****************************************************************************/

int BsonSelectReportFields( bson *fields, int fieldCount, ... )
{
    va_list arguments;

    va_start ( arguments, fieldCount );

    bson_init(fields);

    int count = 0;
    for ( count = 0; count < fieldCount; count++ )
    {
        char *key = va_arg( arguments, char *);
        BsonAppendInt(fields, key, 1);
    }

    va_end ( arguments );
    BsonFinish(fields);

    assert(count == fieldCount);

    return count;
}

/*****************************************************************************/

// false <-> failure
static bool JsonPrimitiveToBson(const JsonElement *primitive, bson *buffer, const char *insert_key)
// if insert_key is null insert object keyname
{
    if ((primitive == NULL) || (buffer == NULL))
    {
        return false;
    }

    if (JsonGetElementType(primitive) != JSON_ELEMENT_TYPE_PRIMITIVE)
    {
        return false;
    }

    bool err = true;
    const char *key = NULL;
    if (insert_key == NULL)
    {
        key = JsonGetPropertyAsString(primitive);
    }
    else
    {
        key = insert_key;
    }
    const char *value = JsonPrimitiveGetAsString(primitive);


    switch (JsonGetPrimitiveType(primitive))
    {
        case JSON_PRIMITIVE_TYPE_INTEGER:
        {
            BsonAppendInt(buffer, key, (int)StringToLong(value));
            break;
        }
        case JSON_PRIMITIVE_TYPE_STRING:
        {
            BsonAppendString(buffer, key, value);
            break;
        }
        case JSON_PRIMITIVE_TYPE_REAL:
        {
            BsonAppendDouble(buffer, key, (double)StringToDouble(value));
            break;
        }
        case JSON_PRIMITIVE_TYPE_BOOL:
        {
            BsonAppendBool(buffer, key, StringSafeCompare(value,"false")? true:false);
            break;
        }
        default:
        {
            err = false;
            break;
        }
    }

    return err;
}

/*****************************************************************************/

// false <-> failure
static bool JsonComplexToBson(const JsonElement *user_query, bson *buffer)
{
    if (user_query == NULL || buffer == NULL)
    {
        return false;
    }

    JsonIterator it = JsonIteratorInit(user_query);
    while (JsonIteratorNextValue(&it) != NULL)
    {
        // SIMPLE QUERY SUPPORT
        if (JsonIteratorCurrentElementType(&it) == JSON_ELEMENT_TYPE_PRIMITIVE)
        {
            if (!JsonPrimitiveToBson(JsonIteratorCurrentValue(&it), buffer, NULL))
            {
                return false;
            }
        }
        else // COMPLEX QUERY SUPPORT
        {
            if (JsonIteratorCurrentContrainerType(&it) == JSON_CONTAINER_TYPE_OBJECT)
            {
                BsonAppendStartObject(buffer, JsonIteratorCurrentKey(&it));

                if (!JsonComplexToBson(JsonIteratorCurrentValue(&it), buffer))
                {
                    return false;
                }

                BsonAppendFinishObject(buffer);
            }
            else // ARRAY
            {
                BsonAppendStartArray(buffer, JsonIteratorCurrentKey(&it));

                char iStr[CF_SMALLBUF];
                int i = 0;

                JsonIterator it2 = JsonIteratorInit(JsonIteratorCurrentValue(&it));
                while(JsonIteratorNextValue(&it2) != NULL)
                {
                    snprintf(iStr, sizeof(iStr), "%d", i);

                    if (JsonIteratorCurrentElementType(&it2) == JSON_ELEMENT_TYPE_CONTAINER)
                    {
                        BsonAppendStartObject(buffer, iStr);

                        if (!JsonComplexToBson(JsonIteratorCurrentValue(&it2), buffer))
                        {
                            return false;
                        }
                        BsonAppendFinishArray(buffer);
                    }
                    else
                    {
                        if (!JsonPrimitiveToBson(JsonIteratorCurrentValue(&it2), buffer, iStr))
                        {
                            return false;
                        }
                    }

                    i++;
                }

                BsonAppendFinishObject(buffer);
            }
        }
    }

    return true;
}


/*****************************************************************************/

bool BsonInitFromJsonString(bson *bson_ret, const char *json_string)
{
    assert( bson_ret );
    assert( !bson_ret->finished );
    assert( json_string );

    const char * json_string_new = SearchAndReplace(json_string, "'", "\"");

    JsonElement *json_query = NULL;
    JsonParseError err = JsonParse(&json_string_new, &json_query);
    if (err != JSON_PARSE_OK)
    {
        return false;
    }
    assert(json_query);

    bson_init(bson_ret);
    if (!JsonComplexToBson(json_query, bson_ret))
    {
        bson_destroy(bson_ret);
        return false;
    }
    BsonFinish(bson_ret);

    return true;
}

/*****************************************************************************/

static bool BsonInitFromJsonStringFV(bson *bson_ret, const char *fmt, va_list ap)
{
    assert( bson_ret );
    assert( !bson_ret->finished );

    char *str = NULL;

    xvasprintf(&str, fmt, ap);
    bool ret = BsonInitFromJsonString(bson_ret, str);

    free(str);
    return ret;
}

/*****************************************************************************/

bool BsonInitFromJsonStringF(bson *bson_ret, const char *fmt, ...)
{
    assert( bson_ret );
    assert( !bson_ret->finished );
    va_list ap;

    va_start(ap, fmt);
    bool res = BsonInitFromJsonStringFV(bson_ret, fmt, ap);
    va_end(ap);

    return res;
}

/*****************************************************************************/

JsonElement* BsonContainerToJsonContainer(const bson *b, bson_type type, _Bool ignore_timestamp) //type: array|object
// function map bson object/array with it's tree to json object/array
// do not support bson_date -> it is skipped from mapping
{
    if (b == NULL)
    {
        return NULL;
    }

    bson_iterator it;
    bson_iterator_init(&it, b);

    JsonElement *json_ret;
    if (type == BSON_ARRAY)
    {
        json_ret = JsonArrayCreate(10);
    }
    else if (type == BSON_OBJECT)
    {
        json_ret = JsonObjectCreate(10);
    }
    else
    {
        return NULL;
    }

    int size = 0;

    while (bson_iterator_next(&it))
    {
        if (ignore_timestamp && StringSafeEqual(bson_iterator_key(&it), cfr_day))
        {
            continue;
        }

        switch ((int)bson_iterator_type(&it))
        {
            case BSON_OBJECT:
            {
                JsonElement *json_obj = NULL;

                bson child;
                bson_iterator_subobject(&it, &child);
                json_obj = BsonContainerToJsonContainer(&child, BSON_OBJECT, ignore_timestamp);
                if (json_obj != NULL)
                {
                    ++size;
                    if (type == BSON_OBJECT)
                    {
                        JsonObjectAppendObject(json_ret,
                                               bson_iterator_key(&it),
                                               json_obj);
                    }
                    else // ARRAY
                    {
                        JsonArrayAppendObject(json_ret,
                                              json_obj);
                    }
                }
                break;
            }
            case BSON_ARRAY:
            {
                JsonElement *json_arr = NULL;

                bson child;
                bson_iterator_subobject(&it, &child);
                json_arr = BsonContainerToJsonContainer(&child, BSON_ARRAY, ignore_timestamp);
                if (json_arr != NULL)
                {
                    ++size;
                    if (type == BSON_OBJECT)
                    {
                        JsonObjectAppendArray(json_ret,
                                              bson_iterator_key(&it),
                                              json_arr);
                    }
                    else // ARRAY
                    {
                        JsonArrayAppendArray(json_ret,
                                             json_arr);
                    }
                }
            break;
            }
            case BSON_BINDATA:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendString(json_ret,
                                           bson_iterator_key(&it),
                                           bson_iterator_bin_data(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendString(json_ret,
                                          bson_iterator_bin_data(&it));
                }
                break;
            }
            case BSON_DOUBLE:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendReal(json_ret,
                                         bson_iterator_key(&it),
                                         bson_iterator_double(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendReal(json_ret,
                                        bson_iterator_double(&it));
                }
                break;
            }
            case BSON_STRING:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendString(json_ret,
                                           bson_iterator_key(&it),
                                           bson_iterator_string(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendString(json_ret,
                                          bson_iterator_string(&it));
                }
                break;
            }
            case BSON_BOOL:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendBool(json_ret,
                                         bson_iterator_key(&it),
                                         bson_iterator_bool(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendBool(json_ret,
                                        bson_iterator_bool(&it));
                }
                break;
            }
            case BSON_DATE: // currently unsupported
            {
                break;
            }
            case BSON_INT:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendInteger(json_ret,
                                            bson_iterator_key(&it),
                                            bson_iterator_int(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendInteger(json_ret,
                                           bson_iterator_int(&it));
                }
                break;
            }
            case BSON_LONG:
            {
                ++size;
                if (type == BSON_OBJECT)
                {
                    JsonObjectAppendInteger(json_ret,
                                            bson_iterator_key(&it),
                                            (int)bson_iterator_long(&it));
                }
                else // ARRAY
                {
                    JsonArrayAppendInteger(json_ret,
                                           (int)bson_iterator_long(&it));
                }
                break;
            }
        }
    }

    if (size == 0)
    {
        JsonElementDestroy(json_ret);
        return NULL;
    }

    return json_ret;
}
/*****************************************************************************
 * Return value:                                                             *
 * -1 = invalid type                                                         *
 * 0  = BSON_EOO (end of object)                                             *
 * 1  = valid bson_type                                                      *
 *****************************************************************************/
int BsonIsTypeValid(bson_type type)
{
    if(((unsigned)type) > 18)
    {
        return -1;
    }
    else if( type == BSON_EOO )
    {
        return BSON_EOO;
    }

    return 1;
}

/*****************************************************************************/

void BsonFinish( bson *b )
{
    assert( b && !b->finished );

    int ret = bson_finish( b );

    assert( ret == BSON_OK );
}

/*****************************************************************************/

void BsonFilterInStringArrayRlist(bson *b, const char *key, const Rlist *string_rlist)
{
    assert( b );
    assert( !b->finished );

    BsonAppendStartObject( b, key );
    {
        BsonAppendStartArray( b, "$in" );

        int i = 0;
        for (const Rlist *rp = string_rlist; rp; rp = rp->next, i++)
        {
            BsonAppendStringAt( b, i, ScalarValue( rp ) );
        }

        BsonAppendFinishArray( b );
    }
    BsonAppendFinishObject(b);
}

/*****************************************************************************/
