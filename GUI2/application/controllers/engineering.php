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
        
        $data = array(
            'title' => $this->lang->line('mission_portal_title'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        
        $this->template->load('template', 'engineering', $data);
    }
}

