<?php

class Bundle extends Cf_Controller {

    function Bundle() {
        parent::__construct();
    }

    function index() {
        $this->blist();
    }

    function blist($key = NULL) {
        $hostkey = $key;
        $name = ".*";
        $regex = 1;
        $hash = NULL;
        $host = NULL;
        $addr = NULL;
        $tago = 0;
        $bc = array(
            'title' => 'Bundle',
            'url' => 'bundle',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => "Cfengine Mission Portal - Bundles",
            'title_header' => "search results",
            'bundle_list' => json_decode(cfpr_report_bundlesseen($hostkey, $name, $regex, NULL, 10000, 1), true),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'bundle/bundle_list', $data);
    }

    function details() {
        $this->carabiner->css('tabs-custom.css');

        $params = $this->uri->uri_to_assoc(3);
        $bundle = isset($params['bundle']) ? urldecode($params['bundle']) : "";
        $type = isset($params['type']) ? $params['type'] : "";
    
        $data = array(
            'title' => "Cfengine Mission Portal - Bundles",
            'title_header' => "search results",
            'nav_text' => "Show : Bundle definition",
            'status' => "current",
            'bundle' => $bundle,
            'allbundles' => json_decode(utf8_encode(cfpr_list_all_bundles(NULL)), TRUE),
            'args' => cfpr_get_args_for_bundle($bundle, $type),
            'classes' => json_decode(utf8_encode(cfpr_get_classes_for_bundle($bundle, $type)), TRUE),
            'list' => json_decode(utf8_encode(cfpr_list_handles_for_bundle($bundle, $type, False)), TRUE),
            'others' => json_decode(utf8_encode(cfpr_list_bundles_using($bundle)), TRUE),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'bundle/bundle_detail', $data);
    }

}
