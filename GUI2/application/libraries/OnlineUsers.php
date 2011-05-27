<?php
/**
 * Users Online class
 *
 * Manages active users
 *
 * @package		CodeIgniter
 * @subpackage	Libraries
 * @category	Add-Ons
 * @author		Leonardo Monteiro Bersan de Araújo
 * @link		hhttp://codeigniter.com/wiki/Library: Online_Users
 * @modified    sudhir pandey
 */

error_reporting(E_ALL);

class OnlineUsers{
	var $file;
	var $data;
	var $ip;
	var $CI;
        function  __construct() {
            $this->ip=$_SERVER['REMOTE_ADDR'];
            $this->CI=& get_instance() ;
            $this->CI->load->library(array('mongo_db','session'));

        }
	function OnlineUsers(){
                
               $this->data=$this->CI->mongo_db->where(array('user_id'=>$this->CI->session->userdata('user_id')))->get('onlineusers');
               
		
		$timeout = time()-120;
		
		//If it's the first hit, add the information to database
               if(!$this->data)
                        {
                        
                        $this->data=array();
                        $this->data['ip']=$this->ip;
                        $this->data['user_id']=$this->CI->session->userdata('user_id');
			$this->data['time'] = time();
			$this->data['uri'] = $_SERVER['REQUEST_URI'];
			//Loads the USER_AGENT class if it's not loaded yet
			if(!isset($this->CI->agent)) { $this->CI->load->library('user_agent'); $class_loaded = true; }
			if($this->CI->agent->is_robot())
				$this->data['bot'] = $this->CI->agent->robot();
			else
				$this->data['bot'] = false;
			
			//Destroys the USER_AGENT class so it can be loaded again on the controller
			if($class_loaded) unset($class_loaded, $this->CI->agent);
                        $this->CI->mongo_db->insert('onlineusers', $this->data);
		}
		else
		{
                        $this->CI->mongo_db->where(array('user_id'=>$this->CI->session->userdata('user_id')));
		        $this->CI->mongo_db->update('onlineusers', array('ip'=>$this->ip,'time' => time(),'uri'=>$_SERVER['REQUEST_URI']));
                        $this->CI->mongo_db->clear();
		}
		
		//Removes expired data
                $this->CI->mongo_db->where_lte('time',$timeout)->delete('onlineusers');
		
		
	}
	
	//this function return the total number of online users
	function total_users(){
		//return count($this->data);
                return $this->CI->mongo_db->count('onlineusers');
	}
	//this function return the total number of online robots
	function total_robots(){
		$i=0;
		foreach($this->data as $value)
		{
			if($value['is_robot']) $i++;
		}
		return $i;
	}
	
	//Used to set custom data
	function set_data($data=false, $force_update=false){
		if(!is_array($data)){ return false;}

		$tmp=false; //Used to control if there are changes

		foreach($data as $key => $value)
		{
			if(!isset($this->data[$this->ip]['data'][$key]) || $force_update)
			{
				$this->data[$this->ip]['data'][$key] = $value; 
				$tmp=true;
			}
		}
		
		//Check if the user's already have this setting and skips the wiriting file process (saves CPU)
		if(!$tmp) return false;
		return $this->_save();
	}	
	//
	function get_info(){
		return @$this->data;
	}	
	
	//Save current data into file
	function _save()
	{
		$fp = fopen($this->file,FOPEN_WRITE_CREATE);
		flock($fp, LOCK_EX);
		//$write = fwrite($fp, serialize($this->data));
                $write=fwrite($fp,json_encode($this->data));
		fclose($fp);
		//var_dump($this->data);
		return $write;
	}
}