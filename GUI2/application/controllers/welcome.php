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

        $all = cfpr_count_all_hosts();
        $r = cfpr_count_red_hosts();
        $y = cfpr_count_yellow_hosts();
        $g = cfpr_count_green_hosts();

        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'graphs/host-meter.js"> </script>
                    ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'widgets/licensemeter.js"></script>
            '
        );

        $expirydate = strtotime(cfpr_getlicense_expiry());
        $startDate = cfpr_getlicense_installtime();
        //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
        $datediff = $expirydate - $startDate;
        $totaldays = floor($datediff / (60 * 60 * 24));
        $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
        $pbarvalue = floor(($dayspassed / $totaldays) * 100);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'all' => $all,
            'r' => $r,
            'y' => $y,
            'g' => $g,
            'pbarvalue' => $pbarvalue,
            'daysleft' => $totaldays - $dayspassed,
        );

        $gdata = cfpr_compliance_summary_graph(null);
        if ($gdata) {
            $graphData = $this->_convert_summary_compliance_graph($gdata);
            $data = array_merge($data, $graphData);
        }


        $this->template->set('injected_item', implode("", $scripts));


        $this->template->load('template', 'index', $data);
    }

    function status() {
        $bc = array(
            'title' => 'engineering status',
            'url' => 'welcome/status',
            'isRoot' => false
        );

        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/Extras/excanvas.js"></script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'graphs/host-meter.js"> </script>
                    ',
            '<link href="' . get_cssdir() . 'jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
             ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.js"> </script>
                ',
            'pie' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.pie.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'reportscontrol.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . '/widgets/notes.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . '/jquery.curvycorners.packed.js"> </script>',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'reportcontrol2.js"> </script>');

        $this->template->set('injected_item', implode("", $scripts));

        $this->breadcrumb->setBreadCrumb($bc);
        //$reports = json_decode(cfpr_select_reports(".*", 100));
        //$goals=cfpr_list_business_goals();
        $data = array(
            'title' => "Cfengine Mission Portal-engineering status",
            //'title_header' => "engineering status",
            //'nav_text' => "Status : hosts",
            //'status' => "current",
            //'ret1' => cfpr_getlicense_owner(),
            //'all' => cfpr_count_all_hosts(),
            //'r' => cfpr_count_red_hosts(),
            //'y' => cfpr_count_yellow_hosts(),
            //'g' => cfpr_count_green_hosts(),
            //'jsondata' => $this->_get_jsondata_for_report_control(cfpr_select_reports(".*", 100)),
            //'jsondata2' => create_json_node_for_report_control(),
            //'allreps' => array_combine($reports, $reports),
            //'allSppReps' => cfpr_cdp_reportnames(),
            'breadcrumbs' => $this->breadcrumblist->display(),
            'goals' => json_decode(cfpr_list_business_goals())
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
            $graphData['compliance_summary'] = $this->_convert_summary_compliance_graph($cdata);
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

        $this->template->load('template', 'status', $data);
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
        if ($cdata) {
            $graphData['compliance_summary'] = $this->_convert_summary_compliance_graph($cdata);

            $jsonString = '{"graphdata":[{ "color":["#A3DF00", "#EEEE00", "#D43030", "#5C5858"],';
            $jsonString .= '"label":' . $graphData['compliance_summary']['graphSeries']['labels'] . ",";
            $jsonString .= '"values":' . $graphData['compliance_summary']['graphSeries']['values'] . "}],";

            // count data
            $jsonString .='"countData":' . $graphData['compliance_summary']['graphSeries']['count'] . ",";
            $jsonString .='"startData":' . $graphData['compliance_summary']['graphSeries']['start'] . "}";


            echo trim($jsonString);
            return;
        }
        return;
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

    function engg() {
        $requiredjs = array(
            array('jit/jit-yc.js'),
            array('widgets/hostfinder.js'),
            array('widgets/classfinder.js'),
            array('widgets/policyfinder.js'),
            array('widgets/reportfinder.js')
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
            case "blue":
                $result = json_decode(cfpr_show_blue_hosts(), true);
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
            'title' => "Cfengine Mission Portal - " . $type . " hosts",
            'tabledata' => $columns,
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        $this->template->load('template', 'hosts', $data);
    }

    function host($hostkey=NULL) {

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
        $reports = (array) json_decode(cfpr_select_reports(".*", 100));
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
            //'title_header' => "host " . $hostname,
            'title' => "Cfengine Mission Portal - host " . $ipaddr,
            //'nav_text' => "Status : host",
            //'status' => "current",
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
       

        $gdata = cfpr_top_n_hosts("compliance", 1000,$rows,$page_number);
        $ret = array();
        if ($gdata) {
            $ret = json_decode($gdata, TRUE);
            foreach ($ret['data'] as $index=>$val) {
                $rawData = cfpr_host_meter($val['key']);
                $graphData = $this->_convert_summary_compliance_graph($rawData);
                $ret['data'][$index] = array_merge($ret['data'][$index], $graphData);
                }
        }

        $data = array(
            'title' => "Cfengine Mission Portal - weakest hosts ",
            'ret' => $ret,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'current' =>  $page_number,
            'number_of_rows'=>$rows
        );
        $this->template->load('template', 'topN', $data);
    }

    function services() {
        $bc = array(
            'title' => 'Services',
            'url' => 'welcome/services',
            'isRoot' => false
        );
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
            'ret' => json_decode(cfpr_report_classes($hostkey, $name, $regex, NULL, 1000, 1), true),
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
        $data = array(
            'title_header' => "body " . $body,
            'title' => "Cfengine Mission Portal - classes ",
            'nav_text' => "show : body",
            'status' => "current",
            'allbodies' => json_decode(utf8_encode(cfpr_list_bodies(".*", $type)),TRUE),
            'def' => json_decode(utf8_encode(cfpr_get_promise_body($body, $type)),TRUE),
            'type' => $type
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