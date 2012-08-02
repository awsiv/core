<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

/**
 * To retrive specific things from app settings and user settings
 */
class Setting_lib
{

    protected $errors = array();
    protected $username;

    public function __construct()
    {
        $this->ci = & get_instance();
        $this->ci->load->model('settings_model');
        $this->username = $this->ci->session->userdata('username');
    }

    public function get_blue_host_threshold()
    {
        return $this->ci->settings_model->app_settings_get_item('bluehost_threshold_global');
    }

    public function get_experimental_mode()
    {
        return $this->ci->settings_model->app_settings_get_item('experimental') === TRUE;
    }

    public function get_backend_mode()
    {
        return $this->ci->settings_model->app_settings_get_item('mode');
    }

    public function get_tooltips_status()
    {
        $value = $this->ci->settings_model->user_settings_get_item($this->username, 'tooltips');
        if ($value == 'tips_off')
        {
            return false;
        }
        return true;
    }

    public function get_no_of_rows()
    {
        $default_rows=20;
        $value = $this->ci->settings_model->user_settings_get_item($this->username, 'num_rows');
        if ($value !== False || $value > 0)
        {
            return $value;
        }
        return $default_rows;
          
    }

    //for testing
    public function set_username($username)
    {
        $this->username = $username;
    }


    /**
     * Returns the state of rbac
     * (In DB it is set as true or false in string)
     * @return boolean  if it is set returns (bool)true else false
     */
    public function get_rbac_setting() {
       return  $this->ci->settings_model->app_settings_get_item('rbac') === "true" ? true : false; 
   }


    /**
     * get fall_back_for MP setting
     *
     * @return string  (This is username which is set as "admin" example result: admin)
     */
    public function get_fall_back_for()
    {
        return $this->ci->settings_model->app_settings_get_item('fall_back_for');
    }
    
    /**
     *  return current authentication mode.
     * @return type Example results: database, ldap, active_directory
     */
    public function get_authentication_mode() {
        return $this->ci->settings_model->app_settings_get_item('mode');
    }

    
    /**
     *  return setting for business room icon on main page
     * @return type boolean
     */
    public function get_show_business_room_setting() {
        return $this->ci->settings_model->app_settings_get_item('business_room');      
    } 
    
    /**
     *  return setting for planning room icon on main page
     * @return type boolean
     */
    public function get_show_planning_room_setting() {
        return $this->ci->settings_model->app_settings_get_item('planning_room');             
    }    
}
?>