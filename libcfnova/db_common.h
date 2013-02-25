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

int MongoRunCommand(EnterpriseDB *conn, const char *ns, const bson *query, bson *out);

/* write/modify */
int MongoUpdate( EnterpriseDB *conn, const char *ns, const bson *cond,
                 const bson *op, int flags, mongo_write_concern *custom_write_concern );
int MongoInsert( EnterpriseDB *conn, const char *ns,
                 const bson *bson, mongo_write_concern *custom_write_concern );
int MongoRemove( EnterpriseDB *conn, const char *ns, const bson *cond,
                 mongo_write_concern *custom_write_concern );
int MongoCount( EnterpriseDB *conn, const char *db, const char *coll,
                 const bson *query );
bool MongoCursorNext(mongo_cursor *cursor);
bool MongoDropCollection(EnterpriseDB *conn, const char *db, const char *coll, bson *out);

#endif
