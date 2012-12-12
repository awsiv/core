/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_API_DB_SERIALIZE_H
#define CFENGINE_API_DB_SERIALIZE_H

#include "cf.nova.h"
#include "json.h"

JsonElement *HubScheduledReportToJson(const HubScheduledReport *scheduled_report);
JsonElement *HubUserToJson(const HubUser *user);
JsonElement *HubRoleToJson(const HubRole *role);
JsonElement *HubHostToJson(const HubHost *host);
JsonElement *HubClassToJson(const HubClass *context);
JsonElement *HubVitalToJson(const HubVital *vital);
JsonElement *HubSettingsToJson(const HubSettings *settings);
JsonElement *HubPromiseToJson(const HubPromise *promise);

HubSettings *HubSettingsFromJson(const JsonElement *json, char error_out[CF_BUFSIZE]);

#endif