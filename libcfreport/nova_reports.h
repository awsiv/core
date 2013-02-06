#ifndef CFENGINE_NOVA_REPORTS_H
#define CFENGINE_NOVA_REPORTS_H

#include "cf3.defs.h"

void Nova_ImportReports(const char *input_file);
int Nova_ExportReports(char *reportName);
void Nova_CommandAPI(char *lsdata, char *name, char *handle, char *hostkey, char *classregex);

void CSV2XML(Rlist *list);

void SummarizeValue(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeVariables(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeCompliance(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizePromiseRepaired(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizePromiseNotKept(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizePerPromiseCompliance(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeSetuid(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeFileChanges(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeSoftware(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);
void SummarizeUpdates(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web);

void GrandSummary(void);

#endif
