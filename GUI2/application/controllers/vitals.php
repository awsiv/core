<?php

require_once APPPATH . 'libraries/Cf_REST_Controller.php';

class Vitals extends Cf_REST_Controller
{

    private static $sortTable = array(
        'last-measured' => '_sortVitalsByLastMeasured',
        'average' => '_sortVitalsByAverage'
    );

    function __construct()
    {
        parent::__construct();
        $this->load->model('vitals_model');
        $this->load->model('host_model');
    }

    function _sortVitalsByLastMeasured($data1, $data2)
    {

        $lastPerformanceData1 = end($data1['perfdata']);
        $lastPerformanceData2 = end($data2['perfdata']);

        $lastValue1 = !empty($lastPerformanceData1) ? $lastPerformanceData1[1] : 0;
        $lastValue2 = !empty($lastPerformanceData2) ? $lastPerformanceData2[1] : 0;
        if ($lastValue1 == $lastValue2)
        {
            return 0;
        }
        return ($lastValue1 < $lastValue2) ? +1 : -1;
    }

    function _sortVitalsByAverage($data1, $data2)
    {
        $data1Count = count($data1['perfdata']);
        $data2Count = count($data2['perfdata']);
        $sumData1 = 0;
        $sumData2 = 0;
        $averageData1 = 0;
        $averageData2 = 0;

        if ($data1Count)
        {
            foreach ($data1['perfdata'] as $d1)
            {

                $sumData1 += $d1[1];
            }
            $averageData1 = $sumData1 / $data1Count;
        }

        if ($data2Count)
        {
            foreach ($data2['perfdata'] as $d2)
            {
                $sumData2 += $d2[1];
            }
            $averageData2 = $sumData2 / $data2Count;
        }
        if ($averageData1 == $averageData2)
        {
            return 0;
        }
        return ($averageData1 < $averageData2) ? +1 : -1;
    }

    function node_vitals_get()
    {
        $username = $this->session->userdata('username');
        $this->config->load('vitals');
        $vitalsMaxNode = $this->config->item('vitals_max_nodes');
        $incList = $this->param_includes();
        $listOfHost = $this->host_model->getHostListByContext($username, $incList, array());
        $data = array();
        $obs = $this->param('obs', TRUE) ? $this->param('obs', TRUE) : 'loadavg';
        $sort = $this->param('sort', TRUE) ? $this->param('sort', TRUE) : 'last-measured';
        foreach ($listOfHost as $index => $host)
        {
            $key = $host['hostkey'];
            $graphData = $this->vitals_model->getVitalsMagnifiedViewJson($username, $key, $obs);
            $lastMeasuredArray = json_decode($graphData);
            $data[$index]['meta'] = $host;
            $data[$index]['meta']['lastUpdated'] = $this->vitals_model->getVitalsLastUpdate($username, $key);
            $data[$index]['perfdata'] = $lastMeasuredArray;
            if (empty($lastMeasuredArray))
            {
                $data[$index]['meta']['nodata'] = true;
            }
        }
        $sortFunction = isset(self::$sortTable[$sort]) ? self::$sortTable[$sort] : self::$sortTable['last-measured'];
        usort($data, array($this, $sortFunction));
        $data = array_slice($data, 0, $vitalsMaxNode);

        $this->respond(200, json_encode($data));
    }

    function host_vitals_get()
    {
        $username = $this->session->userdata('username');
        $incList = $this->param_includes();
        $observables = '';
        $listOfHost = $this->host_model->getHostListByContext($username, $incList, array());
        $vitalList = array();
        if (!empty($listOfHost))
        {
            $host = $listOfHost[0];
            $hostkey = $host['hostkey'];
            $vitalList = $this->vitals_model->getVitalsList($username, $hostkey);
            $vitalList['hostkey'] = $hostkey;
            krsort($vitalList['obs']);
            $data = array();
            foreach ($vitalList['obs'] as $index => $vitals)
            {
                $vitalId = $vitals['id'];
                $vitalList['obs'][$index]['perfdata'] = json_decode($this->vitals_model->getVitalsMagnifiedViewJson($username, $hostkey, $vitalId));
            }
        }
        $this->respond(200, json_encode($vitalList));
    }

    function config_vitals_get()
    {
        $this->config->load('vitals', TRUE);
        $vitalsArray = array();
        $vitalsArray['vitals'] = $this->config->item('vitals', 'vitals');
        $vitalsArray['vitals-sort-by'] = $this->config->item('vitals-sort-by', 'vitals');
        $this->respond(200, json_encode($vitalsArray));
    }

}