<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

// Code here is run before ALL controllers
// BOOTSTRAP CODES
class cf_base_controller extends CI_Controller
{

    var $restClient = null;

    function __construct()
    {
        parent:: __construct();
        //for replication to work and specifies if this is the primary hub
        initializeHub();
        $this->setRestClient();
        $this->load->library(array('session', 'ion_auth', 'breadcrumb', 'breadcrumblist', 'onlineUsers', 'carabiner', 'user_agent', 'setting_lib', 'log_maintenance'));
        $this->lang->load('tooltip');
        $this->load->helper(array('url', 'html', 'language'));
        $this->lang->load('cf_message');
        $this->initialize();
    }

    function initialize()
    {
        $this->ion_auth->setRestClient($this->getRestClient());

    }

    function getProductName()
    {
        $client = $this->getRestClient();
        try
        {
            $data = $client->get('/');
            $data = json_decode($data, true);
            define('CFE_PRODUCT_NAME', 'CFEngine Enterprise');
            define('CFE_HUB_VERSION', $data['data'][0]['hubVersion']);
            define('CFE_CORE_VERSION', '3.3.x');
            /**
             * TODO :: change hubversion to product name
             */
            define(strtoupper($data['data'][0]['hubVersion']), true);
        }
        catch (Exception $e)
        {
            show_error($e->getMessage());
        }
    }

    function setRestClientAuthentication()
    {
        $username = $this->session->userdata('username');
        $password = $this->session->userdata('password');
        $this->restClient->setupAuth($username, $password);
    }

    function setRestClient()
    {
        $apiServer = $this->config->item('rest_server');
        $http_auth = 'basic';
        $config = array('base_url' => $apiServer);
        $this->load->library('pest_json', $config);
        $this->restClient = $this->pest_json;
    }

    function getRestClient()
    {
        return $this->restClient;
    }

}

class Cf_Controller extends cf_base_Controller
{

    function __construct()
    {
        parent::__construct();
        
        if (!$this->ion_auth->logged_in() && !is_ajax())
        {
       
            if (!$this->ion_auth->login_remembered_user())
            {
                redirect('login/index', 'refresh');
            }
            
        }
        
        $this->onlineusers->onlineusers();
        $this->setRestClientAuthentication();
     
        $this->ion_auth->setRestClient($this->getRestClient());
        $this->getProductName();
    }

}

?>