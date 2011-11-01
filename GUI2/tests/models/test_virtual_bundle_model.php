<?php

class test_virtual_bundle_model extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('virtual_bundle_model');
        cfcon_local_subscribe_virtualbundle('nishes','test-VIRTUALBUNDLE', "The description of this VB", "cfengine_correct_cftwin_files_cftwin,update_processes_mongod", "lin.*");
        cfcon_local_subscribe_virtualbundle('sudhir','sudhir-bundle', "The description of this VB", "cfengine_correct_cftwin_files_cftwin,update_processes_mongod", "lin.*");
        cfcon_local_subscribe_virtualbundle('admin','admin-bundle', "The description of this VB", "cfengine_correct_cftwin_files_cftwin,update_processes_mongod", "lin.*");


        
    }

    public function tearDown() {
        
    }

    public function test_getAllReportData() {
        $data = $this->_ci->virtual_bundle_model->getVirtualBundleData();
        $this->dump($data);
        $this->assertTrue(json_decode($data), "Should Return a valid array");
    } 
    
    public function test_getVirtualBundleDetails() {
        $data = $this->_ci->virtual_bundle_model->getVirtualBundleDetails('test-VIRTUALBUNDLE');
        $this->dump($data);
        $this->assertTrue(is_array($data));
    }
    
    public function test_getAllReportDataForSpecificUsername() {
        $data = $this->_ci->virtual_bundle_model->getVirtualBundleData(null,'nishes');
        $this->dump($data);
        $this->assertTrue(json_decode($data), "Should Return a valid array");
    } 
    
    
    public function test_getVirtualBundlePromiseSubscriptionDetails() {
      //  $data = $this->_ci->virtual_bundle_model->getVirtualBundleSubscriptions('test-VIRTUALBUNDLE');
       // $this->dump($data);
       // $this->assertTrue(is_array($data));
    }
    
   
    
    
    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */