<?php
/**
 * This model is for saving the relevant user information in Database related to mission portal
 * Most propably book keeping task like , last login and other information is kept here.
 */
class authentication_model_mongo extends CI_Model
{
  public function __construct()
    {
        parent::__construct();
        $this->load->library('mongo_db');
        $this->load->library('session');
    }
    
    public function get_user($username,$mode)
    {
        return $this->mongo_db->get_where_object('users', array('username' => $username, 'source'=>$mode), 1);
    }
    
    public function is_first_login($username,$mode)
    {
        $user = $this->get_user($username,$mode);
        if ($user === NULL)
        {
            return true;
        }
        return false;
    }
    
     public function update_last_login($username,$mode)
    {
        $this->load->helper('date');
        $this->mongo_db->where(array('username' => $username,'source'=>$mode));
        $this->mongo_db->update('users', array('last_login' => now()));
        return TRUE;
    }
    
    public function add_last_login($username,$mode)
    {
        $this->load->helper('date');
        $data=array(
                 "username"=>$username,
                 "source"=>$mode,
                 "last_login"=>now()
        );
        $this->mongo_db->where(array('username' => $username,'mode'=>$mode));
        $id = $this->mongo_db->insert('users', $data);
        return TRUE;
    }
    
    
}
?>
