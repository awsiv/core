<?php

class test_hosts extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();        
    }

    public function setUp() {
        $this->username  = 'admin';
        $this->test_host_correct = 'SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856';
        $this->test_host_error   = 'SHA=cd941e1c74cefbcc7e7fcf15a8bd8925b1956fe13828ba0cc75ff51b18169062';
        
        $this->_ci->load->model('host_model');        
    }

    public function tearDown() {
        
    }
    
    public function test_cfpr_host_count_red() {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'red');
        $this->assertNotNull($ret, '<span style="color:red">Red</span> hosts ');
        $this->dump($ret);
    }
    
    public function test_cfpr_host_count_yellow() {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'yellow');        
        $this->assertNotNull($ret, '<span style="color:yellow">Yellow</span> hosts ');
        $this->dump($ret);
    }
    
    public function test_cfpr_host_compliance_count_green() {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'green');
        $this->assertNotNull($ret, '<span style="color:green">Green</span> hosts ');
        $this->dump($ret);
    }
    
    public function test_cfpr_host_compliance_count_blue() {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'blue');        
        $this->assertNotNull($ret, '<span style="color:blue">Blue</span> hosts');
        $this->dump($ret);
    }
    
    public function test_network_speed() {
       $ret = $this->_ci->host_model->getNetworkSpeed($this->username, $this->test_host_correct);
       
       $speed = $ret['data']['speed'];
       $this->assertNotNull($speed, 'test_network_speed');       
       $this->dump($ret);               
    }
    
   public function test_network_speed_show_error() {
       $ret = $this->_ci->host_model->getNetworkSpeed($this->username, $this->test_host_error);
       
       $error= $ret['error']['errid'];
       $this->assertNotNull($error, 'test_network_speed <span style="color:red"><b>SHOW ERROR MESSAGE</b></span>');       
       $this->dump($ret);               
    }    
}