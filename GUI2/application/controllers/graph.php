<?php

class Graph extends CF_Controller {

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

    function test() {
        $bc = array(
            'title' => 'All host',
            'url' => 'welcome/status',
            'isRoot' => false
        );


//$reports = json_decode(cfpr_select_reports(".*", 100));
        $data = array(
            'title' => "Cfengine Mission Portal - engineering status",
            'title_header' => "engineering status",
            'nav_text' => "Status : hosts",
            'status' => "current",
            'onlineusers' => '1'
        );

        $this->scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'flot/excanvas.min.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.js"> </script>
                ',
            'mv' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.magnifiedview.js"> </script>
             ', 'stack' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.stack.js"> </script>

', 'fill' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.fillbetween.js"> </script>');


        $this->template->set('injected_item', implode('', $this->scripts));

        $key = 'SHA=ea623bad7e756bf0a39081ac22cfb572de1f7d8d970c026a529172f6c5f6fb98';
        $data['gdata'] = cfpr_get_value_graph($key, NULL, NULL, NULL, NULL);

        $this->template->load('template', 'graph/test', $data);
    }

    function businessValueGraph() {

        $data = array(
            'title' => "Cfengine Mission Portal - engineering status",
            'title_header' => "engineering status",
            'nav_text' => "Status : hosts",
            'status' => "current",
            'onlineusers' => '1'
        );

        $this->scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'flot/excanvas.min.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.js"> </script>
                ',
            'mv' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.magnifiedview.js"> </script>
             ', 'stack' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.stack.js"> </script>

',
            'fill' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.fillbetween.js"> </script>',
            'pie' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.pie.js"> </script>',
            'jit' => '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'jit/jit.js"> </script>');

        $this->template->set('injected_item', implode('', $this->scripts));

        $key = 'SHA=ea623bad7e756bf0a39081ac22cfb572de1f7d8d970c026a529172f6c5f6fb98';
        $data['gdata'] = cfpr_get_value_graph(NULL, NULL, NULL, NULL, NULL);

        $this->template->load('template', 'graph/pie', $data);
    }

}