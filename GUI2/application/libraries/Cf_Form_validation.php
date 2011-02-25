<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Cf_Form_validation extends CI_Form_validation {
     function __construct()
    {
        parent::__construct();
    }
    
    function unique($value, $params) {

        $CI =& get_instance();
        $CI->load->library('mongo_db');

        $CI->form_validation->set_message('unique',
            'The %s is already being used.');

        list($table, $field) = explode(".", $params, 2);

        $count = $CI->mongo_db->select(array('username'))
                ->where(array($field=>$value))->count($table);

        if ($count>0) {
            return false;
        } else {
            return true;
        }

    }  
    
}
?>
