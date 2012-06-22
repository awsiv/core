<?php

class Graph extends CF_Controller
{

    function __construct()
    {
        parent::__construct();


        $requiredjs = array(
            array('flot/jquery.flot.js')
        );
        $jsIE = array('flot/excanvas.min.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));

        $this->carabiner->js($requiredjs);
        $this->load->model('vitals_model');
    }

    /**
     * replace special characters for javascript support such as ':'
     * @param type $obs
     */
    function canonifyObservables($obs)
    {
        $replaceArray = array('!', '"', '#', '$', '%', '&', '(', ')', '*', '+', ',', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '`', '{', '|', '}', '~');

        return str_replace($replaceArray, '', $obs);
    }

    function summary($hostKey = null)
    {

        $requiredjs = array(
            array('flot/jquery.flot.highlighter.js'),
            array('flot/jquery.flot.valuelabels.js'),
            array('flot/jquery.flot.stack.js'),
            array('jit/jit-yc.js')
        );

        $this->carabiner->js($requiredjs);

        $this->template->set('injected_item', implode('', $this->scripts));

        $this->data = array(
            'title' => $this->lang->line('mission_portal_title') . " - Summary",
            'summary' => "current"
        );

        if (!$hostKey)
            $gdata = cfpr_summary_meter();
        else
            $gdata = cfpr_host_meter($this->session->userdata('username'), $hostKey);

        $convertedData = json_decode($gdata, true);

        $keptSeries = array();
        $notKeptSeries = array();
        $repairedSeries = array();

        $values = array();
        $this->data['graphSeries'] = array();
        $labels = array('kept', 'not kept', 'repaired');
        foreach ($convertedData as $key => $graphData)
        {

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

    function magnifiedView($parameter)
    {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $username = $this->session->userdata('username');
        $graphData = $this->vitals_model->getVitalsMagnifiedViewJson($username, $hostKey, $observables);
        $manipulatedSeriesData = json_decode($graphData, true);
        if ($graphData !== null && $manipulatedSeriesData !== false && !empty($manipulatedSeriesData))
        {

            $this->data['graphLastUpdated'] = $this->vitals_model->getVitalsLastUpdate($username, $hostKey);
            $this->data['graphDetails'] = $this->vitals_model->getVitalsMagnifiedAnalysis($username, $hostKey, $observables);
            $this->data['graphdata'] = ($graphData);
            $this->data['observable'] = $this->canonifyObservables($observables);
            $this->data['units'] = $units;


            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values)
            {
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
            echo $this->lang->line('graph_data_not_available');
    }

    function weekView()
    {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;
        $username = $this->session->userdata('username');
        $graphData = $this->vitals_model->getVitalsWeekViewJson($username, $hostKey, $observables);
        $manipulatedSeriesData = json_decode($graphData, true);
        if ($graphData !== null && $manipulatedSeriesData !== false && !empty($manipulatedSeriesData))
        {

            $this->data['graphLastUpdated'] = $this->vitals_model->getVitalsLastUpdate($username, $hostKey);
            $this->data['graphDetails'] = $this->vitals_model->getVitalsWeekAnalysis($username, $hostKey, $observables);

            $this->data['graphdata'] = $graphData;
            $this->data['observable'] = $this->canonifyObservables($observables);
            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values)
            {
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
            echo $this->lang->line('graph_data_not_available');
    }

    function yearView()
    {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;

        $username = $this->session->userdata('username');
        $graphData = $this->vitals_model->getVitalsYearViewJson($username, $hostKey, $observables);
        $manipulatedSeriesData = json_decode($graphData, true);
        if ($graphData !== null && $manipulatedSeriesData !== false && !empty($manipulatedSeriesData))
        {

            $this->data['graphLastUpdated'] = $this->vitals_model->getVitalsLastUpdate($username, $hostKey);
            $this->data['graphDetails'] = $this->vitals_model->getVitalsYearAnalysis($username, $hostKey, $observables);
            $this->data['graphdata'] = $graphData;
            $this->data['observable'] = $this->canonifyObservables($observables);
            $lineSeries1 = array();
            $lineSeries2 = array();
            $tempMaxValue = array();
            $tempMinValue = array();

            foreach ($manipulatedSeriesData as $points => $values)
            {
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
            echo $this->lang->line('graph_data_not_available');
    }

    function histogramView()
    {

        $getparams = $this->uri->uri_to_assoc(3);

        $observables = $getparams['obs'];
        $hostKey = $getparams['host'];
        $units = $getparams['units'];
        $this->data['units'] = $units;


        $username = $this->session->userdata('username');
        $graphData = $this->vitals_model->getVitalsHistogramViewJson($username, $hostKey, $observables);
        $manipulatedSeriesData = json_decode($graphData, true);
        if ($graphData !== null && $manipulatedSeriesData !== false && !empty($manipulatedSeriesData))
        {

            /*
              $lastUpdated = $this->vitals_model->getVitalsLastUpdate($username, $hostKey);
              $lastUpdated = strtotime($lastUpdated) * 1000;
              $this->data['graphLastUpdated'] = $lastUpdated;
             */
            $this->data['graphDetails'] = $this->vitals_model->getVitalsHistogramAnalysis($username, $hostKey, $observables);
            $this->data['graphdata'] = $graphData;
            $this->data['observable'] = $this->canonifyObservables($observables);

            $this->load->view('graph/histogram', $this->data);
        } else
            echo $this->lang->line('graph_data_not_available');
    }

}
