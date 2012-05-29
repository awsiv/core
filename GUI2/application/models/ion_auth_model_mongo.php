<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');
/**
 * Name:  Ion Auth Model
 *
 * Author:  Sudhir Pandey
 *
 * Added Awesomeness: Phil Sturgeon
 *
 * Location: www.cfengine.com
 *
 * Created:  03.01.2011
 *
 *
 * Requirements: PHP5 or above
 *
 */
//  CI 2.0 Compatibility
if (!class_exists('CI_Model'))
{

    class CI_Model extends Model
    {

    }

}

class Ion_auth_model_mongo extends CI_Model
{

    /**
     * Holds an array of tables used
     *
     * @var string
     * */
    public $tables = array();

    /**
     * activation code
     *
     * @var string
     * */
    public $activation_code;

    /**
     * forgotten password key
     *
     * @var string
     * */
    public $forgotten_password_code;

    /**
     * new password
     *
     * @var string
     * */
    public $new_password;

    /**
     * Identity
     *
     * @var string
     * */
    public $identity;

    public function __construct()
    {
        parent::__construct();
        $this->load->config('ion_auth', TRUE);
        $this->load->helper('cookie');
        $this->load->helper('date');
        $this->load->library('session');
        $this->load->library('mongo_db');

        $this->tables = $this->config->item('tables', 'ion_auth');
        $this->columns = $this->config->item('columns', 'ion_auth');

        $this->identity_column = $this->config->item('identity', 'ion_auth');
        $this->store_salt = $this->config->item('store_salt', 'ion_auth');
        $this->salt_length = $this->config->item('salt_length', 'ion_auth');
    }

    /**
     * Misc functions
     *
     * Hash password : Hashes the password to be stored in the database.
     * Hash password db : This function takes a password and validates it
     * against an entry in the users table.
     * Salt : Generates a random salt value.
     *
     * @author Mathew
     */

    /**
     * Hashes the password to be stored in the database.
     *
     * @return void
     * @author Mathew
     * */
    public function hash_password($password, $salt = false)
    {
        if (empty($password))
        {
            return FALSE;
        }

        if ($this->store_salt && $salt)
        {
            return sha1($password . $salt);
        }
        else
        {
            $salt = $this->salt();
            return $salt . substr(sha1($salt . $password), 0, -$this->salt_length);
        }
    }

    public function hash_password_db($identity, $password)
    {
        if (empty($identity) || empty($password))
        {
            return FALSE;
        }

        $result = $this->mongo_db->select(array('password', 'salt'))
                ->where(array($this->identity_column => $identity))
                ->limit(1)
                ->get_object('users');

        if (count($result) !== 1)
        {
            return FALSE;
        }

        if ($this->store_salt)
        {
            return sha1($password . $result->salt);
        }
        else
        {
            $salt = substr($result->password, 0, $this->salt_length);

            return $salt . substr(sha1($salt . $password), 0, -$this->salt_length);
        }
    }

    /**
     * Generates a random salt value.
     *
     * @return void
     * @author Mathew
     * */
    public function salt()
    {
        return substr(md5(uniqid(rand(), true)), 0, $this->salt_length);
    }

    /**
     * Activation functions
     *
     * Activate : Validates and removes activation code.
     * Deactivae : Updates a users row with an activation code.
     *
     * @author Mathew

      /**
     * activate
     *
     * @return void
     * @author sudhir
     * */
    public function activate($id, $code = false)
    {
        if ($code != false)
        {
            /* $query = $this->db->select($this->identity_column)
              ->where('activation_code', $code)
              ->limit(1)
              ->get($this->tables['users']); */
            $result = $this->mongo_db->select(array($this->identity_column))
                    ->where(array('activation_code' => $code))
                    ->limit(1)
                    ->get('users');


            if (count($result) !== 1)
            {
                return FALSE;
            }

            $identity = $result->{$this->identity_column};
            $data = array(
                'activation_code' => '',
                'active' => 1
            );
            $this->mongo_db->where(array($this->identity_column => $identity))
                    ->update('users', $data);
        }
        else
        {
            $data = array(
                'activation_code' => '',
                'active' => 1
            );
            $this->mongo_db->where(array('_id' => new MongoId($id)))
                    ->update('users', $data);
        }

        return true;
    }

    /**
     * Deactivate
     *
     * @return void
     * @author sudhir
     * */
    public function deactivate($id = 0)
    {
        if (empty($id))
        {
            return FALSE;
        }

        $activation_code = sha1(md5(microtime()));
        $this->activation_code = $activation_code;

        $data = array(
            'activation_code' => $activation_code,
            'active' => 0
        );

        //$this->db->where($this->ion_auth->_extra_where);
        // $this->db->update($this->tables['users'], $data, array('id' => $id));
        $this->mongo_db->where(array('_id' => new MongoId($id)))->update('users', $data);
        // return $this->db->affected_rows() == 1;
        return true;
    }

    /**
     * change password
     *
     * @return bool
     * @author sudhir
     * */
    public function change_password($identity, $old, $new)
    {

        $result = $this->mongo_db->select(array('password', 'username'))
                ->where(array('_id' => new MongoId($identity)))
                ->limit(1)
                ->get_object('users');

        //$result = $query->row();

        $db_password = $result->password;
        $old = $this->hash_password_db($result->username, $old);
        $new = $this->hash_password($new, $this->store_salt ? $result->salt : FALSE);

        if ($db_password === $old)
        {
            //store the new password and reset the remember code so all remembered instances have to re-login
            $data = array(
                'password' => $new,
                    //'remember_code' => '',
            );
            $result = $this->mongo_db->where(array('_id' => new MongoId($identity)))->update('users', $data);
            //$this->db->where($this->ion_auth->_extra_where);
            //$this->db->update($this->tables['users'], $data, array($this->identity_column => $identity));
            //return $this->db->affected_rows() == 1;
            return TRUE;
        }

        return FALSE;
    }

    /**
     * Checks username
     *
     * @return bool
     * @author Mathew
     * */
    public function username_check($username = '')
    {
        if (empty($username))
        {
            return FALSE;
        }
        return $this->mongo_db->where(array('username' => $username))->count('users') > 0;
    }

    /**
     * Checks email
     *
     * @return bool
     * @author Mathew
     * */
    public function email_check($email = '')
    {
        if (empty($email))
        {
            return FALSE;
        }

        return $this->mongo_db->where(array('email' => $email))->count('users') > 0;
    }

    /**
     * Identity check
     *
     * @return bool
     * @author Mathew
     * */
    protected function identity_check($identity = '')
    {
        if (empty($identity))
        {
            return FALSE;
        }
        /* if($this->mongo_db->where(array($this->identity_column => $identity))->count('users') > 0)
          return TRUE;
          else
          return FALSE; */

        return $this->mongo_db->where(array($this->identity_column => $identity))->count('users') > 0;
    }

    /**
     * Insert a forgotten password key.
     *
     * @return bool
     * @author Mathew
     * */
    public function forgotten_password($email = '')
    {
        if (empty($email))
        {
            return FALSE;
        }

        $key = $this->hash_password(microtime() . $email);

        $this->forgotten_password_code = $key;

        $this->mongo_db->where(array('email' => $email))->update('users', array('forgotten_password_code' => $key));

        return true;
    }

    /**
     * Forgotten Password Complete
     *
     * @return string
     * @author Mathew
     * */
    public function forgotten_password_complete($code, $salt = FALSE)
    {
        if (empty($code))
        {
            return FALSE;
        }

        $this->mongo_db->where(array('forgotten_password_code' => $code));

        if ($this->mongo_db->count('users') > 0)
        {
            //$password = $this->salt();
            $password = $this->salt();
            $data = array(
                'password' => $this->hash_password($password, $salt),
                'forgotten_password_code' => '0',
                'active' => 1,
            );
            $this->mongo_db->where(array('forgotten_password_code' => $code))->update('users', $data);

            return $password;
        }

        return FALSE;
    }

    /**
     * profile
     *
     * @return void
     * @author Mathew
     * */
    public function profile($identity = '', $is_code = false)
    {
        /* if (empty($identity))
          {
          return FALSE;
          }

          $this->db->select(array(
          $this->tables['users'].'.*',
          $this->tables['roles'].'.name AS '. $this->db->protect_identifiers('role'),
          $this->tables['roles'].'.description AS '. $this->db->protect_identifiers('role_description')
          ));

          if (!empty($this->columns))
          {
          foreach ($this->columns as $field)
          {
          $this->db->select($this->tables['meta'] .'.' . $field);
          }
          }

          $this->db->join($this->tables['meta'], $this->tables['users'].'.id = '.$this->tables['meta'].'.'.$this->meta_join, 'left');
          $this->db->join($this->tables['roles'], $this->tables['users'].'.role_id = '.$this->tables['roles'].'.id', 'left');

          if ($is_code)
          {
          $this->db->where($this->tables['users'].'.forgotten_password_code', $identity);
          }
          else
          {
          $this->db->where($this->tables['users'].'.'.$this->identity_column, $identity);
          }

          $this->db->where($this->ion_auth->_extra_where);

          $this->db->limit(1);
          $i = $this->db->get($this->tables['users']);

          return ($i->num_rows > 0) ? $i->row() : FALSE; */
        if ($is_code)
        {
            return $this->mongo_db->where(array('forgotten_password_code' => $identity))->get_object('users');
        }
        else
        {
            return $this->mongo_db->where(array($this->identity_column => $identity))->get_object('users');
        }
    }

    /**
     * Basic functionality
     *
     * Register
     * Login
     *
     * @author Mathew
     */

    /**
     * register
     *
     * @return bool
     * @author Mathew
     * */
    public function register($username, $password, $email, $roles = array(), $additional_data = false)
    {
        if ($this->identity_column == 'email' && $this->email_check($email))
        {
            $this->ion_auth->set_error('account_creation_duplicate_email');
            return FALSE;
        }
        elseif ($this->identity_column == 'username' && $this->username_check($username))
        {
            $this->ion_auth->set_error('account_creation_duplicate_username');
            return FALSE;
        }

        // If username is taken, use username1 or username2, etc.
        if ($this->identity_column != 'username')
        {
            for ($i = 0; $this->username_check($username); $i++)
            {
                if ($i > 0)
                {
                    $username .= $i;
                }
            }
        }


        // IP Address
        $ip_address = $this->input->ip_address();
        $salt = $this->store_salt ? $this->salt() : FALSE;
        $password = $this->hash_password($password, $salt);

        // Users table.
        $data = array(
            'username' => $username,
            'password' => $password,
            'email' => $email,
            'roles' => $roles,
            'ip_address' => $ip_address,
            'created_on' => now(),
            'last_login' => now(),
            'active' => 1
        );

        if ($this->store_salt)
        {
            $data['salt'] = $salt;
        }

        $id = $this->mongo_db->insert('users', $data);
        return $id;
    }

    /**
     * login
     *
     * @return bool
     * @author Mathew
     * */
    public function login($identity, $password, $remember = FALSE, $ldap_fallback = FALSE)
    {
        if (empty($identity) || empty($password) || !$this->identity_check($identity))
        {
            return FALSE;
        }

        if ($ldap_fallback)
        {
            $this->load->model('settings_model');
            $role = $this->settings_model->app_settings_get_item('fall_back_for');

            $this->mongo_db->select(array($this->identity_column, 'id', 'password', 'roles'));
            $this->mongo_db->where(array($this->identity_column => $identity, 'active' => 1, 'roles' => $role));
        }
        else
        {
            $this->mongo_db->select(array($this->identity_column, 'id', 'password', 'roles'));
            $this->mongo_db->where(array($this->identity_column => $identity, 'active' => 1));
        }

        $result = $this->mongo_db->limit(1)
                ->get_object('users');

        if ($result != NULL)
        {
            $password = $this->hash_password_db($identity, $password);

            if ($result->password === $password)
            {
                $session_data = array(
                    $this->identity_column => $result->{$this->identity_column},
                    'id' => $result->_id, //kept for backwards compatibility
                    'user_id' => $result->_id, //everyone likes to overwrite id so we'll use user_id
                    'roles' => $result->roles
                );

                $this->session->set_userdata($session_data);

                if ($remember && $this->config->item('remember_users', 'ion_auth'))
                {
                    $this->remember_user($result->_id);
                }

                return TRUE;
            }
        }

        return FALSE;
    }

    /**
     * get_users
     *
     * @return object Users
     * @author Ben Edmunds
     * */
    public function get_users_by_role($role = false, $limit = NULL, $offset = NULL)
    {
        if (is_string($role))
        {
            $this->mongo_db->where(array('roles' => $role));
        }
        else if (is_array($role))
        {
            $this->mongo_db->where_in('roles', $role);
        }
        if (isset($limit))
            $this->mongo_db->limit($limit);
        if (isset($offset))
            $this->mongo_db->offset($offset);
        $this->mongo_db->order_by(array('username' => 'asc'));
        return $this->mongo_db->get('users');
    }

    /**
     * get_active_users
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_active_users($role_name = false)
    {
        $this->mongo_db->where(array('active' => 1));
        return $this->get_users($role_name);
    }

    /**
     * get_inactive_users
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_inactive_users($role_name = false)
    {
        $this->mongo_db->where(array('active' => 0));
        return $this->get_users($role_name);
    }

    /**
     * get_user
     *
     * @return object
     * @author Phil Sturgeon
     * */
    public function get_user($id = false)
    {
        //if no id was passed use the current users id
        if (empty($id))
        {
            $id = $this->session->userdata('user_id');
        }
        return $this->mongo_db->get_where_object('users', array('_id' => new MongoId($id)), 1);
    }

    public function get_user_ldap($username = false)
    {
        //if no id was passed use the current users id
        if (empty($username))
        {
            $username = $this->session->userdata('user_id');
        }
        return $this->mongo_db->get_where_object('ldap_users', array('username' => $username), 1);
    }

    /**
     * get_user_by_email
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_user_by_email($email)
    {
        return $this->mongo_db->get_where_object('users', array('email' => $email), 1);
    }

    public function get_user_by_col($col, $col_val)
    {
        return $this->mongo_db->get_where_object('users', array($col => $col_val), 1);
    }

    public function get_ldap_user_by_col($col, $col_val)
    {
        return $this->mongo_db->get_where_object('ldap_users', array($col => $col_val), 1);
    }

    /**
     * get_newest_users
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_newest_users($limit = 10)
    {
        $this->mongo_db->order_by(array('created_on' => 'desc'));
        $this->mongo_db->limit($limit);
        return $this->get_users();
    }

    /**
     * get_users_role
     *
     * @return object
     * @author sudhir
     * */
    public function get_users_role($id = false)
    {
        //if no id was passed use the current users id
        $id || $id = $this->session->userdata('user_id');

        return $this->mongo_db->select(array('roles'))
                        ->where(array('_id' => new MongoId($id)))
                        ->get('users');
    }

    /**
     * get_roles
     *   Username - we should pass this wariable for authentification
     * @return object
     * @author Phil Sturgeon
     * */
    public function get_roles($username = '')
    {
        if (trim($username) == '')
        {
            $this->ion_auth->set_error('no_permission');
            return FALSE;
        }

        try
        {

            $ret = json_decode(cfpr_role_list_all($username), TRUE);

            return $ret;
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage());
            throw $e;
        }
        $this->ion_auth->set_error('no_access');
        return FALSE;
    }

    /**
     * get_role
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_role($username, $rolename)
    {
        $ret = json_decode(cfpr_role_list_by_name($username, $rolename), true);
        if (count($ret) > 0)
        {
            return $ret[0];
        }
        return false;
    }

    /**
     * get_role_by_name
     *
     * @return object
     * @author Ben Edmunds
     * */
    public function get_role_by_name($name)
    {

        // return $this->mongo_db->get_where_object('roles',array('name'=>$name),1);
        return $this->get_role($username, $rolename);
    }

    public function role_check($username, $rolename)
    {
        if (count($this->get_role($username, $rolename)) > 0)
        {
            return true;
        }
        return false;
    }

    /**
     * update_user
     *
     * @return bool
     * @author Phil Sturger modified by sudhir
     * */
    public function update_user($id, $data)
    {
        if (array_key_exists($this->identity_column, $data) && $this->identity_check($data[$this->identity_column]))
        {
            $user = $this->get_user_by_col($this->identity_column, $data[$this->identity_column]);
            if ($user->_id->__toString() != $id)
            {
                $this->ion_auth->set_error('account_creation_duplicate_' . $this->identity_column);
                return FALSE;
            }
        }

        if (array_key_exists('username', $data) || array_key_exists('password', $data) || array_key_exists('email', $data))
        {
            if (array_key_exists('password', $data))
            {
                $data['password'] = $this->hash_password($data['password'], $user->salt);
            }

            $this->mongo_db->where(array('_id' => new MongoId($id)));
            $this->mongo_db->update('users', $data);
            $this->mongo_db->clear();
        }
        return TRUE;
    }

    /**
     *
     * @param type $username
     * @param type $data
     * for updating the roles and related things for ldap user in local db
     */
    public function update_ldap_user($username, $data)
    {
        $this->mongo_db->where(array('username' => $username));
        $this->mongo_db->update('ldap_users', $data);
    }

    /**
     *
     * @param type $data
     * for caching the ldap username for RBAC implemetation
     */
    public function cache_ldap_user($data)
    {
        $id = $this->mongo_db->insert('ldap_users', $data);
        return $id;
    }

    public function total_ldap_users_cached()
    {
        return $this->mongo_db->count('ldap_users');
    }

    /**
     * delete_user
     *
     * @return bool
     * @author Phil Sturgeon
     * */
    public function delete_user($id)
    {
        $res = $this->get_user($id);
        if (empty($res))
        {
            throw new Exception("User doesn't exist");
        }

        return $this->mongo_db->where(array('_id' => new MongoId($id)))->delete('users');
    }

    /**
     * delete_role
     * @param
     * $username - admin by default
     * $name - role name
     *
     * @return bool
     * @author Sudhir Pandey
     * */
    public function delete_role($username, $name)
    {
        try
        {
            // cfpr_role_delete - return 1 if everything ok
            $ret = cfpr_role_delete($username, $name);

            if ($ret === 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage()." " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }

        $this->set_error('role_delete_unsuccessful');
        return FALSE;
    }

    public function update_last_login($username)
    {
        $this->load->helper('date');
        $this->mongo_db->where(array('username' => $username));
        $this->mongo_db->update('users', array('last_login' => now()));
        return TRUE;
    }

    public function update_last_login_ldap($username)
    {
        if ($this->mongo_db->where(array('username' => $username))->count('ldap_users') <= 0)
        {
            $this->cache_ldap_user(array('username' => $username));
        }

        $this->load->helper('date');
        $this->update_ldap_user($username, array('last_login' => now()));
    }

    public function is_first_login()
    {
        $user = $this->get_user();
        if ($user)
        {
            return !array_key_exists('last_login', $user);
        }
        return true;
    }

    public function is_first_login_ldap()
    {
        $user = $this->get_user_ldap();
        if ($user)
        {
            return !array_key_exists('last_login', $user);
        }
        return true;
    }

    /**
     * set_lang
     *
     * @return bool
     * @author Ben Edmunds
     * */
    public function set_lang($lang = 'en')
    {
        set_cookie(array(
            'name' => 'lang_code',
            'value' => $lang,
            'expire' => $this->config->item('user_expire', 'ion_auth') + time()
        ));

        return TRUE;
    }

    /**
     * login_remembed_user
     *
     * @return bool
     * @author Ben Edmunds
     * */
    public function login_remembered_user()
    {
        //check for valid data
        if (!get_cookie('identity') || !get_cookie('remember_code') || !$this->identity_check(get_cookie('identity')))
        {
            return FALSE;
        }

        $result = $this->mongo_db->select(array($this->identity_column, '_id', 'roles'))
                ->where(array($this->identity_column => get_cookie('identity'), 'remember_code' => get_cookie('remember_code')))
                ->limit(1)
                ->get_object('users');

        //if the user was found, sign them in
        if (is_object($result))
        {
            $user = $result;

            $session_data = array(
                $this->identity_column => $result->{$this->identity_column},
                'user_id' => $result->_id, //everyone likes to overwrite id so we'll use user_id
                'role' => $result->roles,
            );
            $this->session->set_userdata($session_data);
            //extend the users cookies if the option is enabled
            if ($this->config->item('user_extend_on_login', 'ion_auth'))
            {
                $this->remember_user($user->_id);
            }

            return TRUE;
        }

        return FALSE;
    }

    /**
     * remember_user
     *
     * @return bool
     * @author Sudhir pandey
     * */
    private function remember_user($id)
    {
        if (!$id)
        {
            return FALSE;
        }

        $user = $this->get_user($id);
        $salt = sha1($user->password);

        $this->mongo_db->clear();
        $this->mongo_db->where(array('_id' => new MongoId($id)));
        $result = $this->mongo_db->update('users', array('remember_code' => $salt));

        if ($result)
        {
            set_cookie(array(
                'name' => 'identity',
                'value' => $user->{$this->identity_column},
                'expire' => $this->config->item('user_expire', 'ion_auth'),
            ));

            set_cookie(array(
                'name' => 'remember_code',
                'value' => $salt,
                'expire' => $this->config->item('user_expire', 'ion_auth'),
            ));

            return TRUE;
        }
        return FALSE;
    }

    /**
     * add roles
     *
     * @ retutn id
     * @ author sudhir pandey
     * */
    public function create_role($username, $data)
    {
        try
        {
            // cfpr_role_create - return 1 if everything ok
            $ret = cfpr_role_create($username, $data['name'], $data['description'], $data['crxi'], $data['crxx'], $data['brxi'], $data['brxx']);

            if ($ret === 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage());
            throw $e;
        }

        $this->set_error('role_creation_unsuccessful');
        return FALSE;
    }

    /**
     * update roles
     *
     * @ return id
     * @ author sudhir pandey
     * */
    public function update_role($username, $data)
    {
        try
        {
            // cfpr_role_update - return 1 if everything ok
            $ret = cfpr_role_update($username, $data['name'], $data['description'], $data['crxi'], $data['crxx'], $data['brxi'], $data['brxx']);

            if ($ret === 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage());
            throw $e;
        }

        $this->set_error('role_update_unsuccessful');
        return FALSE;
    }

    function random_text($code_length = 10)
    {
        for ($code_length, $newcode = ''; strlen($newcode) < $code_length; $newcode .= chr(!rand(0, 2) ? rand(48, 57) : (!rand(0, 1) ? rand(65, 90) : rand(97, 122))))
            ;
        return $newcode;
    }

    function count_users_in_role($rolename)
    {
        return $this->mongo_db->where(array('roles' => $rolename))->count('users');
    }

    /**
     * Unset field in ldap_users item
     * @param type $id
     * @param type $fieldname
     */
    function unset_field_ldap_user($id, $fieldname)
    {
        $this->mongo_db->clear();
        $this->mongo_db->where(array('_id' => new MongoId($id)));
        $this->mongo_db->update_remove_field('ldap_users', $fieldname);
        $this->mongo_db->clear();
    }

}