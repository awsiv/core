<?php

class Search extends Cf_Controller
{

    private $filter_view_mappings = array();

    function Search()
    {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination'));
        $this->load->model(array('report_model'));
        $this->load->helper('form');
        //$this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('widgets/hostfinder.js');
        $this->carabiner->js('widgets/reportfinder.js');
        $this->carabiner->js('widgets/contextfinder.js');
        $this->carabiner->css('contextfinder.css');
        $this->carabiner->css('hostsCompliance.css');
        $this->carabiner->css('hostsConnectivity.css');


        $this->filter_view_mappings = array(
            'bundle-profile' => 'bundleprofile',
            'business-value' => 'business_value_report',
            'contexts' => 'class_profile',
            'promise-compliance' => 'compliance_by_promise',
            'compliance-summary' => 'summary_report',
            'file-change-log' => 'file_change_log',
            'file-change-diffs' => 'file_change_diffs',
            'neighbors' => 'last_saw_hosts',
            'patches-available' => 'patches_available',
            'patches-installed' => 'patches_available',
            'benchmarks' => 'performance',
            'promises-repaired-log' => 'promises_repaired_log',
            'promises-repaired-summary' => 'promises_repaired_log',
            'promises-not-kept-summary' => 'promises_not_kept',
            'promises-not-kept-log' => 'promises_not_kept',
            'setuid-programs' => 'uid_gid_root_programs',
            'software-installed' => 'software_installed',
            'values' => 'variables',
            'virtual-bundles' => 'virtualbundles'
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
                $temp[] = urlencode($val);
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

    function _integrateSaveSearches()
    {
        $this->load->model('search_save_model');

        $username = $this->session->userdata('username');

        $filter = array('username' => $username);

        $result = $this->search_save_model->get_all_search($filter);

        /* group by report type */
        $group = array();
        foreach ($result as $obj)
        {
            $title = $obj->getReportType();
            $group[$title][] = $obj;
        }

        return $group;
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

    // generate the save search menu for the reports menu
    function generateSaveSearchMenu(){
    $data['saved_search_menu'] = $this->_integrateSaveSearches();
    $this->load->view('/searchpages/saveSearchMenu', $data);
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



        $fromEmail = trim($this->settings_model->app_settings_get_item('appemail'));
        $fromEmail = ($fromEmail) ? $fromEmail : '';

        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : ($this->input->post('search') ? $this->input->post('search') : NULL);
        $hostkey = "";
        $many = "";

        $report_type = isset($getparams['report']) ? urldecode($getparams['report']) : ($this->input->post('report') ? urldecode($this->input->post('report')) : NULL);
        if (!$report_type)
        {
            redirect('engineering');
        }
        $host = isset($getparams['host']) ? urldecode(trim($getparams['host'])) : ($this->input->post('host') ? trim($this->input->post('host')) : NULL);
        $class_regex = isset($getparams['class_regex']) ? urldecode($getparams['class_regex']) : $this->input->post('class_regex');
        $hosts_only = isset($getparams['hosts_only']) ? $getparams['hosts_only'] : $this->input->post('hosts_only');


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
        if ($host == "All")
        {
            $hostkey = NULL;
        }
        elseif ($host != "")
        {
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

       // do a redirect here if it is a post
       // fix for back button with Post/Redirect/Get
       if (strtolower($_SERVER["REQUEST_METHOD"]) === "post") {
           // redirect with correct params
           redirect('search/index/'.$this->assoc_to_uri($paramArray));
       }

       try
        {
            $data = array(
                'report_type' => $report_type,
                'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
                'report_title' => $this->report_model->getReportTitle($report_type),
                'breadcrumbs' => $this->breadcrumblist->display(),
                'breadCrumbUrl' => isset($breadcrumbs_url) ? $breadcrumbs_url : '',
                'hide_header' => true,
                'current' => $page_number,
                'number_of_rows' => $rows,
                'params' => $params,
                'paramArray' => $paramArray,
                'classregex' => $class_regex,
                'hostfinderparams' => $hostfinderparams,
                'hostname' => $hostname,
                'hostkey' => $hostkey,
                'resultView' => $hosts_only ? 'search_result_group' : 'default_result_view',
                'fromEmail' => $fromEmail,
                'reports_menu' => $this->__reports_menu(),
                'saved_search_menu' => $this->_integrateSaveSearches(),
                'report_category' => $this->report_model->getReportCategory($report_type),
                'filter_view' => $this->filter_view_mappings[$report_type],
                'hostcount' => $this->host_model->getHostCount($username, NULL, explode(',', $incList), explode(',', $exList)),
                'incList' => $incList,
                'exList' => $exList
            );
        }
        catch (Exception $e)
        {
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


        try
        {
            switch ($report_type)
            {
                case "bundle-profile":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : ($this->input->post('name') ? urldecode($this->input->post('name')) : NULL);
                    $clevel = isset($getparams['clevel']) ? urldecode($getparams['clevel']) : false;
                    if ($hosts_only)
                    {

                        $res = cfpr_hosts_compliance_for_bundles($username, null, $name, true, explode(',', $incList), explode(',', $exList));
                        $complianceData = json_decode($res, true);
                        $data['c_data'] = $complianceData;
                        $data['show_host_summary'] = true;

                        $tempParamArray = $data['paramArray'];
                        unset($tempParamArray['hosts_only']);
                        unset($tempParamArray['clevel']);
                        unset($tempParamArray['page']);
                        unset($tempParamArray['rows']);

                        $data['detail_result_url'] = $this->assoc_to_uri($tempParamArray);
                        $data['clevel'] = $clevel;

                        $data['report_result'] = $this->report_model->getHostWithBundles($username, $name, explode(',', $incList), explode(',', $exList), $rows, $page_number, $clevel);
                        $data['resultView'] = 'search_result_group_with_summary';
                    }
                    else
                    {
                        $data['report_result'] = $this->report_model->getBundleReport($username, $hostkey, $name, explode(',', $incList), explode(',', $exList), $rows, $page_number, $clevel);
                    }

                    $pdfurlParams = array('type' => $report_type,
                        'search' => $name,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'hostkey' => $hostkey,
                        'clevel' => $clevel
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);


                    break;
                case "business-value":
                    $date = isset($getparams['date']) ? urldecode($getparams['date']) : urldecode($this->input->post('date'));

                    if ($hosts_only)
                    {
                        $data['report_result'] = $this->report_model->getHostsWithBusinessValue($username, $date, explode(',', $incList), explode(',', $exList), $rows, $page_number);
                    }
                    else
                    {
                        $data['report_result'] = $this->report_model->getBusinessValueReport($username, $hostkey, $date, explode(',', $incList), explode(',', $exList), $rows, $page_number);
                    }
                    $pdfurlParams = array('type' => $report_type,
                        'date' => $date,
                        'inclist' => $incList,
                        'exlist' => $exList,
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "contexts":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : $this->input->post('name');
                    $data['report_result'] = $this->report_model->getClassReport($username, $hostkey, $name, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'search' => $name
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;


                case "promise-compliance":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $state = isset($getparams['state']) ? urldecode($getparams['state']) : urldecode($this->input->post('state'));
                    $clevel = isset($getparams['clevel']) ? urldecode($getparams['clevel']) : false;

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'search' => $name,
                        'state' => $state,
                        'clevel' => $clevel
                    );

                    if ($hosts_only)
                    {
                        $res = cfpr_hosts_compliance_for_promises($username, NULL, $name, $state, true, explode(',', $incList), explode(',', $exList));
                        $complianceData = json_decode($res, true);
                        $data['c_data'] = $complianceData;
                        $data['show_host_summary'] = true;
                        $data['clevel'] = $clevel;
                        $tempParamArray = $data['paramArray'];
                        unset($tempParamArray['hosts_only']);
                        unset($tempParamArray['clevel']);
                        unset($tempParamArray['page']);
                        unset($tempParamArray['rows']);
                        $data['detail_result_url'] = $this->assoc_to_uri($tempParamArray);
                        $data['resultView'] = 'search_result_group_with_summary';
                    }

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = $this->report_model->getPromiseCompliance($username, $hostkey, $name, $state, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only, $clevel);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;

                case "compliance-summary":

                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'hostkey' => $hostkey,
                        'version' => $version
                    );
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = $this->report_model->getComplianceSummary($username, $hostkey, $version, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $this->template->load('template', 'searchpages/businessresult', $data);


                    break;
                case "file-change-log":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $from = isset($getparams['from']) ? urldecode($getparams['from']) : $this->input->post('from');
                    $to   = isset($getparams['to'])   ? urldecode($getparams['to'])   : $this->input->post('to');
                    
                    $from_timestamp = strtotime($from);
                    $to_timestamp   = strtotime($to);
                    
                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist'  => $exList,
                        'hostkey' => $hostkey,                        
                        'from'    => $from_timestamp,
                        'to'      => $to_timestamp,
                        'search'  => $name
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = $this->report_model->getFileChangeLog($username, $hostkey, $name, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "file-change-diffs":
                    if (!$hosts_only)
                    {
                        $data['resultView'] = 'filechangediff_result';
                        $data['nofix'] = true;
                    }
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $diff = isset($getparams['diff']) ? urldecode($getparams['diff']) : urldecode($this->input->post('diff'));
                    $from = isset($getparams['from']) ? urldecode($getparams['from']) : $this->input->post('from');
                    $to   = isset($getparams['to'])   ? urldecode($getparams['to'])   : $this->input->post('to');

                    $from_timestamp = strtotime($from);
                    $to_timestamp   = strtotime($to);
                    
                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist'  => $exList,
                        'diff'    => $diff,
                        'from'    => $from_timestamp,
                        'to'      => $to_timestamp,
                        'search'  => $name
                    );

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $data['report_result'] = $this->report_model->getFileChangeDiff($username, $hostkey, $name, $diff, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "neighbors":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $address = isset($getparams['address']) ? urldecode($getparams['address']) : urldecode($this->input->post('address'));
                    $key = isset($getparams['key']) ? urldecode($getparams['key']) : urldecode($this->input->post('key'));
                    $ago = isset($getparams['ago']) ? urldecode($getparams['ago']) : urldecode($this->input->post('ago'));

                    $ago = $ago == 0 ? -1 : $ago;

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'key' => $key,
                        'address' => $address,
                        'ago' => $ago,
                        'search' => $name
                    );


                    $data['report_result'] = $this->report_model->getLastSeenReport($username, $hostkey, $key, $name, $address, $ago, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;
                case "patches-available":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'version' => $version,
                        'arch' => $arch,
                        'search' => $name
                    );


                    $data['report_result'] = $this->report_model->getPatchesAvailable($username, $hostkey, $name, $version, $arch, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "patches-installed":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'version' => $version,
                        'arch' => $arch,
                        'search' => $name
                    );

                    $data['report_result'] = $this->report_model->getPatchesInstalled($username, $hostkey, $name, $version, $arch, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;
                case "benchmarks":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'search' => $name,
                    );
                    $pdfurlParams = array_map('urlencode', $pdfurlParams);
                    //$data['report_result'] = cfpr_report_performance($username, NULL, $name, true, explode(',',$incList), explode(',',$exList), "last-performed", true, $rows, $page_number);
                    $data['report_result'] = $this->report_model->getPerformance($username, $hostkey, $name, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;
                case "promises-repaired-log":
                case "promises-repaired-summary":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $from = isset($getparams['from']) ? urldecode($getparams['from']) : $this->input->post('from');
                    $to   = isset($getparams['to'])   ? urldecode($getparams['to'])   : $this->input->post('to');
                    
                    $cause_rx = isset($getparams['cause']) ? urldecode($getparams['cause']) : $this->input->post('cause');
                    $cause_rx = $cause_rx === false ? ".*" : $cause_rx;
               
                    $from_timestamp = strtotime($from);
                    $to_timestamp   = strtotime($to);

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist'  => $exList,
                        'search'  => $name,
                        'from'    => $from_timestamp,
                        'to'      => $to_timestamp,
                        'cause'   => $cause_rx
                    );

                    if ($report_type == "promises-repaired-log")
                        $data['report_result'] = $this->report_model->getPromisesRepairedLog($username, $hostkey, $name, $cause_rx, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    if ($report_type == "promises-repaired-summary")
                        $data['report_result'] = $this->report_model->getPromisesRepairedSummary($username, $hostkey, $name, $cause_rx, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);

                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;
                case "promises-not-kept-summary":
                case "promises-not-kept-log":
                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $from = isset($getparams['from']) ? urldecode($getparams['from']) : $this->input->post('from');
                    $to   = isset($getparams['to'])   ? urldecode($getparams['to'])   : $this->input->post('to');
                    
                    $cause_rx = isset($getparams['cause']) ? urldecode($getparams['cause']) : $this->input->post('cause');
                    $cause_rx = $cause_rx === false ? ".*" : $cause_rx;

                    $from_timestamp = strtotime($from);
                    $to_timestamp   = strtotime($to);
                    
                    if ($report_type == "promises-not-kept-summary")
                        $data['report_result'] = $this->report_model->getPromisesNotKeptSummary($username, $hostkey, $name, $cause_rx, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    if ($report_type == "promises-not-kept-log")
                        $data['report_result'] = $this->report_model->getPromisesNotKeptLog($username, $hostkey, $name, $cause_rx, $from_timestamp, $to_timestamp, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $pdfurlParams = array('type' => $report_type,
                        'inclist'   => $incList,
                        'exlist'    => $exList,
                        'search'    => $name,
                        'from'      => $from_timestamp,
                        'to'        => $to_timestamp,
                        'cause'     => $cause_rx
                    );


                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link']  = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email' . $from . '/to/' . $to);
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "setuid-programs":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $pdfurlParams = array('type' => $report_type, # changed / into -
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'search' => $name
                    );

                    $data['report_result'] = $this->report_model->getReportSetUid($username, $hostkey, $name, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);
                    break;
                case "software-installed":

                    $name = isset($getparams['name']) ? urldecode($getparams['name']) : urldecode($this->input->post('name'));
                    $version = isset($getparams['version']) ? urldecode($getparams['version']) : urldecode($this->input->post('version'));
                    $arch = isset($getparams['arch']) ? urldecode($getparams['arch']) : urldecode($this->input->post('arch'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'version' => $version,
                        'arch' => $arch,
                        'search' => $name
                    );

                    $data['report_result'] = $this->report_model->getSoftwareInstalled($username, $hostkey, $name, $version, $arch, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;
                case "values":
                    if (!$hosts_only)
                    {
                        $data['resultView'] = 'variable_result';
                    }

                    $scope = isset($getparams['scope']) ? urldecode($getparams['scope']) : urldecode($this->input->post('scope'));
                    $lval = isset($getparams['lval']) ? urldecode($getparams['lval']) : urldecode($this->input->post('lval'));
                    $rval = isset($getparams['rval']) ? urldecode($getparams['rval']) : urldecode($this->input->post('rval'));
                    $type = isset($getparams['type']) ? urldecode($getparams['type']) : urldecode($this->input->post('type'));

                    $pdfurlParams = array('type' => $report_type,
                        'inclist' => $incList,
                        'exlist' => $exList,
                        'scope' => $scope,
                        'lval' => $lval,
                        'rval' => $rval,
                        'var_type' => $type
                    );

                    $data['report_result'] = $this->report_model->getVariablesReport($username, $hostkey, $scope, $lval, $rval, $type, explode(',', $incList), explode(',', $exList), $rows, $page_number, $hosts_only);
                    $data['report_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams));
                    $data['email_link'] = site_url('/pdfreports/index/' . $this->assoc_to_uri($pdfurlParams) . '/pdfaction/email');
                    $this->template->load('template', 'searchpages/businessresult', $data);

                    break;

                case "virtual-bundles":
                    $allUsers = isset($getparams['all_user']) ? urldecode($getparams['all_user']) : urldecode($this->input->post('all_user'));

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



                default:
                    $this->template->load('template', 'searchpages/nodata', $data);
            }
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }
    }

    function filterSearchView()
    {
        $getparams = $this->uri->uri_to_assoc(3);
        $report_id = isset($getparams['report']) ? urldecode($getparams['report']) : ($this->input->post('report') ? urldecode($this->input->post('report')) : NULL);

        $paramArray = array_merge($getparams, $_POST);
        $paramArray['report'] = $report_id; // we need this for the ajax queries
        foreach ($paramArray as $index => $value)
        {
            $paramArray[$index] = urldecode($value);
        }

        $data = array('report_type' => $report_id, 'paramArray' => $paramArray);
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
