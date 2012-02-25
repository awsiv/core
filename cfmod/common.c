#include "common.h"

static Rlist *PHPStringArrayToRlist(zval * php_array, bool prune_empty)
{
    zval **data;
    HashTable *hash;
    HashPosition hashPos;
    Rlist *rp = NULL;

    hash = Z_ARRVAL_P(php_array);

    for (zend_hash_internal_pointer_reset_ex(hash, &hashPos);
         zend_hash_get_current_data_ex(hash, (void **) &data, &hashPos) == SUCCESS;
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

JsonElement *JsonObjectWrapper(JsonElement *data, int totalResultCount)
{
    JsonElement *meta = JsonObjectCreate(1);

    JsonObjectAppendInteger(meta, "count", totalResultCount);

    JsonElement *output = JsonObjectCreate(2);

    JsonObjectAppendArray(output, "data", data);
    JsonObjectAppendObject(output, "meta", meta);

    return output;
}
