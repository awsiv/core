#ifndef CFENGINE_NOVA_DB_COMMON_H
#define CFENGINE_NOVA_DB_COMMON_H

#include <mongo.h>

int CFDB_Open(mongo_connection *conn);
int CFDB_Close(mongo_connection *conn);

bool MongoCheckForError(mongo_connection *conn, const char *operation, const char *extra, bool *checkUpdate);

#endif
