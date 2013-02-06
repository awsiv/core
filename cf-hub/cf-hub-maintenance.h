/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_HUB_MAINTENANCE_H
#define CFENGINE_HUB_MAINTENANCE_H

void Nova_Maintain(void);

void Nova_CacheTotalCompliance(EnterpriseDB *conn, bool allSlots);

void Nova_CacheTotalComplianceEnv(EnterpriseDB *conn, char *envName, char *envClass, int slot,
                                  time_t start, time_t now, PromiseContextMode promise_context_mode);

#endif
