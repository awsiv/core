/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BSON_LIB_H
#define CFENGINE_BSON_LIB_H

#include "cf3.defs.h"
#include "cf3.extern.h"

#include "sequence.h"

Item *BsonGetStringArrayAsItemList(const bson *b, const char *key);
Rlist *BsonStringArrayAsRlist(const bson *b, const char *key);
bool BsonStringGet(const bson *b, const char *key, const char **out);
/**
  * BsonBoolGetCheckExists *
  * Returns false if key doens't exist
  * If the key exists, returns true and writes the value into *out
  * Use this when the existence of key matters
  */
bool BsonBoolGetCheckExists(const bson *b, const char *key, bool *out);
/**
  * BsonBoolGet *
  * Returns:
  * - true if the key exists and the value is true
  * - false otherwise
  *
  * Use this for cases where the absence of key
  * can be considered as having a false value.
  */
bool BsonBoolGet(const bson *b, const char *key);
bool BsonIntGet(const bson *b, const char *key, int *out);
bool BsonTimeGet(const bson *b, const char *key, time_t *out);
bool BsonDoubleGet(const bson *b, const char *key, double *out);
bool BsonArrayGet(const bson *b, const char *key, bson *out);
bool BsonObjectGet(const bson *b, const char *key, bson *out);
bool BsonIsArrayNonExistentOrEmpty(const bson *b, const char *key);
void BsonStringWrite(char *dest, int destSz, const bson *b, const char *key);
bool BsonAppendBool( bson *b, const char *key, const bool value );
bool BsonAppendInt( bson *b, const char *key, int value );
bool BsonAppendDouble( bson *b, const char *key, double value );
bool BsonAppendLong( bson *b, const char *key, long value );
bool BsonAppendString(bson *b, const char *key, const char *value);
bool BsonAppendStringNonEmpty(bson *b, const char *key, const char *value);
bool BsonAppendRegex(bson *b, const char *key, const char *pattern, const char *opts);
bool BsonAppendStringAt( bson *b, int pos, const char *value );
bool BsonAppendRegexNonEmpty(bson *b, const char *key, char *rx_value);
void BsonAppendStringArray(bson *b, char *array_name, Item *array_values);
void BsonAppendStringArrayRlist(bson *b, const char *key, const Rlist *string_rlist);
void BsonAppendStringArraySequence(bson *b, const char *key, const Sequence *string_seq);
void BsonAppendArrayRegex(bson *b, const char *key, Rlist *rx_values);
bool BsonAppendHostClassFilter(bson *b, const HostClassFilter *filter);
bool BsonAppendIncludeList(bson *b, char *key, Rlist *include_values);
bool BsonAppendIncludeRegexList(bson *query, char *include_key, Rlist *include_rx_values);
bool BsonAppendExcludeList(bson *query, char *exclude_key, Rlist *exclude_values);
bool BsonAppendExcludeRegexList(bson *query, char *exclude_key, Rlist *exclude_rx_values);
void BsonAppendAddToSetString(bson *b, char *key, char *value);
void BsonAppendRecentQuery(bson *b, int max_age_seconds);
void BsonAppendAgedQuery(bson *b, int max_age_seconds);
bool BsonAppendStartObject( bson *b, const char *key );
bool BsonAppendStartArray( bson *b, const char *key );
bool BsonAppendFinishObject( bson *b );
bool BsonAppendFinishArray( bson *b );

void BsonToString(char *retBuf, int retBufSz, bson *data);
void BsonAppendHostColourFilter(bson *query, HostColourFilter *filter);
void BsonAppendSortField(bson *b, char *sort_field);
long BsonLongGet(const bson *b, const char *key);
bool BsonGetArrayValue(const bson *b, const char *key, bson *sub);
bool BsonIsKeyCorrupt(const char *key);
bool BsonIsEmpty(const bson *object);
JsonElement* BsonContainerToJsonContainer(const bson *b, bson_type type, bool ignore_timestamp);
int BsonIsTypeValid(bson_type type);
void BsonAppendClassFilterFromPromiseContext(bson *b, PromiseContextMode promise_context);

bool BsonInitFromJsonString(bson *bson_ret, const char *json_string);
bool BsonInitFromJsonStringF(bson *bson_ret, const char *fmt, ...);

/**
 * WARNING:
 *   1. Allocates memory for the bson * passed to it which must be freed in the calling function
 *   2. Only use this function to create new bson objects.
        If the passed bson already contains some fields, they will be overwritten.
 *   3. The fieldCount must match the number of parameters passed
*/
int BsonSelectReportFields( bson *fields, int fieldCount, ... );

/**
 * usage:
 *  - NULL bson objects not allowed
 *  - Finished bson objects not allowed
 *  - Makes sure that return value from bson_finish is BSON_OK
 */
void BsonFinish(bson *b);

/**
 * Wrappers specific to filtering
 * Naming:
 *      - The word immediately after BsonFilter is the mongodb operator used
 *          eg. 'In' = $in for BsonFilterIn
 *      - This is followed by the type of data to be appended
 *          eg. StringArrayRlist
 */

void BsonFilterInStringArrayRlist(bson *b, const char *key, const Rlist *string_rlist);

#endif
