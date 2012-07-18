<?php

defined('BASEPATH') OR exit('No direct script access allowed');

if (!class_exists('Controller'))
{

    class Controller extends CI_Controller
    {

    }

}

class Auth extends cf_controller
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
        
        $this->carabiner->css(
                array(
                    array('contextfinder.css'),
                    array('users_roles.css'),                    
                    array('tabs-custom.css')
                    )
                );
        $this->config->load('ion_auth', TRUE);
    }

    //redirect if needed, otherwise display the user list
    function index()
    {
        if (!$this->ion_auth->logged_in())
        {
            //redirect them to the login page
            redirect('login/index', 'refresh');
        }
        $this->data['title'] = $this->lang->line('mission_portal_title') . " - Admin";
        $this->data['username'] = $this->session->userdata('username');  
        //list the users
        $this->data['userroles'] = $this->session->userdata('roles');
        $this->data['is_admin'] = $this->ion_auth->is_admin();
        $this->data['message'] = (validation_errors()) ? '<p class="error">' . validation_errors() . '</p>' : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message'));
        $_data = array('event_loggedin' => true, 'ttlusr' => $this->onlineusers->total_users());
        //notifier( get_nodehost_from_server().'/userloggedin', $_data );
        if (is_ajax())
        {
            $this->data['users'] = $this->ion_auth->get_users_array();
            $this->load->view('auth/user_list', $this->data);
        }
        else
        {
            redirect($this->config->item('base_url'), 'refresh');
        }
    }

    function admin_page()
    {
        if (!$this->ion_auth->logged_in())
        {
            //redirect them to the login page
            redirect('login/index', 'refresh');
        }

        if ($this->ion_auth->is_admin(true) === false)
        {
            redirect('auth/permission_deny', 'refresh');
        }

       
        $this->_check_admin_permissions();
        
        $requiredjs = array(
            //array('widgets/classfinderbox.js'),
            array('widgets/contextfinder.js'),
            array('widgets/classfinder.js'),   
            array('widgets/policyfinder.js'),            
            array('jquery.form.js'),
            array('jquery.blockUI.js'),
        );

        $this->carabiner->js($requiredjs);

        $bc = array(
            'title' => $this->lang->line('breadcrumb_admin'),
            'url' => 'auth/admin_page',
            'isRoot' => false,
            'replace_existing' => true,
            'directchild' => true,
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->data['title'] = $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_admin');
        $this->data['title_header'] = "Admin";
        $this->data['username'] = $this->session->userdata('username');
        $this->data['message'] = (validation_errors()) ? '<p class="error">' . validation_errors() . '</p>' : $this->session->flashdata('message');

        // get system settings to protect "fall_back_for" user from editing
        $this->data['fall_back_for'] = $this->setting_lib->get_fall_back_for();

        // get system setting for RBAC, if it is OFF - show warning message on users and roles page
        $this->data['rbac_setting'] = $this->setting_lib->get_rbac_setting();

        //list the users
        $this->data['users'] = $this->ion_auth->get_users_array();
        $this->data['breadcrumbs'] = $this->breadcrumblist->display();
        $this->data['userroles'] = $this->session->userdata('roles');

        $this->data['is_admin'] = $this->ion_auth->is_admin();
        if (is_ajax())
        {
            $this->load->view('auth/user_list', $this->data);
        }
        else
        {
            $this->template->load('template', 'auth/index', $this->data);
        }
    }

    //change password
    function change_password($id = Null)
    {
        $this->form_validation->set_rules('old', 'Old password', 'required');
        $this->form_validation->set_rules('new', 'New Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[new_confirm]');
        $this->form_validation->set_rules('new_confirm', 'Confirm New Password', 'required');
        $this->data['userrole'] = $this->session->userdata('role');
        $this->data['is_admin'] = $this->ion_auth->is_admin();
        if (!$this->ion_auth->logged_in())
        {
            redirect('login/index', 'refresh');
        }
        //$user = $this->ion_auth->get_user($this->session->userdata('user_id'));

        if ($this->form_validation->run() == false)
        { //display the form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');

            $this->data['old_password'] = array('name' => 'old',
                'id' => 'old',
                'type' => 'password',
                'value' => $this->form_validation->set_value('old')
            );
            $this->data['new_password'] = array('name' => 'new',
                'id' => 'new',
                'type' => 'password',
                'value' => $this->form_validation->set_value('new')
            );
            $this->data['new_password_confirm'] = array('name' => 'new_confirm',
                'id' => 'new_confirm',
                'type' => 'password',
                'value' => $this->form_validation->set_value('new_confirm')
            );
            //render
            $this->load->view('auth/change_password', $this->data);
        }
        else
        {
            $username = isset($id) ? $id : $this->session->userdata('username');
            
            $this->load->model('repository_model');
            $userInfo = array('userId' => $username);
            $oldKey = $this->repository_model->get_key($userInfo); // have to have old pass key before change
            
            $change = $this->ion_auth->change_password($username, $this->input->post('old'), $this->input->post('new'));

            if ($change)
            { //if the password was successfully changed
                //$this->session->set_flashdata('message', $this->ion_auth->messages());
                //$this->logout();
                // change the svn password as well here
                $this->session->set_userdata('password',$this->input->post('new'));
                $this->change_svn_password_entries($username, $oldKey);
                if (is_ajax())
                {
                    $this->data['message'] = $this->ion_auth->messages();
                    $this->data['users'] = $this->ion_auth->get_users_array();

                    $this->data['old_password'] = array('name' => 'old',
                        'id' => 'old',
                        'type' => 'password',
                        'value' => ''
                    );
                    $this->data['new_password'] = array('name' => 'new',
                        'id' => 'new',
                        'type' => 'password',
                        'value' => ''
                    );
                    $this->data['new_password_confirm'] = array('name' => 'new_confirm',
                        'id' => 'new_confirm',
                        'type' => 'password',
                        'value' => ''
                    );

                    $this->load->view('auth/change_password', $this->data);
                }
                else
                {
                    $this->session->set_flashdata('message', $this->ion_auth->messages());
                    redirect('auth/index', 'refresh');
                }
            }
            else
            {
                //$this->session->set_flashdata('message', $this->ion_auth->errors());
                $this->ion_auth->set_info("org_passwd_valid");
                $this->data['message'] = $this->ion_auth->infos() . $this->ion_auth->errors();
                $this->data['old_password'] = array('name' => 'old',
                    'id' => 'old',
                    'type' => 'password',
                    'value' => $this->form_validation->set_value('old')
                );
                $this->data['new_password'] = array('name' => 'new',
                    'id' => 'new',
                    'type' => 'password',
                    'value' => $this->form_validation->set_value('new')
                );
                $this->data['new_password_confirm'] = array('name' => 'new_confirm',
                    'id' => 'new_confirm',
                    'type' => 'password',
                    'value' => $this->form_validation->set_value('new_confirm')
                );

                $this->load->view('auth/change_password', $this->data);
            }
        }
    }

    /**
     * Changes the managed svn repository password.
     * @param type $identity
     */
    function change_svn_password_entries($username, $oldkey)
    {

        $this->load->model('repository_model');
        $key = $oldkey;

        $repo = $this->repository_model->get_all_repository($username);

        foreach ($repo as $obj)
        {
            $info = array('userId' => $obj['userId'], 'username' => $obj['username'], 'repoPath' => $obj['repoPath'], 'password' => $obj['password']);
            $password = $this->repository_model->decrypt_password($info, $key);
            $newInfo = $info;
            $newInfo['password'] = $password;

            $ret = $this->repository_model->update_repository($info, $newInfo); // update the password with re-encryption by new hash
        }
    }

    //forgot password
    function forgot_password()
    {
        $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
        if ($this->form_validation->run() == false)
        {
            //setup the input
            $this->data['email'] = array('name' => 'email',
                'id' => 'email',
            );
            //set any errors and display the form
            $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
            $this->load->view('auth/forgot_password', $this->data);
        }
        else
        {
            //run the forgotten password method to email an activation code to the user
            $forgotten = $this->ion_auth->forgotten_password($this->input->post('email'));

            if ($forgotten)
            { //if there were no errors
                $this->session->set_flashdata('message', $this->ion_auth->messages());
                redirect("login/index", 'refresh'); //we should display a confirmation page here instead of the login page
            }
            else
            {
                $this->session->set_flashdata('message', $this->ion_auth->errors());
                redirect("auth/forgot_password", 'refresh');
            }
        }
    }

    //reset password - final step for forgotten password
    public function reset_password($code)
    {
        $reset = $this->ion_auth->forgotten_password_complete($code);
        if ($reset)
        {  //if the reset worked then send them to the login page
            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("login/index", 'refresh');
        }
        else
        { //if the reset didnt work then send them back to the forgot password page
            $this->session->set_flashdata('message', $this->ion_auth->errors());
            redirect("auth/forgot_password", 'refresh');
        }
    }

    //create a new user
    function create_user()
    {
        $this->data['title'] = "Create User";
        $this->data['user_type'] = 'internal';


        if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin())
        {
            redirect('auth', 'refresh');
        }

        $this->_check_admin_permissions();

        //validate form input
        $this->form_validation->set_rules('user_name', 'User Name', 'required|xss_clean|unique[users.username]');
        $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email|unique[users.email]');
        $this->form_validation->set_rules('password', 'Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[password_confirm]');
        $this->form_validation->set_rules('password_confirm', 'Password Confirmation', 'required');
        $this->form_validation->set_rules('roles[]', 'role', 'xss_clean');

        if ($this->form_validation->run() == true)
        {
           $data=array(
            'username' => strtolower($this->input->post('user_name')),
            'email' => $this->input->post('email'),
            'password' => $this->input->post('password'),
            'roles' => $this->input->post('roles')
            );
        }

        if ($this->form_validation->run() == true && $this->ion_auth->register($data))
        { //check to see if we are creating the user
            //redirect them back to the admin page
            if (is_ajax())
            {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->data['userrole'] = $this->session->userdata('role');
                $this->data['is_admin'] = $this->ion_auth->is_admin();

                // on success - return json - we will  not redraw entire form
                $data['status'] = 'all_ok';
                $data['responseText'] = $this->load->view('auth/user_list', $this->data, true);

                $result = json_encode($data);
                echo $result;
                return;
            }
            else
            {
                $this->session->set_flashdata('message', "User Created");
                redirect("auth", 'refresh');
            }
        }
        else
        { //display the create user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));

            // on error - return json - we will  not redraw entire form
            if ($this->data['message'] != '')
            {
                $data['status'] = 'validation_error';
                $data['responseText'] = $this->data['message'];

                $result = json_encode($data);
                echo $result;
                return;
            }


            $this->data['user_name'] = array('name' => 'user_name',
                'id' => 'user_name',
                'type' => 'text',
                'value' => $this->form_validation->set_value('user_name'),
            );

            $this->data['email'] = array('name' => 'email',
                'id' => 'email',
                'type' => 'text',
                'value' => $this->form_validation->set_value('email'),
            );
            $this->data['password'] = array('name' => 'password',
                'id' => 'password',
                'type' => 'password',
                'value' => $this->form_validation->set_value('password'),
            );
            $this->data['password_confirm'] = array('name' => 'password_confirm',
                'id' => 'password_confirm',
                'type' => 'password',
                'value' => $this->form_validation->set_value('password_confirm'),
            );

            $selected_roles = $this->input->post('roles');

            // create data for checkbox
            if (!empty($selected_roles))
            {
                foreach ($selected_roles as $role)
                {
                    $this->data['user_roles'][$role] = array('name' => 'roles[]',
                        'value' => $role,
                        'checked' => $this->form_validation->set_checkbox('roles[]', $role)
                    );
                }
            }
            else
                $this->data['user_roles'] = '';

            $roles = $this->ion_auth->get_roles($this->session->userdata('username'));


            // get roles list, only roles which NOT assigned to user
            if (!empty($selected_roles))
            {
                //prepare data
                $selected_roles_tmp = array();

                foreach ($selected_roles as $role)
                {
                    $selected_roles_tmp[$role] = array(
                        'name' => $role,
                    );
                }
                //create roles array, which will look as [key] = value
                $roles_tmp = array();
                foreach ($roles as $item => $role)
                {
                    //$roles_tmp[$role['name']] = $role['name'];
                    $roles_tmp[$role['name']] = array(
                        'name' => $role['name'],
                    );
                }

                //diff data
                $roles = arrayRecursiveDiff($roles_tmp, $selected_roles_tmp);
            }


            foreach ($roles as $role)
            {
                $this->data['roles'][$role['name']] = array(
                    'value' => $role['name'],
                );
            }

            $this->data['op'] = 'create';
            $this->load->view('auth/add_edit_user', $this->data);
        }
    }

    function _get_csrf_nonce()
    {
        $this->load->helper('string');
        $key = random_string('alnum', 8);
        $value = random_string('alnum', 20);
        $this->session->set_flashdata('csrfkey', $key);
        $this->session->set_flashdata('csrfvalue', $value);

        return array($key => $value);
    }

    function _valid_csrf_nonce()
    {
        if ($this->input->post($this->session->flashdata('csrfkey')) !== FALSE &&
                $this->input->post($this->session->flashdata('csrfkey')) == $this->session->flashdata('csrfvalue'))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    function view_profile()
    {
        if ($this->setting_lib->get_authentication_mode() != 'internal')
        {
            $userdata = $this->ion_auth->get_ldap_user_details_from_local_db($this->session->userdata('user_id'));
        }
        else
        {
            $userdata = $this->ion_auth->get_user($this->session->userdata('username'));
        }

        $this->data['user'] = $userdata;

        $requiredjs = array(
            array('jquery.form.js'),
            array('jquery.blockUI.js'),
        );

        $this->carabiner->js($requiredjs);
        $bc = array(
            'title' => 'Profile',
            'url' => 'auth/view_profile',
            'isRoot' => false,
            'replace_existing' => true,
            'directchild' => true,
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $this->data['title'] = $this->lang->line('mission_portal_title') . " - Profile";
        $this->data['title_header'] = "Profile";
        $this->data['username'] = $this->session->userdata('username');
        $this->data['breadcrumbs'] = $this->breadcrumblist->display();

        $this->template->load('template', 'auth/view_profile', $this->data);
    }

    function edit_user($id)
    {
        $this->data['title'] = "Edit User";
        $this->data['user_type'] = 'internal';

        if (!$this->ion_auth->logged_in())
        {
            redirect('auth', 'refresh');
        }

        if ($this->ion_auth->is_admin(true) === false)
        {
            redirect('auth/permission_deny', 'refresh');
        }

        $this->_check_admin_permissions();

        //validate form input
        $this->form_validation->set_rules('user_name', 'First Name', 'required|xss_clean');
        $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
        $this->form_validation->set_rules('roles[]', 'Role', 'xss_clean');


        if ($this->form_validation->run() == true)
        {
            $data = array(
                'username' => $this->input->post('user_name'),
                'email' => $this->input->post('email'),
                'roles' => $this->input->post('roles'),
            );
        }

        if ($this->form_validation->run() == true && $this->ion_auth->update_user($id, $data))
        { //check to see if we are creating the user
            //redirect them back to the admin page
            if ($this->input->post('reset_password'))
            {
                $forgotten = $this->ion_auth->forgotten_password($this->input->post('email'));
            }
            
            if (is_ajax())
            {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                // $this->data['userroles'] = $this->session->userdata('roles');
                $this->data['is_admin'] = $this->ion_auth->is_admin();


                // on success - return json - we will  not redraw entire form
                $data['status'] = 'all_ok';
                $data['responseText'] = $this->load->view('auth/user_list', $this->data, true);

                $result = json_encode($data);
                echo $result;
                return;
            }
            else
            {
                $this->session->set_flashdata('message', "User Updated");
                redirect("auth", 'refresh');
            }
        }
        else
        { //display the edit user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));

            // on error - return json - we will  not redraw entire form
            if ($this->data['message'] != '')
            {
                $data['status'] = 'validation_error';
                $data['responseText'] = $this->data['message'];

                $result = json_encode($data);
                echo $result;
                return;
            }

            $user = $this->ion_auth->get_user($id);

            $this->data['user_name'] = array('name' => 'user_name',
                'id' => 'first_name',
                'type' => 'text',
                'value' => $this->form_validation->set_value('user_name', $user['username']),
            );

            $this->data['email'] = array('name' => 'email',
                'id' => 'email',
                'type' => 'text',
                'value' => $this->form_validation->set_value('email',  key_exists('email', $user)?$user['email']:''),
            );

            //get user roles
            $assigned_roles = $this->ion_auth->get_user_role($id);


            // create data for checkbox
            if (!empty($assigned_roles))
            {
                foreach ($assigned_roles as $role)
                {
                    $this->data['user_roles'][$role] = array('name' => 'roles[]',
                        'value' => $role,
                        'checked' => $this->form_validation->set_checkbox('roles[]', $role)
                    );
                }
            }
            else
            {
                $this->data['user_roles'] = '';
            }

            $this->data['roles'] = $this->_get_roles_not_assigned_to_user($assigned_roles);

            $this->data['op'] = 'edit';
            $this->load->view('auth/add_edit_user', $this->data);
        }
    }

    function edit_user_ldap($username)
    {
        $this->data['title'] = "Edit User";
        $this->data['user_type'] = 'external';


        if (!$this->ion_auth->logged_in())
        {
            redirect('auth', 'refresh');
        }

        if ($this->ion_auth->is_admin(true) === false)
        {
            redirect('auth/permission_deny', 'refresh');
        }

        //validate form input
        $this->form_validation->set_rules('user_name', 'First Name', 'required|xss_clean');
        $this->form_validation->set_rules('roles[]', 'Role', 'required|xss_clean');


        if ($this->form_validation->run() == true && $this->ion_auth->update_ldap_users($username, $this->input->post('roles')))
        { //check to see if we are creating the user
            //redirect them back to the admin page
            /* $user = $this->ion_auth->get_ldap_user_details_from_local_db($username);
              $user_roles=array();
              if($user !==NULL){
              $user_roles=$user->roles;
              }
             */
            if (is_ajax())
            {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();

                $this->data['is_admin'] = $this->ion_auth->is_admin();

                $data['status'] = 'all_ok';
                $data['responseText'] = $this->load->view('auth/user_list', $this->data, true);

                $result = json_encode($data);
                echo $result;
                return;
            }
            else
            {

                $this->session->set_flashdata('message', "User Updated");
                redirect("auth", 'refresh');
            }
        }
        else
        { //display the create user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
            // on error - return json - we will  not redraw entire form
            if ($this->data['message'] != '')
            {
                $data['status'] = 'validation_error';
                $data['responseText'] = $this->data['message'];

                $result = json_encode($data);
                echo $result;
                return;
            }


            //$phone=explode('-',$user->phone);
            $this->data['user_name'] = array('name' => 'user_name',
                'id' => 'first_name',
                'type' => 'text',
                'value' => $this->form_validation->set_value('user_name', $username),
            );


            $user = $this->ion_auth->get_ldap_user_details_from_local_db($username);

            $assigned_roles = array();
            if ($user !== NULL && is_object($user) && property_exists($user,'roles'))
            {
                $assigned_roles = $user->roles;
            }
           
           
            // create data for checkbox
            if (!empty($assigned_roles))
            {
                foreach ($assigned_roles as $role)
                {
                    $this->data['user_roles'][$role] = array('name' => 'roles[]',
                        'value' => $role,
                        'checked' => $this->form_validation->set_checkbox('roles[]', $role)
                    );
                }
            }
            else
            {
                $this->data['user_roles'] = '';
            }

            $this->data['roles'] = $this->_get_roles_not_assigned_to_user($assigned_roles);

            $this->data['op'] = 'edit';

            //$this->data['role']=array('name'=>'role','options'=>$options,'default'=>set_value('role', $user->role_id));
            $this->load->view('auth/add_edit_user', $this->data);
        }
    }

    function _get_roles_not_assigned_to_user($assignedRoles = array())
    {

        $roles = $this->ion_auth->get_roles();
        // get roles list, only roles which NOT assigned to user
        if (!empty($assignedRoles))
        {
            //prepare data
            $assignedRoles_tmp = array();

            foreach ($assignedRoles as $role)
            {
                $assignedRoles_tmp[$role] = array(
                    'name' => $role,
                );
            }
            //create roles array, which will look as [key] = value
            $roles_tmp = array();
            foreach ($roles as $item => $role)
            {
                //$roles_tmp[$role['name']] = $role['name'];
                $roles_tmp[$role['name']] = array(
                    'name' => $role['name'],
                );
            }

            //diff data
            $roles = arrayRecursiveDiff($roles_tmp, $assignedRoles_tmp);
        }

        $available_roles = array();

        foreach ($roles as $role)
        {
            $available_roles[$role['name']] = array(
                'value' => $role['name'],
            );
        }
        return $available_roles;
    }

    function manage_role($op = false, $rolename = false)
    {

        if (!$this->ion_auth->logged_in())
        {
            redirect('auth', 'refresh');
        }

        $this->_check_admin_permissions();

        if ($this->ion_auth->is_admin(true) === false)
        {
            redirect('auth/permission_deny', 'refresh');
        }

        $this->data['is_admin'] = $this->ion_auth->is_admin();
      
        if (!empty($op))
        {
            $this->data['title'] = "Create role";
            $this->data['operation'] = "Create";

            if ($op != 'edit')
                $this->form_validation->set_rules('name', 'Name', 'required|xss_clean|unique[roles.name]');
            else
                $this->form_validation->set_rules('name', 'Name', 'required|xss_clean');

            $this->form_validation->set_rules('description', 'Description', 'required|xss_clean|trim');
            
            $this->form_validation->set_rules('crxi', 'Include classes', 'required|xss_clean|callback_includeClassValidate');
            
            $this->form_validation->set_rules('crxx', 'Exclude classes', 'xss_clean');
            $this->form_validation->set_rules('brxi', 'Include bundlers', 'xss_clean');
            $this->form_validation->set_rules('brxx', 'Include bundlers', 'xss_clean');

            
            if ($this->form_validation->run() == true)
            {
                $data = array(
                    'name' => $this->input->post('name'),
                    'description' => $this->input->post('description'),
                    'includeContext' => implode(",", array_filter(array_unique((array) $this->input->post('crxi')))),
                    'excludeContext' => implode(",", array_filter(array_unique((array) $this->input->post('crxx')))),
                    'includeBundles' => implode(",", array_filter(array_unique((array) $this->input->post('brxi')))),
                    'excludeBundles' => implode(",", array_filter(array_unique((array) $this->input->post('brxx'))))
                );

                if (($op == 'edit' && !$this->ion_auth->update_role($rolename, $data)))
                {
                    $this->__load_role_add_edit($op, $rolename);
                    return;
                }
                
                if ($op == 'create' && $this->ion_auth->create_role($data) === FALSE)
                {
                    $this->__load_role_add_edit($op, $rolename);
                    return;
                }

                if (is_ajax())
                {
                    $this->data['message'] = $this->ion_auth->messages();
                    $this->data['roles'] = $this->ion_auth->get_roles($this->session->userdata('username'));


                    // on success - return json - we will  not redraw entire form
                    $data['status'] = 'all_ok';
                    $data['responseText'] = $this->load->view('auth/list_role', $this->data, true);

                    $result = json_encode($data);
                    echo $result;
                    return;
                }
                else
                {
                    $this->session->set_flashdata('message', $this->ion_auth->messages());
                    redirect("auth/manage_role", 'refresh');
                }
            }
            else
            {
                $this->__load_role_add_edit($op, $rolename);
            }
        }
        else
        {
             
            $this->data['roles'] = $this->ion_auth->get_roles();
           
            $this->data['message'] = (validation_errors()) ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message'));
            $this->load->view('auth/list_role', $this->data);
        }
    }
    /**
     *Function to validate include classes for roles. Each role MUST have one class into include(crxi) list
     * @param type $crxi
     * @return boolean 
     */
    function includeClassValidate($crxi) {
        $crxi = array_filter(array_unique($crxi));

        foreach($crxi as $item => $value)
        {
            if (!empty($value))
            {
                return true;
            }
        }

        $this->form_validation->set_message('includeClassValidate', 'Role must have at least one include class');        
        return false;
    }
    
    function __load_role_add_edit($op, $rolename)
    {
        $this->_check_admin_permissions();

        $this->load->helper('create_html_list_from_string');
        $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
        // on error - return json - we will  not redraw entire form
        if ($this->data['message'] != '')
        {
            $data['status'] = 'validation_error';
            $data['responseText'] = $this->data['message'];

            $result = json_encode($data);
            echo $result;
            return;
        }

        $this->data['name'] = array('name' => 'name',
            'id' => 'name',
            'type' => 'text',
            'value' => $this->form_validation->set_value('name'),
        );
        $this->data['description'] = array('name' => 'description',
            'id' => 'description',
            'type' => 'text',
            'rows' => '3',
            'cols' => '20',
            'value' => $this->form_validation->set_value('description'),
        );
        $this->data['crxi'] = array('name' => "crxi",
            'id' => 'crxi',
            'type' => 'text',
            'value' => $this->form_validation->set_value('crxi'),
        );

        $this->data['crxx'] = array('name' => "crxx",
            'id' => 'crxx',
            'type' => 'text',
            'value' => $this->form_validation->set_value('crxx'),
        );

        $this->data['brxi'] = array('name' => "brxi",
            'id' => 'brxi',
            'type' => 'text',
            'value' => $this->form_validation->set_value('brxi'),
        );

        $this->data['brxx'] = array('name' => "brxx",
            'id' => 'brxx',
            'type' => 'text',
            'value' => $this->form_validation->set_value('brxx'),
        );

        if ($op == 'edit')
        {
            if (empty($rolename))
            {
                show_error('cannot update - record not specified', 500);
                return;
            }

            $role = $this->ion_auth->get_role_detail($rolename);
            //if details could not be retrived from the servera about role, don't return edit form.
            if($role!==false)
            {
            $this->data['title'] = "Update Role";
            $this->data['operation'] = "Edit";
            $this->data['name']['enable'] = 'enable';
            $this->data['name']['value'] = $this->form_validation->set_value('name', $role['name']);
            $this->data['description']['value'] = $this->form_validation->set_value('description', array_key_exists('description', $role) ? $role['description'] : "");

            //classes
            $this->data['crxi']['value'] = $this->form_validation->set_value('crxi', array_key_exists('includeContext', $role) ? $role['includeContext'] : "");
            $this->data['crxx']['value'] = $this->form_validation->set_value('crxx', array_key_exists('excludeContext', $role) ? $role['excludeContext'] : "");

            // bundles
            $this->data['brxi']['value'] = $this->form_validation->set_value('brxi', array_key_exists('includeBundles', $role) ? $role['includeBundles'] : "");
            $this->data['brxx']['value'] = $this->form_validation->set_value('brxx', array_key_exists('excludeBundles', $role) ? $role['excludeBundles'] : "");
            }
            else{
                $this->data['message']=$this->ion_auth->errors();
                return;
            }

        }

        $this->data['op'] = $op;

        $this->load->view('auth/add_edit_role', $this->data);
    }

    function delete_user($id)
    {

        try
        {
            $this->ion_auth->delete_user($id);
        }
        catch (Exception $e)
        {

            $this->ion_auth->set_error($e->getMessage());
        }

        if (is_ajax())
        {
            $this->data['message'] = $this->ion_auth->errors() ? $this->ion_auth->errors() : $this->ion_auth->messages();
            $this->data['users'] = $this->ion_auth->get_users_array();
            $this->data['userrole'] = $this->session->userdata('role');
            $this->data['is_admin'] = $this->ion_auth->is_admin();
            $this->load->view('auth/user_list', $this->data);
        }
        else
        {

            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("auth", 'refresh');
        }
    }

    /**
     * Delete role
     *
     * @param type $name  - role name
     */
    function delete_role($name)
    {

        if ($name == '')
        {
            $this->permission_deny($this->lang->line('empty_rolename'));
            return;
        }

        if ($name == 'admin')
        {
            $this->permission_deny($this->lang->line('admin_role_delete_forbidden'));
            return;
        }

        if ($this->ion_auth->is_admin(true) === false)
        {
            $this->permission_deny($this->lang->line('no_permission'));
            return;
        }

        $this->ion_auth->delete_role($name);
       
        if (is_ajax())
        {
            $this->data['message'] = $this->ion_auth->errors() ? $this->ion_auth->errors() : $this->ion_auth->messages();
            $this->data['roles'] = $this->ion_auth->get_roles($this->session->userdata('username'));
            $this->data['is_admin'] = $this->ion_auth->is_admin();

            $this->load->view('auth/list_role', $this->data);
        }
        else
        {
            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("auth/manage_role", 'refresh');
        }
    }

    
    /*
     * if ajax - show message on top of the page div id=error_status
     *
     * if not ajax - redirect to no_permission page, with some message
     */

    function permission_deny($message = '')
    {
        if ($message == '')
        {
            $message = $this->lang->line('no_permission');
        }

        if (is_ajax())
        {
            $this->output->set_status_header(401, $message);
            echo $message;
            exit();
        }
        else
        {
            $bc = array(
                'title' => $this->lang->line('mission_portal_title'),
                'url' => 'welcome/index',
                'isRoot' => true
            );

            $this->breadcrumb->setBreadCrumb($bc);

            $this->data['title'] = $this->lang->line('mission_portal_title');
            $this->data['message'] = $this->lang->line('no_permission');
            $this->data['breadcrumbs'] = $this->breadcrumblist->display();
            $this->session->set_flashdata('message', $this->lang->line('no_permission'));

            $this->template->load('template', 'auth/no_permission', $this->data);
        }
    }

    /**
     *  Show page with explanation about permission for local admin
     *
     */
    function no_permission_for_local_admin()
    {

        $this->data['title'] = $this->lang->line('mission_portal_title');
        $this->data['message'] = $this->lang->line('no_permission');

        $mode = $this->setting_lib->get_authentication_mode();

        switch ($mode)
        {
            case 'ldap': $this->data['current_auth_mode'] = 'LDAP';
                break;
            case 'active_directory': $this->data['current_auth_mode'] = 'Active Directory';
                break;
            default: $this->data['current_auth_mode'] = $mode;
        }

        $this->template->load('template', 'auth/no_permission_for_local_admin', $this->data);
    }

    /**
     * check if current admin user has permissions to work with users and roles
     */
    function _check_admin_permissions()
    {
        //if user auhenticated as internal, and curremt MP mode is LDAP or AD  - show message that he CAN'T work with users
       if ($this->session->userdata('mode') == 'internal' && $this->setting_lib->get_authentication_mode() != 'internal')
        {
            redirect('auth/no_permission_for_local_admin', 'location', 302);
        }
    }

}

