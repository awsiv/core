<?php
class test_license_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    function __testcases($data)
    {
         $array =json_decode(utf8_encode($data),true);
         $retValue = json_last_error();
         $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        if(isset($array['data']))
        {
           $this->assertTrue(!empty ($array),"Is not empty ". count($array['data']) ."items");
        }
        else
        {
           $this->assertTrue(!empty ($array),"Is not empty ". count($array) ."items");
        }

    }
    
    public function test_cfpr_getlicense_summary(){
        $data=cfpr_getlicense_summary();
       $this->__testcases($data);
    }

    public function test_cfpr_getlicense_expiry(){
        $data=cfpr_getlicense_expiry();
        $this->assertTrue(isset($data),"Should return valid String");
        $this->dump($data);
    }

    public function test_cfpr_getlicenses_granted(){
         $data=cfpr_getlicenses_granted();
        $this->assertTrue(isset($data),"Should return valid String");
        $this->dump($data);
    }

    public function test_cfpr_getlicenses_promised(){
         $data=cfpr_getlicenses_promised();
        $this->assertTrue(isset($data),"Should return valid String");
        $this->dump($data);
    }
}

?>
