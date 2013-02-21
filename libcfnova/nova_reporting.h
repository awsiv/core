#ifndef CFENGINE_NOVA_REPORTING_H
#define CFENGINE_NOVA_REPORTING_H

#include "cf3.defs.h"
#include "reporting.h"

void Nova_SummarizeComms(void);

ReportContext *Nova_OpenCompilationReportFiles(const char *fname);
void Nova_ShowPromises(const ReportContext *context, const Seq *bundles, const Seq *bodies);
void Nova_ShowPromise(const ReportContext *context, const char *version, const Promise *pp, int indent);

#endif
