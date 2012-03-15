<?php

require_once APPPATH . '/libraries/Cf_REST_Controller.php';

class AppSettings extends Cf_REST_Controller
{

    function __construct()
    {
        parent::__construct();
        $this->username = $this->session->userdata('username');
    }

    function bluehostthreshold_get()
    {
        $this->respond_ok($this->setting_lib->get_blue_host_threshold());
    }
}
