<?php

class Graph extends CI_Controller {

    function __construct() {
        parent::__construct();
        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'flot/excanvas.min.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.min.js"> </script>
                ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.magnifiedview.js"> </script>
                ');


        $this->template->set('injected_item', implode('', $scripts));
    }

    function summary() {
        $data = cfpr_compliance_summary_graph();
        var_dump($data);
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

            $lastUpdated = strtotime($lastUpdated) * 1000;

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


            $lastUpdated = strtotime($lastUpdated) * 1000;
            //$host = "[" . substr($host, 1, -1);
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
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);

            $lastUpdated = trim(strip_tags($hostData));


            $lastUpdated = strtotime($lastUpdated) * 1000;
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
            $this->template->load('template', 'graph/yearview', $this->data);
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

        $hostKey = 'SHA=38c5642ccb0dc74bc754aa1a63e81760869e1b3405bf9e43ad85c99822628e8e';
        $data = cfpr_performance_analysis($hostKey);
        $convertData = json_decode($data, true);

        if (is_array($convertData)) {
            //var_dump($convertData);
            $this->data['performanceData'] = $convertData;
            $this->template->load('template', 'graph/accordian', $this->data);
        }
    }

}