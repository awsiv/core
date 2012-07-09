<?php

class Authentication_model extends Cf_Model
{

    protected $userResource = 'user';
    protected $baseResource = 'api';
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
        //var_dump($rest);
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
        $this->rest->setupAuth($identity, $password);

        try
        {
            $body = $this->rest->get('/');
            $this->update_last_login($identity);
            return true;
        }
        catch (Pest_Unauthorized $e)
        {
            $this->setError($e->getMessage());
            return false;
        }
        catch (Pest_UnknownResponse $e)
        {
            $this->setError($e->getMessage());
            return false;
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
     *
     */
    function getAllRoles()
    {

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
        $body = $this->rest->get($this->userResource . '/' . $username);
        $status = $this->rest->status();
        try
        {
            if ($status == 200)
            {
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
            else
            {
                $this->setError($body);
                throw new Exception($body);
            }
        }
        catch (Exception $e)
        {
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
