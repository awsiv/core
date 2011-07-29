<?php

class Cf_Exceptions extends CI_Exceptions {

    public $env;

    public function __construct() {
        parent::__construct();
        $this->env = ENVIRONMENT;
    }

    public function show_error($heading, $megetenvssage, $template = 'error_general', $status_code = 500) {

        
        if ($this->env == "development") {
            try {
                switch ($status_code) {
                    case 403: $heading = 'Access Forbidden';
                        break;
                    case 404: $heading = 'Page Not Found';
                        break;
                    case 503: $heading = 'Undergoing Maintenance';
                        break;
                }
                //only for development
                $path = & load_class("URI");
                if (!$page = $path->uri_string()) {
                    $page = 'home';
                }

                /*                 * ************************ */

                throw new Exception($megetenvssage);
            } catch (Exception $e) {
                $trace = "<h1>Call Trace</h1><pre>" . $e->getTraceAsString() . "<pre>";
                $err = $trace;
                $errorMessage =  $e->getMessage() . $err ;
                
                echo  parent::show_error($heading,$errorMessage, $template = 'error_general', $status_code);

                
                log_message('error', $status_code . ' ' . $heading . '-' . $megetenvssage . ' --> ' . $page);
                
            }
        } else {
            $path = & load_class("URI");
            if (!$page = $path->uri_string()) {
                $page = 'home';
            }
            echo parent::show_error($heading, $megetenvssage, $template = 'error_general', $status_code);

            if ($status_code != 404) {
                log_message('error', $status_code . ' ' . $heading . '-' . $megetenvssage . ' --> ' . $page);
            }
        }
    }

    function show_php_error($severity, $message, $filepath, $line) {
        if ($this->env == "development") {
            //parent::show_php_error($severity, $message, $filepath, $line);
            try {
                ob_start();
                parent::show_php_error($severity, $message, $filepath, $line);
                $str = ob_get_contents() . $this->env;
                ob_end_clean();
                throw new Exception($str);
            } catch (Exception $ex) {
                $msg = $ex->getMessage();
                $trace = "<h1>Call Trace</h1><pre>" . $ex->getTraceAsString() . "<pre>";
                $err = str_replace('</div>', $trace . '</div>', $msg);
                echo $err;
            }
        } else {
            //php errors are logged by default
            //$heading="Internal server Error ".$this->env;
            //$message="Problem with in the internal operation of server";
            //$str=parent::show_error($heading, $message, $template = 'error_general', $status_code = 500);
            //log_message('Error', $message . ' --> '. $filepath.'--->'.$line.'--'.$severity);
            //echo $str;
        }
    }

}