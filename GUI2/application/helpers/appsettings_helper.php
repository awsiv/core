<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//this file contains the functions that generates paths to different portion of the application

function get_cssdir()
	{
	$CI=&get_instance();
        if($CI->session->userdata('theme'))
         {
	  return base_url().'themes/'.$CI->session->userdata('theme').'/'.$CI->config->item('css_dir');
         }
         else {

         return base_url().$CI->config->item('default').$CI->config->item('css_dir');
          }
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
        
  function get_nodehost()
     {
          $CI=&get_instance();
          $url=parse_url(site_url());
          return "http://".$url['host'].':'.$CI->config->item('nodeport');
     }

   function get_nodehost_from_server()
   {
       $CI=&get_instance();
        return "http://localhost:".$CI->config->item('nodeport');
   }

?>