/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "utils.h"
#include "cf.nova.h"
#include "db_common.h"
#include "install.h"

static JsonElement *PackageResultMeta(size_t page, size_t count, size_t total);

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t total)
{
    JsonElement *meta = PackageResultMeta(page, JsonElementLength(data_array), total);

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data_array);

    return output;
}

JsonElement *PackageResultSQL(JsonHeaderTable *table)
{
    // we only support one SQL-query at the time for now
    JsonElement *meta = PackageResultMeta(1, 1, 1);

    JsonElement *data1 = JsonObjectCreate(3);
    JsonObjectAppendString(data1, "query", table->title);
    JsonObjectAppendArray(data1, "header", table->header);
    JsonObjectAppendInteger(data1, "row_count", JsonElementLength(table->rows));
    JsonObjectAppendArray(data1, "rows", table->rows);

    JsonElement *data = JsonArrayCreate(1);
    JsonArrayAppendObject(data, data1);

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data);

// transfer of ownership
    free(table->title);
    free(table);

    return output;
}

static JsonElement *PackageResultMeta(size_t page, size_t count, size_t total)
{
    JsonElement *meta = JsonObjectCreate(4);
    JsonObjectAppendInteger(meta, "page", page);
    JsonObjectAppendInteger(meta, "count", count);
    JsonObjectAppendInteger(meta, "total", total);
    JsonObjectAppendInteger(meta, "timestamp", time(NULL));

    return meta;
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

const char *JsonPrimitiveTypeToString(JsonPrimitiveType type)
{
    switch (type)
    {
    case JSON_PRIMITIVE_TYPE_STRING:
        return "string";
    case JSON_PRIMITIVE_TYPE_REAL:
    case JSON_PRIMITIVE_TYPE_INTEGER:
        return "number";
    case JSON_PRIMITIVE_TYPE_BOOL:
        return "boolean";
    default:
        assert(false && "Never reach");
        return "(null)";
    }
}
