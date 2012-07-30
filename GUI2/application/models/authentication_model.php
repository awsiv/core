<?php

class Authentication_model extends Cf_Model
{

    protected $userResource = 'user';
    protected $rolesResource = 'role';

    public function __construct()
    {
        parent::__construct();
        $this->load->config('ion_auth', TRUE);
        $this->load->helper('cookie');
        $this->load->helper('date');
        $this->load->library('mongo_db');
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
            $data = $this->checkData($body);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0];
            }
            throw new Exception($this->getErrorsString());
        }
        catch (Exception $e)
        {
            log_message('error', 'login '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     * Get Roles for a users
     */
    function getRolesForUser($username)
    {
        try{
            
            $user = $this->getUserDetails($username);
            if (key_exists('roles', $user))
            {
                return $user['roles'];
            }
            
        }catch(Exception $e){
             log_message('error', "getRolesForUser , Error grabbing roles for user");
             return false;
        }
       
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
            log_message('error','getAllRoles '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Get the details of a role
     * @param type $rolename 
     */
    function getRoleDetails($rolename)
    {
        try
        {
            $body = $this->rest->get('/' . $this->rolesResource . '/' . $rolename);
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
            log_message('error', 'getRoleDetails '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * For creating users
     * @param type $data 
     */
    function createRole($data)
    {
        try
        {
            $this->rest->put('/' . $this->rolesResource . '/' . $data['name'], $data);
            if ($this->rest->lastStatus() == 201)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', 'createRole '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $data
     * @return type boolean
     */
    function updateRole($rolename, $data)
    {
        try
        {
            $this->rest->post('/' . $this->rolesResource . '/' . $rolename, $data);
            if ($this->rest->lastStatus() == 204)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            log_message('error', 'updateRole '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Deletes a role
     * @param type $rolename 
     */
    function deleteRole($rolename)
    {
        try
        {
            $this->rest->delete('/' . $this->rolesResource . '/' . $rolename);
            if ($this->rest->lastStatus() == 204)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            log_message('error', 'deleteRole '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Create a user
     * @param type $data
     * @return type 
     */
    function createUser($data)
    {
        try
        {
            $this->rest->put('/' . $this->userResource . '/' . $data['username'], $data);
            if ($this->rest->lastStatus() == 201)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', 'createUser '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @return type array data
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
            log_message('error', 'getAllUsers '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
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
            log_message('error', "getUserDetails  ".  get_class($e)." ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $data 
     */
    function update_user($username, $data)
    {

        try
        {
            $this->rest->post('/' . $this->userResource . '/' . $username, $data);
            if ($this->rest->lastStatus() == 204)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            $this->setError($e->getMessage());
            log_message('error', 'update_user '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Delete the supplied user
     * @param type $username 
     */
    function deleteUser($username)
    {
        try
        {
            $this->rest->delete('/' . $this->userResource . '/' . $username);
            if ($this->rest->lastStatus() == 204)
            {
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
            log_message('error','deleteUser '.get_class($e)."  ". $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
    function change_password($username,$old,$new){
     try
        {
          $this->rest->setupAuth($username,$old);
          $this->update_user($username, array('password'=>$new));
          if ($this->rest->lastStatus() == 204)
            {
                $this->rest->setupAuth($username,$new);
                return true;
            }
            return false;
        }
        catch (Exception $e)
        {
           log_message('error', 'change_password '.get_class($e)."  ".$e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
           throw $e; 
        }
        
    }

   
}

?>
