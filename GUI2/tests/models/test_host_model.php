<?php

class test_host_model extends CodeIgniterUnitTestCase {
 Private $hostkey="SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6";
 Private $username="admin";
 public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('host_model');
    }

    public function tearDown() {
        
    }
    
    public function test_getHostName(){
         $name = $this->_ci->host_model->getHostName($this->username,$this->hostkey);
         $this->assertNotNull($name);
         $this->dump($name); 
    }
    
     public function test_getHostIp(){
         $ip = $this->_ci->host_model->getHostIp($this->username,$this->hostkey);
          $this->assertNotNull($ip);
         $this->dump($ip); 
    }
    
    public function test_getLastUpdate(){
        $date=$this->_ci->host_model->getLastUpdate($this->username,$this->hostkey);
        $this->assertTrue(is_numeric($date),"Must be ablid time stamp");
        $this->dump($date);
    }
    
    public function test_getHostVariable(){
        $val=$this->_ci->host_model->getHostVariable($this->username,$this->hostkey,"mon", "av_loadavg");
        $this->assertTrue(is_string($val));
        $this->dump($val);
    }
    
     public function test_getNetWorkSpeed(){
        $val=$this->_ci->host_model->getNetworkSpeed($this->username,$this->hostkey);
         $this->assertTrue(is_string($val));
        $this->dump($val);
    }
    
    public function test_getCompliacnelist(){
        $val=$this->_ci->host_model->getComplianceList($this->username,15,1);
        $this->assertTrue(is_array($val),"should return a valid array");
        $this->assertNotEqual(count($val['data']),0,"should not be empty");
        $this->dump(count($val['data']));
        $this->dump($val);
    }
}
?>
