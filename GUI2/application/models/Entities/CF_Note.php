<?php
class CF_Note {

    var $host;
    var $noteId;
    var $userId;
    var $reportType;
    var $message;
    var $report;
    var $date;
    var $reportTypeMap = array(
        '1' => 'Hosts',
        '2' => 'Promise repaired log',
        '3' => 'Promises repaired summary',
        '4' => 'Performance',
        '5' => 'Business Value Report',
        '6' => 'File Change Log',
        '7' => 'Bundle Profile',
        '8' => 'Promise Not Kept Log'
    );

    function __construct($params=array()) {
        $this->host = isset($params['hostname']) ? $params['hostname'] : null;
        $this->noteId = isset($params['noteId']) ? $params['noteId'] : null;
        $this->userId = isset($params['user']) ? $params['user'] : null;
        $this->message = isset($params['message']) ? $params['message'] : null;
        $this->reportType = isset($params['report_type']) ? $params['report_type'] : null;
        $this->report = isset($params['report']) ? $params['report'] : null;
        $this->date = isset($params['date']) ? $params['date'] : null;
    }

    function getUserId() {
        return $this->userId;
    }

    function getMessage() {
        return $this->message;
    }

    function getDate() {
        return date('D F d h:i:s Y', $this->date);
    }

    function getReportType() {
        return $this->reportTypeMap[$this->reportType];
    }

}

?>