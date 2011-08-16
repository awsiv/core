<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');


/* ---------------------------------------------------------------------
 * DECIDE if the local host is hubmaster or not for the purpose of 
 * writing the logs and other things only in master hub.
 */

function initializeHub() {

    $ishubmaster = cfpr_get_hub_master();
    $is_hub_master = true;
    if ($ishubmaster == 'am_hub_master') {
        define('HUB_MASTER', 'localhost');
    } elseif (preg_match('/(\d+).(\d+).(\d+).(\d+)/', $ishubmaster)) {
        define('HUB_MASTER', $ishubmaster);
    } else {
        show_error('Could not connect to the MongoDB database in hub master. Please ensure that CFEngine Nova is running correctly, by issuing the following command on the hub.
                <br/>
                # /var/cfengine/bin/cf-twin -Kf failsafe.cf');
    }
}

//this file contains the functions that generates paths to different portion of the application

function get_cssdir() {
    $CI = &get_instance();
    if ($CI->session->userdata('theme')) {
        return base_url() . 'themes/' . $CI->session->userdata('theme') . '/' . $CI->config->item('css_dir');
    } else {

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