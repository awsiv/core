/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef REPORTING_ENGINE_H
#define REPORTING_ENGINE_H

#include "cf.nova.h"
#include "json.h"

JsonElement *EnterpriseExecuteSQL(const char *username, const char *select_op,
                                  Rlist *context_include, Rlist *context_exclude);

#endif
