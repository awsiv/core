/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BSON_LIB_H
#define CFENGINE_BSON_LIB_H

#include "cf3.defs.h"
#include "cf3.extern.h"

#ifdef HAVE_LIBMONGOC

Item *BsonGetStringArrayAsItemList(const bson *b, const char *key);
int BsonGetInt(const bson *b, const char *key);
const char *BsonGetString(const bson *b, const char *key);
void BsonAppendStringArray(bson_buffer *bb, char *arrayName, Item *arrayValues);
bool BsonAppendIncludeList(bson_buffer *queryBuffer, char *includeKey, Rlist *includeRxValues);
bool BsonAppendExcludeList(bson_buffer *queryBuffer, char *excludeKey, Rlist *excludeRxValues);
void BsonAppendAddToSetString(bson_buffer *bb, char *key, char *value);
void BsonAppendRecentQuery(bson_buffer *querybuf, int maxAgeInSeconds);
void BsonAppendAgedQuery(bson_buffer *querybuf, int maxAgeInSeconds);
void BsonToString(char *retBuf, int retBufSz, char *data);


#endif  /* HAVE_LIBMONGOC */

#endif  /* NOT CFENGINE_BSON_LIB_H */
