<?php

class Notes extends Cf_Controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->model('note_model');
        $this->load->library('form_validation', 'Setting_lib');
        $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
    }

    function index()
    {

        $params = $this->uri->uri_to_assoc(3);
        $action = isset($params['action']) ? $params['action'] : "";
        $hostkey = isset($params['hostkey']) ? $params['hostkey'] : NULL;
        $this->data = array();
        $rid = isset($params['rid']) ? urldecode(($params['rid'])) : "";
        $reportType = isset($params['reporttype']) ? $params['reporttype'] : "";
        $hostKey = isset($params['hostkey']) ? $params['hostkey'] : "";
        $nid = isset($params['nid']) ? $params['nid'] : "";
        $username = $this->session->userdata('username');

        $this->data['nid'] = $nid;
        $this->data['rid'] = $rid;
        $this->data['hostkey'] = $hostKey;
        $this->data['reporttype'] = $reportType;

        if ($action == "show")
        {

            $filter = array('hostname' => $hostkey,
                'noteId' => $nid,
                'userId' => NULL,
                'dateFrom' => -1,
                'dateTo' => -1,
                'loggedUser' => $username
            );
            $this->data['data'] = $this->note_model->getAllNotes($filter);
            $this->data['form_url'] = site_url() . '/notes/addnote';
        }
        else if ($action == "add")
        {
            $this->data['data'] = array();
            $this->data['form_url'] = site_url() . '/notes/addnote';
        }
        $this->load->view('/notes/view_notes', $this->data);
    }

    function addnote()
    {

        $nid = $this->input->post('nid') ? $this->input->post('nid') : null;
        $message = trim($this->input->post('Message'));
        $username = $this->session->userdata('username');
        $hostkey = $this->input->post('hostkey');
        $date = strtotime("now");
        $ret = false;
        $this->data['nid'] = $nid;
        $this->data['hostkey'] = $hostkey;
        $this->data['form_url'] = site_url() . '/notes/addnote';


        try
        {
            if (trim($message) != null && (strpos($message, '\\') === false))
            {
                $ret = $this->note_model->addNote($nid, $hostkey, $username, $date, $message);
                // return the same view with comments
                if (!$ret)
                {
                    $this->data['updateMessage'] = "Notes not added";
                }
                else
                {
                    $this->data['nid'] = $ret;
                    $this->data['successMessage'] = "Notes sucessfully added";
                }
            }
            else
            {
                $this->data['updateMessage'] = $this->lang->line('note_empty_insert');
            }
        }
        catch (Exception $e)
        {
            $this->data['updateMessage'] = "Something went wrong while adding notes." . $e->getMessage();
        }

        $filter = array('hostname' => $hostkey,
            'noteId' => $this->data['nid'],
            'userId' => NULL,
            'dateFrom' => -1,
            'dateTo' => -1,
            'loggedUser' => $username
        );

        $this->data['data'] = $this->note_model->getAllNotes($filter);
        $this->load->view('/notes/view_notes', $this->data);
        return;
    }

    function showNotes()
    {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');
        $params = $this->uri->uri_to_assoc(3);
        $loggedUser = $this->session->userdata('username');

        $userId = isset($params['userId']) ? $params['userId'] : $this->input->post('userId', TRUE);
        $dateFrom = isset($params['dateFrom']) ? $params['dateFrom'] : strtotime($this->input->post('dateFrom', TRUE));
        $dateTo = isset($params['dateTo']) ? $params['dateTo'] : strtotime($this->input->post('dateTo', TRUE));

        $bc = array(
            'title' => $this->lang->line('breadcrumb_notes'),
            'url' => 'notes/shownotes',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('notes'),
            'breadcrumbs' => $this->breadcrumblist->display()
        );

        $data['currentPage'] = isset($params['page']) ? intval($params['page'], 10) : 1;   
        $data['number_of_rows'] = isset($params['rows']) ? intval($params['rows'], 10):$this->setting_lib->get_no_of_rows();
        $filter = array(
            'userId' => $userId,
            'dateFrom' => $dateFrom ? $dateFrom : -1,
            'dateTo' => $dateTo ? $dateTo : -1,
            'noOfRows' => $data['number_of_rows'],
            'pageNo' => $data['currentPage'],
            'loggedUser' => $loggedUser
        );

        $comments = $this->note_model->getAllNotes($filter);
        $data['data'] = $comments;
        $data['totalNotes'] = $this->note_model->getTotalNoteCount();
        // change the date for js display
        $data['display_dateFrom'] = ($filter['dateFrom'] != -1) ? date("m/d/Y", $dateFrom) : null;
        $data['display_dateTo'] = ($filter['dateTo'] != -1) ? date("m/d/Y", $dateTo) : null;


        $data['filters'] = $filter;
        $data['pagingParam'] = $this->generatePagingParams($filter);

        $this->template->load('template', '/notes/show_notes', $data);
    }

    /**
     * generate parameters for pagination
     * @param type $filter
     */
    function generatePagingParams($filter)
    {
        $filterString = '';
        $pagingArray = array('userId', 'dateFrom', 'dateTo');
        foreach ($filter as $key => $value)
        {
            if (in_array($key, $pagingArray) && ($value != null))
            {
                $filterString .= '/' . $key . '/' . $value;
            }
        }
        return $filterString;
    }

}

?>