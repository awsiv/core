/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_ENTERPRISE_INSTALL_H
#define CFENGINE_ENTERPRISE_INSTALL_H

#include "cf.nova.h"

const HubVitalPoint *HubVitalLastValue(const HubVital *vital);
int HubVitalPointCompare(const void *a, const void *b, void *user_data);

#endif
