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

/*! \brief Path where we think web applications are installed.
 *  \warning Reads from scratch area.
 */
PHP_FUNCTION(cfpr_docroot);

/*! \brief
 * \param hostKey
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_get_class_frequency);
PHP_FUNCTION(cfpr_report_class_frequency);

/*! \brief A string describing the network rate to the host
 * \param hostKey
\verbatim
124 Delta; 44636346 bytes/s
\endverbatim
 */
PHP_FUNCTION(cfpr_get_network_rate);

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
 * \param bodyName
 * \param bodyType
 */
PHP_FUNCTION(cfpr_get_promise_body);

/*! \brief Get the comment for a promise
 * \param promiseHandle
 */
PHP_FUNCTION(cfpr_get_promise_comment);

/*! \brief Get the name of an owner bundle
 * \param promiseHandle
 */
PHP_FUNCTION(cfpr_get_promise_bundle);

/*! \brief Get the type of a promise, e.g. files
 * \param promiseHandle
 */
PHP_FUNCTION(cfpr_get_promise_type);

/*! \brief Get the promiser of a promise
 * \param promiseHandle
 */
PHP_FUNCTION(cfpr_get_promiser);

/*! \brief Get the reported value of a variable
 * \param hostKey
 * \param scope
 * \param variableName
 */
PHP_FUNCTION(cfpr_get_variable);

/*! \brief A list of classes in a bundle
 * \param bundleName
 * \param bundleType
 */
PHP_FUNCTION(cfpr_get_classes_for_bundle);

/*! \brief A list of parameters for a bundle
 * \param bundleName
 * \param bundleType
 */
PHP_FUNCTION(cfpr_get_args_for_bundle);

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
 */
PHP_FUNCTION(cfpr_report_patch_in);

/*! \brief Query reported patch entries available to a host
 * \param hostKey
 * \param patchName
 * \param patchVersion
 * \param patchArchitecture
 * \param classExpression
 */
PHP_FUNCTION(cfpr_report_patch_avail);

/*! \brief Query reported classes at a host
 * \returns A JSON document with probability of seeing class, standard deviation
 * \param hostKey
 * \param name (leave this NULL)
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_report_classes);

/*! \brief A JSON document with hosts and contexts matching the context expressions
 * \param classExpressions comma-delimited list
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
 * \param resultsPerPage
 */
PHP_FUNCTION(cfpr_report_compliance_summary);

/*! \brief Get a cached promise compliance report for the barcharts
 * \param policy
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
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_lastseen);

/*! \brief Query for a performance report for a job
 * \param hostKey
 * \param job
 * \param useRegularExpressions
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_performance);

/*! \brief Query for programs with setuid
 * \param hostKey
 * \param file
 * \param useRegularExpressions
 * \param contextExpression
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
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_report_filediffs_longterm);

/*! \brief A list of all the library documents
 * \param filePath
 */
PHP_FUNCTION(cfpr_list_documents);

/*! \brief A list of all the time contexts at a host
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_list_time_classes);

/*! \brief A list of all the soft contexts at a host
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_list_soft_classes);

/*! \brief A list of all the IP contexts at a host
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_list_ip_classes);

/*! \brief A list of all contexts at a host
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_list_all_classes);

/*! \brief A list of host contexts
 * \param hostKey
 * \param name
 * \param useRegularExpressions
 * \param contextExpression
 */
PHP_FUNCTION(cfpr_list_host_classes);

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
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_summarize_repaired);

/*! \brief A description of a report
 * \param reportName
 */
PHP_FUNCTION(cfpr_report_description);

/*! \brief Get the name of a host with the given key
 * \param hostKey
 */
PHP_FUNCTION(cfpr_hostname);

/*! \brief Get the IP of a host with the given key
 * \param hostKey
 */
PHP_FUNCTION(cfpr_ipaddr);

/*! \brief Get the hubKey
 */
PHP_FUNCTION(cfpr_hub_key);

/*! \brief Determine if we are the hub master, or return IP of hub master
 */
PHP_FUNCTION(cfpr_get_hub_master);

PHP_FUNCTION(cfcon_get_story_by_name);
PHP_FUNCTION(cfcon_get_story_by_id);

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
 * \param policy
 * \param N
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_top_n_hosts);

/*! \brief Get a list of selected hosts (don't use this)
 * \param select
 * \param policy
 */
PHP_FUNCTION(cfpr_select_hosts);

/*! \brief Get a list of selected reports
 */
PHP_FUNCTION(cfpr_select_reports);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_red_hosts);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_yellow_hosts);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_green_hosts);

/*! \brief A list of red hosts
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_blue_hosts);

/*! \brief A list of hosts with an IP address
 * \param ipExpression
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_hosts_ip);

/*! \brief A list of hosts with a hostname
 * \param nameExpression
 * \param contextExpression
 * \param resultsPerPage
 * \param pageNumber
 */
PHP_FUNCTION(cfpr_show_hosts_name);

/*! \brief Get the colour of a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_get_host_colour);

/*! \brief The number of hosts under management of this hub
 */
PHP_FUNCTION(cfpr_count_all_hosts);

/*! \brief The number of red hosts under management of this hub
 */
PHP_FUNCTION(cfpr_count_red_hosts);

/*! \brief The number of yellow hosts under management of this hub
 */
PHP_FUNCTION(cfpr_count_yellow_hosts);

/*! \brief The number of green hosts under management of this hub
 */
PHP_FUNCTION(cfpr_count_green_hosts);

/*! \brief The number of blue hosts under management of this hub
 */
PHP_FUNCTION(cfpr_count_blue_hosts);

/*! \brief A summary document of a promise
 * \param promiseHandle
 */

PHP_FUNCTION(cfpr_summarize_promise);

/*! \brief Query handles by promiser
 * \param promiser
 * \param promiseType
 */
PHP_FUNCTION(cfpr_list_handles);

/*! \brief A list of all bundles by type
 * \param bundleType
 */
PHP_FUNCTION(cfpr_list_all_bundles);

/*! \brief A list of all bundles using a bundle
 * \param bundleName
 */
PHP_FUNCTION(cfpr_list_bundles_using);

/*! \brief Get the number of bundles
 */
PHP_FUNCTION(cfpr_get_bundle_count);

/*! \brief Get a bundle type by name
 * \param bundleName
 */
PHP_FUNCTION(cfpr_get_bundle_type);

/*! \brief Get the number of promises
 */
PHP_FUNCTION(cfpr_get_promise_count);

/*! \brief A list of handles in a bundle
 * \param bundleName
 * \param bundleType
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_list_handles_for_bundle);

/*! \brief A list of handles in a bundle (including comments)
 * \param bundleName
 * \param bundleType
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_get_handles_for_bundle_with_comments);

/*! \brief A list of bodies
 * \param bodyName
 * \param bodyType
 */
PHP_FUNCTION(cfpr_list_bodies);

/*! \brief List handles for policy findler by handle
 * \param handle
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_policy_finder_by_handle);

/*! \brief List handles for policy findler by bundle
 * \param bundle
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_policy_finder_by_bundle);

/*! \brief List handles for policy findler promiser
 * \param promiser
 * \param useRegularExpressions
 */
PHP_FUNCTION(cfpr_policy_finder_by_promiser);

/*! \brief Delete a host from the database
 * \param hostKey
 */
PHP_FUNCTION(cfpr_delete_host);

/*! \brief A list of all environments
 */
PHP_FUNCTION(cfpr_environments_list);

/*! \brief Get the contents of an environment
 * \param environmentName
 */
PHP_FUNCTION(cfpr_environment_contents);

/*! \brief Get the environment of a host
 * \param hostKey
 */
PHP_FUNCTION(cfpr_get_host_environment);

/*! \brief Get the replica status of this hub
 */
PHP_FUNCTION(cfpr_replica_status);

/*
 * For pdf generation
 */

PHP_FUNCTION(cfpr_report_compliance_summary_pdf);
PHP_FUNCTION(cfpr_report_bundlesseen_pdf);
PHP_FUNCTION(cfpr_report_value_pdf);
PHP_FUNCTION(cfpr_report_classes_pdf);
PHP_FUNCTION(cfpr_report_compliance_promises_pdf);
PHP_FUNCTION(cfpr_report_filechanges_pdf);
PHP_FUNCTION(cfpr_report_lastseen_pdf);
PHP_FUNCTION(cfpr_report_patch_avail_pdf);
PHP_FUNCTION(cfpr_report_patch_in_pdf);
PHP_FUNCTION(cfpr_report_software_in_pdf);
PHP_FUNCTION(cfpr_report_performance_pdf);
PHP_FUNCTION(cfpr_report_notkept_pdf);
PHP_FUNCTION(cfpr_report_repaired_pdf);
PHP_FUNCTION(cfpr_summarize_repaired_pdf);
PHP_FUNCTION(cfpr_summarize_notkept_pdf);
PHP_FUNCTION(cfpr_report_vars_pdf);
PHP_FUNCTION(cfpr_report_filediffs_pdf);
PHP_FUNCTION(cfpr_report_setuid_pdf);



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

/*
 * Constellation
 */

#ifdef HAVE_CONSTELLATION

PHP_FUNCTION(cfcon_compliance_summary_graph);
PHP_FUNCTION(cfcon_compliance_virtualbundle_graph);
PHP_FUNCTION(cfcon_count_repaired);
PHP_FUNCTION(cfcon_count_notkept);
PHP_FUNCTION(cfcon_reasons_repaired);
PHP_FUNCTION(cfcon_reasons_notkept);

PHP_FUNCTION(cfcon_count_hubs);
PHP_FUNCTION(cfcon_show_hubs);

PHP_FUNCTION(cfcon_environments_list);

PHP_FUNCTION(cfcon_promise_popularity);
PHP_FUNCTION(cfcon_rank_promise_popularity);


PHP_FUNCTION(cfcon_list_subscriptions);
PHP_FUNCTION(cfcon_local_show_subscription_virtualbundle);

PHP_FUNCTION(cfcon_subscribe_software);
PHP_FUNCTION(cfcon_local_subscribe_virtualbundle);
PHP_FUNCTION(cfcon_local_delete_subscription_virtualbundle);


PHP_FUNCTION(cfcon_subscribe_repairlog);


PHP_FUNCTION(cfcon_report_software);
PHP_FUNCTION(cfcon_local_report_virtualbundle);
PHP_FUNCTION(cfcon_local_report_virtualbundle_promises);

PHP_FUNCTION(cfcon_aggr_filechange);
PHP_FUNCTION(cfcon_aggr_software);
PHP_FUNCTION(cfcon_aggr_classes);
PHP_FUNCTION(cfcon_aggr_repaired);
PHP_FUNCTION(cfcon_aggr_notkept);

// meter and hub colour for observatory
PHP_FUNCTION(cfcon_hub_meter);
PHP_FUNCTION(cfcon_get_hub_colour);
PHP_FUNCTION(cfcon_count_hubs_colour);
PHP_FUNCTION(cfcon_count_hub_hosts);

PHP_FUNCTION(cfcon_list_hub_colour);
PHP_FUNCTION(cfcon_value_graph);
PHP_FUNCTION(cfcon_hub_details);
PHP_FUNCTION(cfcon_getlastupdate);
PHP_FUNCTION(cfcon_hubname);
PHP_FUNCTION(cfcon_ipaddr);
PHP_FUNCTION(cfcon_delete_hub);
#endif  /* HAVE_CONSTELLATION */

#endif
