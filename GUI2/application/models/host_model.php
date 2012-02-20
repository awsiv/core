<?php

class host_model extends Cf_Model {
    
   function getRedHost($username,$rows=10,$page=1){
   try {
            $rawdata = cfpr_host_compliance_list_red($username,$rows,$page);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage());
            throw $e;
        }
    }
    
   function getGreenHost($username,$rows=10,$page=1){
   try {
            $rawdata = cfpr_host_compliance_list_green($username,$rows,$page);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage());
            throw $e;
        }
    }
    
  function getYellowHost($username,$rows=10,$page=1){
   try {
            $rawdata = cfpr_host_compliance_list_yellow($username,$rows,$page);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage());
            throw $e;
        }
    }
    
  function getBlueHost($username,$rows=10,$page=1){
   try {
            $rawdata = cfpr_host_compliance_list_blue($username,$rows,$page);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            generate_errormessage($e);
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $ipregx
     * @param type $rows
     * @param type $page
     * @return type array data
     */
    function getHostByIP($username,$ipregx,$rows=15,$page=1){
        try {
            $rawdata = cfpr_host_list_by_ip_rx(NULL, $ipregx, $rows, $page);
            $data = sanitycheckjson($rawdata,true);
            if (is_array($data)) {
                return $data ;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION -->".$e->getFile()." line:">$e->getLine());
            throw $e;
        }   
    }
    
    
    
    /**
     *
     * @param type $username
     * @param type $hostregx
     * @param type $rows
     * @param type $page
     * @return type array
     */
    
     function getHostByName($username,$hostregx,$rows=15,$page=1){
        try {
            $rawdata = cfpr_host_list_by_name_rx($username, $hostregx, $rows, $page);
            $data = sanitycheckjson($rawdata,true);
            if (is_array($data)){
                return $data ;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }   
    }
    /**
     *
     * @param type $username
     * @param type $hostKey
     * @return type string
     */
    function getHostColor($username,$hostKey){
        try {
            $rawdata = cfpr_host_compliance_colour($username, $hostKey);
            if (is_string($rawdata)){
                return $rawdata ;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }   
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type string hostname for a given key
     */
    function getHostName($username,$hostkey){
         try {
            $rawdata = cfpr_host_by_hostkey($username, $hostkey);
            $data=$this->checkData($rawdata);
            if ($data){
                return $data[0];
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type String Ip address for given hostkey 
     */
    
    function getHostIp($username,$hostkey){
        try {
            $rawdata = cfpr_host_by_hostkey($username, $hostkey);
            $data=$this->checkData($rawdata);
            if ($data){
                return $data[1];
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type timestamp
     */
    function getLastUpdate($username,$hostkey){
        try {
            $rawdata =  cfpr_getlastupdate($username, $hostkey);
            $data=$this->checkData($rawdata);
            if ($data){
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $scope
     * @param type $lval
     * @return type depends on the variable type
     */
    function getHostVariable($username,$hostkey,$scope,$lval){ 
        try {
            $rawdata =  cfpr_report_vars($username, $hostkey, $scope, $lval, NULL, NULL, false, NULL, NULL, "var-name", 1, 1);
            $data=$this->checkData($rawdata);
            if ($data){
                return $data[$scope]['data'][0][3];
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type string
     */
    function getNetworkSpeed($username,$hostkey){
         try {
            $rawdata = cfpr_network_speed($username, $hostkey);
            $data=$this->checkData($rawdata);
            if ($data){
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." CFMOD EXCEPTION");
            throw $e;
        }
    }
   
}
?>
