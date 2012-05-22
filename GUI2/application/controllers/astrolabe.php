<?php

require_once APPPATH . 'libraries/Cf_REST_Controller.php';

class Astrolabe extends Cf_REST_Controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->model('astrolabe_model');
        $this->load->library('JIT_Utils');
        $this->username = $this->session->userdata('username');
    }

    function host_get()
    {
        echo cfpr_astrolabe_host_list($this->username,
                $this->param_includes(), $this->param_excludes());

    }

    function profile_get($id = NULL)
    {

        if (is_null($id))
        {
            $profileList = $this->astrolabe_model->profile_list($this->username);
            $this->respond(200, json_encode($profileList));
        }
        else
        {
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

    function profile_put($id)
    {
        $node_description_list = json_decode($this->_put_args, true);

        if ($this->__is_valid_node_description_list($node_description_list))
        {
            $this->astrolabe_model->profile_delete($this->username, $id);
            $this->astrolabe_model->profile_insert($this->username, $id, $node_description_list);
            $this->respond_ok();
        }
        else
        {
            $this->respond_not_acceptable();
        }
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

    function __is_valid_node_description_list($node_description_list)
    {
        if (!is_array($node_description_list))
        {
            return false;
        }

        foreach ($node_description_list as $child)
        {
            if (!$this->__is_valid_node_description($child))
            {
                return false;
            }
        }

        return true;
    }

    function __is_valid_node_description($node_description)
    {
        if (!array_key_exists('label', $node_description) ||
            !$node_description['label'])
        {
            return false;
        }

        if (!array_key_exists('classRegex', $node_description) ||
            !$node_description['classRegex'])
        {
            return false;
        }

        if (array_key_exists('children', $node_description))
        {
            if (!$this->__is_valid_node_description_list($node_description['children']))
            {
                return false;
            }
        }

        return true;
    }
}
