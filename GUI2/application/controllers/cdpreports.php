<?php

class Cdpreports extends Cf_Controller {

    function Cdpreports() {
        parent::__construct();
        $this->load->library('cf_table');
        $this->load->helper('form');
    }

    function get_report($report=NULL) {
        $report=urldecode($report);
        $getparams = $this->uri->uri_to_assoc(4);
         $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if(!is_numeric($rows))
        {
           $rows=20;
        }
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        $data = cfpr_cdp_report(NULL,$report,$rows,$page_number);
        $tabledata = json_decode(utf8_encode($data), true);
        $bc = array(
                'title' => 'CDP Reports',
                'url' => '/cdpreports/get_report/'.urlencode($report),
                'isRoot' => false,
                'replace_existing' => true
            );
         $this->breadcrumb->setBreadCrumb($bc);
         
         $data = array(
            'title' => "Cfengine Mission Portal - cdp reports ",
             'breadcrumbs' => $this->breadcrumblist->display(),
            'tabledata' =>$tabledata,
            'report'=>$report,
             'number_of_rows'=>$rows,
             'current'=>$page_number
        );
        $this->template->load('template', 'cdpreports/cdpreport', $data);   
    }

}
?>
