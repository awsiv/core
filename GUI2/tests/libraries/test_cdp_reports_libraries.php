<?php
class test_cdp_reports_libraries extends CodeIgniterUnitTestCase {
public function __construct() {
        parent::__construct();

    }

    public function setUp() {
        $this->testHandle = "cfengine_php_mod_cfmod_ini_ubuntu10";
        $this->cdptitle="Commands";
    }

    public function tearDown() {

    }

    public function test_cdp_function(){
         $this->assertTrue(function_exists("cfpr_cdp_reportnames"),"Should be true if there is Php-ldap module loaded");
    }
    public function test_cpd_report_list(){
        
       $result= cfpr_cdp_reportnames();
       $this->dump($result);
       $array = json_decode(utf8_encode($result), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertTrue(is_array($array[0]),"Each element should be an array");
        $this->assertEqual(count($array[0]), 2,"Must have two elements");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_cdp_report_detail(){
     $result=cfpr_cdp_report(NULL,$this->cdptitle,0,0);
     $this->dump($result);
     $array = json_decode(utf8_encode($result), true);
     $this->assertTrue(is_array($array), "Should Return a valid array");
     $this->assertTrue(key_exists('data', $array),"must have data as key");
     if(count($array['data'])>0){
         $this->assertNotEqual($array['meta']['count'], 0,"count should reflect there is data in list");
       }
    }

}
?>
