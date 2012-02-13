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
        
        $username = $this->session->userdata('username');

        if (is_null($handle)) {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        } else {
            $handle = urldecode($handle);
        }
       
        $mybundle = cfpr_bundle_by_promise_handle($this->session->userdata('username'), $handle);

        // get promise details
        $promise  = sanitycheckjson(cfpr_promise_details($this->session->userdata('username'), $handle),true);
        $promiser = $promise['promiser'];
        $type     = $promise['promise_type'];
        
        $pid = cfpr_get_pid_for_topic("promises", $handle);
        $topicDetail = cfpr_show_topic($pid);

        $bc = array(
            'title' => $this->lang->line('breadcrumb_promise'),
            'url' => 'promise/details/'.$handle,
            'isRoot' => false,
            'replace_existing'=>true
        );

        /*
         * cfpr_promise_list_by_promiser  and cfpr_promise_list_by_promise_type
         * 
         * return nested array like array 0 =>  array 0 => 'promise_cdp_commands_cf_9' ..
         * 
         * so we have to use first element from the inner array
         */
        
        $promise_list_by_promiser_arr = array_values(json_decode(utf8_encode(cfpr_promise_list_by_promiser($username, $promiser)),TRUE));
        $allhandlespromiser = &$promise_list_by_promiser_arr[0];
        
        $promise_list_by_promise_type_arr = json_decode(utf8_encode(cfpr_promise_list_by_promise_type($username, $type)),TRUE);
        $allhandlesbytype = &$promise_list_by_promise_type_arr[0];
        
        $this->breadcrumb->setBreadCrumb($bc);
        
        $data = array(
            'handle' => $handle,
            'title'  => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_promise')." " .$handle,
            'pid'    => $pid,
            'type'   => $type,        
            'promise'=> $promise,    
            'allhandlespromiser' => $allhandlespromiser,
            'allhandlesbytype'   => $allhandlesbytype,       
            
            'mybundle'    => cfpr_bundle_by_promise_handle($username, $handle),

            'allhandles'  => json_decode(utf8_encode(cfpr_promise_list_by_bundle($username, cfpr_get_bundle_type($mybundle), $mybundle)),TRUE),
            'topicLeads'  => json_decode(utf8_encode(cfpr_show_topic_leads($pid)),TRUE),
            'topicDetail' => json_decode(utf8_encode($topicDetail), true),

            'breadcrumbs' => $this->breadcrumblist->display()
        );
        
        if(is_ajax()){
            $this->load->view('promise/promise',$data);
        }else{
            $this->template->load('template', 'promise/promise', $data);
        }
        
    }

}
