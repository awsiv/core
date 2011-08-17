<?php

class Welcome extends Cf_Controller {

    function __construct() {
        parent::__construct();
        parse_str($_SERVER['QUERY_STRING'], $_GET);
        $this->load->helper('form');
        $this->load->library('table', 'cf_table');
    }

    function index() {
        $bc = array(
            'title' => 'cfengine mission portal',
            'url' => 'welcome/index',
            'isRoot' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->carabiner->js('widgets/licensemeter.js');
        try {
            $expirydate = strtotime(cfpr_getlicense_expiry());
            $startDate = cfpr_getlicense_installtime();
            //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
            $datediff = $expirydate - $startDate;
            $totaldays = floor($datediff / (60 * 60 * 24));
            $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
            $pbarvalue = "";
           if(!($totaldays < 0))
           { 
               $pbarvalue=floor(($dayspassed / $totaldays) * 100);
           }
        } catch (Exception $e) {
            log_message('license error:' . $e->getMessage());
        }

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'pbarvalue' => $pbarvalue,
            'daysleft' => $totaldays - $dayspassed,
        );

        $gdata = cfpr_compliance_summary_graph(null);
        if ($gdata) {
            $graphData = $this->_convert_summary_compliance_graph($gdata);
            $data = array_merge($data, $graphData);
        }




        $this->template->load('template', 'index', $data);
    }

    function status() {
        $bc = array(
            'title' => 'Status',
            'url' => 'welcome/status',
            'isRoot' => false
        );


        $requiredjs = array(
            array('flot/jquery.flot.js'),
            array('flot/jquery.flot.stack.js'),
            array('flot/jquery.flot.pie.js'),
            array('widgets/notes.js')
        );

        $jsIE = array('flot/excanvas.min.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->js($requiredjs);



        $this->breadcrumb->setBreadCrumb($bc);
        $reports = json_decode(cfpr_select_reports(".*", 100));

        $data = array(
            'title' => "Cfengine Mission Portal-engineering status",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'goals' => json_decode(cfpr_list_business_goals(),true)
        );

        // Summary meter for host
        $gdata = cfpr_summary_meter(null);
        $returnedData = $this->_convert_summary_compliance_graph($gdata);
        $data = array_merge($data, $returnedData);


        // compliance summary meter
        $envList = cfpr_environments_list();

        //$envListArray = json_decode($envList);

        $data['envList'] = $envList;

        $cdata = cfpr_compliance_summary_graph(NULL);
        if ($cdata) {
            $graphData['compliance_summary'] = $this->_convert_summary_compliance_graph_status($cdata);
            $data = array_merge($data, $graphData);
        }





        $businessValuePieData = cfpr_get_value_graph(NULL, NULL, NULL, NULL, NULL);
        $businessValuePieArray = json_decode($businessValuePieData);


// make data ready for pie


        if (is_array($businessValuePieArray) && !empty($businessValuePieArray)) {
            $kept = 0;
            $notkept = 0;
            $repaired = 0;

            foreach ($businessValuePieArray as $val) {


                $kept+=$val[1];
                $notkept+=$val[3];
                $repaired+=$val[2];
            }
            $data['businessValuePie']['kept'] = abs($kept);
            $data['businessValuePie']['notkept'] = abs($notkept);
            $data['businessValuePie']['repaired'] = abs($repaired);
            $data['businessValuePie']['nodata'] = 0;
        } else {
            $data['businessValuePie']['kept'] = 0;
            $data['businessValuePie']['notkept'] = 0;
            $data['businessValuePie']['repaired'] = 0;
            $data['businessValuePie']['nodata'] = 100;
        }


        $data['allHost'] = cfpr_count_all_hosts();
        $data['redhost'] = cfpr_count_red_hosts();
        $data['yellowhost'] = cfpr_count_yellow_hosts();
        $data['greenhost'] = cfpr_count_green_hosts();
        $data['bluehost'] = cfpr_count_blue_hosts();

        $this->template->load('template', 'status', $data);
    }

    function _convert_summary_compliance_graph_status($rawData) {
        $convertedData = json_decode($rawData, true);

        $values = array();
        $data['graphSeries'] = array();
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        $keptSeries = array();
        $repairedSeries = array();
        $notKeptSeries = array();
        $nodataSeries = array();
        foreach ($convertedData as $key => $graphData) {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

// track the count parameter
// because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count'])) {
                $count[$graphData['start']] = $graphData['count'];
            }
            if (isset($graphData['start'])) {
                $start[$graphData['title']] = $graphData['start'];
            }

            $time = $graphData['start'] * 1000;
            $keptSeries[] = array($time, $graphData['kept']);
            $repairedSeries[] = array($time, $graphData['repaired']);
            $notKeptSeries[] = array($time, $graphData['notkept']);
            $nodataSeries[] = array($time, $nodata);
        }


        $data['graphSeries']['labels'] = json_encode($labels);
        $data['graphSeries']['values'] = json_encode($values);
        $data['graphSeries']['keptseries'] = json_encode($keptSeries);
        $data['graphSeries']['repairedseries'] = json_encode($repairedSeries);
        $data['graphSeries']['notkeptseries'] = json_encode($notKeptSeries);
        $data['graphSeries']['nodataseries'] = json_encode($nodataSeries);


        // these are two extra parameters that has to be accessible in the bar chart graph
        if (is_array($count) && !empty($count)) {
            $data['graphSeries']['count'] = json_encode($count);
        }

        if (is_array($start) && !empty($start)) {
            $data['graphSeries']['start'] = json_encode($start);
        }
        return $data;
    }

    function _convert_summary_compliance_graph($rawData) {
        $convertedData = json_decode($rawData, true);

        $values = array();
        $data['graphSeries'] = array();
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        foreach ($convertedData as $key => $graphData) {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

// track the count parameter
// because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count'])) {
                $count[$graphData['title']] = $graphData['count'];
            }
            if (isset($graphData['start'])) {
                $start[$graphData['title']] = $graphData['start'];
            }
        }


        $data['graphSeries']['labels'] = json_encode($labels);
        $data['graphSeries']['values'] = json_encode($values);

        // these are two extra parameters that has to be accessible in the bar chart graph
        if (is_array($count) && !empty($count)) {
            $data['graphSeries']['count'] = json_encode($count);
        }

        if (is_array($start) && !empty($start)) {
            $data['graphSeries']['start'] = json_encode($start);
        }
        return $data;
    }

    /**
     * returns JSON data object for compliance summary graph
     * @param <type> $env environment for the comliance summary graph
     */
    function getJsonComplianceSummary($env=NULL) {
        $cdata = cfpr_compliance_summary_graph($env);
        $convertedData = json_decode($cdata, true);
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        $keptSeries = array();
        $repairedSeries = array();
        $notKeptSeries = array();
        $nodataSeries = array();
        foreach ($convertedData as $key => $graphData) {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

            // track the count parameter
            // because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count'])) {
                $count[$graphData['start']] = $graphData['count'];
            }
            if (isset($graphData['start'])) {
                $start[$graphData['title']] = $graphData['start'];
            }

            $time = $graphData['start'] * 1000;
            $keptSeries[] = array($time, $graphData['kept']);
            $repairedSeries[] = array($time, $graphData['repaired']);
            $notKeptSeries[] = array($time, $graphData['notkept']);
            $nodataSeries[] = array($time, $nodata);
        }
        $jsonString = sprintf('{"graphdata":[
        {  
            "label":"kept",
            "data": %s         
        },
        {  
            "label":"repaired",
            "data": %s            
        },
        {   "label":"notkept",
            "data": %s
         },
         {   "label":"nodata",
            "data": %s
         }
        ],
        "countData":%s

        }', json_encode($keptSeries), json_encode($repairedSeries), json_encode($notKeptSeries), json_encode($nodataSeries), json_encode($count));

        echo trim($jsonString);

        return;
    }

    function engg() {
        $requiredjs = array(
            array('jit/jit-yc.js'),
            array('widgets/hostfinder.js'),
            array('widgets/classfinder.js'),
            array('widgets/policyfinder.js'),
            array('widgets/reportfinder.js'),
            array('widgets/cdpreportsfinder.js'),
            array('widgets/notes.js'),
            array('SmartTextBox.js'),
        );

        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->js($requiredjs);

        $bc = array(
            'title' => 'Engineering',
            'url' => 'welcome/engg',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => "Cfengine Mission Portal - engineering",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'all' => cfpr_count_all_hosts(),
            'r' => cfpr_count_red_hosts(),
            'y' => cfpr_count_yellow_hosts(),
            'g' => cfpr_count_green_hosts(),
            'b' => cfpr_count_blue_hosts()
        );

        // Summary meter for host
        $gdata = cfpr_summary_meter(null);
        $returnedData = $this->_convert_summary_compliance_graph($gdata);
        $data = array_merge($data, $returnedData);

        $this->template->load('template', 'engineering', $data);
    }

    function workingNotes() {
        $this->load->library('userdata');
        $data = array(
            'title' => "Cfengine Mission Portal - planning",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'users' => getonlineusernames(),
            'table' => $this->userdata->get_personal_working_notes('', 9999)
        );
        $this->template->load('template', 'notes/view_working_on_notes', $data);
    }

    function planning() {

        $bc = array(
            'title' => 'Planning',
            'url' => 'welcome/planning',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->load->library('userdata');
        $requiredjs = array(
            array('widgets/notes.js'),
            array('jquery.jcryption-1.1.min.js'),
            array('jquery.autogrowtextarea.js')
        );
        $this->carabiner->js($requiredjs);
        $this->carabiner->css('tabs-custom.css');
        $data = array(
            'title' => "Cfengine Mission Portal - planning",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'users' => getonlineusernames(),
            'working_notes' => $this->userdata->get_personal_working_notes(),
            'goals' => json_decode(cfpr_list_business_goals(),true)
        );
        $this->template->load('template', 'planning', $data);
    }
    
    function goals(){
         $bc = array(
            'title' => 'Goals',
            'url' => 'welcome/goals',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => "Cfengine Mission Portal - company goals",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'goals' => json_decode(cfpr_list_business_goals(),true)
        );
        $this->template->load('template', 'goals', $data);
    }

    function hosts($type=NULL) {
        if ($type == NULL) {
            redirect('welcome/engg');
            return;
        }
        $this->carabiner->js('jquery.tablesorter.min.js');
        $result = array();
        $this->load->library(array('cf_table'));
        $getparams = $this->uri->uri_to_assoc(4);
        $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if (!is_numeric($rows)) {
            $rows = 20;
        }
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        switch ($type) {
            case "red":
                $result = json_decode(cfpr_show_red_hosts($rows, $page_number), true);
                break;
            case "green":
                $result = json_decode(cfpr_show_green_hosts($rows, $page_number), true);
                break;
            case "yellow":
                $result = json_decode(cfpr_show_yellow_hosts($rows, $page_number), true);
                break;
            case "blue":
                $result = json_decode(cfpr_show_blue_hosts($rows, $page_number), true);
                break;
        }

        $table = "";
        $count = $result['meta']['count'];
        if ($count > 0) {
            //foreach ($result as $cols) {
            //array_push($columns, img(array('src' => 'images/' . $type . '.png', 'class' => 'align')) . anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            //array_push($columns,anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            // }
            $list = array_msort($result['data'], array('id' => SORT_ASC), true);
            $table = $this->cf_table->generateSingleColourHostTable($list, $type);
        }
       
        $bc = array(
            'title' => $this->lang->line($type )." hosts",
            'url' => 'welcome/hosts/' . $type,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'type' => $type,
            'title' => "Cfengine Mission Portal - " . $this->lang->line($type ) . " hosts",
            'tabledata' => $table,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' => $page_number,
            'number_of_rows' => $rows,
            'count' => $count
        );
        $this->template->load('template', 'hosts', $data);
    }

    function host($hostkey=NULL) {
        $hostkey_tobe_deleted = $this->input->post('delhost');
        if ($hostkey_tobe_deleted) {
            cfpr_delete_host($hostkey_tobe_deleted);
        }
        $getparams = $this->uri->uri_to_assoc(3);

        if (key_exists('delhost', $getparams)) {
            cfpr_delete_host($getparams['delhost']);
        }
        if (key_exists('type', $getparams)) {
            redirect('welcome/hosts/' . $getparams['type']);
        }


        if ($hostkey == NULL) {
            redirect('welcome/engg');
            return;
        }
        $this->load->library('cf_table');
        //loading required javascript files
        $requiredjs = array(
            array('jit/jit-yc.js'),
            array('jquery.form.js'),
            array('reportscontrol.js'),
            array('widgets/notes.js'),
            array('widgets/reportfinder.js')
        );
        $this->carabiner->js($requiredjs);
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));

        $bc = array(
            'title' => 'Host',
            'url' => 'welcome/host/' . $hostkey,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        if (is_null($hostkey)) {

            $hostkey = isset($_POST['hostkey']) ? $_POST['hostkey'] : "none";
        }
        $reports = json_decode(cfpr_select_reports(".*", 100));
        $hostname = cfpr_hostname($hostkey);
        $ipaddr = cfpr_ipaddr($hostkey);
        $username = isset($_POST['username']) ? $_POST['username'] : "";
        $comment_text = isset($_POST['comment_text']) ? $_POST['comment_text'] : "";
        $is_commented = trim(cfpr_get_host_noteid($hostkey));
        $op = isset($_POST['op']) ? $_POST['op'] : "";
        $allhosts = array();
        $jsonarr = json_decode(cfpr_select_hosts($hostkey, ".*", 100), true);
        $host = array();
        foreach ($jsonarr as $data) {
            $allhosts[$data['key']] = $data['id'];
        }

        $data = array(
            'hostkey' => $hostkey,
            'title' => "Cfengine Mission Portal - host " . $ipaddr,
            'hostname' => $hostname,
            'ipaddr' => $ipaddr,
            'is_commented' => $is_commented,
            'op' => $op,
            //'allreps' => array_combine($reports, $reports),
            'allhosts' => $allhosts,
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        $gdata = cfpr_host_meter($hostkey);

        $returnedData = $this->_convert_summary_compliance_graph($gdata);
        $data = array_merge($data, $returnedData);

        $this->template->load('template', 'host', $data);
    }

    function weakest_host() {

        $requiredjs = array(
            array('jit/jit-yc.js'),
            array('graphs/host-meter.js'),
        );
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js($requiredjs);
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));

        $getparams = $this->uri->uri_to_assoc(3);
        $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : 5);
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        $bc = array(
            'title' => 'Weakest Host',
            'url' => 'welcome/weakest_host',
            'isRoot' => false,
        );
        $this->breadcrumb->setBreadCrumb($bc);


        $gdata = cfpr_top_n_hosts("compliance", 1000, $rows, $page_number);
        $ret = array();
        if ($gdata) {
            $ret = json_decode($gdata, TRUE);
            foreach ($ret['data'] as $index => $val) {
                $rawData = cfpr_host_meter($val['key']);
                $graphData = $this->_convert_summary_compliance_graph($rawData);
                $ret['data'][$index] = array_merge($ret['data'][$index], $graphData);
            }
        }

        $data = array(
            'title' => "Cfengine Mission Portal - weakest hosts ",
            'ret' => $ret,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' => $page_number,
            'number_of_rows' => $rows
        );
        $this->template->load('template', 'topN', $data);
    }

    function services() {
        $bc = array(
            'title' => 'Services',
            'url' => 'welcome/services',
            'isRoot' => false
        );
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->load->library('cf_table');
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title_header' => "service catalogue",
            'title' => "Cfengine Mission Portal - service catalogue ",
            'services' => cfpr_list_all_bundles("agent"),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'services', $data);
    }

    function license() {
        $this->carabiner->js('/widgets/licensemeter.js');
        $bc = array(
            'title' => 'License',
            'url' => 'welcome/license',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);
        try {
            $expirydate = strtotime(cfpr_getlicense_expiry());
            $startDate = cfpr_getlicense_installtime();
            //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
            $datediff = $expirydate - $startDate;
            $totaldays = floor($datediff / (60 * 60 * 24));
            $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
            $pbarvalue ="";
            if(!($totaldays < 0))
            { 
               $pbarvalue=floor(($dayspassed / $totaldays) * 100);
            }
            
        } catch (Exception $e) {
            log_message('license error:' . $e->getMessage());
        }
       
        $data = array(
            'title' => "Cfengine Mission Portal - license usage status ",
            'ret2' => cfpr_getlicenses_promised(),
            'ret3' => cfpr_getlicenses_granted(),
            'started' =>date('D F d h:m:s Y',$startDate),
            'expiry' => cfpr_getlicense_expiry(),
            'txt' => cfpr_getlicense_summary(),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'pbarvalue' => $pbarvalue,
            'daysleft' => $totaldays - $dayspassed,
        );
        $this->template->load('template', 'license', $data);
    }

    function classes($key = NULL) {
        $this->carabiner->js('jquery.tablesorter.min.js');
        $bc = array(
            'title' => 'Classes',
            'url' => 'welcome/classes/'.$key,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $hostkey = $key;
        $name = ".*";
        $regex = 1;
        $hash = NULL;
        $host = NULL;
        $addr = NULL;
        $tago = 0;
        $data = array(
            'title_header' => "classes",
            'title' => "Cfengine Mission Portal - classes ",
            'ret' => json_decode(cfpr_report_classes($hostkey, $name, $regex, NULL, 1000, 1), true),
             'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'classes', $data);
    }

    function cfeditor() {
        redirect('/cfeditor/');
    }

    function search() {
        $params = $this->uri->uri_to_assoc(3);
        redirect('/search/' . $this->uri->assoc_to_uri($params));
    }

    function body() {



        $this->carabiner->css('tabs-custom.css');
        $getparams = $this->uri->uri_to_assoc(3);
        $body = isset($getparams['body']) ? $getparams['body'] : $this->input->post('search');
        $type = isset($getparams['type']) ? $getparams['type'] : $this->input->post('type');



        $this->load->library('cf_table');



        $data = array(
            'title_header' => "body " . $body,
            'title' => "Cfengine Mission Portal - classes ",
            'nav_text' => "show : body",
            'status' => "current",
            'allbodies' => json_decode(utf8_encode(cfpr_list_bodies(".*", $type)), TRUE),
            'def' => json_decode(utf8_encode(cfpr_get_promise_body($body, $type)), TRUE),
            'type' => $type,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'body', $data);
    }

    /**
     *
     * @param <type> $currentclass
     * @return <type>
     * for listing a host calls the hostlist view at the end
     */
    function listhost() {

        $requiredjs = array(
            array('jquery.form.js'),
            array('widgets/classtags.js'),
            array('widgets/hostfinder.js'),
            array('widgets/classfinder.js'),
            array('widgets/policyfinder.js'),
            array('widgets/reportfinder.js')
        );
        $this->carabiner->js($requiredjs);

        // $this->template->set('injected_item', implode("", $scripts));

        $bc = array(
            'title' => 'Host List',
            'url' => 'welcome/listhost',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $res = cfpr_class_cloud(NUll);
        $arr = json_decode($res);

        //for creating the initial table of hosts as cfpr_select_hosts return the json data
        //$result = json_decode(cfpr_select_hosts("none", ".*", 100), true);
        $classes = cfpr_report_classes(NULL, NULL, true, NULL, NULL, NULL);
        $data = array(
            'title' => "Cfengine Mission Portal - Filter",
            'title_header' => "Filter Host",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'classes' => autocomplete($classes, "Class Context"),
            'hoststable' => host_only_table($arr->hosts)
        );
        $this->template->load('template', 'hostlist', $data);
    }

    function ajaxlisthost($currentclass=NULL) {
        $filters = $this->input->post('filter');
        if ($filters) {
            $classlist = implode(",", $filters);
            $arr = json_decode(cfpr_class_cloud($classlist));
            $this->session->set_userdata('lastclasslist', $classlist);
            echo host_only_table($arr->hosts);
        } else {
            $arr = json_decode(cfpr_class_cloud(NULL));
            $this->session->set_userdata('lastclasslist', NULL);
            echo host_only_table($arr->hosts);
        }
    }

    function pulse_vitals() {
        $scripts = array('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jquery.form.js"> </script>');

        $this->template->set('injected_item', implode("", $scripts));

        $bc = array(
            'title' => 'Host List',
            'url' => 'welcome/listhost',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);

        //for creating the initial table of hosts as cfpr_select_hosts return the json data
        $cells = array();
        $result = json_decode(cfpr_select_hosts("none", ".*", 100), true);
        if (count($result) > 0) {
            foreach ($result as $cols) {
                array_push($cells, anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            }
        }

        $data = array(
            'title' => "Cfengine Mission Portal - Filter",
            'title_header' => "Filter Host",
            'tabledata' => $cells,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'hostlist', $data);
    }

}

/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */