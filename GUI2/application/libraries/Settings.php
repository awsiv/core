<?php
if (!defined('BASEPATH'))
    exit('No direct script access allowed');

/**
 * To retrive specific things from app settings and user settings
 */
class Settings {
  protected $errors = array();
  protected $username;
  public function __construct()
	{
		$this->ci =& get_instance();
                $this->ci->load->model('settings_model');
                $this->username=$this->ci->session->userdata('username');
        }

  public function get_backend_mode(){
      return $this->ci->settings_model->app_settings_get_item('mode');
    }

  public function get_tooltips_status(){
          $value=$this->ci->settings_model->user_settings_get_item($this->username,'toottips');
          if($value!==False && $value=='tips_on'){
              return true;
          }
          return false;
  }

  public function get_no_of_rows(){
        $value=$this->ci->settings_model->user_settings_get_item($this->username,'num_rows');
        if($value!==False ){
            return $value;
          }
        return false;
  }

  //for testing
  public function set_username($username){
      $this->username=$username;
  }
    
}
?>
