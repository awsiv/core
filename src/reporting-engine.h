/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef REPORTING_ENGINE_H
#define REPORTING_ENGINE_H

#include "cf.nova.h"
#include "install.h"
#include "sqlite3.h"

typedef enum
{
    REPORTING_ENGINE_ASYNC_ERROR_UNKNOWN = -1,
    REPORTING_ENGINE_ASYNC_SUCCESS = 0,

    REPORTING_ENGINE_ASYNC_ERROR_START_PROC,
    REPORTING_ENGINE_ASYNC_ERROR_ENTERPRISE_DB_CONNECT,
    REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_CONNECT,
    REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_PREPARE,
    REPORTING_ENGINE_ASYNC_ERROR_SQLITE3_QUERY,
    REPORTING_ENGINE_ASYNC_ERROR_IO,
    REPORTING_ENGINE_ASYNC_ERROR_UNEXPECTED_CHILD_EXIT,
    REPORTING_ENGINE_ASYNC_ERROR_DOCROOT_NOT_FOUND,

    REPORTING_ENGINE_ASYNC_ERROR_MAX
} ReportingEngineAsyncError;

JsonElement *EnterpriseExecuteSQLSync(const char *username, const char *select_op);
JsonElement *EnterpriseExecuteSQLAsync(const char *username, const char *select_op);

JsonElement *AsyncQueryStatus(const char *token, int report_type, const char *static_files_uri);
JsonElement *AsyncQueryAbort(const char *token);

bool Sqlite3_DBOpen(sqlite3 **db);
void Sqlite3_DBClose(sqlite3 *db);
bool Sqlite3_Execute(sqlite3 *db, const char *sql, void *fn_ptr, void *arg_to_callback, char *err_msg);
cfapi_errid LoadSqlite3Tables(sqlite3 *db, Rlist *tables, const char *username);

void Sqlite3_FreeString(char *str);

/* Output generation */
int BuildJsonOutput(void *out, int argc, char **argv, char **azColName);
int BuildCSVOutput(void *out, int argc, char **argv, char **azColName);

bool GenerateAllTables(sqlite3 *db);
Rlist *GetTableNamesInQuery(const char *select_op);
#endif
