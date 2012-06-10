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
void BsonAppendStringArray(bson *b, char *arrayName, Item *arrayValues);
void BsonAppendArrayRx(bson *buffer, const char *key, Rlist *rx_values);
bool BsonAppendHostClassFilter(bson *queryBuffer, HostClassFilter *filter);
bool BsonAppendIncludeList(bson *queryBuffer, char *includeKey, Rlist *includeValues);
bool BsonAppendIncludeRxList(bson *query, char *includeKey, Rlist *includeRxValues);
bool BsonAppendExcludeList(bson *query, char *excludeKey, Rlist *excludeValues);
bool BsonAppendExcludeRxList(bson *query, char *excludeKey, Rlist *excludeRxValues);
void BsonAppendAddToSetString(bson *b, char *key, char *value);
void BsonAppendRecentQuery(bson *query, int maxAgeInSeconds);
void BsonAppendAgedQuery(bson *query, int maxAgeInSeconds);
void BsonToString(char *retBuf, int retBufSz, bson *data);
void BsonAppendHostColourFilter(bson *query, HostColourFilter *filter);
void BsonAppendSortField(bson *bb, char *sortField);
long BsonLongGet(const bson *b, const char *key);
bool BsonGetArrayValue(const bson *b, const char *key, bson *sub);
bool BsonIsKeyCorrupt(const char *key);
bool BsonIsEmpty(const bson *object);

#endif
