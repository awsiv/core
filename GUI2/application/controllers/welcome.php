<?php

class Welcome extends Cf_Controller
{

    function __construct()
    {
        parent::__construct();
        parse_str($_SERVER['QUERY_STRING'], $_GET);
        $this->load->helper('form');
        $this->load->library(array('table', 'cf_table'));
        $this->load->model(array('host_model', 'environment_model', 'report_model','goals_model','summary_model'));
    }

    function index()
    {
        $bc = array(
            'title' => $this->lang->line('mission_portal_title'),
            'url' => 'welcome/index',
            'isRoot' => true
        );

        $this->breadcrumb->setBreadCrumb($bc);
        $this->carabiner->js('widgets/licensemeter.js');
        try
        {
            $expirydate = strtotime(cfpr_getlicense_expiry());
            $startDate = cfpr_getlicense_installtime();
            //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
            $datediff = $expirydate - $startDate;
            if ($datediff > 0)
            {
                $totaldays = floor($datediff / (60 * 60 * 24));
                $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
                $pbarvalue = "";
                if ($totaldays > 0)
                {
                    $pbarvalue = floor(($dayspassed / $totaldays) * 100);
                }
                $daysleft = $totaldays - $dayspassed;
            }
            else
            {
                $pbarvalue = 100;
                $daysleft = 0;
            }
        }
        catch (Exception $e)
        {
            log_message('license error:' . $e->getMessage());
        }

        $data = array(
            'title' => $this->lang->line('mission_portal_title'),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'pbarvalue' => $pbarvalue,
            'daysleft' => $daysleft,
            'mp_auth_mode' => $this->setting_lib->get_authentication_mode(),
            'user_auth_mode' => $this->session->userdata('mode')
        );
        $this->template->load('template', 'index', $data);
    }

    /**
     * Status Page related code
     */
    
    function status()
    {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_status'),
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
        $goals = array();
        $goalsError = false;
        try
        {
            $goals = $this->goals_model->getAllGoals();
        }
        catch (Exception $e)
        {
            // if exception is thrown here just pass a empty array to goals
            // errors are already logged from the model
            $goalsError = true;
        }

        try
        {
            $data = array(
                'title' => $this->lang->line('mission_portal_title') . "-" . $this->lang->line('breadcrumb_status'),
                'breadcrumbs' => $this->breadcrumblist->display(),
                'goals' => $goals,
                'goalsError' => $goalsError
            );

            // Summary meter for host
            $gdata = $this->summary_model->getSummaryMeter();
            $returnedData = $this->summary_model->getConvertedSummaryComplianceGraphData($gdata);
            $data = array_merge($data, $returnedData);

            $envList = $this->environment_model->getEnvironmentList($this->session->userdata('username'));

            //$envListArray = json_decode($envList);

            $data['envList'] = $envList;

            $cdata = $this->summary_model->getComplianceSummaryData();
            if ($cdata)
            {
                $graphData['compliance_summary'] = $this->summary_model->getConvertedSummaryComplianceGraphStatus($cdata);
                $data = array_merge($data, $graphData);
            }
            
            
            $pieChartData = $this->summary_model->getBusinessPieChartData();
            $data = array_merge($data, $pieChartData);

            $username = &$this->session->userdata('username');
            $data['allHost'] = $this->host_model->getHostCount($username);
            $data['redhost'] = $this->host_model->getHostCount($username, 'red');
            $data['yellowhost'] = $this->host_model->getHostCount($username, 'yellow');
            $data['greenhost'] = $this->host_model->getHostCount($username, 'green');
            $data['bluehost'] = $this->host_model->getHostCount($username, 'blue');
            $data['blackhost'] = $this->host_model->getHostCount($username, 'black');
        }
        catch (CFModExceptionRBAC $e)
        {
            show_error($this->lang->line('rbac_exception'), 401);
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }

        $this->template->load('template', 'status', $data);
    }

    /**
     * returns JSON data object for compliance summary graph
     * @param <type> $env environment for the comliance summary graph
     */
    function getJsonComplianceSummary($env = NULL)
    {
        $cdata = cfpr_compliance_summary_graph($env);
        $convertedData = json_decode($cdata, true);
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        $keptSeries = array();
        $repairedSeries = array();
        $notKeptSeries = array();
        $nodataSeries = array();
        foreach ($convertedData as $key => $graphData)
        {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

            // track the count parameter
            // because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count']))
            {
                $count[$graphData['start']] = $graphData['count'];
            }
            if (isset($graphData['start']))
            {
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

    function workingNotes()
    {
        $this->load->library('userdata');
        $params = $this->uri->uri_to_assoc(3);
        $rows = isset($params['rows']) ? $params['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if (is_numeric($rows))
        {
            $rows = (int) $rows;
        }
        else
        {
            $rows = 20;
        }
        $page = isset($params['page']) ? intval($params['page'], 10) : 1;
        $totalnotes = $this->userdata->count_personal_working_notes();
        $skip = (int) ($rows * ($page - 1));
        //var_dump($skip);
        $data = array(
            'title' => "CFEngine Mission Portal - Planning",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'users' => getonlineusernames(),
            'table' => $this->userdata->get_personal_working_notes('', $rows, $skip),
            'rows_per_page' => $rows,
            'current_page' => $page,
            'total' => $totalnotes,
        );
        $this->template->load('template', 'notes/view_working_on_notes', $data);
    }

    function planning()
    {

        $bc = array(
            'title' => $this->lang->line('breadcrumb_planning'),
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
        $goals = array();
        $goalsError = false;
        try
        {
            $goals = $this->goals_model->getAllGoals();
        }
        catch (Exception $e)
        {
            // errors are already logged from the model
            $goalsError = true;
        }
        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_planning'),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'users' => getonlineusernames(),
            'working_notes' => $this->userdata->get_personal_working_notes(),
            'goals' => $goals,
            'goalsError' => $goalsError
        );
        $this->template->load('template', 'planning', $data);
    }

    function goals()
    {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_goals'),
            'url' => 'welcome/goals',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $goals = array();
        $goalsError = false;
        try
        {
            $goals = $this->goals_model->getAllGoals();
        }
        catch (Exception $e)
        {
            // errors are already logged from the model
            $goalsError = true;
        }
        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_goals'),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'goals' => $goals,
            'goalsError' => $goalsError
        );
        $this->template->load('template', 'goals', $data);
    }

    function hosts($colour = NULL)
    {
        if ($colour == NULL)
        {
            redirect('engineering');
            return;
        }
        $this->carabiner->js('jquery.tablesorter.min.js');
        $result = array();
        $this->load->library(array('cf_table'));
        $getparams = $this->uri->uri_to_assoc(4);
        $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if (!is_numeric($rows))
        {
            $rows = 20;
        }
        $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
        $includes = isset($getparams['includes']) ? explode(',', urldecode($getparams['includes'])) : array('.*');
        $excludes = isset($getparams['excludes']) ? explode(',', urldecode($getparams['excludes'])) : array('');

        try
        {
            switch ($colour)
            {
                case "red":
                    $result = $this->host_model->getHostByColor('red', $this->session->userdata('username'), $includes, $excludes, $rows, $page_number);
                    break;
                case "green":
                    $result = $this->host_model->getHostByColor('green', $this->session->userdata('username'), $includes, $excludes, $rows, $page_number);
                    break;
                case "yellow":
                    $result = $this->host_model->getHostByColor('yellow', $this->session->userdata('username'), $includes, $excludes, $rows, $page_number);
                    break;
                case "blue":
                    $result = $this->host_model->getHostByColor('blue', $this->session->userdata('username'), $includes, $excludes, $rows, $page_number);
                    break;
                case "black":
                    $result = $this->host_model->getHostByColor('black', $this->session->userdata('username'), $includes, $excludes, $rows, $page_number);
                    break;
            }
        }
        catch (Exception $e)
        {

            show_error($e->getMessage(), 500);
        }

        $table = "";
        $count = $result['meta']['count'];
        if ($count > 0)
        {
            //foreach ($result as $cols) {
            //array_push($columns, img(array('src' => 'images/' . $type . '.png', 'class' => 'align')) . anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            //array_push($columns,anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            // }
            $list = array_msort($result['data'], array('id' => SORT_ASC), true);
            $table = $this->cf_table->generateSingleColourHostTable($list, $colour);
        }

        $bc = array(
            'title' => $this->lang->line($colour) . " hosts",
            'url' => 'welcome/hosts/' . $colour,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'type' => $colour,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line($colour) . " hosts",
            'tabledata' => $table,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' => $page_number,
            'number_of_rows' => $rows,
            'count' => $count,
            'params' => get_parameters_without_paging($getparams)
        );
        $this->template->load('template', 'hosts', $data);
    }

    function host($hostkey = NULL)
    {
        $hostkey_tobe_deleted = $this->input->post('delhost');
        $username = $this->session->userdata('username');
        $getparams = $this->uri->uri_to_assoc(3);
        $this->load->model('note_model');

        try
        {
            if ($hostkey_tobe_deleted)
            {
                $this->host_model->deleteHost($username, $hostkey_tobe_deleted);
            }
            if (key_exists('delhost', $getparams))
            {
                $this->host_model->deleteHost($username, $getparams['delhost']);
            }
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }


        if (key_exists('type', $getparams))
        {
            redirect('welcome/hosts/' . $getparams['type']);
        }


        if ($hostkey == NULL)
        {
            redirect('engineering');
            return;
        }
        $this->load->library('cf_table');
        //loading required javascript files
        $requiredjs = array(
            array('jit/jit-yc.js'),
            array('jquery.form.js'),
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

        if (is_null($hostkey))
        {

            $hostkey = isset($_POST['hostkey']) ? $_POST['hostkey'] : "none";
        }
        $username = isset($_POST['username']) ? $_POST['username'] : "";
        $comment_text = isset($_POST['comment_text']) ? $_POST['comment_text'] : "";
        //$reports = json_decode(cfpr_select_reports(null));
        try
        {
            $hostname = $this->host_model->getHostName($this->session->userdata('username'), $hostkey);
            $ipaddr = $this->host_model->getHostIp($this->session->userdata('username'), $hostkey);
            ;

            $is_commented = trim($this->note_model->hasNotes($hostkey));
            $op = isset($_POST['op']) ? $_POST['op'] : "";

            $tempvar = explode("=", $hostkey);
            $hostclass = "PK_SHA_" . $tempvar[1];

            $data = array(
                'hostkey' => $hostkey,
                'hostclass' => $hostclass,
                'title' => $this->lang->line('mission_portal_title') . " - host " . $ipaddr,
                'hostname' => $hostname,
                'ipaddr' => $ipaddr,
                'is_commented' => $is_commented,
                'op' => $op,
                'breadcrumbs' => $this->breadcrumblist->display(),
                'last' => $this->host_model->getLastUpdate($this->session->userdata('username'), $hostkey),
                'class' => $this->host_model->getHostVariable($this->session->userdata('username'), $hostkey, "sys", "ostype"),
                'flavour' => $this->host_model->getHostVariable($this->session->userdata('username'), $hostkey, "sys", "flavour"),
                'rel' => $this->host_model->getHostVariable($this->session->userdata('username'), $hostkey, "sys", "release"),
                'load' => $this->host_model->getHostVariable($this->session->userdata('username'), $hostkey, "mon", "av_loadavg"),
                'free' => $this->host_model->getHostVariable($this->session->userdata('username'), $hostkey, "mon", "av_diskfree"),
                'speed' => $this->host_model->getNetWorkSpeed($this->session->userdata('username'), $hostkey),
                'colour' => $this->host_model->getHostColor($this->session->userdata('username'), $hostkey),
                'tableData' => $this->report_model->getPromisesNotKeptSummary($this->session->userdata('username'), $hostkey, NULL, ".*", NULL, NULL, array(), array(), NULL, NULL)
            );
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }

        $gdata=$this->host_model->getHostMeter($this->session->userdata('username'), $hostkey);
        $returnedData= $this->summary_model->getConvertedSummaryComplianceGraphData($gdata);
        $data = array_merge($data, $returnedData);

        $this->template->load('template', 'host', $data);
    }

    function services()
    {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_service_catalogue'),
            'url' => 'welcome/services',
            'isRoot' => false
        );
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->load->library('cf_table');
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - Service Catalogue",
            'services' => cfpr_bundle_agent_goals($this->session->userdata('username')),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'services', $data);
    }

    function license()
    {
        $this->carabiner->js('/widgets/licensemeter.js');
        $bc = array(
            'title' => $this->lang->line('breadcrumb_license'),
            'url' => 'welcome/license',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);
        try
        {
            $expirydate = strtotime(cfpr_getlicense_expiry());
            $startDate = cfpr_getlicense_installtime();
            //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
            $datediff = $expirydate - $startDate;
            if ($datediff > 0)
            {
                $totaldays = floor($datediff / (60 * 60 * 24));
                $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
                $pbarvalue = "";
                if (!($totaldays < 0))
                {
                    $pbarvalue = floor(($dayspassed / $totaldays) * 100);
                }
                $daysleft = $totaldays - $dayspassed;
            }
            else
            {
                $pbarvalue = 100;
                $daysleft = 0;
            }
        }
        catch (Exception $e)
        {
            log_message('license error:' . $e->getMessage());
        }

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - License Usage Status ",
            'ret2' => cfpr_getlicenses_promised(),
            'ret3' => cfpr_getlicenses_granted(),
            'started' => date('D F d h:m:s Y', $startDate),
            'expiry' => cfpr_getlicense_expiry(),
            'txt' => cfpr_getlicense_summary(),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'pbarvalue' => $pbarvalue,
            'daysleft' => $daysleft
        );
        $this->template->load('template', 'license', $data);
    }

    function classes($key = NULL)
    {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_classes'),
            'url' => 'welcome/classes/' . $key,
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
        try
        {
            $data = array(
                'title_header' => "classes",
                'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_classes'),
                'ret' => json_decode(cfpr_report_classes($hostkey, $name, $regex, NULL, NULL, "last-seen", true, 1000, 1), true),
                'breadcrumbs' => $this->breadcrumblist->display(),
                'hostname' => $this->host_model->getHostName($this->session->userdata('username'), $hostkey)
            );
            $this->template->load('template', 'classes', $data);
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }
    }

    function cfeditor()
    {
        redirect('/cfeditor/');
    }

    function search()
    {
        $params = $this->uri->uri_to_assoc(3);
        redirect('/search/' . $this->uri->assoc_to_uri($params));
    }

    function body()
    {
        $this->carabiner->css('tabs-custom.css');
        $getparams = $this->uri->uri_to_assoc(3);
        $body = isset($getparams['body']) ? $getparams['body'] : $this->input->post('search');
        $type = isset($getparams['type']) ? $getparams['type'] : $this->input->post('type');

        $this->load->library('cf_table');

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - Promise Body ",
            'status' => "current",
            'allbodies' => json_decode(utf8_encode(cfpr_body_list($type, ".*")), TRUE),
            'def' => json_decode(utf8_encode(cfpr_body_details($type, $body)), TRUE),
            'type' => $type,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'body', $data);
    }

}

/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */
