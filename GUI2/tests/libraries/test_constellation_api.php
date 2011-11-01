<?php
class test_constellation_api extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        
    }

    public function tearDown() {
        
    }
 
 public function test_cf_con_api_exist(){
     $this->assertTrue(function_exists('cfcon_local_report_virtualbundle'),"function exist cfcon_local_report_virtualbundle");
     $this->assertTrue(function_exists('cfcon_local_show_subscription_virtualbundle'),"function exist cfcon_local_show_subscription_virtualbundle");
     }
}
?>
