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
		$this->ci =& get_instance();
		$this->ci->load->config('ion_auth', TRUE);
                $this->ci->load->config('appsettings', TRUE);
		$this->ci->load->library('email');
		$this->ci->load->library('session');
                $this->ci->load->library('Auth_Ldap');
                $this->ci->load->library('encrypt');
		$this->ci->lang->load('ion_auth');
		$this->ci->load->model('ion_auth_model_mongo');
                $this->ci->load->model('settings_model');
		$this->ci->load->helper('cookie');

		$this->messages = array();
		$this->errors = array();
                $this->infos=array();
		$this->message_start_delimiter = $this->ci->config->item('message_start_delimiter', 'ion_auth');
		$this->message_end_delimiter   = $this->ci->config->item('message_end_delimiter', 'ion_auth');
		$this->error_start_delimiter   = $this->ci->config->item('error_start_delimiter', 'ion_auth');
		$this->error_end_delimiter     = $this->ci->config->item('error_end_delimiter', 'ion_auth');
                $this->info_start_delimiter =$this->ci->config->item('info_start_delimiter', 'ion_auth');
                $this->info_end_delimiter =$this->ci->config->item('info_end_delimiter', 'ion_auth');
                //load the mode of authentication
                $this->mode=$this->ci->settings_model->app_settings_get_item('mode');
                if(!$this->mode){
                    $this->set_error('backend_error');
                    $this->mode='database';
		    //return FALSE;
                }
                if($this->ci->config->item('auth_mode') &&$this->ci->config->item('auth_mode') !=''){
                   $this->mode=strtolower($this->ci->config->item('auth_mode'));
                }

                if($this->ci->session->userdata('mode')!==FALSE)
                {
                    $this->mode=$this->ci->session->userdata('mode');
                }
                 //$this->mode='database';

		//auto-login the user if they are remembered
		if (!$this->logged_in() && get_cookie('identity') && get_cookie('remember_code'))
		{
			$this->ci->ion_auth_model_mongo->login_remembered_user();
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
		if ($this->ci->ion_auth_model_mongo->activate($id, $code))
		{
			$this->set_message('activate_successful');
			return TRUE;
		}

		$this->set_error('activate_unsuccessful');
		return FALSE;
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
			$this->ci->email->from($this->ci->config->item('admin_email', 'ion_auth'), $this->ci->config->item('site_title', 'ion_auth'));
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
			$this->ci->email->from($this->ci->config->item('admin_email', 'ion_auth'), $this->ci->config->item('site_title', 'ion_auth'));
			$this->ci->email->to($profile->email);
			$this->ci->email->subject($this->ci->config->item('site_title', 'ion_auth') . ' - New Password');
			$this->ci->email->message($message);

			if ($this->ci->email->send())
			{
				$this->set_message('password_change_successful');
				return TRUE;
			}
			else
			{
				$this->set_error('password_change_unsuccessful');
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
	public function register($username, $password, $email, $group, $additional_data = false) //need to test email activation
	{
		$email_activation = $this->ci->config->item('email_activation', 'ion_auth');

		if (!$email_activation)
		{
			$id = $this->ci->ion_auth_model_mongo->register($username, $password, $email, $group, $additional_data);
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
			$id = $this->ci->ion_auth_model_mongo->register($username, $password, $email, $group, $additional_data);

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
			$this->ci->email->from($this->ci->config->item('admin_email', 'ion_auth'), $this->ci->config->item('site_title', 'ion_auth'));
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
	 * login
	 *
	 * @return void
	 * @author Mathew
	 **/
	public function login($identity, $password, $remember=false)
	{
          
              if($this->mode!='database')
              {
                 $ret=$this->ci->auth_ldap->login($identity,$password);
                     if(is_array($ret))
                     {
                      $this->ci->session->set_userdata($ret);
                      $this->ci->session->set_userdata('pwd', $password);
                      return TRUE;
                     }
                      foreach((array)$this->ci->auth_ldap->get_unformatted_error() as $error){
                       $this->set_error($error);
                   }
                   //fall back to database and store the mode for latter use in application
                   if ($this->ci->ion_auth_model_mongo->login($identity, $password, $remember,true))
		     {
                       $this->set_mode('database');
                       $this->ci->session->set_userdata('mode','database');
			$this->set_message('login_successful');
			return TRUE;
		    }
                //$this->set_error('login_unsuccessful');
		return FALSE;
               }
               
		if ($this->ci->ion_auth_model_mongo->login($identity, $password, $remember))
		{
			$this->set_message('login_successful');
			return TRUE;
		}

		$this->set_error('login_unsuccessful');
		return FALSE;
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
		$this->ci->session->unset_userdata('group');
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
	public function is_admin()
	{
                 $admin_group=$this->ci->settings_model->app_settings_get_item('admin_group');
                 
                 if($admin_group===False){
                     $admin_group = $this->ci->config->item('admin_group', 'ion_auth');
                 }
                   $user_group  = $this->ci->session->userdata('group');
                       if($user_group===False){
                        return false;
                      }
                return in_array($admin_group, $user_group);
	}

        /**
	 * is_in_fallback_group
	 *
	 * @return bool
	 * @author sudhir
	 **/
        public function is_in_fallback_group()
	{
                 $admin_group=$this->ci->settings_model->app_settings_get_item('fall_back_for');

                 if($admin_group===False){
                     $admin_group = $this->ci->config->item('admin_group', 'ion_auth');
                 }
                 $user_group  = $this->ci->session->userdata('group');
                     if($user_group===False){
                        return false;
                    }
                return in_array($admin_group, $user_group);
	}

        /**
         *
         */

        public function is_accessable()
        {
           $isadmin=$this->ci->settings_model->app_settings_get_item('admin_group');
            if($isadmin!==FALSE && (!$this->is_admin() && !$this->is_in_fallback_group())){
                return false;
            }
            return true;
        }
	/**
	 * is_group
	 *
	 * @return bool
	 * @author Phil Sturgeon
	 **/
	public function is_group($check_group)
	{
		$user_group = $this->ci->session->userdata('group');

		if(is_array($check_group))
		{
			return in_array($user_group, $check_group);
		}

		return $user_group == $check_group;
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
	public function get_users($group_name=false, $limit=NULL, $offset=NULL)
	{
		return $this->ci->ion_auth_model_mongo->get_users($group_name, $limit, $offset);
	}


        public function get_user_group($id)
        {
            return $this->ci->ion_auth_model_mongo->get_users_group($id);
        }
        
        public function get_user_grouplist($id){
            $groups= $this->ci->ion_auth_model_mongo->get_users_group($id);
            if(!empty ($groups)&&$groups!==False){
                return $groups[0]['group'];
            }
            return array();
        }

	/**
	 * Get Users Array
	 *
	 * @return array Users
	 * @author Ben Edmunds
	 **/
       public function get_users_array($group_name=false, $limit=NULL, $offset=NULL) {
        if (strtolower($this->mode) != 'database') {
            if (!$this->ci->session->userdata('pwd')) {
                $this->set_error('login_mode_changed');
            }
            return $this->ci->auth_ldap->get_all_ldap_users($this->ci->session->userdata('username'), $this->ci->session->userdata('pwd'));
        }
        return $this->ci->ion_auth_model_mongo->get_users_by_group($group_name, $limit, $offset);
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
	public function get_active_users($group_name = false)
	{
		return (object)$this->ci->ion_auth_model_mongo->get_active_users($group_name);
	}

	/**
	 * Get Active Users Array
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_active_users_array($group_name = false)
	{
		return $this->ci->ion_auth_model_mongo->get_active_users($group_name);
	}

	/**
	 * Get In-Active Users
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_inactive_users($group_name = false)
	{
		return (object)$this->ci->ion_auth_model_mongo->get_inactive_users($group_name);
	}

	/**
	 * Get In-Active Users Array
	 *
	 * @return object Users
	 * @author Ben Edmunds
	 **/
	public function get_inactive_users_array($group_name = false)
	{
		return $this->ci->ion_auth_model_mongo->get_inactive_users($group_name);
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
	$groups=$this->get_user_grouplist($id);
                $admin_group=$this->ci->settings_model->app_settings_get_item('admin_group');
                $count=$this->ci->ion_auth_model_mongo->count_users_in_group($admin_group);
                if($count <= 1 && in_array($admin_group,$groups)&& !in_array($admin_group,$data['group'])){
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


	/**
	 * delete_user
	 *
	 * @return void
	 * @author Phil Sturgeon
	 **/
	public function delete_user($id)
	{
	$groups=$this->get_user_grouplist($id);
                $admin_group=$this->ci->settings_model->app_settings_get_item('admin_group');
                $count=$this->ci->ion_auth_model_mongo->count_users_in_group($admin_group);
                if($count <= 1 && in_array($admin_group,$groups)){
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


        public function delete_group($id)
        {
            if($this->ci->ion_auth_model_mongo->delete_group($id))
            {
                $this->set_message('group_delete_successful');
	        return TRUE;
            }

           $this->set_error('group_delete_unsuccessful');
	   return FALSE;
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
			$_output .= $this->error_start_delimiter . $this->ci->lang->line($error) . $this->error_end_delimiter;
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
	 *get_groups
	 *get groups in the system
	 *
	 *@return array
	 *@author sudhir
	 **/
	 public function get_groups()
	 {
            if(strtolower($this->mode)!='database'){
                if(!$this->ci->session->userdata('pwd')){
                     $this->set_error('login_mode_changed');
                }
               return $this->ci->auth_ldap->get_all_ldap_groups( $this->ci->session->userdata('username'), $this->ci->session->userdata('pwd'));
            }
		 return $this->ci->ion_auth_model_mongo->get_groups();
	 }

          public function get_groups_fromdb()
	 {
		 return $this->ci->ion_auth_model_mongo->get_groups();
	 }
	 
	 /**
	 *get_group
	 * particular get group in the system
	 *
	 *@return object
	 *@author sudhir
	 **/
	 
	  public function get_group($id)
	 {
		 return $this->ci->ion_auth_model_mongo->get_group($id);
	 }
	 
	 /**
	 *create_group
	 *Update the group
	 *
	 *@return object
	 *@author sudhir
	 **/
	 
	 public function create_group($data)
	 {
		 $id=$this->ci->ion_auth_model_mongo->create_group($data);
		 if ($id !== FALSE)
			{
				$this->set_message('group_creation_successful');
				return $id;
			}
			else
			{
				$this->set_error('group_creation_unsuccessful');
				return FALSE;
			}
	  

	 }
	 /**
	 *update_group
	 *Update the group
	 *
	 *@return bool
	 *@author sudhir
	 **/
	 
	 public function update_group($id,$data)
	  {
	    if ($this->ci->ion_auth_model_mongo->update_group($id, $data))
		{
			$this->set_message('group_update_successful');
			return TRUE;
		}

		$this->set_error('group_update_unsuccessful');
		return FALSE;
	  }

          public function set_mode($mode){
              $this->mode=$mode;
          }

}