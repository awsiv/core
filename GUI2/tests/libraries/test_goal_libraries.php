<?php
class test_goal_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    public function test_cfpr_list_all_bundles(){
         $data = cfpr_list_all_bundles("agent");
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
           $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }
}
?>
