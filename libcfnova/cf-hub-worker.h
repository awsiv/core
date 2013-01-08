/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_HUB_WORKER_H
#define CFENGINE_HUB_WORKER_H

Item *Nova_ScanClients(void);
void Nova_SequentialScan(Item *masterlist);
void DBRefreshHostsListCache(Item *list);

#endif
