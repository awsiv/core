<?php
class test_report_model extends CodeIgniterUnitTestCase {
  private $report_id;
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('report_model');
        $this->report_id='promises-repaired-summary';
    }

    public function tearDown() {
        
    }
    
     public function test_getAllReports(){
         $reports = $this->_ci->report_model->getAllReports();
         $this->assertTrue(is_array($reports),"Must be a valid set of class");
         $this->dump($reports); 
    }
   
    public function test_getReportCategory(){
         $report_category = $this->_ci->report_model->getReportCategory($this->report_id);
         $this->assertTrue(is_string($report_category),"Must be a valid name");
         $this->dump($report_category); 
    }
    
    public function test_getReportTitle(){
        $report_title= $this->_ci->report_model->getReportTitle($this->report_id);
        $this->assertTrue(is_string($report_title),"Must be a valid name"); 
        $this->dump($report_title);
    }
   
    public function test_getReportDetails(){
        $report_details= $this->_ci->report_model->getReportDetail($this->report_id);
        $this->assertTrue(is_array($report_details),"Must be a valid array with details"); 
        $this->dump($report_details);
    }
    
   
     public function test_getReportDescription(){
        $report_details= $this->_ci->report_model->getReportDescription($this->report_id);
        $this->assertTrue(is_string($report_details),"Must be a valid array with details"); 
        $this->dump($report_details);
    }
}
?>
