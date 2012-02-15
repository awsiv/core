<?php
class test_miscellaneous_libraries extends CodeIgniterUnitTestCase {
    
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    public function test_get_time_zone(){
        $data=date_default_timezone_get();
        $this->assertTrue(isset($data),"Should return valid String");
        $this->dump($data);
    }

    public function test_cfpr_show_red_hosts(){
        $data=cfpr_show_red_hosts(10, 1);
        $jsondata=json_decode($data,true);
        $this->assertTrue(isset($jsondata),"should return a valid array");
        $this->dump($data);
        $this->assertNotEqual("",trim($jsondata['data'][0]['id']) , "must have some value");
    }
    
     public function test_cfpr_show_yellow_hosts(){
        $data=cfpr_show_yellow_hosts(10, 1);
        $jsondata=json_decode($data,true);
        $this->assertTrue(isset($jsondata),"should return a valid array");
        $this->dump($data);
        $this->assertNotEqual("",trim($jsondata['data'][0]['id']) , "must have some value");
    }

    public function test_cfpr_show_green_hosts(){
        $data=cfpr_show_green_hosts(10, 1);
        $jsondata=json_decode($data,True);
        $this->assertTrue(isset($jsondata),"should return a valid array");
        $this->dump($data);
        $this->assertNotEqual("",trim($jsondata['data'][0]['id']) , "must have some value");
    }

     public function test_cfpr_show_blue_hosts(){
        $data=cfpr_show_blue_hosts(10, 1);
        $jsondata=json_decode($data,True);
        $this->assertTrue(isset($jsondata),"should return a valid array");
        $this->dump($data);
        $this->assertNotEqual("",trim($jsondata['data'][0]['id']) , "must have some value");
    }
    
     public function test_cfpr_show_top_n_hosts(){
        $data = cfpr_top_n_hosts(20, 1);
        $jsondata=json_decode($data,True);
        $this->assertTrue(isset($jsondata),"should return a valid array");
        $this->dump($data);
        $this->assertNotEqual(count($jsondata['data']),0,"should not be empty");
        $this->dump(count($jsondata['data']));
    }
    
    public function test_count_personal_working_logs(){
        $this->_ci->load->library('userdata');
        $count=$this->_ci->userdata->count_personal_working_notes('sudhir');
        $this->assertTrue(is_int($count), "the value must be a munber");
         $this->dump($count);
    }

}

?>
