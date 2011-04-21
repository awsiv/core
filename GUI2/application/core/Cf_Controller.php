<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

// Code here is run before ALL controllers
class Cf_Controller extends CI_Controller {

    function __construct() {
        parent::__construct();

        $this->load->library(array('session', 'ion_auth', 'breadcrumb', 'breadcrumblist', 'onlineUsers', 'carabiner'));
        //$this->load->library('session');

        


        $this->load->helper(array('url', 'html'));
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