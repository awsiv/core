<?php

class Visual extends Cf_Controller
{

    private static $sortTable = array(
        'last-measured' => 'sortVitalsByLastMeasured',
        'average' => 'sortVitalsByAverage'
    );

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

    function vital($hostkey = NULL,$obs=NULL)
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
            'hostKey' => $hostkey,
            'obs' => $obs
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

}