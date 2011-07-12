<?php

class CF_Search {

    var $username;
    var $reportType;
    var $url;
    var $label;
    var $date;
    var $id;

    function __construct($params=array()) {
        $this->id = isset($params['_id']) ? $params['_id']->__toString() : null;
        $this->username = isset($params['username']) ? $params['username'] : null;
        $this->url = isset($params['url']) ? $params['url'] : null;
        $this->reportType = isset($params['reportType']) ? $params['reportType'] : null;
        $this->label = isset($params['label']) ? $params['label'] : null;
        $this->date = isset($params['date']) ? $params['date'] : null;
    }

    function getUsername() {
        return $this->username;
    }

    function getUrl() {
        return $this->url;
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

}

?>