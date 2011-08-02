<?php

class Cdpreports extends Cf_Controller {

    function Cdpreports() {
        parent::__construct();
        $this->load->library('cf_table');
    }

    function get_report($report=NULL) {
        $report=urldecode($report);
        $data = cfpr_cdp_report(NULL,$report);
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
            'report'=>$report
        );
        $this->template->load('template', 'cdpreports/cdpreport', $data);   
    }

}
?>
