#ifndef CFENGINE_NOVA_DB_COMMON_H
#define CFENGINE_NOVA_DB_COMMON_H

#include "cf.nova.h"

bool CFDB_Open(EnterpriseDB *conn);
bool CFDB_Close(EnterpriseDB *conn);

const char *DBHostname();
int DBPort();

bool MongoCheckForError(EnterpriseDB *conn, const char *operation,
                        const char *extra, bool *checkUpdate);

/* read functions */
mongo_cursor *MongoFind( EnterpriseDB *conn, const char *ns, const bson *query,
                          const bson *fields, int limit, int skip, int options );

int MongoFindOne( EnterpriseDB *conn, const char *ns, const bson *query,
                    const bson *fields, bson *out );

#endif
