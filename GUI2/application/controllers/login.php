<?php

defined('BASEPATH') OR exit('No direct script access allowed');

if (!class_exists('Controller'))
{

    class Controller extends CI_Controller
    {

    }

}

class Login extends cf_base_controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->library(array('form_validation'));
        $this->lang->load('cf_message');
        //list of errors wrapped in <p> class of errors
        $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
        //$this->load->database();
        $this->load->helper('url');
        $this->config->load('ion_auth', TRUE);
    }

    //log the user out
    function logout()
    {
        $this->data['title'] = "Logout";

        //log the user out
        $logout = $this->ion_auth->logout();

        //redirect them back to the page they came from
        redirect('login/index', 'refresh');
    }

    //log the user in
    function index()
    {
        $this->data['title'] = "Login";
        $identifier = $this->config->item('identity', 'ion_auth');
        //validate form input
        if ($identifier == "username")
            $this->form_validation->set_rules('username', 'User Name', 'required');
        else
            $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');

        $this->form_validation->set_rules('password', 'Password', 'required');

        if ($this->form_validation->run() == true)
        {
            $remember = (bool) $this->input->post('remember');
            // add a session variable for the timezone information of the user for date conversion
            $this->session->set_userdata('user_timezone', $this->input->post('timezone'));

            if ($this->ion_auth->login(trim($this->input->post($identifier)), $this->input->post('password'), $remember))
            {
                //$this->session->set_flashdata('message', $this->ion_auth->messages());
                $username = trim($this->input->post($identifier));
                $session_data = array(
                    'username' => $username,
                    'roles' => $this->ion_auth->get_user_role($username),
                    'password' => $this->input->post('password')
                );
                $this->session->set_userdata($session_data);
                $this->session->set_flashdata('message', $this->ion_auth->errors());
                redirect('welcome/index', 'refresh');
            }
            else
            { //if the login was un-successful
                //redirect them back to the login page
                $this->session->set_flashdata('message', $this->ion_auth->errors());
                redirect('login/index', 'refresh'); //use redirects instead of loading views for compatibility with MY_Controller libraries
            }
        }
        else
        {  //the user is not logging in so display the login page
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');

            if ($identifier == "username")
            {
                $this->data['lbl_identifier'] = "Username";
                $this->data['identifier'] = array('name' => 'username',
                    'id' => 'username',
                    'type' => 'text',
                    'value' => $this->form_validation->set_value('username'),
                );
            }
            else
            {
                $this->data['lbl_identifier'] = "Email";
                $this->data['identifier'] = array('name' => 'email',
                    'id' => 'email',
                    'type' => 'text',
                    'value' => $this->form_validation->set_value('email'),
                );
            }


            $this->data['password'] = array('name' => 'password',
                'id' => 'password',
                'type' => 'password',
            );
            $mode = $this->setting_lib->get_authentication_mode();
            if ($mode != '' || $mode !== false)
            {
                $this->data['mode'] = $this->lang->line('login_' . $mode);
            }
            else
            {
                $this->data['mode'] = $this->lang->line('login_mode_not_found');
            }


            $this->data['timezone'] = array('name' => 'timezone',
                'id' => 'timezone',
                'type' => 'hidden',
            );

            $this->load->view('auth/login', $this->data);
        }
    }

}