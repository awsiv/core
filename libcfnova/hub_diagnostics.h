/*

  This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_NOVA_HUB_DIAGNOSTICS_H
#define CFENGINE_NOVA_HUB_DIAGNOSTICS_H

#include "buffer.h"
#include "db_common.h"
#include "sequence.h"

/* db namespace */
#define diagnostic_db "local"
#define diagnostic_db_hub diagnostic_db ".hub_stats"
#define diagnostic_db_hub_time "t"

typedef enum
{
    HUBDIAG_SERVER_NO_REPLY,
    HUBDIAG_SERVER_AUTH_FAILURE,
    HUBDIAG_INVALID_DATA,
    HUBDIAG_SUCCESS,
    HUBDIAG_UNKNOWN
} ReportCollectionStatus;


/**
  @short Save report collection stats for host into tmp db collection.
  @param conn Database connection.
  @param  kh Host keyhash. Transfers buffer ownership.
  @param duration Time that took to collect report.
  @param data_len Size of data collected in report (bytes).
  @param status. Status.
  */
void DiagnosticReportHostSave(EnterpriseDB *conn, Buffer *kh, double duration,
                              int data_len, ReportCollectionStatus status);

/**
  @short Function performs tmp host stat analize and store hub stat snapshot into mongo database
  @note This function wait for finishing all write operations to tmp storage.
  @param time_id Snapshot time based id.
  @param total_time Complete duration of reporting round.
  */
void DiagnosticMakeHubSnapshot(time_t time_id, double total_time);

/**
  @short Drop temporary db storege for host stats.
  @note Tmp storage is created and populated by DiagnosticReportHostSave.
  */
void DropReportingHostsTmp(void);

#endif
