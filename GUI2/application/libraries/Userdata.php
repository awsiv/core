<?php
class Userdata{

    protected $user;
    
    public function __construct()
    {
      $this->ci =& get_instance();
      $this->ci->load->library(array('mongo_db','session'));
      $this->ci->load->helper('date');
      $this->user=$this->ci->session->userdata('username');
    }

    public function insert_personal_working_log($text){
        $id=$this->ci->mongo_db->insert('work_logs',array('username'=>$this->user,'working_on'=>$text,'date'=>now()));
        return $id;
       }

    public function get_personal_working_log_latest($username){
        $worklog=$this->ci->mongo_db->select()
                           ->where(array('username' => $username))
                           ->order_by(array('date'=>'desc'))
                           ->limit(1)
                           ->get_object('work_logs');
        if(is_object($worklog))
        {
        return $worklog->working_on;
        }
        else
        {
            return "No work submitted yet";
        }
    }

    public function get_personal_working_notes($username='',$limit=7)
    {
        if($username==''){
            $username=$this->user
           ;}
    
        $worklog=$this->ci->mongo_db->select()
                           ->where(array('username' => $username))
                           ->order_by(array('date'=>'desc'))
                           ->limit($limit)
                           ->get('work_logs');
        if(is_array($worklog))
        {
          return $worklog;
        }
        else
        {
            return "";
        }
    }
    
}
?>
