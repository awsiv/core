<?php

require_once APPPATH . 'models/Entities/CF_tracker.php';

class Tracker_model extends CI_Model
{

    var $collectionName = 'trackers';
    var $errors;

    function __construct()
    {
        parent::__construct();
    }

    function validate($data)
    {
        $valid = true;
        if (trim($data['resource']) == '')
        {
            $valid = false;
            $this->setError('Provide a resource to track');
        }
      
        if (trim($data['dateTimeStamp']) == '' || !is_integer($data['dateTimeStamp']))
        {
            $valid = false;
            $this->setError('Provide a valid start datetime  to track');
        }
        
        if (trim($data['reportType']) == '')
        {
            $valid = false;
            $this->setError('Provide a report type');
        }

        if (trim($data['userName']) == '')
        {
            $valid = false;
            $this->setError('username is empty');
        }
        
         if (trim($data['trackerName']) == '')
        {
            $valid = false;
            $this->setError('Provide a name to this tracker');
        }
        
        //for unique tracker name for each user
        $result=$this->get_all_tracker(array('userName'=>$data['userName'],'trackerName'=>$data['trackerName']));
        if(!empty($result))
        {
            $valid=false;
            $this->setError('Tracker name already used');
        }

        return $valid;
    }

    /**
     *
     * @param type $data an array of input values
     * @return CF_tracker 
     */
    function insert($data)
    {
        if (!is_array($data))
        {
            $this->setError('Invalid parameter passed to function');
            return false;
        }

        if ($this->validate($data))
        {
            $id = $this->mongo_db->insert($this->collectionName, $data);
            if ($id)
            {
                // insert success
                $Obj = new CF_tracker($data);
                return $Obj;
            }
        }
      return false;
    }
    
    /**
     *
     * @param type $filter with username, resources etc as array
     * @return a array of CF_tracker 
     */
    function get_all_tracker($filter=array()){
         
        $mongoresult = $this->mongo_db->where($filter)->get($this->collectionName);
        $result = array();
        foreach ((array) $mongoresult as $searchResult)
        {
            $obj = new CF_tracker($searchResult);
            $result[] = $obj;
        }
        return $result;
        
    }
    
    /**
     *
     * @param type $filter
     * @param type $data
     * @returns the result hash containing no of rows modified and the status of operation
     */
    function update_tracker($filter,$data){
        $result=$this->mongo_db->where($filter)->update($this->collectionName,$data);
        return $result;
    }
    
    /**
     *
     * @param type $filter an array of parameters values as filter
     * @return type 
     */
    function delete($filter=array()){
        
        $this->mongo_db->where($filter)->delete_all($this->collectionName);
        $this->mongo_db->clear();
        return;
    }
    
    /**
     *
     * @param type $errorText  for setting the internal errors
     */
    function setError($errorText)
    {

        $this->errors[] = $errorText;
    }
    
    /**
     *
     * @return type  for getting the error occured during crud operation.
     */
    function getErrors()
    {
        return $this->errors;
    }

}

?>
