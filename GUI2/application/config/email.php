<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
/*
 * method used to send email
 */
$config['protocol'] = 'sendmail';
$config['user_agent']='cfengine';
$config['mailpath'] = '/usr/sbin/sendmail';
$config['charset'] = 'iso-8859-1';
$config['wordwrap'] = TRUE;

?>
