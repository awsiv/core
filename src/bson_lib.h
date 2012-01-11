/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BSON_LIB_H
#define CFENGINE_BSON_LIB_H

#include "cf3.defs.h"
#include "cf3.extern.h"

#ifdef HAVE_LIBMONGOC

struct Item *BsonGetStringArrayAsItemList(const bson *b, const char *key);
int BsonGetInt(const bson *b, const char *key);
const char *BsonGetString(const bson *b, const char *key);
void BsonAppendStringArray(bson_buffer *bb, char *arrayName, struct Item *arrayValues);
void BsonAppendRecentQuery(bson_buffer *querybuf, int maxAgeInSeconds);
void BsonAppendAgedQuery(bson_buffer *querybuf, int maxAgeInSeconds);
void BsonToString(char *retBuf, int retBufSz, char *data);


#endif  /* HAVE_LIBMONGOC */

#endif  /* NOT CFENGINE_BSON_LIB_H */
