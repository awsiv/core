<?php

class Graph extends CF_Controller {

    function __construct() {
        parent::__construct();


        $requiredjs = array(
            array('flot/jquery.flot.js')
        );
        $jsIE = array('flot/excanvas.min.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));

        $this->carabiner->js($requiredjs);
    }

    /**
     * replace special characters for javascript support such as ':'
     * @param type $obs 
     */
    function canonifyObservables($obs) {
        $replaceArray = array('!', '"', '#', '$', '%', '&', '(', ')', '*', '+', ',', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '`', '{', '|', '}', '~');

        return str_replace($replaceArray, '', $obs);
    }

    function summary($hostKey=null) {



        $requiredjs = array(
            array('flot/jquery.flot.highlighter.js'),
            array('flot/jquery.flot.valuelabels.js'),
            array('flot/jquery.flot.stack.js'),
            array('jit/jit-yc.js')
        );

        $this->carabiner->js($requiredjs);

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
        $units = $getparams['units'];
        // $graphData = cfpr_get_magnified_view($hostKey, $observables);
        $graphData = cfpr_vitals_view_magnified($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);

            $lastUpdated = trim(strip_tags($hostData));

            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $this->canonifyObservables($observables);
            $this->data['units'] = $units;

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
            
            $this->data['graphDetails'] = json_decode(cfpr_vitals_analyse_magnified($hostKey, $observables),true);
            
            $this->load->view('graph/magnifiedView', $this->data);
        } else
            echo "No data available.";
//        $this->template->load('template', 'graph/magnifiedView', $this->data);
    }

    function weekView() {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;



        // $graphData = cfpr_get_weekly_view($hostKey, $observables);
        $graphData = cfpr_vitals_view_week($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);

            $lastUpdated = trim(strip_tags($hostData));


            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $this->canonifyObservables($observables);

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
            $this->data['graphDetails'] = json_decode(cfpr_vitals_analyse_week($hostKey, $observables),true);
           
            $this->load->view('graph/weekly', $this->data);
        } else
            echo "No data available.";
//$this->template->load('template', 'graph/weekly', $this->data);
    }

    function yearView() {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;

        //$graphData = cfpr_get_yearly_view($hostKey, $observables);
        $graphData = cfpr_vitals_view_year($hostKey, $observables);
        // var_dump($graphData);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);
            $lastUpdated = trim(strip_tags($hostData));


            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $this->canonifyObservables($observables);
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
             $this->data['graphDetails'] = json_decode(cfpr_vitals_analyse_year($hostKey, $observables),true);
           
            $this->load->view('graph/yearview', $this->data);
        } else
            echo "No data available.";
    }

    function histogramView() {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;


        // $graphData = cfpr_get_histogram_view($hostKey, $observables);
        $graphData = cfpr_vitals_view_histogram($hostKey, $observables);
        $convertData = json_decode($graphData, true);
        if (!empty($convertData)) {

            $hostData = cfpr_getlastupdate($hostKey);
            $lastUpdated = trim(strip_tags($hostData));


            $lastUpdated = strtotime($lastUpdated) * 1000;

            $this->data['graphLastUpdated'] = $lastUpdated;
            $this->data['graphdata'] = $graphData;
            $this->data['observable'] = $this->canonifyObservables($observables);
             $this->data['graphDetails'] = json_decode(cfpr_vitals_analyse_histogram($hostKey, $observables),true);
           
            $this->load->view('graph/histogram', $this->data);
        } else
            echo "No data available";
        //$this->template->load('template', 'graph/histogram', $this->data);
    }

}