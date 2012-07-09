<?php

class Authentication_model extends Cf_Model
{
    protected $userResource= 'user';
    protected $baseResource= 'api';
    protected $rest='';
    
      public function __construct()
    {
        parent::__construct();
        $this->load->config('ion_auth', TRUE);
        $this->load->helper('cookie');
        $this->load->helper('date');
        $this->load->library('mongo_db');
    }
    
    
    public function setRestClient($rest){
        $this->rest=$rest;
         //var_dump($rest);

    }
    /**
     * For login operation
     * @param type $username
     * @param type $password
     * @param type $remmember
     * @return type 
     */
    function login($identity, $password, $mode='basic'){
        $this->rest->setCredentials($identity,$password);
        $this->rest->setAuthMode($mode);
        $body=$this->rest->get('');
        $status = $this->rest->status();
        if($status==200){
            $this->update_last_login($username);
            return true;
        }else{
            $this->setError($body);
            return false;
        }
    }
    
    
    
    /**
     * Generate the details for given object i.e user
     * @param type $username
     * @param type $password
     * @param type $object
     * @return type 
     */
    function getUserDetails($username,$password,$object=NULL){
        $user=$object==NULL?$username:$object;
        $this->rest->initialize(array('server' => $this->apiServer, 'http_auth' => $this->http_auth, 'http_user'=>$username,'http_pass'=>$password));
        $body=$this->rest->get($this->baseResource.'/'.$this->userResource.'/'.$user);
        $status = $this->rest->status();
        try{
            if($status==200){
               $data=$this->checkData($body);
                   if (is_array($data) && $this->hasErrors() == 0)
                    {
                        return $data['data'];
                    }
                    else
                    {
                        throw new Exception($this->getErrorsString());
                    }
            }else{
                $this->setError($body);
                throw new Exception($body);
            }
        }catch(Exception $e){
           log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
    /**
     *Update the last login time for user in local db related to the user.
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
    
    private function remember_user($user,$password)
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
