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
 int temp;
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
