<?php

class Notes extends Cf_Controller {

    function __construct() {
        parent::__construct();
        $this->load->model('note_model');
        $this->load->library('form_validation');
        $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
    }

    function index() {

        $params = $this->uri->uri_to_assoc(3);
        $action = isset($params['action']) ? $params['action'] : "";
        $hostkey = isset($params['hostkey']) ? $params['hostkey'] : NULL;
        $this->data = array();
        $rid = isset($params['rid']) ? urldecode(($params['rid'])) : "";
        $reportType = isset($params['reporttype']) ? $params['reporttype'] : "";
        $hostKey = isset($params['hostkey']) ? $params['hostkey'] : "";
        $nid = isset($params['nid']) ? $params['nid'] : "";

        $this->data['nid'] = $nid;
        $this->data['rid'] = $rid;
        $this->data['hostkey'] = $hostKey;
        $this->data['reporttype'] = $reportType;

        if ($action == "show") {

            $filter = array('hostname' => $hostkey,
                'noteId' => $nid,
                'userId' => NULL,
                'dateFrom' => -1,
                'dateTo' => -1
            );
            $this->data['data'] = $this->note_model->getAllNotes($filter);
            $this->data['form_url'] = '/notes/addnote';
        } else if ($action == "add") {
            $this->data['data'] = array();
            $this->data['form_url'] = '/notes/addnewnote';
        }
        $this->load->view('/notes/view_notes', $this->data);
    }

    function addnote() {

        $nid = $this->input->post('nid');
        $message = trim($this->input->post('Message'));
        $username = $this->session->userdata('username');
        $date = strtotime("now");
        $ret = false;
        $this->data['nid'] = $nid;
        $this->data['rid'] = '';
        $this->data['hostkey'] = '';
        $this->data['reporttype'] = '';
        $this->data['form_url'] = '/notes/addnote';


        if (trim($message) != null) {
            $ret = $this->note_model->addNote($nid, $username, $date, $message);

// return the same view with comments
            if (!$ret) {
// SOMETHING WENT WRONG WHILE ADDITION
                $this->output->set_status_header('400', 'Cannot insert the note.');
                echo $ret;
                exit;
            }
        } else {
            $this->data['updateMessage'] = "Cannot insert empty message.";
        }

        $filter = array('hostname' => NULL,
            'noteId' => $nid,
            'userId' => NULL,
            'dateFrom' => -1,
            'dateTo' => -1
        );

        $this->data['data'] = $this->note_model->getAllNotes($filter);
        $this->load->view('/notes/view_notes', $this->data);
        return;
    }

    function addnewnote() {

        $rid = trim($this->input->post('rid')) ? $this->input->post('rid') : "none";
        $message = trim($this->input->post('Message'));
        $report_type = trim($this->input->post('reporttype')) ? $this->input->post('reporttype') : 1;
        $keyhash = $this->input->post('hash');
        $username = $this->session->userdata('username');
        $date = strtotime("now");
        $ret = false;
        if (trim($message) != null) {
            $ret = $this->note_model->addNewNote($keyhash, $rid, $report_type, $username, $date, $message);
            if (!$ret) {
                // SOMETHING WENT WRONG WHILE ADDITION
                $this->output->set_status_header('400', 'Cannot insert the note.');
                echo $ret;
                exit;
            }
        } else {
            $this->data['updateMessage'] = "Cannot insert empty message.";
        }

        $this->data['nid'] = $ret;
        $this->data['rid'] = $rid;
        $this->data['hostkey'] = $keyhash;
        $this->data['reporttype'] = $report_type;

        $this->data['form_url'] = '/notes/addnewnote';


        $filter = array('hostname' => NULL,
            'noteId' => $ret,
            'userId' => NULL,
            'dateFrom' => -1,
            'dateTo' => -1
        );

        // if we have valid insert get that notes else leave it blank 
        if ($ret) {
            $this->data['form_url'] = '/notes/addnote';
            $this->data['data'] = $this->note_model->getAllNotes($filter);
        } else {
            $this->data['data'] = array();
        }

        $this->load->view('/notes/view_notes', $this->data);
    }

    function showNotes() {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');
        $params = $this->uri->uri_to_assoc(3);
        
        
        $userId = ($this->input->post('username', TRUE) !== FALSE) ? $this->input->post('username', TRUE) :  $this->session->userdata('search_username');
        $dateFrom = ($this->input->post('date_from', TRUE)!== FALSE) ? strtotime($this->input->post('date_from', TRUE)) : $this->session->userdata('search_datefrom');
        $dateTo = ($this->input->post('date_to', TRUE)!== FALSE) ? strtotime($this->input->post('date_to', TRUE)) : $this->session->userdata('search_dateto');

        $searchSession = array(
            'search_username' => $userId,
            'search_dateFrom' => ($dateFrom  !== FALSE )? $dateFrom : -1,
            'search_dateto' =>($dateTo  !== FALSE) ? $dateTo : -1
        ); 
         
        $this->session->set_userdata($searchSession);
        
        
        

        if ($searchSession['search_dateFrom'] && $searchSession['search_dateFrom'] != -1) {
        //we have a start date so set the end date as well
            if ($searchSession['search_dateto'] == -1) {
                $searchSession['search_dateto'] = time();
            }
        }
        
        
         if ($searchSession['search_dateFrom'] == -1 && $searchSession['search_dateto'] != -1 ) {
            $searchSession['search_dateFrom']= 0;
        }
        
        

        $bc = array(
            'title' => 'Notes',
            'url' => 'notes/shownotes',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "Notes overview",
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        
        $data['currentPage'] = isset($params['page']) ? intval($params['page'], 10) : 1;
        $filter = array(
            'userId' =>  $searchSession['search_username'],
            'dateFrom' => $searchSession['search_dateFrom'],
            'dateTo' => $searchSession['search_dateto'],
            'noOfRows' => 10,
            'pageNo' => $data['currentPage']
        );


        $comments = $this->note_model->getAllNotes($filter);
        $data['data'] = $comments;
        $data['totalNotes'] = $this->note_model->getTotalNoteCount();
        $data['filters'] = $filter;

        $this->template->load('template', '/notes/show_notes', $data);
    }

}

?>