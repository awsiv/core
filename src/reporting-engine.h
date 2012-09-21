/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef REPORTING_ENGINE_H
#define REPORTING_ENGINE_H

#include "cf.nova.h"
#include "install.h"
#if defined(HAVE_LIBSQLITE3)
#include "sqlite3.h"
#endif

#define SQL_TABLE_COUNT 8

#define SQL_TABLE_HOSTS "Hosts"
#define SQL_TABLE_FILECHANGES "FileChanges"
#define SQL_TABLE_CONTEXTS "Contexts"
#define SQL_TABLE_VARIABLES "Variables"
#define SQL_TABLE_SOFTWARE "Software"
#define SQL_TABLE_PROMISESTATUS "PromiseStatusLast"
#define SQL_TABLE_PROMISEDEFINITIONS "PromiseDefinitions"

#define CREATE_SQL_HOSTS "CREATE TABLE " SQL_TABLE_HOSTS "(" \
                         "HostKey VARCHAR(100) PRIMARY KEY, " \
                         "Name VARCHAR(100), " \
                         "IPAddress VARCHAR(50), " \
                         "ReportTimeStamp BIGINT);"

#define CREATE_SQL_FILECHANGES "CREATE TABLE " SQL_TABLE_FILECHANGES "(" \
                               "HostKey VARCHAR(100), " \
                               "FileName VARCHAR(400), " \
                               "ChangeTimeStamp BIGINT, " \
                               "FOREIGN key(hostkey) REFERENCES hosts(hostkey));"

#define CREATE_SQL_CONTEXTS "CREATE TABLE " SQL_TABLE_CONTEXTS "(" \
                            "HostKey VARCHAR(100), " \
                            "Name VARCHAR(50), " \
                            "DefineTimeStamp BIGINT, " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_VARIABLES "CREATE TABLE " SQL_TABLE_VARIABLES "(" \
                             "HostKey VARCHAR(100), " \
                             "NameSpace VARCHAR(50), " \
                             "Bundle VARCHAR(50), " \
                             "Name VARCHAR(50), " \
                             "Value VARCHAR(100), " \
                             "Type VARCHAR(20), " \
                             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_SOFTWARE "CREATE TABLE " SQL_TABLE_SOFTWARE "(" \
                            "HostKey VARCHAR(100), " \
                            "Name VARCHAR(50), " \
                            "Version VARCHAR(50), " \
                            "Architecture VARCHAR(20), " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_PROMISESTATUS "CREATE TABLE " SQL_TABLE_PROMISESTATUS "(" \
                                 "HostKey VARCHAR(100), " \
                                 "PromiseHandle VARCHAR(50), " \
                                 "PromiseStatus VARCHAR(10), " \
                                 "CheckTimeStamp BIGINT, " \
                                 "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey), " \
                                 "FOREIGN KEY(PromiseHandle) REFERENCES PromiseDefinitions(PromiseHandle));"

#define CREATE_SQL_PROMISEDEFINITIONS "CREATE TABLE " SQL_TABLE_PROMISEDEFINITIONS "(" \
                                      "NameSpace VARCHAR(50), " \
                                      "PromiseHandle VARCHAR(50), " \
                                      "Promiser VARCHAR(50), " \
                                      "Bundle VARCHAR(50), " \
                                      "Promisee VARCHAR(100));"

JsonHeaderTable *EnterpriseExecuteSQL(const char *username, const char *select_op);

#if defined(HAVE_LIBSQLITE3)
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

#endif
