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
        '7' => 'File Change Diffs',
        '8' => 'Bundle Profile',
        '9' => 'Promise Not Kept Log'
    );

    function __construct($params=array()) {
        $this->host = isset($params['host']) ? $params['host'] : null;
        $this->noteId = isset($params['noteId']) ? $params['noteId'] : null;
        $this->userId = isset($params['user']) ? $params['user'] : null;
        $this->message = isset($params['message']) ? $params['message'] : null;
        $this->reportType = isset($params['report_type']) ? $params['report_type'] : null;
        $this->report = isset($params['report']) ? $params['report'] : null;
        $this->date = isset($params['date']) ? $params['date'] : null;
    }

    /*
     * Returns array with host details   
     */
    function getHost() {
        return $this->host;
    }

    function getUserId() {
        return $this->userId;
    }

    function getMessage() {
        return $this->message;
    }

    function getDate() {
        return getDateStatus($this->date, true);
    }

    function getReportType() {
        return $this->reportTypeMap[$this->reportType];
    }

    /*
     * get the report its a json array
     */

    function getReport() {
        $r = array();
        if (!$this->report)
            return $r;
        $e = explode(',', $this->report);
        foreach ((array) $e as $item) {
            $tempexplode = array_map('trim', explode(':', $item));
            if ($tempexplode[0] === 'Time')
                $tempexplode[1] = getDateStatus($tempexplode[1], true);
            $r[$tempexplode[0]] = $tempexplode[1];
        }


        return $r;
    }

}

?>