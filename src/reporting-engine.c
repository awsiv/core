/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "reporting-engine.h"

JsonElement *ReportingEngineQuery(JsonElement *query)
{
    JsonElement *result = JsonObjectCreate(5);

    JsonObjectAppendString(result, "hello", "world");

    return result;
}
