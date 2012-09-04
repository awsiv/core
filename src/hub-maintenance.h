/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_HUB_MAINTENANCE_H
#define CFENGINE_HUB_MAINTENANCE_H

void Nova_Maintain(void);

void Nova_CacheTotalCompliance(mongo_connection *dbconn, bool allSlots);

void Nova_CacheTotalComplianceEnv(mongo_connection *conn, char *envName, char *envClass, int slot,
                                  time_t start, time_t now);

#endif
