<?php
class Virtualbundle extends Cf_controller{
    
    function __construct() {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination','form_validation'));
        $this->load->helper('form');
        $this->load->model('virtual_bundle_model');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
       $this->carabiner->js('widgets/policyfinder.js');
       $this->carabiner->js('widgets/notes.js');
    }
    
    function search() {
        $data['report_type'] = 'virtual bundle';
        $this->load->view('searchpages/virtualbundle_search', $data);
    }
    
    function searchresult(){
        
    }
    
    function manage(){
        $this->carabiner->css('tabs-custom.css');
          $bc = array(
            'title' => 'virtual bundles',
            'url' => 'virtualbundle/manage',
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        
        $data = array(
            'title' => $this->lang->line('mission_portal_title'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        $this->template->load('template', 'virtualbundles/managebundles', $data);
        
    }
    
    function create(){
    
         $this->form_validation->set_rules('name', 'Virtual Bundlename', 'xss_clean|trim|required');
         $this->form_validation->set_rules('promises', 'Promises', 'xss_clean|trim|required');
         $this->form_validation->set_rules('description', 'Description', 'xss_clean|trim|required');
         //$this->form_validation->set_rules('hostclass', 'Host class', 'xss_clean|trim|required');
         //$this->form_validation->set_rules('description', 'Description', 'xss_clean|trim|required');
         $this->form_validation->set_error_delimiters('<span>', '</span><br/>');
         if($this->form_validation->run() == FALSE){
             echo validation_errors();     
         }else{
         $inputs=array(
                 'username'=>$this->session->userdata('username'),
                 'name'=>$this->input->post('name'),
                 'hostclass'=>$this->input->post('hostclass')?$this->input->post('hostclass'):"",
                 'description'=>$this->input->post('description')?$this->input->post('description'):"",
                 'promises'=>$this->input->post('promises')
                  );
         try{
            $result=$this->virtual_bundle_model->createVirtualBundle($inputs);
             if($result){ 
                echo "<p class=\"success\">".$inputs['name']." is created as virtual bundle</p>";
             }else{
                echo "<p class=\"error\">".$inputs['name']."  was not created</p>";   
             }
           }catch(Exception $e){
            echo "<p class=\"error\">Exception occured while carrying out the operation. Please check the log file</p>"; 
          }
         }
    }
    
    function myBundles(){
        try{
            $page_number=1;
            $getparams = $this->uri->uri_to_assoc(3);
            $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
             if (is_numeric($rows)) {
                $rows = (int) $rows;
             } else {
                $rows = 20;
             }
            $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
            $data=array(
                 'report_title'=>'virtual bundle',
                 'current'=>$page_number,
                 'number_of_rows' => $rows,
                 'params'=>'',
                 'url'=>'virtualbundle/myBundles/'
            );
            $data['report_result'] = $this->virtual_bundle_model->getVirtualBundleData(null,$this->session->userdata('username'),$rows, $page_number);
            $this->load->view('searchpages/default_result_view', $data);
        }catch(Exception $e){
            echo "<p class=\"error\">Exception occured while trying to fetch virtual bundles</p>";
       }
        
    }
}
?>
