/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_NOVA_DB_DIAGNOSTICS_H
#define CFENGINE_NOVA_DB_DIAGNOSTICS_H

#include <time.h>

#include "db_common.h"

#define diagnostic_db_mongo_coll "local.mongo_stats"
#define diagnostic_dbk_time "t"

/*
 * Internal data structures
 */

typedef struct
{
    char *name;
    int object_count;
    int data_size;
    double avg_object_size;
    int storage_size;
    int index_count;
    double padding_factor;
    int total_index_size;
} DiagnosticCollectionStatus;

typedef struct
{
    char *db_name;
    int object_count;
    double avg_object_size;
    int data_size;
    int storage_size;
    int file_size;
    Seq *collection_list; /* DiagnosticCollectionStatus list */
} DiagnosticDatabaseStatus;

typedef struct
{
    char *host;
    char *version;
    double server_uptime;
    double global_lock_total_time_us;
    double global_lock_lock_time_us;
    int global_lock_queue_total;
    int global_lock_queue_readers;
    int global_lock_queue_writers;
    int mem_resident;
    int mem_virtual;
    int mem_mapped;
    int bg_flush_count;
    int bg_flush_total_ms;
    int bg_flush_avg_ms;
    int bg_flush_last_ms;
    Seq *db_list; /* DiagnosticDatabaseStatus list */
} DiagnosticServerStatus;

typedef struct
{
    DiagnosticServerStatus *status;
    time_t time;
} DiagnosticMongoSnaphot;

/**
  @brief Function performs MongoDB status snapshot and stores it in database.
  @param time_id Status snapshot identifier (unix timestamp).
  */
void DiagnosticsMakeMongoSnapshot(time_t time_id);

/**
  @brief Query mongo diagnostic snaphots (return all records).
  @return Return a Sequence of DiagnosticMongoSnaphot data structures.
  */
Seq *DiagnosticsQueryMongoSnapshot(EnterpriseDB *conn);

#endif
