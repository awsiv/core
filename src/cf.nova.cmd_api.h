/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CF_NOVA_CMD_API_H
#define CFENGINE_CF_NOVA_CMD_API_H

#include <stdbool.h>

void Nova_FormatDiff(const char *diffStr, char *returnval, int bufsize);
char *Nova_LongState(char s);
int Nova_TimeWarn(time_t now, time_t then, time_t threshold, char *outStr, int outStrSz);

int Nova2Txt_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize);
int Nova2Txt_software_report(char *key, char *name, char *value, char *arch, bool regex, char *type, char *classreg);
int Nova2Txt_vars_report(char *hostkey, char *scope, char *lval, char *rval, char *type, bool regex, char *classreg);
int Nova2Txt_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               char *classreg);
int Nova2Txt_compliance_promises(char *hostkey, char *handle, char *status, bool regex, char *classreg);
int Nova2Txt_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddr, time_t lago, int lregex,
                             char *classreg);
int Nova2Txt_deadclient_report(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                               char *classreg);
int Nova2Txt_setuid_report(char *hostkey, char *file, bool regex, char *classreg);
int Nova2Txt_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to, char *classreg);
int Nova2Txt_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to, char *classreg);

int Nova2Txt_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg);

#endif
