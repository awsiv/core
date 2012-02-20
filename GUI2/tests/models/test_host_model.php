<?php

class test_host_model extends CodeIgniterUnitTestCase {
 public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('host_model');
    }

    public function tearDown() {
        
    }
    
    public function test_getHostName(){
         $name = $this->_ci->host_model->getHostName('admin','SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6');
         $this->assertNotNull($name);
         $this->dump($name); 
    }
    
     public function test_getHostIp(){
         $ip = $this->_ci->host_model->getHostIp('admin','SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6');
          $this->assertNotNull($ip);
         $this->dump($ip); 
    }
    
    public function test_getLastUpdate(){
        $date=$this->_ci->host_model->getLastUpdate('admin','SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6');
        //$this->assertPattern('\d+\\',$subject);
        $this->dump($date);
    }
    
    public function test_getHostVariable(){
        $val=$this->_ci->host_model->getHostVariable('admin','SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6',"mon", "av_loadavg");
        //$this->assertPattern('\d+\\',$subject);
        $this->dump($val);
    }
    
     public function test_getNetWorkSpeed(){
        $val=$this->_ci->host_model->getNetworkSpeed('admin','SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6');
        //$this->assertPattern('\d+\\',$subject);
        $this->dump($val);
    }
}
?>
