<?php

class savedsearch extends Cf_Controller {

    function __construct() {


        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->helper('form');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js('jquery.qtip-1.0.min.js');
        $this->carabiner->js('widgets/hostfinder.js');

        $this->load->model('search_save_model');
    }

    function save() {

        $username = $this->session->userdata('username');
        $report_title = $this->input->post('report_title', true);
        $label = $this->input->post('search_name', true);
        $searchUrl = $this->input->post('search_url', true);

        $data = array('username' => $username,
            'url' => $searchUrl,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $obj = $this->search_save_model->insert($data);
        if ($obj) {
            echo 'saved search sucessfully.';
            return;
        } else {
            $this->output->set_status_header('404', 'Cannot save the search');
            $output = 'Error while saving the search.<br />';
            $errors = $this->search_save_model->getErrors();
            foreach ($errors as $e) {
                $output .= $e . '<br />';
            }
            echo $output;
            return;
        }
    }

    function listSavedSearches($reportTitle = '', $user='') {

        $username = ($user) ? $user : $this->session->userdata('username');
        $report_title = $reportTitle;
        $filter = array('username' => $username,
        );

        if (trim($reportTitle) != '') {
            $filter['reportType'] = urldecode($reportTitle);
        }


        $result = $this->search_save_model->get_all_search($filter);
        $viewdata['data'] = $result;

        /* group by report type */
        $group = array();
        foreach ($result as $obj) {
            $title = $obj->getReportType();
            $group[$title][] = $obj;
        }
        $viewdata['group'] = $group;

        
        /* is ajax request only server partial view */
        if (is_ajax()) {
            $this->load->view('/savesearch/view_saved_search', $viewdata);
            return;
        }
        
          $bc = array(
            'title' => 'Saved search',
            'url' => 'savedsearch/listSavedSearches/',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "Saved search overview",
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        
        $viewdata = array_merge($viewdata,$data);
        
        
        $this->template->load('template', '/savesearch/show_saved_search', $viewdata);
    }
    
    function delete($id='') {
        
        $filter = array ('_id'=>new MongoId($id)) ;
      
        
       $result =  $this->search_save_model->delete($filter);
       if (!$result) {
           $this->output->set_status_header('404','cannot delete the record');
           return;
       }        
        
    }

}
