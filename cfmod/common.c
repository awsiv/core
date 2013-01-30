#include "common.h"

#include "install.h"

static Rlist *PHPStringArrayToRlist(zval * php_array, bool prune_empty)
{
    zval **data;
    HashTable *hash;
    HashPosition hashPos;
    Rlist *rp = NULL;

    hash = Z_ARRVAL_P(php_array);

    for (zend_hash_internal_pointer_reset_ex(hash, &hashPos);
         zend_hash_get_current_data_ex(hash, (void *)&data, &hashPos) == SUCCESS;
         zend_hash_move_forward_ex(hash, &hashPos))
    {
        if (Z_TYPE_PP(data) == IS_STRING)
        {
            if (strlen(Z_STRVAL_PP(data)) != 0 || !prune_empty)
            {
                AppendRlist(&rp, Z_STRVAL_PP(data), 's');
            }
        }
    }

    return rp;
}

void HostClassFilterAddIncludeExcludeLists(HostClassFilter *filter, zval * includes, zval * excludes)
{
    assert(filter);

    Rlist *includeRlist = PHPStringArrayToRlist(includes, true);
    Rlist *excludeRlist = PHPStringArrayToRlist(excludes, true);

    HostClassFilterAddClassLists(filter, includeRlist, excludeRlist);

    DeleteRlist(includeRlist);
    DeleteRlist(excludeRlist);
}

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t total)
{
    JsonElement *meta = JsonObjectCreate(4);
    JsonObjectAppendInteger(meta, "total", total);
    JsonObjectAppendInteger(meta, "page", page);
    JsonObjectAppendInteger(meta, "count", JsonElementLength(data_array));
    JsonObjectAppendInteger(meta, "timestamp", time(NULL));

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data_array);

    return output;
}

JsonElement *JsonObjectWrapper(JsonElement *data, int total_result_count)
{
    assert(data);

    JsonElement *meta = JsonObjectCreate(2);

    JsonObjectAppendInteger(meta, "count", total_result_count);

    JsonElement *output = JsonObjectCreate(2);

    JsonObjectAppendArray(output, "data", data);
    JsonObjectAppendObject(output, "meta", meta);

    return output;
}

bool PHPArrayStringGet(zval *php_array, char *key, char *buffer, int bufsize)
{
    assert( php_array );
    assert( key );
    assert( strlen(key) > 0 );
    assert( buffer );
    assert( bufsize > 0 );

    zval **zvalue;
    bool retval = zend_hash_find(Z_ARRVAL_P(php_array), key, strlen(key) + 1, (void**)&zvalue) != FAILURE;

    assert( retval && "Couldn't find value for key" && key);

    if( retval )
    {
        snprintf( buffer, bufsize, "%s", Z_STRVAL_PP(zvalue));
    }

    return retval;
}

bool PHPArrayBoolGet(zval *php_array, char *key, bool *out)
{
    assert( php_array );
    assert( key );
    assert( strlen(key) > 0 );

    zval **zvalue;
    bool retval = zend_hash_find(Z_ARRVAL_P(php_array), key, strlen(key) + 1, (void**)&zvalue) != FAILURE;
    assert( retval && "Couldn't find value for key" && key);

    if(retval)
    {
        *out = Z_BVAL_PP(zvalue);
    }

    return retval;
}

bool PHPArrayIsEmpty(zval *php_array)
{
    return zend_hash_num_elements(Z_ARRVAL_P(php_array)) == 0;
}

WebReportFileInfo *PHPArrayWebReportFileInfoGet( zval *php_array )
{
    assert( php_array );

    bool retval = false;

    bool csv_create;
    retval = PHPArrayBoolGet(php_array, cfphp_csv_create, &csv_create);
    assert(retval);

    bool pdf_create;
    retval |= PHPArrayBoolGet(php_array, cfphp_pdf_create, &pdf_create);
    assert(retval);

    if(!retval)
    {
        return NULL;
    }

    char path[CF_MAXVARSIZE] = "\0";
    if(!PHPArrayStringGet(php_array, cfphp_report_path, path, CF_MAXVARSIZE - 1))
    {
        return NULL;
    }

    char filename[CF_MAXVARSIZE] = "\0";
    if(!PHPArrayStringGet(php_array, cfphp_report_filename, filename, CF_MAXVARSIZE - 1))
    {
        return NULL;
    }

    char request_id[CF_MAXVARSIZE] = "\0";
    if(!PHPArrayStringGet(php_array, cfphp_request_id, request_id, CF_MAXVARSIZE - 1))
    {
        return NULL;
    }

    return NewWebReportFileInfo(csv_create|pdf_create, path, filename, request_id);
}

