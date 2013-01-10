/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_NOVA_DB_DIAGNOSTICS_H
#define CFENGINE_NOVA_DB_DIAGNOSTICS_H

#include <time.h>

#define diagnostic_db_mongo_coll "cfdiagnostics.mongo"
#define diagnostic_dbk_time "t"

/**
  @brief Function performs MongoDB status snapshot and stores it in database.
  @param time_id Status snapshot identifier (unix timestamp).
  */
void DiagnosticsMakeMongoSnapshot(time_t time_id);

#endif
