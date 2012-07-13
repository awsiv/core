/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_DATAPACK_H
#define CFENGINE_DATAPACK_H

#include "cf.nova.h"
#include "item_lib.h"

void Nova_PackPerformance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackClasses(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSetuid(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackFileChanges(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackDiffs(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackMonitorMg(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackMonitorWk(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackMonitorYr(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_FormatMonitoringReply(Item **datap, Item **reply, enum cfd_menu type);
void Nova_PackMonitorHist(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackCompliance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSoftware(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackAvailPatches(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackPatchStatus(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_Pack_promise_output_common(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackValueReport(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackVariables2(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackLastSeen(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackTotalCompliance(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackRepairLog(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackNotKeptLog(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackMeter(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackSoftwareDates(Item **reply, char *header, time_t from, enum cfd_menu type);
void Nova_PackBundles(Item **reply, char *header, time_t date, enum cfd_menu type);
void Nova_PackExecutionStatus(Item **reply, char *header);

#ifdef HAVE_LIBMONGOC
void Nova_UnPackPerformance(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackClasses(EnterpriseDB *dbconn, char *id, Item *data);
void Nova_UnPackSetuid(EnterpriseDB *dbconn, char *id, Item *data);
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
