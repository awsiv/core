<?php

class Authentication_model extends Cf_Model
{

    protected $userResource = 'user';
    protected $rolesResource = 'role';
    protected $rest = '';

    public function __construct()
    {
        parent::__construct();
        $this->load->config('ion_auth', TRUE);
        $this->load->helper('cookie');
        $this->load->helper('date');
        $this->load->library('mongo_db');
    }

    public function setRestClient($rest)
    {
        $this->rest = $rest;
    }

    /**
     * For login operation
     * @param type $username
     * @param type $password
     * @param type $remmember
     * @return type
     */
    function login($identity, $password, $mode = 'basic')
    {
        try
        {
            $this->rest->setupAuth($identity, $password);
            $body = $this->rest->get('/');
            $this->update_last_login($identity);
            return true;
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
           
        }
    }

    /**
     * Get Roles for a users
     */
    function getRolesForUser($username)
    {
        $user = $this->getUserDetails($username);
        if (key_exists('roles', $user))
        {
            return $user['roles'];
        }
        return false;
    }

    /**
     * Get all roles
     */
    function getAllRoles()
    {
        try
        {
            $body = $this->rest->get('/' . $this->rolesResource);
            $data = $this->checkData($body);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Get all users 
     */
    function getAllUsers()
    {
        try
        {
            $body = $this->rest->get('/' . $this->userResource);
            $data = $this->checkData($body);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Generate the details for given object i.e user
     * @param type $username
     * @param type $password
     * @param type $object
     * @return type
     */
    function getUserDetails($username)
    {
        
        try
        {
            $body = $this->rest->get('/' . $this->userResource . '/' . $username);
            $data = $this->checkData($body);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    
    /**
     *
     * @param type $username
     * @param type $data 
     */
    function update_user($username,$data){
        
        try
        {
            $this->rest->post('/' . $this->userResource . '/' . $username, $data);
            if($this->rest->lastStatus() == 204){
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
    
    /**
     * Update the last login time for user in local db related to the user.
     * @param type $username
     * @return type
     */
    public function update_last_login($username)
    {
        $this->load->helper('date');
        $this->mongo_db->where(array('username' => $username));
        $this->mongo_db->update('users', array('last_login' => now()));
        return TRUE;
    }

    private function remember_user($user, $password)
    {
        if (!$id)
        {
            return FALSE;
        }

        $user = $this->get_user($user)->row();
        $salt = sha1($password);
        $this->db->update($this->tables['users'], array('remember_code' => $salt), array('id' => $id));

        if ($this->db->affected_rows() > -1)
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

}

?>
