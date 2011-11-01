<?php

class Virtualbundle extends Cf_Controller {

    function Virtualbundle() {
        parent::__construct();
         $this->load->model('virtual_bundle_model');
    }

    function index() {
        $this->details();
    }

    function details($handle=NULL) {
        $this->carabiner->css('tabs-custom.css');

        if (is_null($handle)) {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        } else {
            $handle = urldecode($handle);
        }

      $handleData = $this->virtual_bundle_model->getVirtualBundleDetails($handle);
      $handleStatus = $this->virtual_bundle_model->getVirtualBundleStatus($handle);

        $bc = array(
            'title' => $this->lang->line('breadcrumb_promise'),
            'url' => 'promise/details/' . $handle,
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'handle' => $handle,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_promise') . " " . $handle,
            'bundle_data' => $handleData,
            'bundle_status' => $handleStatus,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'virtualbundle/detail', $data);
    }
    
    
    

}
