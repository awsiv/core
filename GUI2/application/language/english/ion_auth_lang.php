<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');
/**
* Name:  Ion Auth Lang - English
* 
* Author: Ben Edmunds
* 		  ben.edmunds@gmail.com
*         @benedmunds
* 
* Location: http://github.com/benedmunds/ion_auth/
*          
* Created:  03.14.2010 
* 
* Description:  English language file for Ion Auth messages and errors
* 
*/

// Account Creation
$lang['account_creation_successful'] 	  	 = 'Account created successfully';
$lang['account_creation_unsuccessful'] 	 	 = 'Unable to create account';
$lang['account_creation_duplicate_email'] 	 = 'Email already used or invalid';
$lang['account_creation_duplicate_username'] 	 = 'Username already taken or invalid';


// Password
$lang['password_change_successful'] 	 	 = 'Password changed successfully';
$lang['password_change_unsuccessful'] 	  	 = 'Unable to change password';
$lang['forgot_password_successful'] 	 	 = 'Password reset email sent';
$lang['forgot_password_unsuccessful'] 	 	 = 'Unable to reset password';
$lang['org_passwd_valid']                             ="Please make sure original password is valid";

// Activation
$lang['activate_successful'] 		  	 = 'Account activated';
$lang['activate_unsuccessful'] 		 	 = 'Unable to activate account';
$lang['deactivate_successful'] 		  	 = 'Account de-activated';
$lang['deactivate_unsuccessful'] 	  	 = 'Unable to de-activate account';
$lang['activation_email_successful'] 	  	 = 'Activation email sent';
$lang['activation_email_unsuccessful']   	 = 'Unable to send activation email';

// Login / Logout
$lang['login_successful'] 		  	 = 'Logged in successfully';
$lang['login_unsuccessful'] 		  	 = 'The username or password you entered is incorrect.';
$lang['logout_successful'] 		 	 = 'Logged out successfully';
  
// Account Changes
$lang['update_successful'] 		 	 = 'Account information updated successfully';
$lang['update_unsuccessful'] 		 	 = 'Unable to update account information';
$lang['user_delete_successful'] 		 	 = 'User deleted';
$lang['user_delete_unsuccessful'] 		 	 = 'Unable to delete user';
$lang['one_admin_required']                                        ="At least one admin account is required by CFEngine Enterprise Mission Portal";

//role createion
$lang['role_creation_successful']              ="Role created successfully";
$lang['role_creation_unsuccessful']            ="Unable to create role";
$lang['role_creation_duplicate']               ="Role name already exists";

//role changes
$lang['role_delete_successful']                = 'Role deleted';
$lang['role_delete_unsuccessful']              = 'Unable to delete role';
$lang['role_update_successful']                = 'Role information updated successfully';
$lang['role_update_unsuccessful']              = 'Unable to update role';
$lang['no_roles_for_user']                     = "User does not belong to any role, but can login to the Enterprise Mission Portal with limited access";
$lang['backend_error']                         = "Cannot connect to the user database. Please make sure it exists.";
$lang['admin_role_deletion']                   = "Cannot delete the admin role";
$lang['no_admin_role']                         = "No admin role selected yet";
//modes of login
$lang['login_mode_changed']          =  'The mode of authentication has changed, it might be due to config changes or a system fallback to a local database due to failed LDAP login. Please retry login.';

//LDAP related
$lang['error_loading_application_setting']    = "Error loading application settings from database";
$lang['no_entries_found']                            ="Cannot find a match, please try again with new parameter values";
$lang['active_dir_conf_error']                    ="Invalid values supplied for Active Directory" ;
$lang['active_dir_login_error']                   ="Invalid Active Directory credentials supplied";
$lang['ldap_conf_error']                             ="Invalid values supplied for LDAP ";
$lang['ldap_login_error']                            ="failed to bind with LDAP";
$lang['Internal_Ldap_module_error']            ="Error: Please check credentials and configuration for the LDAP.";
$lang['successful_bind']                             ="Authentication sucessful";
$lang['ldap_value_grabbing_error']             =" Cannot fetch some attributes values, please correct the attribute names";
$lang['error_fetching_role']                    ="Cannot query for available role names, but user can login into mission portal with limited access";

//login page message
$lang['login_internal'] ='Internal authentication';
$lang['login_ldap']='Open LDAP';
$lang['login_active_directory']='Active Directory';
$lang['login_mode_not_found']='Cannot find backend';


// permission
$lang['no_permission'] = "You don't have right permission to complete this operation.";
$lang['no_access'] = "The given user has no access to this data";