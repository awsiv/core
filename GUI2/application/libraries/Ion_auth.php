<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');
/**
* Name:  Ion Auth
*
* Author: Ben Edmunds
*		  ben.edmunds@gmail.com
*         @benedmunds
*
* Added Awesomeness: Phil Sturgeon
*
* Location: http://github.com/benedmunds/CodeIgniter-Ion-Auth
*
* Created:  10.01.2009
*
* Description:  Modified auth system based on redux_auth with extensive customization.  This is basically what Redux Auth 2 should be.
* Original Author name has been kept but that does not mean that the method has not been modified.
*
* Requirements: PHP5 or above
*
*/

class Ion_auth
{
	/**
	 * CodeIgniter global
	 *
	 * @var string
	 **/
	protected $ci;

	/**
	 * account status ('not_activated', etc ...)
	 *
	 * @var string
	 **/
	protected $status;

	/**
	 * message (uses lang file)
	 *
	 * @var string
	 **/
	protected $messages;

	/**
	 * error message (uses lang file)
	 *
	 * @var string
	 **/
	protected $errors = array();

	/**
	 * error start delimiter
	 *
	 * @var string
	 **/
	protected $error_start_delimiter;

	/**
	 * error end delimiter
	 *
	 * @var string
	 **/
	protected $error_end_delimiter;

	/**
	 * extra where
	 *
	 * @var array
	 **/
	public $_extra_where = array();

	/**
	 * extra set
	 *
	 * @var array
	 **/
	public $_extra_set = array();


        public $mode='database';
	/**
	 * __construct
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function __construct()
	{
                $this->ci = & get_instance();
                $this->ci->load->config('ion_auth', TRUE);
                //$this->ci->load->config('appsettings', TRUE);
                $this->ci->load->library('email');
                $this->ci->load->library('session');
               // $this->ci->load->library('Auth_Ldap');
                $this->ci->load->library('encrypt');
                $this->ci->lang->load('ion_auth');
                $this->ci->load->model('ion_auth_model_mongo');
                $this->ci->load->model('settings_model');
                $this->ci->load->model(array('astrolabe_model','authentication_model'));
                $this->ci->load->helper('cookie');
                
               

                $this->messages = array();
                $this->errors = array();
                $this->infos = array();
		$this->message_start_delimiter = $this->ci->config->item('message_start_delimiter', 'ion_auth');
		$this->message_end_delimiter   = $this->ci->config->item('message_end_delimiter', 'ion_auth');
		$this->error_start_delimiter   = $this->ci->config->item('error_start_delimiter', 'ion_auth');
		$this->error_end_delimiter     = $this->ci->config->item('error_end_delimiter', 'ion_auth');
                $this->info_start_delimiter =$this->ci->config->item('info_start_delimiter', 'ion_auth');
                $this->info_end_delimiter =$this->ci->config->item('info_end_delimiter', 'ion_auth');
                //load the mode of authentication
                $this->mode=$this->ci->settings_model->app_settings_get_item('mode');

                
                $this->auth_model = $this->ci->authentication_model;
                $this->auth_model->setRestClient($this->getRestClient());
                

		//auto-login the user if they are remembered
		if (!$this->logged_in() && get_cookie('identity') && get_cookie('remember_code'))
		{
			if ($this->ci->ion_auth_model_mongo->login_remembered_user() == TRUE)
                        {
                            $username = $this->ci->session->userdata('username');
                            $this->on_login_successful($username);
                        }
		}
                
                $this->email=$this->ci->settings_model->app_settings_get_item('appemail');
                if(!($this->email || empty($this->email))){
                    $this->email=$this->ci->config->item('admin_email', 'ion_auth');
                }
	}

	/**
	 * __call
	 *
	 * Acts as a simple way to call model methods without loads of stupid alias'
	 *
	 **/
	public function __call($method, $arguments)
	{
		if (!method_exists( $this->ci->ion_auth_model_mongo, $method) )
		{
			throw new Exception('Undefined method Ion_auth::' . $method . '() called');
		}

		return call_user_func_array( array($this->ci->ion_auth_model_mongo, $method), $arguments);
	}

	/**
	 * Activate user.
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function activate($id, $code=false)
	{
		if ($this->ci->login->activate($id, $code))
		{
			$this->set_message('activate_successful');
			return TRUE;
		}

		$this->set_error('activate_unsuccessful');
		return FALSE;
	}
        
        /**
         * Get Rest Client
         */
        
          function getRestClient(){
  
            $apiServer = 'http://10.0.0.74/api';
            $http_auth ='basic';
            $config = array('server' => $apiServer);
            $this->ci->load->spark('restclient/2.1.0');
            $this->ci->load->library('cf_rest');

            if ($this->logged_in())
            {
                $username=$this->ci->session->userdata('username');
                $password=$this->ci->session->userdata('password');
                $config = array('server' => $apiServer, 'http_auth' => $http_auth, 'http_user' => $username, 'http_pass' => $password);
            }
            $this->ci->cf_rest->initialize($config);
            return $this->ci->cf_rest;
         }
        
	/**
	 * Deactivate user.
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function deactivate($id)
	{
		if ($this->ci->ion_auth_model_mongo->deactivate($id))
		{
			$this->set_message('deactivate_successful');
			return TRUE;
		}

		$this->set_error('deactivate_unsuccessful');
		return FALSE;
	}

	/**
	 * Change password.
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function change_password($identity, $old, $new)
	{
		if ($this->ci->ion_auth_model_mongo->change_password($identity, $old, $new))
		{
			$this->set_message('password_change_successful');
			return TRUE;
		}

		$this->set_error('password_change_unsuccessful');
		return FALSE;
	}

	/**
	 * forgotten password feature
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function forgotten_password($email)
	{
		if ( $this->ci->ion_auth_model_mongo->forgotten_password($email) )
		{
			// Get user information
			$user = $this->get_user_by_email($email);

			$data = array(
				'identity'		=> $user->{$this->ci->config->item('identity', 'ion_auth')},
				'forgotten_password_code' => $user->forgotten_password_code
			);

			$message = $this->ci->load->view($this->ci->config->item('email_templates', 'ion_auth').$this->ci->config->item('email_forgot_password', 'ion_auth'), $data, true);
			$this->ci->email->clear();
			$config['mailtype'] = $this->ci->config->item('email_type', 'ion_auth');
			$this->ci->email->initialize($config);
			$this->ci->email->set_newline("\r\n");
			$this->ci->email->from($this->email, $this->ci->config->item('site_title', 'ion_auth'));
			$this->ci->email->to($user->email);
			$this->ci->email->subject($this->ci->config->item('site_title', 'ion_auth') . ' - Forgotten Password Verification');
			$this->ci->email->message($message);

			if ($this->ci->email->send())
			{
				$this->set_message('forgot_password_successful');
				return TRUE;
			}
			else
			{
				$this->set_error('forgot_password_unsuccessful');
				return FALSE;
			}
		}
		else
		{
			$this->set_error('forgot_password_unsuccessful');
			return FALSE;
		}
	}

	/**
	 * forgotten_password_complete
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function forgotten_password_complete($code)
	{
		$identity = $this->ci->config->item('identity', 'ion_auth');
		$profile  = $this->ci->ion_auth_model_mongo->profile($code, true); //pass the code to profile

		if (!is_object($profile))
		{
			$this->set_error('password_change_unsuccessful');
			return FALSE;
		}

		//$new_password = $this->ci->ion_auth_model_mongo->forgotten_password_complete($code, $profile->salt);
                 $new_password = $this->ci->ion_auth_model_mongo->forgotten_password_complete($code);

		if ($new_password)
		{
			$data = array(
				'identity'     => $profile->{$identity},
				'new_password' => $new_password
			);

			$message = $this->ci->load->view($this->ci->config->item('email_templates', 'ion_auth').$this->ci->config->item('email_forgot_password_complete', 'ion_auth'), $data, true);

			$this->ci->email->clear();
			$config['mailtype'] = $this->ci->config->item('email_type', 'ion_auth');
			$this->ci->email->initialize($config);
			$this->ci->email->set_newline("\r\n");
			$this->ci->email->from($this->email, $this->ci->config->item('site_title', 'ion_auth'));
			$this->ci->email->to($profile->email);
			$this->ci->email->subject($this->ci->config->item('site_title', 'ion_auth') . ' - New Password');
			$this->ci->email->message($message);

			if ($this->ci->email->send())
			{
				$this->set_message('forgot_password_successful');
				return TRUE;
			}
			else
			{
				$this->set_error('forgot_password_unsuccessful');
				return FALSE;
			}
		}

		$this->set_error('password_change_unsuccessful');
		return FALSE;
	}

	/**
	 * register
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function register($username, $password, $email, $role, $additional_data = false) //need to test email activation
	{
		$email_activation = $this->ci->config->item('email_activation', 'ion_auth');

		if (!$email_activation)
		{
			$id = $this->ci->ion_auth_model_mongo->register($username, $password, $email, $role, $additional_data);
			if ($id !== FALSE)
			{
				$this->set_message('account_creation_successful');
				return $id;
			}
			else
			{
				$this->set_error('account_creation_unsuccessful');
				return FALSE;
			}
		}
		else
		{
			$id = $this->ci->ion_auth_model_mongo->register($username, $password, $email, $role, $additional_data);

			if (!$id)
			{
				$this->set_error('account_creation_unsuccessful');
				return FALSE;
			}

			$deactivate = $this->ci->ion_auth_model_mongo->deactivate($id);

			if (!$deactivate)
			{
				$this->set_error('deactivate_unsuccessful');
				return FALSE;
			}

			$activation_code = $this->ci->ion_auth_model_mongo->activation_code;
			$identity        = $this->ci->config->item('identity', 'ion_auth');
			$user            = $this->ci->ion_auth_model_mongo->get_user($id);

			$data = array(
				'identity'   => $user->{$identity},
				'id'         => $user->id,
				'email'      => $email,
				'activation' => $activation_code,
			);

			$message = $this->ci->load->view($this->ci->config->item('email_templates', 'ion_auth').$this->ci->config->item('email_activate', 'ion_auth'), $data, true);

			$this->ci->email->clear();
			$config['mailtype'] = $this->ci->config->item('email_type', 'ion_auth');
			$this->ci->email->initialize($config);
			$this->ci->email->set_newline("\r\n");
			$this->ci->email->from($this->email, $this->ci->config->item('site_title', 'ion_auth'));
			$this->ci->email->to($email);
			$this->ci->email->subject($this->ci->config->item('site_title', 'ion_auth') . ' - Account Activation');
			$this->ci->email->message($message);

			if ($this->ci->email->send() == TRUE)
			{
				$this->set_message('activation_email_successful');
				return $id;
			}

			$this->set_error('activation_email_unsuccessful');
			return FALSE;
		}
	}

   /**
    *
    * @param type $identity
    * @param type $password
    * @param type $remember
    * @return type 
    */
	public function login($identity, $password, $remember=false)
	{
             $val=$this->auth_model->login($identity, $password);
             if($val){
                 return true;
             }
             return false;
	}

        protected function on_login_successful($username)
        {
            if ($this->mode != 'database')
            {
                if ($this->ci->ion_auth_model_mongo->is_first_login_ldap())
                {
                    $this->ci->astrolabe_model->add_builtin_profiles($username);
                }

                $this->ci->ion_auth_model_mongo->update_last_login_ldap($username);
            }
            else
            {
                if ($this->ci->ion_auth_model_mongo->is_first_login())
                {
                    $this->ci->astrolabe_model->add_builtin_profiles($username);
                }

                $this->ci->ion_auth_model_mongo->update_last_login($username);
            }
        }

	/**
	 * logout
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function logout()
	{
		$identity = $this->ci->config->item('identity', 'ion_auth');
		$this->ci->session->unset_userdata($identity);
		$this->ci->session->unset_userdata('role');
		$this->ci->session->unset_userdata('id');
		$this->ci->session->unset_userdata('user_id');

		//delete the remember me cookies if they exist
		if (get_cookie('identity'))
		{
			delete_cookie('identity');
		}
		if (get_cookie('remember_code'))
		{
			delete_cookie('remember_code');
		}

		$this->ci->session->sess_destroy();

		$this->set_message('logout_successful');
		return TRUE;
	}

	/**
	 * logged_in
	 *
	 * @return bool
	 * @author Mathew
	 **/
	public function logged_in()
	{
		$identity = $this->ci->config->item('identity', 'ion_auth');

		return (bool) $this->ci->session->userdata($identity);
	}

	/**
	 * is_admin
	 *
	 * @return bool
	 * @author Ben Edmunds
	 **/

        public function is_admin($check_real_assigned_roles = false)
	{
                $admin_role=$this->ci->settings_model->app_settings_get_item('admin_role');

                if($admin_role===False){
                    $admin_role = $this->ci->config->item('admin_role', 'ion_auth');
                }

                if ($check_real_assigned_roles == false) {
                    $user_role = $this->ci->session->userdata('roles');
                }
                else
                {
                    if ($this->ci->settings_model->app_settings_get_item('mode') == 'database' || $this->ci->session->userdata('mode') == 'database')
                    {
                        $tmp = $this->get_user_role($this->ci->session->userdata('id'));
                        $user_role = $tmp[0]['roles'];
                    }
                    else
                    {
                        $tmp = $this->get_ldap_user_details_from_local_db($this->ci->session->userdata('user_id'));
                        $user_role = $tmp->roles;
                    }
                }

                if($user_role === False || empty($user_role)){
                    return false;
                }

                return in_array($admin_role, $user_role);
	}



        /**
	 * is_in_fallback_role
	 *
	 * @return bool
	 * @author sudhir
	 **/
        public function is_in_fallback_role()
	{
                 $admin_role=$this->ci->settings_model->app_settings_get_item('fall_back_for');

                 if($admin_role===False){
                     $admin_role = $this->ci->config->item('admin_role', 'ion_auth');
                 }
                 $user_role  = $this->ci->session->userdata('role');
                     if($user_role===False){
                        return false;
                    }
                return in_array($admin_role, $user_role);
	}

        /**
         *
         */

        public function is_accessable()
        {
           $isadmin=$this->ci->settings_model->app_settings_get_item('admin_role');
            if($isadmin!==FALSE && (!$this->is_admin() && !$this->is_in_fallback_role())){
                return false;
            }
            return true;
        }
	/**
	 * is_role
	 *
	 * @return bool
	 * @author Phil Sturgeon
	 **/
	public function is_role($check_role)
	{
		$user_role = $this->ci->session->userdata('role');

		if(is_array($check_role))
		{
			return in_array($user_role, $check_role);
		}

		return $user_role == $check_role;
	}

	/**
	 * Profile
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function profile()
	{
		$session  = $this->ci->config->item('identity', 'ion_auth');
		$identity = $this->ci->session->userdata($session);

		return $this->ci->ion_auth_model_mongo->profile($identity);
	}

	/**
	 * Get Users
	 *
	 * @return objects Users
	 * @author Ben Edmunds
	 **/
	public function get_users($role_name=false, $limit=NULL, $offset=NULL)
	{
		return $this->ci->ion_auth_model_mongo->get_users($role_name, $limit, $offset);
	}


        public function get_user_role($id)
        {
            return $this->ci->ion_auth_model_mongo->get_users_role($id);
        }

        public function get_user_rolelist($id){
            $roles= $this->ci->ion_auth_model_mongo->get_users_role($id);
            if(!empty ($roles)&&$roles!==False){
                return $roles[0]['roles'];
            }
            return array();
        }

	/**
	 * Get Users Array
	 *
	 * @return array Users
	 * @author Ben Edmunds
	 **/
       public function get_users_array($role_name=false, $limit=NULL, $offset=NULL) {
        if (strtolower($this->mode) != 'database') {
            if (!$this->ci->session->userdata('pwd')) {
                $this->set_error('login_mode_changed');
            }
            if($this->ci->session->userdata('dn')){
                $this->ci->auth_ldap->set_user_dn($this->ci->session->userdata('dn'));
            }

            $users=$this->ci->auth_ldap->get_all_ldap_users($this->ci->session->userdata('username'), $this->ci->session->userdata('pwd'));
            $users_collection=array();

            foreach ($users as $user) {
                $user_with_roles = $user;

                $user_details_from_db = $this->get_ldap_user_details_from_local_db($user['name']);

                if (is_object($user_details_from_db)) {

                    $user_with_roles['roles'] = $this->ci->auth_ldap->get_role_for_user_from_localdb($user['name']);
                }
                array_push($users_collection, $user_with_roles);
            }
            //var_dump($users_collection);
            return $users_collection;
        }
        return $this->ci->ion_auth_model_mongo->get_users_by_role($role_name, $limit, $offset);
    }

	/**
	 * Get Newest Users
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_newest_users($limit = 10)
	{
		return (object)$this->ci->ion_auth_model_mongo->get_newest_users($limit);
	}

	/**
	 * Get Newest Users Array
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_newest_users_array($limit = 10)
	{
		return $this->ci->ion_auth_model_mongo->get_newest_users($limit);
	}

	/**
	 * Get Active Users
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_active_users($role_name = false)
	{
		return (object)$this->ci->ion_auth_model_mongo->get_active_users($role_name);
	}

	/**
	 * Get Active Users Array
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_active_users_array($role_name = false)
	{
		return $this->ci->ion_auth_model_mongo->get_active_users($role_name);
	}

	/**
	 * Get In-Active Users
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_inactive_users($role_name = false)
	{
		return (object)$this->ci->ion_auth_model_mongo->get_inactive_users($role_name);
	}

	/**
	 * Get In-Active Users Array
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_inactive_users_array($role_name = false)
	{
		return $this->ci->ion_auth_model_mongo->get_inactive_users($role_name);
	}

	/**
	 * Get User
	 *
	 * @return object User
	 * @author Ben Edmunds
	 **/
	public function get_user($id=false)
	{
		return $this->ci->ion_auth_model_mongo->get_user($id);
	}

	/**
	 * Get User by Email
	 *
	 * @return object User
	 * @author Ben Edmunds
	 **/
	public function get_user_by_email($email)
	{
		return $this->ci->ion_auth_model_mongo->get_user_by_email($email);
	}

	/**
	 * Get User as Array
	 *
	 * @return array User
	 * @author Ben Edmunds
	 **/
	public function get_user_array($id=false)
	{
		return $this->ci->ion_auth_model_mongo->get_user($id);
	}


	/**
	 * update_user
	 *
	 * @return void
	 * @author Phil Sturgeon
	 **/
	public function update_user($id, $data)
	{
	$roles=$this->get_user_rolelist($id);
                $admin_role=$this->ci->settings_model->app_settings_get_item('admin_role');
                $count=$this->ci->ion_auth_model_mongo->count_users_in_role($admin_role);
                if($count <= 1 && in_array($admin_role,$roles)&& !in_array($admin_role,$data['roles'])){
                    $this->set_error('one_admin_required');
                    return FALSE;
                }
                if ($this->ci->ion_auth_model_mongo->update_user($id, $data))
		{
			$this->set_message('update_successful');
			return TRUE;
		}

		$this->set_error('update_unsuccessful');
		return FALSE;
	}

       public function is_ldap_user_exists(){

           $number=$this->ci->ion_auth_model_mongo->total_ldap_users_cached();
             if($number>0){
                 return true;
             }
             return false;
       }

       /**
        *
        * @param type $username
        * @return type get the details of cached ldap user
        */
        public function get_ldap_user_details_from_local_db($username){
            return $this->ci->ion_auth_model_mongo->get_ldap_user_by_col("username",$username);
        }


        /**
         *
         * @param type $username
         * @return type for checking the is there a previous entry present for this user
         */
        public function ldap_user_role_entry_exits($username)
        {
           $user=$this->ci->ion_auth_model_mongo->get_ldap_user_by_col("username",$username);
           if(is_null($user)){
             return FALSE;
           }
           return TRUE;
        }

        /**
         *
         * @param type $username
         * @param type $data which contains all the things as well as roles
         * @return type
         */
        public function update_ldap_users($username,$data){
            $user_entry=array("username"=>$username,"roles"=>$data);
            if($this->ldap_user_role_entry_exits($username)){
               $this->ci->ion_auth_model_mongo->update_ldap_user($username,$user_entry);
               $this->set_message('update_successful');
               return true;
              }else{
               $id=$this->ci->ion_auth_model_mongo->cache_ldap_user($user_entry);
               $this->set_message('update_successful');
               if($id !== FALSE){
                   return true;
               }
               $this->set_error('update_unsuccessful');
               return false;
             }
        }


	/**
	 * delete_user
	 *
	 * @return void
	 * @author Phil Sturgeon
	 **/
	public function delete_user($id)
	{
	$roles=$this->get_user_rolelist($id);
                $admin_role=$this->ci->settings_model->app_settings_get_item('admin_role');
                $count=$this->ci->ion_auth_model_mongo->count_users_in_role($admin_role);
                if($count <= 1 && in_array($admin_role,$roles)){
                    $this->set_error('one_admin_required');
                    return FALSE;
                }
                if ($this->ci->ion_auth_model_mongo->delete_user($id))
		{
			$this->set_message('user_delete_successful');
			return TRUE;
		}

		$this->set_error('user_delete_unsuccessful');
		return FALSE;
	}

        /**
         *Delete role
         *
         * @param type $name role name
         * @return type bool
         */

        public function delete_role($username, $name)
        {
          try{
                if($this->ci->ion_auth_model_mongo->delete_role($username, $name))
                {
                    $this->set_message('role_delete_successful');
                    return TRUE;
                }
             $this->set_error('role_delete_unsuccessful');
             return FALSE;
          }catch(Exception $e){
               $this->set_error('role_delete_unsuccessful');
	       throw $e;
          }  
        }


	/**
	 * extra_where
	 *
	 * Crazy function that allows extra where field to be used for user fetching/unique checking etc.
	 * Basically this allows users to be unique based on one other thing than the identifier which is helpful
	 * for sites using multiple domains on a single database.
	 *
	 * @return void
	 * @author Phil Sturgeon
	 **/
	public function extra_where()
	{
		$where =& func_get_args();

		$this->_extra_where = count($where) == 1 ? $where[0] : array($where[0] => $where[1]);
	}

	/**
	 * extra_set
	 *
	 * Set your extra field for registration
	 *
	 * @return void
	 * @author Phil Sturgeon
	 **/
	public function extra_set()
	{
		$set =& func_get_args();

		$this->_extra_set = count($set) == 1 ? $set[0] : array($set[0] => $set[1]);
	}

	/**
	 * set_message_delimiters
	 *
	 * Set the message delimiters
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function set_message_delimiters($start_delimiter, $end_delimiter)
	{
		$this->message_start_delimiter = $start_delimiter;
		$this->message_end_delimiter   = $end_delimiter;

		return TRUE;
	}

        /**
         * set_info_delimiters
         */
        public function set_info_delimiters($start_delimiter, $end_delimiter)
	{
		$this->info_start_delimiter = $start_delimiter;
		$this->info_end_delimiter   = $end_delimiter;
		return TRUE;
	}

	/**
	 * set_error_delimiters
	 *
	 * Set the error delimiters
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function set_error_delimiters($start_delimiter, $end_delimiter)
	{
		$this->error_start_delimiter = $start_delimiter;
		$this->error_end_delimiter   = $end_delimiter;

		return TRUE;
	}

	/**
	 * set_message
	 *
	 * Set a message
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function set_message($message)
	{
		$this->messages[] = $message;

		return $message;
	}

	/**
	 * messages
	 *
	 * Get the messages
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function messages()
	{
		$_output = '';
		foreach ($this->messages as $message)
		{
			$_output .= $this->message_start_delimiter . $this->ci->lang->line($message) . $this->message_end_delimiter;
		}

		return $_output;
	}

	/**
	 * set_error
	 *
	 * Set an error message
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function set_error($error)
	{
		$this->errors[] = $error;

		return $error;
	}

	/**
	 * errors
	 *
	 * Get the error message
	 *
	 * @return void
	 * @author Ben Edmunds
	 **/
	public function errors()
	{
		$_output = '';
		foreach ($this->errors as $error)
		{
                    if ($this->ci->lang->line($error) != '')
			$_output .= $this->error_start_delimiter . $this->ci->lang->line($error) . $this->error_end_delimiter;
                    else
                        $_output .= $this->error_start_delimiter . $error . $this->error_end_delimiter;
		}

		return $_output;
	}
      /**
       *To set the information to be displayed to user
       * @param <type> $info
       * @return <type>
       */
        public function set_info($info)
        {
            $this->infos[] = $info;
           return $info;
        }

      /**
       *get all the information to be displayed
       * @return string
       */
        public function infos()
	{
		$_output = '';
		foreach ($this->infos as $info)
		{
			$_output .= $this->info_start_delimiter . $this->ci->lang->line($info) . $this->info_end_delimiter;
		}
		return $_output;
	}
	/**
	 *get_roles
	 *get roles in the system
	 *
	 *@return array
	 *@author sudhir
	 **/
	 public function get_roles($username=NULL)
	 {
            /*if(strtolower($this->mode)!='database'){
                if(!$this->ci->session->userdata('pwd')){
                     $this->set_error('login_mode_changed');
                }
                 if($this->ci->session->userdata('dn')){
                 $this->ci->auth_ldap->set_user_dn($this->ci->session->userdata('dn'));
            }
               return $this->ci->auth_ldap->get_all_ldap_roles( $this->ci->session->userdata('username'), $this->ci->session->userdata('pwd'));
            }*/
		 return $this->ci->ion_auth_model_mongo->get_roles($username);
	 }

         public function get_roles_fromdb()
	 {
		 return $this->ci->ion_auth_model_mongo->get_roles($this->ci->session->userdata('username'));
	 }

	 /**
	 *get_role
	 * particular get role in the system
	 *
	 *@return object
	 *@author sudhir
	 **/

	 public function get_role($username = NULL, $rolename = NULL)
	 {
		 return $this->ci->ion_auth_model_mongo->get_role($username, $rolename);
	 }

	 /**
	 *create_role
	 *Update the role
	 *
	 *@return object
	 *@author sudhir
	 **/

	 public function create_role($username, $data)
	 {
            $id = $this->ci->ion_auth_model_mongo->create_role($username, $data);

            if ($id !== FALSE)
            {
                $this->set_message('role_creation_successful');
                return $id;
            }
            else
            {
                $this->set_error('role_creation_unsuccessful');
                return FALSE;
            }
	 }
	 /**
	 *update_role
	 *Update the role
	 *@param
          * username - current logged user
          * rolename - name of the role
          * data - new values
         *
	 *@return bool
	 *@author sudhir
	 **/

	 public function update_role($username, $data)
	  {
	    if ($this->ci->ion_auth_model_mongo->update_role($username, $data))
		{
			$this->set_message('role_update_successful');
			return TRUE;
		}
	  }

          public function set_mode($mode){
              $this->mode=$mode;
          }

}