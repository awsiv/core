<?php

require_once APPPATH.'/libraries/Cf_REST_Controller.php';

class Host extends Cf_REST_Controller {

    function __construct() {
        parent::__construct();
        $this->username = $this->session->userdata('username');
        $this->load->model('host_model');
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

    function countCompliance_get(){
      try{
         $hostcount=array(
                  "red"=> $this->host_model->getHostCount($this->username,'red',$this->param_includes(),$this->param_excludes()),
                  "green"=>$this->host_model->getHostCount($this->username,'green',$this->param_includes(),$this->param_excludes()),
                  "yellow"=>$this->host_model->getHostCount($this->username,'yellow',$this->param_includes(),$this->param_excludes())
         );
         $this->respond_ok(json_encode($hostcount));
         }catch(Exception $e){
          show_error_custom($e->getMessage(), $e->getCode());
       }
    }

    function countConnectivity_get(){
        try{
         $hostcount=array(
                  "blue"=> $this->host_model->getHostCount($this->username,'blue',$this->param_includes(),$this->param_excludes()),
                  "black"=>$this->host_model->getHostCount($this->username,'black',$this->param_includes(),$this->param_excludes())
         );
        $this->respond_ok(json_encode($hostcount));
         }catch(Exception $e){
          show_error_custom($e->getMessage(), $e->getCode());
       }
    }

    function info_get($hostKey = NULL) {

        if (is_null($hostKey)) {
            $this->respond_not_found();
        }

        try
        {
            $result = cfpr_host_info($this->username, $hostKey);
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

                case 'timeseries_shifts':
                    $this->respond_ok(cfpr_host_compliance_timeseries_shifts($this->username,
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
