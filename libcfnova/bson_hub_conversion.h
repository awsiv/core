#ifndef CFENGINE_NOVA_BSON_HUB_CONVERSION_H
#define CFENGINE_NOVA_BSON_HUB_CONVERSION_H

#include "cf3.defs.h"
#include "cf3.extern.h"

/* CFE data structures */
HubPromiseCompliance *BsonIteratorGetPromiseCompliance(bson_iterator *it, HubHost *hh, char *rhandle);
HubBundleSeen *BsonIteratorGetBundleSeen(bson_iterator *it, HubHost *hh, char *rname);

bool BsonIterGetBundleReportDetails(bson_iterator *it, char *lname, time_t blueHorizonTime,
                                    HubHost *hh, Rlist **record_list,
                                    PromiseContextMode promise_context, WebReportFileInfo *wr_info,
                                    Writer *writer, int db_options );
bool BsonIterGetPromiseComplianceDetails(bson_iterator *it, char *lhandle, PromiseState lstatus,
                                         time_t from, time_t to, time_t blueHorizonTime,
                                         HubHost *hh, Rlist **record_list,
                                         PromiseContextMode promise_context,
                                         WebReportFileInfo *wr_info, Writer *writer,
                                         int db_options);
void CFDB_ScanHubHost(bson_iterator *it1, char *keyhash, char *ipaddr, char *hostnames);

/* utilities */
bool CompareStringOrRegex(const char *value, const char *compareTo, bool regex);
bool IsTimeWithinRange(time_t from, time_t to, time_t t);

#endif
