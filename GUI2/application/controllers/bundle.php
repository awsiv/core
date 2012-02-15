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
            'title' => $this->lang->line('breadcrumb_bundle'),
            'url' => 'bundle',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title'       => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_bundle'),
            'bundle_list' => json_decode(cfpr_report_bundlesseen($hostkey, $name, $regex, NULL, "last-verified", true, 10000, 1), true),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'bundle/bundle_list', $data);
    }

    function details() {
        $this->carabiner->css('tabs-custom.css');

        $params = $this->uri->uri_to_assoc(3);
        $bundle = isset($params['bundle']) ? urldecode($params['bundle']) : "";
        $type = isset($params['type']) ? $params['type'] : cfpr_get_bundle_type($bundle);
        $bc = array(
            'title' => $this->lang->line('breadcrumb_bundle'),
            'url' => 'bundle/details/bundle/'.urlencode($bundle).'/type/'.urlencode($type) ,
            'isRoot' => false,
             'replace_existing'=>true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $username = $this->session->userdata('username');
        try{
        $data = array(
            'title'       => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_bundle'),
            'status'      => "current",
            'bundle'      => $bundle,
            'allbundles'  => json_decode(utf8_encode(cfpr_bundle_list_all($username)), TRUE),
            'args'        => json_decode(utf8_encode(cfpr_bundle_arguments($username, $type, $bundle))),
            'classes'     => json_decode(utf8_encode(cfpr_bundle_classes_used($username, $type, $bundle)), TRUE),
            'list'        => json_decode(utf8_encode(cfpr_promise_list_by_bundle($username, $type, $bundle)), TRUE),
            'others'      => json_decode(utf8_encode(cfpr_bundle_list_by_bundle_usage($username, $bundle)), TRUE),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        }
        catch(Exception $e){
            $data['error']= generate_errormessage($e);
        }
        $this->template->load('template', 'bundle/bundle_detail', $data);
    }

}
