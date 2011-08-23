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
$lang['account_creation_successful'] 	  	 = 'Account Successfully Created';
$lang['account_creation_unsuccessful'] 	 	 = 'Unable to Create Account';
$lang['account_creation_duplicate_email'] 	 = 'Email Already Used or Invalid';
$lang['account_creation_duplicate_username'] 	 = 'Username Already Used or Invalid';


// Password
$lang['password_change_successful'] 	 	 = 'Password Successfully Changed.';
$lang['password_change_unsuccessful'] 	  	 = 'Unable to Change Password';
$lang['forgot_password_successful'] 	 	 = 'Password Reset Email Sent';
$lang['forgot_password_unsuccessful'] 	 	 = 'Unable to Reset Password';
$lang['org_passwd_valid']                             ="Please make sure original password is valid";

// Activation
$lang['activate_successful'] 		  	 = 'Account Activated';
$lang['activate_unsuccessful'] 		 	 = 'Unable to Activate Account';
$lang['deactivate_successful'] 		  	 = 'Account De-Activated';
$lang['deactivate_unsuccessful'] 	  	 = 'Unable to De-Activate Account';
$lang['activation_email_successful'] 	  	 = 'Activation Email Sent';
$lang['activation_email_unsuccessful']   	 = 'Unable to Send Activation Email';

// Login / Logout
$lang['login_successful'] 		  	 = 'Logged In Successfully';
$lang['login_unsuccessful'] 		  	 = 'The username or password you entered is incorrect.';
$lang['logout_successful'] 		 	 = 'Logged Out Successfully';
  
// Account Changes
$lang['update_successful'] 		 	 = 'Account Information Successfully Updated';
$lang['update_unsuccessful'] 		 	 = 'Unable to Update Account Information';
$lang['user_delete_successful'] 		 	 = 'User Deleted';
$lang['user_delete_unsuccessful'] 		 	 = 'Unable to Delete User';
$lang['one_admin_required']                                         ="Atleast one admin is required by mission portal";

//Group createion
$lang['group_creation_successful']              ="Group Sucessfully Created";
$lang['group_creation_unsuccessful']            ="Unable to Create Group";
$lang['group_creation_duplicate']               ="Group name already Exists";

//Group changes
$lang['group_delete_successful'] 		= 'Group Deleted';
$lang['group_delete_unsuccessful'] 		= 'Unable to Delete Group';
$lang['group_update_successful']                = 'Group Information Sucessfully Updated';
$lang['group_update_unsuccessful']              = 'Unable to Update group';
$lang['backend_error']              = "Cannot connect to database .Make sure database exist";
$lang['admin_group_deletion']= "Cannot delete the admin group";
$lang['no_admin_group']="No Admin group selected yet in app config";
//modes of login
$lang['login_mode_changed']          =  'The mode of authentication is changed , it might be due to config changes or system fall back to use database in case of unsucessful ldap login
                                                       Please relogin';

//ldap related
$lang['error_loading_application_setting']    = "Error loading Application settings from database";
$lang['no_entries_found']                            ="Cannot find any entry in database";
$lang['unsucessful_active_directory_bind']   ="Invalid credential or Configuration, Unable to authenticate using Active directory as backend.";
$lang['Internal_Ldap_module_error']            ="Please check credentials and Configuration for the ldap, Ldap module error";
$lang['unsucessful_ldap_bind']                    ="Invalid credential or Configuration, Unable to authenticate using Open LDAP as backend";
$lang['successful_bind']                             ="Sucessfully Authenticated against the desired directory server";

