<?php

require_once APPPATH.'libraries/Cf_REST_Controller.php';

class Astrolabe extends Cf_REST_Controller {
    
    function __construct() {
        parent::__construct();
        $this->load->model('astrolabe_model');
        $this->load->library('JIT_Utils');
        $this->username = $this->session->userdata('username');
    }

    function host_get() {
        
        $params = $this->uri->uri_to_assoc();
        
        $includes = array();
        $excludes = array();
        
        if (array_key_exists('includes', $params)) {
            $includes = explode(',', urldecode($params['includes']));
        }
        
        if (array_key_exists('excludes', $params)) {
            $excludes = explode(',', urldecode($params['excludes']));
        }

        echo cfpr_astrolabe_host_list($includes, $excludes);
    }
    
    
    function hostcount_get() 
    {    
        $params = $this->uri->uri_to_assoc();
        
        $includes = array();
        $colour = NULL;
        
        if (array_key_exists('includes', $params)) {
            $includes = explode(',', urldecode($params['includes']));
        }
        
        if (array_key_exists('colour', $params)) {
            $colour = $params['colour'];
        }

        echo cfpr_astrolabe_host_count($colour, $includes, array());
    }
    
    function profile_get($id = NULL) {
        
        if (is_null($id)) {
            $profileList = $this->astrolabe_model->profile_list($this->username);
            $this->respond(200, json_encode($profileList));
        }
        else {
            $profile = $this->astrolabe_model->profile_get($this->username, $id);
            if ($profile !== null)
            {
                $this->respond(200, json_encode($profile->data));
            }
            else
            {
                $this->respond(404);
            }
        }
    }
    
    function profile_put($id) {
        
        $nodeDescriptionList = json_decode($this->_put_args, true);
        
        $this->astrolabe_model->profile_delete($this->username, $id);
        $result = $this->astrolabe_model->profile_insert($this->username, $id, $nodeDescriptionList);
    }
    
    function profile_delete($id) 
    {
        if ($this->astrolabe_model->profile_delete($this->username, $id) == true)
        {
            $this->respond(204);
        }
        else
        {
            $this->respond(500, "Error deleting profile");
        }
        
    }
    
    function meters_get($hostKey = NULL)
    {
        $rawData = cfpr_host_meter($hostKey);
        $convertedData = $this->jit_utils->meters_to_jit($rawData);
        
        echo json_encode($convertedData);
    }
    
}
