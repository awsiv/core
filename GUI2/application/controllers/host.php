<?php

require_once APPPATH.'/libraries/Cf_REST_Controller.php';

class Host extends Cf_REST_Controller {

    function __construct() {
        parent::__construct();
        $this->username = $this->session->userdata('username');
    }

    function info_get($hostKey) {

        if (is_null($hostKey)) {
            $this->respond_not_found();
        }

        try
        {            
            $result = cfpr_host_info_get($this->username, $hostKey);
            if (!is_null($result))
            {
                $this->respond_ok($result);
            }
            else
            {
                $this->respond_not_found();
            }
        }
        catch (CFModExceptionRBAC $e)
        {
            $this->respond_forbidden();
        }

        $this->respond_ok(json_encode($info));
    }
}
