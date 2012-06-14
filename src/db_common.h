#ifndef CFENGINE_NOVA_DB_COMMON_H
#define CFENGINE_NOVA_DB_COMMON_H

#include <mongo.h>

int CFDB_Open(EnterpriseDB *conn);
int CFDB_Close(EnterpriseDB *conn);

bool MongoCheckForError(EnterpriseDB *conn, const char *operation, const char *extra, bool *checkUpdate);

#endif
