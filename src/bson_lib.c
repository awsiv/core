/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"

#ifdef HAVE_LIBMONGOC

static const char* BsonGetArrayValue(const bson* b, const char* key);


/*****************************************************************************/

struct Item* BsonGetStringArrayAsItemList(const bson* b, const char* key)
{
 const char* array = BsonGetArrayValue(b, key);

 if(!array)
    {
    return NULL;
    }
 
 bson_iterator it;
 bson_iterator_init(&it, array);

 struct Item* values = NULL;
 
 while (bson_iterator_next(&it))
    {
    PrependItem(&values, bson_iterator_string(&it), NULL);
    }

 return values;
}

/*****************************************************************************/

int BsonGetInt(const bson* b, const char* key)
{
 bson_iterator it;
 
 if(bson_find(&it, b, key) == bson_int)
    {
    return bson_iterator_int(&it);
    }

 CfOut(cf_verbose, "", "BsonFindInt: No match for \"%s\"", key);

 return 0;  // TODO: handle this better?
}

/*****************************************************************************/

const char* BsonGetString(const bson* b, const char* key)
{
 bson_iterator it;
 
 if(bson_find(&it, b, key) == bson_string)
    {
    return bson_iterator_string(&it);
    }

 CfOut(cf_verbose, "", "BsonGetString: No match for \"%s\"", key);

 return NULL;
}

/*****************************************************************************/

static const char* BsonGetArrayValue(const bson* b, const char* key)
{
 bson_iterator it;
 
 if(bson_find(&it, b, key) == bson_array)
    {
    return bson_iterator_value(&it);
    }

 CfOut(cf_verbose, "", "BsonGetArrayValue: No match for \"%s\"", key);

 return NULL;
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
 const char * key;
 char oidhex[25];
 char buf[CF_BUFSIZE];

 memset(retBuf,0,retBufSz);
 bson_iterator_init( &i , data );

 while ( bson_iterator_next( &i ) ){

 bson_type t = bson_iterator_type( &i );

 if ( t == 0 )
    {
    break;
    }

 key = bson_iterator_key( &i );

 snprintf(buf,sizeof(buf),"%s : ",key);
 Join(retBuf,buf,retBufSz);
	

 switch ( t ){
 case bson_int: 
     snprintf(buf,sizeof(buf), "%d" , bson_iterator_int( &i ) ); 
     break;

 case bson_double: 
     snprintf(buf,sizeof(buf), "%f" , bson_iterator_double( &i ) ); 
     break;

 case bson_bool: 
     snprintf(buf,sizeof(buf), "%s" , bson_iterator_bool( &i ) ? "true" : "false" ); 
     break;

 case bson_string:
     snprintf(buf,sizeof(buf), "%s" , bson_iterator_string( &i ) ); 
     break;

 case bson_regex:
     snprintf(buf,sizeof(buf), "/%s/" , bson_iterator_string( &i ) ); 
     break;

     
 case bson_null: 
     snprintf(buf,sizeof(buf), "null"); 
     break;

 case bson_oid: 
     bson_oid_to_string(bson_iterator_oid(&i), oidhex); 
     snprintf(buf,sizeof(buf), "%s" , oidhex ); 
     break;

 case bson_object:
     buf[0] = '{';
     BsonToString(buf+1, sizeof(buf-1), (char *)bson_iterator_value( &i ));
     EndJoin(buf,"}",sizeof(buf));
     break;

 case bson_array:

     buf[0] = '[';
     BsonToString(buf+1, sizeof(buf-1), (char *)bson_iterator_value( &i ));
     EndJoin(buf,"]",sizeof(buf));
     break;

 default:
     snprintf(buf,sizeof(buf) , "can't print type : %d\n" , t );
 }

 Join(retBuf,buf,retBufSz);
 Join(retBuf,", ",retBufSz);
 }
    
 retBuf[strlen(retBuf)-2] = 0;  // clear last comma
}


#endif  /* HAVE_LIBMONGOC */
