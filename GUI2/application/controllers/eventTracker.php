<?php

class EventTracker extends Cf_controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination', 'form_validation'));
        $this->load->helper('form');
        $this->load->model(array('tracker_model','report_model'));
    }

    function createView()
    {
        $this->load->view('eventviewer/addTracker');
    }

    function create()
    {
        try
        {
            $inputs = array(
                "userName" => trim($this->session->userdata('username')),
                "trackerName" => trim($this->input->post('trackerName')),
                "reportType" => trim($this->input->post('reportType')),
                "resource" => trim($this->input->post('resource')),
                "dateTimeStamp" => strtotime(trim($this->input->post('time'))),
            );
            $tracker = $this->tracker_model->insert($inputs);

            if ($tracker !== FALSE)
            {
                //echo '<p class="success">' . $tracker->getName() . " was sucessfully created";
               echo json_encode(array(
                           'id'=>$tracker->getName(),
                           'resource'=>$tracker->getResource(),
                           'startTime'=>$tracker->getDateTime(),
                           'type'=>$tracker->getReportType()
                        ));
            }
            else
            {
                $this->output->set_status_header('500', "Cannot create tracker" . $inputs['trackerName']);

                $error_html = '<p class="error">';
                foreach ($this->tracker_model->getErrors() as $error)
                {
                    $error_html.= '<span>' . $error . '</span><br/>';
                };
                $error_html.='<p>';
                log_message('error', 'Could not confirm the data was inserted');
                echo $error_html;
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', "Cannot create tracker" . $inputs['trackerName']);
            log_message('error', "Exception was encounterd when performing database operation " . $e->getMessage() . " " . $e->getLine());
        }
    }

    function listTrackers()
    {
        try
        {
            $filter = array('userName' => $this->session->userdata('username'));
            $trackers = $this->tracker_model->get_all_tracker($filter);
            $data = array(
                    'trackers'=>$this->tracker_model->get_all_tracker($filter)
                    );
            $return_data=array();
            foreach($trackers as $tracker){
                $return_data[]=array(
                           'id'=>$tracker->getName(),
                           'resource'=>$tracker->getResource(),
                           'startTime'=>$tracker->getDateTime(),
                           'type'=>$tracker->getReportType()
                        );
            }
            //$this->load->view('eventviewer/listTrackers',$data);
            echo json_encode($return_data);
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', "Cannot retrive trackers");
            log_message('error', "Exception was encounterd when performing database operation " . $e->getMessage() . " " . $e->getLine());
        }
    }
    
    function track(){
      $username=$this->session->userdata('username');
      $reportType= $this->input->post('report');
      $resource=$this->input->post('resource');
      $incList=$this->input->post('inclist');
      $exList=$this->input->post('exlist');
      $from =$this->input->post('startTime');
      
      $cause_rx='.*'; /* regx to include every thig */
      $to=0; /* get current time stamp */
      $rows=-1; /* un limited rows*/
      $rows=50;
      $page_number=-1; /*unlimited pages */
      $page_number=1;
      
      if($resource =='' || $reportType=="" || $from ==""){
          $this->output->set_status_header('400', "Cannot retrive trackers");
           echo 'Invalid Inputs';
           return;
      }
      
      try
       {
          $logData=array();
          if($reportType =='not_kept'){
              $logData=$this->report_model->getPromisesNotKeptLog($username, NULL,$resource, $cause_rx, $from, $to, explode(',',$incList), explode(',',$exList), $rows, $page_number);
          }
          
          if($reportType =='repaired'){
              $logData = $this->report_model->getPromisesRepairedLog($username, NULL, $resource, $cause_rx, $from, $to, explode(',',$incList), explode(',',$exList), $rows, $page_number);
          }    
          
          $return_data=array();
          $return_data['data']=array();
          if(is_array($logData) && key_exists('data', $logData) && count($logData['data'])>0){
             foreach ($logData['data'] as $records){
                 if($records[3] >= $from){
                       $unique_id = md5($records[0].$records[1].$records[2].$records[3]); //hashing the sha_key time stamp and promise_handle
                       $records[] = $unique_id;
                       $return_data['data'][$records[3]][]=$records;
                 }
             } 
          }
          
          $return_data['meta']=array('update_time'=> now(),'hosts'=>$logData['meta']['related'],'events'=>$logData['meta']['count'],'type'=>$reportType);
          
          ksort($return_data['data']);
          echo json_encode($return_data);
          
       }catch(Exception $e)
       {
           $this->output->set_status_header('500', "Cannot retrive trackers");
           echo json_encode(array('message'=>'Error retriving '.$reportType.' data from'. strtotime($from) .'for'. $resource));
       }
        
    }
    
    function delete($id)
    {
        $filter=array('trackerName'=>$id,'userName'=>$this->session->userdata('username'));
        try
        {
           $this->tracker_model->delete($filter); 
           
        }catch(Exception $e){
           $this->output->set_status_header('500', "Cannot delete tracker"); 
        }
    }
    
    function update(){
        $inputs = array(
                "userName" => trim($this->session->userdata('username')),
                "trackerName" => trim($this->input->post('trackerName')),
                "reportType" => trim($this->input->post('reportType')),
                "resource" => trim($this->input->post('resource')),
                "dateTimeStamp" => strtotime(trim($this->input->post('time'))),
            );
        
        $filter = array(
                    'trackerName'=>trim($this->input->post('oldName')),
                    'userName' => trim($this->session->userdata('username')),
                   );
        try{
        $result=$this->tracker_model->update_tracker($filter,$inputs);
        if($result!==FALSE){
            echo json_encode(array(
                           'id'=>$result->getName(),
                           'resource'=>$result->getResource(),
                           'startTime'=>$result->getDateTime(),
                           'type'=>$result->getReportType()
                  ));
           }
           
        }catch(Exception $e){
            $this->output->set_status_header('500', "Cannot update the tracker"); 
        }
    }

}

?>
