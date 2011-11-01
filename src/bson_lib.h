/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BSON_LIB_H
#define CFENGINE_BSON_LIB_H

#include "cf3.defs.h"
#include "cf3.extern.h"

#ifdef HAVE_LIBMONGOC

void BsonAppendRecentQuery(bson_buffer *querybuf, int maxAgeInSeconds);
void BsonAppendAgedQuery(bson_buffer *querybuf, int maxAgeInSeconds);

#endif  /* HAVE_LIBMONGOC */

#endif  /* NOT CFENGINE_BSON_LIB_H */
