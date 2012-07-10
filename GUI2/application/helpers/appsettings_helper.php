<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');


/* ---------------------------------------------------------------------
 * DECIDE if the local host is hubmaster or not for the purpose of 
 * writing the logs and other things only in master hub.
 */

function initializeHub() {
    
    $mongoPort = getenv('CFENGINE_TEST_OVERRIDE_MONGO_PORT');
    if ($mongoPort !== false)
    {
        define('MONGO_PORT', $mongoPort);
    }
    else
    {
        define('MONGO_PORT', 27017);
    }
    
    $ishubmaster = cfpr_get_hub_master();
     $CI = &get_instance();
    if (!trim($ishubmaster)) {
        show_error('Database connection failure. Please ensure that CFEngine Enterprise is running correctly, by issuing the following command on the hub.
                <br/>
                # /var/cfengine/bin/cf-twin -Kf failsafe.cf');
    } elseif ($ishubmaster == 'am_hub_master') {
        define('HUB_MASTER', 'localhost');
    } else {
        // if it does not say am_hub_master , it must be a host name or IP
        //define('HUB_MASTER', $ishubmaster);
        //preg_match('/(\d+).(\d+).(\d+).(\d+)/', $ishubmaster)
        $errorMessage = sprintf("Not a hub master, please click on the link below to navigate to 
                                             the CFEngine 3 Enterprise Mission Portal <br />
                                             Link ::<a href=\"http://%s/\">%s</a>", $ishubmaster, $ishubmaster);

        show_error($errorMessage);
    }
}

/**
 * To get the version and the product name;
 */
   function get_productname(){
       $CI = &get_instance();
       $rawdata=cfpr_enterprise_version();
       $CI->load->helper('cf_util_helper');
       $data=sanitycheckjson($rawdata, true);
       $ret=array();
       if(is_array($data) && $data['error']['errid']==0){
           $ret['name']=$data['name'];
           $ret['version']=$data['version'];
       }
       $ret['name']="Enterprise";
       $ret['version']='2.3.xx';
       return $ret;
   }
   
   function init_productname(){
       $data=get_productname();
       define(strtoupper($data['name']),true);
   }
   
   function is_constellation(){
      return !defined('CONSTELLATION') ? false : constant('CONSTELLATION');
   }


//this file contains the functions that generates paths to different portion of the application

function get_cssdir() {
    $CI = &get_instance();
    if ($CI->session->userdata('theme')) {
        return base_url() . 'themes/' . $CI->session->userdata('theme') . '/' . $CI->config->item('css_dir');
    } else {
        if(is_constellation()){
            return base_url() . $CI->config->item('constellation') . $CI->config->item('css_dir');
        }
        return base_url() . $CI->config->item('default') . $CI->config->item('css_dir');
    }
}

function get_scriptdir() {
    $CI = &get_instance();
    return base_url() . $CI->config->item('scripts_dir');
}

function get_imagedir() {
    $CI = &get_instance();
    return base_url() . $CI->config->item('image_dir');
}

function get_hubdir() {
    $CI = &get_instance();
    //return base_url().
    return $CI->config->item('hub_dir');
}

function get_graphdir() {
    $CI = &get_instance();
    //return base_url().$CI->config->item('graph_dir');
    return $CI->config->item('graph_dir');
}

function get_policiesdir() {
    $CI = &get_instance();
    return $CI->config->item('policies_dir');
}

function get_nodehost() {
    $CI = &get_instance();
    $url = parse_url(site_url());
    return "http://" . $url['host'] . ':' . $CI->config->item('nodeport');
}

function get_nodehost_from_server() {
    $CI = &get_instance();
    return "http://localhost:" . $CI->config->item('nodeport');
}

?>