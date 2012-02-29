<?php
class test_report_model extends CodeIgniterUnitTestCase {
  
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('report_model');
    }

    public function tearDown() {
        
    }
    
     public function test_getAllReports(){
         $reports = $this->_ci->report_model->getAllReports();
         $this->assertTrue(is_array($reports),"Must be a valid set of class");
         $this->dump($reports); 
    }
   
}
?>
