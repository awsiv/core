<?php

class Promise extends Cf_Controller {

    function Promise() {
        parent::__construct();
    }

    function index() {
        $this->details();
    }
   
    
    function details($handle=NULL) {
        $this->carabiner->css('tabs-custom.css');
        
        $username = $this->session->userdata('username');

        if (is_null($handle)) {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        } else {
            $handle = urldecode($handle);
        }
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
       
        $data1= array(
            'handle' => $handle,
            'title' => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_promise')." " .$handle,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        
        try{  
         $mybundle = cfpr_bundle_by_promise_handle($this->session->userdata('username'), $handle);
         $promiser = cfpr_get_promiser($handle);
         $type = cfpr_get_promise_type($handle);
         $pid = cfpr_get_pid_for_topic("promises", $handle);
         $topicDetail = cfpr_show_topic($pid);
        $data2 = array(
            'pid' => $pid,
            'type' => $type,
            'mybundle' => cfpr_bundle_by_promise_handle($this->session->userdata('username'), $handle),
            'allhandles' => json_decode(utf8_encode(cfpr_list_handles_for_bundle($mybundle, cfpr_get_bundle_type($mybundle), false)),TRUE),
            'allhandlespromiser' => json_decode(utf8_encode(cfpr_list_handles($promiser, "", false)),TRUE),
            'allhandlesbytype' =>json_decode(utf8_encode(cfpr_list_handles("", $type, false)),TRUE),
            'topicLeads' => json_decode(utf8_encode(cfpr_show_topic_leads($pid)),TRUE),
            'topicDetail' => json_decode(utf8_encode($topicDetail), true),
            'promise' => sanitycheckjson(cfpr_promise_details($this->session->userdata('username'), $handle),true),
        );
        $data=array_merge($data1,$data2);
        }catch(Exception $e){
           $data['error']= generate_errormessage($e);
        }
        if(is_ajax()){
            $this->load->view('promise/promise',$data);
        }else{
            $this->template->load('template', 'promise/promise', $data);
        }
        
    }

}
