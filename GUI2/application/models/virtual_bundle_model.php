<?php

class virtual_bundle_model extends Cf_Model {

    /**
     * function to return virtual bundle report  data
     * This returns json string
     * @return json string
     */
    function getVirtualBundleData($virtualBundle=null, $username=null, $recordPerPage=null, $currentPage=1) {


        try {
            $rawdata = cfcon_local_report_virtualbundle($username, $virtualBundle, $recordPerPage, $currentPage);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $rawdata;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage());
            throw $e;
        }
    }

    function getVirtualBundleDetails($handle) {

        try {
            $rawdata = cfcon_local_show_subscription_virtualbundle(NULL, $handle, 1, 1);
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

    function getVirtualBundleSubscriptions($handle) {
        try {
            $rawdata = cfcon_show_subscription_virtualbundle($handle);
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

    function getVirtualBundleStatus($handle) {

        try {
            $rawdata = cfcon_local_report_virtualbundle(NULL, $handle, 1, 1);
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
    
    /**
     * function to create a virtual bundle
     * @return  array
     */
  
    function createVirtualBundle($inputs){
        try{
            $result=cfcon_local_subscribe_virtualbundle($inputs['username'], $inputs['name'], $inputs['description'], $inputs['promises'], $inputs['hostclass']);
            if($result){
                return true;
            }else{
                $this->setError('Cannot create virtual bundle'); 
                return false;
            }   
        }
        catch(Exception $e){
            log_message('error',$e->getMessage());
            throw $e;
        }
      
    }
}

?>