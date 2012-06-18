<?php

class EventTracker extends Cf_controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination', 'form_validation'));
        $this->load->helper('form');
        $this->load->model('tracker_model');
    }
    
    function createView(){
          $this->load->view('eventviewer/addTracker');
    }
    
    function create()
    {
        $this->form_validation->set_rules('trackerName', 'Tracker identifier', 'xss_clean|trim|required');
        $this->form_validation->set_rules('resource', 'Unique resource', 'xss_clean|trim|required');
        $this->form_validation->set_rules('time', 'Start time for tracking', 'xss_clean|trim|required');
        $this->form_validation->set_rules('reportType', 'Report type', 'xss_clean|trim|required');
        $this->form_validation->set_error_delimiters('<span>', '</span><br/>');
        
         if ($this->form_validation->run() == FALSE)
        {
            $this->output->set_status_header('500', "Cannot create tracker bundle");
            echo validation_errors();
        }
        else
        {
            $inputs=array(
                    "userName" => $this->session->userdata('username'),
                    "trackerName"=>$this->input->post('trackerName'),
                    "reportType"=>$this->input->post('reportType'),
                    "resource"=>$this->input->post('resource'),
                    "datetimeStamp"=>$this->input->post('time'),
            );
            
            try
            {
                $tracker = $this->tracker_model->insert($inputs);
                if ($tracker instanceof CF_tracker)
                {
                      echo '<p class="success">'.$tracker->getName() ." was sucessfully created"; 
                }
                else
                {
                    $this->output->set_status_header('500', "Cannot create tracker" . $inputs['trackerName']);
                    log_message('error','Could not confirm the data was inserted');
                }
                
            }catch(Exception $e){
                $this->output->set_status_header('500', "Cannot create tracker" . $inputs['trackerName']);
                log_message('error', "Exception was encounterd when performing database operation ".$e->getMessage()." ".$e->getLine());
            }
        }
        
    }

}
?>
