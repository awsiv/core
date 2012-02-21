<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

class Cf_REST_Controller extends CI_Controller {

    protected $request = NULL;
    protected $response = NULL;
    protected $_get_args = array();
    protected $_post_args = array();
    protected $_put_args = array();

    public function __construct() {
        parent::__construct();
        
        initializeHub();

        $this->request->method = $this->_detect_method();
        $this->_get_args = array_merge($this->_get_args, $this->uri->ruri_to_assoc());
        $this->request->body = NULL;

        switch ($this->request->method) {
            case 'get':
                parse_str(parse_url($_SERVER['REQUEST_URI'], PHP_URL_QUERY), $get);
                empty($get) OR $this->_get_args = $get;
                break;

            case 'post':
                $this->_post_args = $_POST;
                $this->request->body = file_get_contents('php://input');
                break;

            case 'put':
                $this->_put_args = file_get_contents('php://input');
                break;

            case 'delete':
                break;
        }
    }

    public function _remap($object_called, $arguments) {

        $controller_method = $object_called . '_' . $this->request->method;

        if (!method_exists($this, $controller_method)) {
            $this->respond(array('status' => false, 'error' => 'Unknown method.'), 404);
        }

        call_user_func_array(array($this, $controller_method), $arguments);
    }

    public function respond($http_code, $data = NULL) {
        header('HTTP/1.1: ' . $http_code);
        header('Status: ' . $http_code);
        header('Content-Length: ' . strlen($data));
        exit($data);
    }
    
    public function respond_ok($data) {
        if (is_null($data)) {
            return $this->respond(204); // No Content
        }
        else {
            return $this->respond(200, $data); // OK
        }
    }
    
    public function respond_not_found() {
        return $this->respond(404);
    }
    
    public function respond_forbidden()
    {
        return $this->respond(403);
    }

    protected function _detect_method() {
        $method = strtolower($this->input->server('REQUEST_METHOD'));

        if (in_array($method, array('get', 'delete', 'post', 'put'))) {
            return $method;
        }

        return 'get';
    }

}
