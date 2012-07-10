/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "utils.h"
#include "cf.nova.h"
#include "db_common.h"

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

EnterpriseDB *EnterpriseDBAcquire(void)
{
    EnterpriseDB *conn = xmalloc(sizeof(EnterpriseDB));
    if (!CFDB_Open(conn))
    {
        return NULL;
    }
    return conn;
}

bool EnterpriseDBRelease(EnterpriseDB *conn)
{
    assert(conn);
    bool released = CFDB_Close(conn);
    free(conn);
    return released;
}

Rlist *PHPStringArrayToRlist(zval *php_array, bool prune_empty)
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
