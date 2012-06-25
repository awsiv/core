<?php
/**
 * Entities for the Tracker 
 * 
 */

class CF_tracker{
    var $userName;
    var $trackerName;
    var $reportType;
    var $resource; // can be any promise handle, bundle, class exp etc. but only handle for now
    var $dateTimeStamp;
    var $frequency;
    var $id;
    
    function __construct($params=array())
    {
       $this->id = isset($params['_id']) ? $params['_id']->__toString() : null;
       $this->userName=isset($params['userName']) ? $params['userName'] : null;
       $this->resource=isset($params['resource']) ? $params['resource'] : null;
       $this->dateTimeStamp=isset($params['dateTimeStamp']) ? $params['dateTimeStamp'] : null;
       $this->reportType=isset($params['reportType']) ? $params['reportType'] : null;
       $this->frequency= isset($params['frequency']) ? $params['frequency'] : null;
       $this->trackerName= isset($params['trackerName']) ? $params['trackerName'] : null;
    }
    
    function getUsername() {
        return $this->userName;
    }
    
    function getReportType() {
        return $this->reportType;
    }
    
    function getDateTime(){
        return date('Y-m-d H:i:s', $this->dateTimeStamp);
    }
    
    function getDateTimeStamp(){
        return $this->dateTimeStamp;
    }
    
    function getResource(){
        return $this->resource;
    }
    
    function getFrequency(){
        return $this->frequency;
    }
    
    function getId() {
        return $this->id;
    }
    
    function getName(){
        return $this->trackerName;
    }
}
?>
