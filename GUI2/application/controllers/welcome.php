<?php

class Welcome extends Cf_Controller {

    function __construct() {
        parent::__construct();
        parse_str($_SERVER['QUERY_STRING'], $_GET);
        $this->load->helper('form');
        $this->load->library('table');
    }

    function index() {
        $bc = array(
            'title' => 'Dash Board',
            'url' => 'welcome/index',
            'isRoot' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $all = cfpr_count_all_hosts();
        $r = cfpr_count_red_hosts();
        $y = cfpr_count_yellow_hosts();
        $g = cfpr_count_green_hosts();

        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'hostsummary.js"></script>
            ',
            '<style type="text/css">
            /*<![CDATA[*/


            .node{
                background-color: #ccccff;
                border: 1px;
                -moz-box-shadow: 3px 3px 5px #888;
                -webkit-box-shadow: 3px 3px 5px #888;
                box-shadow: 3px 3px 5px #888;

            }


            /*]]>*/
        </style>
         ');

        $this->template->set('injected_item', implode("", $scripts));

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'all' => $all,
            'r' => $r,
            'y' => $y,
            'g' => $g
        );
        $this->template->load('template', 'index', $data);
    }

    function status() {
        $bc = array(
            'title' => 'All host',
            'url' => 'welcome/status',
            'isRoot' => false
        );

        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'reportscontrol.js"> </script>');

        $this->template->set('injected_item', implode("", $scripts));

        $this->breadcrumb->setBreadCrumb($bc);
        //$reports = json_decode(cfpr_select_reports(".*", 100));
        $data = array(
            'title' => "Cfengine Mission Portal - engineering status",
            'title_header' => "engineering status",
            'nav_text' => "Status : hosts",
            'status' => "current",
            'ret1' => cfpr_getlicense_owner(),
            'all' => cfpr_count_all_hosts(),
            'r' => cfpr_count_red_hosts(),
            'y' => cfpr_count_yellow_hosts(),
            'g' => cfpr_count_green_hosts(),
            'jsondata' => $this->_get_jsondata_for_report_control(cfpr_select_reports(".*", 100)),
            //'allreps' => array_combine($reports, $reports),
            'allSppReps' => cfpr_cdp_reportnames(),
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        $gdata = cfpr_summary_meter();
        $returnedData = $this->_convert_summary_compliance_graph($gdata);
        $data = array_merge($data, $returnedData);
        $this->template->load('template', 'status', $data);
    }

    function _convert_summary_compliance_graph($rawData) {
        $convertedData = json_decode($rawData, true);

        $values = array();
        $data['graphSeries'] = array();
        $labels = array('kept', 'repaired', 'not kept');
        foreach ($convertedData as $key => $graphData) {

            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept']));
        }


        $data['graphSeries']['labels'] = json_encode($labels);
        $data['graphSeries']['values'] = json_encode($values);
        return $data;
    }

    /**
     * For getting json data for report report control using info vis sunbrust
     * @return <json>
     * @author sudhir
     * @param <array>
     *
     */
    function _get_jsondata_for_report_control($reportlist) {
        $reports = json_decode($reportlist);
        $adjacencies = array();
        $rootnode = array("id" => "node0", "name" => "", "data" => array("\$type" => "none"));
        $control = array();

        $i = 1;
        foreach ($reports as $report) {
            $node = array(
                'nodeTo' => 'node' . $i,
                'data' => array("\$type" => 'none')
            );
            $adjacencies[$i - 1] = $node;
            $i++;
        }
        $rootnode['adjacencies'] = $adjacencies;
        array_push($control, $rootnode);

        $i = 1;
        foreach ($reports as $report) {
            $node_property = array(
                'id' => 'node' . $i,
                'name' => $report,
                'data' => array("\$angularwidth" => "20", "\$color" => $this->_rand_colorCode(), "\$height" => 90 + $i),
                'adjacencies' => array()
            );
            array_push($control, $node_property);
            $i++;
        }
        //print_r($nodelist);
        //print_r(json_encode($control));
        return json_encode($control);
    }

    /**
     * For generating random color
     * @author sudhir pandey
     * @return <string>
     *
     */
    function _rand_colorCode() {
        $r = dechex(mt_rand(0, 255)); // generate the red component
        $g = dechex(mt_rand(0, 255)); // generate the green component
        $b = dechex(mt_rand(0, 255)); // generate the blue component
        $rgb = $r . $g . $b;
        if ($r == $g && $g == $b) {
            $rgb = substr($rgb, 0, 3); // shorter version
        }
        if (strlen($rgb) == 4) {
            $rgb = $rgb . rand(10, 99);
        } else if (strlen($rgb) == 5) {
            $rgb = $rgb . rand(0, 9);
        }
        return '#' . $rgb;
    }

    function helm() {
        $bc = array(
            'title' => 'Configure',
            'url' => 'welcome/helm',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => "Cfengine Mission Portal - control",
            'title_header' => "control",
            'nav_text' => "Planning : menu",
            'planning' => "current",
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'helm', $data);
    }

    function knowledge() {
        $bc = array(
            'title' => 'Knowledge Map',
            'url' => 'welcome/knowledge',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $topic = isset($getparams['topic']) ? $getparams['topic'] : $this->input->post('topic');
        $pid = isset($getparams['pid']) ? $getparams['pid'] : $this->input->post('pid');
        if (!$pid)
            $pid = cfpr_get_pid_for_topic("", "system policy");

        $data = array(
            'search' => $search,
            'topic' => $topic,
            'pid' => $pid,
            'title' => "Cfengine Mission Portal - Knowledge bank",
            'title_header' => "Knowledge bank",
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $gdata = cfpr_get_knowledge_view($pid, '');


        $data['graphdata'] = ($gdata);
        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>
            ',
            '<style type="text/css">
            /*<![CDATA[*/


            .node{
                background-color: #ccccff;
                border: 1px;
                -moz-box-shadow: 3px 3px 5px #888;
                -webkit-box-shadow: 3px 3px 5px #888;
                box-shadow: 3px 3px 5px #888;

            }


            /*]]>*/
        </style>
',
            '<link href="' . get_cssdir() . 'jquery.fancybox-1.3.1.css" rel="stylesheet" media="screen" />');


        $this->template->set('injected_item', implode("", $scripts));
        $this->template->load('template', 'knowledge', $data);
    }

    function hosts($type) {
        $result = array();
        switch ($type) {
            case "red":
                $result = json_decode(cfpr_show_red_hosts(), true);
                break;
            case "green":
                $result = json_decode(cfpr_show_green_hosts(), true);
                break;
            case "yellow":
                $result = json_decode(cfpr_show_yellow_hosts(), true);
                break;
        }

        $columns = array();
        if (count($result) > 0) {
            foreach ($result as $cols) {
                array_push($columns, img(array('src' => 'images/' . $type . '.png', 'class' => 'align')) . anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            }
        }
        $bc = array(
            'title' => "$type hosts",
            'url' => 'welcome/hosts/' . $type,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'type' => $type,
            'title_header' => "engineering status",
            'title' => "Cfengine Mission Portal - " . $type . " hosts",
            'tabledata' => $columns,
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        $this->template->load('template', 'hosts', $data);
    }

    function host($hostkey=NULL) {


        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'reportscontrol.js"> </script>');

        $this->template->set('injected_item', implode("", $scripts));

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
        $is_commented = cfpr_get_host_noteid($hostkey);
        $op = isset($_POST['op']) ? $_POST['op'] : "";
        $allhosts = array();
        $jsonarr = json_decode(cfpr_select_hosts($hostkey, ".*", 100), true);
        $host = array();
        foreach ($jsonarr as $data) {
            $allhosts[$data['key']] = $data['id'];
        }

        $data = array(
            'hostkey' => $hostkey,
            'title_header' => "host " . $hostname,
            'title' => "Cfengine Mission Portal - host " . $ipaddr,
            'nav_text' => "Status : host",
            'status' => "current",
            'hostname' => $hostname,
            'ipaddr' => $ipaddr,
            'is_commented' => $is_commented,
            'op' => $op,
            'allreps' => array_combine($reports, $reports),
            'allhosts' => $allhosts,
            'jsondata' => $this->_get_jsondata_for_report_control(cfpr_select_reports(".*", 100)),
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        $gdata = cfpr_host_meter($hostkey);

        $returnedData = $this->_convert_summary_compliance_graph($gdata);
        $data = array_merge($data, $returnedData);

        $this->template->load('template', 'host', $data);
    }

    function weakest_host() {
        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'reportscontrol.js"> </script>
                ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jquery-ui-1.8.9.custom.min.js"> </script>
                ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'graphs/host-meter.js"> </script>'
        );
        $this->template->set('injected_item', implode("", $scripts));

        $bc = array(
            'title' => 'Weakest Host',
            'url' => 'welcome/weakest_host',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        

        $gdata = cfpr_top_n_hosts("compliance", 1000);
        $ret = array();
        if ($gdata) {

            $ret = json_decode($gdata, TRUE);
            // get the value
            
            foreach ($ret as $index => $val) {
                $rawData = cfpr_host_meter($val['key']);
                $graphData = $this->_convert_summary_compliance_graph($rawData);
                $ret[$index] = array_merge($ret[$index],$graphData);
            }
        }

        $data = array(
            'title_header' => "weakest hosts",
            'title' => "Cfengine Mission Portal - weakest hosts ",
            'nav_text' => "Status : weakest hosts",
            'status' => "current",
            'ret' => $ret,
            'breadcrumbs' => $this->breadcrumblist->display()

        );
        $this->template->load('template', 'topN', $data);
    }

    function services() {
        $bc = array(
            'title' => 'Services',
            'url' => 'welcome/services',
            'isRoot' => false
        );
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
        $data = array(
            'title_header' => "license usage status",
            'title' => "Cfengine Mission Portal - license usage status ",
            'nav_text' => "License : status",
            'planning' => "current",
            'ret2' => cfpr_getlicenses_promised(),
            'ret3' => cfpr_getlicenses_granted(),
            'expiry' => cfpr_getlicense_expiry(),
            'txt' => cfpr_getlicense_summary()
        );
        $this->template->load('template', 'license', $data);
    }

    function classes($key = NULL) {
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
            'nav_text' => "Status : classes",
            'status' => "current",
            'ret' => cfpr_report_classes($hostkey, $name, $regex, NULL),
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
        $getparams = $this->uri->uri_to_assoc(3);
        $body = isset($getparams['body']) ? $getparams['body'] : $this->input->post('search');
        $type = isset($getparams['type']) ? $getparams['type'] : $this->input->post('type');
        $data = array(
            'title_header' => "body " . $body,
            'title' => "Cfengine Mission Portal - classes ",
            'nav_text' => "show : body",
            'status' => "current",
            'allbodies' => cfpr_list_bodies(".*", $type),
            'def' => cfpr_get_promise_body($body, $type),
            'type' => $type
        );
        $this->template->load('template', 'body', $data);
    }

    function listhost() {
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