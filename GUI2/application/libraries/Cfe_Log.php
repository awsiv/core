<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

class Cfe_Logs extends CI_Log {

    protected $mongo;

    public function __construct() {
        parent::__construct();
        //$this->load->library('mongo_db');
        $this->env = ENVIRONMENT;
        if ($this->env == "development") {
            $this->_threshold = 2;
            $this->_levels = array('ERROR' => '1', 'INFO' => '2', 'DEBUG' => '3', 'ALL' => '4');
        } else {
            $this->_threshold = 4;
        }
        try {
            $this->mongo = new Mongo("mongodb://".HUB_MASTER.":27017", array("persist" => "ci_mongo_persist"));
        } catch (Exception $e) {
            die('Could not connect to the MongoDB database in hub master. Please ensure that CFEngine Enterprise is running correctly, by issuing the following command on the hub.

            # /var/cfengine/bin/cf-twin -Kf failsafe.cf Hub master appears to be '.HUB_MASTER);
            exit();
        }
        $this->_enabled = true;
    }

    function write_log($level = 'error', $msg, $php_error = FALSE) {
        if ($this->_enabled === FALSE) {
            return FALSE;
        }

        $level = strtoupper($level);

        if (!isset($this->_levels[$level]) || ($this->_levels[$level] > $this->_threshold)) {
            return FALSE;
        }
        $db = $this->mongo->phpcfengine->app_logs;
        //$output=$this->mongo_db->insert('app_logs',array(
        try {
            $output = $db->insert(array(
                        // Server Info
                        'server_name' => $_SERVER['SERVER_NAME'],
                        'server_ip' => (isset($_SERVER['SERVER_ADDR'])) ? $_SERVER['SERVER_ADDR'] : '0.0.0.0',
                        'domain' => (!empty($_SERVER['HTTP_HOST'])) ? $_SERVER['HTTP_HOST'] : '',
                        //User Info
                        'user_agent' => (!empty($_SERVER['HTTP_USER_AGENT'])) ? $_SERVER['HTTP_USER_AGENT'] : '',
                        'ip_address' => (!empty($_SERVER['REMOTE_ADDR'])) ? $_SERVER['REMOTE_ADDR'] : '',
                        'uri' => (!empty($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : '',
                        'query_string' => (!empty($_SERVER['QUERY_STRING'])) ? $_SERVER['QUERY_STRING'] : '',
                        'timestamp' => date($this->_date_fmt),
                        'message' => $msg,
                        'level' => $level,
                    ));
        } catch (Exception $e) {
            show_error('Something went wrong while trying to write the logs in MongoDB. Please make sure MongoDB is running correctly',500);
        }
        return $output;
    }

}

/* End of file Cf_Log.php */
/* Location: ./application/libraries/Cf_Log.php */