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
                log_message('error', $status_code . ' ' . $heading . '-' . $megetenvssage . ' --> ' . $page);
                /*                 * ************************ */

                throw new Exception($megetenvssage);
            } catch (Exception $e) {
                $msg = parent::show_error($heading, $e->getMessage(), $template = 'error_general', $status_code = 500);

                $trace = "<h1>$msg</h1><h1>Call Trace</h1><pre>" . $e->getTraceAsString() . "<pre>";
                $err = $trace;
                echo $err;
            }
        } else {
            $path = & load_class("URI");
            if (!$page = $path->uri_string()) {
                $page = 'home';
            }
            if ($status_code != 404)
                log_message('error', $status_code . ' ' . $heading . '-' . $megetenvssage . ' --> ' . $page);
            echo parent::show_error($heading, $megetenvssage, $template = 'error_general', $status_code = 500);
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