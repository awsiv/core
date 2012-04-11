<?php

class test_finder_libraries extends CodeIgniterUnitTestCase {
    
    var $username = 'admin';
    
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        
    }

    public function tearDown() {
        
    }

    public function cfpr_promise_list_by_promiser_rx() {
        $data = cfpr_promise_list_by_promiser_rx($this->username, NULL,0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is not empty " . count($array) . "items");
    }

     public function test_cfpr_promise_list_by_promiser_rx_using_somevalue() {
        $data = cfpr_promise_list_by_promiser_rx($this->username, 'dir',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is not empty " . count($array) . "items");
    }

    public function test_cfpr_promise_list_by_promiser_rx_using_regx() {
        $data = cfpr_promise_list_by_promiser_rx($this->username, '^D',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }
    
    public function test_cfpr_promise_list_by_bundle_rx() {
        $data = cfpr_promise_list_by_bundle_rx($this->username,".*", NULL,0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }
    
     public function test_cfpr_promise_list_by_bundle_rx_usingsomevalue(){
        $data = cfpr_promise_list_by_bundle_rx($this->username,".*", 'def',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_promise_list_by_bundle_rx_using_regex() {
        $data = cfpr_promise_list_by_bundle_rx($this->username,".*", '^m',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_promise_list_by_bundle_rx_using_regex_for_empty_results() {
        $data = cfpr_promise_list_by_bundle_rx($this->username,".*", '^testetets212dxxxx',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(empty($array), " Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_promise_list_by_handle_rx() {
        $data = cfpr_promise_list_by_handle_rx($this->username, NULL,0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_promise_list_by_handle_rx_usingsomevalue() {
        $data = cfpr_promise_list_by_handle_rx($this->username, 'cfengine',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }
    
    public function test_cfpr_promise_list_by_handle_rx_using_regex() {
        $data = cfpr_promise_list_by_handle_rx($this->username, '^k',0,0);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }
    
   
    public function test_cfpr_class_cloud() {
        //should return zero when used with windows and linux
        $data = cfpr_class_cloud("linux");
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_show_hosts_name_using_regx() {
        $data = cfpr_show_hosts_name('^u', NULL, 10, 1);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_show_hosts_name_using_someval() {
        $data = cfpr_show_hosts_name('ubuntu', NULL, 10, 1);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_show_hosts_name_novalues() {
        $data = cfpr_show_hosts_name(NULL, NULL, NULL, NULL);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

    public function test_cfpr_select_reports() {
        $data = cfpr_select_reports(".*", 100);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty($array), "Is  Empty " . count($array) . "items");
    }

}

?>
