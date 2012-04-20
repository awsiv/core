<?php

class Summaryreports extends Cf_Controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->helper('form');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js('widgets/hostfinder.js');
        $this->username = $this->session->userdata('username');
    }

    function search()
    {
        $data = '';
        $this->load->view('summaryreports/compliance_by_promise', $data);
    }

    function show()
    {


        $requiredjs = array(
            array('widgets/classfinder.js'),
            array('widgets/policyfinder.js'),
            array('widgets/notes.js'),
            array('SmartTextBox.js'),
        );

        $this->carabiner->js($requiredjs);

        $getparams = $this->uri->uri_to_assoc(3);
        $handle = isset($getparams['name']) ? urldecode($getparams['name']) : $this->input->post('name');
        $classRegex = isset($getparams['class_regex']) ? urldecode($getparams['class_regex']) : $this->input->post('class_regex');
        $host = isset($getparams['host']) ? urldecode($getparams['host']) : $this->input->post('host', true);
        $state = 'x';
        $regex = true;
        $time = isset($getparams['time']) ? urldecode($getparams['time']) : $this->input->post('time', true);

        $breadcrumbs_url = "";
        $result = cfpr_report_overall_summary($this->username, $host, $handle, $state, $regex, $classRegex);
        if (count($getparams) > 0)
        {
            foreach ($getparams as $key => $value)
            {
                if (!empty($value))
                {
                    $breadcrumbs_url.='/' . $key . '/' . $value;
                }
            }
        }
        else
        {
            foreach ($_POST as $key => $value)
            {
                if (!empty($value))
                {
                    $breadcrumbs_url .= '/' . $key . '/' . urlencode($value);
                }
            }
        }
        $bc = array(
            'title' => $this->lang->line('breadcrumb_summary_report'),
            'url' => '/summaryreports/show' . $breadcrumbs_url,
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_summary_report'),
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