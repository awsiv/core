#ifndef CFENGINE_NOVA_DB_COMMON_H
#define CFENGINE_NOVA_DB_COMMON_H

#include <mongo.h>

int CFDB_Open(mongo_connection *conn);
int CFDB_Close(mongo_connection *conn);

#endif
