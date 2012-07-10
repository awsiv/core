/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef REPORTING_ENGINE_H
#define REPORTING_ENGINE_H

#include "cf.nova.h"
#include "json.h"


JsonElement *ReportingEngineQuery(JsonElement *query);

JsonElement *EnterpriseExecuteSQL(char *select_op);

#endif
