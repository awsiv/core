/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_DATAPACK_H
#define CFENGINE_DATAPACK_H

#include "cf.nova.h"
#include "item_lib.h"

#ifdef HAVE_LIBMONGOC
void Nova_UnPackPerformance(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackClasses(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackSetuid(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackFileChangesOld(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackFileChanges(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackDiffs(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorWeek(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorMag(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorHist(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorYear(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorMg(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorWk(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorYr(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMonitorHg(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackCompliance(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackSoftware(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackAvailPatches(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackPatchStatus(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPack_promise_output_common(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackValueReport(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackVariables(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackVariables2(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackLastSeen(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackTotalCompliance(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackRepairLog(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackNotKeptLog(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackMeter(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackSoftwareDates(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackBundles(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackExecutionStatus(EnterpriseDB *dbconn, char *id, Item *data);

#endif // HAVE_LIBMONGOC

#endif
