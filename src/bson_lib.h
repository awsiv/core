/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BSON_LIB_H
#define CFENGINE_BSON_LIB_H

#include "cf3.defs.h"
#include "cf3.extern.h"

Item *BsonGetStringArrayAsItemList(const bson *b, const char *key);
Rlist *BsonStringArrayAsRlist(const bson *b, const char *key);
bool BsonStringGet(const bson *b, const char *key, const char **out);
bool BsonBoolGet(const bson *b, const char *key, bool *out);
bool BsonIntGet(const bson *b, const char *key, int *out);
bool BsonTimeGet(const bson *b, const char *key, time_t *out);
bool BsonArrayGet(const bson *b, const char *key, const char **out);
bool BsonObjectGet(const bson *b, const char *key, bson **out);
bool BsonIsArrayNonExistentOrEmpty(const bson *b, const char *key);
void BsonStringWrite(char *dest, int destSz, const bson *b, const char *key);
bool BsonAppendStringSafe(bson *b, char *key, char *value);
bool BsonAppendRegexSafe(bson *bb, char *key, char *rxValue);
void BsonAppendStringArray(bson *bb, char *arrayName, Item *arrayValues);
void BsonAppendArrayRx(bson *buffer, const char *key, Rlist *rx_values);
bool BsonAppendHostClassFilter(bson *queryBuffer, HostClassFilter *filter);
bool BsonAppendIncludeList(bson *queryBuffer, char *includeKey, Rlist *includeValues);
bool BsonAppendIncludeRxList(bson *queryBuffer, char *includeKey, Rlist *includeRxValues);
bool BsonAppendExcludeList(bson *queryBuffer, char *excludeKey, Rlist *excludeValues);
bool BsonAppendExcludeRxList(bson *queryBuffer, char *excludeKey, Rlist *excludeRxValues);
void BsonAppendAddToSetString(bson *bb, char *key, char *value);
void BsonAppendRecentQuery(bson *querybuf, int maxAgeInSeconds);
void BsonAppendAgedQuery(bson *querybuf, int maxAgeInSeconds);
void BsonToString(char *retBuf, int retBufSz, char *data);
void BsonAppendHostColourFilter(bson *query_buffer, HostColourFilter *filter);
void BsonAppendSortField(bson *bb, char *sortField);
long BsonLongGet(const bson *b, const char *key);
const bson *BsonGetArrayValue(const bson *b, const char *key);
bool BsonIsKeyCorrupt(const char *key);
bool BsonIsEmpty(const bson *object);

#endif
