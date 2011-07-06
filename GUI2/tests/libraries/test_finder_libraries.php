<?php
class test_finder_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    public function test_cfpr_policy_finder_by_promiser() {
        $data = cfpr_policy_finder_by_promiser(NULL,false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is not empty ". count($array) ."items");
    }

     public function test_cfpr_policy_finder_by_promiser_using_somevalue() {
        $data = cfpr_policy_finder_by_promiser('dum',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is not empty ". count($array) ."items");
    }

    public function test_cfpr_policy_finder_by_promiser_using_regx(){
        $data = cfpr_policy_finder_by_promiser('^D',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
           $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_policy_finder_by_bundle(){
        $data = cfpr_policy_finder_by_bundle(NULL,false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }
    
     public function test_cfpr_policy_finder_by_bundle_usingsomevalue(){
        $data = cfpr_policy_finder_by_bundle('cfengine',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_policy_finder_by_bundle_using_regex(){
        $data = cfpr_policy_finder_by_bundle('^m',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_policy_finder_by_handle(){
        $data = cfpr_policy_finder_by_handle(NULL,false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

     public function test_cfpr_policy_finder_by_handle_usingsomevalue(){
        $data = cfpr_policy_finder_by_handle('cfengine',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_policy_finder_by_handle_using_regex(){
        $data = cfpr_policy_finder_by_handle('^k',false);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_list_all_classes(){
        $data =  cfpr_list_all_classes(NULL,NULL,NULL,NULL);;
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_class_cloud(){
        //should return zero when used with windows and linux
        $data =  cfpr_class_cloud("windows");
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");

    }

    public function test_cfpr_show_hosts_name_using_regx(){
        $data =  cfpr_show_hosts_name('^1',NULL,10,1);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_show_hosts_name_using_someval(){
        $data =  cfpr_show_hosts_name('35',NULL,10,1);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_show_hosts_name_novalues(){
        $data=cfpr_show_hosts_name(NULL, NULL, NULL, NULL);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    public function test_cfpr_select_reports(){
        $data=cfpr_select_reports(".*", 100);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($data);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }

    
}
?>
