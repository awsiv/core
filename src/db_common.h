#ifndef CFENGINE_NOVA_DB_COMMON_H
#define CFENGINE_NOVA_DB_COMMON_H

#include "cf.nova.h"

bool CFDB_Open(EnterpriseDB *conn);
bool CFDB_Close(EnterpriseDB *conn);

const char *DBHostname();
int DBPort();

bool MongoCheckForError(EnterpriseDB *conn, const char *operation, const char *extra, bool *checkUpdate);

#endif
