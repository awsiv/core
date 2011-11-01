/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"

#ifdef HAVE_LIBMONGOC

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
{
 time_t currentTimeStamp = time(NULL);
 time_t minTimeStamp = currentTimeStamp - maxAgeInSeconds;

 bson_buffer *sub = bson_append_start_object(querybuf, cfr_time);
 bson_append_int(sub, "$lt", minTimeStamp);
 bson_append_finish_object(sub);
}


#endif  /* HAVE_LIBMONGOC */
