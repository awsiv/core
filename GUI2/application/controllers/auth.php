<?php

defined('BASEPATH') OR exit('No direct script access allowed');

if (!class_exists('Controller')) {

    class Controller extends CI_Controller {
        
    }

}

class Auth extends Controller {

    function __construct() {
        parent::__construct();
        initializeHub();
        init_productname();
        $this->load->library(array('ion_auth', 'ion_auth', 'form_validation', 'breadcrumb', 'breadcrumblist', 'carabiner', 'onlineUsers','user_agent','setting_lib'));
        $this->lang->load('cf_message');        
        //list of errors wrapped in <p> class of errors
        $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
        //$this->load->database();
        $this->load->helper('url');
    
        $this->config->load('ion_auth', TRUE);
    }

    //redirect if needed, otherwise display the user list
    function index() {
        if (!$this->ion_auth->logged_in()) {
            //redirect them to the login page
            redirect('auth/login', 'refresh');
        }
        else {           
            $this->data['title'] = $this->lang->line('mission_portal_title')." - Admin";
            $this->data['username'] = $this->session->userdata('username');
            //list the users
            //
            $this->data['userroles'] = $this->session->userdata('roles');
            $this->data['is_admin'] = $this->ion_auth->is_admin();
            $this->data['message'] = (validation_errors()) ? '<p class="error">' . validation_errors() . '</p>' : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message'));

            $_data = array('event_loggedin' => true, 'ttlusr' => $this->onlineusers->total_users());
            //notifier( get_nodehost_from_server().'/userloggedin', $_data );
            if (is_ajax ()) {
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->load->view('auth/user_list', $this->data);
            } else {
                // $this->template->load('template', 'auth/index',$this->data);
                redirect($this->config->item('base_url'), 'refresh');
            }           
        }
    }

    function admin_page() {
         if (!$this->ion_auth->logged_in()) {
            //redirect them to the login page
            redirect('auth/login', 'refresh');
        }
        
        $requiredjs = array(
            array('widgets/classfinderbox.js')            
        );
        
        $this->carabiner->js($requiredjs);   
        
        $bc = array(
            'title' => $this->lang->line('breadcrumb_admin'),
            'url' => 'auth/admin_page',
            'isRoot' => false,
           'replace_existing'=>true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->data['title'] = $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_admin');
        $this->data['title_header'] = "Admin";
        $this->data['username'] = $this->session->userdata('username');
        $this->data['message']  = (validation_errors()) ? '<p class="error">' . validation_errors() . '</p>' : $this->session->flashdata('message');
        
        // get system settings to protect "fall_back_for" user from editing
        $this->load->model('settings_model');
        $settings = $this->settings_model->get_app_settings();
        $this->data['fall_back_for'] = $settings->fall_back_for;
      
        //list the users
        $this->data['users'] = $this->ion_auth->get_users_array();         
        //$this->load->view('auth/index', $this->data);
        $this->data['breadcrumbs'] = $this->breadcrumblist->display();
        $this->data['userroles']    = $this->session->userdata('roles');

        $this->data['is_admin'] = $this->ion_auth->is_admin();
        if (is_ajax ()) {
            $this->load->view('auth/user_list', $this->data);
            //$this->load->view('welcome/index',$this->data);
        } else {
            $this->template->load('template', 'auth/index', $this->data);
        }
    }

    //log the user in
    function login() {
        $this->data['title'] = "Login";
        $identifier = $this->config->item('identity', 'ion_auth');
        //validate form input
        if ($identifier == "username")
            $this->form_validation->set_rules('username', 'User Name', 'required');
        else
            $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');

        $this->form_validation->set_rules('password', 'Password', 'required');

        if ($this->form_validation->run() == true) { //check to see if the user is logging in
            //check for "remember me"
            $remember = (bool) $this->input->post('remember');
            
            // add a session variable for the timezone information of the user for date conversion 
             $this->session->set_userdata('user_timezone', $this->input->post('timezone'));

            if ($this->ion_auth->login(trim($this->input->post($identifier)), $this->input->post('password'), $remember)) { //if the login is successful
                //redirect them back to the home page
                $this->session->set_flashdata('message', $this->ion_auth->messages());
                //redirect($this->config->item('base_url'), 'refresh');
                redirect('auth/index', 'refresh');
            } else { //if the login was un-successful
                //redirect them back to the login page
                $this->session->set_flashdata('message', $this->ion_auth->errors());
                redirect('auth/login', 'refresh'); //use redirects instead of loading views for compatibility with MY_Controller libraries
            }
        } else {  //the user is not logging in so display the login page
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');

            if ($identifier == "username") {
                $this->data['lbl_identifier'] = "Username";
                $this->data['identifier'] = array('name' => 'username',
                    'id' => 'username',
                    'type' => 'text',
                    'value' => $this->form_validation->set_value('username'),
                );
            } else {
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
            $mode=$this->setting_lib->get_backend_mode();
            if($mode!='' || $mode !==false){
            $this->data['mode']=$this->lang->line('login_'.$mode);
            }
            else{
               $this->data['mode']=$this->lang->line('login_mode_not_found');
            }
            
            
            $this->data['timezone'] = array('name' => 'timezone',
                'id' => 'timezone',
                'type' => 'hidden',
            );

            $this->load->view('auth/login', $this->data);
        }
    }

    //log the user out
    function logout() {
        $this->data['title'] = "Logout";

        //log the user out
        $logout = $this->ion_auth->logout();

        //redirect them back to the page they came from
        redirect('auth', 'refresh');
    }

    //change password
 function change_password($id=Null) {
        $this->form_validation->set_rules('old', 'Old password', 'required');
        $this->form_validation->set_rules('new', 'New Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[new_confirm]');
        $this->form_validation->set_rules('new_confirm', 'Confirm New Password', 'required');
        $this->data['userrole'] = $this->session->userdata('role');
        $this->data['is_admin'] = $this->ion_auth->is_admin();
        if (!$this->ion_auth->logged_in()) {
            redirect('auth/login', 'refresh');
        }
        //$user = $this->ion_auth->get_user($this->session->userdata('user_id'));

        if ($this->form_validation->run() == false) { //display the form
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
        } else {
            $identity = isset($id) ? $id : $this->session->userdata($this->session->userdata('user_id'));
            $result = $this->mongo_db->select(array('password', 'username'))
                            ->where(array('_id' => new MongoId($identity)))
                            ->limit(1)
                            ->get_object('users');

            $username = $result->username;
            $oldPass = $result->password;
            $this->load->model('repository_model');
            $userInfo = array('userId'=>$username);
            $oldKey = $this->repository_model->get_key($userInfo); // have to have old pass key before change
            
            $change = $this->ion_auth->change_password($identity, $this->input->post('old'), $this->input->post('new'));

            if ($change) { //if the password was successfully changed
                //$this->session->set_flashdata('message', $this->ion_auth->messages());
                //$this->logout();
                // change the svn password as well here
                $this->change_svn_password_entries($username, $oldKey);
                if (is_ajax ()) {
                    $this->data['message'] = $this->ion_auth->messages();
                    $this->data['users'] = $this->ion_auth->get_users_array();

                    $this->load->view('auth/user_list', $this->data);
                } else {
                    $this->session->set_flashdata('message', $this->ion_auth->messages());
                    redirect('auth/index', 'refresh');
                }
            } else {
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
    function change_svn_password_entries($username, $oldkey) {

        $this->load->model('repository_model');
        $key = $oldkey;

        $repo = $this->repository_model->get_all_repository($username);

        foreach ($repo as $obj) {
            $info = array('userId' => $obj['userId'], 'username' => $obj['username'], 'repoPath' => $obj['repoPath'], 'password' => $obj['password']);
            $password = $this->repository_model->decrypt_password($info, $key);
            $newInfo = $info;
            $newInfo['password'] = $password;

            $ret = $this->repository_model->update_repository($info, $newInfo); // update the password with re-encryption by new hash
        }
    }

    //forgot password
    function forgot_password() {
        $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
        if ($this->form_validation->run() == false) {
            //setup the input
            $this->data['email'] = array('name' => 'email',
                'id' => 'email',
            );
            //set any errors and display the form
            $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
            $this->load->view('auth/forgot_password', $this->data);
        } else {
            //run the forgotten password method to email an activation code to the user
            $forgotten = $this->ion_auth->forgotten_password($this->input->post('email'));

            if ($forgotten) { //if there were no errors
                $this->session->set_flashdata('message', $this->ion_auth->messages());
                redirect("auth/login", 'refresh'); //we should display a confirmation page here instead of the login page
            } else {
                $this->session->set_flashdata('message', $this->ion_auth->errors());
                redirect("auth/forgot_password", 'refresh');
            }
        }
    }

    //reset password - final step for forgotten password
    public function reset_password($code) {
        $reset = $this->ion_auth->forgotten_password_complete($code);
        if ($reset) {  //if the reset worked then send them to the login page
            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("auth/login", 'refresh');
        } else { //if the reset didnt work then send them back to the forgot password page
            $this->session->set_flashdata('message', $this->ion_auth->errors());
            redirect("auth/forgot_password", 'refresh');
        }
    }

    //activate the user
    function activate($id, $code=false) {
        $activation = $this->ion_auth->activate($id, $code);

        if ($activation) {
            if (is_ajax ()) {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->data['is_admin'] = $this->ion_auth->is_admin();
                $this->load->view('auth/user_list', $this->data);
            } else {
                $this->session->set_flashdata('message', $this->ion_auth->messages());
                redirect('auth/index', 'refresh');
            }
        } else {
            //redirect them to the forgot password page
            $this->session->set_flashdata('message', $this->ion_auth->errors());
            redirect("auth/forgot_password", 'refresh');
        }
    }

    //deactivate the user
    function deactivate($id = NULL) {


        $this->load->library('form_validation');
        $this->form_validation->set_rules('confirm', 'confirmation', 'required');
        $this->form_validation->set_rules('id', 'user ID', 'required');

        if ($this->form_validation->run() == FALSE) {

            // insert csrf check
            $this->data['csrf'] = $this->_get_csrf_nonce();
            $this->data['user'] = $this->ion_auth->get_user($id);
            $this->load->view('auth/deactivate_user', $this->data);
        } else {
            // do we really want to deactivate?
            if ($this->input->post('confirm') == 'yes') {
                // do we have a valid request?
                if ($this->_valid_csrf_nonce() === FALSE || $id != $this->input->post('id')) {
                    show_404();
                }

                // do we have the right userlevel?
                if ($this->ion_auth->logged_in() && $this->ion_auth->is_admin()) {
                    $this->ion_auth->deactivate($id);
                }
            }

            //redirect them back to the auth page
            if (is_ajax ()) {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->data['is_admin'] = $this->ion_auth->is_admin();
                $this->load->view('auth/user_list', $this->data);
            } else {
                $this->session->set_flashdata('message', $this->ion_auth->messages());
                redirect('auth/index', 'refresh');
            }
        }
    }

    //create a new user
    function create_user() {
        $this->data['title'] = "Create User";

        if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin()) {
            redirect('auth', 'refresh');
        }

        //validate form input
        $this->form_validation->set_rules('user_name', 'User Name', 'required|xss_clean|unique[users.username]');
        $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email|unique[users.email]');
        $this->form_validation->set_rules('password', 'Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[password_confirm]');
        $this->form_validation->set_rules('password_confirm', 'Password Confirmation', 'required');
        $this->form_validation->set_rules('role[]', 'role', 'required|xss_clean');

        if ($this->form_validation->run() == true) {
            $username = strtolower($this->input->post('user_name'));
            $email = $this->input->post('email');
            $password = $this->input->post('password');
            $role = $this->input->post('role');
            /* $additional_data = array('first_name' => $this->input->post('first_name'),
              'last_name' => $this->input->post('last_name'),
              'company' => $this->input->post('company'),
              'phone' => $this->input->post('phone1') . '-' . $this->input->post('phone2') . '-' . $this->input->post('phone3'),
              'role_id'=>$this->input->post('role')
              ); */
        }
        if ($this->form_validation->run() == true && $this->ion_auth->register($username, $password, $email, $role)) { //check to see if we are creating the user
            //redirect them back to the admin page
            if (is_ajax ()) {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->data['userrole'] = $this->session->userdata('role');
                $this->data['is_admin'] = $this->ion_auth->is_admin();
                $this->load->view('auth/user_list', $this->data);
            } else {
                $this->session->set_flashdata('message', "User Created");
                redirect("auth", 'refresh');
            }
        } else { //display the create user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));

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

            /* changing the data obtained from database into
             * key value pair used in select element
             */

            /* $options =array();
              $roles=$this->ion_auth->get_roles();
              foreach ($roles as $role)
              {
              $options[$role['id']]=$role['name'];
              } */
            $roles = $this->ion_auth->get_roles($this->session->userdata('username'));
            foreach ($roles as $role) {
                $this->data['roles'][$role['name']] = array('name' => 'role[]',
                    'id' => $role['name'],
                    'value' => $role['name'],
                    'checked' => $this->form_validation->set_checkbox('role[]', $role['name'])
                );
            }
            //$this->data['role']=array('name'=>'role','options'=>$options,'default'=>set_value('role', '2'));
            $this->load->view('auth/create_user', $this->data);
        }
    }

    function _get_csrf_nonce() {
        $this->load->helper('string');
        $key = random_string('alnum', 8);
        $value = random_string('alnum', 20);
        $this->session->set_flashdata('csrfkey', $key);
        $this->session->set_flashdata('csrfvalue', $value);

        return array($key => $value);
    }

    function _valid_csrf_nonce() {
        if ($this->input->post($this->session->flashdata('csrfkey')) !== FALSE &&
                $this->input->post($this->session->flashdata('csrfkey')) == $this->session->flashdata('csrfvalue')) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    function edit_user($id) {

        $this->data['title'] = "Edit User";
        if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin()) {
            redirect('auth', 'refresh');
        }
        //validate form input
        $this->form_validation->set_rules('user_name', 'First Name',    'required|xss_clean');
        $this->form_validation->set_rules('email',     'Email Address', 'required|valid_email');
        $this->form_validation->set_rules('roles[]',   'Role',          'required|xss_clean');

        
        if ($this->form_validation->run() == true) {
            $data = array(
                'username' => $this->input->post('user_name'),
                'email'    => $this->input->post('email'),
                'roles'    => $this->input->post('roles'),
            );
        }
        if ($this->form_validation->run() == true && $this->ion_auth->update_user($id, $data)) { //check to see if we are creating the user
            //redirect them back to the admin page
              if ($this->input->post('reset_password')) {
                $forgotten = $this->ion_auth->forgotten_password($this->input->post('email'));
              }
            if (is_ajax ()) {
                $this->data['message']   = $this->ion_auth->messages();
                $this->data['users']     = $this->ion_auth->get_users_array();
                $this->data['userroles'] = $this->session->userdata('roles');
                $this->data['is_admin']  = $this->ion_auth->is_admin();
                $this->load->view('auth/user_list', $this->data);
            } else {
                $this->session->set_flashdata('message', "User Updated");
                redirect("auth", 'refresh');
            }
        } else { //display the create user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
            $user = $this->ion_auth->get_user($id);

            $this->data['user_name'] = array('name' => 'user_name',
                'id'    => 'first_name',
                'type'  => 'text',
                'value' => $this->form_validation->set_value('user_name', $user->username),
            );

            $this->data['email'] = array('name' => 'email',
                'id'    => 'email',
                'type'  => 'text',
                'value' => $this->form_validation->set_value('email', $user->email),
            );

            $roles = $this->ion_auth->get_roles($this->session->userdata('username'));

            foreach ($roles as $role) {
                $this->data['roles'][$role['name']] = array('name' => 'roles[]',
                    'id'      => $role['name'],
                    'value'   => $role['name'],
                    'checked' => $this->form_validation->set_checkbox('roles[]', $role['name'], (in_array($role['name'], $user->roles)) ? TRUE : FALSE)
                );
            }

            $this->load->view('auth/edit_user', $this->data);
        }
    }
    
     function edit_user_ldap($username) {
        $this->data['title'] = "Edit User";
       
        //validate form input
        $this->form_validation->set_rules('user_name', 'First Name', 'required|xss_clean');
        $this->form_validation->set_rules('role[]', 'role', 'required|xss_clean');

        /*if ($this->form_validation->run() == true) {
            $data = array(
                'username' => $this->input->post('user_name'),
                'roles' => $this->input->post('role'),
            );
        }*/
        if ($this->form_validation->run() == true && $this->ion_auth->update_ldap_users($username, $this->input->post('role'))) { //check to see if we are creating the user
            //redirect them back to the admin page
            $user = $this->ion_auth->get_ldap_user_details_from_local_db($username);
            $user_roles=array();
            if($user !==NULL){
                $user_roles=$user->roles;
            }
            $this->session->set_userdata('role',$user_roles);
            if (is_ajax ()) {
                $this->data['message'] = $this->ion_auth->messages();
                $this->data['users'] = $this->ion_auth->get_users_array();
                $this->data['userrole'] = $this->session->userdata('role');
                $this->data['is_admin'] = $this->ion_auth->is_admin();
                $this->load->view('auth/user_list', $this->data);
            } else {
               
                $this->session->set_flashdata('message', "User Updated");
                redirect("auth", 'refresh');
            }
        } else { //display the create user form
            //set the flash data error message if there is one
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
            
            //$phone=explode('-',$user->phone);
            $this->data['user_name'] = array('name' => 'user_name',
                'id' => 'first_name',
                'type' => 'text',
                'value' => $this->form_validation->set_value('user_name', $username),
            );
            $roles = $this->ion_auth->get_roles($this->session->userdata('username'));
          
            $user = $this->ion_auth->get_ldap_user_details_from_local_db($username);
            $user_roles=array();
            if($user !==NULL){
                $user_roles=$user->roles;
            }
            foreach ($roles as $role) {
                $this->data['roles'][$role['name']] = array('name' => 'role[]',
                    'id' => $role['name'],
                    'value' => $role['name'],
                    'checked' => $this->form_validation->set_checkbox('role[]', $role['name'], (in_array($role['name'], $user_roles)) ? TRUE : FALSE)
                );
            }

            //$this->data['role']=array('name'=>'role','options'=>$options,'default'=>set_value('role', $user->role_id));
            $this->load->view('auth/edit_user_ldap', $this->data);
        }
    }

    function manage_role($op=false, $rolename=false) {
        
        if (!$this->ion_auth->logged_in()) {
            redirect('auth', 'refresh');
        }

        if ($this->ion_auth->is_admin() === false) {
            $this->permission_deny($this->lang->line('no_permission'));
        }

                     
       $this->data['is_admin'] = $this->ion_auth->is_admin();
        
        if (!empty($op)) {
            $this->data['title'] = "Create role";
            $this->data['operation'] = "Create";
            
            if ($op != 'edit')
                $this->form_validation->set_rules('name', 'Name', 'required|xss_clean|unique[roles.name]');
            else
                $this->form_validation->set_rules('name', 'Name', 'required|xss_clean');
            
            $this->form_validation->set_rules('description',     'Description',      'required|xss_clean|trim');
            $this->form_validation->set_rules('crxi', 'Include classes',  'xss_clean|trim');
            $this->form_validation->set_rules('crxx', 'Exclude classes',  'xss_clean|trim');
            $this->form_validation->set_rules('brxi', 'Include bundlers', 'xss_clean|trim'); 
            $this->form_validation->set_rules('brxx', 'Include bundlers', 'xss_clean|trim');             

            
            if ($this->form_validation->run() == true) {
                $data = array('name'            => $this->input->post('name'),
                              'description'     => $this->input->post('description'),
                              'crxi' => $this->input->post('crxi'),
                              'crxx' => $this->input->post('crxx'),
                              'brxi' => $this->input->post('brxi'),
                              'brxx' => $this->input->post('brxx')                    
                );
      
                if (($op == 'edit' && !$this->ion_auth->update_role($this->session->userdata('username'), $data))) {
                    $this->__load_role_add_edit($op, $rolename);
                    return;
                }
                
                if ($op == 'create' && $this->ion_auth->create_role($this->session->userdata('username'), $data) === FALSE) {
                    $this->__load_role_add_edit($op, $rolename);
                    return;
                }

                if (is_ajax ()) {
                    $this->data['message'] = $this->ion_auth->messages();
                    $this->data['roles'] = $this->ion_auth->get_roles($this->session->userdata('username'));
                    
                    $this->load->view('auth/list_role', $this->data);
                } else {
                    $this->session->set_flashdata('message', $this->ion_auth->messages());
                    redirect("auth/manage_role", 'refresh');
                }
            } else {
                $this->__load_role_add_edit($op, $rolename);
            }
        } else {
            $this->data['roles'] = $this->ion_auth->get_roles($this->session->userdata('username'));

            $this->data['message'] = (validation_errors()) ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message'));
            $this->load->view('auth/list_role', $this->data);
        }
    }

    function __load_role_add_edit($op, $rolename) {
        $this->load->helper('roles_checkbox_list');

        $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
        $this->data['name'] = array('name' => 'name',
            'id'    => 'name',
            'type'  => 'text',
            'value' => $this->form_validation->set_value('name'),
        );
        $this->data['description'] = array('name' => 'description',
            'id'    => 'last_name',
            'type'  => 'text',
            'rows'  => '3',
            'cols'  => '20',
            'value' => $this->form_validation->set_value('description'),
        );
        $this->data['crxi'] = array('name' => "crxi",
            'id'    => 'crxi',
            'type'  => 'text',
            'value' => $this->form_validation->set_value('crxi'),
        );
        
        $this->data['crxx'] = array('name' => "crxx",
            'id'    => 'crxx',
            'type'  => 'text',
            'value' => $this->form_validation->set_value('crxx'),
        );        
        
        $this->data['brxi'] = array('name' => "brxi",
            'id'    => 'brxi',
            'type'  => 'text',
            'value' => $this->form_validation->set_value('brxi'),
        );  
        
        $this->data['brxx'] = array('name' => "brxx",
            'id'    => 'brxx',
            'type'  => 'text',
            'value' => $this->form_validation->set_value('brxx'),
        );  
        
        if ($op == 'edit')
        {   
            if (empty($rolename)) {
                show_error('cannot update - record not specified', 500);
                return;
            }
    
            $role = $this->ion_auth->get_role($this->session->userdata('username'), $rolename);

            $this->data['title']          = "Edit role";
            $this->data['operation']      = "Edit";
            $this->data['name']['enable'] = 'enable';
            
            $this->data['name']['value']        = $this->form_validation->set_value('name',        $role['name']); 
            $this->data['description']['value'] = $this->form_validation->set_value('description', array_key_exists('description', $role) ? $role['description'] : "");
            
            //classes
            $this->data['crxi']['value'] = $this->form_validation->set_value('crxi', array_key_exists('classrxinclude',  $role) ? $role['classrxinclude'] : "");
            $this->data['crxx']['value'] = $this->form_validation->set_value('crxx', array_key_exists('classrxexclude',  $role) ? $role['classrxexclude'] : "");
            
            // bundles
            $this->data['brxi']['value'] = $this->form_validation->set_value('brxi', array_key_exists('bundlerxinlcude', $role) ? $role['bundlerxinlcude'] : "");
            $this->data['brxx']['value'] = $this->form_validation->set_value('brxx', array_key_exists('bundlerxexclude', $role) ? $role['bundlerxexclude'] : "");
        }
        
        
        $this->data['op'] = $op;
                    
        $this->load->view('auth/add_edit_role', $this->data);
    }

    function delete_user($id) {
        $this->ion_auth->delete_user($id);
        if (is_ajax ()) {
            $this->data['message'] = $this->ion_auth->errors()?$this->ion_auth->errors():$this->ion_auth->messages();
            $this->data['users'] = $this->ion_auth->get_users_array();
            $this->data['userrole'] = $this->session->userdata('role');
            $this->data['is_admin'] = $this->ion_auth->is_admin();
            $this->load->view('auth/user_list', $this->data);
        } else {
            
            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("auth", 'refresh');
        }
    }

    /**
     * Delete role
     * 
     * @param type $name  - role name
     */
    
    function delete_role($name) {
        
        if($name == '')
        {
            $this->permission_deny($this->lang->line('empty_rolename'));
        } 
        
        if($name == 'admin')
        {
            $this->permission_deny($this->lang->line('admin_role_delete_forbidden'));
        }
        
        if ($this->ion_auth->is_admin() === false) {
            $this->permission_deny($this->lang->line('no_permission'));
        }
        

        $this->ion_auth->delete_role($this->session->userdata('username'), $name); 
                
        if (is_ajax ()) {
            $this->data['message']  = $this->ion_auth->errors()?$this->ion_auth->errors():$this->ion_auth->messages();
            $this->data['roles']    = $this->ion_auth->get_roles($this->session->userdata('username'));     
            $this->data['is_admin'] = $this->ion_auth->is_admin();
            $this->load->view('auth/list_role', $this->data);
        } else {
            $this->session->set_flashdata('message', $this->ion_auth->messages());
            redirect("auth/manage_role", 'refresh');
        }
    }

    function setting() {
        if (!$this->ion_auth->logged_in()) {
            //redirect them to the login page
            redirect('auth/login', 'refresh');
        }
        $bc = array(
            'title' => $this->lang->line('breadcrumb_admin'),
            'url' => 'auth/setting',
            'isRoot' => false,
            'replace_existing'=>true
        );
        
        $data = array(
            'title' => $this->lang->line('mission_portal_title')." - Settings",
            'breadcrumbs' => $this->breadcrumblist->display(),
            "is_displayable"=>$this->ion_auth->is_accessable()
        );
        $this->template->load('template', 'auth/usersettings', $data);
    }
          
        /*
        * if ajax - show message on top of the page div id=error_status
        * 
        * if not ajax - redirect to no_permission page, with some message
        */
        function permission_deny($message = '') {
            if ($message == '') {
                $message = $this->lang->line('no_permission');
            }
            
            if (is_ajax()) {
                $this->output->set_status_header(401, $message);
                echo $message;
                exit();
            }
            else {
                $bc = array(
                    'title' => $this->lang->line('mission_portal_title'),
                    'url' => 'welcome/index',
                    'isRoot' => true
                );

                $this->breadcrumb->setBreadCrumb($bc);

                $this->data['title']       = $this->lang->line('mission_portal_title');
                $this->data['message']     = $this->lang->line('no_permission');
                $this->data['breadcrumbs'] = $this->breadcrumblist->display();
                $this->session->set_flashdata('message', $this->lang->line('no_permission'));

                $this->template->load('template', 'auth/no_permission', $this->data);
            }
      }
}   