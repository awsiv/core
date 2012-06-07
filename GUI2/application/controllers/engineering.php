<?php

class Engineering extends Cf_Controller
{

    function index()
    {
        $breadcrumb = array(
            'title' => $this->lang->line('breadcrumb_engineering'),
            'url' => 'engineering',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($breadcrumb);

        $requiredjs = array(
            array('flot/jquery.flot.js'),
            array('flot/jquery.flot.stack.js'),
            array('flot/jquery.flot.resize.min.js'),
            array('widgets/astrolabe.js'),
            array('widgets/astrolabeLocation.js'),
            array('widgets/combobox.js'),
            array('widgets/searchEntry.js'),
            array('widgets/hostsCompliance.js'),
            array('widgets/hostsConnectivity.js'),
            array('widgets/hostsComplianceTimeseries.js'),
            array('widgets/hostInfo.js'),
            array('widgets/classfinder.js'),
            array('widgets/hostfinder.js'),
            array('widgets/policyfinder.js'),
            array('jScrollPane.js'),
            array('jquery.mousewheel.js'),
            array('jquery.contextMenu.js'),
            array('jquery.blockUI.js'),
            array('widgets/notes.js'),
            array('widgets/classfinder.js'),
            array('widgets/contextfinder.js'),
            array('widgets/reportUI.js')
        );

        $this->carabiner->css(
                array(
                    array('classfinderbox.css'),
                    array('view/engineering.css'),
                    array('astrolabe.css'),
                    array('astrolabeLocation.css'),
                    array('combobox.css'),
                    array('searchEntry.css'),
                    array('hostsCompliance.css'),
                    array('hostsConnectivity.css'),
                    array('hostsComplianceTimeseries.css'),
                    array('hostInfo.css'),
                    array('jScrollPane.css'),
                    array('contextfinder.css'),
                    array('tabs-custom.css')
                )
        );


        $jsIE = array('flot/excanvas.min.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->js($requiredjs);


        $data = array(
            'title' => $this->lang->line('mission_portal_title'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $this->template->load('template', 'engineering', $data);
    }

}

