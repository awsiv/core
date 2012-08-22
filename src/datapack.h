/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_DATAPACK_H
#define CFENGINE_DATAPACK_H

#include "cf.nova.h"
#include "item_lib.h"

#define CFR_PERF "PRF"
#define CFR_CLASS "CLS"
#define CFR_SETUID "SUI"
#define CFR_FCHANGE_OLD "CHG"
#define CFR_FCHANGE "CNG"
#define CFR_FDIFF "DIF"
#define CFR_MONITOR_MAG "MNM"   // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_WEEK "MNW"  // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_YEAR "MNY"  // DEPRECATED from Nova 2.1.0 onwards
#define CFR_MONITOR_HIST "MNH"  // DEPRECATED from Nova 2.1.0 onwards

#define CFR_MONITOR_MG "MOM"
#define CFR_MONITOR_WK "MOW"
#define CFR_MONITOR_YR "MOY"
#define CFR_MONITOR_HG "MOH"

#define CFR_PCOMPLIANCE "PCP"
#define CFR_TCOMPLIANCE "TCP"
#define CFR_SOFTWARE "SOF"
#define CFR_AVAILPATCH "AVL"
#define CFR_PATCHSTATUS "PST"
#define CFR_PROMISEOUT "POT"
#define CFR_VALUE "VAL"
#define CFR_VARS "VAR"          // DEPRECATED from Nova 2.0.3 onwards
#define CFR_VARD "VRD"          // with date stamp
#define CFR_LASTSEEN "SEN"
#define CFR_REPAIRLOG "PRL"
#define CFR_NOTKEPTLOG "NKL"
#define CFR_METER "MET"
#define CFR_SWDATES "SWD"
#define CFR_BUNDLES "BUN"
#define CFR_EXECUTION_STATUS "EXS"

Item **NewReportBook(void);
bool ReportBookHasData(Item **reports);
void UnpackReportBook(EnterpriseDB *dbconn, char *id, Item **reports);
void DeleteReportBook(Item **reports);
int Nova_StoreIncomingReports(char *reply, Item **reports, int current_report);

void Nova_PackAllReports(Item **reply, time_t from, time_t delta1, enum cfd_menu type);

#endif
