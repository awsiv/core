/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "bson_lib.h"

#ifdef HAVE_LIBMONGOC

static const char* BsonGetArrayValue(const bson* b, const char* key);


/*****************************************************************************/

Item *BsonGetStringArrayAsItemList(const bson* b, const char* key)
// TODO: Deprecate in favour of BsonStringArrayAsRlist()
{
 const char *array = BsonGetArrayValue(b, key);

 if(!array)
    {
    return NULL;
    }
 
 bson_iterator it;
 bson_iterator_init(&it, array);

 Item *values = NULL;
 
 while (bson_iterator_next(&it))
    {
    PrependItem(&values, bson_iterator_string(&it), NULL);
    }

 return values;
}

/*****************************************************************************/

Rlist *BsonStringArrayAsRlist(const bson* b, const char* key)
{
 const char *array = BsonGetArrayValue(b, key);

 if(!array)
    {
    return NULL;
    }
 
 bson_iterator it;
 bson_iterator_init(&it, array);

 Rlist *values = NULL;
 
 while (bson_iterator_next(&it))
    {
    if(bson_iterator_type(&it) == bson_string)
       {
       // NOTE: preserve ordering (don't prepend)
       AppendRScalar(&values, (char *)bson_iterator_string(&it), CF_SCALAR);
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

int BsonIntGet(const bson *b, const char *key)
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

const char *BsonGetString(const bson *b, const char *key)
// TOOD: DEPRECATE in favour of BsonStringWrite()
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

void BsonStringWrite(char *dest, int destSz, const bson *b, const char *key)
{
 bson_iterator it;
 
 if(bson_find(&it, b, key) == bson_string)
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

static const char *BsonGetArrayValue(const bson *b, const char *key)
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

bool BsonAppendRegexSafe(bson_buffer *bb, char *key, char *rxValue)
{
 if(key == NULL || key[0] == '\0')
    {
    return false;
    }

 char anchoredRx[CF_MAXVARSIZE];
 AnchorRegex(rxValue, anchoredRx, sizeof(anchoredRx));

 bson_append_regex(bb, key, anchoredRx, "");

 return true;
}

/*****************************************************************************/

void BsonAppendStringArray(bson_buffer *bb, char *arrayName, Item *arrayValues)
{
 bson_buffer *arr = bson_append_start_array(bb, arrayName);
 int i = 0;
 char iStr[32];

 Item *ip;
 for(ip = arrayValues; ip != NULL; ip = ip->next, i++)
    {
    snprintf(iStr, sizeof(iStr), "%d", i);
    bson_append_string(bb, iStr, ip->name);
    }
 
 bson_append_finish_object(arr);
}

/*****************************************************************************/

bool BsonAppendIncludeRxList(bson_buffer *queryBuffer, char *includeKey, Rlist *includeRxValues)
{
 if(!includeRxValues)
    {
    return false;
    }

 for (Rlist *rp = includeRxValues; rp; rp = rp->next)
    {
    char anchoredRx[CF_BUFSIZE];
    AnchorRegex(ScalarValue(rp), anchoredRx, sizeof(anchoredRx));
    bson_append_regex(queryBuffer, includeKey, anchoredRx, "");
    }
 
 return true;
}

/*****************************************************************************/

bool BsonAppendExcludeRxList(bson_buffer *queryBuffer, char *excludeKey, Rlist *excludeRxValues)
{
 if(!excludeRxValues)
    {
    return false;
    }
 
 bson_buffer *excludeClassBuffer = bson_append_start_object(queryBuffer, excludeKey);
 bson_buffer *excludeClassArray = bson_append_start_array(excludeClassBuffer, "$nin");
 
 for (Rlist *rp = excludeRxValues; rp != NULL; rp = rp->next)
    {
    char anchoredRx[CF_BUFSIZE];
    AnchorRegex(rp->item, anchoredRx, sizeof(anchoredRx));
    bson_append_regex(excludeClassArray, excludeKey, anchoredRx, "");
    }
 
 bson_append_finish_object(excludeClassArray);
 bson_append_finish_object(excludeClassBuffer);

 return true;
}

/*****************************************************************************/

void BsonAppendAddToSetString(bson_buffer *bb, char *key, char *value)
{
 bson_append_start_object(bb, "$addToSet");
 bson_append_string(bb, key, value);
 bson_append_finish_object(bb);
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
