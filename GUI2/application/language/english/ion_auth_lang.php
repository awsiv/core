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
$lang['account_creation_successful'] 	  	 = 'Account successfully created';
$lang['account_creation_unsuccessful'] 	 	 = 'Unable to create account';
$lang['account_creation_duplicate_email'] 	 = 'Email already used or invalid';
$lang['account_creation_duplicate_username'] 	 = 'Username already taken or invalid';


// Password
$lang['password_change_successful'] 	 	 = 'Password changed successfully.';
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
$lang['one_admin_required']                                        ="At least one admin account is required by CFEngine Nova Mission Portal";

//Group createion
$lang['group_creation_successful']              ="Group sucessfully created";
$lang['group_creation_unsuccessful']            ="Unable to create group";
$lang['group_creation_duplicate']               ="Group name already exists";

//Group changes
$lang['group_delete_successful'] 		= 'Group deleted';
$lang['group_delete_unsuccessful'] 		= 'Unable to delete group';
$lang['group_update_successful']                = 'Group information sucessfully updated';
$lang['group_update_unsuccessful']              = 'Unable to update group';
$lang['no_groups_for_user']                     = "User does not belong to any group. But user can login into misssion portal with limited access";
$lang['backend_error']                          = "Cannot connect to the database. Please make sure it exists.";
$lang['admin_group_deletion']= "Can not delete the admin group";
$lang['no_admin_group']="Admin group not selected yet in app config";
//modes of login
$lang['login_mode_changed']          =  'The mode of authentication is changed, it might be due to config changes or system fall back to use database in case of unsucessful ldap login. Please relogin';

//ldap related
$lang['error_loading_application_setting']    = "Error loading application settings from database";
$lang['no_entries_found']                            ="Cannot find any entry, please adjust the parameters value";
$lang['active_dir_conf_error']                    ="Invalid values supplied for Active Directory" ;
//$lang['active_dir_login_error']                   ="Invalid Active Directory credentials supplied";
$lang['ldap_conf_error']                             ="Invalid values supplied for ldap ";
//$lang['ldap_login_error']                            ="invalid values supplied for ldap";
$lang['Internal_Ldap_module_error']            ="Ldap module error: Please check credentials and configuration for the ldap.";
$lang['successful_bind']                             ="Sucessfully authenticated against the desired directory server";
$lang['ldap_value_grabbing_error']             =" Cannot fetch some attributes values, please correct the attribute names";
$lang['error_fetching_group']                    ="Cannot query for available group names, but user can login into mission portal with limited access";

//login page message
$lang['login_database'] ='Internal authentication';
$lang['login_ldap']='Open Ldap';
$lang['login_active_directory']='Active Directory';
$lang['login_mode_not_found']='Cannot find backend';
