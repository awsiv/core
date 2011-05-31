<?php
require_once APPPATH.'models/Entities/CF_Note.php';
class Note_model extends CI_Model {


    function __construct() {
        parent::__construct();
    }

    function getAllNotes($filter=array()) {

        $hostFilter = isset($filter['hostname']) ? $filter['hostname'] : null;
        $noteId = isset($filter['noteId']) ? $filter['noteId'] : null;
        $userId = isset($filter['userId']) ? $filter['userId'] : null;
        
        $dateFrom = isset($filter['dateFrom']) ? $filter['dateFrom'] : -1;
        $dateTo = isset($filter['dateTo']) ? $filter['dateTo'] : -1;
        $commentsJson = cfpr_query_note($hostFilter, $noteId, $userId, $dateFrom, $dateTo);
        $commentsJson = str_replace(array("\r\n", "\n", "\r"), '<br />', utf8_encode(trim($commentsJson)));
        $comments = json_decode($commentsJson, TRUE);
        $note = array();
        foreach ((array)$comments['data'] as $comment) {
            $note[] = new CF_Note($comment);
        }
        return $note;
    }

    function addNote($nid, $username, $date, $message) {
        return cfpr_add_note($nid, $username, $date, $message);
    }

    function addNewNote($keyhash, $rid, $report_type, $username, $date, $message) {
        return cfpr_new_note($keyhash, $rid, $report_type, $username, $date, $message);
    }

}

?>
