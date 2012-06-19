<?php

class Visual extends Cf_Controller
{

    function Visual()
    {
        parent::__construct();
        // inject the required js files
        $this->carabiner->js('flot/jquery.flot.js');
        $this->carabiner->js('flot/jquery.flot.magnifiedview.js');
        $this->carabiner->js('flot/jquery.flot.resize.min.js');
        $jsIE = array('flot/excanvas.min.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->css('tabs-custom.css');
        $this->load->model('vitals_model');
         $this->load->model('host_model');
    }

    function vital($hostkey = NULL)
    {
        $hostkey = isset($_POST['hostkey']) ? $_POST['hostkey'] : $hostkey;
        $username = $this->session->userdata('username');

        $bc = array(
            'title' => $this->lang->line('breadcrumb_vital'),
            'url' => 'visual/vital/' . $hostkey,
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_vital') . " Signs",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'hostKey' => $hostkey
        );

        if ($hostkey != 'none')
        {
            $convertData = $this->vitals_model->getVitalsList($username, $hostkey);
            if (is_array($convertData) && array_key_exists('obs', $convertData) && !empty($convertData['obs']))
            {
                $data['performanceData'] = $convertData;
                $this->template->load('template', 'visualization/vital', $data);
            }
            else
            {
                $data['noDataMessage'] = "No data available for this host";
                $this->template->load('template', '/visualization/nohost', $data);
            }
        }
        else
        {
            $data['noDataMessage'] = "No host selected";
            $this->template->load('template', '/visualization/nohost', $data);
        }
    }


    function sortVitals($data1,$data2) {

        $lastPerformanceData1 = end($data1['perfdata']);
        $lastPerformanceData2 = end($data2['perfdata']);

        $lastValue1 = !empty($lastPerformanceData1) ? $lastPerformanceData1[1] : 0 ;
        $lastValue2 = !empty($lastPerformanceData2) ? $lastPerformanceData2[1] : 0;
        if ($lastValue1 == $lastValue2) {
            return 0;
        }
        return ($lastValue1 < $lastValue2) ? +1 : -1;
    }

    function getNodeVitals() {
        $username = $this->session->userdata('username');
        $incList = $this->input->post('includes') ? $this->input->post('includes') : array() ;
        $listOfHost = $this->host_model->getHostListByContext($username,$incList,array());
        $data = array();
        foreach($listOfHost as $index=>$host){
            $key = $host['hostkey'];
            $graphData = $this->vitals_model->getVitalsMagnifiedViewJson($username, $key, 'loadavg');
            $lastMeasuredArray = json_decode($graphData);
            $data[$index]['meta'] = $host;
            $data[$index]['meta']['lastUpdated'] = $this->vitals_model->getVitalsLastUpdate($username, $key);
            $data[$index]['perfdata'] = $lastMeasuredArray;
            if (empty($lastMeasuredArray)) {
                $data[$index]['meta']['nodata'] = true;
            }

        }
        usort($data, array($this,'sortVitals'));
        $json = json_encode($data);
        echo $json;




    }

}