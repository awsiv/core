#if defined( HAVE_SQLITE3 )

#ifndef CFENGINE_SCHEDULED_REPORTS_H
#define CFENGINE_SCHEDULED_REPORTS_H

/* top level: Level 0 */
#define cfr_user_id "user" /* Must be unique */
#define cfr_user_fullname "fullname"
#define cfr_user_email "email"

#define cfr_query_id "query_id" /* must be unique */
#define cfr_query "query"
#define cfr_created "created"
#define cfr_last_modified "last_modified"
#define cfr_last_run "last_executed"
#define cfr_run_history "run_history" /* Array */
#define cfr_run_classes "run_classes"
#define cfr_enabled "enabled"
#define cfr_already_run "already_run"

void RunScheduledEnterpriseReports(void);

#endif

#endif
