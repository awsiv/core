<?php
/**
 * page title mission portal
 */
$lang['mission_portal_title']='Mission Portal';
$lang['policy_editor']='Policy Editor';
$lang['nova_logo_link_text']='Nova mission portal';
$lang['cons_logo_link_text']='Constellation mission portal';


/**
 * Bread crumbs text
 */
$lang['breadcrumb_status']           ='Status';
$lang['breadcrumb_engineering']      ='Engineering';
$lang['breadcrumb_planning']         ='Planning';
$lang['breadcrumb_goals']            ='Company goals';
$lang['breadcrumb_weakest_host']     ='Weakest Host';
$lang['breadcrumb_service_catalogue']='Services';
$lang['breadcrumb_license']          ='License';
$lang['breadcrumb_classes']          ='Classes';
$lang['breadcrumb_admin']            ='Admin';
$lang['breadcrumb_bundle']           ='Bundle';
$lang['breadcrumb_cdpreport']        ='CDP Reports';
$lang['breadcrumb_docs']             ='Documents';
$lang['breadcrumb_library']          ='Library';
$lang['breadcrumb_kw_bank']          ='Knowledge Map';
$lang['breadcrumb_notes']            ='Notes';
$lang['breadcrumb_promises']         ='Promises';
$lang['breadcrumb_promise']          ='Promise';
$lang['breadcrumb_repo_manage']      ='Manage Repository';
$lang['breadcrumb_approved_policies']='Approved Policies';
$lang['breadcrumb_saved_search']     ='Saved Search';
$lang['breadcrumb_report']           ='Report';
$lang['breadcrumb_summary_report']   ='Summary Report';
$lang['breadcrumb_vital']            ='Vitals';
$lang['breadcrumb_notes']            ='Notes';
$lang['breadcrumb_setting']          ="Settings";
/**
 * Policy editor related things
 *
 */
$lang['revision_not_known']='unknown';
$lang['approvals_not_known']='unknown';
$lang['repo_not_known']='Empty working directory';
$lang['not_comitted_no_changes']='Not committed: no changes was detected';
$lang['single_file_commit_fail']=' Was not committed';
$lang['write_file_success']='Write to file succeeded';
$lang['write_file_error']='Error writing to file';
$lang['same_file_exist']="A file with the same name exists in repository. Couldn't write file";
$lang['promise_file_not_exist']='Failed to find promises.cf';


/**
 * Graphs message from graph controller
 */
$lang['graph_data_not_available']='No data available.';


/**
 * notescontroller text
 */
$lang['note_empty_insert']="Cannot insert empty message";
$lang['note_insert_error']="An error occured while inserting the note";
$lang['notes'] = 'Notes';

/**
 * Repository controller related text
 */
 $lang['repo_add_sucess']='Repository added sucessfully';
 $lang['proceed_to_editor']='Go to policy editor';

 /**
 * Hub status page text
 */
 $lang['hub_status']='Hub replication status';

 /**
  * Saved search related text
  */
 $lang['search_save_error']='Error while saving the search';

 /**
  * Vital pulse and sign related test
  */
 $lang['vital_no_data']='No data available for this host';
 $lang['vital_no_host']='No host selected';

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
$lang['no_data'] 	         = 'Sorry, this report is empty.';
$lang['license_to']               ='This edition is licensed to ';
$lang['front_greeting']          ='CFEngine 3 Nova';
$lang['report_hostgp_help']    ='SOFT CLASSES or  HARD CLASSES with combination of regular expression as input, To open <strong>classfinder</strong> click me!';
$lang['no_revisions'] ='No revisions exist for the selected repository. Operation halted,
                                   Please checkout, update or commit using the system for selected repository';
$lang['no_repos'] = "No repositories are defined.";

$lang['already_approved']= 'You  have already approved this version of policiy. See approved log for more details';
$lang['green']='Compliant';
$lang['blue']='Unreachable';
$lang['yellow']='Repaired';
$lang['red']='Non compliant';
$lang['black'] = 'Scheduling deviation';
$lang['not_hub_master_msg']='This is not a hub master, please click on the link below to navigate to the Mission Portal hosting the hubmaster<br />
                                             Link :: ';
/**
 * Text diaplayed in the status page
 */

$lang['business-value-pie-chart-container_title']="Business value";
$lang['compliance-value-pie-chart-container_title']="Host status";
$lang['compliance_summary_graph_title']='Compliance summary';
$lang['services_goals']='Services/goals';


/*
 * Text displayed in the engineering page
 */
$lang['header_host_compliance_timeseries'] = "Host Compliance";
$lang['header_host_status'] = "Host status (last known hour)";
$lang['header_host_connectivity'] = "Host Connectivity (last known hour)";
$lang['header_promise_compliance'] = "Promise compliance summary for reachable hosts";
$lang['header_finders'] = "Finders";
$lang['host_known'] = "Hosts Known";
$lang['worst_host'] = "Worst available host rank";
$lang['hub_replication_status'] = "Hub replication status";
$lang['host'] = "Host";
$lang['class'] = "Class";
$lang['promises'] = "Promises";
$lang['reports'] = "Reports";
$lang['summary_reports'] = "Summary reports";
$lang['cdp_reports'] = "CDP reports";




$lang['not_compliant'] = "hosts ( > 20% not compliant)";
$lang['now_compliant'] = "hosts ( > 20% repaired, now compliant)";
$lang['compliant'] = "hosts ( > 80% compliant)";
$lang['unreachable'] = "hosts unreachable";
$lang['not_executing'] = "hosts with scheduling deviation";

/*
 * Planning page
 */
$lang['header_policy_goals'] = "Policy goals";
$lang['header_logged_on'] = "Logged on";
$lang['header_activity_log'] = "My activity log";


$lang['edit_policies'] = "Edit policies";
$lang['track_record'] = "Track records";
$lang['approved_policies'] = "Approve policies";
$lang['service catalogue'] = "Service catalogue";

/*
 * Library page
 */
$lang['docs'] = "Docs";
$lang['find_topic'] = "Find topic";
$lang['notes_archive'] = "Notes archive";
$lang['community'] = "Community";


/*
 * Host page
 */

$lang['header_action'] = "Actions";
$lang['header_analysis'] = "Analysis";
$lang['header_monitor_job'] = "Monitored jobs";
$lang['header_status'] = "Status (measured)";
$lang['header_host_details'] = "Host details (discovered)";
$lang['header_host_delete'] = "Proceed with host deletion";

$lang['host_reports'] = "Reports";
$lang['host_notes'] = "Notes";
$lang['host_vitals'] = "Vitals";
$lang['host_problems'] = "Problems";

$lang['host_total_surveillance'] = "Total number under surveillance";
$lang['host_on_host'] = "On this host";

$lang['host_average_load'] = "Average load";
$lang['host_free_disk'] = "Average free disk";
$lang['host_network_speed'] = "Average network speed";
$lang['host_alias'] = "Alias";
$lang['host_os_class'] = "OS class";
$lang['host_release'] = "Release";
$lang['host_flavour'] = "Flavour";
$lang['host_ip'] = "Last IP-address";
$lang['host_last_data'] = "Last data";
$lang['host_id'] = "ID";
$lang['host_not_discovered'] = '(Not discovered yet)';
$lang['host_delete'] = 'Delete this host';
$lang['host_delete_confirm'] = ' The host will be deleted from the report database on the hub.
        The hub will still try to pull it for reports, and it may thus <strong>Reappear</strong>.Are you sure you want to delete this host?';
$lang['host_promise_not_kept'] = 'Promises not kept in the past week';

/*
 * Promise page
 */
$lang['header_promise_definition'] = 'Promise definition';
$lang['header_body_promise'] = 'Body of the promise';

$lang['promise_tab_definition'] = 'Definition';
$lang['promise_tab_leads'] = 'Leads';
$lang['promise_tab_same_bundle'] = 'Same bundle';
$lang['promise_tab_same_promiser'] = 'Same promiser';
$lang['promise_tab_same_type'] = 'Same type';

$lang['promise_belong_to_bundle'] = 'Belonging to agent bundle';
$lang['promise_reference_handle'] = 'Reference handle';
$lang['promise_affected_object'] = 'Affected object (promiser)';
$lang['promise_stakeholder'] = 'Stakeholders (promisees)';
$lang['promise_comment_intention'] = 'Comment on original intention';
$lang['promise_about'] = 'Promise is about';
$lang['promise_class_context'] = 'Applies in the class context';
$lang['promise_defined_file'] = 'Defined in file';
$lang['promise_body_not_found'] = 'Body definition was not found';
$lang['promise_other_promise'] = 'Other promises in bundle';
$lang['promise_other_promiser'] = 'Other promises by promiser';
$lang['promise_other_promiser_type'] = 'Other promises of type';


/*
 * Bundle page
 */
$lang['bundle_tab_details'] = 'Bundle Details';
$lang['bundle_tab_bundle_using'] = 'Bundles using this bundle';
$lang['bundle_tab_all_bundle'] = 'All Bundle';

$lang['bundle_definition_of'] = 'Bundle definition of';
$lang['bundle_name'] = 'Name';
$lang['bundle_arguments'] = 'Arguments';
$lang['bundle_handle'] = 'Handle';
$lang['bundle_comment'] = 'Comment';
$lang['bundle_host_class'] = 'Host classes used in this bundle';
$lang['bundle_promise'] = 'Promises in this bundle';
$lang['bundle_bundle'] = 'Bundles';


/*
 * Knowledge map
 */
$lang['knowledge_tab_map'] = 'Map';
$lang['knowledge_tab_leads'] = 'Leads';
$lang['knowledge_tab_references'] = 'References';
$lang['knowledge_tab_same_context'] = 'Same context';
$lang['knowledge_tab_subtopic'] = 'Sub topic';
$lang['knowledge_copernicus'] = 'Copernicus local cluster view for topic';
$lang['knowledge_insight_leads'] = 'Insight, leads and perspectives';
$lang['knowledge_access_denied'] = "You don't have access to knowledgemap";



/*
 * Roles
 */
$lang['admin_role_delete_forbidden'] = "You can't delete admin role";
$lang['empty_rolename'] = 'Role name is empty';


/*
 * General
 */
$lang['nothing_found'] = 'Nothing Found';
$lang['500_error']     = 'Internal Server Error';
$lang['cfmod_exception']= "CFMOD Exception caught";
$lang['session_expired']= "Session Expired ! Please relogin";
$lang['svn_update_no_user']="Svn credentials not found for user";
$lang['rbac_exception']= "User not authorised to access this data";
$lang['invalid_json']="Valid json data cannot be generated";
$lang['invalid_report_id'] = "Invalid report type";

/*
 * Exception messages
 */
$lang['cf_mod_expection_error']='Server Internal Error.';
?>
