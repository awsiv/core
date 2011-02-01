<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//this file contains the functions that generates paths to different portion of the application

function get_cssdir()
	{
	 $CI=&get_instance();
	 return base_url().$CI->config->item('css_dir');
	}

function get_scriptdir()
	{
	 $CI=&get_instance();
	 return base_url().$CI->config->item('scripts_dir');
	}

function get_imagedir()
	{
	 $CI=&get_instance();
	 return base_url().$CI->config->item('image_dir');
	}

function get_hubdir()
	{
	 $CI=&get_instance();
	 //return base_url().
	 return $CI->config->item('hub_dir');
	}

function get_graphdir()
	{
	$CI=&get_instance();
	 //return base_url().$CI->config->item('graph_dir');
	 return $CI->config->item('graph_dir');
	}

function get_policiesdir()
	{
	 $CI=&get_instance();
	 return $CI->config->item('policies_dir');
	}
        
function is_ajax() {
    return (isset($_SERVER['HTTP_X_REQUESTED_WITH']) &&
    ($_SERVER['HTTP_X_REQUESTED_WITH'] == 'XMLHttpRequest'));
}

function set_success_limiters($message)
{
  return '<p class="success">'.$message.'</p>';
}


?>