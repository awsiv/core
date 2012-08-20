/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_DATAPACK_H
#define CFENGINE_DATAPACK_H

#include "cf.nova.h"
#include "item_lib.h"

Item **NewReportBook(void);
bool ReportBookHasData(Item **reports);
void UnpackReportBook(EnterpriseDB *dbconn, char *id, Item **reports);
void DeleteReportBook(Item **reports);
int Nova_StoreIncomingReports(char *reply, Item **reports, int current_report);

#endif
