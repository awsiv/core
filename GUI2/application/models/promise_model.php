<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 * 
 */

/**
 * Code returned as http status codes
 */
class promise_model extends Cf_Model {
    
    /**
     *
     * @param type $username
     * @param type $handle
     * @return type string
     */
     function getBundleByPromiseHandle($username, $handle)
    {
        try
        {
            $rawdata = cfpr_bundle_by_promise_handle($username, $handle);
            //$data = $this->checkData($rawdata);
            if(is_string($rawdata) && $rawdata!=""){
                return $rawdata;
            }else{
                throw new Exception($this->lang->line('promise_body_null'),204);
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $handle
     * @return type array
     */
    
     function getPromiseDetails($username, $handle)
    {
        try
        {
            $rawdata = cfpr_promise_details($username, $handle);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    function getPromiseListByPromiser($username,$promiser="")
    {
        if($promiser==""){
            return array();
        }
        
        try
        {
            $rawdata =  cfpr_promise_list_by_promiser($username, $promiser);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
   /**
    *
    * @param type $username
    * @param type $type
    * @return type array
    */
            
    function getPromiseListByType($username,$type="")
    {
        if($type==""){
            return array();
        }
        
        try
        {
            $rawdata =   cfpr_promise_list_by_promise_type($username, $type);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $bundle
     * @return type array
     */
   
    function getPromiseListByBundle($username,$bundle)
    {
        if($bundle==""){
            return array();
        }
        
        try
        {   $bundle_type=$this->getBundleType($bundle);
            $rawdata = cfpr_promise_list_by_bundle($username,$bundle_type,$bundle);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * List of promise searching by bundle
     * @param string $username
     * @param string $bundle
     * @return array
     */
    function getPromiseListByBundleRx($username, $bundle)
    {

        try
        {
            $rawdata = cfpr_promise_list_by_bundle_rx($username, $bundle);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * List of promise by handle
     * @param String $username
     * @param string $handle
     * @return array
     */
    function getPromiseListByHandleRx($username, $handle = '.*')
    {
        try
        {
            $rawdata = cfpr_promise_list_by_handle_rx($username, $handle);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

 /**
     * List of promise by promiser
     * @param String $username
     * @param string $promiser
     * @return array
     */
    function getPromiseListByPromiserRx($username, $promiser = null)
    {
        try
        {
            $rawdata =  cfpr_promise_list_by_promiser_rx($username, $promiser);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    
    /**
     *
     * @param type $bundle
     * @return type string
     */
    function getBundleType($bundle)
    {
      $bundle_type=cfpr_get_bundle_type($bundle);
        try
        {   
           $rawdata=cfpr_get_bundle_type($bundle);
           if(is_string($rawdata) && $rawdata!=""){
                return $rawdata;
            }else{
                throw new Exception("Empty type returned for given bundle");
            }
            
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
}
?>
