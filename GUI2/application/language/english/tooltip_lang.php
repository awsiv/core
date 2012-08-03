<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

$lang['tooltip_host_finder'] = 'Find hosts by name or address.';
$lang['tooltip_class_finder'] = 'Find hosts by the classes they have defined.';
$lang['tooltip_promise_finder'] = 'Find promises that make up the policy.';


$lang['tooltip_reports_finder'] = 'Search and browse the available reports.';

$lang['tooltip_summary_finder']  = 'Show the current promise compliance.';

$lang['tooltip_license_report'] = 'Displays a summary of the utilization of CFEngine licenses for this cluster.';

$lang['tooltip_vital_signs'] = 'Performance measurements that characterize the observed runtime state of a host.';

$lang['tooltip_redhost'] = 'More than 20% of promises could not be kept at last measurement.';
$lang['tooltip_yellowhost'] = 'More than 20% of promises needed repair at last measurement.';
$lang['tooltip_greenhost'] = 'More than 80% of promises are stable and kept.';
$lang['tooltip_bluehost'] = 'Hosts that have not been reachable (their state is unknown) for more than: ';
$lang['tooltip_blackhost'] = 'Hosts failed to execute their agents for last 3 scheduled rounds.';

$lang['tooltip_statusroom'] = 'The Business room shows how close you are to an ideal state.';
$lang['tooltip_engineeringroom'] = 'The Status and Reports room shows the technical status of the managed hosts.';
$lang['tooltip_planningroom'] = 'The Planning room contains tools for collaboration, planning and policy changes.';
$lang['tooltip_libraryroom'] = 'The Library room contains knowledge and documentation about CFEngine and your policy.';

$lang['tooltip_edit_policies'] = 'Choose an SVN repository to edit the policies.';
$lang['tooltip_track_records'] = 'Review successes and failures of the current policy version.';
$lang['tooltip_approve_policies'] = 'Add your approval to adopt a named policy revision.';
$lang['tooltip_service_catalogue'] = 'List technical services (promise bundles) and the business goals they address.';


$lang['tooltip_docs'] = 'Documentation dump for all official CFEngine documents.';
$lang['tooltip_find_topics'] = 'Look up keywords or phrases in the knowledge map.';
$lang['tooltip_notes_archive'] = 'Browse public notes added manually by CFEngine users.';
$lang['tooltip_community'] = 'Link to the community help-forum.';
$lang['tooltip_application_email']='Notifications from the Mission Portal will be sent from this email address.';
$lang['tooltip_host_name']= "The full domain name of the directory server  e.g test.cfengine.com";
$lang['tooltip_base_dn']="Base domain name of the directory server  e.g dc=test,dc=cfengine,dc=com";
$lang['tooltip_login_attr']="The attribute used by directory server to match the username e.g uid, sAMAccountName";
$lang['tooltip_user_dir']="The directory used by the server to store users information  e.g cn=users or ou=people, If this directory is under sub directory specify its name as well eg cn=users,ou=sales";
$lang['tooltip_member_attr']="The attribute used to connect a user with group e.g memberUid";
$lang['tooltip_ad_domain_name']="The domain named used by the active directory e.g test.cfengine.com";
$lang['tooltip_external_admin_username']="This user will have full privileges when external authentication is used";

$lang['tooltip_fall_back']="Members of the selected group will be able to log in if the directory server is down";
$lang['tooltip_admin_grp']="Members of the selected group will be administrators of the Mission Portal";
$lang['tooltip_bluehost_threshold']="Blue-state threshold is an amount of time after which host state is considered to be unknown. Range: 5 minutes - 5 weeks";
$lang['tool_tip_host_delete']='Delete this host from the report database. Note the host will automatically reappear if CFEngine is still running on it. To avoid this stop all CFEngine processes on the concerned host before deletion.';
$lang['tool_tip_download_report']='Reports in Pdf or CSV format';














?>
