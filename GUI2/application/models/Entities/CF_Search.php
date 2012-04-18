<?php

class CF_Search {

    var $username;
    var $reportType;
    var $url;
    var $label;
    var $date;
    var $id;
    var $params;
    function __construct($params=array()) {
        $this->id = isset($params['_id']) ? $params['_id']->__toString() : null;
        $this->username = isset($params['username']) ? $params['username'] : null;
        $this->url = isset($params['url']) ? ($params['url']) : null;
        $this->reportType = isset($params['reportType']) ? $params['reportType'] : null;
        $this->label = isset($params['label']) ? $params['label'] : null;
        $this->date = isset($params['date']) ? $params['date'] : null;
        $this->params=isset($params['params']) ? $params['params'] : null;
    }

    function getUsername() {
        return $this->username;
    }

    function getUrl() {
        return site_url().'/'.$this->url;
    }

    function getDate() {
        return date('D F d h:i:s Y', $this->date);
    }

    function getReportType() {
        return $this->reportType;
    }

    function getLabel() {
        return $this->label;
    }

    function getId() {
        return $this->id;
    }
    /**
     * Returns the parameter form the url that is constructed
     * return @array with key as parameter and value as the searched value, if nothing returns empty array 
     */
    function getParameters() {
       
        $params = $this->params;
        return $params;
        
    }

}

?>