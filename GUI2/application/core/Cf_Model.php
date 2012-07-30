<?php

class Cf_Model extends CI_Model {

    
    const WARNING = 'WARNING';
    const ERROR = 'ERROR';
    
    protected $errors = array();
    protected $rest ='';

    public function __construct() {
        parent::__construct();
    }

    function setError($error,$errID = null, $level=self::ERROR) {
        $this->errors[$level][$errID] = $error;
    }

    function getErrors($level=self::ERROR) {
        return isset($this->errors[$level]) ? $this->errors[$level] : array();
    }
    
    function getErrorsString($level=self::ERROR){
         return isset($this->errors[$level]) ? implode(',', $this->errors[$level]) : $this->lang->line('unknown_error');
    }

    /**
     * 0 if no errors
     * @return int  
     */
    function hasErrors($level=self::ERROR) {
        return isset($this->errors[$level]) ? count($this->getErrors($level)) : 0;
    }

    /**
     * Clear the error array
     */
    function clearErrors() {
        $this->errors = array();
    }
    
    /**
     * For setting the rest Client library to make rest calls
     * @param type $rest 
     */
     public function setRestClient($rest)
    {
        $this->rest = $rest;
    }

    /**
     * Checks for valid JSON data and if no error parameter is set
     * @param array $data parsed else false if error 
     */
    function checkData($data) {
        $this->clearErrors(); //make sure that no error exist as only one instance is loaded.
        $data_check = json_decode($data, true);
        $returnVal = true;
        if (function_exists("json_last_error")) {
            if (json_last_error() != 0) {
                if(ENVIRONMENT=='development'){
                  $this->_jsoncause(json_last_error());
                }
                $this->setError( $this->lang->line('invalid_json')." DATA is ::". $data);
                $returnVal = false;
            }
         } else {
            if ($data_check === NULL) {
                $this->setError($this->lang->line('invalid_json')."DATA ::".$data);
                $returnVal = false;
            }
        }
        if ($returnVal) {
            /* check for the error flag set from the core.
              [error] => Array
              (
              [errid] => 0
              [msg] => Success
              ) */
            
            //skipped it temporary as invalid status is being reported from core.
           /* if (isset($data_check['error'])) {

                if ($this->_is_warning($data_check['error']['errid'])) {
                    $this->setError($data_check['error']['msg'],$data_check['error']['errid'],  self::WARNING);
                    $returnVal = $data_check;
                } else if ($this->_is_error($data_check['error']['errid'])) {
                    $this->setError($data_check['error']['msg'],$data_check['error']['errid']);
                    $returnVal = $data_check;
                }
            }*/
        }
        if ($returnVal) {
            $returnVal = $data_check;
        }

        return $returnVal;
    }

    function _is_warning($errId) {
        return ($errId >= 100 && $errId < 199);
    }

    function _is_error($errId) {
        return ($errId >= 1 && $errId < 99);
    }
    
    function _jsoncause($code){
       switch (json_last_error()) {
                case JSON_ERROR_NONE:
                echo ' - No errors';
                break;
                case JSON_ERROR_DEPTH:
                echo ' - Maximum stack depth exceeded';
                break;
                case JSON_ERROR_STATE_MISMATCH:
                echo ' - Underflow or the modes mismatch';
                break;
                case JSON_ERROR_CTRL_CHAR:
                echo ' - Unexpected control character found';
                break;
                case JSON_ERROR_SYNTAX:
                echo ' - Syntax error, malformed JSON';
                break;
                case JSON_ERROR_UTF8:
                echo ' - Malformed UTF-8 characters, possibly incorrectly encoded';
                break;
                default:
                echo ' - Unknown error';
                break;
         }
    }

}

?>
