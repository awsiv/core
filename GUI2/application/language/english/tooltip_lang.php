<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

$lang['tooltip_host_finder'] = 'Find hosts by name or address within the local cluster.';
$lang['tooltip_class_finder'] = 'Find hosts by the class contexts they occupy in the local cluster.';
$lang['tooltip_promise_finder'] = 'Find promises that make up the policy in the local cluster.';


$lang['tooltip_reports_finder'] = 'Search and browse the available reports in the local cluster.';

$lang['tooltip_summary_finder']  = 'Show the current state of knowledge about promise compliance. If a single promise handle is specified, this returns the number of hosts matching a class regex that is compliant. If a regex is given for the promise the result is an expectation value of number of hosts wrt all the matching promises.';

$lang['tooltip_license_report'] = 'Displays a summary of the utilization of CFEngine licenses for this cluster.';

$lang['tooltip_vital_signs'] = 'Performance measurements that characterize the observed runtime state of a host.';

$lang['tooltip_redhost'] = 'More than 20% of promises could not be kept at last measurement.';
$lang['tooltip_yellowhost'] = 'More than 20% of promises needed repair at last measurement.';
$lang['tooltip_greenhost'] = 'More than 80% of promises are stable and kept.';
$lang['tooltip_bluehost'] = 'Hosts that have not been reachable for more than the time horizon.';

$lang['tooltip_statusroom'] = 'The mission status room shows how close you are an ideal state. The average compliance of hosts is graphed for the present and over the past week. If business values have been configured, the monetary value of the promises is also represented. A reminder of the documented goals is shown in the panel.';
$lang['tooltip_engineeringroom'] = 'Engineering shows technical status of hosts in the local cluster.';
$lang['tooltip_planningroom'] = 'The mission planning room allows you to see how the current goals and policies are performing, approve a change of course and communicate with co-workers.';
$lang['tooltip_libraryroom'] = 'A place where users can learn about the software and the system.';

$lang['tooltip_edit_policies'] = 'Choose an SVN repository to edit the policies.';
$lang['tooltip_track_records'] = 'Review successes and failures of the current version.';
$lang['tooltip_approve_policies'] = 'Add your approval to adopt a named policy revision.';
$lang['tooltip_service_catalogue'] = 'List technical services (promise bundles) and the business goals they address.';


$lang['tooltip_docs'] = 'Documentation dump for all official CFEngine documents.';
$lang['tooltip_find_topics'] = 'Look up keywords or phrases in the knowledge map.';
$lang['tooltip_notes_archive'] = 'Browse public notes added manually by CFEngine users.';
$lang['tooltip_community'] = 'Link to the community help-forum.';
$lang['tooltip_application_email']='The email to be used by mission portal while sending mails';
$lang['tooltip_host_name']= "The full domain name of the directory server  e.g test.cfengine.com";
$lang['tooltip_base_dn']="Base domain name of the directory server  e.g dc=test,dc=cfengine,dc=com";
$lang['tooltip_login_attr']="The attribute used by directory server to match the username e.g uid, sAMAccountName";
$lang['tooltip_user_dir']="The directory used by the server to store users information  e.g cn=users or ou=people";
$lang['tooltip_member_attr']="The attribute used to connect a user with group e.g memberUid";
$lang['tooltip_ad_domain_name']="The domain named used by the active directory e.g test.cfengine.com";

$lang['tooltip_fall_back']="Users falling in the selected group will be able to login if directory server is down";
$lang['tooltip_admin_grp']="User belonging to this role will act as admin in mission portal"















?>
