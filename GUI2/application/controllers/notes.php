<?php
class Notes extends Cf_Controller {

    function __construct() {
        parent::__construct();
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
            $comments = (cfpr_query_note($hostkey, $nid, '', -1, -1));
            $comments = utf8_encode($comments);
            $this->data['data'] = json_decode($comments, TRUE);
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
            $ret = cfpr_add_note($nid, $username, $date, $message);

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

        $comments = (cfpr_query_note(NULL, $nid, '', -1, -1));
        $comments = utf8_encode($comments);
        $this->data['data'] = json_decode($comments, TRUE);
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
            $ret = cfpr_new_note($keyhash, $rid, $report_type, $username, $date, $message);
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
        $this->data['reporttype'] = $rid;

        $this->data['form_url'] = '/notes/addnote';

        $comments = (cfpr_query_note(NULL, $ret, '', -1, -1));
        $comments = utf8_encode($comments);

        $this->data['data'] = json_decode($comments, TRUE);
        $this->load->view('/notes/view_notes', $this->data);

        

    }

    function showNotes() {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');


        $hostFilter = $this->input->post('hostname', TRUE);
        $noteId = '';
        $userId = $this->input->post('username', TRUE);
        $dateFrom = $this->input->post('date_from', TRUE) ? strtotime($this->input->post('date_from', TRUE)) : -1;
        $dateTo = $this->input->post('date_to', TRUE) ? strtotime($this->input->post('date_to', TRUE)) : -1;


        if ($dateFrom && $dateFrom != -1) {
            //we have a start date so set the end date as well
            if ($dateTo == -1) {
                $dateTo = time();
            }
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

        $comments = cfpr_query_note($hostFilter, $noteId, $userId, $dateFrom, $dateTo);
        $comments = utf8_encode($comments);
        $data['data'] = json_decode($comments, TRUE);
        $this->template->load('template', '/notes/show_notes', $data);
    }

}

?>