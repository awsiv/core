<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

// Code here is run before ALL controllers
class Cf_Controller extends CI_Controller {

    function __construct() {
        parent::__construct();


        //for replication to work and specifies if this is the primary huc
        initializeHub();
        
        //for setting the constant ie constellation or nova as product name
        init_productname();

        $this->load->library(array('session', 'ion_auth', 'breadcrumb', 'breadcrumblist', 'onlineUsers', 'carabiner', 'user_agent', 'setting_lib','log_maintenance'));
        $this->lang->load('tooltip');
        $this->load->helper(array('url', 'html', 'language'));
        $this->lang->load('cf_message');
        ///cfpr_set_siteurl(site_url());
        // Login: If not logged in, redirect to  authentication index   
        if ($this->session->userdata('lastclasslist')) {
            $this->session->unset_userdata('lastclasslist');
        };
        if (!$this->ion_auth->logged_in() && !is_ajax()) {
            redirect('auth/index', 'refresh');
        } else {
            $this->onlineusers->onlineusers();
        }
    }
    
    protected function param_includes()
    {
        $value = $this->param_list('includes', TRUE);
        if ($value)
        {
            return $value;
        }
        else
        {
            return array();
        }
    }
    
    protected function param_excludes()
    {
        $value = $this->param_list('excludes', TRUE);
        if ($value)
        {
            return $value;
        }
        else
        {
            return array();
        }
    }
    
    protected function param_list($key, $xss_clean = TRUE)
    {
        $value = $this->param($key, $xss_clean);
        if ($value)
        {
            return explode(',', urldecode($value));
        }
        else
        {
            return FALSE;
        }
    }
    
    protected function param($key, $xss_clean = FALSE)
    {
        switch ($this->request->method)
        {
            case 'get':
                return array_key_exists($key, $this->_get_args) ? $this->_xss_clean($this->_get_args[$key], $xss_clean) : FALSE;

            case 'post':
                return $this->input->post($key, $xss_clean);

            case 'put':
                return array_key_exists($key, $this->_put_args) ? $this->_xss_clean($this->_put_args[$key], $xss_clean) : FALSE;

            case 'delete':
                return array_key_exists($key, $this->_delete_args) ? $this->_xss_clean($this->_delete_args[$key], $xss_clean) : FALSE;
        }
    }

    protected function _xss_clean($val, $bool)
    {
        return $bool ? $this->security->xss_clean($val) : $val;
    }

}

?>