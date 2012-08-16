/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef REPORTING_ENGINE_H
#define REPORTING_ENGINE_H

#include "cf.nova.h"
#include "install.h"

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
                         "HostName VARCHAR(100), " \
                         "HostIPAddress VARCHAR(50), " \
                         "HostLastReport BIGINT, " \
                         "HostColour VARCHAR(20));"

#define CREATE_SQL_FILECHANGES "CREATE TABLE " SQL_TABLE_FILECHANGES "(" \
                               "hostkey VARCHAR(100), " \
                               "filename VARCHAR(400), " \
                               "changetime BIGINT, " \
                               "FOREIGN key(hostkey) REFERENCES hosts(hostkey));"

#define CREATE_SQL_CONTEXTS "CREATE TABLE " SQL_TABLE_CONTEXTS "(" \
                            "HostKey VARCHAR(100), " \
                            "ContextName VARCHAR(50), " \
                            "ContextLastDefined BIGINT, " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_VARIABLES "CREATE TABLE " SQL_TABLE_VARIABLES "(" \
                             "HostKey VARCHAR(100), " \
                             "VariableScope VARCHAR(50), " \
                             "VariableName VARCHAR(50), " \
                             "VariableValue VARCHAR(100), " \
                             "VariableType VARCHAR(20), " \
                             "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_SOFTWARE "CREATE TABLE " SQL_TABLE_SOFTWARE "(" \
                            "HostKey VARCHAR(100), " \
                            "SoftwareName VARCHAR(50), " \
                            "SoftwareVersion VARCHAR(50), " \
                            "SoftwareArchitecture VARCHAR(20), " \
                            "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey));"

#define CREATE_SQL_PROMISESTATUS "CREATE TABLE " SQL_TABLE_PROMISESTATUS "(" \
                                 "HostKey VARCHAR(100), " \
                                 "PromiseHandle VARCHAR(50), " \
                                 "PromiseStatus VARCHAR(10), " \
                                 "PromiseLastRun BIGINT, " \
                                 "FOREIGN KEY(HostKey) REFERENCES Hosts(HostKey), " \
                                 "FOREIGN KEY(PromiseHandle) REFERENCES PromiseDefinitions(PromiseHandle));"

#define CREATE_SQL_PROMISEDEFINITIONS "CREATE TABLE " SQL_TABLE_PROMISEDEFINITIONS "(" \
                                      "PromiseHandle VARCHAR(50), " \
                                      "Promiser VARCHAR(50), " \
                                      "Bundle VARCHAR(50), " \
                                      "Promisees VARCHAR(100));"

JsonHeaderTable *EnterpriseExecuteSQL(const char *username, char *select_op,
                                  Rlist *context_include, Rlist *context_exclude);

#endif
