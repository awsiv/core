<?php

class test_hosts extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->username = 'admin';
    }

    public function tearDown() {
        
    }
    
    public function test_cfpr_host_compliance_count_red() {
        $ret = cfpr_host_compliance_count_red($this->username);
        $this->assertNotNull($ret, '<span style="color:red">Red</span> hosts ');
        $this->dump($ret);
    }
    public function test_cfpr_host_compliance_count_yellow() {
        $ret = cfpr_host_compliance_count_yellow($this->username);
        $this->assertNotNull($ret, '<span style="color:yellow">Yellow</span> hosts ');
        $this->dump($ret);
    }
    
    public function test_cfpr_host_compliance_count_green() {
        $ret = cfpr_host_compliance_count_green($this->username);
        $this->assertNotNull($ret, '<span style="color:green">Green</span> hosts ');
        $this->dump($ret);
    }
    
    public function test_cfpr_host_compliance_count_blue() {
        $ret = cfpr_host_compliance_count_blue($this->username);
        $this->assertNotNull($ret, '<span style="color:blue">Blue</span> hosts');
        $this->dump($ret);
    }

}
