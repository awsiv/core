<<<<<<< HEAD
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

        
        $this->breadcrumb->setBreadCrumb($bc);
       


        try{  
            $mybundle = cfpr_bundle_by_promise_handle($username, $handle);


            // get promise details
            $promise  = sanitycheckjson(cfpr_promise_details($this->session->userdata('username'), $handle),true);
            $promiser = $promise['promiser'];
            $type     = $promise['promise_type'];


            $pid = cfpr_get_pid_for_topic("promises", $handle);

            $topicDetail = cfpr_show_topic($pid);

            $tmp_arr = array();
            $tmp_arr = json_decode(utf8_encode(cfpr_promise_list_by_promiser($username, $promiser)),TRUE);
            foreach($tmp_arr as $item => $value) {
                $allhandlespromiser[] = $value[0];
            }
            
            $tmp_arr = json_decode(utf8_encode(cfpr_promise_list_by_promise_type($username, $type)),TRUE);
            foreach($tmp_arr as $item => $value) {
                $allhandlesbytype[] = $value[0];
            }
     
             
            $tmp_arr = json_decode(utf8_encode(cfpr_promise_list_by_bundle($username, cfpr_get_bundle_type($mybundle), $mybundle)),TRUE);
            foreach($tmp_arr as $item => $value) {
                $allhandles[] = $value[0];
            }            


            $data = array(
                'handle' => $handle,
                'title'  => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_promise')." " .$handle,
                'pid'    => $pid,
                'type'   => $type,  

                'promise'=> $promise,    
                'allhandlespromiser' => $allhandlespromiser,
                'allhandlesbytype'   => $allhandlesbytype,   
                'allhandles'         => $allhandles,

                'mybundle'    => cfpr_bundle_by_promise_handle($username, $handle),

                'topicLeads'  => json_decode(utf8_encode(cfpr_show_topic_leads($pid)),TRUE),
                'topicDetail' => json_decode(utf8_encode($topicDetail), true),

                'breadcrumbs' => $this->breadcrumblist->display()
            );
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
