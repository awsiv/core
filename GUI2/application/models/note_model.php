<?php
require_once APPPATH.'models/Entities/CF_Note.php';
class Note_model extends CI_Model {

    var $totalResults = 0;
    function __construct() {
        parent::__construct();
    }

    function getAllNotes($filter=array()) {

        $hostFilter = isset($filter['hostname']) ? $filter['hostname'] : null;
        $noteId = isset($filter['noteId']) ? $filter['noteId'] : null;
        $userId = isset($filter['userId']) ? $filter['userId'] : null;
        
        $dateFrom = isset($filter['dateFrom']) ? $filter['dateFrom'] : -1;
        $dateTo = isset($filter['dateTo']) ? $filter['dateTo'] : -1;
        $noOfRows = isset($filter['noOfRows']) ? $filter['noOfRows'] : 0;
        $pageNo = isset($filter['pageNo']) ? $filter['pageNo'] : 0;
        
        
        $commentsJson = cfpr_query_note($hostFilter, $noteId, $userId, $dateFrom, $dateTo,$noOfRows,$pageNo);
        $commentsJson = str_replace(array("\r\n", "\n", "\r"), '<br />', utf8_encode(trim($commentsJson)));
        $comments = json_decode($commentsJson, TRUE);
        $note = array();
        foreach ((array)$comments['data'] as $comment) {
            $note[] = new CF_Note($comment);
        }
        
        // set the total returned by the query 
        $this->totalResults = $comments['meta']['count'];
        return $note;
    }
    
    function getTotalNoteCount() {
        return $this->totalResults;
    }
    
    

    function addNote($nid, $username, $date, $message) {
        return cfpr_add_note($nid, $username, $date, $message);
    }

    function addNewNote($keyhash, $rid, $report_type, $username, $date, $message) {
        return cfpr_new_note($keyhash, $rid, $report_type, $username, $date, $message);
    }

}

?>
