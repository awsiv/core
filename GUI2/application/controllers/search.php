<?php

class Search extends Cf_Controller {

    function Search() {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->helper('form');
        $this->carabiner->js('jquery.tablesorter.min.js');
        //$this->carabiner->js('picnet.jquery.tablefilter.js');
        //$this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js('widgets/hostfinder.js');
        
    }

    /**
     * Function to conver associative array to path segement
     * Discards the empty value 
     */
    function assoc_to_uri($array) {
        $temp = array();
        foreach ((array) $array as $key => $val) {
            if (trim($val)) {
                $temp[] = $key;
                $temp[] = $val;
            }
        }
        return implode('/', $temp);
    }

    function index() {


        $requiredjs = array(
            array('widgets/notes.js'),
            array('jquery.form.js'),
            array('SmartTextBox.js'),
            array('widgets/classfinder.js')
        );
        $this->carabiner->js($requiredjs);




        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $hostkey = "";
        $many = "";
        // $hostkey = isset($getparams['hostkey']) ? $getparams['hostkey'] : $this->input->post('hostkey'); //shoud be read from param
        $report_type = isset($getparams['report']) ? urldecode($getparams['report']) : urldecode($this->input->post('report'));
        //$many = isset($getparams['manyhosts']) ? $getparams['manyhosts'] : $this->input->post('manyhosts'); //shoud be read from param


        $host = isset($getparams['host']) ? urldecode($getparams['host']) : $this->input->post('host');
        $hours_deltafrom = isset($getparams['hours_deltafrom']) ? $getparams['hours_deltafrom'] : $this->input->post('hours_deltafrom');
        $hours_deltato = isset($getparams['hours_deltato']) ? $getparams['hours_deltato'] : $this->input->post('hours_deltato');
        $class_regex = isset($getparams['class_regex']) ? urldecode($getparams['class_regex']) : $this->input->post('class_regex');
        $hosts_only = isset($getparams['hosts_only']) ? $getparams['hosts_only'] : $this->input->post('hosts_only');
        $state = isset($getparams['state']) ? $getparams['state'] : $this->input->post('state');
        $longterm_data = isset($getparams['long_term']) ? $getparams['long_term'] : $this->input->post('long_term');

       
        $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if(is_numeric($rows))
        {
            $rows=(int)$rows;
        }
        else{
             $rows=20;
        }
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        //necessary for search result view
        //must use site_url for making bread crumbs work
        if ($host == "All") {
            $hostkey = "";
            $many = true;
        } elseif ($host != "") {
            $many = false;
            $hostkey = $host;
        }
        $hostname = cfpr_hostname($hostkey);

        $params = '';
        $breadcrumbs_url = "search/index/";
        $hostfinderparams = "";
        if (!is_ajax()) {

            if (count($getparams) > 0) {
                //$params=$this->uri->assoc_to_uri($getparams);
                foreach ($getparams as $key => $value) {
                    if (!empty($value)) {

                        $breadcrumbs_url.=$key . '/' . $value . '/';
                        if ($key <> "page" && $key <> "rows") {
                            $params.=$key . '/' . $value . '/';
                        }
                        if ($key <> "host" && $key <> "page" && $key <> "rows") {
                            $hostfinderparams.=$key . '/' . $value . '/';
                        }
                    }
                }
            } else {

                foreach ($_POST as $key => $value) {
                    if (!empty($value)) {
                        //$params.=$key.'/';
                        $params.=$key . '/' . urlencode($value) . '/';
                        $breadcrumbs_url .= $key . '/' . urlencode($value) . '/';
                        /* $key == "host" || $key == "report" old condition */
                        if ($key <> "page" && $key <> "rows" && $key <> "host") {
                            $hostfinderparams.=$key . '/' . urlencode($value) . '/';
                        }
                    }
                }
            }

            //$modified=$this->breadcrumblist-> replace_last_with_current("search/index",site_url("$breadcrumbs_url"));
            $bc = array(
                'title' => 'Reports',
                'url' => site_url("$breadcrumbs_url"),
                'isRoot' => false,
                'replace_existing' => true
            );
            $this->breadcrumb->setBreadCrumb($bc);
        }
      
        $data = array(
            'report_type' => $report_type,
            'title' => "Cfengine Mission Portal - search results",
            //'title_header' => "search results",
            'report_title' => $report_type,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' => $page_number,
            'number_of_rows' => $rows,
            'params' => $params,
            'classregex' => $class_regex,
            'hostfinderparams' => $hostfinderparams,
            'breadCrumbUrl' => isset($breadcrumbs_url) ? $breadcrumbs_url : '',
            'hostname'=>$hostname
        );
        
     
        
        if (isset($getparams['name'])) {
            $data['name'] = urldecode($getparams['name']);
        } elseif ($this->input->post('name')) {
            $data['name'] = urldecode($this->input->post('name'));
        }

        if ($search == "") {
            $search = ".*";
        }

        switch ($report_type) {
            case "Bundle profile":

                if ($many) {
                  
                     $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {// when host only field is checked  to geat a group pf hosts
                        $data['report_result'] = cfpr_hosts_with_bundlesseen(NULL, $name, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'search' => $name,
                            'class_regex' => $class_regex
                        );

                        $data['report_result'] = cfpr_report_bundlesseen(NULL, $name, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") { //when only a particular host is selected
                    $pdfurlParams = array('type' => $report_type,
                        'search' => $search,
                        'class' => $class_regex,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_bundlesseen($hostkey, $search, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    //not nothing else is satisfied display extra form for more search paramaters
                    is_ajax() ? $this->load->view('searchpages/bundleprofile', $data) : $this->template->load('template', 'searchpages/bundleprofile', $data);
                }
                break;
            case "Business value report":
                if ($many) {
                     $days = isset($getparams['days']) ? urldecode($getparams['days']) : urldecode($this->input->post('days'));
                     $months = isset($getparams['months']) ? urldecode($getparams['months']) : urldecode($this->input->post('months'));
                     $years = isset($getparams['years']) ? urldecode($getparams['years']) : urldecode($this->input->post('years'));                    
                    if ($hosts_only) {
                        $data['report_title'] = $report_type . " Days: $days<br>Months: $months<br>Years: $years";
                        $data['report_result'] = cfpr_hosts_with_value(NULL, $days, $months, $years, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'days' => $days,
                            'months' => $months,
                            'year' => $years,
                            'class_regex' => $class_regex
                        );

                        $data['report_title'] = $report_type;
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $data['report_result'] = cfpr_report_value(NULL, $days, $months, $years, $class_regex, $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'hostKey' => $hostkey,
                        'class_regex' => $class_regex
                    );
                    $data['report_title'] = $report_type;
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_value($hostkey, NULL, NULL, NULL, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/business_value_report', $data) : $this->template->load('template', 'searchpages/business_value_report', $data);
                }
                break;
            case "Class profile":
                if ($many) {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : $this->input->post('name');
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_classes(NULL, $name, true, $class_regex);
                        is_ajax() ? $this->load->view('searchpages/search_result_group', $data) : $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {


                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex
                        );

                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $data['report_result'] = cfpr_report_classes(NULL, $name, true, $class_regex, $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {
                    $pdfurlParams = array('type' => $report_type,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_classes($hostkey, $search, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/class_profile', $data) : $this->template->load('template', 'searchpages/class_profile', $data);
                }
                break;
            case "Compliance by promise":
                if ($many) {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $state = isset($getparams['state']) ? urldecode($getparams['state']) : urldecode($this->input->post('state'));
                  
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_compliance_promises(NULL, $name, $state, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'search' => $name,
                            'state' => $state
                        );

                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $data['report_result'] = cfpr_report_compliance_promises(NULL, $name, $state, true, $class_regex, $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {


                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'state' => $state,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_compliance_promises($hostkey, $search, $state, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/compliance_by_promise', $data) : $this->template->load('template', 'searchpages/compliance_by_promise', $data);
                }
                break;
            case "Compliance summary":
                if ($many) {
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_compliance_summary($hostkey, NULL, -1, -1, -1, -1, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'hostkey' => $hostkey
                        );
                        $data['report_title'] = $report_type;
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $data['report_result'] = cfpr_report_compliance_summary(NULL, NULL, -1, -1, -1, -1, ">", $class_regex, $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_compliance_summary($hostkey, NULL, -1, -1, -1, -1, ">", $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/summary_report', $data) : $this->template->load('template', 'searchpages/summary_report', $data);
                }
                break;
            case "File change log":
                if ($many) {
                     $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_filechanges(NULL, $name, true, -1, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'hostkey' => $hostkey
                        );
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        if ($longterm_data) {
                            $data['report_result'] = cfpr_report_filechanges_longterm(NULL, $name, true, -1, ">", $class_regex, $rows, $page_number);
                        } else {
                            $data['report_result'] = cfpr_report_filechanges(NULL, $name, true, -1, ">", $class_regex, $rows, $page_number);
                        }
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_filechanges($hostkey, $search, true, -1, ">", $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/file_change_log', $data) : $this->template->load('template', 'searchpages/file_change_log', $data);
                }
                break;
            case "File change diffs":
                $cal = -1;
                if ($many) {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $diff = isset($getparams['diff']) ? urldecode($getparams['diff']) : urldecode($this->input->post('diff'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_filediffs(NULL, $name, $diff, true, $cal, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'diff' => $diff,
                            'cal' => $cal,
                            'search' => $name
                        );
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        if ($longterm_data) {
                            $data['report_result'] = cfpr_report_filediffs_longterm(NULL, $name, $diff, true, $cal, ">", $class_regex, $rows, $page_number);
                        } else {
                            $data['report_result'] = cfpr_report_filediffs(NULL, $name, $diff, true, $cal, ">", $class_regex, $rows, $page_number);
                        }
                        $this->template->load('template', 'searchpages/filechangediff_result', $data);
                    }
                } elseif ($hostkey != "") {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'cal' => $cal,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_filediffs($hostkey, $search, NULL, true, $cal, ">", $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/filechangediff_result', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/file_change_diffs', $data) : $this->template->load('template', 'searchpages/file_change_diffs', $data);
                }
                break;
            case "Last saw hosts":
                if ($many) {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                     $address = isset($getparams['address']) ? urldecode($getparams['address']) : urldecode($this->input->post('address'));
                     $key = isset($getparams['key']) ? urldecode($getparams['key']) : urldecode($this->input->post('key'));
                    $ago = isset($getparams['ago']) ? urldecode($getparams['ago']) : urldecode($this->input->post('ago'));
                   
                    $ago = $ago == 0 ? -1 : $ago;
                    if ($hosts_only) {// when host only field is checked  to geat a group pf hosts
                        $data['report_result'] = cfpr_hosts_with_lastseen(NULL, $key, $name, $address, $ago, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'key' => $key,
                            'address' => $address,
                            'ago' => $ago,
                            'search' => $name
                        );



                        $data['report_result'] = cfpr_report_lastseen(NULL, $key, $name, $address, $ago, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'ago' => -1,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email/');
                    $data['report_result'] = cfpr_report_lastseen($hostkey, NULL, $search, NULL, -1, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/last_saw_hosts', $data) : $this->template->load('template', 'searchpages/last_saw_hosts', $data);
                }
                break;
            case "Patches available":
                if ($many) {
                   
                   $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                   $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                   $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_patch_avail(NULL, $name, $version, $arch, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );

                        $data['report_result'] = cfpr_report_patch_avail(NULL, $name, $version, $arch, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {


                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_patch_avail($hostkey, $search, NULL, NULL, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/patches_available', $data) : $this->template->load('template', 'searchpages/patches_available', $data);
                }
                break;
            case "Patch status":
                if ($many) {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                   $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                   $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));
                   
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_patch_in(NULL, $name, $version, $arch, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );


                        $data['report_result'] = cfpr_report_patch_in(NULL, $name, $version, $arch, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_patch_in($hostkey, $search, NULL, NULL, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/patch_status', $data) : $this->template->load('template', 'searchpages/patch_status', $data);
                }
                break;
            case "Performance":
                if ($many) {
                    
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_performance(NULL, $name, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'search' => $name
                        );

                        $data['report_result'] = cfpr_report_performance(NULL, $name, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_performance($hostkey, $search, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/performance', $data) : $this->template->load('template', 'searchpages/performance', $data);
                }
                break;
            case "Promises repaired log":
            case "Promises repaired summary":
                if ($many) {
                   
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_repaired(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'search' => $name,
                            'hours_deltafrom' => $hours_deltafrom,
                            'hours_deltato' => $hours_deltato
                        );
                        if ($report_type == "Promises repaired log")
                            $data['report_result'] = cfpr_report_repaired(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
                        if ($report_type == "Promises repaired summary")
                            $data['report_result'] = cfpr_summarize_repaired(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
                        // $data['report_result'] = cfpr_report_repaired(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    if ($report_type == "Promises repaired log")
                        $data['report_result'] = cfpr_report_repaired($hostkey, NULL, 0, 0, $class_regex, $rows, $page_number);
                    if ($report_type == "Promises repaired summary")
                        $data['report_result'] = cfpr_summarize_repaired($hostkey, NULL, NULL, NULL, NULL, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else {
                    is_ajax() ? $this->load->view('searchpages/promises_repaired_log', $data) : $this->template->load('template', 'searchpages/promises_repaired_log', $data);
                }
                break;
            case "Promises not kept summary":
            case "Promises not kept log":
                if ($many) {
                   
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_notkept(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {
                        if ($report_type == "Promises not kept summary")
                            $data['report_result'] = cfpr_summarize_notkept(NULL, NULL, NULL, NULL, NULL, $rows, $page_number);
                        if ($report_type == "Promises not kept log")
                            $data['report_result'] = cfpr_report_notkept(NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'search' => $name,
                            'hours_deltafrom' => $hours_deltafrom,
                            'hours_deltato' => $hours_deltato
                        );


                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email' . $hours_deltafrom . '/hours_deltato/' . $hours_deltato);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "") {



                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey
                    );


                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    if ($report_type == "Promises not kept summary")
                        $data['report_result'] = cfpr_summarize_notkept($hostkey, NULL, NULL, NULL, NULL, $rows, $page_number);
                    if ($report_type == "Promises not kept log")
                        $data['report_result'] = cfpr_report_notkept($hostkey, NULL, 0, 0, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else {
                    is_ajax() ? $this->load->view('searchpages/promises_not_kept', $data) : $this->template->load('template', 'searchpages/promises_not_kept', $data);
                }
                break;
            case "Setuid/gid root programs":
                if ($many) {
                   
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_setuid(NULL, $name, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => 'Setuid-gid root programs', # changed / into - 
                            'class_regex' => $class_regex,
                            'search' => $name
                        );

                        $data['report_result'] = cfpr_report_setuid(NULL, $name, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_setuid($hostkey, $search, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/searchresult', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/uid_gid_root_programs', $data) : $this->template->load('template', 'searchpages/uid_gid_root_programs', $data);
                }
                break;
            case "Software installed":
                if ($many) {
                     $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                     $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));
                   
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_software_in(NULL, $name, $version, $arch, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );

                        $data['report_result'] = cfpr_report_software_in(NULL, $name, $version, $arch, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "") {

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_software_in($hostkey, $search, NULL, NULL, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                } else {
                    $data['options'] = $this->__host_availabe();
                    $data['package_name_list'] = $this->__software_installed_autocomplete('Name');
                    $data['architecture'] = $this->__software_installed_autocomplete('Architecture');
                    $data['version'] = $this->__software_installed_autocomplete('Version');
                    is_ajax() ? $this->load->view('searchpages/software_installed', $data) : $this->template->load('template', 'searchpages/software_installed', $data);
                }
                break;
            case "Variables":
                if ($many) {
                    $scope = isset($getparams['scope']) ? urldecode($getparams['scope']) : urldecode($this->input->post('scope'));
                    $lval = isset($getparams['lval']) ? urldecode($getparams['lval']) : urldecode($this->input->post('lval'));
                    $rval = isset($getparams['rval']) ? urldecode($getparams['rval']) : urldecode($this->input->post('rval'));
                    $type = isset($getparams['type']) ? urldecode($getparams['type']) : urldecode($this->input->post('type'));
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_vars(NULL, $scope, $lval, $rval, $type, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'class_regex' => $class_regex,
                            'scope' => $scope,
                            'lval' => $lval,
                            'rval' => $rval,
                            'var_type' => $type
                        );

                        $data['report_result'] = cfpr_report_vars(NULL, $scope, $lval, $rval, $type, true, $class_regex, $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/variable_result', $data);
                    }
                } elseif ($hostkey != "") {


                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_vars($hostkey, NULL, $search, NULL, NULL, true, $class_regex, $rows, $page_number);
                    $this->template->load('template', 'searchpages/variable_result', $data);
                } else {
                    is_ajax() ? $this->load->view('searchpages/variables', $data) : $this->template->load('template', 'searchpages/variables', $data);
                }
                break;
            default:
                $this->template->load('template', 'searchpages/nodata', $data);
        }
    }

    function __software_installed_autocomplete($column) {
        $data = cfpr_report_software_in(NULL, NULL, NULL, NULL, true, NULL, NULL, NULL);
        $decoded_data = json_decode($data);
        $column_index = $decoded_data->meta->header->$column;
        $column = array();
        foreach ($decoded_data->data as $rows) {
            array_push($column, $rows[$column_index]);
        }
        $unique_elements = array_unique($column);
        if (is_array($unique_elements)) {  //necessary because the array_unique some times return associative array
            $val = array_values($unique_elements);
            return json_encode($val);
        } else {
            return json_encode($unique_elements);
        }
    }

    function __host_availabe() {
        $result = json_decode(cfpr_select_hosts("none", ".*", 100));
        $allhosts = array('All' => 'All host');
        if (count($result) > 0) {
            foreach ($result as $host) {
                $allhosts[$host->key] = $host->id;
            }
        }
        return $allhosts;
    }

    function __host_only_table($data_array) {
        $table = "";
        $cells = array();
        if (count($data_array) > 0) {
            foreach ($data_array as $cols) {
                array_push($cells, anchor('welcome/host/' . $cols['hostkey'], $cols['hostname'], 'class="imglabel"'));
            }
        }
        if (count($cells) > 0) {
            $table.="<table><tr>";
            for ($i = 0; $i < count($cells); $i++) {
                if ($i % 6 == 0 && $i != 0) {
                    $table.='</tr><tr>';
                }
                $table.='<td>' . $cells[$i] . '</td>';
            }
            $table.="</tr></table>";
        } else {
            $table.="<table><tr><td>No Host Found !!!</td></tr></table>";
        }
        return $table;
    }

}
