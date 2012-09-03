/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_API_DB_SERIALIZE_H
#define CFENGINE_API_DB_SERIALIZE_H

#include "cf.nova.h"
#include "json.h"

JsonElement *HubUserToJson(const HubUser *user);
JsonElement *HubRoleToJson(const HubRole *role);
JsonElement *HubHostToJson(const HubHost *host);
JsonElement *HubClassToJson(const HubClass *context);

#endif
