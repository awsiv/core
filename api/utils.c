/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "utils.h"
#include "cf.nova.h"
#include "db_common.h"
#include "install.h"
#include "string_lib.h"

static JsonElement *PackageResultMeta(size_t page, size_t count, size_t total);

JsonElement *PackageResult(JsonElement *data_array, size_t page, size_t total)
{
    JsonElement *meta = PackageResultMeta(page, JsonElementLength(data_array), total);

    JsonElement *output = JsonObjectCreate(2);
    JsonObjectAppendObject(output, "meta", meta);
    JsonObjectAppendArray(output, "data", data_array);

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
        syslog(LOG_ERR, "Acquiring database connection FAILED");
        free(conn);
        return NULL;
    }
    return conn;
}

bool EnterpriseDBRelease(EnterpriseDB *conn)
{
    assert(conn);
    bool released = CFDB_Close(conn);
    if (!released)
    {
        syslog(LOG_ERR, "Releasing database connection FAILED");
    }
    free(conn);
    return released;
}

Trinary PHPZvalToTrinary(const zval *null_or_bool)
{
    switch (Z_TYPE_P(null_or_bool))
    {
    case IS_BOOL:
        if (Z_LVAL_P(null_or_bool))
        {
            return TRINARY_TRUE;
        }
        else
        {
            return TRINARY_FALSE;
        }

    case IS_NULL:
        return TRINARY_UNDEFINED;

    default:
        assert("Never reach");
        return TRINARY_UNDEFINED;
    }
}

bool PHPArrayIsStringArray(const zval *php_array)
{
    zval **data;
    HashTable *hash;
    HashPosition hashPos;

    switch (Z_TYPE_P(php_array))
    {
    case IS_ARRAY:
        break;

    default:
        return false;
    }

    hash = Z_ARRVAL_P(php_array);

    for (zend_hash_internal_pointer_reset_ex(hash, &hashPos);
         zend_hash_get_current_data_ex(hash, (void *)&data, &hashPos) == SUCCESS;
         zend_hash_move_forward_ex(hash, &hashPos))
    {
        if (Z_TYPE_PP(data) != IS_STRING)
        {
            return false;
        }
    }

    return true;
}

Rlist *PHPStringArrayToRlist(zval *php_array, bool prune_empty)
{
    zval **data;
    HashTable *hash;
    HashPosition hashPos;

    switch (Z_TYPE_P(php_array))
    {
    case IS_NULL:
    case IS_ARRAY:
        break;

    default:
        assert(false && "Not an array or null");
        return NULL;
    }

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
                RlistAppend(&rp, Z_STRVAL_PP(data), 's');
            }
        }
    }

    return rp;
}

Seq *PHPStringArrayToSequence(zval *php_array, bool prune_empty)
{
    zval **data;
    HashTable *hash;
    HashPosition hashPos;

    switch (Z_TYPE_P(php_array))
    {
    case IS_NULL:
        return NULL;

    case IS_ARRAY:
        break;

    default:
        assert(false && "Not an array or null");
        return NULL;
    }

    Seq *result = SeqNew(100, free);
    hash = Z_ARRVAL_P(php_array);

    for (zend_hash_internal_pointer_reset_ex(hash, &hashPos);
         zend_hash_get_current_data_ex(hash, (void *)&data, &hashPos) == SUCCESS;
         zend_hash_move_forward_ex(hash, &hashPos))
    {
        if (Z_TYPE_PP(data) == IS_STRING)
        {
            if (strlen(Z_STRVAL_PP(data)) != 0 || !prune_empty)
            {
                SeqAppend(result, SafeStringDuplicate(Z_STRVAL_PP(data)));
            }
        }
    }

    return result;
}

Rlist *StringSequenceToRlist(Seq *seq)
{
    assert(seq);
    Rlist *list = NULL;
    for (size_t i = 0; i < seq->length; i++)
    {
        RlistAppend(&list, seq->data[i], RVAL_TYPE_SCALAR);
    }
    return list;
}

void HostClassFilterAddIncludeExcludeLists(HostClassFilter *filter, zval * includes, zval * excludes)
{
    assert(filter);

    Rlist *includeRlist = PHPStringArrayToRlist(includes, true);
    Rlist *excludeRlist = PHPStringArrayToRlist(excludes, true);

    HostClassFilterAddClassLists(filter, includeRlist, excludeRlist);

    RlistDestroy(includeRlist);
    RlistDestroy(excludeRlist);
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