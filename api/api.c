#include "api.h"

#include "utils.h"

static const char *API_NAME = "CFEngine Enterprise API";
static const char *API_VERSION = "v2";

PHP_FUNCTION(cfapi)
{
    JsonElement *info = JsonObjectCreate(4);

    JsonObjectAppendString(info, "apiName", API_NAME);
    JsonObjectAppendString(info, "apiVersion", API_VERSION);

    JsonElement *output = JsonArrayCreate(1);
    JsonArrayAppendObject(output, info);

    RETURN_JSON(output);
}
