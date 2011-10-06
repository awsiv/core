<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
Class Log_maintenance {
    function __construct(){
        log_message('debug','Log_Maintenance : class loaded');
        $this->CI =& get_instance();
        // check whether the config is a numeric number before assigning
        $this->log_days_to_keep = (int) (is_numeric($this->CI->config->item('log_days_to_keep')) ? $this->CI->config->item('log_days_to_keep') : 30);
        $this->delete_old_logs(); // delete the old log files
    }

    function delete_old_logs(){

        $dir = ($this->CI->config->item('log_path') != '') ? $this->CI->config->item('log_path') : APPPATH.'logs/';
        // this can be:
        // $dir = ($this->CI->config->item('log_path') ?: APPPATH.'logs/');
        // ternary shorthand if operator (for PHP >= 5.3 only)

        log_message('debug','Log_Maintenance : log dir: '.$dir);
        
        if( ! is_dir($dir) OR ! is_really_writable($dir)){ return false; }

        $deleted = 0;
        $kept = 0;
        
        $files = glob($dir . 'log*'.LOGEXT); // all files in the directory that starts with 'log' and ends with log extension

        foreach($files as $file){ // loop over all matched files
            // check how old they are
            if( filemtime($file) < strtotime('-'.$this->log_days_to_keep.' days') ) { //strtotime('-'.$this->log_days_to_keep.' days')
                unlink($file);
                $deleted++;
            }else{
                $kept++;
            }
        }
        $total = $deleted+$kept;
        if( $deleted>0 ){
            log_message('info', $total.' log files: '.$deleted.' deleted, '.$kept.' kept.');
        }
        $a = array('total'=>$total, 'deleted'=>$deleted, 'kept'=>$kept);
        return $a;
    }
} 
?>