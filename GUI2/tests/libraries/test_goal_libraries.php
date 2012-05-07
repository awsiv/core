<?php
class test_goal_libraries extends CodeIgniterUnitTestCase {
    public $username = 'admin'; // set username who will "run" test - we need this for RBAC
    
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    public function test_cfpr_bundle_agent_goals(){
        $data = cfpr_bundle_agent_goals($this->username);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
           $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is  Empty ". count($array) ."items");
    }
    
    public function test_business_goals(){
        $data =cfpr_list_business_goals();
        $array = json_decode(utf8_encode($data), true);
        $this->dump($data);
    }
}
?>
