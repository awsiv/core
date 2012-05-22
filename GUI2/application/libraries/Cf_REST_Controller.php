<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

class Cf_REST_Controller extends CI_Controller
{

    protected $request = NULL;
    protected $response = NULL;
    protected $_get_args = array();
    protected $_post_args = array();
    protected $_put_args = array();

    public function __construct()
    {
        parent::__construct();

        initializeHub();

        $this->load->library(array(
            'security',
            'setting_lib',
            'ion_auth',
            'OnlineUsers'
        ));

        if (!$this->ion_auth->logged_in() && !is_ajax())
        {
            $this->respond_unauthorized();
        }
        else
        {
            $this->onlineusers->OnlineUsers();
        }

        $this->request->method = $this->_detect_method();
        $this->_get_args = array_merge($this->_get_args, $this->uri->ruri_to_assoc());
        $this->request->body = NULL;

        switch ($this->request->method)
        {
            case 'get':
                $get = null;
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

    public function _remap($object_called, $arguments)
    {

        $controller_method = $object_called . '_' . $this->request->method;

        if (!method_exists($this, $controller_method))
        {
            $this->respond(array('status' => false, 'error' => 'Unknown method.'), 404);
        }

        try
        {
            call_user_func_array(array($this, $controller_method), $arguments);
        }
        catch (CFModExceptionRBAC $e)
        {
             return $this->respond(401);
        }
        catch(Exception $e){
             return $this->respond(500, $e->getMessage());
        }
    }

    public function respond($http_code, $data = NULL)
    {
        header('HTTP/1.1: ' . $http_code);
        header('Status: ' . $http_code);
        header('Content-Length: ' . strlen($data));
        header('Cache-Control: no-cache');
        header('Pragma: no-cache');
        exit($data);
    }

    public function respond_ok($data = null)
    {
        if (is_null($data))
        {
            return $this->respond(204); // No Content
        }
        else
        {
            return $this->respond(200, $data); // OK
        }
    }

    public function respond_unauthorized()
    {
        return $this->respond(401);
    }

    public function respond_not_found()
    {
        return $this->respond(404);
    }

    public function respond_forbidden()
    {
        return $this->respond(403);
    }

    public function respond_internal_error($message)
    {
        return $this->respond(500, $message);
    }

    public function respond_not_acceptable($message = null)
    {
        return $this->respond(406, $message);
    }

    protected function _detect_method()
    {
        $method = strtolower($this->input->server('REQUEST_METHOD'));

        if (in_array($method, array('get', 'delete', 'post', 'put')))
        {
            return $method;
        }

        return 'get';
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
