<?php

require_once APPPATH.'/libraries/Cf_REST_Controller.php';

class Host extends Cf_REST_Controller {

    function __construct() {
        parent::__construct();
        $this->username = $this->session->userdata('username');
    }

    function count_get()
    {
        try
        {
            $result = cfpr_host_count($this->username,
                    $this->param('colour'), $this->param_includes(), 
                    $this->param_excludes());

            $this->respond_ok((string)$result);
        }
        catch (CFModExceptionRBAC $e)
        {
            $this->respond_forbidden();
        }
    }
    
    function info_get($hostKey = NULL) {

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
    
    function compliance_get($arg = NULL) 
    {
        try
        {
            switch ($arg)
            {
                case 'timeseries':
                    $this->respond_ok(cfpr_host_compliance_timeseries($this->username,
                            $this->param_includes(), $this->param_excludes()));
                
                default:
                    $this->respond_not_found();
            }
        }
        catch (CFModExceptionGeneric $e)
        {
            $this->respond_internal_error($e->getMessage());
        }
        catch (CFModExceptionRBAC $e)
        {
           $this->respond_forbidden();
        }
    }

}
