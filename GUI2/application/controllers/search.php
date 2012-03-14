<?php

class Search extends Cf_Controller {
    
private $filter_view_mappings=array();

    function Search()
    {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->model(array('report_model'));
        $this->load->helper('form');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('widgets/hostfinder.js');
        $this->carabiner->js('widgets/reportfinder.js');
        $this->carabiner->js('widgets/contextfinder.js');
        $this->carabiner->css('contextfinder.css');
        
        
        $this->filter_view_mappings=array(
            'bundle-profile'=>'bundleprofile',
            'business-value'=>'business_value_report',
            'contexts'=>'class_profile',
            'promise-compliance'=>'compliance_by_promise',
            'compliance-summary'=>'summary_report',
            'file-change-log'=>'file_change_log',
            'file-change-diffs'=>'file_change_diffs',
            'neighbors'=>'last_saw_hosts',
            'patches-available'=>'patches_available',
            'patches-installed'=>'patches_available',
            'benchmarks'=>'performance',
            'promises-repaired-log'=>'promises_repaired_log',
            'promises-repaired-summary'=>'promises_repaired_log',
            'promises-not-kept-summary'=>'promises_not_kept',
            'promises-not-kept-log'=>'promises_not_kept',
            'setuid-programs'=>'uid_gid_root_programs',
            'software-installed'=>'software_installed',
            'values'=>'variables',
            'virtual-bundles'=>'virtualbundles'
        );
                
    }

    /**
     * Function to convert associative array to path segement
     * Discards the empty value
     */
    function assoc_to_uri($array)
    {
        $temp = array();
        foreach ((array) $array as $key => $val)
        {
            if (trim($val))
            {
                $temp[] = $key;
                $temp[] = $val;
            }
        }
        return implode('/', $temp);
    }

    function __reports_menu()
    {
        try
        {
            $reportsData = $this->report_model->getAllReports();
            $reports = $reportsData['data'];
            $treeview_reports = array();
            foreach ($reports as $report)
            {
                if (key_exists($report['category'], $treeview_reports))
                {
                    array_push($treeview_reports[$report['category']], $report);
                }
                else
                {
                    $treeview_reports[$report['category']] = array();
                    array_push($treeview_reports[$report['category']], $report);
                }
            }
            return $treeview_reports;
        }
        catch (Exception $e)
        {
            return array();
        }
    }

    /**
     * function to generate reports menu
     */
    function generateReportMenu($selectedCat = '', $selectedType = '')
    {
        $reportArray = $this->__reports_menu();
        $data['reports_menu'] = $reportArray;
        $data['report_category'] = $selectedCat;
        $data['report_type'] = $selectedType;
        $this->load->view('/searchpages/reportsMenu', $data);
    }

    function index()
    {

        $this->load->model('settings_model');
        $this->load->model('host_model');
        // we will use username for RBAC
        $username = $this->session->userdata('username');

        $requiredjs = array(
            array('widgets/notes.js'),
            array('jquery.form.js'),
            array('SmartTextBox.js'),
            array('widgets/classfinder.js')
        );
        $this->carabiner->js($requiredjs);
        $virtualBundleModel = $this->load->model('virtual_bundle_model');


        $fromEmail = trim($this->settings_model->app_settings_get_item('appemail'));
        $fromEmail = ($fromEmail) ? $fromEmail : '';

        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search'])? $getparams['search']:($this->input->post('search')?$this->input->post('search'):NULL);
        $hostkey = "";
        $many = "";
        $report_type = isset($getparams['report']) ? urldecode($getparams['report']) : ($this->input->post('report')?urldecode($this->input->post('report')):NULL);

        $host = isset($getparams['host']) ? urldecode(trim($getparams['host'])) : ($this->input->post('host')?trim($this->input->post('host')):NULL);
        $hours_deltafrom = isset($getparams['hours_deltafrom']) ? $getparams['hours_deltafrom'] : $this->input->post('hours_deltafrom');
        $hours_deltato = isset($getparams['hours_deltato']) ? $getparams['hours_deltato'] : $this->input->post('hours_deltato');
        $class_regex = isset($getparams['class_regex']) ? urldecode($getparams['class_regex']) : $this->input->post('class_regex');
        $hosts_only = isset($getparams['hosts_only']) ? $getparams['hosts_only'] : $this->input->post('hosts_only');
        $state = isset($getparams['state']) ? $getparams['state'] : $this->input->post('state');
        $longterm_data = isset($getparams['long_term']) ? $getparams['long_term'] : $this->input->post('long_term');
        $incList = isset($getparams['inclist']) ? urldecode($getparams['inclist']) : $this->input->post('inclist');
        $exList = isset($getparams['exlist']) ? urldecode($getparams['exlist']) : $this->input->post('exlist');
        $hostname = "";


        $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if (is_numeric($rows))
        {
            $rows = (int) $rows;
        }
        else
        {
            $rows = 20;
        }
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        
        //necessary for search result view
        //must use site_url for making bread crumbs work
        if (!is_ajax() && $host == "All") {
            $hostkey = NULL;
            $many = true;
        }
        elseif (!is_ajax() && $host != "")
        {
            $many = false;
            $hostkey = $host;
            $hostname = $this->host_model->getHostName($username, $hostkey);
        }


        $params = '';
        $breadcrumbs_url = "search/index/";
        $hostfinderparams = "";

        if (!is_ajax())
        {

            if (count($getparams) > 0)
            {
                foreach ($getparams as $key => $value)
                {
                    if (!empty($value))
                    {

                        $breadcrumbs_url.=$key . '/' . $value . '/';
                        if ($key <> "page" && $key <> "rows")
                        {
                            $params.=$key . '/' . $value . '/';
                        }
                        if ($key <> "host" && $key <> "page" && $key <> "rows")
                        {
                            $hostfinderparams.=$key . '/' . $value . '/';
                        }
                    }
                }
            }
            else
            {

                foreach ($_POST as $key => $value)
                {
                    if (!empty($value))
                    {
                        $params.=$key . '/' . urlencode($value) . '/';
                        $breadcrumbs_url .= $key . '/' . urlencode($value) . '/';
                        if ($key <> "page" && $key <> "rows" && $key <> "host")
                        {
                            $hostfinderparams.=$key . '/' . urlencode($value) . '/';
                        }
                    }
                }
            }

            //$modified=$this->breadcrumblist-> replace_last_with_current("search/index",site_url("$breadcrumbs_url"));
            $bc = array(
                'title' => $this->lang->line('breadcrumb_report'),
                'url' => site_url("$breadcrumbs_url"),
                'isRoot' => false,
                'replace_existing' => true
            );
            $this->breadcrumb->setBreadCrumb($bc);
        }

        $paramArray = array_merge($getparams, $_POST);
        $paramArray['report'] = $report_type; // we need this for the ajax queries
        foreach ($paramArray as $index => $value)
        {
            $paramArray[$index] = urldecode($value);
        }

        try{
        $data = array(
            'report_type' => $report_type,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
            'report_title' => $this->report_model->getReportTitle($report_type),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' => $page_number,
            'number_of_rows' => $rows,
            'params' => $params,
            'paramArray' => $paramArray,
            'classregex' => $class_regex,
            'hostfinderparams' => $hostfinderparams,
            'breadCrumbUrl' => isset($breadcrumbs_url) ? $breadcrumbs_url : '',
            'hostname' => $hostname,
            'hostkey' => $hostkey,
            'resultView' => 'default_result_view',
            'fromEmail' => $fromEmail,
            'reports_menu'=>$this->__reports_menu(),
            'report_category'=>$this->report_model->getReportCategory($report_type),
            'filter_view'=>$this->filter_view_mappings[$report_type]
        );
        }catch(Exception $e){
            show_error($e->getMessage(), 500);
        }



        if (isset($getparams['name']))
        {
            $data['name'] = urldecode(trim($getparams['name']));
        }
        elseif ($this->input->post('name'))
        {
            $data['name'] = urldecode(trim($this->input->post('name')));
        }

        if ($search == "")
        {
            $search = ".*";
        }


     try{
        switch ($report_type) {
            case "bundle-profile":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : ($this->input->post('name')?urldecode($this->input->post('name')):NULL);
                    if ($hosts_only) {
                        //$data['report_result'] = cfpr_hosts_with_bundlesseen($username, NULL, $name, true, $class_regex);
                        $data['report_result']=$this->report_model->getHostWithBundles($username,$name,$class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                      } else {

                        $pdfurlParams = array('type' => $report_type,
                            'search' => $name,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'hostkey' => $hostkey
                        );

                        //$data['report_result'] = cfpr_report_bundlesseen($username, NULL, $name, true, explode(',',$incList), explode(',',$exList), "last-verified", true, $rows, $page_number);
                      
                        $data['report_result'] = $this->report_model->getBundleReport($username,$hostkey,$name,explode(',',$incList), explode(',',$exList),$rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
               
                break;
            case "business-value":
                    $date = isset($getparams['date']) ? urldecode($getparams['date']) : urldecode($this->input->post('date'));
                  
                    if ($hosts_only) {
                        $data['report_title'] = $report_type . " Days: $days<br>Months: $months<br>Years: $years";
                        //$data['report_result'] = cfpr_hosts_with_value($username, NULL, $days, $months, $years, $class_regex);
                        //$data['report_result'] = $this->report_model->getHostsWithBusinessValue($username, $days, $months, $years, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                            'date' => $date,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                        );

                        $data['report_title'] = $report_type;
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        //$data['report_result'] = cfpr_report_value($username, NULL, $days, $months, $years, explode(',',$incList), explode(',',$exList), "day", true, $rows, $page_number);
                        $data['report_result']= $this->report_model->getBusinessValueReport($username, $hostkey, $date, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                break;
            case "contexts":
                    
                $name = isset($getparams['name']) ? urldecode($getparams['name']) : $this->input->post('name');
                    if ($hosts_only) {
                        $data['report_result'] = cfpr_hosts_with_classes($username, NULL, $name, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    } else {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'search' => $name
                        );

                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        //$data['report_result'] = cfpr_report_classes($username, NULL, $name, true, explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getClassReport($username, $hostkey, $name, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                break;
                
                
            case "promise-compliance":
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $state = isset($getparams['state']) ? urldecode($getparams['state']) : urldecode($this->input->post('state'));

                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_compliance_promises($username, NULL, $name, $state, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'search' => $name,
                            'state' => $state
                        );

                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        //$data['report_result'] = cfpr_report_compliance_promises($username, NULL, $name, $state, true,explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getPromiseCompliance($username, $hostkey, $name, $state, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {


                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'state' => $state,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_compliance_promises($username, $hostkey, $search, $state, true,explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getPromiseCompliance($username, $hostkey, $name, $state, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/compliance_by_promise', $data) : $this->template->load('template', 'searchpages/compliance_by_promise', $data);
                }
                break;
            case "compliance-summary":
                if ($many)
                {
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_compliance_summary($username, $hostkey, NULL, -1, -1, -1, -1, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'hostkey' => $hostkey
                        );
                        $data['report_title'] = $report_type;
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        //$data['report_result'] = cfpr_report_compliance_summary($username, NULL, NULL, -1, -1, -1, -1, ">", explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                        $data['report_result']= $this->report_model->getComplianceSummary($username, $hostkey, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_compliance_summary($username, $hostkey, NULL, -1, -1, -1, -1, ">",explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                    $data['report_result']= $this->report_model->getComplianceSummary($username, $hostkey, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/summary_report', $data) : $this->template->load('template', 'searchpages/summary_report', $data);
                }
                break;
            case "file-change-log":
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_filechanges($username, NULL, $name, true, -1, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                           'inclist' =>$incList,
                            'exlist'=>$exList,
                            'hostkey' => $hostkey,
                            'search' => $name,
                            'long_term' => $longterm_data
                        );
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        /*if ($longterm_data) {
                            $data['report_result'] = cfpr_report_filechanges_longterm($username, NULL, $name, true, -1, ">",explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                        } else {
                            $data['report_result'] = cfpr_report_filechanges($username, NULL, $name, true, -1, ">",explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                        }*/
                        $data['report_result']=$this->report_model->getFileChangeLog($username, $hostkey, $name, explode(',',$incList), explode(',',$exList), $longterm_data, $rows , $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_filechanges($username, $hostkey, $search, true, -1, ">", explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getFileChangeLog($username, $hostkey, $name, explode(',',$incList), explode(',',$exList), FALSE, $rows , $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/file_change_log', $data) : $this->template->load('template', 'searchpages/file_change_log', $data);
                }
                break;
            case "file-change-diffs":
                $data['resultView'] = 'filechangediff_result';
                $data['nofix'] = true;
                $cal = -1;
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $diff = isset($getparams['diff']) ? urldecode($getparams['diff']) : urldecode($this->input->post('diff'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_filediffs($username, NULL, $name, $diff, true, $cal, ">", $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'diff' => $diff,
                            'cal' => $cal,
                            'search' => $name,
                            'long_term' => $longterm_data
                        );
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        /*if ($longterm_data) {
                            $data['report_result'] = cfpr_report_filediffs_longterm($username, NULL, $name, $diff, true, $cal, ">", explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                        } else {
                            $data['report_result'] = cfpr_report_filediffs($username, NULL, $name, $diff, true, $cal, ">", explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                        }*/
                        $data['report_result']=$this->report_model->getFileChangeDiff($username, $hostkey, $name, $diff, $cal, explode(',',$incList), explode(',',$exList), $longterm_data, $rows, $page_number);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'cal' => $cal,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_filediffs($username, $hostkey, $search, NULL, true, $cal, ">", explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getFileChangeDiff($username, $hostkey, $search, $diff, $cal, explode(',',$incList), explode(',',$exList), FALSE, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/file_change_diffs', $data) : $this->template->load('template', 'searchpages/file_change_diffs', $data);
                }
                break;
            case "neighbors":
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $address = isset($getparams['address']) ? urldecode($getparams['address']) : urldecode($this->input->post('address'));
                    $key = isset($getparams['key']) ? urldecode($getparams['key']) : urldecode($this->input->post('key'));
                    $ago = isset($getparams['ago']) ? urldecode($getparams['ago']) : urldecode($this->input->post('ago'));

                    $ago = $ago == 0 ? -1 : $ago;
                    if ($hosts_only)
                    {// when host only field is checked  to geat a group pf hosts
                        $data['report_result'] = cfpr_hosts_with_lastseen($username, NULL, $key, $name, $address, $ago, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'key' => $key,
                            'address' => $address,
                            'ago' => $ago,
                            'search' => $name
                        );



                        //$data['report_result'] = cfpr_report_lastseen($username, NULL, $key, $name, $address, $ago, true, explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                        $data['report_result'] =$this->report_model->getLastSeenReport($username, $hostkey, $key, $name, $address, $ago, explode(',',$incList), explode(',',$exList), $rows = 50, $page_number = 1);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {

                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'hostkey' => $hostkey,
                        'ago' => -1,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email/');
                    //$data['report_result'] = cfpr_report_lastseen($username, $hostkey, NULL, $search, NULL, -1, true, explode(',',$incList), explode(',',$exList), "last-seen", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getLastSeenReport($username, $hostkey, $key, $search, $address, $ago, explode(',',$incList), explode(',',$exList), $rows = 50, $page_number = 1);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/last_saw_hosts', $data) : $this->template->load('template', 'searchpages/last_saw_hosts', $data);
                }
                break;
            case "patches-available":
                if ($many)
                {

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_patch_avail($username, NULL, $name, $version, $arch, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                           'inclist' =>$incList,
                            'exlist'=>$exList,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );

                        //$data['report_result'] = cfpr_report_patch_avail($username, NULL, $name, $version, $arch, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getPatchesAvailable($username, $hostkey, $name, $version, $arch, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {


                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_patch_avail($username, $hostkey, $search, NULL, NULL, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getPatchesAvailable($username, $hostkey, $search, $version, $arch, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/patches_available', $data) : $this->template->load('template', 'searchpages/patches_available', $data);
                }
                break;
            case "patches-installed":
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_patch_in($username, NULL, $name, $version, $arch, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );


                        //$data['report_result'] = cfpr_report_patch_in($username, NULL, $name, $version, $arch, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                        $data['report_result']= $this->report_model->getPatchesInstalled($username, $hostkey, $name, $version, $arch, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {

                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_patch_in($username, $hostkey, $search, NULL, NULL, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                    $data['report_result']= $this->report_model->getPatchesInstalled($username, $hostkey, $search, $version, $arch, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/patches_available', $data) : $this->template->load('template', 'searchpages/patches_available', $data);
                }
                break;
            case "benchmarks":
                if ($many)
                {

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_performance($username, NULL, $name, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'search' => $name,
                            
                        );

                        //$data['report_result'] = cfpr_report_performance($username, NULL, $name, true, explode(',',$incList), explode(',',$exList), "last-performed", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getPerformance($username, $hostkey, $name, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {
                    $pdfurlParams = array('type' => $report_type,
                        'class_regex' => $class_regex,
                        'search' => $search,
                        'hostkey' => $hostkey
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result']=$this->report_model->getPerformance($username, $hostkey, $search, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/performance', $data) : $this->template->load('template', 'searchpages/performance', $data);
                }
                break;
            case "promises-repaired-log":
            case "promises-repaired-summary":
                $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                if ($many)
                {
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_repaired($username, NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'search' => $name,
                            'hours_deltafrom' => $hours_deltafrom,
                            'hours_deltato' => $hours_deltato
                        );
                        if ($report_type == "promises-repaired-log")
                           // $data['report_result'] = cfpr_report_repaired($username, NULL, $name, intval($hours_deltafrom), intval($hours_deltato), explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                              $data['report_result']=$this->report_model->getPromisesRepairedLog($username, $hostkey, $name, $hours_deltafrom, $hours_deltato, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                            if ($report_type == "promises-repaired-summary")
                               $data['report_result']= $this->report_model->getPromisesRepairedSummary($username, $hostkey, $name, $hours_deltafrom, $hours_deltato, explode(',',$incList), explode(',',$exList), $rows , $page_number );
                            // $data['report_result'] = cfpr_summarize_repaired($username, NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex, "promise-handle", true, $rows, $page_number);

                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                } elseif ($hostkey != "")
                {

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $name
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    if ($report_type == "promises-repaired-log")
                        $data['report_result'] = cfpr_report_repaired($username, $hostkey, NULL, 0, 0, explode(',', $incList), explode(',', $exList), "time", true, $rows, $page_number);
                    if ($report_type == "promises-repaired-summary")
                        $data['report_result'] = cfpr_summarize_repaired($username, $hostkey, NULL, NULL, NULL, array(), array(), "time", true, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/promises_repaired_log', $data) : $this->template->load('template', 'searchpages/promises_repaired_log', $data);
                }
                break;
            case "promises-not-kept-summary":
            case "promises-not-kept-log":
                $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                if ($many)
                {
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_notkept($username, NULL, $name, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {
                        if ($report_type == "promises-not-kept-summary")
                            //$data['report_result'] = cfpr_summarize_notkept($username, NULL, $name, NULL, NULL, $class_regex, "time", true, $rows, $page_number);
                             $data['report_result']= $this->report_model->getPromisesNotKeptSummary($username, $hostkey, $name, $hours_deltafrom, $hours_deltato, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                       if ($report_type == "promises-not-kept-log")
                            //$data['report_result'] = cfpr_report_notkept($username, NULL, $name, intval($hours_deltafrom), intval($hours_deltato), explode(',',$incList), explode(',',$exList), "time", true, $rows, $page_number);
                             $data['report_result']=$this->report_model->getPromisesNotKeptLog($username, $hostkey, $name, $hours_deltafrom, $hours_deltato, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'search' => $name,
                            'hours_deltafrom' => $hours_deltafrom,
                            'hours_deltato' => $hours_deltato
                        );


                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email' . $hours_deltafrom . '/hours_deltato/' . $hours_deltato);
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {



                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $name,
                    );


                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    if ($report_type == "promises-not-kept-summary")
                        $data['report_result'] = cfpr_summarize_notkept($username, $hostkey, NULL, NULL, NULL, NULL, "time", true, $rows, $page_number);
                    if ($report_type == "promises-not-kept-log")
                        $data['report_result'] = cfpr_report_notkept($username, $hostkey, NULL, 0, 0, explode(',', $incList), explode(',', $exList), "time", true, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/promises_not_kept', $data) : $this->template->load('template', 'searchpages/promises_not_kept', $data);
                }
                break;
            case "setuid-programs":
                if ($many)
                {

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_setuid($username, NULL, $name, true, explode(',', $incList), explode(',', $exList));
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type , # changed / into - 
                           'inclist' =>$incList,
                           'exlist'=>$exList,
                            'search' => $name
                        );

                        //$data['report_result'] = cfpr_report_setuid($username, NULL, $name, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                        $data['report_result']= $this->report_model->getReportSetUid($username, $hostkey, $name, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_setuid($username, $hostkey, $search, true, explode(',', $incList), explode(',', $exList), "hostname", true, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/uid_gid_root_programs', $data) : $this->template->load('template', 'searchpages/uid_gid_root_programs', $data);
                }
                break;
            case "software-installed":
                if ($many)
                {
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_software_in($username, NULL, $name, $version, $arch, true, explode(',', $incList), explode(',', $exList));
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'version' => $version,
                            'arch' => $arch,
                            'search' => $name
                        );

                        //$data['report_result'] = cfpr_report_software_in($username, NULL, $name, $version, $arch, true, explode(',',$incList), explode(',',$exList), "hostname", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getSoftwareInstalled($username, $hostkey, $search, $version, $arch, explode(',',$incList), explode(',',$exList), $rows, $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = cfpr_report_software_in($username, $hostkey, $search, $version, $arch, true, explode(',', $incList), explode(',', $exList), "hostname", true, $rows, $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/software_installed', $data) : $this->template->load('template', 'searchpages/software_installed', $data);
                }
                break;
            case "values":
                $data['resultView'] = 'variable_result';

                if ($many)
                {
                    $scope = isset($getparams['scope']) ? urldecode($getparams['scope']) : urldecode($this->input->post('scope'));
                    $lval = isset($getparams['lval']) ? urldecode($getparams['lval']) : urldecode($this->input->post('lval'));
                    $rval = isset($getparams['rval']) ? urldecode($getparams['rval']) : urldecode($this->input->post('rval'));
                    $type = isset($getparams['type']) ? urldecode($getparams['type']) : urldecode($this->input->post('type'));
                    if ($hosts_only)
                    {
                        $data['report_result'] = cfpr_hosts_with_vars($username, NULL, $scope, $lval, $rval, $type, true, $class_regex);
                        $this->template->load('template', 'searchpages/search_result_group', $data);
                    }
                    else
                    {

                        $pdfurlParams = array('type' => $report_type,
                            'inclist' =>$incList,
                            'exlist'=>$exList,
                            'scope' => $scope,
                            'lval' => $lval,
                            'rval' => $rval,
                            'var_type' => $type
                        );

                        //$data['report_result'] = cfpr_report_vars($username, NULL, $scope, $lval, $rval, $type, true, explode(',',$incList), explode(',',$exList), "var-name", true, $rows, $page_number);
                        $data['report_result']=$this->report_model->getVariablesReport($username, $hostkey, $scope, $lval, $rval, $type,explode(',',$incList), explode(',',$exList), $rows , $page_number);
                        $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                        $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                        $this->template->load('template', 'searchpages/businessresult', $data);
                    }
                }
                elseif ($hostkey != "")
                {


                    $pdfurlParams = array('type' => $report_type,
                        'class' => $class_regex,
                        'hostkey' => $hostkey,
                        'search' => $search
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    //$data['report_result'] = cfpr_report_vars($username, $hostkey, NULL, $search, NULL, NULL, true, explode(',',$incList), explode(',',$exList), "var-name", true, $rows, $page_number);
                    $data['report_result']=$this->report_model->getVariablesReport($username, $hostkey, $scope, $search, $rval, $type, explode(',',$incList), explode(',',$exList), $rows , $page_number);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    is_ajax() ? $this->load->view('searchpages/variables', $data) : $this->template->load('template', 'searchpages/variables', $data);
                }
                break;

            case "virtual-bundles":
                $allUsers = isset($getparams['all_user']) ? urldecode($getparams['all_user']) : urldecode($this->input->post('all_user'));
                if ($many)
                {
                    $username = (trim($allUsers) == '') ? $this->session->userdata('username') : null;
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));


                    $pdfurlParams = array('type' => $report_type,
                        'name' => $name,
                        'all_user' => $username
                    );


                    $data['report_result'] = $this->virtual_bundle_model->getVirtualBundleData($name, $username, $rows, $page_number);

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);
                }
                else
                {
                    // not nothing else is satisfied display extra form for more search paramaters
                    is_ajax() ? $this->load->view('searchpages/virtualbundles', $data) : $this->template->load('template', 'searchpages/virtualbundles', $data);
                }
                break;


            default:
                $this->template->load('template', 'searchpages/nodata', $data);
        }
     }
     catch(Exception $e){
      show_error($e->getMessage(), 500);
     }
  }

   /* function __host_only_table($data_array) {
        $table = "";
        $cells = array();
        if (count($data_array) > 0)
        {
            foreach ($data_array as $cols)
            {
                array_push($cells, anchor('welcome/host/' . $cols['hostkey'], $cols['hostname'], 'class="imglabel"'));
            }
        }
        if (count($cells) > 0)
        {
            $table.="<table><tr>";
            for ($i = 0; $i < count($cells); $i++)
            {
                if ($i % 6 == 0 && $i != 0)
                {
                    $table.='</tr><tr>';
                }
                $table.='<td>' . $cells[$i] . '</td>';
            }
            $table.="</tr></table>";
        }
        else
        {
            $table.="<table><tr><td>No Host Found !!!</td></tr></table>";
        }
        return $table;
    }*/
    
    function filterSearchView() {
        $getparams = $this->uri->uri_to_assoc(3);
        $report_id = isset($getparams['report']) ? urldecode($getparams['report']) : ($this->input->post('report')?urldecode($this->input->post('report')):NULL);
       
        $paramArray = array_merge($getparams, $_POST);
        $paramArray['report'] = $report_id; // we need this for the ajax queries
        foreach ($paramArray as $index => $value) {
            $paramArray[$index] = urldecode($value);
        }
        
        $data=array('report_type'=> $report_id, 'paramArray' => $paramArray);
        if (isset($this->filter_view_mappings[$report_id]))
        {
            $filename = $this->filter_view_mappings[$report_id] . ".php";
            if (!file_exists(APPPATH . '/views/searchpages/' . $filename))
            {
                show_error_custom("View not found", 400);
                exit();
            }

            $this->load->view('searchpages/' . $filename, $data);
            return;
        }
        else
        {
            show_error_custom("View not found", 400);
        }
    }

    

}
