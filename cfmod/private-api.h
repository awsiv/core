/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_PRIVATE_API_H
#define CFENGINE_CFMOD_PRIVATE_API_H

#include "php_cfmod.h"

/*! \brief
 * \param hostKey
 * \param contextExpression
 */
// used on assets page to count monitored jobs
// port plan: /context
PHP_FUNCTION(cfpr_get_class_frequency);
PHP_FUNCTION(cfpr_report_class_frequency);

/*! \brief A string describing the network speed to the host
 * \param hostKey
\verbatim
124 Delta; 44636346 bytes/s
\endverbatim
 */

// field in host. used on asset page.
// port plan: /host
PHP_FUNCTION(cfpr_network_speed);

/*! \brief A JSON document summarizing the meters for all the hosts.
 */
// old status graph
// used on new compliance models prototype page
// port plan: needs to be redone or deprecated
PHP_FUNCTION(cfpr_summary_meter);

/*! \brief A JSON document listing the meters for a particular host
 * \param hostKey
 */

// old hosts status graph
// not currently used
// port plan: deprecate (wait and see)
PHP_FUNCTION(cfpr_host_meter);

/*! \brief A JSON document summarizing performance data for a detailed query.
 * \param hostKey
 * \param promiseHandle
 * \param promiseStatus c (compliant), r (repaired) or n (not kept), x (any)
 * \param context
\verbatim
{
  kept: 12.34,
  not_kept: 24.56,
  repaired: 15.22,
  host_count: 50,
  code_blue: 3,
  class: linux,
  start: 50234,
  end: 60214
}
\endverbatim
 */

// made in april 2012, feature for jpmc, replace summary reports
PHP_FUNCTION(cfpr_hosts_compliance_for_bundles);
PHP_FUNCTION(cfpr_hosts_compliance_for_promises);

/*! \brief A list of vitals descriptions for a host
 * \param hostKey
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_list);

/*! \brief A recent time series for a given vital
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_view_magnified);

/*! \brief A time series for the past week
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_view_week);

/*! \brief A time series for the past year
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_view_year);

/*! \brief Special vitals function, should be killed ASAP, replaced by RE or something
 */
PHP_FUNCTION(cfpr_hosts_sorted_by_last_vital_value);


/*! \brief A histogram of open ports
 */
// port plan: (further investigate)
PHP_FUNCTION(cfpr_service_histogram);

/*! \brief A histogram of levels on a given port
 */
// port plan: (further investigate)
PHP_FUNCTION(cfpr_service_level_histogram);

/*! \breif A histogram of a vital
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_view_histogram);

/*! \brief A string containing recent min, max, average variability values for a vital
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_analyse_magnified);

/*! \brief A string containing min, max, average variability values for a vital, for the past week
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_analyse_week);

/*! \brief A string containing min, max, average variability values for a vital, for the past year
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_analyse_year);

/*! \brief A string containing min, max frequencies, spectral mode for a vital histogram
 * \param hostKey
 * \param vitalId
 */
// port plan: /host/vitals
PHP_FUNCTION(cfpr_vitals_analyse_histogram);

/*! \brief Get last date when host was updated
 * \param hostKey
 */
// returns host cfr_day, or 'never'
// port plan: /host/:id
PHP_FUNCTION(cfpr_getlastupdate);

/*! \brief A promise body as JSON
 * \param bodyType
 * \param bodyName
 */
// port plan: /policy
PHP_FUNCTION(cfpr_body_details);

/*! \brief Get the name of an owner bundle
 * \param promiseHandle
 */
// port plan: /policy
PHP_FUNCTION(cfpr_bundle_by_promise_handle);

/*! \brief A list of classes in a bundle
 * \param bundleName
 * \param bundleType
 */
// port plan: /context
PHP_FUNCTION(cfpr_bundle_classes_used);

/*! \brief A list of parameters for a bundle
 * \param bundleType
 * \param bundleName
 */
// port plan: /policy
PHP_FUNCTION(cfpr_bundle_arguments);

/*! \brief A JSON document with hosts and contexts matching the context expressions
 * \param classExpressions comma-delimited list
 * \param sortColumnName
 * \param sortDescending
 */
// not used anymore
// port plan: deprecate
PHP_FUNCTION(cfpr_class_cloud);

/*! \brief Query for variables reported by a host
 * \param hostKey
 * \param scope
 * \param variableName
 * \param variableValue
 * \param type
 * \param contextExpression
 */
// port plan: /query
PHP_FUNCTION(cfpr_report_vars);

/*! \brief Get a cached promise compliance report for the barcharts
 * \param policy
 * \param sortColumnName
 * \param sortDescending
 */
// old status graph
// now used in new prototype compliance model page
// port plan: deprecate
PHP_FUNCTION(cfpr_compliance_summary_graph);

/*! \brief Query for lastknown promise compliance report, given a promise handle expression and hostcolour
 * \param hostKey
 * \param hostcolour
 * \param handle
 * \param status
 * \param useRegularExpressions
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
// after a compliance by promise report, show how many hosts fall into each color category
// given the select policy context
// port plan: (further investigate)
PHP_FUNCTION(cfpr_report_lastknown_compliance_promises);

/*! \brief Query for programs with setuid
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */

// port plan: /query, /host/setuid, (deprecate)
PHP_FUNCTION(cfpr_report_setuid);

/*! \brief A list of all the library documents
 * \param filePath
 */
// port plan: (further investigate)
PHP_FUNCTION(cfpr_list_documents);

/*! \brief A list of all the time contexts at a host
 * \param userName
 */
// port plan: /context
PHP_FUNCTION(cfpr_class_list_time_distinct_by_name_rx);

/*! \brief A list of all the soft contexts at a host
 * \param userName
 */
// port plan: /context
PHP_FUNCTION(cfpr_class_list_soft_distinct_by_name_rx);

/*! \brief A list of all contexts at a host
 * \param userName
 */
// port plan: /host/context
PHP_FUNCTION(cfpr_class_list_distinct_by_name_rx);

/*! \brief Query for hosts with setuid programs
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param contextExpression
 */

// port plan: /query
PHP_FUNCTION(cfpr_hosts_with_setuid);

/*! \brief Query for hosts with promises repaired
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 */
// port plan: /query
PHP_FUNCTION(cfpr_hosts_with_repaired);

/*! \brief Query for hosts with promises not kept
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 */
// port plan: /query
PHP_FUNCTION(cfpr_hosts_with_notkept);

/*! \brief Query for hosts with business value
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
// business value graph on compliance models page
// port plan: /query
PHP_FUNCTION(cfpr_hosts_with_value);

/*! \brief Query for a business value report
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
// port plan: /query
PHP_FUNCTION(cfpr_report_value);

/*! \brief Get the barchart values for a business value report
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
// bar chart on compliance models page
// port plan: (further investigate)
PHP_FUNCTION(cfpr_get_value_graph);

/*! \brief Get a summary report of promises not kept
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
// port plan: /query
PHP_FUNCTION(cfpr_summarize_notkept);

/*! \brief Get a summary report of promises repaired
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
// port plan: /query
PHP_FUNCTION(cfpr_summarize_repaired);

/*! \brief Get information about a host with the given key
 * \param hostKey
 */
// port plan: /host/:id
PHP_FUNCTION(cfpr_host_by_hostkey);

/*! \brief Get the hubKey
 */
// port plan: /host?context=AM_POLICY_HUB
PHP_FUNCTION(cfpr_hub_key);

/*! \brief Determine if we are the hub master, or return IP of hub master
 */
// port plan: (further investigate)
PHP_FUNCTION(cfpr_get_hub_master);

/*! \brief Get a list of the top N hosts (sorted by compliance score)
 * \param resultsPerPage
 * \param pageNumber
 */
// port plan: /query
PHP_FUNCTION(cfpr_host_compliance_list_all);

/*! \brief Get a list of selected reports
 */
// port plan: /query
PHP_FUNCTION(cfpr_select_reports);

/*! \brief A list of hosts by colour (sorted by host name)
 * \param colour (green/yellow/red/blue)
 * \param resultsPerPage
 * \param pageNumber
 */
// takes a color and returns a list
// port plan: /host?color
PHP_FUNCTION(cfpr_host_compliance_list);

/*! \brief A list of hosts with an IP address
 * \param ipExpression
 * \param resultsPerPage
 * \param pageNumber
 */

// port plan: /host?ip
PHP_FUNCTION(cfpr_host_list_by_ip_rx);

/*! \brief A list of hosts with a hostname
 * \param nameExpression
 * \param resultsPerPage
 * \param pageNumber
 */
// port plan: /host?name
PHP_FUNCTION(cfpr_host_list_by_name_rx);

/*! \brief Get the colour of a host
 * \param hostKey
 */
// port plan: /host/color
PHP_FUNCTION(cfpr_host_compliance_colour);

/*! \brief /host/compliance/timeseries
*/
// port plan: (further investigate)
PHP_FUNCTION(cfpr_host_compliance_timeseries);
PHP_FUNCTION(cfpr_host_compliance_timeseries_shifts);

/*! \brief /host/count
 */
// port plan: /host
PHP_FUNCTION(cfpr_host_count);

/*! \brief /host/info
 */
// port plan: /host
PHP_FUNCTION(cfpr_host_info);

/*! \brief A summary document of a promise
 * \param promiseHandle
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_details);

/*! \brief A list of all bundles
 */
// port plan: /policy
PHP_FUNCTION(cfpr_bundle_list_all);

/*! \brief A list of all bundles using a bundle
 * \param bundleName
 */
// port plan: /policy
PHP_FUNCTION(cfpr_bundle_list_by_bundle_usage);

/*! \brief Get a bundle type by name
 * \param bundleName
 */
// port plan: /policy
PHP_FUNCTION(cfpr_get_bundle_type);

/*! \brief A list of bodies
 * \param bodyType
 * \param bodyName
 */
// port plan: /policy
PHP_FUNCTION(cfpr_body_list);

/*! \brief List handles for policy findler by handle
 * \param handle
 * \param useRegularExpressions
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_handle_rx);

/*! \brief List handles by bundle
 * \param bundle
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_bundle);

/*! \brief List handles by bundle regex
 * \param bundlerx
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_bundle_rx);

/*! \brief List promises
 * \param promiser
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_promiser);

/*! \brief List handles for policy findler promiser
 * \param promiser
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_promiser_rx);

/*! \brief List promises by promise type
 * \param promiseType
 */
// port plan: /policy
PHP_FUNCTION(cfpr_promise_list_by_promise_type);

/*! \brief Return reverse lookup host name
 * \param user
 * \param hostkey
 */
PHP_FUNCTION( cfpr_get_reverse_ip_lookup_name );

/*! \brief Update the host identifier variable
 * \param username
 * \param scope
 * \param variable name
 */
// port plan: (further investigate)
PHP_FUNCTION( cfpr_update_host_identifier );
PHP_FUNCTION( cfpr_get_host_identifier );

/*! \brief Resets the host identifier to default hostname ( reverse IP lookup )
 * \param username ( must be admin )
 */
PHP_FUNCTION( cfpr_reset_host_identifier );

/*! \brief A list of all environments
 */
// port plan: deprecate in favor of namespaces
PHP_FUNCTION(cfpr_environment_list);


/*! \brief Get the replica status of this hub
 */
// port plan: further investigate
PHP_FUNCTION(cfpr_replica_status);

/*
 * Commenting
 */

/*! \brief Add note
 * \param noteId for add note
 * \param hostKey for add and create note
 * \param user
 * \param date
 * \param note
 */
// port plan: /host/note, /user/note
PHP_FUNCTION(cfpr_add_note);

/*! \brief Query for a note
 * \param hostKey
 * \param noteId
 * \param user
 * \param filter_user
 * \param fromDate
 * \param toDate
 * \param resultsPerPage
 * \param pageNumber
 */
// port plan: /host/note, /user/note
PHP_FUNCTION(cfpr_query_note);

/*! \brief List note-ids for a host
 * \param hostKey
 */
// port plan: /host/note, /user/note
PHP_FUNCTION(cfpr_get_host_noteid);

// still used by public-api
// port plan: deprecate
PHP_FUNCTION(cfpr_user_authenticate);


/******************************************************************************/
/* Mission Tree-Control (Astrolabe)                                           */
/******************************************************************************/

// port plan: /host
PHP_FUNCTION(cfpr_astrolabe_host_list);
PHP_FUNCTION(cfpr_astrolabe_host_count);

#endif
