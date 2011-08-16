<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

// Code here is run before ALL controllers
class Cf_Controller extends CI_Controller {

    function __construct() {
        parent::__construct();



        initializeHub();


        $this->load->library(array('session', 'ion_auth', 'breadcrumb', 'breadcrumblist', 'onlineUsers', 'carabiner', 'user_agent', 'setting_lib'));
        $this->lang->load('tooltip');
        $this->load->helper(array('url', 'html', 'language'));
        $this->lang->load('cf_message');
        ///cfpr_set_siteurl(site_url());
        // Login: If not logged in, redirect to  authentication index   
        if ($this->session->userdata('lastclasslist')) {
            $this->session->unset_userdata('lastclasslist');
        };
        if (!$this->ion_auth->logged_in()) {
            redirect('auth/index', 'refresh');
        } else {
            $this->onlineusers->onlineusers();
        }
    }

}

?>