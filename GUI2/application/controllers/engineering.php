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
            array('flot/jquery.flot.stack.js')
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

