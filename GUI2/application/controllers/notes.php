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
        $rid = isset($params['rid']) ? urldecode(base64_decode($params['rid'])) : "";
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
        $message = $this->input->post('Message');
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
        $message = $this->input->post('Message');
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

}

?>
