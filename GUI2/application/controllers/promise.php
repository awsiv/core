<?php

class Promise extends Cf_Controller {

    function Promise() {
        parent::__construct();
    }

    function index() {
        $this->details();
    }
    
    /*function plist($key = NULL) {
        $bc = array(
            'title' => 'Promises',
            'url' => 'promise',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $hostkey = $key;
        $name = ".*";
        $regex = 1;
        $handle = NULL;
        $data = array(
            'title' => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_promise'),
            'promise_list' => json_decode(cfpr_report_compliance_promises($hostkey, $handle, NULL, false, NULL,"last-seen", true, 1000,1), true),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'promise/promises', $data);
    }*/
    
    function details($handle=NULL) {
         $this->carabiner->css('tabs-custom.css');

        if (is_null($handle)) {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        } else {
            $handle = urldecode($handle);
        }
        $mybundle = cfpr_bundle_by_promise_handle($handle);
        $promiser = cfpr_get_promiser($handle);
        $type = cfpr_get_promise_type($handle);
        $pid = cfpr_get_pid_for_topic("promises", $handle);
        $topicDetail = cfpr_show_topic($pid);

            $bc = array(
            'title' => $this->lang->line('breadcrumb_promise'),
            'url' => 'promise/details/'.$handle,
            'isRoot' => false,
             'replace_existing'=>true
          );
        $this->breadcrumb->setBreadCrumb($bc);
       
        $data = array(
            'handle' => $handle,
            'title' => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_promise')." " .$handle,
            'pid' => $pid,
            'mybundle' => cfpr_bundle_by_promise_handle($handle),
            'allhandles' => json_decode(utf8_encode(cfpr_list_handles_for_bundle($mybundle, cfpr_get_bundle_type($mybundle), false)),TRUE),
            'allhandlespromiser' => json_decode(utf8_encode(cfpr_list_handles($promiser, "", false)),TRUE),
            'type' => $type,
            'allhandlesbytype' =>json_decode(utf8_encode(cfpr_list_handles("", $type, false)),TRUE),
            'topicLeads' => json_decode(utf8_encode(cfpr_show_topic_leads($pid)),TRUE),
            'topicDetail' => json_decode(utf8_encode($topicDetail), true),
            'promise' => sanitycheckjson(cfpr_promise_details($this->session->userdata('username'), $handle),true),
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        if(is_ajax()){
            $this->load->view('promise/promise',$data);
        }else{
            $this->template->load('template', 'promise/promise', $data);
        }
        
    }

}
