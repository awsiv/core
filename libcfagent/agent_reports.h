/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_AGENT_REPORTS_H
#define CFENGINE_AGENT_REPORTS_H

#include "cf.nova.h"

void NoteEfficiency(double e);

void Nova_NoteVarUsage(void);
void Nova_NoteVarUsageDB(void);
void Nova_TrackExecution(const char *input_file);

void ReportPatches(PackageManager *list);

#endif
