#include "common.h"

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
