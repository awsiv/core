<?php

class Summaryreports extends Cf_Controller {

    function __construct() {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->helper('form');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js('widgets/hostfinder.js');
    }

    function search() {
        $data = '';
        $this->load->view('summaryreports/compliance_by_promise', $data);
    }

    function show() {


        $requiredjs = array(
            array('widgets/classfinder.js'),
            array('widgets/policyfinder.js'),
            array('widgets/notes.js'),
            array('SmartTextBox.js'),
        );
        
        $this->carabiner->js($requiredjs);
        
        $handle = $this->input->post('name');
        $classRegex = $this->input->post('class_regex');
        $host = $this->input->post('host', true);
        $state = 'x';
        $regex = true;
        $time = $this->input->post('time', true);

        
       

        $result = cfpr_report_overall_summary($host, $handle, $state, $regex, $classRegex);


        $data = array(
            'title' => "Cfengine Mission Portal",
            'title_header' => "Summary Report",
            'breadcrumbs' => $this->breadcrumblist->display()
        );

        $timeFrameArray = array('h' => 'Hour',
            'w' => 'Week',
            'd' => 'Day');

        $data['result'] = json_decode(utf8_encode($result), true);
        $data['timeFrame'] = isset($timeFrameArray[$time]) ? $timeFrameArray[$time] : '';
        $classRegex = trim($classRegex);
        $data['classRegex'] = $classRegex;
        $data['handle'] = trim($handle);
        $this->template->load('template', 'summaryreports/summaryresult', $data);
    }

}
