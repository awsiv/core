/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_CFMOD_PRIVATE_API_H
#define CFENGINE_CFMOD_PRIVATE_API_H

#include "php_cfmod.h"

/*! \brief Get the name and version of an enterprise version of CFEngine
\verbatim
{
  name: Nova,
  version: "2.2.0"
}
\endverbatim
 */
PHP_FUNCTION(cfpr_enterprise_version);

/*! \brief Get the name and version of the core/community version
\verbatim
CFEngine Core 3.0.0
\endverbatim
 */
PHP_FUNCTION(cfpr_community_version);

/*! \brief Gets a license summary.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicense_summary);

/*! \brief Gets the license expiry date.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicense_expiry);

/*! \brief Gets the owner of the license.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicense_owner);

/*! \brief Gets the number of licenses the policy promises to have.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicenses_promised);

/*! \brief Gets the number of licenses granted by the license.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicenses_granted);

/*! \brief Gets the date the license file was written to the masterfiles directory
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_getlicense_installtime);

/*! \brief
 * \param hostKey
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_get_class_frequency);
PHP_FUNCTION(cfpr_report_class_frequency);

/*! \brief A string describing the network speed to the host
 * \param hostKey
\verbatim
124 Delta; 44636346 bytes/s
\endverbatim
 */
PHP_FUNCTION(cfpr_network_speed);

/*! \brief A JSON document summarizing the meters for all the hosts.
 */
PHP_FUNCTION(cfpr_summary_meter);

/*! \brief A JSON document listing the meters for a particular host
 * \param hostKey
 */
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
PHP_FUNCTION(cfpr_report_overall_summary);

/*! \brief A list of vitals descriptions for a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_vitals_list);

/*! \brief A recent time series for a given vital
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_view_magnified);

/*! \brief A time series for the past week
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_view_week);

/*! \brief A time series for the past year
 * \param hostKey
 * \param vitalId
 */

PHP_FUNCTION(cfpr_vitals_view_year);

/*! \breif A histogram of a vital
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_view_histogram);

/*! \brief A string containing recent min, max, average variability values for a vital
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_analyse_magnified);

/*! \brief A string containing min, max, average variability values for a vital, for the past week
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_analyse_week);

/*! \brief A string containing min, max, average variability values for a vital, for the past year
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_analyse_year);

/*! \brief A string containing min, max frequencies, spectral mode for a vital histogram
 * \param hostKey
 * \param vitalId
 */
PHP_FUNCTION(cfpr_vitals_analyse_histogram);

/*! \brief Authenticate against an LDAP database
 * \param hostname
 * \param domain
 * \param password
 * \param startTLS
 * \returns True if successful
 */
PHP_FUNCTION(cfpr_ldap_authenticate);

/*! \brief LDAP Search values for a single attribute
 * \param uri
 * \param user
 * \param baseDN
 * \param filter
 * \param attributeName
 * \param scope
 * \param security
 * \param password
 * \param page
 * \param linesPerPage
 * \param startTLS
 */
PHP_FUNCTION(cfpr_ldap_get_single_attribute_list);

/*! \brief LDAP Search values for a list of attributes
 * \param uri
 * \param user
 * \param baseDN
 * \param filter
 * \param attributeNames
 * \param scope
 * \param security
 * \param password
 * \param page
 * \param linesPerPage
 * \param startTLS
 */
PHP_FUNCTION(cfpr_ldap_get_several_attributes);

/*! \brief Get last date when host was updated
 * \param hostKey
 */
PHP_FUNCTION(cfpr_getlastupdate);

/*! \brief A promise body as JSON
 * \param bodyType
 * \param bodyName
 */
PHP_FUNCTION(cfpr_body_details);

/*! \brief Get the name of an owner bundle
 * \param promiseHandle
 */
PHP_FUNCTION(cfpr_bundle_by_promise_handle);

/*! \brief A list of classes in a bundle
 * \param bundleName
 * \param bundleType
 */
PHP_FUNCTION(cfpr_bundle_classes_used);

/*! \brief A list of parameters for a bundle
 * \param bundleType
 * \param bundleName
 */
PHP_FUNCTION(cfpr_bundle_arguments);

/*! \brief Get the list of business goals
 */
PHP_FUNCTION(cfpr_list_business_goals);

/*! \brief Query reported software entries in a host
 * \param hostKey
 * \param softwareName
 * \param softwareVersion
 * \param softwareArchitecture
 * \param classExpression
 */
PHP_FUNCTION(cfpr_report_software_in);

/*! \brief Query reported patch entries in a host
 * \param hostKey
 * \param patchName
 * \param patchVersion
 * \param patchArchitecture
 * \param classExpression
 * \param sortColumnName
 * \param sortDescending
 */
PHP_FUNCTION(cfpr_report_patch_in);

/*! \brief Query reported patch entries available to a host
 * \param hostKey
 * \param patchName
 * \param patchVersion
 * \param patchArchitecture
 * \param classExpression
 * \param sortColumnName
 * \param sortDescending
 */
PHP_FUNCTION(cfpr_report_patch_avail);

/*! \brief Query reported classes at a host
 * \returns A JSON document with probability of seeing class, standard deviation
 * \param hostKey
 * \param name (leave this NULL)
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 */
PHP_FUNCTION(cfpr_report_classes);

/*! \brief A JSON document with hosts and contexts matching the context expressions
 * \param classExpressions comma-delimited list
 * \param sortColumnName
 * \param sortDescending
 */
PHP_FUNCTION(cfpr_class_cloud);

/*! \brief Query for variables reported by a host
 * \param hostKey
 * \param scope
 * \param variableName
 * \param variableValue
 * \param type
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_report_vars);

/*! \brief Query for a promise compliance report, i.e. proportion of kept promises
 * \param hostKey
 * \param version
 * \param time
 * \param kept
 * \param notKept
 * \param repaired
 * \param comparator
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_compliance_summary);

/*! \brief Get a cached promise compliance report for the barcharts
 * \param policy
 * \param sortColumnName
 * \param sortDescending
 */
PHP_FUNCTION(cfpr_compliance_summary_graph);

/*! \brief Query for a promise compliance report, given a promise handle expression
 * \param hostKey
 * \param handle
 * \param status
 * \param useRegularExpressions
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_compliance_promises);

/*! \brief Query for a report of when hosts were last seen
 * \param hostKey
 * \param hubKey
 * \param hostname
 * \param address
 * \param maxLastSeen
 * \param useRegularExpressions
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_lastseen);

/*! \brief Query for a performance report for a job
 * \param hostKey
 * \param job
 * \param useRegularExpressions
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_performance);

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
PHP_FUNCTION(cfpr_report_setuid);

/*! \brief Query for changed files at a host
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param time
 * \param comparator
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_filechanges);

/*! \brief Query for file diffs
 * \param hostKey
 * \param file
 * \param diff
 * \param useRegularExpressions
 * \param time
 * \param comparator
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_filediffs);

PHP_FUNCTION(cfpr_report_bundlesseen);

/*! \brief Query for changed files at a host since the beginning of time
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param time
 * \param comparator
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_filechanges_longterm);

/*! \brief Query for file diffs since the beginning of time
 * \param hostKey
 * \param file
 * \param diff
 * \param useRegularExpressions
 * \param time
 * \param comparator
 * \param contextExpression
 * \param sortColumnName
 * \param sortDescending
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_filediffs_longterm);

/*! \brief A list of all the library documents
 * \param filePath
 */
PHP_FUNCTION(cfpr_list_documents);

/*! \brief A list of all the time contexts at a host
 * \param userName
 */
PHP_FUNCTION(cfpr_class_list_time);

/*! \brief A list of all the soft contexts at a host
 * \param userName
 */
PHP_FUNCTION(cfpr_class_list_soft);

/*! \brief A list of all the IP contexts at a host
 * \param userName
 */
PHP_FUNCTION(cfpr_class_list_ip);

/*! \brief A list of all contexts at a host
 * \param userName
 */
PHP_FUNCTION(cfpr_class_list_all);

/*! \brief A list of host contexts
 * \param userName
 */
PHP_FUNCTION(cfpr_class_list_hostname);

/*! \brief Query for hosts using some software
 * \param hostKey
 * \param name
 * \param version
 * \param architecture
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_software_in);

/*! \brief Query for hosts with some patch installed
 * \param hostKey
 * \param name
 * \param version
 * \param architecture
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_patch_in);

/*! \brief Query for hosts with some patch available to it
 * \param hostKey
 * \param name
 * \param version
 * \param architecture
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_patch_avail);

/*! \brief Query for hosts with the given classes
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_classes);

/*! \brief Query for hosts with the given classes
 * \param hostKey
 * \param scope
 * \param name
 * \param value
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_vars);

/*! \brief Query for hosts with a given compliance summary
 * \param hostKey
 * \param version
 * \param time
 * \param kept
 * \param notKept
 * \param repaired
 * \param comparator
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_compliance_summary);

/*! \brief Query for hosts with a given promise compliance
 * \param hostKey
 * \param handle
 * \param status
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_compliance_promises);

/*! \brief Query for hosts with a given promise compliance
 * \param hostKey
 * \param hubKey
 * \param hostname
 * \param address
 * \param timeAgo
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_lastseen);

/*! \brief Query for hosts with some performance metric
 * \param hostKey
 * \param job
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_performance);

/*! \brief Query for hosts with setuid programs
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_setuid);

/*! \brief Query for hosts with file changes
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_filechanges);

/*! \brief Query for hosts with file diffs
 * \param hostKey
 * \param file
 * \param diff
 * \param useRegularExpressions
 * \param timeAgo
 * \param comparator
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_filediffs);

/*! \brief Query for hosts with promises repaired
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_repaired);

/*! \brief Query for hosts with promises not kept
 * \param hostKey
 * \param handle
 * \param fromTime
 * \param toTime
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_notkept);

/*! \brief Query for hosts with bundle
 * \param hostKey
 * \param bundle
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_bundlesseen);

/*! \brief Query for hosts with business value
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_hosts_with_value);

/*! \brief Query for a business value report
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_report_value);

/*! \brief Get the barchart values for a business value report
 * \param hostKey
 * \param day
 * \param month
 * \param year
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_get_value_graph);

/*! \brief Get a report of promises not kept
 * \param hostKey
 * \param handle
 * \param hoursFrom
 * \param hoursTo
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_notkept);

/*! \brief Get a report of promises repaired
 * \param hostKey
 * \param handle
 * \param hoursFrom
 * \param hoursTo
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_repaired);

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
PHP_FUNCTION(cfpr_summarize_repaired);

/*! \brief A description of a report
 * \param reportName
 */
PHP_FUNCTION(cfpr_report_description);

/*! \brief Get information about a host with the given key
 * \param hostKey
 */
PHP_FUNCTION(cfpr_host_by_hostkey);

/*! \brief Get the hubKey
 */
PHP_FUNCTION(cfpr_hub_key);

/*! \brief Determine if we are the hub master, or return IP of hub master
 */
PHP_FUNCTION(cfpr_get_hub_master);

/*! \brief Get the topic-id for a topic
 * \param type
 * \param topic
 */
PHP_FUNCTION(cfpr_get_pid_for_topic);

/*! \brief Retrieve a topic
 * \param topicId
 */
PHP_FUNCTION(cfpr_show_topic);

/*! \brief Search for topics
 * \param queryString
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_search_topics);

/*! \brief Get leads for a topic
 * \param topicId
 */
PHP_FUNCTION(cfpr_show_topic_leads);

/*! \brief Get hits for a topic
 * \param topicId
 */
PHP_FUNCTION(cfpr_show_topic_hits);

/*! \brief Get the cateogry of a topic
 * \param topicId
 */
PHP_FUNCTION(cfpr_show_topic_category);

/*! \brief Get the view of a topic
 * \param topicId
 */
PHP_FUNCTION(cfpr_get_knowledge_view);

/*! \brief Get a list of the top N hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_compliance_list_all);

/*! \brief Get a list of selected reports
 */
PHP_FUNCTION(cfpr_select_reports);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_compliance_list_red);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_compliance_list_yellow);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_compliance_list_green);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_compliance_list_blue);

/*! \brief The blue hosts current threshold 
 */
PHP_FUNCTION(cfpr_get_bluehost_threshold);

/*! \brief A list of hosts with an IP address
 * \param ipExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_list_by_ip_rx);

/*! \brief A list of hosts with a hostname
 * \param nameExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_host_list_by_name_rx);

/*! \brief Get the colour of a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_host_compliance_colour);

/*! \brief The number of hosts under management of this hub
 */
PHP_FUNCTION(cfpr_host_count_all);

/*! \brief The number of blue hosts under management of this hub
 */
PHP_FUNCTION(cfpr_host_compliance_count_blue);

/*! \brief The number of green hosts under management of this hub
 */
PHP_FUNCTION(cfpr_host_compliance_count_green);

/*! \brief The number of yellow hosts under management of this hub
 */
PHP_FUNCTION(cfpr_host_compliance_count_yellow);

/*! \brief The number of red hosts under management of this hub
 */
PHP_FUNCTION(cfpr_host_compliance_count_red);


/*! \brief A summary document of a promise
 * \param promiseHandle
 */

PHP_FUNCTION(cfpr_promise_details);

/*! \brief Goals for agent bundles
 * \param bundleType
 */
PHP_FUNCTION(cfpr_bundle_agent_goals);

/*! \brief A list of all bundles
 */
PHP_FUNCTION(cfpr_bundle_list_all);

/*! \brief A list of all bundles using a bundle
 * \param bundleName
 */
PHP_FUNCTION(cfpr_bundle_list_by_bundle_usage);

/*! \brief Get a bundle type by name
 * \param bundleName
 */
PHP_FUNCTION(cfpr_get_bundle_type);

/*! \brief A list of bodies
 * \param bodyType
 * \param bodyName
 */
PHP_FUNCTION(cfpr_body_list);

/*! \brief List handles for policy findler by handle
 * \param handle
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_promise_list_by_handle_rx);

/*! \brief List handles by bundle
 * \param bundle
 */
PHP_FUNCTION(cfpr_promise_list_by_bundle);

/*! \brief List handles by bundle regex
 * \param bundlerx
 */
PHP_FUNCTION(cfpr_promise_list_by_bundle_rx);

/*! \brief List promises
 * \param promiser
 */
PHP_FUNCTION(cfpr_promise_list_by_promiser);

/*! \brief List handles for policy findler promiser
 * \param promiser
 */
PHP_FUNCTION(cfpr_promise_list_by_promiser_rx);

/*! \brief List promises by promise type
 * \param promiseType
 */
PHP_FUNCTION(cfpr_promise_list_by_promise_type);

/*! \brief Delete a host from the database
 * \param hostKey
 */
PHP_FUNCTION(cfpr_delete_host);

/*! \brief A list of all environments
 */
PHP_FUNCTION(cfpr_environment_list);

/*! \brief Get the contents of an environment
 * \param environmentName
 */
PHP_FUNCTION(cfpr_host_list_by_environment);

/*! \brief Get the environment of a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_environment_by_hostkey);

/*! \brief Get the replica status of this hub
 */
PHP_FUNCTION(cfpr_replica_status);


/*
 * SVN helper
 */
/*! \brief Check if a policy file is valid
 * \param file
 */
PHP_FUNCTION(cfpr_validate_policy);

/*
 *  CDP reports
 */
/*! \brief A list of all report names for content-driven policies
 */
PHP_FUNCTION(cfpr_cdp_reportnames);

/*! \brief Get a content-driven policy report for a host
 * \param hostKey
 * \param reportName
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_cdp_report);



/*
 * Commenting
 */

/*! \brief Add to an existing note
 * \param noteId
 * \param user
 * \param date
 * \param note
 */
PHP_FUNCTION(cfpr_add_note);

/*! \brief Create a new note for a report
 * \param hostKey
 * \param reportId
 * \param reportType
 * \param user
 * \param date
 * \param note
 */
PHP_FUNCTION(cfpr_new_note);

/*! \brief Query for a note
 * \param hostKey
 * \param noteId
 * \param user
 * \param fromDate
 * \param toDate
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_query_note);

/*! \brief List note-ids for a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_get_host_noteid);

PHP_FUNCTION(cfpr_user_authenticate);
PHP_FUNCTION(cfpr_role_create);
PHP_FUNCTION(cfpr_role_delete);
PHP_FUNCTION(cfpr_role_update);
PHP_FUNCTION(cfpr_role_list_all);
PHP_FUNCTION(cfpr_role_list_by_name);


/******************************************************************************/
/* Mission Tree-Control (Astrolabe)                                           */
/******************************************************************************/

PHP_FUNCTION(cfpr_astrolabe_host_list);
PHP_FUNCTION(cfpr_astrolabe_host_count);


#endif  /* CFENGINE_CFMOD_PRIVATE_API_H */
