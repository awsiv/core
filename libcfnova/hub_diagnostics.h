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

typedef struct
{
    double time;
    Buffer *kh;
    int data_len;
    ReportCollectionStatus status;
} DiagnosticReportingHost;

typedef struct
{
    time_t timestamp;
    int collected_report_count;
    double total_duration_time;
    double avg_duration_time;
    double largest_duration_time;
    Buffer *largest_duration_time_host_id;
    int avg_data_size;
    int largest_data_size;
    Buffer *largest_data_size_host_id;
    Seq *collection_failed_list; /* DiagnosticReportingHost list */
    double sample_analyze_duration;
} DiagnosticHubSnapshot;


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

/**
  @short Query hub diagnostic database
  @param conn Database connection handle
  @return Sequence of DiagnosticHubSnapshot's. Return NULL on error.
  */
Seq *DiagnosticQueryHubSnapshot(EnterpriseDB *conn);

/**
  @short Convert ReportCollectionStatus to string.
  @param status ReportCollectionStatus to convert.
  @return Converted string.
  */
const char *ReportCollectionStatusToString(ReportCollectionStatus status);

#endif
