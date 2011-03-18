<?php

class Graph extends CI_Controller {

    function __construct() {
        parent::__construct();
        $this->scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'flot/excanvas.min.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.js"> </script>
                ',
            'mv' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.magnifiedview.js"> </script>
                ');


        $this->template->set('injected_item', implode('', $this->scripts));
    }

    function complianceSummary() {

        $data = cfpr_compliance_summary_graph('');
        //print $data;
//$data = utf8_encode($data);
        $array = json_decode($data, true);
// break down the array in day slot
        foreach ($array as $val) {
            $title = trim($val['title']);
            $date = substr($title, 0, strripos($title, '  '));
            $timeSlot = trim(strrchr($title, '  '));
            $slot[$date][$timeSlot] = $val;
        }

        // make a average of everything
        foreach ($slot as &$s) {
            $totalSlots = count($s);
            $totalKept = 0;
            $totalRepaired = 0;
            $totalNotKept = 0;
            $noData = 0;
            $totalCount = 0;

            foreach ($s as $timeSlots) {
                // we compute average of kept,repaired, not kept, no data
                if ($timeSlots['nodata'] == '100')
                    continue; // no data so..


                    $totalKept += $timeSlots['kept'];
                $totalRepaired += $timeSlots['repaired'];
                $totalNotKept += $timeSlots['notkept'];
                $noData += $timeSlots['nodata'];
                $totalCount += $timeSlots['count'];
            }

            $avgKept = $totalKept / $totalSlots;
            $avgRepaired = $totalNotKept / $totalSlots;
            $avgNotKept = $totalRepaired / $totalSlots;
            $avgNoData = $noData / $totalSlots;

            // make a new node
            $avg = array('kept' => $avgKept,
                'notkept' => $avgNotKept,
                'repaired' => $avgRepaired,
                'nodata' => $avgNoData,
                'count' => $totalCount);
            $s['avg'] = $avg;
        }


        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.highlighter.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.valuelabels.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.stack.js"> </script>
                ');

        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit.js"> </script>
                ');




        $this->template->set('injected_item', implode('', $this->scripts));
        $this->data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'nav_text' => "Home : overview",
            'summary' => "current",
            'pie' => $slot
        );

        $this->template->load('template', 'graph/pie', $this->data);
    }

    function summaryhost() {



        $hostKey = 'SHA=e28c6c3484481d758ced809933abf404478d48272e64a23a65e38ee4cc28a920';
        unset($this->scripts['mv']);
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.highlighter.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.valuelabels.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.stack.js"> </script>
                ');

        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit.js"> </script>
                ');




        $this->template->set('injected_item', implode('', $this->scripts));

        $this->data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'nav_text' => "Home : overview",
            'summary' => "current"
        );

        $gdata = cfpr_host_meter($hostKey);
        var_dump($gdata);
        $convertedData = json_decode($gdata, true);
        die($gdata);
        $keptSeries = array();
        $notKeptSeries = array();
        $repairedSeries = array();

        $values = array();
        $this->data['graphSeries'] = array();
        $labels = array('kept', 'not kept', 'repaired');
        foreach ($convertedData as $key => $graphData) {

            $keptSeries[] = array($key, $graphData['kept']);
            $notKeptSeries[] = array($key, $graphData['notkept']);
            $repairedSeries[] = array($key, $graphData['repaired']);

            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['notkept'], $graphData['repaired']));
        }

        $this->data['graphSeries']['kept'] = json_encode($keptSeries);
        $this->data['graphSeries']['notkept'] = json_encode($notKeptSeries);
        $this->data['graphSeries']['repaired'] = json_encode($repairedSeries);
        $this->data['graphSeries']['labels'] = json_encode($labels);
        $this->data['graphSeries']['values'] = json_encode($values);


        $this->template->load('template', 'graph/summaryCompliance', $this->data);
    }

    function summary($hostKey=null) {




        unset($this->scripts['mv']);
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.highlighter.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.valuelabels.js"> </script>
                ');
        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.stack.js"> </script>
                ');

        $this->scripts[] = ('<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit.js"> </script>
                ');




        $this->template->set('injected_item', implode('', $this->scripts));

        $this->data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "overview",
            'nav_text' => "Home : overview",
            'summary' => "current"
        );

        if (!$hostKey)
            $gdata = cfpr_summary_meter();
        else
            $gdata = cfpr_host_meter($hostKey);

        $convertedData = json_decode($gdata, true);

        $keptSeries = array();
        $notKeptSeries = array();
        $repairedSeries = array();

        $values = array();
        $this->data['graphSeries'] = array();
        $labels = array('kept', 'not kept', 'repaired');
        foreach ($convertedData as $key => $graphData) {

            $keptSeries[] = array($key, $graphData['kept']);
            $notKeptSeries[] = array($key, $graphData['notkept']);
            $repairedSeries[] = array($key, $graphData['repaired']);

            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['notkept'], $graphData['repaired']));
        }

        $this->data['graphSeries']['kept'] = json_encode($keptSeries);
        $this->data['graphSeries']['notkept'] = json_encode($notKeptSeries);
        $this->data['graphSeries']['repaired'] = json_encode($repairedSeries);
        $this->data['graphSeries']['labels'] = json_encode($labels);
        $this->data['graphSeries']['values'] = json_encode($values);


        $this->template->load('template', 'graph/summaryCompliance', $this->data);
    }

    function magnifiedView($parameter) {
        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];

        $graphData = cfpr_get_magnified_view($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);

            $lastUpdated = trim(strip_tags($hostData));

            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $observables;
            $manipulatedSeriesData = json_decode($graphData);
            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values) {
                $lineSeries1[] = array($values[0], $values[1]);
                $lineSeries2[] = array($values[0], $values[2]);
                $tempMaxValue[] = ($values[2] + $values[3]);
                $tempMinValue[] = ($values[2] - $values[3]);
            }

# we can calculate the min and maximum y value for float as well
            $this->data['graphdatamax'] = ceil(max($tempMaxValue) + 1);
            $min = (min($tempMinValue) != 0) ? min($tempMinValue) - 1 : 0;
            $min = ceil($min);
            $this->data['graphdatamin'] = $min;



            $this->data['graphdatalineseries1'] = json_encode($lineSeries1);
            $this->data['graphdatalineseries2'] = json_encode($lineSeries2);
            $this->load->view('graph/magnifiedView', $this->data);
        } else
            echo "No data available.";
//        $this->template->load('template', 'graph/magnifiedView', $this->data);
    }

    function weekView() {
        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];


        $graphData = cfpr_get_weekly_view($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);

            $lastUpdated = trim(strip_tags($hostData));


            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $observables;

            $manipulatedSeriesData = json_decode($graphData);

            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values) {
                $lineSeries1[] = array($values[0], $values[1]);
                $lineSeries2[] = array($values[0], $values[2]);
                $tempMaxValue[] = ($values[2] + $values[3]);
                $tempMinValue[] = ($values[2] - $values[3]);
            }

# we can calculate the min and maximum y value for float as well
            $this->data['graphdatamax'] = ceil(max($tempMaxValue) + 1);
            $min = (min($tempMinValue) != 0) ? min($tempMinValue) - 1 : 0;
            $min = ceil($min);
            $this->data['graphdatamin'] = $min;



            $this->data['graphdatalineseries1'] = json_encode($lineSeries1);
            $this->data['graphdatalineseries2'] = json_encode($lineSeries2);
            $this->load->view('graph/weekly', $this->data);
        } else
            echo "No data available.";
//$this->template->load('template', 'graph/weekly', $this->data);
    }

    function yearView() {
        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];

        $graphData = cfpr_get_yearly_view($hostKey, $observables);
        // var_dump($graphData);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);
            $lastUpdated = trim(strip_tags($hostData));
         

            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $observables;
            $manipulatedSeriesData = json_decode($graphData);

            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values) {
                $lineSeries1[] = array($values[0], $values[1]);
                $lineSeries2[] = array($values[0], $values[2]);
                $tempMaxValue[] = ($values[2] + $values[3]);
                $tempMinValue[] = ($values[2] - $values[3]);
            }

# we can calculate the min and maximum y value for float as well
            $this->data['graphdatamax'] = ceil(max($tempMaxValue) + 1);
            $min = (min($tempMinValue) != 0) ? min($tempMinValue) - 1 : 0;
            $min = ceil($min);
            $this->data['graphdatamin'] = $min;



            $this->data['graphdatalineseries1'] = json_encode($lineSeries1);
            $this->data['graphdatalineseries2'] = json_encode($lineSeries2);
            $this->load->view('graph/yearview', $this->data);
        } else
            echo "No data available.";
    }

    function histogramView() {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];

        $graphData = cfpr_get_histogram_view($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);
            $lastUpdated = trim(strip_tags($hostData));


            $lastUpdated = strtotime($lastUpdated) * 1000;

            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = $graphData;
            $this->data['observable'] = $observables;
            $this->load->view('graph/histogram', $this->data);
        } else
            echo "No data available";
//$this->template->load('template', 'graph/histogram', $this->data);
    }

    function knowledgeMap() {

        $pid = 758;

        $data = cfpr_get_knowledge_view($pid, '');
        var_dump($data);
        die();
        $this->data['graphdata'] = ($data);
        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'jit/Extras/excanvas.js">  </script><![endif]-->',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit-yc.js"> </script>',
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


        $this->template->set('injected_item', implode('', $scripts));

        $this->template->load('template', 'graph/knowledgemap', $this->data);
    }

    function accordian() {

        $observables = 1;
        $hostKey = 'SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e';
        echo "12";
        $graphData = cfpr_get_yearly_view($hostKey, $observables);
        var_dump($graphData);
        die();

        $hostKey = 'SHA=38c5642ccb0dc74bc754aa1a63e81760869e1b3405bf9e43ad85c99822628e8e';
        $data = cfpr_performance_analysis($hostKey);
        $convertData = json_decode($data, true);


        if (is_array($convertData)) {
            var_dump($convertData);
            die();
            $this->data['performanceData'] = $convertData;
            $this->template->load('template', 'graph/accordian', $this->data);
        }
    }

    function test() {

        $observables = 1;
        $hostKey = 'SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e';
        echo "12";
        $graphData = cfpr_get_yearly_view($hostKey, $observables);
        var_dump($graphData);
        die();
    }

}