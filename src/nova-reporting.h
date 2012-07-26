#ifndef CFENGINE_NOVA_REPORTING_H
#define CFENGINE_NOVA_REPORTING_H

#include "cf3.defs.h"
#include "reporting.h"

void Nova_SummarizePromiseRepaired(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot,
                                   char *web);
void Nova_NoteVarUsage(void);
void Nova_SummarizeComms(void);
void Nova_ZenossSummary(const char *docroot);
void Nova_NoteVarUsageDB(void);

ReportContext *Nova_OpenCompilationReportFiles(const char *fname);
void Nova_ShowPromises(const ReportContext *context, ReportOutputType type, const Bundle *bundles, const Body *bodies);
void Nova_ShowPromise(const ReportContext *context, ReportOutputType type, const char *version, const Promise *pp, int indent);
int Nova_ExportReports(char *reportName);
void Nova_CommandAPI(char *lsdata, char *name, char *handle, char *hostkey, char *classregex);

#ifdef HAVE_LIBMONGOC
#include "db_common.h"

void Nova_ImportReports(const char *input_file);

int Nova_ImportHostReportsFromStream(EnterpriseDB *dbconn, char *header, FILE *fin);
#endif

#endif
