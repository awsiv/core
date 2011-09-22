<?php
/**
 * page title mission portal
 */
$lang['mission_portal_title']='CFEngine Mission Portal';
$lang['policy_editor']='Policy Editor';



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
/**
 * Policy editor related things
 * 
 */
$lang['revision_not_known']='unknown';
$lang['approvals_not_known']='unknown';
$lang['repo_not_known']='Empty Working Director';
$lang['not_comitted_no_changes']='Not Committed as no changes was detected';
$lang['single_file_commit_fail']=' Was not committed';
$lang['write_file_success']='Success writing to file';
$lang['write_file_error']='Error writing to file';
$lang['same_file_exist']="File with same name exist in repository.Couldn't write file";
$lang['promise_file_not_exist']='Failed to find promises.cf';


/**
 * Graphs message from graph controller
 */
$lang['graph_data_not_available']='No data available.';


/**
 * notescontroller text
 */
$lang['note_empty_insert']="Cannot insert empty message.";
$lang['note_insert_error']="Some error occured while inserting note.";

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
 $lang['search_save_error']='Error while saving the search.';
 
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
$lang['front_greeting']          ='Welcome to CFEngine';
$lang['report_hostgp_help']    ='SOFT CLASSES or  HARD CLASSES with combination of regular expression as input, To open <strong>classfinder</strong> click me!';
$lang['no_revisions'] ='No revisions exist for the selected repository ,operation was halted,
                                   Please checkout/update or commit using the system for selected repository';
$lang['no_repos'] = "No repositories are defined.";

$lang['already_approved']= 'You  have already approved this version of policiy. See approved log for more details';
$lang['green']='Compliant';
$lang['blue']='Unreachable';
$lang['yellow']='Repaired';
$lang['red']='Non compliant';
$lang['not_hub_master_msg']='The mongo db in this hub is not a hub master, please click on the link below to navigate to the mission portal having its mongo db as hubmaster<br />
                                             Link :: ';
/**
 * Text diaplayed in the status page
 */

$lang['business-value-pie-chart-container_title']="Business value";
$lang['compliance-value-pie-chart-container_title']="Host status";
$lang['compliance_summary_graph_title']='Compliance summary';
$lang['services_goals']='Services/goals';
?>
